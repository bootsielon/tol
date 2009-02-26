/* oisloader.cpp: main and init functions of GNU/TOL language.

   Copyright (C) 2005-2007, Bayes Decision, SL (Spain [EU])

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

//#define TRACE_LEVEL 5

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_oisloader.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bscanner.h>
#include <tol/tol_bparser.h>
#include <tol/tol_btmsgrav.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bvmatgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bnameblock.h>
#include <tol/tol_bstruct.h>

BTraceInit("oisloader.cpp");


//#define TRACE_OIS_HIERARCHY
#ifdef TRACE_OIS_HIERARCHY
  static FILE* logHrchyRead_ = 
  fopen((BSys::TolAppData()+"syslog/OisHierarchyLogRead.log").String,"w");
#endif

//--------------------------------------------------------------------
// BOisLoader::BOffsetObject Functions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  BSyntaxObject* BOisLoader::BOffsetObject::PutObject(BSyntaxObject* obj)
//--------------------------------------------------------------------
{
//Std(BText("\nBOisLoader::BOffsetObject::PutObject ")+(int)offset_+",["+(int)obj+"] "+obj->Identify());
  if(offset_ && obj)
  {
    if(obj->OisOffset() && (obj->OisOffset()!=offset_))
    {
      ::Warning(I2("OIS Object already has assigned offset",
                   "El objecto OIS ya tiene offset asignado")+
                ": "+obj->Identify());
      return(obj);
    } 
    object_ = obj;
    obj->PutOisOffset(offset_);
    obj->IncNRefs();
    return(obj); 
  }
  else
  {
    if(!offset_) 
    { 
      ::Error(I2("OIS cannot assign NULL offset",
                 "OIS no puede asignar un offset NULO")+
              ": "+obj->Identify()); 
    }
    if(!obj    ) 
    { 
      ::Error(I2("OIS cannot assign offset to NULL object",
                 "OIS no puede asignar un offset al objeto NULO")+
              ": "+obj->Identify()); 
    }
    return(NULL);
  }
}

//--------------------------------------------------------------------
// BOisLoader Functions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  BOisLoader::BOisLoader() 
//--------------------------------------------------------------------
: BOis(),
  data_(NULL),
  curHrchyPartialPath_(""),
  checkSourceExists_(false),
  checkSourceSize_(false),
  checkSourceDate_(false),
  obsoleteSource_(false),
  showCheckingTraces_(false),
  oisHasPriorityOnConflict_(true),
  lostSource_(false),
  tolSources_()
{  
}


//--------------------------------------------------------------------
  BOisLoader::~BOisLoader() 
//--------------------------------------------------------------------
{  
}


#define EnsureFileOpenR(fn,T,N)                     \
if(! ( fn = streamHandler_->Open(T,SubPath()+N) ) ) \
{                                                   \
  return(false);                                    \
}

//--------------------------------------------------------------------
  bool BOisLoader::BinError(const char* fileName)
//--------------------------------------------------------------------
{
  return(Error(I2("Corrupted binary file: ",
                  "Fichero binario corrupto: ")+
         fileName));  
}


//--------------------------------------------------------------------
  bool BOisLoader::Open(bool errorWarning)
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("BOisLoader::Open()");
#endif
  TRACE_SHOW_LOW(fun,"BEGIN");
  streamHandler_ = BStreamHandler::GetConnect
  (
    connection_, 
    BStreamHandler::BSHOM_READ, 
    errorWarning
  );
  TRACE_SHOW_LOW(fun,"1");
  if(streamHandler_)
  {
    TRACE_SHOW_LOW(fun,"2.1");
    EnsureFileOpenR(header_,  "header",  "header.xml");
    header_->SetPos(header_->Bytes()-10);
    BText foundEnd="NO_FOUND_";
    header_->Read(foundEnd.Buffer(), 1, 9);
    foundEnd.PutLength(9);
    TRACE_SHOW_LOW(fun,"2.2");
    BText expectedEnd = "</header>";
    if(foundEnd!=expectedEnd) 
    { 
      Error(I2("Corrupted XML file: ",
               "Fichero XML corrupto: ")+
            header_->Name()+
            I2("\nThis file should ends with '",
               "\nEl fichero debería acabar con '")+
            expectedEnd+"'"+
            I2(" instead of with '",
               " en lugar de con '")+
            foundEnd+"'");
      Ensure(false); 
    }
    header_->SetPos(0);
    EnsureFileOpenR(tolref_,  "tolref",  ".tolref"   );
    EnsureFileOpenR(oisref_,  "oisref",  ".oisref"   );
    EnsureFileOpenR(object_,  "object",  ".object"   );
    EnsureFileOpenR(set_,     "set",     ".set"      );
    EnsureFileOpenR(serie_,   "serie",   ".serie"    );
    EnsureFileOpenR(timeset_, "timeset", ".timeset"  );
    EnsureFileOpenR(matrix_,  "matrix",  ".matrix"   );
    EnsureFileOpenR(polyn_,   "polyn",   ".polyn"    );
    EnsureFileOpenR(ratio_,   "ratio",   ".ratio"    );
    EnsureFileOpenR(code_,    "code",    ".code"     );
    if(streamHandler_->HasFile(SubPath()+".hrchyDetail"))  
    {
      EnsureFileOpenR(hrchyDetail_, "hrchyDetail", ".hrchyDetail"    );
      TRACE_SHOW_LOW(fun,"2.15");
      EnsureFileOpenR(hrchyOffset_, "hrchyOffset", ".hrchyOffset"    );
      TRACE_SHOW_LOW(fun,"2.16");
      EnsureFileOpenR(hrchyOrder_,  "hrchyOrder",  ".hrchyOrder"     );
      TRACE_SHOW_LOW(fun,"2.17");
    }
    if(streamHandler_->HasFile(SubPath()+"export.csv"))
    {
      export_ = streamHandler_->Open("export",SubPath()+"export.csv");
    }
  }
  TRACE_SHOW_LOW(fun,"3");
  SetAllFiles();
  TRACE_SHOW_LOW(fun,"4");
  return((streamHandler_!=NULL)&&(streamHandler_->Connected()));
}

//--------------------------------------------------------------------
  bool BOisLoader::CheckFileSizes()
//--------------------------------------------------------------------
{
  int n;
  int numFiles = allFiles_.Size();
  bool ok = true;
  BText aux;
  BText address = SubPath();
  enable_BSE_=
    (control_.oisEngine_.oisVersion_>="01.04") &&
    (control_.oisEngine_.oisVersion_< "01.08");
  ERead(aux, tolref_ ); if(aux!=address+"/.tolref>\n" ) { return(BinError(".tolref"  )); }
  ERead(aux, oisref_ ); if(aux!=address+"/.oisref>\n" ) { return(BinError(".oisref"  )); }
  ERead(aux, object_ ); if(aux!=address+"/.object>\n" ) { return(BinError(".object"  )); }
  ERead(aux, set_    ); if(aux!=address+"/.set>\n"    ) { return(BinError(".set"     )); }
  ERead(aux, serie_  ); if(aux!=address+"/.serie>\n"  ) { return(BinError(".serie"   )); }
  ERead(aux, timeset_); if(aux!=address+"/.timeset>\n") { return(BinError(".timeset" )); }
  ERead(aux, matrix_ ); if(aux!=address+"/.matrix>\n" ) { return(BinError(".matrix"  )); }
  ERead(aux, polyn_  ); if(aux!=address+"/.polyn>\n"  ) { return(BinError(".polyn"   )); }
  ERead(aux, ratio_  ); if(aux!=address+"/.ratio>\n"  ) { return(BinError(".ratio"   )); }
  ERead(aux, code_   ); if(aux!=address+"/.code>\n"   ) { return(BinError(".code"    )); }
  if(control_.oisEngine_.oisVersion_>="01.07") {
  ERead(aux, hrchyDetail_); if(aux!=address+"/.hrchyDetail>\n") { return(BinError(".hrchyDetail" )); }
  ERead(aux, hrchyOffset_); if(aux!=address+"/.hrchyOffset>\n") { return(BinError(".hrchyOffset" )); }
  ERead(aux, hrchyOrder_ ); if(aux!=address+"/.hrchyOrder>\n" ) { return(BinError(".hrchyOrder"  )); } }
  else { numFiles-=3; }
  enable_BSE_=(control_.oisEngine_.oisVersion_>="01.08")!=0;
  for(n=1; n<numFiles; n++)
  {
    if(allFiles_[n])
    {
      BINT64 size = allFiles_[n]->Bytes();
      if(stat_.fileStat_[n].bytes_!=size)
      {
        Error(I2("OIS: File size integrity check failed",
                 "OIS: Fallo en el test de integridad de tamaño de ficheros")+
                 ".\n  "+allFiles_[n]->Name()+
              I2(" seems to be corrupted due to has ",
                 " parece estar corrupto pues ocupa ")+
              size + " Kb "+ 
              I2(" instead of "," en lugar de ")+
              stat_.fileStat_[n].bytes_);
        ok = false; 
      }
    }
  }
  Ensure(ok);
  return(ok);
}


//--------------------------------------------------------------------
  bool BOisLoader::InitReaded() 
//--------------------------------------------------------------------
{
  int n;
  readed_.AllocBuffer(stat_.fileStat_[BFI_oisref].entries_);
  BINT64 offset;
  for(n=0; n<readed_.Size(); n++)
  {     
    ERead(offset, oisref_);
    readed_[n].PutOffset(offset);
    readed_[n].PutNullObject();
  }
  readed_.Sort(CompareOffset);
//for(n=0; n<readed_.Size(); n++) { printf("\n%"_LLD64_, readed_[n].OisOffset()); }
  return(true);
}


//--------------------------------------------------------------------
bool BOisLoader::Read(BText& v, BStream* stream)
//--------------------------------------------------------------------
{
  static char aux_[65536];
  int sz;
  ERead(sz,stream);
  Ensure(Read(aux_,1,sz, stream));
  aux_[sz]=0; 
  v.Copy(aux_);
  return(true);
};

//--------------------------------------------------------------------
bool BOisLoader::Read(BDat& v, BStream* stream)
//--------------------------------------------------------------------
{
  if(control_.typeSizes_.sizeof_BDat_==sizeof(BDatOld))
  {
    char   k;
    double x;
    ERead(k, stream);
    ERead(x, stream);
    if(k) { v = x; }
    else  { v = BDat::Unknown(); }
    return(true);
  }
  else if(control_.typeSizes_.sizeof_BDat_==sizeof(BDat))
  {
    double x;
    ERead(x, stream);
    v.PutValue(x);
    return(true);
  }
  else
  {
    assert(1); 
    return(false);
  }
};

//--------------------------------------------------------------------
bool BOisLoader::Read(BComplex& v, BStream* stream)
//--------------------------------------------------------------------
{
  BDat x, y;
  ERead(x, stream);
  ERead(y, stream);
  v = BComplex(x,y);
  return(true);
};

//--------------------------------------------------------------------
bool BOisLoader::Read(BDate& v, BStream* stream)
//--------------------------------------------------------------------
{
  double h;
  ERead(h,stream);
  v.PutHash(h);
  return(true);
};

//--------------------------------------------------------------------
  bool BOisLoader::Read(BDat* buf, int s, BStream* stream)
//--------------------------------------------------------------------
{
  if(control_.typeSizes_.sizeof_BDat_==sizeof(BDatOld))
  {
    BArray<BDatOld> d_(s);
    if(!Read(d_.GetBuffer(),sizeof(BDatOld),s,stream))
    { 
      return(false); 
    }
    for(int i=0; i<s; i++)
    {
      if(d_(i).known_) { buf[i].PutValue(d_(i).value_); }
      else             { buf[i] = BDat::Unknown(); }
    }
    return(true);
  }
  else if(control_.typeSizes_.sizeof_BDat_==sizeof(BDat))
  {
    return(Read(buf,sizeof(BDat),s,stream));
  }
  else
  {
    assert(1); 
    return(false);
  }
};


//--------------------------------------------------------------------
  bool BOisLoader::Read(BPol& x, BStream* stream)
//--------------------------------------------------------------------
{
  int s, i;
  bool ok = true;
  ERead(s, stream);
  x.AllocBuffer(s);

  if(control_.typeSizes_.sizeof_BCoefDeg_==sizeof(BCoefDegOld))
  {
    BCoefDegOld* buf = new BCoefDegOld[s];
    Ensure(Read(buf,sizeof(BCoefDegOld),s,stream));
    for(i=0; i<x.Size(); i++)
    {
      x[i].PutDegree(buf[i].degree_);
      x[i].PutCoef  (buf[i].coefficient_.value_);
    }
    delete [] buf;
  }
  else if(control_.typeSizes_.sizeof_BCoefDeg_==sizeof(BCoefDeg))
  {
    BCoefDeg* buf = new BCoefDeg[s];
    Ensure(Read(buf,sizeof(BCoefDeg),s,stream));
    for(i=0; i<x.Size(); i++)
    {
      x[i].PutDegree(buf[i].degree_);
      x[i].PutCoef  (buf[i].coefficient_);
    }
    delete [] buf;
  }
  else
  {
    assert(1); 
    return(false);
  }
  Ensure(ok);
  return(ok);
}


//--------------------------------------------------------------------
  bool BOisLoader::Read(BMemberOwner& owner,
                        BMemberOwner::BClassByNameHash& x, 
                        BStream* stream)
//--------------------------------------------------------------------
{
  BMemberOwner::BClassByNameHash::const_iterator iterC;
  int n = 0;
  int s;
  ERead(s,stream);
  for(n=0; n<s; n++)
  {
    BText parentName;
    ERead(parentName,stream);
    BClass* parent = FindClass(parentName);
    if(!parent)
    {
      return(NullError(
        I2("Cannot load from OIS Class ","No se puede cargar del OIS Class ")+ 
        owner.getName()+
        I2("due to ascent Class ","Debido a que su ascendiente Class ")+
        parentName+I2(" doesn't exist"," no existe")));
    }
    x[parent->Name()] = parent;
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisLoader::Read(BClass& cls, BStream* stream)
//--------------------------------------------------------------------
{
  BMemberOwner::BClassByNameHash& parents = *cls.parentHash_;
  Ensure(Read(cls,parents,stream));
  BMemberOwner::BClassByNameHash::const_iterator iterC;
  int n = 0;
  for(iterC=parents.begin(); iterC!=parents.end(); iterC++, n++)
  {
    iterC->second->IncNRefs();
  }
  Ensure(Read(cls,*cls.ascentHash_,stream));
  int s;
  ERead(s,stream);
  cls.member_.AllocBuffer(s);
  for(n=0; n<s; n++)
  {
    BText parentName;
    BText name;
    ERead(parentName, stream);
    ERead(name      , stream);
    BMember* mbr = NULL;
    if(parentName==cls.getName())
    {
      char isMethod;
      mbr = new BMember;
      mbr->parent_ = &cls;
      mbr->name_  = name;
      mbr->branch_ = ReadTree(stream);
      ERead(mbr->declaration_ , stream);
      ERead(mbr->definition_  , stream);
      ERead(isMethod          , stream);
      mbr->isMethod_ = isMethod!=0;
      mbr->isGood_ = true;
    }
    else
    {
      BClass* parent = FindClass(parentName);
      mbr = parent->FindMember(name);
      assert(mbr);
    }
    BMbrNum* mbrNum = new BMbrNum;
    mbrNum->member_ = mbr;
    mbrNum->position_ = n;
    cls.member_[n] = mbrNum;
    (*cls.memberHash_)[mbr->name_] = mbrNum;
    if(mbr->definition_.HasName())
    {
      (*cls.mbrDefHash_)[mbr->declaration_] = mbr;
    }
    else
    {
      (*cls.mbrDecHash_)[mbr->declaration_] = mbr;
    }
  }
  return(true);
}


//--------------------------------------------------------------------
  BToken* BOisLoader::ReadTokenFromFile(BStream* stream)
//--------------------------------------------------------------------
{
  BText name;
  char  tt;
  short pr;
  char  cl;
  ERead(name, stream);
  ERead(tt, stream);
  ERead(pr, stream);
  ERead(cl, stream);
  BToken* token = new BToken(name,(BTokenType)tt,pr);
  token->PutClose(GetTokenCloseFromId(cl));
#ifdef TRACE_OIS_TOKEN
  BText aux0 = Compact(name);
  BText aux1 = Replace(aux0,'\"','|');
  BText aux2 = Replace(aux1,'\'','|');
  fprintf(tokRead_,"\n%ld\t%ld\t%ld\t%s",(int)tt,(int)pr,(int)cl,aux2.String());
  fflush(tokRead_);
#endif
  return(token);
};


//--------------------------------------------------------------------
  List* BOisLoader::ReadTreeFromFile(BStream* stream)
//--------------------------------------------------------------------
{
  char cl; 
  ERead(cl,stream);
  if(!cl)
  {
    return(NULL);
  }
  else
  {
    List* tre = new List;
    if(cl==1)
    {
      BToken* tok = ReadTokenFromFile(stream);
      tre->setCar(tok);
    }
    else
    {
      tre->setCar(ReadTreeFromFile(stream));
    }
    tre->setCdr(ReadTreeFromFile(stream));
    return(tre);
  }
};


//--------------------------------------------------------------------
  BToken* BOisLoader::ReadTokenFromStream(BArray<char>& streamBuf, unsigned int& pos)
//--------------------------------------------------------------------
{
  unsigned short   len;
  char  tt;
  short pr;
  char  cl;

  assert(pos+sizeof(unsigned short)<=unsigned(streamBuf.Size()));
  memcpy(&len,streamBuf.Buffer()+pos,sizeof(unsigned short));
  pos+=sizeof(unsigned short);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&len,sizeof(unsigned short));
  }

  BText name(len+1);
  assert(pos+len<=unsigned(streamBuf.Size()));
  memcpy(name.Buffer(),streamBuf.Buffer()+pos,len);
  pos+=len;

  assert(pos+1<=unsigned(streamBuf.Size()));
  tt = streamBuf[pos];
  pos+=sizeof(char);

  assert(pos+sizeof(short)<=unsigned(streamBuf.Size()));
  memcpy(&pr,streamBuf.Buffer()+pos,sizeof(short));
  pos+=sizeof(short);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&pr,sizeof(short));
  }

  assert(pos+1<=unsigned(streamBuf.Size()));
  cl = streamBuf[pos];
  pos+=sizeof(char);

  BToken* token = new BToken(name,(BTokenType)tt,pr);
  token->PutClose(GetTokenCloseFromId(cl));
#ifdef TRACE_OIS_TOKEN
  BText aux0 = Compact(name);
  BText aux1 = Replace(aux0,'\"','|');
  BText aux2 = Replace(aux1,'\'','|');
  fprintf(tokRead_,"\n%ld\t%ld\t%ld\t%s",(int)tt,(int)pr,(int)cl,aux2.String());
  fflush(tokRead_);
#endif
  return(token);
};


//--------------------------------------------------------------------
  List* BOisLoader::ReadTreeFromStream(BArray<char>& streamBuf, unsigned int& pos)
//--------------------------------------------------------------------
{
  assert(pos+1<=unsigned(streamBuf.Size()));
  char cl = streamBuf[pos];
  pos+=sizeof(char);
  if(!cl)
  {
    return(NULL);
  }
  else
  {
    List* tre = new List;
    if(cl==1)
    {
      BToken* tok = ReadTokenFromStream(streamBuf,pos);
      tre->setCar(tok);
    }
    else
    {
      tre->setCar(ReadTreeFromStream(streamBuf,pos));
    }
    tre->setCdr(ReadTreeFromStream(streamBuf,pos));
    return(tre);
  }
};


//--------------------------------------------------------------------
  List* BOisLoader::ReadTree(BStream* stream)
//--------------------------------------------------------------------
{
  if(control_.oisEngine_.oisVersion_<"01.04")
  {
    return ReadTreeFromFile(stream);
  }
  else
  {
    unsigned int pos;
    ERead(pos,stream);
    BArray<char> streamBuf(pos);
    Read(streamBuf.GetBuffer(),1,pos,stream);
    pos=0;
    return(ReadTreeFromStream(streamBuf, pos));
  }
};


//--------------------------------------------------------------------
  BSyntaxObject* BOisLoader::PutVariableName(BSyntaxObject* result, 
                                             const BText& name,
                                             char is_referenceable)
//--------------------------------------------------------------------
{
  BSyntaxObject* obj = result;
  result->PutName(name);
  if(is_referenceable) 
  { 
    obj = result->Grammar()->FindOperand(name,false);
    if(obj)
    {
      #if !defined NDEBUG && defined WARN_ALREADY_EXISTS
      Warning(I2("OIS referenceable object "+result->LexInfo()+
                 " already exists in current TOL session "
                 "and cannot be exported.",
                 "El objeto OIS referenciable "+result->LexInfo()+
                 " ya existe en la sesión actual de TOL "+
                 "y no puede ser exportado."));
      #endif
      if(!oisHasPriorityOnConflict_)
      {
        SAFE_DESTROY(result, obj);
      }
      else
      {
        obj = result;
      }
    }
    else
    {
      BGrammar::AddObject(result); 
      obj = result;
    }
  }
  return(obj);
}

//--------------------------------------------------------------------
  BSyntaxObject* BOisLoader::ReadNextObject()
//--------------------------------------------------------------------
{
  static BOffsetObject ofob;
  ofob.PutOffset(object_->Offset());
  int found = readed_.FindSorted(ofob, CompareOffset);
  if(found<0)
  {
    return(NullError("FATAL BOisLoader::ReadNextObject misplaced offset"));
  }
  if(readed_[found].Object()) 
  { return(readed_[found].Object()); }
  unsigned int n, s;
  BText name, description, aux;
  char system, mode, tol_type, is_referenceable, format, gid;
  BSyntaxObject* result = NULL;

  ERead(system,           object_);
  ERead(is_referenceable, object_);
  ERead(mode,             object_);
  ERead(tol_type,         object_);
  ERead(name,             object_);
  BGrammar* gra = BGrammar::FindByGid((BGrammarId)tol_type,true);
  if(system) 
  {
    if(mode==BOBJECTMODE) 
    { 
      return(gra->FindOperand(name,false)); 
    }
    else if((mode==BBUILTINFUNMODE)||(mode==BUSERFUNMODE))
    { 
      return(gra->FindOperator(name)); 
    }
    else if(mode==BSTRUCTMODE) 
    { 
      return(FindStruct(name)); 
    }
    else if(mode==BCLASSMODE) 
    { 
      return(FindClass(name)); 
    }
    else 
    { 
      return(NullError("FATAL in BOisLoader::ReadNextObject()"));
    }
  }
  else
  {
    if(mode==BSTRUCTMODE) 
    { 
      ERead(s, object_);
      BStruct* str = NULL;
      BStruct* fieldStr=NULL; 
      if(is_referenceable) 
      { 
        str = FindStruct(name);
        if(str)
        {
          #if !defined NDEBUG && defined WARN_ALREADY_EXISTS
          Warning(I2("OIS referenceable Struct "+str->LexInfo()+
                     " already exists in current TOL session "
                     "and cannot be exported.",
                     "El Struct OIS referenciable "+str->LexInfo()+
                     " ya existe en la sesión actual de TOL "+
                     "y no puede ser exportado."));
          #endif

        }
      }
      BArray<BField> field(s);
      BText strFieldName; 
      for(n=0; n<s; n++)
      {
        ERead(aux, object_);
        ERead(gid, object_);
        BGrammar* graGid = BGrammar::FindByGid((BGrammarId)gid);
        if(graGid==GraSet())
        {
          ERead(strFieldName, object_);
          if(strFieldName.HasName()) { fieldStr=FindStruct(strFieldName); }
        }  
        field[n].PutGrammar(graGid);
        field[n].PutName(aux);
        field[n].PutStruct(fieldStr);
      } 
      if(!str)
      {
        str = new BStruct(name, false);
        str->PutField(field);
        BGrammar::AddObject(str); 
        str->PutFunction(new BNewStruct(*str));
      }
      return(readed_[found].PutObject(str));
    }
    else if(mode==BCLASSMODE) 
    { 
      BClass* cls = new BClass;
      cls->PutName(name);
      BScanner::AddSymbol(new BTypeToken(name,BTypeToken::BCLASS));
      Ensure(Read(*cls,object_));
      BGrammar::AddObject(cls); 
      return(readed_[found].PutObject(cls));
    }
    else
    {
      ERead(description, object_);
      ERead(format,      object_);
      if(mode==BUSERFUNMODE)
      { 
        BINT64 offset;
        ERead(offset, object_);
        code_->SetPos(offset);
        BUserFunction* usf = new BUserFunction("",gra);
        usf->PutName(name);
        List* dec = ReadTree(code_);
        List* def = ReadTree(code_);
        if(control_.oisEngine_.oisVersion_<"01.04")
        { 
          BText decTxt_, defTxt_;
          ERead(decTxt_,code_);
          ERead(defTxt_,code_);
          decTxt_.Compact();
          defTxt_.Compact();
          BText decTxt = Compact(BParser::Unparse(dec,"  ","\n"));
          BText defTxt = Compact(BParser::Unparse(def,"  ","\n"));
          if(decTxt!=decTxt_)
          {
            return(NullError(BText("FATAL BOisLoader::ReadNextObject: incongruent tree and unparsed declaration of function ")+
                             name +"\n" + 
                             "///////////////////////////////////////////////////////////////////////////////\n"+
                             decTxt+"\n" + 
                             "///////////////////////////////////////////////////////////////////////////////\n"+
                             " != \n" 
                             "///////////////////////////////////////////////////////////////////////////////\n"+
                             decTxt_+"\n" + 
                             "///////////////////////////////////////////////////////////////////////////////\n"));
          }
          if(decTxt!=decTxt_)
          {
            return(NullError(BText("FATAL BOisLoader::ReadNextObject: incongruent tree and unparsed definition of function ")+
                             name +"\n" + 
                             "///////////////////////////////////////////////////////////////////////////////\n"+
                             defTxt+"\n" + 
                             "///////////////////////////////////////////////////////////////////////////////\n"+
                             " != \n" 
                             "///////////////////////////////////////////////////////////////////////////////\n"+
                             defTxt_+"\n" + 
                             "///////////////////////////////////////////////////////////////////////////////\n"));
          }
        }
        if(!(usf->SetExpression(dec,def)))
        {
          return(NullError(Out()+I2("Function ","La función ")+name+
                           I2(" could not been created", " no se pudo crear")));
        } 
        else
        {
          result=usf->GetCode();
          result->PutName(name);
          if(is_referenceable) 
          { 
            BOperator* ope = GraAnything()->FindOperator(name);
            if(ope)
            {
              #if !defined NDEBUG && defined WARN_ALREADY_EXISTS
              Warning(I2("OIS referenceable function "+ope->LexInfo()+
                         " already exists in current TOL session "
                         "and cannot be exported.",
                         "La función OIS referenciable "+ope->LexInfo()+
                         " ya existe en la sesión actual de TOL "+
                         "y no puede ser exportado."));
              #endif
            }
            else
            {
              BGrammar::AddObject(result);
              BGrammar::AddObject(usf);
            }
          }
        }
      } 
      else if(mode==BOBJECTMODE) 
      { 
        if(tol_type == BGI_Real) 
        { 
          BDat x;
          ERead(x, object_); 
          result = new BContensDat("", x, description);
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_Date) 
        { 
          BDate x;
          ERead(x, object_); 
          result = new BContensDate("", x, description);
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_Text) 
        { 
          BText x;
          ERead(x, object_); 
          result = new BContensText("", x, description);
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_Complex) 
        { 
          BComplex x;
          ERead(x, object_); 
          result = new BContensCmp("", x, description);
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_Code)
        {
          char  oprType;
          BText oprName;
          ERead(oprType, object_);
          ERead(oprName, object_);
          BGrammar*  oprGra = BGrammar::FindByGid((BGrammarId)oprType,true);
          if(!oprGra)
          {
            return(NullError(BText("FATAL BOisLoader::ReadNextObject: cannot find grammar of object Code ")+name)); 
          }
          BStandardOperator* opr = (BStandardOperator*)oprGra->FindOperator(oprName);
          if(!opr)
          {
            return(NullError(BText("FATAL BOisLoader::ReadNextObject: cannot find operator ")+oprGra->Name()+" "+oprName+" of object Code "+name)); 
          }
          BCode code;
          BUserCode* uCode = new BContensCode("", code, description);
          uCode->Contens().PutOperator(opr);
          result = uCode;
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_Matrix) 
        {
          BINT64 offset;
          ERead(offset, object_);
          matrix_->SetPos(offset);
          int r,c;
          ERead(r, matrix_);
          ERead(c, matrix_);
          BMat x(r,c);
          Ensure(Read(x.GetData().GetBuffer(),r*c,matrix_));
          result = new BContensMat("", x, description);
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_VMatrix) 
        {       
          BVMat x;
          result = new BContensVMat("", x, description);
          VMat(result).Read(*this, matrix_);
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_Polyn) 
        {
          BINT64 offset;
          ERead(offset, object_);
          polyn_->SetPos(offset);
          BPol x;
          ERead(x, polyn_);
          result = new BContensPol("", x, description);
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_Ratio) 
        {
          BINT64 offset;
          ERead(offset, object_);
          ratio_->SetPos(offset);
          BPol x, y;
          ERead(x, ratio_);
          ERead(y, ratio_);
          BRat z=x/y; 
          result = new BContensRat("", z, description);
          result=PutVariableName(result,name,is_referenceable);
        }
        else if(tol_type == BGI_Set)
        {
          BINT64 offset;
          ERead(offset, object_);
          set_->SetPos(offset);
          ERead(s, set_);  BSet x; x.PrepareStore(s);
          char sbt; ERead(sbt, set_);
          x.PutSubType(BSet::BSubType(sbt));
          BSetFromFile* sff = NULL;
          const BSourcePath* oldSource = BSourcePath::Current();
          if(control_.oisEngine_.oisVersion_>"02.04")
          {
            char isNameBlock;
            ERead(isNameBlock,set_);
            assert(!isNameBlock);
            if(doc_.category_.BeginWith("Code.") && 
               (x.SubType()>=BSet::MODFile)&&(x.SubType()<=BSet::BMIFile))
            {
              BText tolPath;
              ERead(tolPath, set_);
              sff = new BSetFromFile(tolPath);
              x.PutSourcePath(BSourcePath::Current());
            }
          }
          ERead(offset, set_);
          BStruct* str = NULL;
          if(offset) 
          { 
            object_->SetPos(offset);
            BSyntaxObject* r = ReadNextObject(); 
            if(!r || (r->Mode()!=BSTRUCTMODE)) 
            { 
              return(NullError("FATAL BOisLoader::ReadNextObject: cannot build structure of set")); 
            } 
            str = (BStruct*)r;
          }
          BSyntaxObject* r=NULL;
          for(n=1; n<=s; n++)
          {
            ERead(offset, set_);
            object_->SetPos(offset);
            r = ReadNextObject();
            if(!r) 
            { 
              return(NullError("BOisLoader::ReadNextObject: NULL element of set ")); 
            } 
            if(r) 
            {
              x.AddElement(r); 
            }
          }  
          x.PutStruct (str); 
          if(sff) 
          { 
            sff->PutContens(x); 
            BSourcePath::SetCurrent(oldSource);
            result = GraSet()->New("", sff);
            result->PutDescription(description);
          }
          else
          {
            result = new BContensSet("", x, description);
          }
          result=PutVariableName(result,name,is_referenceable);
        }  
        else if(tol_type == BGI_NameBlock)
        {
          BINT64 offset;
          BText fullName;
          BText localName;
          ERead(offset, object_);
          set_->SetPos(offset);
          ERead(s, set_);  
          BUserNameBlock* unb = new BGraContensP<BNameBlock>(new BNameBlock);
          BNameBlock& x = unb->Contens(); 
          x.Set().PrepareStore(s);
          char sbt; ERead(sbt, set_);
          x.Set().PutSubType(BSet::BSubType(sbt));

          char isNameBlock;
          ERead(isNameBlock,set_);
          assert(isNameBlock);
          ERead(fullName,set_);
          x.PutName(fullName);
          if(control_.oisEngine_.oisVersion_>="02.07")
          {
            ERead(localName,set_);
            x.PutLocalName(localName);
          }
          ERead(offset, set_);
          BStruct* str = NULL;
          if(offset) 
          { 
            object_->SetPos(offset);
            BSyntaxObject* r = ReadNextObject(); 
            if(!r || (r->Mode()!=BSTRUCTMODE)) 
            { 
              return(NullError("FATAL BOisLoader::ReadNextObject: cannot build structure of set")); 
            } 
            str = (BStruct*)r;
          }
          x.Set().PutStruct (str); 
          BSyntaxObject* r=NULL;
          for(n=1; n<=s; n++)
          {
            ERead(offset, set_);
            object_->SetPos(offset);
            r = ReadNextObject();
            if(!r) 
            { 
              return(NullError("BOisLoader::ReadNextObject: NULL element of set ")); 
            } 
            if(r) 
            {
              x.Set().AddElement(r); 
              r->PutNameBlock(&x);
            }
          } 
          x.Set().PutNameBlock(&x);
          x.Build();
          if(control_.oisEngine_.oisVersion_>="02.08")
          {
            char hasClass;
            Ensure(Read(hasClass, object_));
            if(hasClass)
            {
              BText className;
              Ensure(Read(className, object_));
              BClass* cls = FindClass(className);
              if(!cls)
              {
                return(NullError(
                  I2("Cannot load from OIS NameBlock ","No se puede cargar del OIS NameBlock ")+ 
                  name+
                  I2("due to ascent Class ","Debido a que su ascendiente Class ")+
                  className+I2(" doesn't exist"," no existe")));
              }
              cls->IncNRefs();
              x.PutClass(cls);
            }
          }
          result = unb;
          result->PutDescription(description);
          result=PutVariableName(result,name,is_referenceable);
        }  
        else if(tol_type == BGI_Serie) 
        {
          BINT64 offset;
          ERead(offset, object_);
          serie_->SetPos(offset);
          ERead(offset, serie_);
          BUserTimeSerie* x = NULL;  
          if(!offset)
          {
            BDat d;
            ERead(d, serie_);
            x = new BTsrFromReal(d);  
          }
          else
          {
            object_->SetPos(offset);
            BSyntaxObject* r = ReadNextObject();
            if(!r || (r->Grammar()!=GraTimeSet())) 
            { 
              return(NullError("FATAL cannot built dating of serie")); 
            }
            BUserTimeSet* dating = (BUserTimeSet*)r;
            BDate first, last, beginCache, endCache;
            ERead(first, serie_);
            ERead(last,  serie_);
            ERead(beginCache, serie_);
            ERead(endCache,   serie_);
            ERead(s,serie_);
            BData d(s);
            Ensure(Read(d.GetBuffer(),s,serie_));
            char loadTree;
            ERead(loadTree,serie_);
            if(loadTree)
            {
              List* tree = ReadTree(serie_);
              if(!tree) 
              { 
                return(NullError(BText("Cannot load syntax tree of Serie (name='")+
                       name+"' description='"+description+"')")); 
              }
              bool oldEnabled = BOut::Disable();
              x = (BUserTimeSerie*)GraSerie()->EvaluateTree(tree); 
              if(oldEnabled) { BOut::Enable(); }
              if(x) 
              { 
                x->GetDataBuffer() = d;
                x->PutFirstCache(beginCache);
                x->PutOisTree(tree);
                tree->Destroy();
              }
              else 
              {
                BText expr = BParser::Unparse(tree,"  ","\n");
                expr = Compact(expr); 
               #ifndef NDEBUG
                Warning(BText("Cannot rebuild virtual expression of "
                              "non bounded Serie ")+name+"="+expr+
                              "\nOnly cached data will be accessible "
                              "between ["+beginCache+","+endCache+"]"
                              "\nTo avoid this problem save just "
                              "bounded time series "); 
               #endif  
                if(tree) { tree->Destroy(); }
              }
            }
            if(!x)
            {
              x = new BTsrPrimary("",description,dating,beginCache,endCache,d);
              PutVariableName(x,name,is_referenceable);
            }
          }
          if(x->Name()!=name) 
          { 
            result = new BTsrRenamed("", x); 
            result=PutVariableName(result,name,is_referenceable);
          }
          else                
          { result = x; }
        }
        else if(tol_type == BGI_TimeSet) 
        {
          BINT64 offset;
          ERead(offset, object_);
          timeset_->SetPos(offset);
          BDate inf, sup, beginCache, endCache;
          ERead(inf,        timeset_);
          ERead(sup,        timeset_);
          ERead(beginCache, timeset_);
          ERead(endCache,   timeset_);
          ERead(s,          timeset_);
          BHash h(s);
          Ensure(Read(h.GetBuffer(),sizeof(double),h.Size(),timeset_));
          BUserTimeSet* x = NULL;
          char loadTree;
          ERead(loadTree,timeset_);
          if(loadTree)
          {
            List* tree = ReadTree(timeset_);
            if(!tree) 
            { 
              return(NullError(BText("Cannot load syntax tree TimeSet (name='")+
                     name+"' description='"+description+"')")); 
            }
            bool oldEnabled = BOut::Disable();
            x = (BUserTimeSet*)GraTimeSet()->EvaluateTree(tree); 
            if(oldEnabled) { BOut::Enable(); }
            if(x) 
            { 
              assert(s || (tree!=NULL));
              if(s && (control_.oisEngine_.oisVersion_>="02.03"))
              {
                x->PutCache(h, beginCache, endCache);
              }
              x->PutOisTree(tree); 
              tree->Destroy();
            }
            else 
            {
              BText expr = Compact(BParser::Unparse(tree,"  ","\n"));
             #ifndef NDEBUG
              Warning(BText("Cannot rebuild virtual expression of non "
                      "bounded TimeSet ")+name+"="+expr+ "\nOnly cached "
                      "dates will be accessible between ["+beginCache+","+
                      endCache+"]\nTo avoid this problem save just "
                      "bounded time sets " );
             #endif
              if(tree) { tree->Destroy(); }
            } 
          }
          if(!x)
          {
            x = new BTmsDatesOfSet(h);
          }
          if(x->Name()!=name) 
          { 
            result = new BTmsRenamed("", x); 
            result=PutVariableName(result,name,is_referenceable);
          }
          else                
          { result = x; }
        }
      }
      else 
      { 
        return(NullError("FATAL invalid object mode for BOisLoader::ReadNextObject()"));
      }
    }
    if(result && !result->Description().HasName() & description.HasName()) 
    { 
      result->PutDescription(description); 
    }
    return(readed_[found].PutObject(result));
  }
}

//--------------------------------------------------------------------
  bool BOisLoader::Remove ()
//--------------------------------------------------------------------
{
  if(streamHandler_) 
  { 
    Close();
  }
  streamHandler_ = BStreamHandler::GetConnect
  (
    connection_, 
    BStreamHandler::BSHOM_WRITE, 
    true
  );
  bool ok = RemoveImage();
  return(ok);
}

//--------------------------------------------------------------------
  BDate BOisLoader::Time() 
//--------------------------------------------------------------------
{ 
  if(!streamHandler_) { Open(false); }
  if(!header_) { return(BDate::Unknown()); } 
  else         { return(BTimer::TimeToDate(header_->Time())); } 
}

//--------------------------------------------------------------------
  bool BOisLoader::CheckUpdatedSource(int n, const BText& path)
//--------------------------------------------------------------------
{
  bool isOza = (options_.compressor_.fileManager_.engine_==BAE_ZIPARC_);
  BDir dir(path);
  lostSource_=!dir.Exist();
  bool updated; 
  if(lostSource_) 
  {
    updated = true; 
  }
  else
  {
    BText name = BText("._tol_source_/")+PlainPath(path);
    BStream* fn = isOza?streamHandler_->Open(name,name):NULL;
    source_.Add(fn);  
    if(isOza && !fn) 
    { 
      updated = true; 
    }
    else
    {
      int size    = dir.Bytes();
      int oldSize = (int)tolSources_[n].bytes_;
      if(oldSize!=size) 
      { 
        updated = false; 
      }
      else
      {
        time_t time      = dir.Time();
        BDate  date      = BTimer::TimeToDate(time);
        BDate  oldDate   = tolSources_[n].release_;
        double diff      = DifInSeconds(oldDate,date);
        if(fabs(diff)<1.0) 
        { 
          updated = true; 
        }
        else if(!isOza)
        { 
          updated = false; 
        }
        else if(fn && fn->Bytes()!=size) 
        { 
          updated = false; 
        }
        else 
        {
          char* contens = new char[size+1];
          char* oldCont = new char[size+1];
          FILE* file=fopen(path.String(),"rb");
          if(!file) 
          { 
            Warning(I2("Cannot check if source file ",
                       "No se pudo comprobar si el fichero fuente ")+path+
                    I2(" is synchronized with this module.",
                       " está sincronizado con este módulo."));
            updated = true; 
          }
          else
          {
            int r = fread(contens,1,size,file);  
            contens[r]='\0';
            fclose(file);
            assert(r==size);
            Read(oldCont,1,size,fn);
            oldCont[size]='\0';
            int cmp = memcmp(oldCont,contens,r);
            delete [] contens;
            delete [] oldCont;
            updated = (cmp==0);
            if(updated) 
            { 
            //tolSources_[n].release_ = date; 
              SetFileTime(path, BTimer::DateToTime(tolSources_[n].release_));
            }
          }
        }
      }
    }
  }
  return(updated);
}

//--------------------------------------------------------------------
  bool BOisLoader::Load(bool errorWarning, 
                        bool doShowHierarchy,
                        bool checkIsUpdate,
                        bool doLoadData,
                        int  showHrchyDepth,
                        int  showHrchyMaxChilds,
                        const BSet* partial)
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("BOisLoader::Load()");
#endif
  bool ok = true;
  Ensure(Open(errorWarning));
  Ensure(XMLReadHeader());
  const BSourcePath* curSourcePath = BSourcePath::Current();
  if(checkIsUpdate) { Ensure(!HasLostSource ()); }
  if(checkIsUpdate) { Ensure(!ObsoleteSource()); }
  if(doLoadData || doShowHierarchy)
  {
    Ensure(ReadHierarchyIndex());
  }
  if(doShowHierarchy)
  {
    ShowHierarchy();
  }
  if(doLoadData)
  {
    if(doc_.name_.HasName())
    {
      if(doc_.category_=="Code.Package")
      {
        Std(I2("OIS: Loading module ","OIS: Cargando el módulo ")+
            doc_.name_+"\n");
      }
    }
    Ensure(InitReaded());
    Ensure(SearchOffsetInHierarchy(partial));
    object_->GetPos();
    data_ = ReadNextObject();
    ok = (data_!=NULL);
  }
  Close();
  BSourcePath::SetCurrent(curSourcePath);
  return(ok);
}

//--------------------------------------------------------------------
  bool BOisLoader::Close()
//--------------------------------------------------------------------
{
  if(closed_) { return(false); }
  int n;
  for(n=0; n<readed_.Size(); n++)
  {
    if(readed_[n].Object())
    {
      readed_[n].Object()->PutOisOffset(0);
      readed_[n].Object()->DecNRefs();
    }
  }
  BOis::Close();
  return(true);
}

