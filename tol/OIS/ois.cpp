/* ois.cpp: main and init functions of GNU/TOL language.

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

#include <tol/tol_ois.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_btmsgrav.h>
#include <tol/tol_btxtgra.h>


#include <errno.h>

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("ois.cpp");

#if _WINDOWS || (__BYTE_ORDER == __LITTLE_ENDIAN)
  bool BOis::isLittleEndian_=true;
#else
  bool BOis::isLittleEndian_=false;
#endif
BText BOis::oisCurrentVersion_ = OIS_VERSION;
BText BOis::oisDefRoot_        = "";
int BOis::def_CLv_ = 9;

#ifdef __USE_ZIP_ARCHIVE__
BOis::BSerialEngine  BOis::def_BSE_ = BOis::BSE_NONE_;
BOis::BArchiveEngine BOis::def_BAE_ = BOis::BAE_ZIPARC_;
#else
BOis::BSerialEngine  BOis::def_BSE_ = BOis::BSE_BZIP2_;
BOis::BArchiveEngine BOis::def_BAE_ = BOis::BAE_NONE_;
#endif

BDateFormat BOis::dateFmt_("%c%Y-%m-%d %h:%i:%s");
BStruct*  BOis::addressStr_  = NULL;
BStruct*  BOis::docStr_      = NULL;
BUserSet* BOis::defDoc_      = NULL;

bool BOis::runningUseModule_ = false;

BArray<BSystemDat*> BOis::loadModeOptions_;

#ifdef TRACE_OIS_FILE
FILE* BOis::logWrite_ = fopen((BSys::TolAppData()+"syslog/OisLogWrite.log").String(),"w");
FILE* BOis::logRead_  = fopen((BSys::TolAppData()+"syslog/OisLogRead.log" ).String(),"w");
#endif

#ifdef TRACE_OIS_TOKEN
FILE* BOis::tokWrite_ = fopen((BSys::TolAppData()+"syslog/OisTokWrite.log").String(),"w");
FILE* BOis::tokRead_  = fopen((BSys::TolAppData()+"syslog/OisTokRead.log" ).String(),"w");
#endif


//--------------------------------------------------------------------
// BOis Functions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  BOis::BOis()
//--------------------------------------------------------------------
: 
  xmlParam_     (),
  control_      (),
  options_      (),
  address_      (),
  doc_          (),
  stat_         (),
  closed_       (false),
  connection_   (),
  streamHandler_(NULL),
  source_       (),
  packages_     ()
{
  InitBuild();
}


//--------------------------------------------------------------------
 BOis::~BOis() 
//--------------------------------------------------------------------
{
  Close();
}

//--------------------------------------------------------------------
  bool BOis::SetRunningUseModule(bool r) 
//--------------------------------------------------------------------
{ 
  bool old = runningUseModule_;
  runningUseModule_ = r; 
  return(old);
}

//--------------------------------------------------------------------
  void BOis::SetConnection(const BText& connection)
//--------------------------------------------------------------------
{
  connection_ = AutoPath(connection);
}

  //--------------------------------------------------------------------
  void BOis::SetPath(const BText& connection,
                           const BSet*  address) 
//--------------------------------------------------------------------
{  
  SetConnection(connection);
  SetAddress (address);
}

//--------------------------------------------------------------------
  void BOis::SetModulePath(const BText& tolFile) 
/*
  Returns a unique path to store the OIS module of a TOL file but 
  moving it do the TOL application data directory.
  tolFile is the relative (or absolute) path in the call of function 
  Ois.UseModule. 
*/
//--------------------------------------------------------------------
{  
  BText dirOriPath = GetFilePath(tolFile);
  BText filPrf = GetFilePrefix(tolFile);
  BText dirAbsPath = Replace(RemoveLastSlash(
    GetStandardAbsolutePath(dirOriPath)),"\\","/");
  BText oisSubPath = PlainPath(dirAbsPath+ArchiveExtension());
  connection_ = Replace(oisDefRoot_,"\\","/")+"module/"+oisSubPath;
  address_.concept_  = "";
  address_.version_  = "";
  address_.node_     = dirOriPath+filPrf;
  options_.tolSourceSearchPaths_.AllocBuffer(1);
  options_.tolSourceSearchPaths_[0].alias_ = ToName(GetFilePrefix(dirAbsPath));
  options_.tolSourceSearchPaths_[0].value_ = dirAbsPath;
/*
  Std(BText("\nTRACE BOis::SetModulePath('")+tolFile+"'");
  Std(BText("\nTRACE   connection_='")+connection_+"'");
  Std(BText("\nTRACE   node_='")+address_.node_+"'");
  Std(BText("\nTRACE   alias_='")+options_.tolSourceSearchPaths_[0].alias_+"'");
  Std(BText("\nTRACE   value_='")+options_.tolSourceSearchPaths_[0].value_+"'");
*/
}

//--------------------------------------------------------------------
  void BOis::InitBuild()
//--------------------------------------------------------------------
{
  xmlParam_.maxXmlStrLen_  = OIS_MaxXmlStrLen;
  control_.oisEngine_.oisVersion_="";
  control_.oisEngine_.release_=BDate::Unknown();
  control_.oisEngine_.curDir_="";
  control_.machine_.hostName_="";
  control_.machine_.isLittleEndian_=false;
  control_.machine_.osName_="";
  control_.tolEngine_.cppCompiler_="";
  control_.tolEngine_.tolVersion_="";
  control_.tolEngine_.tolRelease_="";
  control_.tolEngine_.tolUserLang_="";
  
  options_.oisConfig_.buildHierarchy_ = false;
  options_.compressor_.serialization_.engine_       = BOis::def_BSE_; 
  options_.compressor_.serialization_.level_        = BOis::def_CLv_;
  options_.compressor_.serialization_.minSizeCmprs_ = OIS_MinStreamSizeToCompress;
  options_.compressor_.fileManager_.engine_         = BOis::def_BAE_;
  options_.compressor_.fileManager_.level_          = BOis::def_CLv_;
  options_.tolSourceSearchPaths_.AllocBuffer(0);

  address_.concept_  ="";
  address_.version_  ="";
  address_.node_     ="";
  doc_.name_         ="";
  doc_.category_     ="";
  doc_.subject_      ="";
  doc_.keys_         ="";
  doc_.abstract_     ="";
  doc_.authors_      ="";
  doc_.distribution_ ="";
  doc_.repository_   ="";

  stat_.buildingTime_=-1.0;
  stat_.fileStat_.AllocBuffer(BFI_maxnum);
  streamHandler_ = NULL;
  header_        = NULL;
  tolref_        = NULL;
  oisref_        = NULL;
  object_        = NULL; 
  set_           = NULL;
  serie_         = NULL;   
  timeset_       = NULL;
  matrix_        = NULL; 
  polyn_         = NULL;
  ratio_         = NULL;
  code_          = NULL;
  hrchyDetail_   = NULL;
  hrchyOffset_   = NULL;
  hrchyOrder_    = NULL;
  export_        = NULL;
  allFiles_.AllocBuffer(BFI_maxnum);
  for(int n=0; n<allFiles_.Size(); n++) 
  { 
    allFiles_[n] = NULL;
  }
  hierarchyIndex_.AllocBuffer(0);
  hierarchyOrder_.AllocBuffer(0);
  source_        .AllocBuffer(0);
  hierarchyIndex_.ReallocBuffer(1024);
  source_        .ReallocBuffer(128);
  hierarchyIndex_.ReallocBuffer(0);
  source_        .ReallocBuffer(0);

  elapsed_.Reset();
  enable_BSE_ = false;
}

//--------------------------------------------------------------------
BText BOis::SerialEngine(BOis::BSerialEngine e)
//--------------------------------------------------------------------
{
  switch(e)
  {
    case BSE_DEFAULT_: return(SerialEngine(def_BSE_)); 
    case BSE_NONE_:    return("_NONE_"); 
    case BSE_BZIP2_:   return("_BZIP2_");
  }
  assert(0);
  return("_NONE_");
}

//--------------------------------------------------------------------
  void BOis::PutDefaultSerialEngine(BOis::BSerialEngine e)
//--------------------------------------------------------------------
{
  def_BSE_ = e;
}

//--------------------------------------------------------------------
BText BOis::ArchiveEngine(BOis::BArchiveEngine a)
//--------------------------------------------------------------------
{
  switch(a)
  {
    case BAE_DEFAULT_:  return(ArchiveEngine(def_BAE_)); 
    case BAE_NONE_:     return("_NONE_"); 
    case BAE_ZIPARC_:   return("_ZIPARC_");
  }
  assert(0);
  return("_NONE_");
}

//--------------------------------------------------------------------
  void BOis::PutDefaultArchiveEngine(BOis::BArchiveEngine a)
//--------------------------------------------------------------------
{
  def_BAE_ = a;
}

//--------------------------------------------------------------------
BText BOis::ArchiveExtension(BOis::BArchiveEngine a)
//--------------------------------------------------------------------
{
  switch(a)
  {
    case BAE_DEFAULT_:  return(ArchiveExtension(def_BAE_)); 
    case BAE_ZIPARC_:   return(".oza");
    case BAE_NONE_:     return("");
  }
  assert(0);
  return("");
}

//--------------------------------------------------------------------
BOis::BSerialEngine BOis::SerialEngine(const BText& txt)
//--------------------------------------------------------------------
{
       if(txt=="_BZIP2_") { return(BSE_BZIP2_); }
  else if(txt=="_BZIP_")  { return(BSE_BZIP2_); }
  else                    { return(BSE_NONE_); }
}

//--------------------------------------------------------------------
BOis::BArchiveEngine BOis::ArchiveEngine      (const BText& txt)
//--------------------------------------------------------------------
{
  if(txt=="_ZIPARC_") { return(BAE_ZIPARC_); }
  else                { return(BAE_NONE_); }
}


//--------------------------------------------------------------------
   void BOis::SwapEndian(void* Addr, const int Nb) 

// PURPOSE: Swap the byte order of a structure in order to avoid
//          endian-ness problems
//--------------------------------------------------------------------
{
	static char Swapped[16];
	switch (Nb) 
  {
		case 2:	
        Swapped[0]=*((char*)Addr+1);
				Swapped[1]=*((char*)Addr  );
				break;
		case 3:	// As far as I know, 3 is used only with RGB images
				Swapped[0]=*((char*)Addr+2);
				Swapped[1]=*((char*)Addr+1);
				Swapped[2]=*((char*)Addr  );
				break;
		case 4:	
        Swapped[0]=*((char*)Addr+3);
				Swapped[1]=*((char*)Addr+2);
				Swapped[2]=*((char*)Addr+1);
				Swapped[3]=*((char*)Addr  );
				break;
		case 8:	
        Swapped[0]=*((char*)Addr+7);
				Swapped[1]=*((char*)Addr+6);
				Swapped[2]=*((char*)Addr+5);
				Swapped[3]=*((char*)Addr+4);
				Swapped[4]=*((char*)Addr+3);
				Swapped[5]=*((char*)Addr+2);
				Swapped[6]=*((char*)Addr+1);
				Swapped[7]=*((char*)Addr  );
				break;
		case 16:
        Swapped[ 0]=*((char*)Addr+15);
				Swapped[ 1]=*((char*)Addr+14);
				Swapped[ 2]=*((char*)Addr+13);
				Swapped[ 3]=*((char*)Addr+12);
				Swapped[ 4]=*((char*)Addr+11);
				Swapped[ 5]=*((char*)Addr+10);
				Swapped[ 6]=*((char*)Addr+ 9);
				Swapped[ 7]=*((char*)Addr+ 8);
				Swapped[ 8]=*((char*)Addr+ 7);
				Swapped[ 9]=*((char*)Addr+ 6);
				Swapped[10]=*((char*)Addr+ 5);
				Swapped[11]=*((char*)Addr+ 4);
				Swapped[12]=*((char*)Addr+ 3);
				Swapped[13]=*((char*)Addr+ 2);
				Swapped[14]=*((char*)Addr+ 1);
				Swapped[15]=*((char*)Addr   );
				break;
    default: assert(0);
	}
	memcpy(Addr,Swapped,Nb);
}

//--------------------------------------------------------------------
   void BOis::SetAllFiles()
//--------------------------------------------------------------------
{
  allFiles_[BFI_header     ] = header_;
  allFiles_[BFI_tolref     ] = tolref_;
  allFiles_[BFI_oisref     ] = oisref_;
  allFiles_[BFI_object     ] = object_;
  allFiles_[BFI_set        ] = set_;
  allFiles_[BFI_serie      ] = serie_;
  allFiles_[BFI_timeset    ] = timeset_;
  allFiles_[BFI_matrix     ] = matrix_;
  allFiles_[BFI_polyn      ] = polyn_;
  allFiles_[BFI_ratio      ] = ratio_;
  allFiles_[BFI_code       ] = code_;
  allFiles_[BFI_hrchyDetail] = hrchyDetail_;
  allFiles_[BFI_hrchyOffset] = hrchyOffset_;
  allFiles_[BFI_hrchyOrder ] = hrchyOrder_;
  allFiles_[BFI_export     ] = export_;
}

//--------------------------------------------------------------------
  void BOis::SetAddress(const BSet* address)
//--------------------------------------------------------------------
{
  if(address && (address->Struct()==BOis::addressStr_))
  {
    address_.concept_  = Text(address->Field("Concept"));
    address_.version_  = Text(address->Field("Version"));
    address_.node_     = Text(address->Field("Node"));
  }
  else
  {
    address_.concept_  = "";
    address_.version_  = "";
    address_.node_     = "";
  }
}

//--------------------------------------------------------------------
  BText BOis::SubPath() const
//--------------------------------------------------------------------
{
  BText dir=""; 
  if(address_.concept_.HasName()) { dir += address_.concept_+"/"; }
  if(address_.version_.HasName()) { dir += address_.version_+"/"; }
  if(address_.node_   .HasName()) { dir += address_.node_   +"/"; }
  return(dir);
}

//--------------------------------------------------------------------
  BText BOis::Path() const
//--------------------------------------------------------------------
{
  if(!streamHandler_) { return(""); }
  else                { return(streamHandler_->Connection()); }
/*
  BText sp = SubPath();
  BText p  = (sp.HasName()&&root_.HasName())?root_+"/"+sp:root_+sp;
  return(p);
*/
}

//--------------------------------------------------------------------
  bool BOis::RemoveImage() 
//--------------------------------------------------------------------
{
  if(!streamHandler_) { return(false); }
  BArray<BText> all(allFiles_.Size());
  int n=0;
//Std(BText("TRACE  BOis::RemoveImage() 1\n"));
  BText sp = ReplaceSlash(SubPath());
//Std(BText("TRACE  BOis::RemoveImage() 2\n"));
  all[n++]=sp+"header.xml";
  all[n++]=sp+".tolref";
  all[n++]=sp+".oisref";
  all[n++]=sp+".object";
  all[n++]=sp+".set";
  all[n++]=sp+".serie";
  all[n++]=sp+".timeset";
  all[n++]=sp+".matrix";
  all[n++]=sp+".polyn";
  all[n++]=sp+".ratio";
  all[n++]=sp+".code";
  all[n++]=sp+".hrchyDetail";
  all[n++]=sp+".hrchyOffset";
  all[n++]=sp+".hrchyOrder";
  all[n++]=sp+"export.csv";
//Std(BText("TRACE  BOis::RemoveImage() 3\n"));
  streamHandler_->RemoveFiles(all);
//Std(BText("TRACE  BOis::RemoveImage() 4\n"));
  return(true);
}

//--------------------------------------------------------------------
 bool BOis::Close()
//--------------------------------------------------------------------
{
  if(closed_) { return(false); }
  int n;
  if(streamHandler_) { streamHandler_->Disconnect(); }
  for(n=0; n<allFiles_.Size(); n++) 
  { 
    if(allFiles_[n]) 
    { 
      allFiles_[n]->Close();
      delete allFiles_[n]; 
      allFiles_[n] = NULL; 
    }
  }
  for(n=0; n<source_.Size(); n++) 
  { 
    if(source_[n]) 
    { 
      source_[n]->Close();
      delete source_[n]; 
      source_[n] = NULL; 
    }
  }
  closed_ = true;
  return(true);
}

//--------------------------------------------------------------------
  bool BOis::Error(const BText& msg) const
//--------------------------------------------------------------------
{
  ::Error(BText("OIS: ")+SubPath()+"\n"+ msg);  
  return(false);
}

//--------------------------------------------------------------------
  BSyntaxObject* BOis::NullError(const BText& msg) const
//--------------------------------------------------------------------
{
  ::Error(BText("OIS: ")+SubPath()+"\n"+ msg);  
  return(NULL);
}

//--------------------------------------------------------------------
  bool BOis::Warning(const BText& msg) const
//--------------------------------------------------------------------
{
  ::Warning(BText("OIS: ")+SubPath()+"\n"+ msg);  
  return(false);
}

