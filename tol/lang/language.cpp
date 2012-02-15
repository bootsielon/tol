/* language.cpp: main and init functions of GNU/TOL language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_blanguag.h>
#include <tol/tol_init.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bdir.h>
#include <tol/tol_init.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bmoninfo.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bcmpgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bnameblock.h>
#include <tol/tol_bvmatgra.h>
#include <tol/tol_oiscreator.h>
#include <tol/tol_StoreZipArchive.h>

#ifdef __USE_TC__
#  include <tol/tol_bctmigra.h>
#  include <tol/tol_bctmsgra.h>
#  include <tol/tol_bctsrgra.h>
#endif
#include <tol/tol_baia.h>
#include <tol/tol_btmsgra.h>
#include <tol/tol_btmsgrav.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bmatimp.h>
#include <tol/tol_delay_init.h>
#include <tol/tol_bmethodhook.h>
#include <tol/tol_oisloader.h>
#include <tol/tol_bsymboltable.h>

#include <malloc.h>
#include <locale.h>

#define TRACE_CLASS_SIZE(CLASS) \
  Std(BText("\nsizeof("#CLASS")=") << ((int)sizeof(CLASS))); 

#define TRACE_TEMPLATE_TYPE_SIZE(CLASS)  \
TRACE_CLASS_SIZE(CLASS);                 \
TRACE_CLASS_SIZE(BGraContens < CLASS >); \
TRACE_CLASS_SIZE(BGraConstant< CLASS >); \
TRACE_CLASS_SIZE(BRenContens < CLASS >); \
TRACE_CLASS_SIZE(BCopyContens < CLASS >); \
TRACE_CLASS_SIZE(BTmpContens < CLASS >); 

//--------------------------------------------------------------------
// static variables and initializations
//--------------------------------------------------------------------
BTraceInit("language.cpp");

BList* BPackage::required_;

//--------------------------------------------------------------------
  TOL_API void StopFlagOn() 
//--------------------------------------------------------------------
{
    BGrammar::Turn_StopFlag_On();
}

//--------------------------------------------------------------------
  TOL_API void StopFlagOff() 
//--------------------------------------------------------------------
{
    BGrammar::Turn_StopFlag_Off();
}

//--------------------------------------------------------------------
  TOL_API bool StopFlag()
//--------------------------------------------------------------------
{
    return BGrammar::StopFlag();
}

BGrammar* GraAnything () { return(BAnyGrammar     ::AnyGrammar()); }
BGrammar* GraCode     () { return(BUserCode       ::OwnGrammar()); }
BGrammar* GraDate     () { return(BUserDate       ::OwnGrammar()); }
#ifdef __USE_TC__
BGrammar* GraCTime    () { return(BUserCTime      ::OwnGrammar()); }
BGrammar* GraCTimeSet () { return(BUserCTimeSet   ::OwnGrammar()); }
BGrammar* GraCSeries  () { return(BUserCTimeSeries::OwnGrammar()); }
#endif /* __USE_TC__ */
BGrammar* GraPolyn    () { return(BUserPol        ::OwnGrammar()); }
BGrammar* GraRatio    () { return(BUserRat        ::OwnGrammar()); }
BGrammar* GraReal     () { return(BUserDat        ::OwnGrammar()); }
BGrammar* GraComplex  () { return(BUserCmp        ::OwnGrammar()); }
BGrammar* GraMatrix   () { return(BUserMat        ::OwnGrammar()); }
BGrammar* GraVMatrix  () { return(BUserVMat       ::OwnGrammar()); }
BGrammar* GraSet      () { return(BUserSet        ::OwnGrammar()); }
BGrammar* GraText     () { return(BUserText       ::OwnGrammar()); }
BGrammar* GraTimeSet  () { return(BUserTimeSet    ::OwnGrammar()); }
BGrammar* GraSerie    () { return(BUserTimeSerie  ::OwnGrammar()); }
BGrammar* GraNameBlock() { return(BUserNameBlock  ::OwnGrammar()); }

static BStruct* _str_ref_Anything  = NULL;
static BStruct* _str_ref_Code      = NULL;
static BStruct* _str_ref_Date      = NULL;
static BStruct* _str_ref_Polyn     = NULL;
static BStruct* _str_ref_Ratio     = NULL;
static BStruct* _str_ref_Real      = NULL;
static BStruct* _str_ref_Complex   = NULL;
static BStruct* _str_ref_Matrix    = NULL;
static BStruct* _str_ref_VMatrix   = NULL;
static BStruct* _str_ref_Set       = NULL;
static BStruct* _str_ref_NameBlock = NULL;
static BStruct* _str_ref_Text      = NULL;
static BStruct* _str_ref_TimeSet   = NULL;
static BStruct* _str_ref_Serie     = NULL;

BStruct* RefAnything()  { return(_str_ref_Anything); }
BStruct* RefCode()      { return(_str_ref_Code); }
BStruct* RefDate()      { return(_str_ref_Date); }
BStruct* RefPolyn()     { return(_str_ref_Polyn); }
BStruct* RefRatio()     { return(_str_ref_Ratio); }
BStruct* RefReal()      { return(_str_ref_Real); }
BStruct* RefComplex()   { return(_str_ref_Complex); }
BStruct* RefMatrix()    { return(_str_ref_Matrix); }
BStruct* RefVMatrix()   { return(_str_ref_VMatrix); }
BStruct* RefSet()       { return(_str_ref_Set); }
BStruct* RefNameBlock() { return(_str_ref_NameBlock); }
BStruct* RefText()      { return(_str_ref_Text); }
BStruct* RefTimeSet()   { return(_str_ref_TimeSet); }
BStruct* RefSerie()     { return(_str_ref_Serie); }

//--------------------------------------------------------------------
bool InitRefStructs()
//--------------------------------------------------------------------
{
  if(_str_ref_Anything) { return(false); }
  _str_ref_Anything = NewStructSymbol("@Anything","Anything:V");
  _str_ref_Code = NewStructSymbol("@Code","Code:V");
  _str_ref_Date = NewStructSymbol("@Date","Date:V");
  _str_ref_Polyn = NewStructSymbol("@Polyn","Polyn:V");
  _str_ref_Ratio = NewStructSymbol("@Ratio","Ratio:V");
  _str_ref_Real = NewStructSymbol("@Real","Real:V");
  _str_ref_Complex = NewStructSymbol("@Complex","Complex:V");
  _str_ref_Matrix = NewStructSymbol("@Matrix","Matrix:V");
  _str_ref_VMatrix = NewStructSymbol("@VMatrix","VMatrix:V");
  _str_ref_Set = NewStructSymbol("@Set","Set:V");
  _str_ref_NameBlock = NewStructSymbol("@NameBlock","NameBlock:V");
  _str_ref_Text = NewStructSymbol("@Text","Text:V");
  _str_ref_TimeSet = NewStructSymbol("@TimeSet","TimeSet:V");
  _str_ref_Serie = NewStructSymbol("@Serie","Serie:V");
}


//--------------------------------------------------------------------
int AliveObjects()
//--------------------------------------------------------------------
{
  int n = BSyntaxObject::NSyntaxObject()
         -BSourcePath::GetItemNumber();
  return(n);
}

//--------------------------------------------------------------------
  void TestAliveObjects(int before, int after, const BText& step)
//--------------------------------------------------------------------
{
#ifdef _DEBUG
  Std(BText("\nTesting alive objects at ")+step+" step:"
      " before:"<<before<<"; after:"<<after);
  if(before<after)
    {
      Warning(BText("There are ")<<(after-before)<<
                " undeleted objects at "<<step<<" step. \n" 
            "There is a referential integrity fail and TOL could be "
            "using more memory than neccessary.\n"
            "If you can to reproduce the error and isolate it in a "
            "simple TOL file, please, send us a bug report to "
            "bugs@tol-project.org with this information in order to "
            "help us improving Tol");
    }
  else if(before>after)
    {
      Error(BText("There are ")<<(before-after)<<
                " lost objects at "<<step<<" step\n"
            "There is a referential integrity fail and TOL could be "
            "destroying objects that should survive.\n"
            "If you can to reproduce the error and isolate it in a "
            "simple TOL file, please, send us a bug report to "
            "bugs@tol-project.org with this information in order to "
            "help us improving Tol");
    }
  else
  {
    Std("\nAlive objects test is OK\n");
  }
#endif
}

//--------------------------------------------------------------------
void InteractiveTOLHelp()

/*! PURPOSE: Help on commands of terminal dialog for probe the language.
 */
//--------------------------------------------------------------------
{
  BText helpEng = "\n"
  "=========================\n"
  "Interactive TOL Commands:\n" 
  "=========================\n"
  "  <tol expression>  : executes a TOL expression\n"
  "  $HELP             : displays this help\n" 
  "  $SYS <command>    : executes an operative system command\n" 
  "  $GRAMMAR          : sets the default TOL grammar\n" 
  "  $DELETE           : deletes TOL objects that has been previously created by user\n" 
  "  $DUMP             : displays a TOL object\n" 
  "  $STATUS           : displays some statistics about current TOL status\n" 
  "  $SHOW_CLASS_SIZES : displays size of some internal C++ classes\n" 
  "  $OIS              : interactive handler for OIS stores\n" 
  "  $END              : exits from Interactive TOL\n";
  BText helpSpa = "\n"
  "============================\n"
  "Comandos de TOL Interactivo:\n" 
  "============================\n"
  "  <tol expression>  : ejecuta una expresi�n TOL\n"
  "  $HELP             : muestra esta ayuda\n" 
  "  $SYS <command>    : ejecuta un comando del sistema operativo\n" 
  "  $GRAMMAR          : especifica la gram�tica por defecto\n" 
  "  $DELETE           : borra objetos TOL creados previamente\n" 
  "  $DUMP             : muestra un objeto TOL\n" 
  "  $STATUS           : muestra algunas estatid�sticas acerca del estado de TOL\n" 
  "  $SHOW_CLASS_SIZES : muestra el tama�o de algunas clases C++ internas\n" 
  "  $OIS              : visualizador interactivo de imagenes OIS\n" 
  "  $END              : sale del modo interactivo de TOL\n"
  "Si no visualiza correctamente las tildes o la � en el modo de di�logo "
  "escoja un tipo de letra adecuado como el 'lucilda console' y ejecute "
  "el comando:\n"
  "$SYS chcp 1252\n";
  Std(I2(helpEng,helpSpa));
};

//--------------------------------------------------------------------
static BText DialogInfo()
//--------------------------------------------------------------------
{
  int aliveObjects = BSyntaxObject::NSyntaxObject();

    return(BText("\nTime = ")    + BTimer::Text() +
     "\tCores = "    + BCore::NCore() +
     "\tObjects = "  + aliveObjects +
     "\tFiles = "    + BSetFromFile::NSetFromFile());
}

//--------------------------------------------------------------------
  void GetLineFromStdIn(BText& txt)
//--------------------------------------------------------------------
{
  GetLine(cin, txt);
}

//--------------------------------------------------------------------
void TolShowStatus()
//--------------------------------------------------------------------
{
  Std("\n------------------------------------------------------------------------------");
  Std(BText("\nTOL Status "));
  Std("\n------------------------------------------------------------------------------\n");

  BTimer::PrintProcess();
  Std("\n------------------------------------------------------------------------------");
  Std(BText("\nStatus of hashed global symbol table "));
  Std("\n------------------------------------------------------------------------------\n");
  BGrammar::SymbolTable().Dump();


  Std("\n------------------------------------------------------------------------------");
  Std(BText("\nGeneral status"));
  Std("\n------------------------------------------------------------------------------\n");
  Std(DialogInfo());
//Std(BText("\nGrammar hashing size    = ")<<BGrammarHashSize/1024.0<<" KBytes");
  Std(BText("\nTime Set cache size     = ")<<BTimeSet::HashUsedKBytes()<<" KBytes");
  Std(BText("\nTime Serie cache number = ")<<BData::Count());
  Std(BText("\nTime Serie cache size   = ")<<BData::KBytes()<<" KBytes");
  BVMat::ShowCholmodStats();

#     ifdef TRACE_OPTINFO
  Std(BText("\nOptional info. number   = ")<<BSyntaxObject::OptInfoCount()<<" of "<<(BSyntaxObject::NSyntaxObject()));
  Std(BText("\nOptional info. named    = ")<<BSyntaxObject::OptInfoCountName());
  Std(BText("\nOptional info. loc.nam. = ")<<BSyntaxObject::OptInfoCountExpression());
  Std(BText("\nOptional info. desc.    = ")<<BSyntaxObject::OptInfoCountDescription());
  Std(BText("\nOptional info. size     = ")<<BSyntaxObject::OptInfoKBytes()<<" KBytes");
#     endif

#if   (__USE_POOL__==__POOL_BFSMEM__)
  void BFSMEM_ShowStatsOfAllInstances();
  BFSMEM_ShowStatsOfAllInstances();
#     endif 

//#     define __USE_TOL_IEEE__
#     ifdef __USE_TOL_IEEE__
  BINT64& is_nan_num_call();
  Std(BText("\nis_nan_num_call = ")<<is_nan_num_call());
#     endif

  void StatRealloc();
  StatRealloc();
  Std("\n------------------------------------------------------------------------------");
  Std(BText("\nEnd of TOL Status "));
  Std("\n------------------------------------------------------------------------------\n\n");
}

//--------------------------------------------------------------------
void InteractiveTOL()

/*! PURPOSE: Makes a terminal dialog for probe the language.
 */
//--------------------------------------------------------------------
{
  Std("\n-----------------------------------------------------------------");
  Std(I2("\nTOL Simple Expression Analizer.",
         "\nAnalizador de expresiones simples TOL."));
  Std("\n-----------------------------------------------------------------");
  Std(I2("\nType $HELP and return to know more about this TOL environment.",
         "\nTeclee $HELP para saber m�s de este entorno TOL."));
  BDat x;
  BList* stack = NIL;
  BText txt;

  while(ToUpper(txt)!="$END")
  {
  //Std(DialogInfo());
    Std(Out() << "\n> ");
    GetLineFromStdIn(txt);
    BText command = Compact(ToUpper(txt));
    if(command.BeginWith("$SYS ")) //
    { 
      system(txt.String()+5); 
    }
    else if(command=="$GRAMMAR")
    {
      BGrammar* gra;
      do
      {
        cout << "\nGrammar name ? : ";
        GetLineFromStdIn(txt);
        gra = BGrammar::FindByName(txt);
      }
      while(!gra);
      BGrammar::PutLast(BGrammar::FindByName(txt));
    } 
    else if(command=="$DELETE")
    {
      Std(DialogInfo());
      Std(I2("\nWrite object name or $ALL to delete all created objects : ", 
             "\nEscribe el nombre del objeto o $ALL para borrarlos todos : "));
      GetLineFromStdIn(txt);
      if(txt=="$ALL") { DESTROY(stack); }
      else
      {
        BSyntaxObject* del = (BSyntaxObject*)LstFindByName(stack, txt);
        if(del)
        { 
          stack = LstRemoveAtom(stack, del);
          DESTROY(del);
        }
        else { Error(txt + I2(" not found."," no se encuentra")); }
      }
      Std(DialogInfo());
    }
    else if(command=="$DUMP")
    {
      Std(I2("\nWrite object name or $ALL to show all created objects : ", 
              "\nEscribe el nombre del objeto o $ALL para mostrarlos todos : "));
      GetLineFromStdIn(txt);
      BGrammar* graD = NIL;
      if(txt=="$ALL")
      {
        BList* lst = stack;
        while(lst)
        {
          BSyntaxObject* obj = (BSyntaxObject*)lst->Car(); 
          Std(obj->Dump());  Std("\n");
          Std(BText(" Number of references for this object is ")+obj->NRefs()+"\n");
          lst=lst->Cdr();
        }
      }
      else if(graD=BGrammar::FindByName(txt))
      {
        BList* lst = graD->Variables();
        while(lst)
        {
          BSyntaxObject* obj = (BSyntaxObject*)lst->Car();
          Std(obj->Info());  Std("\n");
          Std(BText(" Number of references for this object is ")+obj->NRefs()+"\n");
          lst=lst->Cdr();
        }
      }
      else
      {
        BSyntaxObject* obj = (BSyntaxObject*)LstFindByName(stack, txt);
        if(obj) 
        { 
          Std(obj->Identify()); Std("\n"); 
          Std(BText(" Number of references for this object is ")+obj->NRefs()+"\n");
        }
        else { Error(txt + I2(" not found."," no se encuentra")); }
      }
    }
    else if(command=="$STATUS")
    {
      TolShowStatus();
    }
    else if(command=="$SHOW_CLASS_SIZES")
    {
      TRACE_CLASS_SIZE(BAtom);
      TRACE_CLASS_SIZE(BSyntaxObject);
      TRACE_TEMPLATE_TYPE_SIZE(BText);
      TRACE_TEMPLATE_TYPE_SIZE(BDat);
      TRACE_TEMPLATE_TYPE_SIZE(BDate);
      TRACE_TEMPLATE_TYPE_SIZE(BCode);
      TRACE_TEMPLATE_TYPE_SIZE(BSet);
      TRACE_TEMPLATE_TYPE_SIZE(BMat);
      TRACE_TEMPLATE_TYPE_SIZE(BCmp);
      TRACE_TEMPLATE_TYPE_SIZE(BPol);
      TRACE_TEMPLATE_TYPE_SIZE(BRat);
    }
    else if(command=="$HELP")
    {
      InteractiveTOLHelp();
    } 
    else if(command=="$OIS")
    {
      BOisLoader::Interactive();
    } 
    else if(txt.HasName())
    {
      if(command=="$END") { break; }
      //BTimer tms(BText("Evaluating ")+txt);

      time_t start, stop;
      double elapsedTime;
      char label[30];
      start = time(NULL);
      BSyntaxObject* result = GraAnything()->EvaluateExpr(txt);
      stop = time(NULL);

      elapsedTime = difftime(stop, start);
      if(result)
      {
        if((result->Grammar()==GraSet())||
           (result->Grammar()==GraMatrix())||
           (result->Grammar()==GraVMatrix()))
        { 
          Std(result->Info());
        } 
        else 
        { 
          Std(result->Dump());
        }
        if(elapsedTime>0.09) { sprintf(label, " (%0.2f sec)", elapsedTime); Std(label); }
        Std("\n"); 
        BGrammar::PutLast(result->Grammar());
        if(!(result->Name().HasName())) { DESTROY(result); }
        else                            { stack = Cons(result, stack); }
      }
      else
      {
        if(command=="END") { break; } 
      }
    }
  }
  void TknFindReport();
  TknFindReport();
  DESTROY(stack);
}


// Methods hooks

// Serie methods

static BMethodHookPtr SerieChartHook = NULL;
BMethodHookPtr InstallSerieChartHook(BMethodHookPtr Handler)
{
    BMethodHookPtr tmp = SerieChartHook;
    SerieChartHook = Handler;
    return tmp;
}

static BMethodHookPtr SerieTableHook = NULL;
BMethodHookPtr InstallSerieTableHook(BMethodHookPtr Handler)
{
    BMethodHookPtr tmp = SerieTableHook;
    SerieTableHook = Handler;
    return tmp;
}


static BMethodHookPtr SerStatHook = NULL;
BMethodHookPtr InstallSerStatHook(BMethodHookPtr Handler)
{
    BMethodHookPtr tmp = SerStatHook;
    SerStatHook = Handler;
    return tmp;
}


//--------------------------------------------------------------------
static void BSTWriteFile(BList* lst, 
                         BText fileName     = "Std",
                         BText header       = "",
                         BBool rewrite      = false,
                         BText colSeparator = ";", 
                         BText rowSeparator = ";\n")
//--------------------------------------------------------------------
{
  ofstream out;
  if(fileName!="Std") 
  {
    if(rewrite) { out.open(fileName.String()); }
    else        { out.open(fileName.String(),ios::app); }
  }
//Std(Out()+"BSTWriteFile("+fileName+") 1");
  while(lst)
  {
    BSyntaxObject* syn = (BSyntaxObject*)Car(lst);
    if(syn->Grammar()->Name()=="Set")
    {
      BSet     set = Set(syn);
      BStruct* str = Set(set[1]).Struct();
      if((set.Card()>=1)&&(set[1]->Grammar()->Name()=="Set"))
      {
        BInt m, c;
        if (str) {
          if (header=="_DEFAULT_HEADER_") {
            WriteFileOrStd(fileName, out, (str->FullName() + colSeparator));
            for(c=0; c<str->Size()-1; c++)
              {
                WriteFileOrStd(fileName, out, ((*str)[c].Name() + colSeparator));
              }
            WriteFileOrStd(fileName, out, ((*str)[c].Name() + rowSeparator));
          } else if (header.HasName()) {
            WriteFileOrStd(fileName, out, (header+rowSeparator));
          }
        } else if (header.HasName() && header!="_DEFAULT_HEADER_") {
          WriteFileOrStd(fileName, out, (header+rowSeparator));
        }
        
        for(m=1;m<=set.Card();m++)
        {
          BSet& setm = Set(set[m]);
          if(str)
          {
            WriteFileOrStd(fileName, out, (set[m]->Name() + colSeparator));
          }
          for(c=1; c< setm.Card(); c++)
          {
            WriteFileOrStd(fileName, out, (setm[c]->Dump() + colSeparator));
          }
          WriteFileOrStd(fileName, out, (setm[c]->Dump() + rowSeparator));
        }
      }
    }
    lst = Cdr(lst);
  }
//Std(Out()+"BSTWriteFile("+fileName+") 2");
  if(fileName!="Std") { out.close(); }
}



//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetTable);

DefExtOpr(1, BSetTable, "BSTFile", 1, 6, "Set Text Text Real Text Text",
    I2("(Set set [, Text fileName=\"Std\", Text header=\"_DEFAULT_HEADER_\", Real rewrite=TRUE, Text colSeparator = \";\", Text rowSeparator = \";\\n\"])",
       "(Set cto [, Text nombreFichero=\"Std\", Text cabecera=\"_DEFAULT_HEADER_\", Real reescribir=CIERTO, Text separadorDeColumnas = \";\", Text separadorDeFilas = \";\\n\"])"),
    I2("Writes a set of sets in a file with BST format.\n"
       "If no file is specified standard output \"Std\" will be used\n"
       "You can specify a header string that will be written at first line\n"
       "If header is \"_DEFAULT_HEADER_\" and first line has structure then default header will be the list of field names" 
       "If header is \"\" no header will be written.\n" 
       "If you want to append results to an existant file use rewrite=FALSE\n"
       "You can specify any column and row separators.\n"
       "Results only will be readable by TOL if default parameters are used.",
       "Escribe un conjunto de conjuntos de tipo tabla en un fichero con formato BST.\n"
       "Si no se especifica ning�n fichero se usar� la salida est�ndar \"Std\" \n"
       "Se puede especificar una cadena de cabecera que se escribir� en la primera l�nea\n"
       "Si la cabecera es \"_DEFAULT_HEADER_\" y la primera l�nea tiene estructura se usar� la lista de nombres de campos como cabecera" 
       "Si la cabecera es \"\" no se escribir� cabecera.\n" 
       "Si se quiere a�adir los resultados a un fichero existente usese rewrite=FALSE\n"
       "Es posible especificar cualquier separador de columna y fila \n"
       "El resultado s�lo ser� legible por TOL si se usan los par�metros por defecto."),
    BOperClassify::General_);

//--------------------------------------------------------------------
void BSetTable::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("BSTFile")) { return; }
  contens_ = Set(Arg(1));
  BText  fileName     = "Std";
  BText  header       = "_DEFAULT_HEADER_";
  BBool  rewrite      = BTRUE;
  BText  colSeparator = ";";
  BText  rowSeparator = ";\n";
  if(Arg(2)) { fileName     = Text(Arg(2));    }
  if(Arg(3)) { header       = Text(Arg(3));    }
  if(Arg(4)) { rewrite      = Real(Arg(4))!=0; }
  if(Arg(5)) { colSeparator = Text(Arg(5));    }
  if(Arg(6)) { rowSeparator = Text(Arg(6));    }

  BList* lst = NCons(Arg(1));
  BSTWriteFile(lst, fileName, header, rewrite, colSeparator, rowSeparator);
  DESTROY(lst);
}

//--------------------------------------------------------------------
static void BMTWriteFile(BList* lst,
                         BText fileName     = "Std",
                         BText header       = "",
                         BBool rewrite      = false,
                         BText colSeparator = ";", 
                         BText rowSeparator = ";\n")
//--------------------------------------------------------------------
{
  int i, j;
  ofstream out;
  if(fileName!="Std") 
  { 
    if(rewrite) { out.open(fileName.String()); }
    else        { out.open(fileName.String(),ios::app); }
  }
  if(header.HasName() && (header!="_DEFAULT_HEADER_"))
  {
    WriteFileOrStd(fileName, out, (header+rowSeparator));
  } 
  while(lst)
  {
    BSyntaxObject* syn = (BSyntaxObject*)Car(lst);
    if(syn->Grammar()->Name()=="Matrix")
    {
      BMat& mat = Mat(syn);
      for(i=0;i<mat.Rows();i++)
      {
        for(j=0;j<mat.Columns()-1;j++)
        {
          WriteFileOrStd(fileName, out, (mat(i,j).Name() + colSeparator));
        }
        WriteFileOrStd(fileName, out, (mat(i,j).Name() + rowSeparator));
      }
    } 
    lst = Cdr(lst);
  }
  if(fileName!="Std") { out.close(); }
}


//--------------------------------------------------------------------
class BSetBMTFile: public BSetTemporary
//--------------------------------------------------------------------
{
public:
    BSetBMTFile(BList* arg=NIL)
  : BSetTemporary(arg) 
  { 
      CalcContens(); flags_.calculated_= BTRUE; 
  }
    void CalcContens();
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DeclareEvaluator(BSetBMTFile);

DefExtOpr(1, BSetBMTFile, "BMTFile", 1, 6, "Set Text Text Real Text Text",
    I2("(Set set [, Text fileName=\"Std\", Text header=\"_DEFAULT_HEADER_\", Real rewrite=TRUE, Text colSeparator = \";\", Text rowSeparator = \";\\n\"])",
       "(Set cto [, Text nombreFichero=\"Std\", Text cabecera=\"_DEFAULT_HEADER_\", Real reescribir=CIERTO, Text separadorDeColumnas = \";\", Text separadorDeFilas = \";\\n\"])"),
    I2("Writes a set of matrices in a file with BMT format.\n"
       "If no file is specified standard output \"Std\" will be used\n"
       "You can specify a header string that will be written at first line\n"
       "If header is \"_DEFAULT_HEADER_\" or \"\" no header will be written.\n" 
       "If you want to append results to an existant file use rewrite=FALSE\n"
       "You can specify any column and row separators.\n"
       "Results only will be readable by TOL if default parameters are used.",
       "Escribe un conjunto de matrices en un fichero con formato BMT.\n"
       "Si no se especifica ning�n fichero se usar� la salida est�ndar \"Std\" \n"
       "Se puede especificar una cadena de cabecera que se escribir� en la primera l�nea\n"
       "Si la cabecera es \"_DEFAULT_HEADER_\" o \"\" no se escribir� cabecera\n" 
       "Si se quiere a�adir los resultados a un fichero existente usese rewrite=FALSE\n"
       "Es posible especificar cualquier separador de columna y fila\n"
       "El resultado s�lo ser� legible por TOL si se usan los par�metros por defecto."),
    BOperClassify::General_);

//--------------------------------------------------------------------
void BSetBMTFile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("BMTFile")) { return; }
  contens_ = Set(Arg(1));
  BText  fileName     = "Std";
  BText  header       = "";
  BBool  rewrite      = BTRUE;
  BText  colSeparator = ";";
  BText  rowSeparator = ";\n";
  if(Arg(2)) { fileName     = Text(Arg(2));    }
  if(Arg(3)) { header       = Text(Arg(3));    }
  if(Arg(4)) { rewrite      = Real(Arg(4))!=0; }
  if(Arg(5)) { colSeparator = Text(Arg(5));    }
  if(Arg(6)) { rowSeparator = Text(Arg(6));    }

  BList* lst_aux_copy = contens_.ToReversedList();
  BMTWriteFile(lst_aux_copy, fileName, header, rewrite, colSeparator, rowSeparator);
  DESTROY(lst_aux_copy);

}

//--------------------------------------------------------------------
static void StatWriteFile(BList* lst, 
                          BText fileName     = "Std",
                          BText header       = "",
                          BBool rewrite      = false,
                          BText colSeparator = ";", 
                          BText rowSeparator = ";\n")
//--------------------------------------------------------------------
{
  BArray< BArray< BText > > table;
  BInt i,j, statNumber = 11;
  ofstream out;
  if(fileName!="Std") 
  { 
    if(rewrite) { out.open(fileName.String()); }
    else        { out.open(fileName.String(),ios::app); }
  }
  if(!out.good()) 
  {
    Error(I2("Cannot open for writing file ",
             "No se puede abrir para escritura el fichero ")+fileName);
    Std(BText("\nNo se puede abrir para escritura el fichero ") +fileName);
    return;
  }
  table.ReallocBuffer(statNumber);
  BInt numSer = LstLength(lst);
  for (i=0; i<statNumber; i++) { table[i].ReallocBuffer(numSer+1); }
  i=0;
  if(header=="_DEFAULT_HEADER_")
  {
    table[i++][0] = I2("Statistic  ", "Estad�stico");
    table[i++][0] = I2("Count      ", "Longitud   ");
    table[i++][0] = I2("Maximum    ", "M�ximo     ");
    table[i++][0] = I2("Minimum    ", "M�nimo     ");
    table[i++][0] = I2("Sum        ", "Suma       ");
    table[i++][0] = I2("Average    ", "Media      ");
    table[i++][0] = I2("Std. Desv. ", "Desv. Tip. ");
    table[i++][0] = I2("Varianze   ", "Varianza   ");
    table[i++][0] = I2("Asymmetry   ", "Asimetr�a  ");
    table[i++][0] = I2("Kurtosis   ", "Curtosis   ");
    table[i++][0] = I2("Median     ", "Mediana    ");
    j=0;
  }
  else
  {
    if(header.HasName())
    {
      WriteFileOrStd(fileName, out, (header+rowSeparator));
    } 
    j=-1;
  }
  while(lst)
  {
    BSyntaxObject* syn = (BSyntaxObject*)Car(lst);
    if(syn->Grammar()->Name()=="Serie")
    {
      j++;
      BUserTimeSerie* ser = Tsr(syn);
      i=0;
      table[i++][j] = ser->Identify();
      table[i++][j] = StatCount   (ser).Name();
      table[i++][j] = StatMax     (ser).Name();
      table[i++][j] = StatMin     (ser).Name();
      table[i++][j] = StatSum     (ser).Name();
      table[i++][j] = StatAverage (ser).Name();
      table[i++][j] = StatStDs    (ser).Name();
      table[i++][j] = StatVarianze(ser).Name();
      table[i++][j] = StatAsimetry(ser).Name();
      table[i++][j] = StatKurtosis(ser).Name();
      table[i++][j] = StatMedian  (ser).Name();
    }
    lst = Cdr(lst);
  }
  numSer = j;
  if(out.good()) 
  {
    for (j=0; j<=numSer; j++) 
    {
      for (i=0; i<statNumber-1; i++) 
      {
        WriteFileOrStd(fileName, out, table[i][j]+colSeparator);
      }
      WriteFileOrStd(fileName, out, table[i][j]+rowSeparator);
    }
  } 
  else 
  {
    Error(I2("Cannot open for writing file ",
             "No se puede abrir para escritura el fichero ")+fileName);
    Std(BText("\nNo se puede abrir para escritura el fichero ") +fileName);
  }
  if(fileName!="Std") 
  {
    out.close(); 
  }
}


//--------------------------------------------------------------------
class BSetStatFile: public BSetTemporary
//--------------------------------------------------------------------
{
public:
    BSetStatFile(BList* arg=NIL)
  : BSetTemporary(arg) 
  { 
      CalcContens(); flags_.calculated_= BTRUE; 
  }
    void CalcContens();
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------  
DeclareEvaluator(BSetStatFile);

DefExtOpr(1, BSetStatFile, "StatFile", 1, 6, "Set Text Text Real Text Text",
    I2("(Set set [, Text fileName=\"Std\", Text header=\"_DEFAULT_HEADER_\", Real rewrite=TRUE, Text colSeparator = \";\", Text rowSeparator = \";\\n\"])",
       "(Set cto [, Text nombreFichero=\"Std\", Text cabecera=\"_DEFAULT_HEADER_\", Real reescribir=CIERTO, Text separadorDeColumnas = \";\", Text separadorDeFilas = \";\\n\"])"),
    I2("Write a set of statistics about a set of series in a file.\n"
       "If no file is specified standard output \"Std\" will be used\n"
       "You can specify a header string that will be written at first line\n"
       "If header==\"_DEFAULT_HEADER_\" statistics names will be written as header.\n"
       "If header is \"\" no header will be written.\n" 
       "If you want to append results to an existant file use rewrite=FALSE\n"
       "You can specify any column and row separators.",
       "Escribe un conjunto de estad�sticos acerca de un conjunto de series en un fichero.\n"
       "Si no se especifica ning�n fichero se usar� la salida est�ndar \"Std\" \n"
       "Se puede especificar una cadena de cabecera que se escribir� en la primera l�nea\n"
       "Si la cabecera es \"_DEFAULT_HEADER_\" se usar�n los nombres de los estad�sticos como cabecera\n" 
       "Si la cabecera es \"\" no se escribir� cabecera.\n" 
       "Si se quiere a�adir los resultados a un fichero existente usese rewrite=FALSE\n"
       "Es posible especificar cualquier separador de columna y fila"),
    BOperClassify::General_);
//--------------------------------------------------------------------
void BSetStatFile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("StatFile")) { return; }
  contens_ = Set(Arg(1));
  BText  fileName     = "Std";
  BText  header       = "_DEFAULT_HEADER_";
  BBool  rewrite      = BTRUE;
  BText  colSeparator = ";";
  BText  rowSeparator = ";\n";
  if(Arg(2)) { fileName     = Text(Arg(2));    }
  if(Arg(3)) { header       = Text(Arg(3));    }
  if(Arg(4)) { rewrite      = Real(Arg(4))!=0; }
  if(Arg(5)) { colSeparator = Text(Arg(5));    }
  if(Arg(6)) { rowSeparator = Text(Arg(6));    }
  BList* lst_aux_copy = contens_.ToReversedList();
  StatWriteFile(lst_aux_copy, fileName, header, rewrite, colSeparator, rowSeparator);
  DESTROY(lst_aux_copy);
}

//--------------------------------------------------------------------
class BSetBDTFile: public BSetTemporary
//--------------------------------------------------------------------
{
public:
    BSetBDTFile(BList* arg=NIL)
  : BSetTemporary(arg) 
  { 
      CalcContens(); flags_.calculated_= BTRUE; 
  }
    void CalcContens();
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DeclareEvaluator(BSetBDTFile);

DefExtOpr(1, BSetBDTFile, "BDTFile", 1, 6, "Set Text Text Real Text Text",
    I2("(Set set [, Text fileName=\"Std\", Text header=\"_DEFAULT_HEADER_\", Real rewrite=TRUE, Text colSeparator = \";\", Text rowSeparator = \";\\n\"])",
       "(Set cto [, Text nombreFichero=\"Std\", Text cabecera=\"_DEFAULT_HEADER_\", Real reescribir=CIERTO, Text separadorDeColumnas = \";\", Text separadorDeFilas = \";\\n\"])"),
    I2("Writes a set of series in a file with BDT format.\n"
       "If no file is specified standard output \"Std\" will be used\n"
       "You can specify a header string that will be written at first line\n"
       "If header==\"_DEFAULT_HEADER_\" then dating and list of time series names will be written as header.\n"
       "If header is \"\" no header will be written.\n" 
       "If you want to append results to an existant file use rewrite=FALSE\n"
       "You can specify any column and row separators.\n"
       "Results only will be readable by TOL if default parameters are used.",
       "Escribe un conjunto de series en un fichero con formato BDT."
       "Si no se especifica ning�n fichero se usar� la salida est�ndar \"Std\" \n"
       "Se puede especificar una cadena de cabecera que se escribir� en la primera l�nea\n"
       "Si la cabecera es \"_DEFAULT_HEADER_\" se usar�n los nombres del fechado y de la lista de series como cabecera\n" 
       "Si la cabecera es \"\" no se escribir� cabecera.\n" 
       "Si se quiere a�adir los resultados a un fichero existente usese rewrite=FALSE\n"
       "Es posible especificar cualquier separador de columna y fila.\n"
       "El resultado s�lo ser� legible por TOL si se usan los par�metros por defecto."),
    BOperClassify::General_);

//--------------------------------------------------------------------
void BSetBDTFile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("BDTFile")) { return; }
  contens_ = Set(Arg(1));
  BText  fileName     = "Std";
  BText  header       = "_DEFAULT_HEADER_";
  BBool  rewrite      = BTRUE;
  BText  colSeparator = ";";
  BText  rowSeparator = ";\n";
  if(Arg(2)) { fileName     = Text(Arg(2));    }
  if(Arg(3)) { header       = Text(Arg(3));    }
  if(Arg(4)) { rewrite      = Real(Arg(4))!=0; }
  if(Arg(5)) { colSeparator = Text(Arg(5));    }
  if(Arg(6)) { rowSeparator = Text(Arg(6));    }

  BInt n, m;

  BSerieTable table;
  table.AddSet(contens_);
  table.Fill();
  if(table.Dating() && (table.NumDates()*table.NumSeries()>0))
  {
    ofstream out;
    if(fileName!="Std")
    {
      if(rewrite) { out.open(fileName.String()); }
      else        { out.open(fileName.String(),ios::app); }
      if(header=="_DEFAULT_HEADER_")
      {
        out << table.Dating()->Identify() <<colSeparator;
        for(n=1; n<table.NumSeries(); n++) 
        { out << table.Name(n) << colSeparator; }
        out << table.Name(n) << rowSeparator;
      }
      else if(header.HasName()) 
      {
        out << header+rowSeparator; 
      }
      for(m=1; m<=table.NumDates(); m++)
      {
        out << BTableFormat::DateFormat().DateToText(table.Date(m)) << colSeparator;
        for(n=1; n<table.NumSeries(); n++)
        {
          out << table.TextData(n,m) << colSeparator;
        }
        out << table.TextData(n,m) << rowSeparator;
      }
    }
    else
    {
      if(header=="_DEFAULT_HEADER_")
      {
        Std(table.Dating()->Identify() + "; ");
        for(n=1; n<=table.NumSeries(); n++) { Std(table.Name(n) + colSeparator); }
        Std(rowSeparator);
      }
      else if(header.HasName()) 
      { 
        Std(header+rowSeparator); 
      }
      for(m=1; m<=table.NumDates(); m++)
      {
        Std(BTableFormat::DateFormat().DateToText(table.Date(m)) + colSeparator);
        for(n=1; n<table.NumSeries(); n++)
        {
          Std(table.TextData(n,m)+colSeparator);
        }
        Std(table.TextData(n,m)+rowSeparator);
      }
    }
  }
}

//--------------------------------------------------------------------
// Methods for TimeSet
//--------------------------------------------------------------------



//--------------------------------------------------------------------
// Methods for Serie
//--------------------------------------------------------------------




//--------------------------------------------------------------------
DefineMethodClass(BSerieStatistics, "Statistics", "Serie");

void BSerieStatistics::Apply(BList* lst, const BText& fileName) const
//--------------------------------------------------------------------
{
    StatWriteFile(lst, fileName);
    if((fileName!="Std") && BSys::Editor().HasName())
    { 
  BSys::EditTable(fileName, BSTFILE); 
    }
}


//--------------------------------------------------------------------
DefineMethodClass(BSeriePeriodicTable, "PeriodicTable", "Serie");

void BSeriePeriodicTable::Apply(BList* lst, const BText& fileName) const
//--------------------------------------------------------------------
{
    BBool edit = (fileName!="Std") && BSys::Editor().HasName();
    while(lst)
    {
  PeriodicTable(Tsr(Car(lst)),fileName);
  if(edit)
  { 
      BSys::EditTable(fileName, BSTFILE); 
  }
  lst = Cdr(lst);
    }
}


//--------------------------------------------------------------------
DefineMethodClass(BSerieBDCMethod, "BDC File", "Serie");

void BSerieBDCMethod::Apply(BList* lst, const BText& fileName) const
//--------------------------------------------------------------------
{
  BSerieTable table;
  table.AddList(lst);
  table.Fill();
  {
    ofstream out(fileName.String());
    table.WriteAsBDC(out);
  }
  BSys::EditUnlink(fileName, BDCFILE);
  DESTROY(lst);
}




//--------------------------------------------------------------------
class BSetBSIFile: public BSetTemporary
{
public:
    BSetBSIFile(BList* arg=NIL)
  : BSetTemporary(arg) 
  { 
      CalcContens(); flags_.calculated_= BTRUE; 
  }
    void CalcContens();
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DeclareEvaluator(BSetBSIFile);

DefExtOpr(1, BSetBSIFile, "BSIFile", 2, 2, "Set Text",
    I2("(Set set, Text fileName)",
       "(Set cto, Text nombreFichero)"),
    I2("Writes a set of series in a file with BDT format.",
       "Escribe un conjunto de series en un fichero con formato BDT."),
    BOperClassify::General_);

//--------------------------------------------------------------------
void  BSetBSIFile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("BSIFile")) { return; }
    BText fileName = Text(Arg(2));
    contens_ = Set(Arg(1));
    BList* lst_aux_copy = contens_.ToList();
    BTsrBSI::WriteFile(fileName, lst_aux_copy);
    DESTROY(lst_aux_copy);
}


//--------------------------------------------------------------------
DefineMethodClass(BTimeSetPage, "TextCalendar", "TimeSet");

void BTimeSetPage::Apply(BList* lst, const BText& fileName) const
//--------------------------------------------------------------------
{
    {
  ofstream out;
  if(fileName!="Std") { out.open(fileName.String()); }
  while(lst)
  {
      BText txt;
      BUserTimeSet* tms = (BUserTimeSet*)(Car(lst));
      BHash hash;
      tms->GetHashBetween(hash);
      BMonthInformation::PageMaker(txt, tms);
      WriteFileOrStd(fileName, out, txt);
      lst = Cdr(lst);
  }
  if(fileName!="Std") { out.close(); }
    }
    BSys::EditUnlink(fileName, TOLFILE);
}


//--------------------------------------------------------------------
static void BTimeSetTableMethod_Apply(BList* lst, const BText& fileName) 
//--------------------------------------------------------------------
{
    ofstream out;
    if(fileName!="Std") { out.open(fileName.String()); }
    
    BTmsAllUnion tmsUnion(LstDuplicate(lst));
    
    BHash hash; tmsUnion.GetHashBetween(hash);
    
    BDate d;
    BInt i,n;
    if(tmsUnion.NumArgs()>1)
    {
  WriteFileOrStd(fileName, out, "Union; ");
    }
    for(i=1; i<=tmsUnion.NumArgs(); i++)
    {
  BUserTimeSet* tms = Tms(tmsUnion.Arg(i));
  WriteFileOrStd(fileName, out, (tms->Identify()+"; "));
    }
    WriteFileOrStd(fileName, out, "\n");
    for(n=0; n<hash.Size(); n++)
    {
  d.PutHash(hash[n]);
  BText dateText = BTableFormat::DateFormat().DateToText(d) +"; ";
  if(tmsUnion.NumArgs()>1)
  {
      WriteFileOrStd(fileName, out, dateText);
  }
  for(i=1; i<=tmsUnion.NumArgs(); i++)
  {
      BUserTimeSet* tms = Tms(tmsUnion.Arg(i));
      BText txt = "; ";
      if(tms->Contain(d)) { txt = dateText; }
      WriteFileOrStd(fileName, out, txt);
  }
  WriteFileOrStd(fileName, out, "\n");
    }
    out.close();
    if((fileName!="Std") && BSys::Editor().HasName())
    { BSys::EditTable(fileName, BDTFILE); }
    if(fileName!="Std") { out.close(); }
}

//--------------------------------------------------------------------
static void BSerieTableMethod_Apply(BList* lst, const BText& fileName) 
//--------------------------------------------------------------------
{
  BDTWriteFileL(lst, fileName);
  if((fileName!="Std") && BSys::Editor().HasName())
  { BSys::EditTable(fileName, BDTFILE); }
}

//--------------------------------------------------------------------
static void BMatrixTableMethod_Apply(BList* lst, const BText& fileName) 
//--------------------------------------------------------------------
{
  BMTWriteFile(lst, fileName);
  if((fileName!="Std") && BSys::Editor().HasName())
  { BSys::EditTable(fileName, BMTFILE); }
}

//--------------------------------------------------------------------
static void BSetTableMethod_Apply(BList* lst, const BText& fileName)
//--------------------------------------------------------------------
{
  BSTWriteFile(lst, fileName);
  if((fileName!="Std") && BSys::Editor().HasName())
  {
    BSys::EditTable(fileName, BSTFILE); 
  }
}

//--------------------------------------------------------------------
DefineMethodClass(BAnythingTableMethod, "Table", "Anything");
void BAnythingTableMethod::Apply(BList* lst, const BText& fileName) const
//--------------------------------------------------------------------
{
  if(lst && lst->Car())
  {
    BSyntaxObject* obj = (BSyntaxObject*)lst->Car();
    BGrammar* g = obj->Grammar();
    BList* aux = lst;
    while(aux && g && (g!=GraAnything())) 
    { 
      obj = (BSyntaxObject*)lst->Car();
      if(g!=obj->Grammar()) { g = GraAnything(); }
      aux = aux->Cdr();
    }
    if(g==GraSet())
    {
      BSetTableMethod_Apply(lst,fileName);
    }
    else if(g==GraSerie())
    {
      BSerieTableMethod_Apply(lst,fileName);
    } 
    else if(g==GraMatrix())
    {
      BMatrixTableMethod_Apply(lst,fileName);
    }
    else if(g==GraTimeSet())
    {
      BTimeSetTableMethod_Apply(lst,fileName);
    }
    else
    {
      Error(I2("Can't apply method Table ",
               "No se puede aplicar el m�todo Table ")+
		        I2(" over objects of type ", 
               " sobre objetos de tipo ")+g->Name()); 
    }
  }
}


//--------------------------------------------------------------------
static void BSerieChartMethod_Apply(BList* lst, const BText& fileName) 
//--------------------------------------------------------------------
{
    if (SerieChartHook)
  (*SerieChartHook)(lst,fileName);
}

//--------------------------------------------------------------------
static void BMatrixChartMethod_Apply(BList* lst, const BText& fileName) 
//--------------------------------------------------------------------
{
  BMTWriteFile(lst, fileName);
  BSys::EditChart(fileName, BMTFILE);
}

//--------------------------------------------------------------------
static void BSetChartMethod_Apply(BList* lst, const BText& fileName) 
//--------------------------------------------------------------------
{
  BSTWriteFile(lst, fileName);
  BSys::EditChart(fileName, BSTFILE);
}

//--------------------------------------------------------------------
DefineMethodClass(BAnythingChartMethod, "Chart", "Anything");
void BAnythingChartMethod::Apply(BList* lst, const BText& fileName) const
//--------------------------------------------------------------------
{
  if(lst && lst->Car())
  {
    BSyntaxObject* obj = (BSyntaxObject*)lst->Car();
    BGrammar* g = obj->Grammar();
    BList* aux = lst;
    while(aux && g && (g!=GraAnything())) 
    { 
      obj = (BSyntaxObject*)lst->Car();
      if(g!=obj->Grammar()) { g = GraAnything(); }
      aux = aux->Cdr();
    }
    if(g==GraSet())
    {
      BSetChartMethod_Apply(lst,fileName);
    }
    else if(g==GraSerie())
    {
      BSerieChartMethod_Apply(lst,fileName);
    } 
    else if(g==GraMatrix())
    {
      BMatrixChartMethod_Apply(lst,fileName);
    }
    else
    {
      Error(I2("Can't apply method Chart ",
               "No se puede aplicar el m�todo Chart ")+
		        I2(" over objects of type ", 
               " sobre objetos de tipo ")+g->Name()); 
    }
  }
}


//--------------------------------------------------------------------
// Methods for Set.
//--------------------------------------------------------------------



//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTextCompiledFiles);

DefExtOpr(1, BTextCompiledFiles, "CompiledFiles", 1, 1, "Text",
    I2("(Text fileName)",
       "(Text nombreFichero)"),
    I2("Writes a table with the compiled files.",
       "Escribe una tabla con los ficheros compilados."),
    BOperClassify::General_);

//--------------------------------------------------------------------
void BTextCompiledFiles::CalcContens()
//--------------------------------------------------------------------
{
    BText fileName = Text(Arg(1));
    contens_ = fileName;
    ofstream out;
    if(fileName!="Std") { out.open(fileName.String()); }
    WriteFileOrStd(fileName,out,(I2("Name~   Path~   Elements~",
            "Nombre~ Camino~ Elementos~")+"\n"));
    
    for(BInt n=1; n<=BSetFromFile::NSetFromFile(); n++)
    {
  BSetFromFile* file = (BSetFromFile*)BSetFromFile::FindCompiled(n);
  BSet& set = file->Contens();
  WriteFileOrStd
      (
    fileName,out,
    (
        file->Identify() + "~" +
        file->TolPath()      + "~" +
        set.Card()   + "~" + "\n"
        )
    );
    }
    
//    if(fileName!="Std") { BSys::EditTable(fileName, BSTFILE); }
    if(fileName!="Std") { out.close(); }
}


//--------------------------------------------------------------------
BDate TsrFirstDate(BSyntaxObject* obj)
//--------------------------------------------------------------------
{
    return(Tsr(obj)->FirstDate());
}


//--------------------------------------------------------------------
BDate TsrLastDate(BSyntaxObject* obj)
//--------------------------------------------------------------------
{
    return(Tsr(obj)->LastDate());
}

BText BPackage::help_ = I2(
  "Read information about TOL packages on ",
  "Lea información acerca de los paquetes TOL en ")+
  "https://www.tol-project.org/wiki/TolPackageRulesAndComments\n";


BText BPackage::localRoot_ = "";

//--------------------------------------------------------------------
  BText BPackage::LocalPath(const BText& package_version)
//--------------------------------------------------------------------
{
  if(!localRoot_.HasName())
  {
    localRoot_ = EvalText("TolPackage::Client::_.localRoot","");
    if(!localRoot_.HasName()) 
    { 
      localRoot_ = BSys::TolAppData()+"TolPackage/Client/"; 
    }
  }
  BText path = localRoot_+package_version+"/"+package_version+".oza";
  return(path);
}

//--------------------------------------------------------------------
  bool BPackage::Install(const BText& package_version)
//--------------------------------------------------------------------
{
  bool oldRunningUseModule = BOis::SetRunningUseModule(false);
  BDat ri = EvalReal(BText("Real TolPackage::Client::RemoteInstall("
    "\"\",\"")+package_version+"\",True);",0);
  return(ri.Value()!=0.0);
}

//--------------------------------------------------------------------
static void PkgStartActions(
  BSyntaxObject* pkg,
  const BText& package, 
  const BText path)
//--------------------------------------------------------------------
{
  BNameBlock& nb = NameBlock(pkg);
  if(!nb.startedPackage)
  {
    nb.startedPackage = true;
    BText startActionsExpr = package+"::StartActions(0)";
    bool oldRunningUseModule = BOis::SetRunningUseModule(false);
    BText oldDir = BDir::GetCurrent();
    BDir::ChangeCurrent( GetFilePath(path) );
    EvalReal(startActionsExpr,0);
    BDir::ChangeCurrent(oldDir);
    BOis::SetRunningUseModule(oldRunningUseModule);
  }
}

//--------------------------------------------------------------------
  BText LocalLastCompatible(const BText& package)
//--------------------------------------------------------------------
{
  BText package_version = EvalText(BText(
    "Text TolPackage::Client::LocalLastCompatible("
    "\"")+package+"\");", "");
  if(package_version=="") { package_version = package; }
  return(package_version);
};

//--------------------------------------------------------------------
  BSyntaxObject* BPackage::Load(
    const BText& package_identifier, 
    bool retry)
//--------------------------------------------------------------------
{
  BText package_version = package_identifier;
  BText package;
//Std(BText("\nTRACE BPackage::Load 1 package_version='")+package_version+"'");
  int point = package_version.Find(".",1);
  int point2 = package_version.Find(".",point+1);
  if(point>=0 && point2<0)
  {
    package_version += ".";
    point2 = package_version.Find(".",point+1);
  }
  bool hasHighVersion = (point>0); 
  bool hasLowVersion = hasHighVersion && (point2<package_version.Length()-2); 
  if(hasLowVersion)
  {
    package = package_version.SubString(0,point-1);
  }
  else if(hasHighVersion)
  {
    package = package_version.SubString(0,point-1);
  }
  else
  {
    package = package_version;
  }
  BSyntaxObject* pkg = GraNameBlock()->FindOperand(package,false);
  bool ok = pkg!=NULL;
  BText help = ""; 
  BText load = "";
  if(ok) { 
    BText sourcePath = GetFilePath(pkg->SourcePath());
    PkgStartActions(pkg, package, sourcePath);
  }
  else
  {
    load = I2("Loaded","Ha sido cargado");
    if(hasHighVersion)
    {
      package_version = LocalLastCompatible(package_version);
      point2 = package_version.Find(".",point+1);
      hasLowVersion = (point2<=package_version.Length()-2); 
    }
    else
    {
      package_version = LocalLastCompatible(package);
    }

    BText path = LocalPath(package_version);
  //Std(BText("\nTRACE BPackage::Load 3 path='")+path+"'");
    BDir dirPath(":::");
    if(path.HasName()) { dirPath = path; }

  //Std(BText("\nTRACE BPackage::Load 3 dirPath='")+dirPath.Name()+"'");
    if(!ok)
    {
      ok=dirPath.Exist()!=0;
      if(!ok & path.HasName())
      {
        BDir zipPath = path+".zip";
        if(zipPath.Exist())
        {
          StoreZipArchive zip;
          zip.Open(zipPath.Name(),'r');
          zip.DirExtract("*",dirPath.Name());
          zip.Close();
          dirPath = path;  
        }
        ok = dirPath.Exist();
      }
    //Std(BText("\nTRACE BPackage::Load 4 ok=")+ok);
      if(!ok && retry) 
      { 
        ok = Install(package_version); 
        package_version = LocalLastCompatible(package_version);
      //Std(BText("\nTRACE BPackage::Load 5 ok=")+ok);
        if(ok) { return(BPackage::Load(package_version, false)); }
      }
      if(ok)
      {
      //Std(BText("\nTRACE BPackage::Load 6 "));
        BGrammar::IncLevel();
        BSyntaxObject* aux = BOisLoader::LoadFull(path);
        if(aux && (aux->Grammar()==GraSet()))
        { 
        //Std(BText("\nTRACE BPackage::Load 7 "));
          BSet& set = Set(aux);
          if((set.Card()==1)&&(set[1]->Grammar()==GraNameBlock()))
          {
          //Std(BText("\nTRACE BPackage::Load 8 "));
            pkg = set[1];
            int oldLevel = BGrammar::Level();
            pkg->PutLevel(0);
            BGrammar::PutLevel(0);
            BGrammar::AddObject(pkg);
            BGrammar::PutLevel(oldLevel);
            pkg->IncNRefs();
            pkg->IncNRefs();
            required_ = Cons(pkg, required_);
            BNameBlock::AddGlobalRequiredPackage(package);
            PkgStartActions(pkg, package, path);
          }
        }      
      //Std(BText("\nTRACE BPackage::Load 9 "));
        BGrammar::DecLevel(); 
        ok = pkg!=NULL;
      //Std(BText("\nTRACE BPackage::Load 10 ok=")+ok);
      } 
    }
  //Std(BText("\nTRACE BPackage::Load 11 "));
    if(!ok)
    {
    //Std(BText("\nTRACE BPackage::Load 12 dirPath='")+dirPath.Name()+"'");
      if(path.HasName()) { dirPath = path; }
      if(!dirPath.Exist())
      {
        Error(I2("Unknown package ",
                 "El paquete desconocido ")+package_identifier+
              I2(" must be manually installed.",
                 " debe ser instalado manualmente."));
      }
      else
      {
        BSys::Remove(path);
        if(retry)
        {
          Error(I2("Corrupted package ",
                   "El paquete corrupto ")+package_version+
                I2(" has been locally removed ",
                   " ha sido eliminado localmente ")+
                I2("and will be reinstalled.",
                   "y será reinstalado."));
          return(Load(package_version, false));
        } 
        else
        {
          Error(I2("Corrupted package ",
                   "El paquete corrupto ")+package_version+
                I2(" has been locally removed ",
                   " ha sido eliminado localmente ")+
                I2("and it will not be reinstalled again.",
                   "y no será reinstalado de nuevo."));
        }  
      } 
      load = I2("NOT loaded","No ha sido cargado");
      help = help_;
    }
    Std(load+I2(" package ", " el paquete ")+package_version+"\n"+help);
  }
  if(pkg)
  {
    BUserNameBlock* nbBuilding = BNameBlock::Building();
    if(nbBuilding)
    {
      nbBuilding->Contens().AddRequiredPackage(package);
    }
  }
  return(pkg);
}

