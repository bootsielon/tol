/* oisxml.cpp: main and init functions of TOL language.

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

#include <tol/tol_oiscreator.h>
#include <tol/tol_oisloader.h>
#include <tol/tol_bdir.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdtegra.h>

BTraceInit("oisxml.cpp");

//--------------------------------------------------------------------
// OIS writing XML functions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  bool BOisCreator::WriteHeader()
//--------------------------------------------------------------------
{
  int n, m;

  header_->Print("<?xml version='1.0' encoding='ISO-8859-1'?>\n");
  header_->Print("<header>\n");

  header_->Print("<xmlParam>\n");
  header_->Print("<maxXmlStrLen>%d</maxXmlStrLen>\n",            xmlParam_.maxXmlStrLen_);
  header_->Print("</xmlParam>\n");

  header_->Print("<control>\n");

  header_->Print("<oisEngine>\n");
  header_->Print("<version>%s</version>\n",                       control_.oisEngine_.oisVersion_.String());
  header_->Print("<release>%s</release>\n",                       dateFmt_.DateToText(control_.oisEngine_.release_).String());
  header_->Print("<curDir>%s</curDir>\n",                         control_.oisEngine_.curDir_.String());
  header_->Print("</oisEngine>\n");

  header_->Print("<machine>\n");
  header_->Print("<hostName>%s</hostName>\n",                     control_.machine_.hostName_.String());
  header_->Print("<isLittleEndian>%d</isLittleEndian>\n",         control_.machine_.isLittleEndian_);
  header_->Print("<osName>%s</osName>\n",                         control_.machine_.osName_.String());
  header_->Print("</machine>\n");

  header_->Print("<tolEngine>\n");
  header_->Print("<cppCompiler>%s</cppCompiler>\n",               control_.tolEngine_.cppCompiler_.String());
  header_->Print("<tolVersion>%s</tolVersion>\n",                 control_.tolEngine_.tolVersion_.String());
  header_->Print("<tolRelease>%s</tolRelease>\n",                 control_.tolEngine_.tolRelease_.String());
  header_->Print("<tolUserLang>%s</tolUserLang>\n",               control_.tolEngine_.tolUserLang_.String());
  if(oisWriteVersion_>="02.17")
  {
    header_->Print("<globalRequiredPackages>\n");
    const BRequiredPackage& grp = BNameBlock::GlobalRequiredPackages();
    int grp_n = grp.CountRequiredPackage();
    header_->Print("<entries>%ld</entries>\n",grp_n);
    for(n=1; n<=grp_n; n++)
    {
      header_->Print("<package_%ld>%s</package_%ld>\n",
                     n, grp.GetRequiredPackageVersion(n-1).String(), n);
    }
    header_->Print("</globalRequiredPackages>\n");
  }
  header_->Print("</tolEngine>\n");

  header_->Print("<typeSizes>\n");
  header_->Print("<pointer>%d</pointer>\n",                       control_.typeSizes_.sizeof_pointer_);
  header_->Print("<char>%d</char>\n",                             control_.typeSizes_.sizeof_char_);
  header_->Print("<short>%d</short>\n",                           control_.typeSizes_.sizeof_short_);
  header_->Print("<int>%d</int>\n",                               control_.typeSizes_.sizeof_int_);
  header_->Print("<BINT64>%d</BINT64>\n",                         control_.typeSizes_.sizeof_BINT64_);
  header_->Print("<size_t>%d</size_t>\n",                         control_.typeSizes_.sizeof_size_t_);
  header_->Print("<double>%d</double>\n",                         control_.typeSizes_.sizeof_double_);
  header_->Print("<BDat>%d</BDat>\n",                             control_.typeSizes_.sizeof_BDat_);
  header_->Print("<BCoefDeg>%d</BCoefDeg>\n",                     control_.typeSizes_.sizeof_BCoefDeg_);
  header_->Print("<BGrammarId>%d</BGrammarId>\n",                 control_.typeSizes_.sizeof_BGrammarId_);
  header_->Print("</typeSizes>\n");
  header_->Print("</control>\n");

  header_->Print("<options>\n");

  header_->Print("<oisConfig>\n");
  header_->Print("<buildHierarchy>%d</buildHierarchy>\n",  (short)options_.oisConfig_.buildHierarchy_);
  header_->Print("</oisConfig>\n");

  header_->Print("<compression>\n");
  header_->Print("<serialization>\n");
  header_->Print("<engine>%s</engine>\n",                         SerialEngine(options_.compressor_.serialization_.engine_).String());
  header_->Print("<level>%ld</level>\n",                          options_.compressor_.serialization_.level_);
  header_->Print("<minSizeCmprs>%ld</minSizeCmprs>\n",            options_.compressor_.serialization_.minSizeCmprs_);
  header_->Print("</serialization>\n");
  header_->Print("<fileManager>\n");
  header_->Print("<engine>%s</engine>\n",                         ArchiveEngine(options_.compressor_.fileManager_.engine_).String());
  header_->Print("<level>%ld</level>\n",                          options_.compressor_.fileManager_.level_);
  header_->Print("</fileManager>\n");
  header_->Print("</compression>\n");

  header_->Print("<tolSourceSearchPaths>\n");
  header_->Print("<entries>%d</entries>\n", options_.tolSourceSearchPaths_.Size());
  for(n=0; n<options_.tolSourceSearchPaths_.Size(); n++)
  {
    options_.tolSourceSearchPaths_[n].value_ = GetStandardAbsolutePath(options_.tolSourceSearchPaths_[n].value_);
    header_->Print("<root_%d><alias>%s</alias><path>%s</path></root_%d>\n", 
            n+1,
            options_.tolSourceSearchPaths_[n].alias_.String(),
            options_.tolSourceSearchPaths_[n].value_.String(),
            n+1);
  }
  header_->Print("</tolSourceSearchPaths>\n");
  header_->Print("</options>\n");

  header_->Print("<address>\n");
  header_->Print("<concept>%s</concept>\n",   address_.concept_ .String());
  header_->Print("<version>%s</version>\n",   address_.version_ .String());
  header_->Print("<node>%s</node>\n",         address_.node_    .String());
  header_->Print("</address>\n");

  header_->Print("<doc>\n");

  header_->Print("<name>%s</name>\n",                 doc_.name_         .String());
  header_->Print("<category>%s</category>\n",         doc_.category_     .String());
  header_->Print("<subject>%s</subject>\n",           doc_.subject_      .String());
  header_->Print("<keys>%s</keys>\n",                 doc_.keys_         .String());
  header_->Print("<abstract>%s</abstract>\n",         doc_.abstract_     .String());
  header_->Print("<authors>%s</authors>\n",           doc_.authors_      .String());
  header_->Print("<distribution>%s</distribution>\n", doc_.distribution_ .String());
  header_->Print("<repository>%s</repository>\n",     doc_.repository_   .String());
  header_->Print("</doc>\n");

  header_->Print("<TOLEnvironment>\n");
  if(!doc_.category_.BeginWith("Code."))
  {
    header_->Print("<entries>%0</entries>\n");
  }
  else
  {
    header_->Print("<entries>%ld</entries>\n", tolEnvironment_.Size());
    for(n=0; n<tolEnvironment_.Size(); n++)
    {
      BText value = "";
      if(tolEnvironment_[n]->Grammar()==GraReal())
      {
        value += Dat(tolEnvironment_[n]).Format("%.15lg");
      }
      else if(tolEnvironment_[n]->Grammar()==GraDate())
      {
        value += Date(tolEnvironment_[n]);
      }
      else if(tolEnvironment_[n]->Grammar()==GraText())
      {
        value += Text(tolEnvironment_[n]);
      }
      header_->Print("<var_%d><type>%s</type><name>%s</name><value>%s</value></var_%d>\n", 
              n+1,
              tolEnvironment_[n]->Grammar()->Name().String(), 
              tolEnvironment_[n]->Name().String(),
              value.String(),
              n+1);
    }
  }
  header_->Print("</TOLEnvironment>\n");

  header_->Print("<TOLDependencies>\n");
  if(!doc_.category_.BeginWith("Code."))
  {
    header_->Print("<entries>0</entries>\n");
  }
  else
  {
    header_->Print("<entries>%ld</entries>\n",allSourcePath_.Size());
    if(options_.compressor_.fileManager_.engine_==BAE_ZIPARC_)
    {
      RemoveSource();
    }
    for(n=0; n<allSourcePath_.Size(); n++)
    {
      BText path = allSourcePath_[n];
      path.Replace("\\","/");
      BDir dir(path);
      BDate fileTime = BDate::Unknown();
      BINT64 fileSize = -1;
      if(dir.Exist())
      {
        fileTime = BTimer::TimeToDate(dir.Time());
        fileSize = dir.Bytes();
        if(options_.compressor_.fileManager_.engine_==BAE_ZIPARC_)
        {
          WriteSource(n,(int)fileSize,path);
        }
      }
      for(m=0; m<options_.tolSourceSearchPaths_.Size(); m++)
      {
        if(options_.tolSourceSearchPaths_[m].Code(path,true))
        {
          break;
        }
      }
      header_->Print("<file_%d><path>%s</path><release>%s</release><size>%"_LLD64_"</size></file_%d>\n", 
                     n+1,
                     path.String(), 
                     dateFmt_.DateToText(fileTime).String(),
                     fileSize,
                     n+1);
    }
  }
  header_->Print("</TOLDependencies>\n");

  header_->Print("<RequiredPackages>\n");
  header_->Print("<entries>%ld</entries>\n",packages_.Size());
  for(n=0; n<packages_.Size(); n++)
  {
    BText name = packages_[n];
    header_->Print("<package_%d>%s</package_%d>\n", 
                   n+1,
                   name.String(), 
                   n+1);
  }
  header_->Print("</RequiredPackages>\n");

  DoStat();

  header_->Print("<statistics>\n");
  header_->Print("<buildingSeconds>%.3lf</buildingSeconds>\n", stat_.buildingTime_);
  header_->Print("<filestats>\n");
  for(n=1; n<allFiles_.Size(); n++)
  {
    if(allFiles_[n])
    {				
      header_->Print("<%s><size>%"_LLD64_"</size><entries>%ld</entries></%s>\n",
                    allFiles_[n]->Title().String(), 
                    stat_.fileStat_[n].bytes_, 
                    stat_.fileStat_[n].entries_, 
                    allFiles_[n]->Title().String());
    }
  }
  header_->Print("</filestats>\n");
  header_->Print("</statistics>\n");
  header_->Print("</header>\n");
  header_->Flush();
  return(true);
}

//--------------------------------------------------------------------
// OIS reading XML functions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  bool BOisLoader::XMLError()
//--------------------------------------------------------------------
{
  return(Error(I2("Bad header XML format has been found trying to read tag",
                  "Error en el formato de la cabecera XML al intentar leer la etiqueta")+
               currentXMLTag_));  
}


#define XMLEnsure(doit) if(!doit) { return(XMLError()); }


//--------------------------------------------------------------------
  bool BOisLoader::XMLGoToNextTag()
//--------------------------------------------------------------------
{
  char ch;
  do
  {
    XMLEnsure(!header_->Eof());
    ch = header_->GetChar();
  } while(ch!='<');
  return(true);
}

//--------------------------------------------------------------------
  bool BOisLoader::XMLGetNextTagTitle(char* title, const char* ensure, bool goToNextTag)
//--------------------------------------------------------------------
{
  currentXMLTag_ = BText("<")+ensure+">";
  char ch;
  int n=0;
  if(goToNextTag) { XMLEnsure(XMLGoToNextTag()); }
  do
  {
    XMLEnsure(!header_->Eof());
    ch = header_->GetChar();
	  assert(n<xmlParam_.maxXmlStrLen_);
    title[n++] = (ch=='>')?'\0':ch;
  } 
  while(ch!='>');
  return(!ensure ||!ensure[0] || !strcmp(ensure,title));
}

//--------------------------------------------------------------------
  bool BOisLoader::XMLEnsureEndTag(const char* title, bool goToNextTag)
//--------------------------------------------------------------------
{
  char ch;
  currentXMLTag_ = BText("</")+title+">";
  BText endTag_(xmlParam_.maxXmlStrLen_);
  if(goToNextTag) { XMLEnsure(XMLGoToNextTag()); }
  XMLEnsure(!header_->Eof());
  ch = header_->GetChar();
  if(ch!='/') { return(false); }
  return(XMLGetNextTagTitle(endTag_.Buffer(),title,false));
}

//--------------------------------------------------------------------
  bool BOisLoader::XMLGetNextTagValue(char* title, char* value, const char* ensure)
//--------------------------------------------------------------------
{
  char ch;
  int n=0;
  if(ensure) { XMLEnsure(XMLGetNextTagTitle(title,ensure)); }
  do
  {
    XMLEnsure(!header_->Eof());
    ch = header_->GetChar();
	  assert(n<xmlParam_.maxXmlStrLen_);
    value[n++] = (ch=='<')?'\0':ch;
  } 
  while(ch!='<');
//Std(BText("\nBOisLoader::XMLGetNextTagValue('")+title+"','"+value+"');");
  return(XMLEnsureEndTag(title,false));
}


//--------------------------------------------------------------------
  bool BOisLoader::XMLReadHeader()
//--------------------------------------------------------------------
{
  int n, m;
  int auxInt;
  BText tag__  (xmlParam_.maxXmlStrLen_);
  BText value__(xmlParam_.maxXmlStrLen_);
  char* tag_   = tag__  .Buffer();
  char* value_ = value__.Buffer();
  XMLEnsure(XMLGetNextTagTitle(tag_,NULL));
  XMLEnsure(XMLGetNextTagTitle(tag_, "header"));

  XMLEnsure(XMLGetNextTagTitle(tag_, NULL));
  if(tag__!="xmlParam") 
  {
    return(Error(I2("Sorry! Cannot read a OIS image with too old format",
                    "¡Lo siento! No se puede leer una imagen OIS con formato demasiado viejo")+
                 " (01.0?<=01.07<02.01) "+
                 I2("because it's incompatible with current version (",
                    "porque es incompatible con la versión actual (")+ 
                 OIS_VERSION+")"));
  }
  //Reading section <xmlParam>
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "maxXmlStrLen"  )); sscanf(value_,"%d",&xmlParam_.maxXmlStrLen_  );
  XMLEnsure(XMLEnsureEndTag   ("xmlParam"));
  //Reading section <control>
  XMLEnsure(XMLGetNextTagTitle(tag_, "control"));
  XMLEnsure(XMLGetNextTagTitle(tag_, "oisEngine"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "version")); control_.oisEngine_.oisVersion_ = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "release")); control_.oisEngine_.release_ = dateFmt_.TextToDate(value_);
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "curDir" )); control_.oisEngine_.curDir_  = value_;
  XMLEnsure(XMLEnsureEndTag   ("oisEngine"));
  XMLEnsure(XMLGetNextTagTitle(tag_, "machine"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "hostName")); control_.machine_.hostName_ = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "isLittleEndian")); sscanf(value_,"%d",&auxInt); control_.machine_.isLittleEndian_ = auxInt!=0;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "osName"        )); control_.machine_.osName_       = value_;
  XMLEnsure(XMLEnsureEndTag   ("machine"));
  XMLEnsure(XMLGetNextTagTitle(tag_, "tolEngine"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "cppCompiler")); control_.tolEngine_.cppCompiler_  = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "tolVersion" )); control_.tolEngine_.tolVersion_   = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "tolRelease" )); control_.tolEngine_.tolRelease_   = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "tolUserLang")); control_.tolEngine_.tolUserLang_  = value_;
  if(control_.oisEngine_.oisVersion_>="02.17")
  {
    XMLEnsure(XMLGetNextTagTitle(tag_, "globalRequiredPackages"));
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "entries")); sscanf(value_,"%d",&m); 
    control_.tolEngine_.globalPackages_.AllocBuffer(m);
    for(n=0; n<m; n++)
    {
      BText fn = BText("package_")+(n+1);
      XMLEnsure(XMLGetNextTagValue(tag_, value_, fn)); control_.tolEngine_.globalPackages_[n] = value_;
    }
    XMLEnsure(XMLEnsureEndTag("globalRequiredPackages"));
  }

  XMLEnsure(XMLEnsureEndTag   ("tolEngine"));
  if(control_.oisEngine_.oisVersion_>="02.02")
  {
    XMLEnsure(XMLGetNextTagTitle(tag_, "typeSizes"));
    if(control_.oisEngine_.oisVersion_>="02.16") {
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "pointer"));    sscanf(value_,"%hd",&control_.typeSizes_.sizeof_pointer_); }    
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "char"));       sscanf(value_,"%hd",&control_.typeSizes_.sizeof_char_);
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "short"));      sscanf(value_,"%hd",&control_.typeSizes_.sizeof_short_);
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "int"));        sscanf(value_,"%hd",&control_.typeSizes_.sizeof_int_);
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "BINT64"));     sscanf(value_,"%hd",&control_.typeSizes_.sizeof_BINT64_);
    if(control_.oisEngine_.oisVersion_>="02.16") {
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "size_t"));     sscanf(value_,"%hd",&control_.typeSizes_.sizeof_size_t_); }    
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "double"));     sscanf(value_,"%hd",&control_.typeSizes_.sizeof_double_);
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "BDat"));       sscanf(value_,"%hd",&control_.typeSizes_.sizeof_BDat_);
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "BCoefDeg"));   sscanf(value_,"%hd",&control_.typeSizes_.sizeof_BCoefDeg_);
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "BGrammarId")); sscanf(value_,"%hd",&control_.typeSizes_.sizeof_BGrammarId_);
    XMLEnsure(XMLEnsureEndTag   ("typeSizes"));
  }
  else
  {
    control_.typeSizes_.sizeof_pointer_    =  4;
    control_.typeSizes_.sizeof_char_       =  1;
    control_.typeSizes_.sizeof_short_      =  2;
    control_.typeSizes_.sizeof_int_        =  4;
    control_.typeSizes_.sizeof_BINT64_     =  8;
    control_.typeSizes_.sizeof_size_t_     =  4;
    control_.typeSizes_.sizeof_double_     =  8;
    control_.typeSizes_.sizeof_BDat_       = 12;
    control_.typeSizes_.sizeof_BCoefDeg_   = 16;
    control_.typeSizes_.sizeof_BGrammarId_ =  4;
  }
  XMLEnsure(XMLEnsureEndTag   ("control"));
  //Reading section <options>
  XMLEnsure(XMLGetNextTagTitle(tag_, "options")); 
  XMLEnsure(XMLGetNextTagTitle(tag_, "oisConfig"));
  unsigned short int usintAux;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "buildHierarchy")); sscanf(value_,"%hu",&usintAux);
  options_.oisConfig_.buildHierarchy_ = usintAux!=0;
  XMLEnsure(XMLEnsureEndTag   ("oisConfig"));
  XMLEnsure(XMLGetNextTagTitle(tag_, "compression"));
  XMLEnsure(XMLGetNextTagTitle(tag_, "serialization"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "engine"      )); options_.compressor_.serialization_.engine_  = SerialEngine(value_);
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "level"       )); sscanf(value_,"%d",&auxInt); options_.compressor_.serialization_.level_        = auxInt;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "minSizeCmprs")); sscanf(value_,"%d",&auxInt); options_.compressor_.serialization_.minSizeCmprs_ = auxInt;
  XMLEnsure(XMLEnsureEndTag   ("serialization"));
  XMLEnsure(XMLGetNextTagTitle(tag_, "fileManager"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "engine")); options_.compressor_.fileManager_.engine_  = ArchiveEngine(value_);
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "level" )); sscanf(value_,"%d",&auxInt); options_.compressor_.fileManager_.level_ = auxInt;
  XMLEnsure(XMLEnsureEndTag   ("fileManager"));
  XMLEnsure(XMLEnsureEndTag   ("compression"));
  XMLEnsure(XMLGetNextTagTitle(tag_, "tolSourceSearchPaths"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "entries")); sscanf(value_,"%d",&m); options_.tolSourceSearchPaths_.AllocBuffer(m);
  for(n=0; n<options_.tolSourceSearchPaths_.Size(); n++)
  {
    BText vn = BText("root_")+(n+1);  
    XMLEnsure(XMLGetNextTagTitle(tag_, vn));
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "alias")); options_.tolSourceSearchPaths_[n].alias_ = value_;
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "path" )); options_.tolSourceSearchPaths_[n].value_ = value_;
    XMLEnsure(XMLEnsureEndTag   (vn)); 
  }
  XMLEnsure(XMLEnsureEndTag("tolSourceSearchPaths"));
  XMLEnsure(XMLEnsureEndTag("options"));
  if(control_.oisEngine_.oisVersion_<"02.01")
  {
    return(Error(I2("Sorry! Cannot read a OIS image with too old format",
                    "¡Lo siento! No se puede leer una imagen OIS con formato demasiado viejo")+
                 " ("+control_.oisEngine_.oisVersion_+"<02.01) "+
                 I2("because it's incompatible with current version (",
                    "porque es incompatible con la versión actual (")+ 
                 OIS_VERSION+")"));
  }
  //Reading section <address>
  XMLEnsure(XMLGetNextTagTitle(tag_, "address"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "concept")); address_.concept_ = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "version")); address_.version_ = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "node"   )); address_.node_    = value_;
  XMLEnsure(XMLEnsureEndTag   ("address"));
  //Reading section <doc>
  XMLEnsure(XMLGetNextTagTitle(tag_, "doc"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "name"));          doc_.name_         = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "category"));      doc_.category_     = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "subject"));       doc_.subject_      = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "keys"));          doc_.keys_         = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "abstract"));      doc_.abstract_     = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "authors" ));      doc_.authors_      = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "distribution" )); doc_.distribution_ = value_;
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "repository" ));   doc_.repository_   = value_;
  XMLEnsure(XMLEnsureEndTag   ("doc"));
  //Reading section <TOLEnvironment>
  XMLEnsure(XMLGetNextTagTitle(tag_, "TOLEnvironment"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "entries")); sscanf(value_,"%d",&m); 
  for(n=0; n<m; n++)
  {
    BText type, name, vn = BText("var_")+(n+1);
    XMLEnsure(XMLGetNextTagTitle(tag_, vn.String()));
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "type" )); type = value_;
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "name" )); name = value_;
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "value"));                
    XMLEnsure(XMLEnsureEndTag(vn.String()));
    BGrammar* gra = BGrammar::FindByName(type);
    BSyntaxObject* var = NULL;
    if(gra==GraReal())
    {
      BUserDat* x = (BUserDat*)gra->FindVariable(name);
      if(x)
      {
        var = x;
        BDat value; value.PutValue(value_);
        if(!x->IsConstant()) { x->PutContens(value); }
      }
    }
    else if(gra==GraDate())
    {
      BUserDate* x = (BUserDate*)gra->FindVariable(name);
      if(x)
      {
        var = x;
        BDate value; value.PutDate(value_);
        if(!x->IsConstant()) { x->PutContens(value); }
      }
    }
    else if(gra==GraText())
    {
      BUserText* x = (BUserText*)gra->FindVariable(name);
      if(x)
      {
        var = x;
        BText value = value_;
        if(!x->IsConstant()) { x->PutContens(value); }
      }
    }
    if(!var) 
    { 
      return(Error(I2("Unknown TOLEnvironment variable in ",
                      "Variable de entorno TOL (TOLEnvironment) desconocida en ")+
                      "header.xml "+vn+" "+type+" "+name )); 
    }
  }
  XMLEnsure(XMLEnsureEndTag("TOLEnvironment"));
  //Reading section <TOLDependencies>
  XMLEnsure(XMLGetNextTagTitle(tag_, "TOLDependencies"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "entries")); sscanf(value_,"%d",&m); tolSources_.AllocBuffer(m);
  obsoleteSource_ = false;
  if(control_.oisEngine_.oisVersion_<oisWriteVersion_)
  {
    obsoleteSource_ = true;
  }
  for(n=0; n<m; n++)
  {
    BText fn = BText("file_")+(n+1);
    XMLEnsure(XMLGetNextTagTitle(tag_, fn.String()));
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "path"   )); tolSources_[n].path_ = value_;
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "release")); tolSources_[n].release_ = dateFmt_.TextToDate(value_);
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "size"   )); 
    sscanf(value_,"%"_LLD64_,&tolSources_[n].bytes_  ); 
    XMLEnsure(XMLEnsureEndTag   (fn.String()));
    
    bool checkSource = checkSourceExists_ || checkSourceSize_ || checkSourceDate_;
    if(checkSource)
    {
      BText& path = tolSources_[n].path_;
      for(int k=0; k<options_.tolSourceSearchPaths_.Size(); k++)
      {
        if(options_.tolSourceSearchPaths_[k].Decode(path,true))
        {
          break;
        }
      }
      if(!obsoleteSource_) { 
       obsoleteSource_ = !CheckUpdatedSource(n, path); }
    }
  }
  XMLEnsure(XMLEnsureEndTag("TOLDependencies"));
  if(control_.oisEngine_.oisVersion_>="02.15")
  {
    XMLEnsure(XMLGetNextTagTitle(tag_, "RequiredPackages"));
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "entries")); sscanf(value_,"%d",&m); 
    packages_.AllocBuffer(m);
    for(n=0; n<m; n++)
    {
      BText fn = BText("package_")+(n+1);
      XMLEnsure(XMLGetNextTagValue(tag_, value_, fn)); packages_[n] = value_;
    }
    XMLEnsure(XMLEnsureEndTag("RequiredPackages"));
  }
  //Reading section <statistics>
  XMLEnsure(XMLGetNextTagTitle(tag_, "statistics"));
  XMLEnsure(XMLGetNextTagValue(tag_, value_, "buildingSeconds")); sscanf(value_,"%lf",&stat_.buildingTime_);
  XMLEnsure(XMLGetNextTagTitle(tag_, "filestats"));
  int numFiles = allFiles_.Size();
  if(control_.oisEngine_.oisVersion_<"02.07.b")
  {
    numFiles=14;
  }
  BArray<int> allFilesOrder(numFiles);
  for(n=0; n<numFiles; n++) { if(allFiles_[n]) { allFilesOrder[n] = n; } }
  for(m=1; m<numFiles; m++)
  {
    n = allFilesOrder[m];
    if(n<0) { continue; }
    XMLEnsure(XMLGetNextTagTitle(tag_, allFiles_[n]->Title().String()));
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "size"   ));
    sscanf(value_,"%"_LLD64_, &stat_.fileStat_[n].bytes_  );
    XMLEnsure(XMLGetNextTagValue(tag_, value_, "entries")); sscanf(value_,"%d",   &stat_.fileStat_[n].entries_);
    XMLEnsure(XMLEnsureEndTag   (allFiles_[n]->Title().String()));
  }
  XMLEnsure(XMLEnsureEndTag("filestats"));
  XMLEnsure(XMLEnsureEndTag("statistics"));
  XMLEnsure(XMLEnsureEndTag("header"));
  return(CheckFileSizes());
}

