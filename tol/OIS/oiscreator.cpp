/* oiscreator.cpp: main and init functions of TOL language.

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

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_init.h>
#include <tol/tol_oiscreator.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bscanner.h>
#include <tol/tol_bparser.h>
#include <tol/tol_btmsgrav.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bcmpgra.h>
#include <tol/tol_bvmatgra.h>
#include <tol/tol_bstruct.h>
#include "tol_ois_macros.h"

        
BTraceInit("oiscreator.cpp");



//--------------------------------------------------------------------
// BOisCreator Functions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  BOisCreator::BOisCreator() 
//--------------------------------------------------------------------
: BOis                   (),
  data_                  (),
  curHrchyEntry_         (0),
  allSourcePath_         (),
  tolEnvironment_        (),
  saved_                 ()
{  
}

//--------------------------------------------------------------------
 BOisCreator::~BOisCreator() 
//--------------------------------------------------------------------
{
}


//--------------------------------------------------------------------
  void BOisCreator::Create(const BText& connection,
                           BUserSet*    uData,
                           const BSet*  address,
                           const BSet*  doc,
                           const BSet*  opt) 
//--------------------------------------------------------------------
{  
  SetConnection(connection);
  SetAddress(address);
  SetDoc    (doc);
  SetControl();
  SetOptions(opt);
  SetData   (uData);
}

//--------------------------------------------------------------------
  void BOisCreator::CreateModule(const BText& tolFile, BUserSet* data)
//--------------------------------------------------------------------
{  
  SetModulePath(tolFile);
  BSet& defDoc = Set(defDoc_);
  BText name_         = Text(defDoc["Name"]);
  BText category_     = Text(defDoc["Category"]);
  BText subject_      = Text(defDoc["Subject"]);
  BText keys_         = Text(defDoc["Keys"]);
  BText abstract_     = Text(defDoc["Abstract"]);
  BText authors_      = Text(defDoc["Authors"]);
  BText distribution_ = Text(defDoc["Distribution"]);
  BText repository_   = Text(defDoc["Repository"]);
  doc_.name_          = (name_.HasName()?name_+"::":BText(""))+tolFile;
  doc_.category_      = "Code.Package";
  doc_.subject_       = (subject_?subject_+". ":BText(""))+
                        I2("Autogenerated TOL module from ",
                          "M�dulo TOL autogenerado a partir de ")+tolFile;
  doc_.keys_          = (keys_?keys_+" ":BText(""))+"module";
  doc_.abstract_      = abstract_;
  doc_.authors_       = authors_;
  doc_.abstract_      = abstract_;
  doc_.distribution_  = distribution_;
  doc_.repository_    = repository_;
  SetControl();
  SetData(data);
}

//--------------------------------------------------------------------
  void BOisCreator::SetData(BUserSet* uData)
//--------------------------------------------------------------------
{  
  bool ok = false;
  data_ = uData;
  if(uData)
  {
    BSet& data  = data_->Contens();   
    if(data.Card())
    {
      ok = true; 
    }
  }
  if(doc_.name_.HasName())
  {
    if(doc_.category_=="Code.Package")
    {
      if(ok)
      {
        Std(I2("OIS: Creating module ","OIS: Creando el m�dulo ")+
            doc_.name_+"\n");
      }
      else
      {
        Std(I2("OIS: Cannot create empty module ","OIS: NO se puede crear el m�dulo vac�o ")+
            doc_.name_+"\n");
      }
    }
  }
}

//--------------------------------------------------------------------
  void BOisCreator::SetControl() 
//--------------------------------------------------------------------
{  
  static BText tolCurrentVersion_ = TOLVersion();
  char* CppCompiler();

  control_.oisEngine_.curDir_       = BDir::GetCurrent();
  control_.oisEngine_.release_      = DteNow();
  control_.oisEngine_.oisVersion_   = oisCurrentVersion_;
 
  control_.machine_.hostName_       = BSys::HostName();
  control_.machine_.isLittleEndian_ = isLittleEndian_;
  control_.machine_.osName_         = SYSTEM;

  control_.tolEngine_.cppCompiler_  = CppCompiler();
  control_.tolEngine_.tolVersion_   = tolCurrentVersion_;
  control_.tolEngine_.tolRelease_   = BText(__DATE__)+" "+__TIME__;
  control_.tolEngine_.tolUserLang_  = I2("ENGLISH","CASTELLANO");


}

//--------------------------------------------------------------------
  void BOisCreator::SetDoc(const BSet* doc)
//--------------------------------------------------------------------
{
  if(doc && (doc->Struct()==BOis::docStr_))
  {
    doc_.name_         = Text(doc->Field("Name"));
    doc_.category_     = Text(doc->Field("Category"));
    doc_.subject_      = Text(doc->Field("Subject"));
    doc_.keys_         = Text(doc->Field("Keys"));
    doc_.distribution_ = Text(doc->Field("Distribution"));
    doc_.abstract_     = Text(doc->Field("Abstract"));
    doc_.authors_      = Text(doc->Field("Authors"));
    doc_.repository_   = Text(doc->Field("Repository"));
  }
  else
  {
    doc_.name_         = "";
    doc_.category_     = "";
    doc_.subject_      = "";
    doc_.keys_         = "";
    doc_.distribution_ = "";
    doc_.abstract_     = "";
    doc_.authors_      = "";
    doc_.repository_   = "";
  }
}


#define EnsureDirExist(dir)                               \
if(!BDir::CheckIsDir(dir)) { BSys::Mkdir(dir,true); }     \
if(!BDir::CheckIsDir(dir))                                \
{                                                         \
  return(Error(BText("Cannot create directory ")+dir));   \
}


#define EnsureFileOpenW(fn,T,N)                        \
  if(! ( fn = streamHandler_->Open(T,SubPath()+N) ) )  \
{                                                      \
  return(false);                                       \
}


//--------------------------------------------------------------------
  bool BOisCreator::Open()
//--------------------------------------------------------------------
{
  streamHandler_ = BStreamHandler::GetConnect
  (
    connection_, 
    BStreamHandler::BSHOM_WRITE, 
    true
  );
  if(streamHandler_)
  {
    RemoveImage();
    EnsureFileOpenW(header_,      "header",      "header.xml"   );
    EnsureFileOpenW(tolref_,      "tolref",      ".tolref"      );
    EnsureFileOpenW(oisref_,      "oisref",      ".oisref"      );
    EnsureFileOpenW(object_,      "object",      ".object"      );
    EnsureFileOpenW(set_,         "set",         ".set"         );
    EnsureFileOpenW(serie_,       "serie",       ".serie"       );
    EnsureFileOpenW(timeset_,     "timeset",     ".timeset"     );
    EnsureFileOpenW(matrix_,      "matrix",      ".matrix"      );
    EnsureFileOpenW(polyn_,       "polyn",       ".polyn"       );
    EnsureFileOpenW(ratio_,       "ratio",       ".ratio"       );
    EnsureFileOpenW(code_,        "code",        ".code"        );
    EnsureFileOpenW(hrchyDetail_, "hrchyDetail", ".hrchyDetail" );
    EnsureFileOpenW(hrchyOffset_, "hrchyOffset", ".hrchyOffset" );
    EnsureFileOpenW(hrchyOrder_,  "hrchyOrder",  ".hrchyOrder"  );
    EnsureFileOpenW(export_,      "export",      "export.csv"   );

    BText address = SubPath();
    EWrite(address+"/.tolref>\n",      tolref_);
    EWrite(address+"/.oisref>\n",      oisref_);
    EWrite(address+"/.object>\n",      object_);
    EWrite(address+"/.set>\n",         set_);
    EWrite(address+"/.serie>\n",       serie_);
    EWrite(address+"/.timeset>\n",     timeset_);
    EWrite(address+"/.matrix>\n",      matrix_);
    EWrite(address+"/.polyn>\n",       polyn_);
    EWrite(address+"/.ratio>\n",       ratio_);
    EWrite(address+"/.code>\n",        code_);
    EWrite(address+"/.hrchyDetail>\n", hrchyDetail_);
    EWrite(address+"/.hrchyOffset>\n", hrchyOffset_);
    EWrite(address+"/.hrchyOrder>\n",  hrchyOrder_);
    export_->Print("ModeId;GrammarId;Name;Mode;Grammar;\n");
    enable_BSE_ = true;
  }
  else
  {
    Error("Cannot create image");
  }
  SetAllFiles();
  return(streamHandler_!=NULL);
}

//--------------------------------------------------------------------
  bool BOisCreator::WriteSource(int n, int size, const BText& path)
//--------------------------------------------------------------------
{
  if(!streamHandler_) { return(false); }
  FILE* file=fopen(path.String(),"rb");
  if(!file) { return(false); }
  char* contens = new char[size+1];
  int r = fread(contens,1,size,file);  
  contens[r]='\0';
  fclose(file);
  assert(r==size);
  BText name = BText("._tol_source_/")+PlainPath(path);
  BStream* fn = streamHandler_->Open(name,name);
  Write(contens,1,size,fn);
  delete [] contens;
  fn->Flush();
  source_.Add(fn);
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::RemoveSource() 
//--------------------------------------------------------------------
{
  if(!streamHandler_) { return(false); }
  BArray<BText> all(allSourcePath_.Size());
  BText path;
  for(int n=0; n<allSourcePath_.Size(); n++)
  {
    path = allSourcePath_[n];
    path = ReplaceSlash(BText("._tol_source_/")+PlainPath(path)); 
    all[n]=path;
  };
  streamHandler_->RemoveFiles(all);
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::DoStat()
//--------------------------------------------------------------------
{
  stat_.buildingTime_ = double(elapsed_.MSec())/1000.0;
  stat_.fileStat_[0].entries_ = -1;
  stat_.fileStat_[0].bytes_   = -1;
  for(int n=1; n<allFiles_.Size(); n++) 
  { 
    if(allFiles_[n])
    {
      stat_.fileStat_[n].entries_ = allFiles_[n]->Entries(); 
      stat_.fileStat_[n].bytes_   = allFiles_[n]->Bytes(); 
    }
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BText& v, BStream* stream)
//--------------------------------------------------------------------
{
  TRACE_OIS_STREAM("WRITE",stream,"BText", v);
  int length = v.Length();
  EWrite(length,stream);
  Ensure(Write((const void*)v.String(),1,length, stream));
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BDat& v, BStream* stream)
//--------------------------------------------------------------------
{
  TRACE_OIS_STREAM("WRITE",stream,"BDat", BText("")<<v);
  double x = v.Value();
  EWrite(x, stream);
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BComplex& v, BStream* stream)
//--------------------------------------------------------------------
{
  TRACE_OIS_STREAM("WRITE",stream,"BComplex", BText("")<<v);
  EWrite(v.X(), stream);
  EWrite(v.Y(), stream);
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BDate& v, BStream* stream)
//--------------------------------------------------------------------
{
  TRACE_OIS_STREAM("WRITE",stream,"BDate", BText("")<<v);
  double h = v.Hash();
  return(Write(h,stream));
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BDat* buf, int s, BStream* stream)
//--------------------------------------------------------------------
{
  TRACE_OIS_STREAM("WRITE",stream,"BDat*", BText("")<<s);
  Ensure(Write(buf,sizeof(BDat),s,stream));
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BPol& x, BStream* stream)
//--------------------------------------------------------------------
{
  TRACE_OIS_STREAM("WRITE",stream,"BPol", x.Name());
  int s = x.Size();
  EWrite(s, stream);
  BCoefDeg* buf = new BCoefDeg[x.Size()];
  for(int i=0; i<x.Size(); i++)
  {
    buf[i].degree_      = x(i).Degree();
    buf[i].coefficient_ = x(i).Coef  ();
  }
  Ensure(Write(buf,sizeof(BCoefDeg),x.Size(),stream));
  delete [] buf;
  return(true);
};


//--------------------------------------------------------------------
  bool BOisCreator::Write(const BMemberOwner& owner, 
                          const BMemberOwner::BClassByNameHash& x, 
                          BStream* stream)
//--------------------------------------------------------------------
{
  int n = 0;
  int s = x.size();
  TRACE_OIS_STREAM("WRITE",stream,"BMemberOwner", "");
  BMemberOwner::BClassByNameHash::const_iterator iterC;
  EWrite(s,stream);
  for(iterC=x.begin(); iterC!=x.end(); iterC++, n++)
  {
    EWrite(iterC->second->Name(),stream);
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BClass& cls, BStream* stream)
//--------------------------------------------------------------------
{
  TRACE_OIS_STREAM("WRITE",stream,"BClass", BText("")<<cls.FullName());
  Ensure(Write(cls, *(cls.parentHash_), object_));
  Ensure(Write(cls, *(cls.ascentHash_), object_));
  int n = 0;
  int s = cls.member_.Size();
  EWrite(s,stream);
  for(n=0; n<cls.member_.Size(); n++)
  {
    const BMember& mbr = *(cls.member_[n]->member_);
    EWrite(mbr.parent_->getName(), stream);
    EWrite(mbr.name_             , stream);
    if(mbr.parent_==&cls)
    {
      char isMethod = mbr.isMethod_;
      char isStatic = mbr.isStatic_;
      Ensure(WriteTree(mbr.branch_, stream));
      EWrite(mbr.declaration_     , stream);
      EWrite(mbr.definition_      , stream);
      EWrite(isMethod             , stream);
      EWrite(isStatic             , stream);      
    }
  }
  return(true);
}


//--------------------------------------------------------------------
  bool BOisCreator::WriteToken(const BToken& token, 
                               BArray<char>& streamBuf,
                               unsigned int& pos)
//--------------------------------------------------------------------
{
  const BText& name = token.Name();
  char  tt = token.TokenType ();
  short pr = token.Precedence();
  char  cl = GetIdFromTokenClose(token.Close());
  unsigned short len = name.Length();
  int needed = 
    pos+ 
    sizeof(unsigned short)+
    len+
    sizeof(char)+
    sizeof(short)+
    sizeof(char);
  if(streamBuf.MaxSize()<=needed+10)
  {
    streamBuf.ReallocBuffer(needed*2);
  }
  memcpy(streamBuf.GetBuffer()+pos,&len,sizeof(unsigned short));
  pos+=sizeof(unsigned short);
  memcpy(streamBuf.GetBuffer()+pos,name.String(),len);
  pos+=len;
  memcpy(streamBuf.GetBuffer()+pos,&tt,sizeof(char));
  pos+=sizeof(char);
  memcpy(streamBuf.GetBuffer()+pos,&pr,sizeof(short));
  pos+=sizeof(short);
  memcpy(streamBuf.GetBuffer()+pos,&cl,sizeof(char));
  pos+=sizeof(char);
#ifdef TRACE_OIS_TOKEN
  BText aux0 = Compact(name);
  BText aux1 = Replace(aux0,'\"','|');
  BText aux2 = Replace(aux1,'\'','|');
  fprintf(tokWrite_,"\n%ld\t%ld\t%ld\t%s",(int)tt,(int)pr,(int)cl,aux2.String());
  fflush(tokWrite_);
#endif
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::WriteTree(const List* tre, 
                              BArray<char>& streamBuf,
                              unsigned int& pos)
//--------------------------------------------------------------------
{
  char cl = 0; 
  if(!tre)
  {
    memcpy(streamBuf.GetBuffer()+pos,&cl,sizeof(char));
    pos+=sizeof(char);
  }
  else
  {
    cl = tre->car()->IsListClass()?2:1; 
    memcpy(streamBuf.GetBuffer()+pos,&cl,sizeof(char));
    pos+=sizeof(char);
    if(cl==1)
    {
      BToken* tok = (BToken*)tre->car();
      Ensure(WriteToken(*tok,streamBuf,pos));
    }
    else
    {
      List* lst= (List*)tre->car();
      Ensure(WriteTree(lst, streamBuf,pos));
    }
    Ensure(WriteTree(tre->cdr(),streamBuf,pos));
  }
  return(true);
};


//--------------------------------------------------------------------
  bool BOisCreator::WriteTree(const List* tre, BStream* stream)
//--------------------------------------------------------------------
{
  BArray<char> streamBuf(1024*32);
  unsigned int pos = 0;
  WriteTree(tre, streamBuf, pos);
  EWrite(pos,stream);
  Write(streamBuf.Buffer(),1,pos,stream);
  return(true);
};


//--------------------------------------------------------------------
  bool BOisCreator::Write(BSyntaxObject* v)
//--------------------------------------------------------------------
{
  if(!v)
  {
    return(Error("FATAL BOisCreator::Write(NULL)"));
  }
  BGrammar* gra  = v->Grammar();
  if(v->System() && 
     ( 
       (gra==GraReal()) ||
       (gra==GraDate()) ||
       (gra==GraText())
     ))
  {
    tolEnvironment_.AddUniqueSorted(v,CompareAddress);
  }
//If object has already offset no insert it
  if(v->OisOffset()) { return(true); }
  char  format = 0;
  const BText& name   = v->Name();
  char         system = v->System();
  char         mode   = v->Mode();
  if(mode == BBUILTINFUNMODE) { return(true); }
  if( (mode != BSTRUCTMODE)  &&
      (mode != BCLASSMODE)  &&
      (mode != BOBJECTMODE) )
  {
    Warning(I2("Cannot add special objects",
               "No se pueden a�adir objetos especiales")+
            ".\n "+v->ModeName()+" "+name+" "+
            I2("will be ignored.","ser� ignorado."));
    return(true);
  }  
  if(!system)
  {
    BText path = GetStandardAbsolutePath(v->SourcePath());
    if(path.HasName())
    {
      allSourcePath_.AddUniqueSorted(path,BTextOrderCriterium);
    }
  }
  BUserFunction* usf = NULL;
  if(gra==GraCode())
  {
    BUserCode* code = UCode(v);
    BCode&     cod  = code->Contens();
    BOperator* opr  = cod.Operator();
    if(!opr) { return(Error("FATAL BOisCreator::Write NULL Code operator")); }
    if(opr && (opr->Mode()==BUSERFUNMODE))
    {
      mode   = opr->Mode   ();
      gra    = opr->Grammar();
      system = opr->System ();
      usf = (BUserFunction*)opr;
    }
  }
  char tol_type         = gra->Gid();  
  char is_referenceable = !system && 
                          name.HasName() && 
                          BParser::DefaultParser()->Filter()->IsIdentifier(name) &&
                          (v->Level()<=BGrammar::Level());

  object_->GetPos();
  v->PutOisOffset(object_->Offset());
  if(saved_.Size()>=saved_.MaxSize()) { saved_.ReallocBuffer(2*saved_.MaxSize()); }
  saved_.Add(v);
  if(is_referenceable)
  {
    EWrite(object_->Offset(), tolref_);
    tolref_->IncEntries();
  }
  EWrite(object_->Offset(), oisref_);
  oisref_->IncEntries();

  EWrite(system,           object_);
  EWrite(is_referenceable, object_);
  EWrite(mode,             object_);
  EWrite(tol_type,         object_);
  EWrite(name,             object_);

//System objects save no more information
  if(!system)
  {
    int n, s;
    if(mode==BSTRUCTMODE)
    {
      BStruct& str = *((BStruct*) v);
      s = str.Size();
      EWrite(s, object_);
      for(n=0; n<s; n++)
      {
        char gid = str[n].Grammar()->Gid();
        EWrite(str[n].Name(), object_);
        EWrite(gid, object_);
        if(str[n].Grammar()==GraSet())
        {
          if(str[n].Struct())
          {
            EWrite(str[n].Struct()->Name(), object_);
          }
          else
          {
            EWrite("", object_);
          }
        }  
      } 
    }
    else if(mode==BCLASSMODE)
    {
      BClass& cls = *((BClass*) v);
      Ensure(Write(cls, object_));
    }
    else
    {
      EWrite(v->Description(), object_);
      EWrite(format, object_);
      if(mode==BUSERFUNMODE)
      {
        const List* dec = usf->Declaration();
        const List* def = usf->Definition ();
        EWrite(code_->GetPos(), object_);
        Ensure(WriteTree(dec, code_));
        Ensure(WriteTree(def, code_));
      //EWrite(BParser::Unparse(dec,"  ","\n"), code_);
      //EWrite(BParser::Unparse(def,"  ","\n"), code_);
      }
      else //if(mode==BOBJECTMODE)
      {
        Ensure(WriteData(v, tol_type));
      }
    }
  }
  if(is_referenceable)
  {
    int gid = gra->Gid();
    int m   = mode;
    BText modeTxt = GetModeName(m);
    export_->Print("%ld;%ld;%s;%s;%s;\n",
                   m,gid,name.String(),gra->Name().String(),modeTxt.String());
  }
  return(true);
}


//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BSyntaxObject* v, char tol_type)
//--------------------------------------------------------------------
{
       if(tol_type == BGI_Real      ) { return(Write(UDat (v)->Contens(), object_)); }
  else if(tol_type == BGI_Date      ) { return(Write(UDate(v)->Contens(), object_)); }
  else if(tol_type == BGI_Text      ) { return(Write(UText(v)->Contens(), object_)); }
  else if(tol_type == BGI_Date      ) { return(Write(UDate(v)->Contens(), object_)); }
  else if(tol_type == BGI_Complex   ) { return(Write(UCmp (v)->Contens(), object_)); }
  else if(tol_type == BGI_Code      ) { return(WriteData(UCode      (v))); }
  else if(tol_type == BGI_Set       ) { return(WriteData(USet       (v))); }
  else if(tol_type == BGI_NameBlock ) { return(WriteData(UNameBlock (v))); }
  else if(tol_type == BGI_Serie     ) { return(WriteData(Tsr        (v))); }
  else if(tol_type == BGI_TimeSet   ) { return(WriteData(Tms        (v))); }
  else if(tol_type == BGI_Matrix    ) { return(WriteData(UMat       (v))); }
  else if(tol_type == BGI_VMatrix   ) { return(WriteData(UVMat      (v))); }
  else if(tol_type == BGI_Polyn     ) { return(WriteData(UPol       (v))); }
  else if(tol_type == BGI_Ratio     ) { return(WriteData(URat       (v))); }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserCode* v)
//--------------------------------------------------------------------
{
  BCode& code = v->Contens();
  BOperator* opr = code.Operator();
  if(!opr) { return(Error("FATAL BOisCreator::Write NULL Code operator")); }
  char oprType = opr->Grammar()->Gid();
  EWrite(oprType, object_);
  EWrite(opr->Name(), object_);
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BSet& x)
//--------------------------------------------------------------------
{
  char sbt = x.SubType();
  int h, n, s = x.Card(), hrchyEntry=curHrchyEntry_++;
  BArray<BOis::BIndexElement   > idx;
  BArray<BOis::BGlossaryElement> glo;
  if(options_.oisConfig_.buildHierarchy_)
  {
    idx.ReallocBuffer(s); 
    glo.ReallocBuffer(s); 
    glo.ReallocBuffer(0); 
  }

  EWrite(set_->GetPos(), object_);
  EWrite(s, set_);
  EWrite(sbt, set_);
  char isNameBlock = x.GetNameBlock()!=NULL;
  EWrite(isNameBlock,set_);
  if(isNameBlock)
  {
    EWrite(x.GetNameBlock()->Name(),     set_);
    EWrite(x.GetNameBlock()->LocalName(),set_);
  } 
  else if(doc_.category_.BeginWith("Code.") && 
         (x.SubType()>=BSet::MODFile)&&(x.SubType()<=BSet::BMIFile))
  {
    assert(x.SourcePath());
    BText tolPath = x.SourcePath()->Set()->TolPath();
    EWrite(tolPath, set_);
  }
  if(!x.Struct())
  {
    assert(sbt!=BSet::Structured);
    BINT64 offset = 0;
    EWrite(offset, set_);
  }
  else
  {
    assert(sbt==BSet::Structured);
    BINT64 offset = x.Struct()->OisOffset();
    if(!offset) { offset = object_->GetPos(); }
    EWrite(offset, set_);
    Ensure(Write(x.Struct()));
  }
  h=0;
  int classForwardDeclarations = 0;
  for(n=1; n<=s; n++)
  {
    if(x[n] && x[n]->Mode()==BCLASSMODE) 
    {
      classForwardDeclarations++;
    }
  }
  EWrite(classForwardDeclarations, set_);
  for(n=1; n<=s; n++)
  {
    if(x[n] && x[n]->Mode()==BCLASSMODE) 
    {
      EWrite(x[n]->Name(), set_);
    }
  }
  for(n=1; n<=s; n++)
  {
    if(!x[n]) 
    {
      Error(I2("Cannot add NULL objects",
               "No se pueden a�adir objetos nulos"));
      return(false);
    }
    if((x[n]->Mode() == BUSERFUNMODE   ) ||
       (x[n]->Mode() == BSTRUCTMODE    ) ||
       (x[n]->Mode() == BCLASSMODE     ) ||
       (x[n]->Mode() == BOBJECTMODE    ) ||
       (x[n]->Mode() == BBUILTINFUNMODE)   )
    {
      BINT64 offset = x[n]->OisOffset();
      if(!offset) { offset = object_->GetPos(); }
      if(options_.oisConfig_.buildHierarchy_)
      {
        idx[h].gid_          = x[n]->Grammar()->Gid();
        idx[h].mode_         = x[n]->Grammar()->Mode();
        idx[h].objectOffset_ = offset;
        idx[h].hrchyEntry_   = curHrchyEntry_;
#       ifdef TRACE_OIS_HIERARCHY
        fprintf(logHrchyWrite_,"\nidx[%ld,%ld,%"_LLD64_",(idx:%ld,glo:%ld)]->%s '%s'",
                h,
                idx[h].objectOffset_,
                idx[h].hrchyEntry_, 
                hi.hrchyOffset_,
                x[n]->Grammar()->Name(),
                x[n]->Identify());
                fflush(logHrchyWrite_);
#       endif
        if(x[n]->Name().HasName())
        {
          BOis::BGlossaryElement ge;
          ge.name_     = x[n]->Name();
          ge.position_ = n-1;
          glo.Add(ge);
        }
      }
      EWrite(offset, set_);
      Ensure(Write(x[n]));
      h++;             
    }
    else 
    {
      Error(I2("Cannot add special objects",
               "No se pueden a�adir objetos especiales")+
               ".\n "+x[n]->ModeName()+" "+x[n]->Name()+" "+
               I2("will be ignored.","ser� ignorado."));
      return(false);
    }
  }  
  if(options_.oisConfig_.buildHierarchy_)
  {
    hrchyOffset_->GetPos();
    hrchyDetail_->GetPos();
    int sz = hierarchyIndex_.Size();
    if(sz>=hierarchyIndex_.MaxSize())
    {
      hierarchyIndex_.ReallocBuffer(2*sz);
      hierarchyIndex_.ReallocBuffer(sz);
    }
    BHierarchyIndex hi;
    hi.hrchyOrder_ = sz;
    hi.hrchyEntry_ = hrchyEntry;
    hi.hrchyOffset_= hrchyDetail_->Offset();
    hierarchyIndex_.Add(hi);
  # ifdef TRACE_OIS_HIERARCHY
    fprintf(logHrchyWrite_,"\nhrchyEntry[%ld,%ld,%"_LLD64_",(idx:%ld,glo:%ld)]->'%s'", 
      sz,
      hi.hrchyEntry_, 
      hi.hrchyOffset_,
      idx.Size(),
      glo.Size(),
      v->Name());
      fflush(logHrchyWrite_);
  # endif
    glo.Sort(CompareGlossaryElement);
    EWrite(hi.hrchyEntry_,  hrchyOffset_);
    EWrite(hi.hrchyOffset_, hrchyOffset_);
    Ensure(WriteHierarchy(idx,glo,hrchyDetail_))
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserSet* v)
//--------------------------------------------------------------------
{
  return(Write(v->Contens()));
};

//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserNameBlock* v)
//--------------------------------------------------------------------
{
  BNameBlock& nb = v->Contens();
  Ensure(Write(nb.Set()));
  char hasClass = nb.Class()!=NULL;
  Ensure(Write(hasClass, set_));
  if(hasClass)
  {
    BText className = nb.Class()->FullName();
    Ensure(Write(className, set_));
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserTimeSerie* v)
//--------------------------------------------------------------------
{
  EWrite(serie_->GetPos(), object_);
  if(!v->Dating())
  {
    BINT64 offset = 0;
    BDat d = (*v)[DteToday()];
    EWrite(offset, serie_);
    EWrite(d,      serie_);
  }
  else
  { 
    BINT64 offset = v->Dating()->OisOffset();
    if(!offset) { offset = object_->GetPos(); }
    EWrite(offset, serie_);
    Ensure(Write(v->Dating()));
    EWrite(v->FirstDate(), serie_);
    EWrite(v->LastDate (), serie_);
    BHash h; v->Dating()->GetHashBetween(h, v->FirstDate(), v->LastDate());
    BDate beginCache = HashToDte(h[0]);
    BDate endCache   = HashToDte(h[h.Size()-1]);
    BData d; v->GetData(d, beginCache, endCache);
    EWrite(beginCache, serie_);
    EWrite(endCache,   serie_);
    EWrite(d.Size(), serie_);
    Ensure(Write(d.Buffer(),d.Size(),serie_));

    char saveTree = false;
    if(!v->FirstDate().HasValue() || !v->LastDate().HasValue())
    {
      const List* tree = v->GetOisTree(); 
      if(!tree)
      {
        EWrite(saveTree,serie_); 
        #ifdef WARNING_NON_TREE_NOR_BOUNDED
        Warning(I2("Cannot save virtual definition non bounded Serie ",
                   "No se puede salvar la definici�n virtual de la Serie ")+v->Identify()+
                I2(" due to its expression is unknown at this evaluation level.",
                   " no acotado porque su expresi�n es desconocida en este nivel de evaluaci�n."));     
        #endif
      }  
      saveTree = true;
      EWrite(saveTree,serie_);
      Ensure(WriteTree(tree,serie_)); 
    //BText expr = BParser::Unparse(tree,"  ","\n");
    //EWrite(expr,serie_); 
    }
    else
    {
      EWrite(saveTree,serie_);
    }
  //for(int n=0; n<d.Size(); n++) { EWrite(d[n], serie_); }  
  }
  return(true);
}


//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserTimeSet* v)
//--------------------------------------------------------------------
{
  EWrite(timeset_->GetPos(), object_);
  v->ForceCache();
  BDate inf = v->Inf();
  BDate sup = v->Sup();
  BDate bc  = v->BeginCache();
  BDate ec  = v->EndCache  ();
  BDate a = inf.HasValue()?inf:(bc.HasValue()?bc:BDate::DefaultFirst());
  BDate b = sup.HasValue()?sup:(ec.HasValue()?ec:BDate::DefaultLast ());
  BHash h; v->GetHashBetween(h, a, b);
  BDate beginCache = v->BeginCache();
  BDate endCache   = v->EndCache  ();
  EWrite(inf,        timeset_);
  EWrite(sup,        timeset_);
  EWrite(beginCache, timeset_);
  EWrite(endCache,   timeset_);
  EWrite(h.Size(),   timeset_);
  Ensure(Write(h.Buffer(),sizeof(double),h.Size(),timeset_));
//for(int n=0; n<h.Size(); n++) { EWrite(h[n], timeset_); }  
  char saveTree = false;
  if(!inf.HasValue() || !sup.HasValue())
  {
    const List* tree = v->GetOisTree(); 
    if(!tree)
    {
      EWrite(saveTree,timeset_); 
      #ifdef WARNING_NON_TREE_NOR_BOUNDED
      Warning(I2("Cannot save virtual definition non bounded TimeSet ",
                 "No se puede salvar la definici�n virtual del TimeSet ")+v->Identify()+
              I2(" due to its expression is unknown at this evaluation level.",
                 " no acotado porque su expresi�n es desconocida en este nivel de evaluaci�n.")); 
      #endif
    }  
    saveTree = true;
    EWrite(saveTree,timeset_);
    Ensure(WriteTree(tree,timeset_)); 
  //BText expr = BParser::Unparse(tree,"  ","\n");
  //EWrite(expr,timeset_); 
  }
  else
  {
    EWrite(saveTree,timeset_);
  }
  return(true);
}


//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserMat* v)
//--------------------------------------------------------------------
{
  EWrite(matrix_->GetPos(), object_);
  BMat& x = v->Contens();
  int r = x.Rows(), c =  x.Columns();
  EWrite(r, matrix_);
  EWrite(c, matrix_);
  if(x.Data().Size()==r*c)
  {
    Ensure(Write(x.Data().Buffer(),r*c,matrix_));
  }
  else
  {
    BDat* buf = new BDat[r*c];
    int i, j, k;
    for(i=k=0; i<r; i++) { for(j=0; j<c; j++) { buf[k++] = x(i,j); } }
    Ensure(Write(buf,r*c,matrix_));
    delete [] buf;
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserVMat* v)
//--------------------------------------------------------------------
{
  EWrite(matrix_->GetPos(), object_);
  BVMat& x = v->Contens();
  return(x.Write(*this, matrix_));
};


//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserPol* v)
//--------------------------------------------------------------------
{
  EWrite(polyn_->GetPos(), object_);
  EWrite(v->Contens(), polyn_);
  return(true);
};


//--------------------------------------------------------------------
  bool BOisCreator::WriteData(BUserRat* v)
//--------------------------------------------------------------------
{
  EWrite(ratio_->GetPos(), object_);
  BRat& x = v->Contens();
  EWrite(x.Numerator  (), ratio_);
  EWrite(x.Denominator(), ratio_);
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Build()
//--------------------------------------------------------------------
{
  saved_.AllocBuffer(16);
  saved_.AllocBuffer(0);
  Ensure(BuilTolEnv    ());
  Ensure(Open          ());
  Ensure(Write         (data_));
  Ensure(SortHierarchy ());
  Ensure(Flush         ());
  Ensure(WriteHeader());
  Ensure(Close         ());
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::Flush()
//--------------------------------------------------------------------
{
  int n;
  for(n=1; n<allFiles_.Size(); n++) 
  { 
    if(allFiles_[n])
    {
      allFiles_[n]->Flush(); 
    }
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::Close()
//--------------------------------------------------------------------
{
  if(closed_) { return(false); }
  int n;
  for(n=0; n<saved_.Size(); n++)
  {
    if(saved_[n])
    {
      saved_[n]->PutOisOffset(0);
    }
  }
  BOis::Close();
  return(true);
}

