/* oprimp.cpp: Implemantation functions for class BOperator
               of GNU/TOL language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

//#define TRACE_LEVEL 1

#define DONOT_USE_THREAD_PROFILE

#include <iostream>

#ifndef DONOT_USE_THREAD_PROFILE
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#endif

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_boper.h>
#include <tol/tol_bout.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bdir.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bclass.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bnameblock.h>
#include <tol/tol_btimer.h>

#ifdef UNIX
#else // WINDOWS
  #include <windows.h>
#endif

#if defined(_MSC_VER)
  #define SIZE_T_SPECIFIER    "%Iu"
  #define SSIZE_T_SPECIFIER   "%Id"
#elif defined(__GNUC__)
  #define SIZE_T_SPECIFIER    "%zu"
  #define SSIZE_T_SPECIFIER   "%zd"
#else
  #define SIZE_T_SPECIFIER    "%d"
  #define SSIZE_T_SPECIFIER   "%d"
#endif

BTraceInit("oprimp.cpp");

BText BEqualOperator::creatingName_;
BText BEqualOperator::currentFatherName_;
bool   BEqualOperator::isCreatingNameBlock_= false;
BText BEqualOperator::currentFullName_;
const BClass* BEqualOperator::creatingClass_ = NULL;
bool BStandardOperator::evaluatingFunctionArgument_ = false;

#define Do_TolOprProfiler
#ifdef Do_TolOprProfiler

#ifndef DONOT_USE_THREAD_PROFILE
typedef boost::shared_mutex Lock;
typedef boost::unique_lock< Lock > WriteLock;
typedef boost::shared_lock< Lock > ReadLock;

Lock myLock;

#define WRITE_LOCK \
  WriteLock w_lock(myLock);
#define READ_LOCK \
  ReadLock r_lock(myLock);

void SampleProfiler();
#else
#define WRITE_LOCK
#define READ_LOCK
#endif

//--------------------------------------------------------------------
struct BTolOprProfiler
// This class allows to profile all the calls to TOL functions,
// both, built-in and user ones.
// Stores a symbol table of status of each called function with 
// information of calling number and time elapsed and dumps all it in
// a log file at the end of TOL session
//--------------------------------------------------------------------
{
  int    index;      
  BText  identify;   //Function identifier Type+Name+Path
  double calls;      //Number of times that function has been called
  double availMem;   //Difference of available memory in bytes between the end and the begin of function executions
  double time;       //Total time elapsed during function executions
  double time2;      //Total squared time to calculate standard deviation
  double timeMin;    //Minimun time elapsed in one execution
  double timeMax;    //Maximun time elapsed in one execution
  bool   isRunning_; //To avoid spureous time reporting in recursive
                     //functions
  size_t TickCounter; // Number of times the function was observed to
                      // be running at sampling resolution given in
                      // static member TickResolution
  static BTolOprProfiler* ptrCurrentProfiler;
  static size_t TickResolution;

#ifndef DONOT_USE_THREAD_PROFILE
  static boost::thread *ptrThread;
#endif

  struct CalllingStr
  {
    int caller;
    int called;
    int calls;
  };
  typedef hash_map_by_BIntPair<int>         ::sparse_ BCallingTable;
  typedef hash_map_by_name<BTolOprProfiler*>::sparse_ BProfileTable;
  static BProfileTable profilerTable_;   //Hashed table of profilers
  static BCallingTable callingTable_;
  static BTolOprProfiler* nullProfiler_; //The null profiler to be skiped
  static BDat enabled_;
  static BText profilerPath_;
  static BText callingPath_;
  static int currentIndex_; 
  static int callingNumber_;
  static std::vector<int> runningIndex_;
  static BArray<BTolOprProfiler*> profilers;
  static BArray<CalllingStr> calling;
  //Builds and initializes the profiler for a given operator
  BTolOprProfiler(BOperator* opr)
  : calls  (0.0),
    availMem    (0.0),
    time   (0.0),
    time2  (0.0),
    timeMin(BDat::PosInf()),
    timeMax(BDat::NegInf()),
    isRunning_(false),
    TickCounter(0)
  {
    identify = GetIdentify(opr);
  }

  //Gives the symbol identifier of a given operator
  static BText GetIdentify(BOperator* opr)
  {
    if(!opr) { return(""); }
    BText name = opr->FullName();
    if(!name.HasName()) 
    { 
      //Null or unnamed operators will not be profiled
      return(""); 
    }
    else
    {
      if(!isalpha(name[0])) { name=BText("'")+name; }
      //Uses "..."\t to create a CSV formated string of three fields 
      return(BText("\"")+opr->Grammar()->Name()+"\"\t"+
                   "\"" +name+"\"\t"+
                   "\"" +opr->SourcePath()+"\"");
    }
  }

  //Initializes the hashed table of profilers
  static int Initialize()
  {
    static int initialized_ = false;
    if(!initialized_)
    {
      profilers.AllocBuffer(1000000);
      profilers.ReallocBuffer(1);
      nullProfiler_ = new BTolOprProfiler(NULL);
      nullProfiler_->identify = "\"#_NOT_IDENTIFIED_#\"\t\"#_NOT_IDENTIFIED_#\"\t\"\"";
      nullProfiler_->index = 0;
      profilerTable_[""] = nullProfiler_;
      profilers[0] = nullProfiler_;
      runningIndex_.push_back(0);
      BText df = BOut::DumpFile();
      profilerPath_ = GetAbsolutePath(df+".TolOprProfiler.csv");
      callingPath_  = GetAbsolutePath(df+".TolOprCalling.csv");
      BParamDat* _enabled_  = new BParamDat
      (
        "TolOprProfiler.Enabled",
        enabled_,
        I2("If true then a full timing and calling profiler report of each TOL "
           "operator will be generated and saved into file ",
           "Si es cierto se generará al final de la sesión TOL un informe de "
           "perfil de llamadas y tiempo de cada operador TOL, y se almacenarán "
           "en los ficheros ")+profilerPath_+" and "
      );
    //SetEmptyKey(profilerTable_, NULL);
      initialized_ = true;
      return(1);
    }
    return(0);
  }


  //Search the profile corresponding to a given to an operator or 
  //creates it if not exists and it's a non null and named operator
  static BTolOprProfiler* Find(BOperator* opr)
  {
    Initialize();
    BTolOprProfiler* profiler = (BTolOprProfiler*)opr->GetProfiler();
    if(!profiler)
    {
      BText id = GetIdentify(opr);
      if(!id.HasName()) { return(NULL); }
      BProfileTable::const_iterator fc = profilerTable_.find(id);
      if(fc==profilerTable_.end())
      {
        profiler = new BTolOprProfiler(opr);
        profilerTable_[profiler->identify] = profiler;
        profiler->index = profilers.Size();
        assert(profiler->index<profilers.MaxSize());
        profilers.ReallocBuffer(profiler->index+1);
        profilers[profiler->index] = profiler;
      //Std(BText("TRACE PROFILER[")+profiler->index+"]="+profiler->identify);
      }
      else
      {
        profiler = fc->second;
        assert(id==profiler->identify);
      }
    }
    return(profiler);
  }

  static BTolOprProfiler* StartProfile(BOperator* opr, 
    double& initTime_, 
    double& initAvailMem_)
  {
    if(!(enabled_.Value())) { return(NULL); }
    BTolOprProfiler* profiler = BTolOprProfiler::nullProfiler_;
    if(opr) 
    { 
      profiler = (BTolOprProfiler*)opr->GetProfiler(); 
      if(!profiler)
      {
        profiler  = BTolOprProfiler::Find(opr);
        opr->PutProfiler(profiler);
      }
      if(profiler==BTolOprProfiler::nullProfiler_) 
      {
        profiler=NULL;
      }
    }
    if(profiler) 
    { 
      if(profiler->isRunning_)
      {
        profiler=NULL;
      }
      else
      {
        BIntPair p;
        p.r_ = runningIndex_.back();
        p.c_ = profiler->index;
        BCallingTable::iterator fc = callingTable_.find(p);
        if(fc==callingTable_.end())
        {
          callingTable_[p] = 0;
          callingNumber_++;
        }
        else
        {
          fc->second ++;
        }
        runningIndex_.push_back(profiler->index);
        profiler->isRunning_ = true;
        initTime_ = BSys::SessionTime(); 
        initAvailMem_ = BSys::SessionAvailMem();
        BTolOprProfiler::SetCurrentProfiler(profiler);
      }
    }
    return(profiler);
  }

  static void StartThread()
  {
#ifndef DONOT_USE_THREAD_PROFILE
    if (!BTolOprProfiler::ptrThread)
      {
      BTolOprProfiler::ptrThread = new boost::thread(&SampleProfiler);
      }
#endif
  }

  static void StopThread()
  {
#ifndef DONOT_USE_THREAD_PROFILE
    if (BTolOprProfiler::ptrThread)
      {
      BTolOprProfiler::ptrThread->interrupt();
      delete BTolOprProfiler::ptrThread;
      BTolOprProfiler::ptrThread = NULL;
      }
#endif
  }

  static void SetCurrentProfiler(BTolOprProfiler* profiler)
  {
    // REVIEW: adquire an exclusive lock
    WRITE_LOCK;
    BTolOprProfiler::ptrCurrentProfiler = profiler;
    // REVIEW: release the lock (released when leaving scope
  }

  static BTolOprProfiler* GetCurrentProfiler()
  {
    // REVIEW: adquire an exclusive lock
    READ_LOCK;
    return BTolOprProfiler::ptrCurrentProfiler;
    // REVIEW: release the lock (released when leaving scope
  }

  static size_t IncrCurrentTickCounter()
  {
    // REVIEW: adquire a lock
    WRITE_LOCK;
    if (BTolOprProfiler::ptrCurrentProfiler) 
      {
      return ++(BTolOprProfiler::ptrCurrentProfiler->TickCounter);
      }
    // REVIEW: release the lock, released when leaving the scope
    return 0;
  }

  static size_t SetCurrentTickCounter(size_t counter)
  {
    // REVIEW: adquire a lock
    WRITE_LOCK;
    if (BTolOprProfiler::ptrCurrentProfiler) 
      {
      size_t previous = BTolOprProfiler::ptrCurrentProfiler->TickCounter;
      BTolOprProfiler::ptrCurrentProfiler->TickCounter = counter;
      return previous;
      }
    // REVIEW: release the lock, released when leaving the scope
    return 0;
  }

  size_t GetCurrentTickCounter()
  {
    // REVIEW: adquire a lock
    READ_LOCK;
    if (BTolOprProfiler::ptrCurrentProfiler)
      {
      return BTolOprProfiler::ptrCurrentProfiler->TickCounter;
      }
    // REVIEW: release the lock, relased when leaving the scope    
    return 0;
  }

  static size_t SetTickResolution(size_t resolution)
  {
    // REVIEW: adquire a lock
    WRITE_LOCK;
    size_t previous = BTolOprProfiler::TickResolution;
    BTolOprProfiler::TickResolution = resolution;
    // REVIEW: release the lock, released when leaving the scope  
    return previous;
  }

  static size_t GetTickResolution()
  {
    // REVIEW: adquire a lock
    READ_LOCK;
    return BTolOprProfiler::TickResolution;
    // REVIEW: release the lock, released when leaving scope 
  }

  static void StopProfile(
    BTolOprProfiler* profiler, 
    double initTime_, 
    double initAvailMem_)
  {
    if(profiler) 
    { 
      runningIndex_.pop_back();
      profiler->isRunning_ = false;
      double availMem_ = (BSys::SessionAvailMem() - initAvailMem_)*1000.0;
      double time_ = (BSys::SessionTime() - initTime_)*1000.0;
      profiler->calls  += 1.0;
      profiler->availMem   += availMem_;
      profiler->time   += time_;
      profiler->time2  += time_*time_;
      profiler->timeMax = Maximum(profiler->timeMax, time_);
      profiler->timeMin = Minimum(profiler->timeMin, time_);
    }
  }

  //--------------------------------------------------------------------
  static BInt CompareIndex(const void* v1, const void* v2)
  //--------------------------------------------------------------------
  {
    BTolOprProfiler* p1 = *((BTolOprProfiler**)v1);
    BTolOprProfiler* p2 = *((BTolOprProfiler**)v2);
    return(p1->index-p2->index);
  }
  //--------------------------------------------------------------------
  static BInt CompareTime(const void* v1, const void* v2)
  //--------------------------------------------------------------------
  {
    BTolOprProfiler* p1 = *((BTolOprProfiler**)v1);
    BTolOprProfiler* p2 = *((BTolOprProfiler**)v2);
    double diff = p2->time-p1->time;
    return((diff<0)?-1:(diff>0)?1:0);
  }
  //--------------------------------------------------------------------
  static BInt CompareCalling(const void* v1, const void* v2)
  //--------------------------------------------------------------------
  {
    CalllingStr* p1 = (CalllingStr*)v1;
    CalllingStr* p2 = (CalllingStr*)v2;
    int diffTime = profilers[p1->called]->time-profilers[p2->called]->time;
    if(diffTime!=0) { return(diffTime); }
    else
    {
      return(p2->calls - p1->calls);
    }
  }

  static FILE* tryOpen(const BText& path)
  {
    FILE* dump = fopen(path,"w");
    int tryNum;
    for(tryNum=1; !dump && (tryNum<=10); tryNum++)
    {
      if(!dump)
      {
        Error(I2("Cannot open for writting file ",
                 "No se puedo abrir para escritura el archivo ")+path+ 
              I2(" (trying again after 10 seconds)",
                 " (reintentando tras 10 segundos)"));
        BSys::SleepMilliSeconds(10000);
        dump = fopen(path,"w");
      }
    }
    return(dump);
  }
  //Writes the profiler information of all called functions into a system 
  //log file
  static int DumpProfilerTable()
  {
    if(!(enabled_.Value())) { return(0); }
    Std(BText("TolOprProfiler saved at ")+profilerPath_+"\n");
    FILE* dump = tryOpen(profilerPath_);
    if(!dump) { return(0); }
    BText title = BText()+
     "Index\t"+
     "Type\t"+
     "Name\t"+
     "Path\t"+
     "Calls\t"+
     "MemoryDiffInBytes\t"+
     "MiliSeconds\t"+
     "TimeAverage\t"+
     "TimeStdDev\t"+
     "TimeMin\t"+
     "TimeMax\t"+
     "Ticks\n";
    fprintf(dump,title.String());
    fflush(dump);
    BProfileTable::const_iterator iter;
    int n = 0;

    BTolOprProfiler* sop;
    profilers.Sort(CompareTime);
    for(n=0; n<profilers.Size(); n++)
    {
      sop = profilers[n];
      if(sop->index)
      {
        sop = profilers[n];
        double timeAverage = sop->time / sop->calls;
        double timeStdDev  = sqrt(sop->time2 / sop->calls - timeAverage*timeAverage);
        fprintf(dump,"%d\t%s\t%.16lg\t%.16lg\t%.16lg\t%.16lg\t%.16lg\t%.16lg\t%.16lg\t"SIZE_T_SPECIFIER"\n",
          sop->index,
          sop->identify.String(),
          sop->calls,
          sop->availMem,
          sop->time,
          timeAverage,
          timeStdDev,
          sop->timeMin,
          sop->timeMax,
          sop->TickCounter
        );
      }
    }
    profilers.Sort(CompareIndex);
    fclose(dump);
    return(n);
  }
  static int DumpCallingTable()
  {
    if(!(enabled_.Value())) { return(0); }
    BSys::MkDir(GetFilePath(callingPath_),true);
    Std(BText("TolOprCalling saved at ")+callingPath_+"\n");
    FILE* dump = tryOpen(callingPath_);
    if(!dump) { return(0); }
    BText title = BText()+
     "CallerIndex\t"+
     "CallerType\t"+
     "CallerName\t"+
     "CallerPath\t"+
     "CalledIndex\t"+
     "CalledType\t"+
     "CalledName\t"+
     "CalledPath\t"+
     "Calls\n";
    fprintf(dump,title.String());
    fflush(dump);
    BCallingTable::const_iterator iter;
    int n = 0;
    BTolOprProfiler* caller;
    BTolOprProfiler* called;

    calling.AllocBuffer(callingNumber_);
    for(iter=callingTable_.begin(); iter!=callingTable_.end(); iter++)
    {
      calling[n].caller = iter->first.r_;
      calling[n].called = iter->first.c_;
      calling[n].calls = iter->second;
      n++;
    }
    calling.ReallocBuffer(n);
    calling.Sort(CompareCalling);

    for(n=0; n<calling.Size(); n++)
    {
      caller = profilers[calling[n].caller];
      called = profilers[calling[n].called];
      fprintf(dump,"%d\t%s\t%d\t%s\t%d\n",
        caller->index,
        caller->identify.String(),
        called->index,
        called->identify.String(),
        calling[n].calls
      );
    }
    profilers.Sort(CompareIndex);
    fclose(dump);
    return(n);
  }
  static int Dump()
  {
    DumpProfilerTable();
    DumpCallingTable();
    return(1);
  }
};

// http://www.boost.org/doc/libs/1_53_0/doc/html/boost_asio/tutorial/tuttimer3/src.html
//boost::this_thread::sleep(boost::posix_time::milliseconds(100));

void SampleProfiler()
{
#ifndef DONOT_USE_THREAD_PROFILE
  size_t resolution = BTolOprProfiler::GetTickResolution();
  while(1) 
    {
    size_t t = BTolOprProfiler::IncrCurrentTickCounter();
    //printf("tick = %d\n", t);
    boost::this_thread::sleep(boost::posix_time::microseconds(resolution));
    }
#endif
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTolOprProfilerDump);
  DefExtOpr(1, BDatTolOprProfilerDump, "TolOprProfiler.Dump", 1, 1, 
  "Text", "(Text pathPrefix)",
  I2("Dumps to specified file current stored information about "
     "performance profiling of called TOL functions since "
     "variable TolOprProfiler.Enabled was changed to true.\n"
     "NOTE: At this release it can be used just one time by TOL "
     "session.",
     "Vuelca al fichero especificado la información almacenada del "
     "perfil de rendimiento de las funciones de TOL llamadas "
     "desde el momento en que se cambió la variable "
     "TolOprProfiler.Enabled a cierto.\n"
     "NOTA: en esta versión sólo se puede utilizar una vez por "
     "cada sesión TOL."),
     BOperClassify::TimeAlgebra_);
  void BDatTolOprProfilerDump::CalcContens()
//--------------------------------------------------------------------
{
  BText& pathPrefix=Text(Arg(1));
  BTolOprProfiler::StopThread();
  BTolOprProfiler::profilerPath_ = GetAbsolutePath(pathPrefix+".TolOprProfiler.csv");
  BTolOprProfiler::callingPath_  = GetAbsolutePath(pathPrefix+".TolOprCalling.csv");
  BTolOprProfiler::Dump();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTolOprProfilerInit);
  DefExtOpr(1, BDatTolOprProfilerInit, "TolOprProfiler.Init", 1, 1, 
  "Real", "(Real microSec)",
  I2("Enable profiling with sampling resolution of microSec meaasured in microseconds",
     "Habilita la medición de rendimiento por función. El argumento microSec indica la frecuencia de medición en microsegundos."),
     BOperClassify::TimeAlgebra_);
  void BDatTolOprProfilerInit::CalcContens()
//--------------------------------------------------------------------
{
#ifndef DONOT_USE_THREAD_PROFILE
  if (BTolOprProfiler::ptrThread)
    {
    Error(I2("There is already one profiler running",
             "Ya hay un profiler ejecutándose"));
    return;
    }
#endif
  double freq = Real(Arg(1));
  BTolOprProfiler::enabled_ = 1;
  int iFreq = int(round(freq));
  if (iFreq<=0)
    {
    char buffer[256];
    snprintf(buffer, 255, 
             I2("Invalid sampling (tick) resolution %d, must be a possitive integer",
                "Resolution de muestreo (tick) inválida, debe ser un entero positivo"),
             iFreq);
    Warning(buffer);
    iFreq = 1;
    }
  size_t previous = BTolOprProfiler::SetTickResolution(size_t(iFreq));
  BTolOprProfiler::StartThread();
  contens_ = double(previous);
}

BTolOprProfiler* BTolOprProfiler::nullProfiler_;
BTolOprProfiler::BProfileTable BTolOprProfiler::profilerTable_;
BTolOprProfiler::BCallingTable BTolOprProfiler::callingTable_;
std::vector<int> BTolOprProfiler::runningIndex_; 
BArray<BTolOprProfiler*> BTolOprProfiler::profilers;
BArray<BTolOprProfiler::CalllingStr> BTolOprProfiler::calling;

BDat  BTolOprProfiler::enabled_ = 0.0;
BText BTolOprProfiler::profilerPath_ = "";
BText BTolOprProfiler::callingPath_ = "";
int   BTolOprProfiler::callingNumber_ =0;

BTolOprProfiler* BTolOprProfiler::ptrCurrentProfiler = NULL;
size_t BTolOprProfiler::TickResolution = 1;

#ifndef DONOT_USE_THREAD_PROFILE
boost::thread *BTolOprProfiler::ptrThread = NULL;
#endif

int BTolOprProfiler_Init() { return(BTolOprProfiler::Initialize()); }
int BTolOprProfiler_Dump() { return(BTolOprProfiler::Dump      ()); }

#define Do_TolOprProfiler_Start(opr) \
  double initTime_; \
  double initAvailMem_; \
  BTolOprProfiler* saveProfiler = BTolOprProfiler::GetCurrentProfiler(); \
  BTolOprProfiler* profiler = BTolOprProfiler::StartProfile(opr, initTime_, initAvailMem_);

#define Do_TolOprProfiler_Stop(opr) \
  BTolOprProfiler::StopProfile(profiler, initTime_, initAvailMem_); \
  BTolOprProfiler::SetCurrentProfiler(saveProfiler);

#else

int BTolOprProfiler_Init() { return(0); }
int BTolOprProfiler_Dump() { return(0); }
#define Do_TolOprProfiler_Start(opr)
#define Do_TolOprProfiler_Stop(opr) 

#endif


//--------------------------------------------------------------------
BText BOperator::Dump() const
    
/*! Returns a text with all important information about this operator
 */
//--------------------------------------------------------------------
{
    BText gra = "Anything";
    if(Grammar()) { gra = Grammar()->Name(); }
    return(gra + " " + Name() + Arguments().String());
}

//--------------------------------------------------------------------
BText BOperator::ThemeName () const
/*! Returns the name of the operator's classify
 */
//--------------------------------------------------------------------
{
    if(theme_) { return(theme_->Name());      }
    else       { return(I2("None","Niguna")); }
}

//--------------------------------------------------------------------
void BOperator::PutTheme(BOperClassify* cl)

/*! Sets classify of operator to \a cl
 * \param cl Classify of operator
 */
//--------------------------------------------------------------------
{
    theme_ = cl;
    theme_->Add(this);
}

//--------------------------------------------------------------------
BBool Compare(BOperator** ope1, BOperator** ope2)
//--------------------------------------------------------------------
{
    return(0);
}

//--------------------------------------------------------------------
BGrammar* GetLeft(BGrammar* grammar,
		  List* left,
		  BText& name,
		  List*& rest,
		  BStruct*& str,
		  BClass*& cls)
    
/*! Analizes declarations of functions and variables and returns
 *  the grammar of declarated object.
 * \param grammar
 * \param left
 * \param name
 * \param rest
 * \param str
 */
//--------------------------------------------------------------------
{
  if(!left) { return(NIL); }
  List* left_orig = left; 
  BToken* tok = BParser::treToken(left);
  BGrammar* gra=NULL;
  str=NULL;
  cls = NULL;
  rest = NULL;
  if((tok->TokenType() == TYPE))
  {
    BTypeToken* tt = (BTypeToken*)tok;
    const BText& tokName = tok->Name();
    if(tt->type_==BTypeToken::BSYSTEM)
    {
      gra = BGrammar::FindByName(tokName,false);
    }
    else if(tt->type_==BTypeToken::BSTRUCT)
    {
      str = FindStruct(tokName);
    }
    else if(tt->type_==BTypeToken::BCLASS)
    {
      cls = FindClass(tokName,-1);
    }
    else 
    {
    //Backward compatibility with old OIS stored functions
      gra = BGrammar::FindByName(tokName,false);
      if(!gra) { str = FindStruct(tokName); }
      if(!str) { cls = FindClass(tokName,-1); }
    }
    if(str) { gra = GraSet(); }
    else if(cls) { gra = GraNameBlock(); }
    left = Tree::treLeft(left);
  } 
  else if((tok->TokenType()==BINARY)&&(tok->Name()=="::"))
  {
    if(BParser::Unparse(left_orig)=="BysMcmc::NameBlock config")
      printf("");
  //BText expr = BParser::Unparse(left);
  //Std(BText("Unparse(left)=\n")<<BParser::Unparse(left)+"\n");
    List* nbLst = Tree::treLeft(left);
  //Std(BText("Unparse(nbLst)=\n")<<BParser::Unparse(nbLst)+"\n");
    BUserNameBlock* unb = (BUserNameBlock*)GraNameBlock()->LeftEvaluateTree(nbLst);
    BText err_msg = 
      I2("Data type identifier was expected instead of",
         "Se esperaba identificador de tipo de datos en lugar de")+"\n  "+
      BParser::Unparse(left_orig);
    if(!unb) { Error(err_msg); }
    left = Tree::treRight(left);
    while(unb)
    {
      tok = BParser::treToken(left);
      const BText& tokName = tok->Name();
      //VBR: Misteriosamente aprecen en los OIS expresiones como BysMcmc::NameBlock
      BGrammar* g = BGrammar::FindByName(tokName,false);
      if(g)
      {
        gra = g;
        unb = NULL;
        left = Tree::treLeft(left);
      }
      else
      {
        BSyntaxObject* member = unb->Contens().Member(tokName);
        if(!member) 
        { 
          cls = FindClass(tokName,0);
          if(cls)
          {
            gra = GraNameBlock();
            unb = NULL;
            left = Tree::treLeft(left);
          }
          else 
          {
            Error(err_msg); 
          }
        }
        else if(member->Mode()== BSTRUCTMODE)
        {
          str = (BStruct*)member;
          gra = GraSet(); 
          unb = NULL;
          left = Tree::treLeft(left);
        }
        else if(member->Mode() == BCLASSMODE)
        {
          cls = (BClass*)member;
          gra = GraNameBlock();
          unb = NULL;
          left = Tree::treLeft(left);
        }
        else if((member->Mode() == BOBJECTMODE) && (member->Grammar() == GraNameBlock()))
        {
          unb = (BUserNameBlock*)member;
          left = Tree::treRight(left);
        }
        else
        {
          Error(err_msg);
        }
      }
    }
  }
  else if(tok->TokenType()!=FUNCTION) 
  {
    gra = grammar;
  }
  BToken* leftTok = BParser::treToken(left);
  if(leftTok &&
     ((leftTok->TokenType()==ARGUMENT)||
      (leftTok->TokenType()==FUNCTION)))
  { 
    name = leftTok->Name(); 
  }
  if (left) 
  {
    rest = Tree::treLeft(left);
  }
  return(gra);
}


//--------------------------------------------------------------------
static BBool IsIdentifier(const BText& expression)
    
/*! Returns true if the expression is an identifier.
 */
//--------------------------------------------------------------------
{
    if(! expression.HasName())  { return(BFALSE); }
    if(!BParser::DefaultParser()->Filter()->
        StartIdentifier(expression.Get(0)) || 
        (expression=="Stop"))
    {
	Error(expression + I2(" is not a valid variable identifier.",
			      " no es un identificador de variable válido."));
	return(BFALSE);
    }
    if((expression.Length()>=5) &&
       (expression(0)=='y')     &&
       isdigit(expression(1))   &&
       isdigit(expression(2))   &&
       isdigit(expression(3))   &&
       isdigit(expression(4))	 )
    {
	Error(expression + I2(" is not a valid variable identifier because it "
			      "has date format.",
			      " no es un identificador de variable válido "
			      "porque tiene formato de fecha."));
	return(BFALSE);
    }

    if((expression(0)>='0') && (expression(0)<='9'))
    {
	Deprecated(expression + I2(" will be soon not accepted as an "
				   "identifier.", " no será aceptado "
				   "próximamente como identificador."));
    }

    return(BTRUE);
}


//--------------------------------------------------------------------
static BSyntaxObject* CreateObject(      List*	   tre,
				         BGrammar* gra,
				   const BText&	   name)
//--------------------------------------------------------------------
{
#ifdef  TRACE_LEVEL
  BText fun = BText("CreateObject(")+gra->Name()+","+name+")";
#endif
  TRACE_SHOW_LOW(fun," BEGIN");
  BSyntaxObject* result = NIL, *rightResult=NIL;
  TRACE_SHOW_MEDIUM(fun," 1");
  if(!IsIdentifier(name)) 
  { 
    Error(name+I2(" is not a valid identifier and could not be created.", 
                  " no es un identificador válido y no se pudo crear."));
    return(NIL); 
  }

  TRACE_SHOW_MEDIUM(fun," 2");
  BInt level = BGrammar::Level();
//Std(BText("\nCreating Object ") + name + " at level " + level);
  TRACE_SHOW_MEDIUM(fun," 3");
  BGrammar::PutLast(gra);
  rightResult = gra->EvaluateTree(tre); 
	if(!rightResult || ((rightResult->Mode()!=BOBJECTMODE) && 
                      (rightResult->Mode()!=BBUILTINFUNMODE) && 
                      (rightResult->Mode()!=BUSERFUNMODE))) 
  {
    TRACE_SHOW_MEDIUM(fun," 4");
	  if(!BGrammar::StopFlag())
     Error(name+I2(" could not be created.", " no se pudo crear."));
	} 
  else 
  {
    TRACE_SHOW_MEDIUM(fun," 5");
    if(level>0) 
    {
      TRACE_SHOW_MEDIUM(fun," 6");
/*
      if(!BEqualOperator::CreatingName().HasName())
      {
        result = BNameBlock::LocalMember(name); 
        if(result && result->NameBlock())
        {
          Warning(I2("Local variable","La variable local")+
                  " "+gra->Name()+" "+name+" "+
                  I2("hides published NameBlock member",
                     "oculta el miembro de NameBlock publicado")+
                  " "+result->FullName());
          result = NULL; 
        }
      }
*/
      if(!result) 
      { 
        result = BGrammar::FindLocal(name); 
        TRACE_SHOW_MEDIUM(fun," 7");
        if(result && (result != rightResult) && (result->Level()!=level)) { result = NIL; }
      }
    } 
    else 
    {
      result = gra->FindVariable(name);
      if(result && (result != rightResult) && result->NameBlock())
      {
        Warning(I2("Global variable","La variable global")+
                " "+gra->Name()+" "+name+" "+
                I2("hides published NameBlock member",
                   "oculta el miembro de NameBlock publicado")+
                " "+result->FullName());
        result = NULL;
      }
    }
    if(result && (result != rightResult)) // send an error messages
    {
      TRACE_SHOW_MEDIUM(fun," 8");
      BText functionName = "";
      BText functionPath = "";
      BText sentence = result->FullName() + " " +result->Description();
#     ifdef WINDOWS
      path += BText("\n")+SysPath(path);
#     endif
      TRACE_SHOW_MEDIUM(fun," 9");
      if(BUserFunction::ActiveFunction()) 
      {
        functionName = BUserFunction::ActiveFunction()->Name();
        functionPath = BUserFunction::ActiveFunction()->InFile();
      }
      TRACE_SHOW_MEDIUM(fun," 10");
      if(name == result->Name())
      {
        TRACE_SHOW_MEDIUM(fun," 11");
  	    if(sentence.HasName()) 
        {
          TRACE_SHOW_MEDIUM(fun," 12");
          Error(BText("Variable '") + name +
                I2("' already defined as ",
                   "' ya definida como \"") + sentence + "\"\n");
          } 
        else 
        {
          TRACE_SHOW_MEDIUM(fun," 13");
          Error(BText("Variable '") + name +
                I2("' already defined ",
                   "' ya definida ") + 
                ((!result->SourcePath().HasName())?"":
                I2(" in file "," en el fichero ")+result->SourcePath()));
  	    }
        TRACE_SHOW_MEDIUM(fun," 14");
  	    Std(I2("Variable \"","No se ha podido crear la variable \"") + 
  		      gra->Name() + " " + name + "\"" + 
  		      I2(" can not be created\n",".\n"));
        TRACE_SHOW_MEDIUM(fun," 15");
   	    if(functionName.HasName()) 
  	    {
          TRACE_SHOW_MEDIUM(fun," 16");
  		    Std(I2("This error has been produced in function ",
  		           "El error se ha producido en la función \"") + 
  		        functionName + "\"");
          if(functionPath.HasName()) 
          {
            TRACE_SHOW_MEDIUM(fun," 17");
  		      Std(I2(" in the file:\n","en el fichero:\n") + 
  			           functionPath + "\n\n");
  	      }
        }
  	    else if(name == result->LocalName()) 
  	    {
          TRACE_SHOW_MEDIUM(fun," 18");
  	      if(functionName.HasName())
          {
            TRACE_SHOW_MEDIUM(fun," 19");
            Error(BText("Variable '") + name +
  		            I2("' already defined as parameter of the function '",
  			             "' ya definida como parámetro de la función '")
  		               + functionName + "'");
          }
  	      else 
          {
            TRACE_SHOW_MEDIUM(fun," 20");
            Error(BText("Variable '") + name +
  		            I2("' already defined as a parameter of the function",
  			             "' ya definida como un parámetro de la función"));
            if(functionPath.HasName()) 
            {
              Std(I2("File: ","Fichero: ") + functionPath + "\n");
            }
  	      }
        }
        else 
  	    {
          TRACE_SHOW_MEDIUM(fun," 21");
  	      Error(I2("Conflict between two variables.", 
  		             "Conflicto entre variables.\n") +
  		          I2("Trying to change \"","Se ha intentado modificar \"") + 
  		          result->Name() + 
  		          I2("using ","\" a través de la ") + "variable \"" + name + 
  		          "\"");
        }
  	  }
      TRACE_SHOW_MEDIUM(fun," 22");
  	  result = NIL;
    }
    else
    {
      TRACE_SHOW_MEDIUM(fun," 23");
      if(result != rightResult)
      {
        result=gra->New(name,rightResult);
      }
      TRACE_SHOW_MEDIUM(fun," 24");
      if(result && !result->GetOisTree())
      {
        result->PutOisTree(tre); 
      }
    }
  }
  TRACE_SHOW_LOW(fun," END");
  return(result);
}

//--------------------------------------------------------------------
// Member functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BSyntaxObject* BEqualOperator::Evaluate(const List* argList)

/*! Creates a new object. If stack_ exists places it in stack_, else
 *  places it in variable list of the corresponding grammar.
 *  If an object with same name and type already exists or the
 *  right expression is not correct sends an error message and returns
 *  NIL, else returns the created object.
 */
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NIL;
  BText name;
  List* rest = NULL;
  BStruct* str = NULL;
  BClass*  cls = NULL;
  static int trace_counter = 0;
  bool carIsList = argList->car()->IsListClass();
  List* dec = carIsList?Tree::treNode((List*) argList):(List*)argList;
/* * /
  if(BParser::Unparse(dec,"")=="nb::@InputDef ll")
  {
    printf("");
    Std(BText("\nTRACE BEqualOperator::Evaluate dec:'")+
        BParser::Unparse(dec,"")+"'\n"+BParser::treWrite(dec,"  ",false));
  }
/* */
  BGrammar* gra = GetLeft(Grammar(), dec, name, rest, str, cls);
  List* left = NULL;
  bool defInst = false;
  if(!carIsList && cls && !rest)
  {
    defInst = true; 
    BToken* tok = new BToken("#DefaultInstance",ARGUMENT);
    left = new List(tok, NIL);
  }
  else
  {
    left = Tree::treLeft((List*)argList);
  } 
  creatingClass_ = NULL;
  bool oldIsCreatingNameBlock = isCreatingNameBlock_;
  bool isNameBlock = gra==GraNameBlock();
  isCreatingNameBlock_ = isNameBlock;

  BText oldName, oldFullName;
  const BClass* oldClass;
  if(isNameBlock) 
  { 
    oldFullName = currentFullName_;
    currentFatherName_ = oldFullName;
    oldName = creatingName_;
    oldClass = creatingClass_;
    creatingName_  = name; 
    if(currentFullName_.HasName())
    {
      currentFullName_ = currentFullName_ + "::" +name;
    //Std(BText("\nTRACE currentFullName_ <- ")<<currentFullName_+"\n");
    }
    else
    {
      currentFullName_ = name;
    //Std(BText("\nTRACE currentFullName_ <- ")<<currentFullName_+"\n");
    }
    creatingClass_ = cls;
  }
  if(!rest && gra) 
  {
    result = CreateObject(left, gra, name);
    if(str && result)
    {
      BSet& set = Set(result);
      if((result->Grammar()==GraSet())&&(set.Struct()!=str))
      {
        set.PutStruct(str);
      }
      if((result->Grammar()!=GraSet())||(Set(result).Struct()!=str))
      {
        Error(I2("A set with structure ",
                 "Se esperaba un conjunto con estructura ")+str->Name()+
              I2(" was expected instead of ",
                 " en lugar de ")+
              result->Grammar()->Name()+ " "+
              result->Identify());
        DESTROY(result);
      }
    }
  } 
  else 
  {
	  Error(I2("Bad use for \"=\" in\n", "Uso indebido de \"=\" en\n") +
	        BParser::Unparse(argList));
  }
  if(isNameBlock) 
  { 
    currentFullName_ = oldFullName;
  //Std(BText("\nTRACE currentFullName_ <- ")<<currentFullName_+"\n");
    creatingName_    = oldName; 
    creatingClass_   = oldClass;
    isCreatingNameBlock_ = oldIsCreatingNameBlock;
  }
  if(defInst)
  {
    DESTROY(left);
  }
  return(result);
}


//--------------------------------------------------------------------
BSyntaxObject* BUserFunctionCreator::Evaluate(const List* argList)

/*! Creates a new function. If a function with same name already
 *  exists or the definition expression is not correct sends an error
 *  message and returns NIL, else returns the created function.
 */
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NIL;
  BText name;
  List* rest = NULL;
  BStruct* str = NULL;
  BClass* cls = NULL;
  List* dec = Tree::treNode((List*) argList);
//if(BParser::Unparse(dec,"")=="Proj::MiClase fun_aux.2.2(Real i)")
  //printf("");
  BGrammar* gra = GetLeft(NIL, dec, name, rest, str, cls);
  if(rest && gra)
  {
    result = gra->FindOperator(name); 
    if(result && result->NameBlock() && !BNameBlock::Building())
    {
      Warning(I2("Function ","La función ")+
              " "+gra->Name()+" "+name+" "+
              I2("hides published NameBlock member",
                 "oculta el miembro de NameBlock publicado")+
              " "+result->FullName());
      result = NULL;
    }
    if(result && BNameBlock::Building())
    {
      result = NULL;
    }
    if(result && (result->Level()<BGrammar::Level()))
    {
      result = NULL;
    }
    if(result)
    {
      BText path = result->SourcePath();
#ifdef WINDOWS
      path += BText("\n")+SysPath(path);
#endif
      Error(I2("Operator ", "El operador '") + name +
            I2("' is already defined in ",
               "' ya ha sido definido en ") + " :\n"+path+
            I2("as "," como ")+result->Dump());
      result = NIL;
    }
    else
    {
      BUserFunction* usf =  new BUserFunction(name,gra);
      result = usf->GetCode();
    //usf->PutName(name);
      if(!(usf->SetExpression(Tree::treNode((List*)argList),
           Tree::treLeft((List*)argList))))
      {
        Error(Out()+I2("Function ","La función ")+name+
              I2(" could not been created", " no se pudo crear"));
        DESTROY(result);
      }
//	  if(result) { Std(Out()+"\nNew operator :\n"+result->Dump()); }
    }
  }
  else
  {
    BText msg = "";
    if(!gra) 
    { 
      msg = BText("\n<")+name+"> "+
            I2("is not a valid type of data.",
               "no es un tipo de datos válido."); 
    }
    else if(!rest)
    {
      msg = I2("There are no arguments list.",
               "No hay lista de argumentos."); 
    }
    BText args = BParser::Unparse(Tree::treNode((List*)argList));  
	  Error(I2("Wrong function declaration\n","Declaración errónea de función\n")+
          args+msg);
  }
  return(result);
}


//--------------------------------------------------------------------
BNewStruct::BNewStruct(BStruct& str)
//--------------------------------------------------------------------
    : BExternalOperator
(
    str.Name(),
    GraSet(),
    StructGrammars(str),
    NIL,
    str.Size(),
    str.Size(),
    StructArguments(str),
    I2("Creates a new set with structure ",
       "Crea un nuevo conjunto con estructura ") + str.Name() + ".",
    BOperClassify::System_
    ),
      struct_(&str)
{
    DecNRefs();
    DecNRefs();
//  Std(BText("\nCreating struct creator ")+Name());
}



//--------------------------------------------------------------------
BText BNewStruct::StructGrammars(BStruct& str)
//--------------------------------------------------------------------
{
    BText arg = "";
    for(BInt n=0; n<str.Size(); n++)
    {
	if(arg.HasName()) { arg += " "; }
	arg += str[n].GetType();
    }
    return(arg);
}

//--------------------------------------------------------------------
BText BNewStruct::StructArguments(BStruct& str)
//--------------------------------------------------------------------
{
    BText arg = "";
    for(BInt n=0; n<str.Size(); n++)
    {
	if(arg.HasName()) { arg += ", "; }
	arg += str[n].Dump() ;
    }
    return(BText("(")+arg+")");
}


//--------------------------------------------------------------------
BSyntaxObject* BNewStruct::Evaluate(const List* arg)
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NIL;
  List* t = (List*) arg;
  BToken* line = BParser::treToken(Tree::treNode(t));
//Std(BText("\narg=")+BParser::Unparse(arg));
  if(line->TokenType() == SEPARATOR) 
  {
    t = Tree::treNode((List*) arg);
  //Std(BText("\nt (1) =")+BParser::Unparse(t));
    if(!t)
    {
      Error(I2("Bad defined structure ",
               "Estructura mal definida ") + Name());
    }
    else 
    {
      if(t->cdr()) { t = t->cdr(); }
      else         { t = (List*) arg; }
    //Std(BText("\nt (2) =")+BParser::Unparse(t));
    } 
  }

  result = BStandardOperator::Evaluate(t);
  return result;
}


//--------------------------------------------------------------------
BSyntaxObject* BNewStruct::Evaluator(BList* arg) const
//--------------------------------------------------------------------
{
//  Std(BText("\nBNewStruct::Evaluator ") + LstText(arg));
    BSetOfType* uSet = new BSetOfType(arg);
    uSet->Contens().PutStruct(struct_);
    return(uSet);
}


//--------------------------------------------------------------------
BSyntaxObject* BStructCreator::Evaluate(const List* argList)
    
/*! Creates a new struct and its object-creator function with same
 *  name. If a struct with same name already exists or definition
 * expression isn't correct sends an error message, and returns NIL,
 * else returns the object-creator function of this struct.
 */
//--------------------------------------------------------------------
{
  BStruct*     structure  = NIL;
  const BText& name       = BParser::treToken(argList)->Name();
  BGrammar*    setGrammar = GraSet();
  BStruct*     str        = FindStruct(name); 
  if(str && str->NameBlock()  && !BNameBlock::Building())
  {
    Warning(I2("Global structure ","La estructura global")+
            " Struct "+name+" "+
            I2("hides published NameBlock member",
               "oculta el miembro de NameBlock publicado")+
            " "+str->FullName());
    str = NULL;
  }
  if(str && BNameBlock::Building())
  {
    str = NULL;
  }
  if(str && (str->Level()<BGrammar::Level()))
  {
    str = NULL;
  }
  if(str)
  {
  //if(!argList->cdr()) { return(str); }
    BText path = str->SourcePath();
#ifndef UNIX
    path += BText("\n")+SysPath(path);
#endif
    Error(BText("Struct '")+name+
          I2("' is already defined in ",
             "' ya ha sido definido en ") + " :\n"+path);
  }
  else
  {
    BGrammar* graArg  = setGrammar;
    List*     args    = (Tree::treNode((List*)argList))->cdr();
    BInt      numArgs = 0;
    if (args) numArgs = args->length();
    BText     nameArg;
    structure = new BStruct(name);
    BArray<BField> field(numArgs);
    for(BInt n=0; (n<numArgs) && graArg; n++)
    {
      List*    rest_ = NIL;
      BStruct* str_  = NIL;
      BClass*  cls_  = NIL;
      if((graArg = GetLeft(setGrammar,
        Tree::treNode(args),
        nameArg, rest_, str_, cls_)))
      {
        field[n].PutName    (nameArg);
        field[n].PutGrammar (graArg);
        field[n].PutStruct  (str_);
        field[n].PutClass   (cls_);
        args = args->cdr();
      }
      else 
      { 
        DESTROY(structure); 
      }
    }
    if(structure) 
    {
      structure->PutField(field);
      structure->PutFunction(new BNewStruct(*structure));
    }
  }
  return(structure);
}


//--------------------------------------------------------------------
BSyntaxObject* BStandardOperator::EvaluateArgument(BGrammar*gra, const List* branch)
//--------------------------------------------------------------------
{
  bool oldEvFunArg = evaluatingFunctionArgument_; 
  evaluatingFunctionArgument_ = true;
//Std(BText("\nBStandardOperator::EvaluateArgument ")+Name()+
//    " starting evaluatingFunctionArgument_="+int(evaluatingFunctionArgument_));
  BText oldCreatingName = BEqualOperator::creatingName_;
  BText oldCreatingFullName = BEqualOperator::currentFullName_;
  const BClass* oldCreatinClass = BEqualOperator::creatingClass_;
  BEqualOperator::creatingName_ = "";
  BEqualOperator::currentFullName_ =  "";
  BEqualOperator::creatingClass_ = NULL;
  BSyntaxObject* var = gra->EvaluateTree(branch);
  BEqualOperator::creatingName_ = oldCreatingName;
  BEqualOperator::currentFullName_ =  oldCreatingFullName;
  BEqualOperator::creatingClass_ = oldCreatinClass;
  evaluatingFunctionArgument_ = oldEvFunArg;
//Std(BText("\nBStandardOperator::EvaluateArgument ")+Name()+
//    " ending evaluatingFunctionArgument_="+int(evaluatingFunctionArgument_));
  return(var);
}


//--------------------------------------------------------------------
BSyntaxObject* BStandardOperator::Evaluate(const List* argTrees)
    
/*! Evaluates an argument-trees for an operator calling ensuring they
 * are in good number, type and order and calls its virtual evaluator
 * function with all evaluated argument-objects. This evaluator
 * function must to destroy this list of arguments because it needs
 * all its arguments along its life time.
 */
//--------------------------------------------------------------------
{
#ifdef  TRACE_LEVEL
  BText fun = BText("Evaluate[")+Grammar()->Name()+" "+Name()+"]";
#endif
  TRACE_SHOW_LOW(fun," BEGIN");
  if(!argTrees) { return(NIL); }
  const List* branch = argTrees;
  BList* args = NIL;
  BList* first = NIL;
  BGrammar* gra;
  BSyntaxObject* result = NIL;
  BSyntaxObject* var;
  BInt n = 0, k;
//Std(BText("\nEvaluating function ")+ Identify());
  do 
  {
    List* b = Tree::treNode((List*)branch);
    if(!b) 
    { 
      branch = NULL;
      break; 
    }
	  n++;
	  if(!(gra = GrammarForArg(n))) 
    {
	    BText desc   = Grammar()->Name()+" "+Name()+" "+Arguments();
	    Error(BText(I2("Too many arguments in call to ",
			   "Demasiados argumentos para ")) + desc);
	    DESTROY(args);
	  } 
    else 
    {
	  //Std(BText("\n  Argument ")+n+"["+gra->Name()+"] = "+BParser::Unparse(b));
      int numOpt = NumOptForArg(n);
      if(numOpt==1)
      {
	      if(var = EvaluateArgument(gra,b)) 
        {
		      BStruct* str = (n<=structs_.Size())?structs_(n-1):NIL;
		      BClass*  cls = (n<=classes_.Size())?classes_(n-1):NIL;
		      if(str && (gra==GraSet())) 
          {
            BSet& set = Set(var);
		        if(set.Struct()!=str) 
            {
			        BText bText = BParser::Unparse(b);
			        Error(bText + I2(" is not a Set with structure ",
					                     " no es un Set con estructura ") +
			              str->FullName());
			        DESTROY(var);
		        }
		      }
          else if(cls && (gra==GraNameBlock())) 
          {
            BNameBlock& nb = ((BUserNameBlock*)var)->Contens();
            if(cls->Name()=="@BsrMaster")
              printf(""); 
		        if(!nb.IsInstanceOf(cls)) 
            {
			        BText bText = BParser::Unparse(b);
              BText but = "";
              if(nb.Class())
              {
                but = I2(" but it's an instance of ",
                         " sino de " )+nb.Class()->FullName();
              }
              else
              {
                but = I2(" nor any other class.",
                         " ni de ninguna otra clase.");
              } 
			        Error(bText + I2(" is not a NameBlock instance of ",
					                     " no es un NameBlock instancia de ") +
			              cls->FullName()+but);
			        DESTROY(var);
		        } 
		      }
	      }
      }
      else
      {
        var = NULL;
        BOut::Disable();
        for(k=0; !var&&(k<numOpt); k++)
        {
          BGrammar* g = GrammarForArg(n,k);
          BGrammar::PutLast(g);
          int stackPos = BGrammar::StackSize();
          var = EvaluateArgument(g,b);
          if(!var) 
          { 
            BGrammar::CleanTreeCache(b, true); 
            BGrammar::DestroyStackUntil(stackPos, NULL);    
          }
        }
        BOut::Enable();
        if(!var)
        {
          BText bText = BParser::Unparse(b);
          BText opt = "{";
          for(k=0; k<numOpt; k++) 
          {
            if(k>0) { opt<<"|"; }
            opt << GrammarForArg(n,k)->Name(); 
          }
          opt<<"}";
          Error(bText + I2(" has none of these allowed types ",
					                 " no tiene ninguno de los tipos permitidos ") +
			          opt);
        }
      }
	    if(var) 
      {
		    LstFastAppend(args, first, var);
		    branch = branch->cdr();
	    } 
      else
      { 
        BToken* tokBr = BParser::treToken((List*)branch);
        if(tokBr && tokBr->Name()=="PutDescription")
        {
  		    branch = branch->cdr();
          n--;
        }
        else
        {
          DESTROY(args);
        }
      }
	  }
  } while(branch && gra && args);  
  if(args) 
  {
	  if(n<MinArg()) 
    {
	    BText desc   = Grammar()->Name()+" "+Name()+" "+Arguments();
	    Error(BText(I2("Insuficient arguments in call to ",
		            	   "Número de argumentos insuficientes para ")) + desc);
	  } 
    else if(branch) 
    {
	    BText desc   = Grammar()->Name()+" "+Name()+" "+Arguments();
	    Error(BText(I2("Too many arguments in call to ",
			   "Demasiados argumentos para ")) + desc);
	  } 
    else 
      {
      numArg_ = n;
      BGrammar::PutLast(Grammar()); 
      Do_TolOprProfiler_Start(this);
      result = (BSyntaxObject*)Evaluator(args);
      if(result) { result->Do(); }
      Do_TolOprProfiler_Stop(this);
      }
  } 
  else 
    {
    BText desc = Grammar()->Name()+" "+Name()+" "+Arguments();
    Error(I2("Wrong arguments in call to ",
             "Argumentos erróneos para ") + desc);
    }
  BGrammar::PutLast(Grammar());
  TRACE_SHOW_LOW(fun," END");
  return(result);
}


//--------------------------------------------------------------------
BText BStandardOperator::Dump() const

/*! Returns a text with all important information about this operator
 */
//--------------------------------------------------------------------
{
  return(Grammar()->Name() + " " + FullName() + Arguments().String());
}


//--------------------------------------------------------------------
BInt BExternalOperator::NumOptForArg(BInt n) const

/*! Returns the number of optional grammars for \a n-th argument
 */
//--------------------------------------------------------------------
{
  if(maxArg_>0)
  {
    if(n>maxArg_) { return(0); }
  }
  else
  {
    if(n>minArg_) { n = minArg_+1; }
  }
  return((grammars_[n-1].Size()));
}

//--------------------------------------------------------------------
BGrammar* BExternalOperator::GrammarForArg (BInt n, BInt k) const

/*! Returns the grammar for \a n-th argument
 */
//--------------------------------------------------------------------
{
  if(maxArg_>0)
  {
    if(n>maxArg_) { return(NULL); }
  }
  else
  {
    if(n>minArg_) { n = minArg_+1; }
  }
  return((grammars_[n-1][k]));
}

//--------------------------------------------------------------------
BGrammar* BExternalOperator::GrammarForArg(BInt n) const

/*! Returns the grammar for \a n-th argument
 */
//--------------------------------------------------------------------
{
  if(maxArg_>0)
  {
    if(n>maxArg_) { return(NULL); }
  }
  else
  {
    if(n>minArg_) { n = minArg_+1; }
  }
  return((grammars_[n-1][0]));
}


//--------------------------------------------------------------------
BText BUserFunction::FullName() const
    
/*! Returns a text with all important information about this operator
 */
//--------------------------------------------------------------------
{
  if(staticOwner_) { return(staticOwner_->FullName()+"::"+Name()); }
  else             { return(BSyntaxObject::FullName()); }
}


//--------------------------------------------------------------------
BText BUserFunction::Dump() const
    
/*! Returns a text with all important information about this operator
 */
//--------------------------------------------------------------------
{
    return(declare_);
}


//--------------------------------------------------------------------
BBool BUserFunction::Compile()
    
/*! Ensures function are well defined and makes lists of arguments
 *  and its grammars.
 */
//--------------------------------------------------------------------
{
//Std(BText("\nCompiling user function ")+ declare_);
  BBool  allRight = BTRUE;
  List*  dec      = declaration_;
  BInt   n        = 0;
  BText* errMsg   = NIL;
  arguments_ = "";
  BToken* tok = BParser::treToken(dec);
  BTokenType type = tok->TokenType();
  while(tok && (type==BINARY)&&(tok->Name()=="::"))
  {
    dec = Tree::treRight(dec);
    tok = dec?BParser::treToken(dec):NULL;
  }
  assert(tok->TokenType()==TYPE);
  dec = dec->cdr();
//  if(((BToken*)Car((BTree*)Car(dec)))->TokenType()!=TYPE)
  if(dec)
  {
    List* auxTre = Tree::treNode(dec);
    if(auxTre)
    {
      BCore* auxCar = auxTre->car();
      if(auxCar && !auxCar->IsListClass()) 
      {
        BToken* auxTok = (BToken*) auxCar; 
        BTokenType auxType = auxTok->TokenType();
        if((auxType==FUNCTION) || (auxType==SEPARATOR))
        {
          dec = auxTre->cdr();
        }
      }
    }
  }
  tok = BParser::treToken(dec);
  type = tok->TokenType();
  if(allRight)
  {
    numArg_ = minArg_ = maxArg_ = dec->length();
    lastCalling_.ReallocBuffer(maxArg_);
    names_      .ReallocBuffer(maxArg_);
    grammars_   .ReallocBuffer(maxArg_);
    structs_    .ReallocBuffer(maxArg_);
    classes_    .ReallocBuffer(maxArg_);
  }
  while(dec && allRight)
  {
    List*     rest = NIL;
    BStruct*  str  = NIL;
    BClass*   cls  = NIL;
    structs_ [n] = NIL;
    classes_ [n] = NIL;
    bool carIsList = dec->car()->IsListClass();
    List* decLst = carIsList?Tree::treNode((List*) dec):(List*)dec;
    BGrammar* gra  = GetLeft
    (
      NIL, decLst,
      names_[n], rest, str, cls
    );
    if(!rest && gra) 
    {
      if(arguments_.HasName()) { arguments_ += ", "; }
      arguments_ += gra->Name() + " " + names_[n];
      grammars_[n].AllocBuffer(1);
      grammars_[n][0] = gra;
      structs_ [n] = str;
      classes_ [n] = cls;
      if(str) { str->IncNRefs(); }
      if(cls) { cls->IncNRefs(); }
      dec = dec->cdr();
    } 
    else 
    {
      arguments_ = "";
      dec = NIL;
      allRight = BFALSE;
    }
    n++;
  }
  if(!allRight)
  {
    BText msg = I2("Wrong declaration in user function ",
                   "Declaración errónea en función de usuario ")+
                BParser::Unparse(declaration_);
    if(errMsg)
    {
      msg << "\n" << (*errMsg);
      delete errMsg;
    }
    Error(msg);
  }
  else
  {
    if(arguments_) 
    {
      BText aux = arguments_;
      aux.Replace(", ",",");
      argTable_.ReallocBuffer(0);
      ReadAllCells(aux, argTable_, ',', ' ');
      argTable_.ReallocBuffer(maxArg_?maxArg_:minArg_+1);
    }
    arguments_ = BText("(") + arguments_ +")";
  }
  return(allRight);
}

//--------------------------------------------------------------------
BSyntaxObject* BUserFunction::Evaluate(const List* argTrees)
//--------------------------------------------------------------------
{
  BUserFunction::AddActiveFunction(this);
  BSyntaxObject* result = BExternalOperator::Evaluate(argTrees);
  BUserFunction::RemoveActiveFunction();
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BUserFunction::Evaluator(BList* argList) const
    
/*! Evaluates this user function with the arguments in \a argList
 *  and returns the result.
 */
//--------------------------------------------------------------------
{
  BInt n;
  BSyntaxObject* result = NIL;
//BText           desc   = Name()+"(";
  const BList*   arg;
  BGrammar*       gra;
  BSyntaxObject* syn;
  int save_level = BGrammar::Level();
  BGrammar::IncLevel();
    
  BList* argInstances = NIL;
  BInt   stackPos     = BGrammar::StackSize();
  int    arglistOK    = 1;
    
#ifndef __USE_DYNSCOPE__
  BGrammar::PushStackLevel(Level()+1);
#endif
  BArray<BSyntaxObject*> funArg(grammars_.Size());
  for(n=0; n<grammars_.Size(); n++)
  {
    funArg[n] = NULL;
  }
  for((n=0),(arg=argList); arg; n++, arg = Cdr(arg))
  {
    gra = (BGrammar*)(grammars_[n][0]);
    syn = (BSyntaxObject*)Car(arg);
    int m = syn->Mode();
    BGrammar* g = (m==BUSERFUNMODE || m==BBUILTINFUNMODE)?GraCode():syn->Grammar();
    if (!gra || ((gra!=GraAnything()) && (gra!=g)))
    {
      char sp[1024];
      char en[1024];
        
      sprintf(sp, "Tipos no coincidentes para argumento # %d en "
                  "llamada a función %s: %s != %s",
            n+1, Name().String(), gra?gra->Name().String():"NONE", 
            g->Name().String());
      sprintf(en, "Type mismatch for argument # %d in calling "
                  "function %s: %s != %s",
            n+1, Name().String(), gra?gra->Name().String():"NONE", 
            g->Name().String());
        Error(I2(en,sp));
      arglistOK = 0;
      break;
    }
    //if(n>0) { desc += ","; }
    //desc += syn->Identify();
    BSyntaxObject* argument = gra->NewFunArg(names_[n], syn);
      argInstances = Cons(argument, argInstances);
      gra->AddObject(argument);
    funArg[n] = argument;
  }
    
  //desc += ")";
  /* next block "{" will IncLevel */
  BGrammar::CleanTreeCache(definition_, true);
  if(arglistOK)
  {
    const BClass* oldStaticOwner = BClass::currentStatic_;
    const BNameBlock* oldNameBlock = BNameBlock::Current();
    const BNameBlock* funNameBlock = NameBlock();
    if(funNameBlock && !funNameBlock->EnsureIsAssigned())
    { 
      if(!funNameBlock->Class()) 
      {
        Error(BText("Corrupted method ")+Identify()+"\n"
          "Possibly this problem is due to a non standard use of OOP, if "
          "this function was assigned to a member of type Code of a "
          "NameBlock or Class instance that has been destroyed already.");
      } 
      funNameBlock = NULL;
    }
    if(funNameBlock)
    {
      BNameBlock::SetCurrent(funNameBlock);
      BClass::currentStatic_ = funNameBlock->Class();
    }
    else
    {
      BClass::currentStatic_ = staticOwner_;
    }
    if(!funNameBlock && BClass::currentStatic_ &&
       BClass::currentStatic_->NameBlock())
    {
      funNameBlock = BClass::currentStatic_->NameBlock();
      BNameBlock::SetCurrent(funNameBlock);
    }
    result = Grammar()->EvaluateTree(definition_, 1);
    BClass::currentStatic_ = oldStaticOwner;
    BNameBlock::SetCurrent(oldNameBlock);
  }
  for(n=0; n<funArg.Size(); n++)
  {
    if(funArg[n]) { funArg[n]->PutLocalName(""); }
  }

  if(result)
  {
    result->IncNRefs();
    BGrammar::DestroyStackUntil(stackPos, result);
    DESTROY(argInstances);
    result->DecNRefs();
  }
  else if(Grammar()!=GraAnything())
  {
    if(!BGrammar::StopFlag())
    {
      if(inFile_.HasName()) 
      {
        Error(I2("Fail in function \"","Fallo en la función \"") + 
              Name() + I2("\" declared in file:\n",
              "\" declarada en el fichero:\n") + 
              inFile_ + "\n");
      }
      else
      {
        Error(I2("Fail in function \"","Fallo en la función \"") + 
              Name() + "\"");
      }
    }
    BGrammar::DestroyStackUntil(stackPos, NIL);
    SAFE_DESTROY(argInstances,result);
  }
  SAFE_DESTROY(argList,result);

#ifndef __USE_DYNSCOPE__
  BGrammar::PopStackLevel();
  /* restore evaluation level, i think this is not need because the previous
   * "}" should restore the BGrammar's level_ */
#endif

  BGrammar::PutLevel(save_level);
  if(result)
  {
    BGrammar* rg = result->Grammar();
    if(rg && rg->IsAutoContens())
    {
      if((rg!=GraNameBlock())&&(rg!=GraSet())&&(rg!=GraCode()))
      {
        BSyntaxObject* copy = result->CopyContens();
        if(result->HasName()) { copy->PutName(result->Name()); }
        if(result->Description().HasName()) { copy->PutDescription(result->Description()); }
        DESTROY(result);
        result = copy;
      }
    }
  }
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BUserFunction::FastEvaluator(BList* argList) const

/*! Evaluates this user function with the arguments in argList and
 *  returns the result. It tries to do it fastest as possible: avoiding
 *  unnnecesary information. 
 */
//--------------------------------------------------------------------
{
  Error("FATAL: NOT IMPLEMENTED BUserFunction::FastEvaluator");
  return(NULL);
}


//--------------------------------------------------------------------
BBool BUserFunction::SetExpression(const BText& dec, const BText& def)
    
/*! Makes the user function from a text declaration and a text
 *  definition.
 */
//--------------------------------------------------------------------
{
    Clean();
    declare_     = dec;
    define_      = def;
//    declaration_ = Parsing(declare_);
//    definition_  = Parsing(define_ );
    Tree* decTree = BParser::parsing(declare_);
    Tree* defTree = BParser::parsing(define_);
    declaration_ = decTree->getTree();
    definition_ = defTree->getTree();
    
//    BInt pos = declare_.Find('(');
//    arguments_   = declare_.Buffer()+pos;//SubString(pos, declare_.Length()-1);
    arguments_   = declare_;
    return(Compile());
}


//--------------------------------------------------------------------
BBool BUserFunction::SetExpression(const List* dec, const List* def)

/*! Makes the user function from a tree declaration and a text
 *  definition.
 */
//--------------------------------------------------------------------
{
  Clean();
  //declaration_ = LstDuplicate(dec);
  //definition_  = LstDuplicate(def);
  declaration_ = dec->duplicate();
  definition_ = def->duplicate();
  return(Compile());
}


//--------------------------------------------------------------------
void BUserFunction::Clean()
    
/*! Destroy all list created in the object.
 */
//--------------------------------------------------------------------
{
  DESTROY(declaration_);
  DESTROY(definition_);
  declare_     = "";
  define_      = "";
  for(int n = 0; n<maxArg_; n++)
  {
    if(classes_[n]) { classes_[n]->DecNRefs(); }
    if(structs_[n]) { structs_[n]->DecNRefs(); }
  }
}


//--------------------------------------------------------------------
BUserFunction* BUserFunction::Replicate() const
    
/*! DestCreates an exact copy of user function
 */
//--------------------------------------------------------------------
{
  BUserFunction* usf = new BUserFunction("",Grammar());
  usf->PutName(Name());
  usf->PutDescription(Description());
  usf->SetExpression(declaration_, definition_); 
  return(usf);
}
