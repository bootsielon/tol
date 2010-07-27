/* oisapitol.cpp: main and init functions of GNU/TOL language.

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

#include <tol/tol_oiscreator.h>
#include <tol/tol_oisloader.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bstruct.h>

BTraceInit("oisapitol.cpp");


//--------------------------------------------------------------------
// OIS API for TOL
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  static BText& GetOisDefBSE()
//--------------------------------------------------------------------
{
  static BText txt;
  txt = BOis::SerialEngine();
  return(txt);
};

//--------------------------------------------------------------------
  static BText& GetOisDefBAE()
//--------------------------------------------------------------------
{
  static BText txt;
  txt = BOis::ArchiveEngine();
  return(txt);
};

//--------------------------------------------------------------------
   bool BOis::Initialize()
//--------------------------------------------------------------------
{
//If addressStr_ is not null  BOis::Initialize() has been called already.
  if(addressStr_) { return(false); }
  oisCurrentVersion_ = OIS_VERSION;

  Std(I2("OIS: implemented version: ",
         "OIS: versión implementada: ") << oisCurrentVersion_);
  oisDefRoot_ = BSys::GetEnv("Ois.DefRoot");
  if(!oisDefRoot_.HasName())
  {
    oisDefRoot_ = BSys::GetEnv("Ois_DefRoot");
    if(!oisDefRoot_.HasName())
    {
      oisDefRoot_ = BSys::TolAppData()+"OIS/";
    }
  }

  dateFmt_ = BDateFormat("%c%Y-%m-%d %h:%i:%s");
//ensures that __BIG_ENDIAN macro is well defined.
  long _TestEndian = 1;
  int	_aux = *(char*)&_TestEndian;
#if _WINDOWS || (__BYTE_ORDER == __LITTLE_ENDIAN)
  assert(_aux);
#else
  assert(!_aux);
#endif

//Creates TOL structures used with OIS
  addressStr_ = NewStructSymbol("@Ois.AddressStr",
	                              "Text:Concept,"
	                              "Text:Version,"
	                              "Text:Node");

  docStr_ = NewStructSymbol("@Ois.DocStr",
	                          "Text:Name,"
	                          "Text:Category,"
	                          "Text:Subject,"
	                          "Text:Keys,"
	                          "Text:Abstract,"
	                          "Text:Authors,"
	                          "Text:Distribution,"
	                          "Text:Repository");

  BSystemText* oisVersion_ = new BSystemText
  (
    "Ois.Version",	 
    OIS_VERSION,
	  I2("OIS version identifier",
	     "Identificador de la versión de OIS")
  );
  BParamText* OIS_DefRoot_ = new BParamText
	(
    "Ois.DefRoot", oisDefRoot_,
    I2("Default OIS root path used in Ois functions that will "
       "replace in paths begining with alias \"ois:/\" by this text\n"
       "If the default value is not desirable, can be changed using := "
       "within TOL, but it's not advisable except in a file initialization "
       "project that can be ensured to be always running first than any "
       "other one. It is also possible to create an environment variable "
       "operating system with the same name Ois.DefRoot",
       "Directorio raíz de OIS por defecto usado en las funciones Ois "
       "que reemplazarán en los caminos que empiezen por el alias "
       "\"ois:/\" por este texto. Si el valor por defecto no es "
       "conveniente, se puede cambiar con := dentro de TOL "
       "aunque no es muy recomendable hacerlo salvo en un archivo "
       "de inicialización de proyecto que se pueda asegurar que se "
       "va a correr siempre. También es posible crear una variable de "
       "entorno del sistema operativo con el mismo nombre Ois.DefRoot")
  );
  defDoc_ = (BUserSet*) GraSet()->EvaluateExpr(
    "Set Ois.DefDoc = @Ois.DocStr\n"
    "(\n"
    "  {Name = \"\"},\n"
    "  {Category=\"\"},\n"
    "  {Subject=\"\"},\n"
    "  {Keys=\"\"},\n"
    "  {Abstract=\"\"},\n"
    "  {Authors=\"\"},\n"
    "  {Distribution=\"\"},\n"
    "  {Repository=\"\"}\n"
    ");\n");


  BSystemDat* OIS_Load_Mode_ShowHierarchy_ = new BSystemDat
  (
    "Ois.Load.Mode.ShowHierarchy",	 
    OIS_Load_Mode_ShowHierarchy,
	  I2("When is passed to Ois.Load just header information will be loaded from OIS directory "
       "and objects hierarchy will be displayed by standard output.",
	     "Indica a la función Ois.Load que ha de cargar sólo el contenido de la cabecera OIS "
       "y que la jerarquí de objetos OIS se muestre por la salida estándar.")
  );
  BSystemDat* OIS_Load_Mode_Header_ = new BSystemDat
  (
    "Ois.Load.Mode.Header",	 
    OIS_Load_Mode_Header,
	  I2("When is passed to Ois.Load just header information will be loaded from OIS directory",
	     "Indica a la función Ois.Load que ha de cargar sólo el contenido de la cabecera OIS")
  );
  BSystemDat* OIS_Load_Mode_Data_ = new BSystemDat
  (
    "Ois.Load.Mode.Data",	 
    OIS_Load_Mode_Data,
	  I2("When is passed to Ois.Load TOL objects definition and data will be loaded from OIS directory",
	     "Indica a la función Ois.Load que ha de cargar las definiciones y los datos de los objetos TOL contenidos en el directorio OIS")
  );
  
  BParamText* OIS_def_BSE_ = new BParamText
  (
    "Ois.DefaultSerialEngine",
    GetOisDefBSE(),
    I2("The current default value of OIS images serialization engine.\n"
       "Allowed values of Ois.DefaultSerialEngine are:\n"
       " \"_NONE_\"  : OIS files will be serialized in a plain file.\n"
       " \"_BZIP2_\" : OIS files will be serialized in a file with independently "
       "compressed strings using open-source library BZIP, to allow indexes and "
       "accessing to internal items of the file.\n"
       "In order to change this value you can use function "
       "Ois.PutDefaultSerialEngine.",
       "El valor por defecto del motor de serialización de imágenes OIS.\n"
       "Los valores permitidos para Ois.DefaultSerialEngine son:\n"
       " \"_NONE_\"   : los ficheros OIS se archivarán en un directorio plano.\n"
       " \"_BZIP2_\" : los ficheros OIS se archivarán en un fichero con cadenas "
       "de texto comprimidas independientemente unas de otras mediante la "
       "librería de código abierto BZIP2, para permitir la indexación y el "
       "acceso a puntos internos del fichero.\n"
       "Para cambiar el valor úsese la función Ois.PutDefaultSerialEngine")
  );
  OIS_def_BSE_->PutConstant();

  BParamText* OIS_def_BAE_ = new BParamText
  (
    "Ois.DefaultArchiveEngine",
    GetOisDefBAE(),
    I2("The current default value of OIS images archive engine.\n"
       "Allowed values of Ois.DefaultArchiveEngine are:\n"
       " \"_NONE_\"   : OIS files will be archived in a plain directory.\n"
       " \"_ZIPARC_\" : OIS files will be archived in a ZIP file compressed "
       "using open-source library ZipArchive and with extension .oza that you "
       "can open with any any format ZIP compatible archiver\n"
       "In order to change this value you can use function "
       "Ois.PutDefaultArchiveEngine.",
       "El valor por defecto del motor de archivador de imágenes OIS.\n"
       "Los valores permitidos para Ois.DefaultArchiveEngine son:\n"
       " \"_NONE_\"   : los ficheros OIS se archivarán en un directorio plano.\n"
       " \"_ZIPARC_\" : los ficheros OIS se archivarán en un fichero ZIP "
       "comprimido mediante la librería de código abierto ZipArchive, y con "
       "la extensión .oza, que se pued eabrir con cualquier archivador "
       "compatible con el formato ZIP.\n"
       "Para cambiar el valor úsese la función Ois.PutDefaultArchiveEngine")
  );
  OIS_def_BAE_->PutConstant();

  loadModeOptions_.AllocBuffer(3);
  loadModeOptions_[0] = OIS_Load_Mode_ShowHierarchy_;
  loadModeOptions_[1] = OIS_Load_Mode_Header_;
  loadModeOptions_[2] = OIS_Load_Mode_Data_;

  return(true);
}

//--------------------------------------------------------------------
   BText BOis::AutoPath(const BText& path)
//--------------------------------------------------------------------
{
  BText aux = path;
  aux.Replace("\\","/");
  if(aux.BeginWith("ois:/"))
  {
    int len = aux.Length();
    aux = oisDefRoot_+aux.SubString(5,len);
  }
  else
  {
    aux = GetStandardAbsolutePath(aux);
  }
  aux.Replace("\\","/");
  return(aux);
}

//--------------------------------------------------------------------
   BText BOis::PlainPath(const BText& path)
//--------------------------------------------------------------------
{
  BText aux = path;
  aux.Replace(":","_");
  aux.Replace("\\","/");
//aux.Replace("\\","_");
//aux.Replace("/", "_");
  return(aux);
}

//--------------------------------------------------------------------
   BText BOis::GetModulePath(const BText& tolFile)
//--------------------------------------------------------------------
{
  BText aux = GetFilePath(GetStandardAbsolutePath(tolFile))+
              GetFilePrefix(tolFile)+ArchiveExtension();
  aux = PlainPath(aux);
  aux = Replace(oisDefRoot_,"\\","/")+"module/"+aux;
  return(aux);
}

//--------------------------------------------------------------------
   bool BOis::RemoveModule(const BText& tolFile)
//--------------------------------------------------------------------
{
  BOisLoader ois;
  ois.SetModulePath(tolFile);
  bool ok = ois.Remove();
  ois.Close();
  return(ok);
}  

//--------------------------------------------------------------------
   BDate BOis::GetModuleTime(const BText& tolFile)
//--------------------------------------------------------------------
{
  BOisLoader ois;
  ois.SetModulePath(tolFile);
  BDate time = ois.Time();
  ois.Close();
  return(time);
}  

//--------------------------------------------------------------------
  bool BOisCreator::BuilTolEnv()
//--------------------------------------------------------------------
{
  tolEnvironment_.AllocBuffer(32);
  tolEnvironment_.AllocBuffer(0);
  tolEnvironment_.AddUniqueSorted(GraDate()->FindVariable("DefFirst"),             CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraDate()->FindVariable("DefLast" ),             CompareAddress); 
  tolEnvironment_.AddUniqueSorted(GraText()->FindVariable("JacobianMethod"),       CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("CGMaxIter"),            CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("DiffDist"),             CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("DoDiagnostics"),        CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("DoKernel"),             CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("DoStatistics"),         CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("DoUnparseNodes"),       CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("GlobalizeSeries"),      CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("MarqFactor"),           CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("MaxIter"),              CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("MinOutlierLikelyhood"), CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("NullInitResiduals"),    CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("RelativeTolerance"),    CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("StrassenCutoff"),       CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("Tolerance"),            CompareAddress);
  tolEnvironment_.AddUniqueSorted(GraReal()->FindVariable("MarqLinMet"),           CompareAddress);
  return(true);
}

//--------------------------------------------------------------------
  static void WarWrongType(const BText& fun, 
                           const BText& arg, 
                           const BText& type,
                           const BSet&  set,
                           int pos)

//--------------------------------------------------------------------
{
  Warning(I2(BText("OIS: Argument ")+arg+" of "+fun+" function must be of type ",
             BText("OIS: El argumento ")+arg+" de la función "+fun+" debe ser de tipo ")+
          type+
          I2("Skipping invalid argument ","No se tendrá en cuenta el argumento inválido ")+
             set[pos]->Grammar()->Name()+" "+set[pos]->Name());
}

//--------------------------------------------------------------------
  static void WarUnkArg(const BText& fun, 
                        const BText& arg, 
                        const BText& opt,
                        const BSet&  set,
                        int pos)

//--------------------------------------------------------------------
{
  Warning(I2(BText("OIS: Argument ")+arg+" of "+fun+" function must have just one or more of these objects: ",
             BText("OIS: El argumento ")+arg+" de la función "+fun+" sólo puede contener uno o más de los siguientes objetos: ")+
          opt+" "+
          I2("Skipping invalid argument ","No se tendrá en cuenta el argumento inválido ")+
             set[pos]->Grammar()->Name()+" "+set[pos]->Name());
}

//--------------------------------------------------------------------
  void BOis::GetTolSourceSearchPaths(const BText& fun, const BSet& options, int opt)
//--------------------------------------------------------------------
{
  int k;
  BSet& tolSourceSearchPaths = Set(options[opt]);
  options_.tolSourceSearchPaths_.ReallocBuffer(tolSourceSearchPaths.Card());
  for(k=1; k<=tolSourceSearchPaths.Card(); k++)
  {
    if(tolSourceSearchPaths[k]->Grammar()==GraText())
    {
      options_.tolSourceSearchPaths_[k-1].alias_ = tolSourceSearchPaths[k]->Name();
      options_.tolSourceSearchPaths_[k-1].value_ = Text(tolSourceSearchPaths[k]);
    }
    else
    {
      WarWrongType(fun,tolSourceSearchPaths[k]->Name(),"Text",tolSourceSearchPaths,k);
    }
  }
}

//--------------------------------------------------------------------
  void BOisCreator::SetOptions(const BSet* options__)
//--------------------------------------------------------------------
{
  options_.oisConfig_.buildHierarchy_ = false;
  options_.compressor_.serialization_.engine_       = BOis::def_BSE_; 
  options_.compressor_.serialization_.level_        = BOis::def_CLv_;
  options_.compressor_.serialization_.minSizeCmprs_ = OIS_MinStreamSizeToCompress;
  options_.compressor_.fileManager_.engine_         = BOis::def_BAE_;
  options_.compressor_.fileManager_.level_          = BOis::def_CLv_;
  if(options__)
  {
    int opt, m, n;
    const BSet& options = *options__;
    for(opt=1; opt<=options.Card(); opt++)
    {
      if((options[opt]->Grammar()==GraSet())&&(options[opt]->Name()=="xmlParam"))
      {
        BSet& xmlParam = Set(options[opt]);
        for(m=1; m<=xmlParam.Card(); m++)
        {
          if((xmlParam[m]->Grammar()==GraReal())&&(xmlParam[m]->Name()=="maxXmlStrLen"))
          { 
            xmlParam_.maxXmlStrLen_ = (int)Real(xmlParam[m]);
          }
          else
          {
            WarUnkArg("Ois.Create", "options->xmlParam", "Real maxXmlStrLen", xmlParam, m);
          }
        }
      }
      else if((options[opt]->Grammar()==GraSet())&&(options[opt]->Name()=="oisConfig"))
      {
        BSet& oisConfig = Set(options[opt]);
        for(m=1; m<=oisConfig.Card(); m++)
        {
          if((oisConfig[m]->Grammar()==GraReal())&&(oisConfig[m]->Name()=="buildHierarchy"))
          { 
            options_.oisConfig_.buildHierarchy_ = (int)Real(oisConfig[m])!=0;
          }
          else
          {
            WarUnkArg("Ois.Create", "options->oisConfig", "Real buildHierarchy", oisConfig, m);
          }
        }
      }
      else if((options[opt]->Grammar()==GraSet())&&(options[opt]->Name()=="compression"))
      {
        BSet& compression = Set(options[opt]);
        for(m=1; m<=compression.Card(); m++)
        {
          if((compression[m]->Grammar()==GraSet())&&(compression[m]->Name()=="serialization"))
          { 
            BSet& serialization = Set(compression[m]);
            for(n=1; n<=serialization.Card(); n++)
            {
              if((serialization[n]->Grammar()==GraText())&&(serialization[n]->Name()=="engine"))
              { 
                options_.compressor_.serialization_.engine_ = SerialEngine(Text(serialization[n]));
                if(options_.compressor_.serialization_.engine_!=BSE_BZIP2_)
                {
                }
                else
                {
                  options_.compressor_.serialization_.engine_=BOis::BSE_BZIP2_;
                  Warning(I2("OIS: Argument options->compression->serialization->engine of Ois.Create function must be \"_BZIP2_\"",
                             "OIS: El options->compression->serialization->engine de la función Ois.Create ha de ser \"_BZIP2_\""));
                }
              }
              else if((serialization[n]->Grammar()==GraReal())&&(serialization[n]->Name()=="level"))
              { 
                options_.compressor_.serialization_.level_ = (int)Real(serialization[n]);
                static BText wrnMsg_ = 
                  I2("OIS: Argument options->compression->serialization->level of Ois.Create function must be between 1 and 9",
                     "OIS: El argumento compression->serialization->level de la función Ois.Create ha de estar entre 1 y 9");
                if(options_.compressor_.serialization_.level_<1)
                {
                  options_.compressor_.serialization_.level_ = 1;
                  Warning(wrnMsg_+I2(" and will be set to 1"," y se tomará como 1"));
                }
                else if(options_.compressor_.serialization_.level_>9)
                {
                  options_.compressor_.serialization_.level_ = 9;
                  Warning(wrnMsg_+I2(" and will be set to 9"," y se tomará como 9"));
                }
              }
              else if((serialization[n]->Grammar()==GraReal())&&(serialization[n]->Name()=="minSizeCmprs"))
              { 
                options_.compressor_.serialization_.minSizeCmprs_ = (int)Real(serialization[n]);
                if(options_.compressor_.serialization_.minSizeCmprs_<16)
                {
                  options_.compressor_.serialization_.minSizeCmprs_ = 16;
                  Warning(I2("OIS: Argument options->compression->serialization->minSizeCmprs of Ois.Create function must have greater or equal than 16",
                             "OIS: El argumento compression->serialization->minSizeCmprs de la función Ois.Create ha de ser mayor o igual que 16"));
                }
              }
              else
              {
                WarUnkArg("Ois.Create", "options->compression->serialization", "Text engine, Real level, Real minSizeCmprs", serialization, n);
              }
            }
          }
          else
          {
            WarUnkArg("Ois.Create", "options->compression", "Set serialization", compression, m);
          }
        }
      }
      else if((options[opt]->Grammar()==GraSet())&&(options[opt]->Name()=="tolSourceSearchPaths"))
      {
        GetTolSourceSearchPaths("Ois.Create", options, opt);
      }
      else
      {
        WarUnkArg("Ois.Create", "options", "Set xmlParam, Set compression, Set tolSourceSearchPaths", options, opt);
      }
    }
  }
}


static const char* aliasEnglishDescription_ = 
     "  //You can specify any number of alias for absolute paths of your \n"
     "  //TOL source root directories. Then each path_k at the begining of \n"
     "  //any TOL source file path by will be replaced by its correspondent \n"
     "  //alias codified as {&alias_k}\n"
     "  Set tolSourceSearchPaths = \n" 
     "  [[\n"
     "    Text alias_1 =\"path_1\",\n"
     "    ...\n"
     "    Text alias_N =\"path_N\"\n"
     "  ]] \n";

static const char* aliasSpanishDescription_ = 
     "  //Es posible definir un conjunto de alias para los caminos absolutos \n"
     "  //de los directorios raíces de los ficheros de código fuente TOL. \n"
     "  //Entonces en los caminos de todos los ficheros TOL que empiezen por \n"
     "  //dichas raíces path_k éstas se reemplazarán por sus correspondientes \n"
     "  //alias codificadas como {&alias_k} \n"
     "  Set tolSourceSearchPaths = \n" 
     "  [[\n"
     "    Text alias_1 =\"path_1\",\n"
     "    ...\n"
     "    Text alias_N =\"path_N\"\n"
     "  ]] \n";

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatOisCreate);
  DefExtOpr(1, BDatOisCreate, "Ois.Create", 3, 5, "Text Set Set Set Set",
  "(Text root, Set address, Set data [, Set docInfo=Empty, Set options=Empty)",
  I2("Builds an OIS directory containing arbitrary data set.\n"
     "Argument address must have @Ois.AddressStr structure.\n"
     "Empty items of address will be skiped and resto of them will be used "
     "to define the directory where OIS will be created following this organization rule:\n"
     "  root+\"/\"+address->Concept+\"/\"+address->Version+\"/\"+address->Node+\"/\"+address->Name"
     "Optional argument docInfo must have Ois.DocInfoStr structure.\n",
     "Construye un directorio OIS que contiene el conjunto arbitrario data.\n"
     "El argumento address debe tener estructura @Ois.AddressStr.\n"
     "Los elementos vacíos serán ignorados y el resto se usarán para definir "
     "eldirectorio donde se ubicará el OIS siguiendo esta regla organizativa:\n"
     "  root+\"/\"+address->Concept+\"/\"+address->Version+\"/\"+address->Node+\"/\"+address->Name"
     "El argumento opcional docInfo debe tener estructura Ois.DocInfoStr.\n")+
     I2("With argument 'options' you can configure the creating process:\n"
     "Set {options = \n"
     "[[ \n"
     "  //XML header file configuration\n"
     "  Set xmlParam = \n"
     "  [[  \n"
     "    //Maximum size for XML strings\n"
     "    Real maxXmlStrLen = "+OIS_MaxXmlStrLen+" \n"
     "  ]], \n"
     "  //OIS engine configuration\n"
     "  Set oisConfig = \n"
     "  [[  \n"
     "    //If false index hierarchy files will not be built and no partial \n"
     "    //searching nor loading will be available\n"
     "    Real buildHierarchy = True \n"
     "  ]], \n"
     "  //Compression engine configuration\n"
     "  Set compression = \n"
     "  [[ \n"
     "    Set serialization = \n"
     "    [[ \n"
     "    //Default micro-compressor for internal serialization\n"
     "    //For the moment BZIP is the only one implemented\n"
     "      Text engine = \"_BZIP2_\",\n"
     "    //Compression level (1..9) In BZIP is the argument blockSize100k\n"
     "      Real level = "+BOis::def_CLv_+", \n"
     "    //Serial blocks with length shorter than this will not be compressed\n"
     "      Real minSizeCmprs = "+OIS_MinStreamSizeToCompress+" \n"
     "    ]], \n"
     "    Set fileManager = \n"
     "    [[ \n"
     "    //Default macro-compressor for external file manager\n"
     "    //For the moment there are no one implemented\n"
     "      Text engine = \"_ZIPARC_\",\n"
     "    //Compression level (1..9)\n"
     "      Real level = "+BOis::def_CLv_+" \n"
     "    ]] \n"
     "  ]], \n"+
     aliasEnglishDescription_+
     "]]};",
     "Con el argumento 'options' se puede configurar el proceso de creación.\n"
     "Por defecto se cargan los datos completos del OIS lo cual se podría "
     "expresar explícitamente de esta forma:\n"
     "Set {options = \n"
     "[[ \n"
     "  //Configuración del fichero XML de cabecera\n"
     "  Set xmlParam = \n"
     "  [[  \n"
     "    //Máximo tamaño para las cadenas XML\n"
     "    Real maxXmlStrLen_ = "+OIS_MaxXmlStrLen+" \n"
     "  ]], \n"
     "  //Configuración del motor de OIS\n"
     "  Set oisConfig = \n"
     "  [[  \n"
     "    //Si es falso no se crearán los ficheros de índice de la jerarquía y no "
     "    //será posible hacer búsquedas ni carga parcial.\n"
     "    Real buildHierarchy_ = False \n"
     "  ]], \n"
     "  //Configuración del motor de compresión\n"
     "  Set compression = \n"
     "  [[ \n"
     "    Set microSerialization = \n"
     "    [[ \n"
     "    //Compresor por defecto para la serialización interna\n"
     "    //Por el momneto BZIP es el único implementado\n"
     "      Text engine = \"_BZIP2_\",\n"
     "    //Nivel de compresión (1..9) En BZIP corresponde al argumento blockSize100k\n"
     "      Real level = "+BOis::def_CLv_+", \n"
     "    //Los bloques seriales de menor longitud no se comprimen\n"
     "      Real minSizeCmprs = "+OIS_MinStreamSizeToCompress+" \n"
     "    ]], \n"
     "    Set macroFileManager = \n"
     "    [[ \n"
     "    //Compresor por defecto para el manejo externo de archivos\n"
     "    //Por el momento no hay ninguno implementado\n"
     "      Text engine = \"_ZIPARC_\",\n"
     "    //Nivel de compresión (1..9)\n"
     "      Real level = "+BOis::def_CLv_+" \n"
     "    ]] \n"
     "  ]], \n"+
     aliasSpanishDescription_+
     "]]};")+"\n\n"+
  I2("",""),
     BOperClassify::Conversion_);
  void BDatOisCreate::CalcContens()
//--------------------------------------------------------------------
{
  BText&    root  = Text(Arg(1));
  BSet&     add   = Set (Arg(2));
  BUserSet* uData = USet(Arg(3));
  BSet*     doc   = NULL;
  BSet*     opt   = NULL;
  if(Arg(4)) { doc = &Set(Arg(4)); }
  if(Arg(5)) { opt = &Set(Arg(5)); }
  BOisCreator ois;
  ois.Create(root,uData,&add,doc,opt);
//Std(BText("TRACE BDatOisCreate::CalcContens() 1\n"));
  contens_ = ois.Build();
//Std(BText("TRACE BDatOisCreate::CalcContens() 2\n"));
}  
/* */


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatOisCheckIsUpdated);
  DefExtOpr(1, BDatOisCheckIsUpdated, "Ois.CheckIsUpdated", 1, 6, "Text Set Real Real Real Real",
  "(Text root [, Set address=Empty"
              ", Real checkSourceExists=true"
              ", Real checkSourceSize=true"
              ", Real checkSourceDateTime=true"
              ", Real showCheckingTraces=false])",
  I2("Returns true if no difference is found between the TOL code with which "
     "OIS was constructed  and the present state of this code. Concept of "
     "difference can be configured with optional checking arguments.",
     "Devuelve cierto si no se encuentran diferencias entre el código TOL con el "
     "que se construyó el OIS y el estado actual de dicho código."
     "El concepto de diferencia se puede modificar con los arguentos "
     "opcionales de deficnión del chequeo.\n"),
     BOperClassify::Conversion_);
  void BDatOisCheckIsUpdated::CalcContens()
//--------------------------------------------------------------------
{
  BText& root = Text(Arg(1));
  BSet* add  = NULL;
  if(Arg(2)) { add = &Set(Arg(2)); }
#ifdef TRACE_LEVEL
  BText fun = BText("Ois.CheckIsUpdated('")+root+"')";
#endif
  TRACE_SHOW_LOW(fun,"BEGIN");
  BOisLoader ois;
  ois.SetPath(root,add);
  TRACE_SHOW_LOW(fun,"1.1");
  ois.SetCheckSourceExists ((Arg(3))?Real(Arg(3))!=0.0:true);
  ois.SetCheckSourceSize   ((Arg(4))?Real(Arg(4))!=0.0:true);
  ois.SetCheckSourceDate   ((Arg(5))?Real(Arg(5))!=0.0:true);
  ois.SetShowCheckingTraces((Arg(6))?Real(Arg(6))!=0.0:false);
  TRACE_SHOW_LOW(fun,"1.2");
  bool errorWarning        = false;
  bool doShowHierarchy     = false;
  bool checkIsUpdate       = true;
  bool doLoadData          = false;
  int  showHrchyDepth      = -1;
  int  showHrchyMaxChilds  = -1;
  const BSet* partial      = NULL;
  bool ok = ois.Load
  (
    errorWarning,
    doShowHierarchy,
    checkIsUpdate,
    doLoadData,
    showHrchyDepth,
    showHrchyMaxChilds,
    partial
  );
  TRACE_SHOW_LOW(fun,"1.3");
  TRACE_SHOW_MEDIUM(fun,BText("ois.Exists()=")+ois.Exists());
  TRACE_SHOW_MEDIUM(fun,BText("ois.HasLostSource()=")+ois.HasLostSource());
  TRACE_SHOW_MEDIUM(fun,BText("ois.ObsoleteSource()=")+ois.ObsoleteSource());
  TRACE_SHOW_MEDIUM(fun,BText("ok=")+ok);
  TRACE_SHOW_LOW(fun,"1.4");
  if(ok) 
  {
    if(ois.control_.oisEngine_.oisVersion_<BOis::CurrentVersion())
    {
      Warning(I2(BText("OIS: version '")<ois.control_.oisEngine_.oisVersion_+
                 "' of \n  "+ois.Path()+"\n"  
                 " is previous than currently implemented "<<BOis::CurrentVersion(),
                 BText("OIS: La versión ")<<ois.control_.oisEngine_.oisVersion_+
                 " de \n  "+ois.Path()+"\n"  
                 " es anterior a la actualmente implementada '"<<BOis::CurrentVersion()+"'"));
    }
    if(ois.control_.oisEngine_.oisVersion_>BOis::CurrentVersion())
    {
      Error(I2(BText("OIS: version '")>ois.control_.oisEngine_.oisVersion_+
               "' of \n  "+ois.Path()+"\n"  
               " is later than currently implemented "<<BOis::CurrentVersion(),
               BText("OIS: La versión ")<<ois.control_.oisEngine_.oisVersion_+
               " de \n  "+ois.Path()+"\n"  
               " es posterior a la actualmente implementada '"<<BOis::CurrentVersion()+"'"));
      ok=false;
    }
  }
  if(0 && !ok)
  {
    Warning(I2("OIS: module at location ",
               "OIS: El módulo ubicado en ")+ ois.Path()+
            I2(" doesn't exist or isn't updated.",
               " no existe o no está actualizado."));
  }
  contens_ = ok;
  TRACE_SHOW_LOW(fun,"END");

}  


//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetOisLoad);
  DefExtOpr(1, BSetOisLoad, "Ois.Load", 1, 3, "Text Set Set",
    "(Text root [, Set address=Empty, Set options])",
  I2("OIS means Objects Indexed Serialization "
     "Loads information from an OIS directory.\n"
     "  1) Argument address must have @Ois.AddressStr structure or be empty."
     "Empty items of address will be skiped and resto of them will be used "
     "to define the directory where OIS will be created following this rule:\n"
     "  root+\"/\"+address->Concept+\"/\"+address->Version+\"/\"+address->Node\n"
     "You can specify no address argument and pass full path as root argument.\n\n"
     "  2) With argument 'options' you can configure the loading process.\n"
     "By default, it will be loaded full data from OIS, which could be explicity expressed as:\n"
     "Set {options = \n"
     "[[ \n"
     "  Set loadMode = [[Ois.Load.Mode.Data]], \n"
     "  Set showMode = \n"
     "  [[\n"
     "    Real showHrchyDepth = -1 , \n"
     "    Real showHrchyMaxChilds = -1 \n"
     "  ]], \n"
     "  Set partialLoad = Empty, \n"+
     aliasEnglishDescription_+
     "]]}"
     ,
     "OIS son las siglas de Objects Indexed Serialization, es decir, Serialización Indexada de Objetos "
     "mediante el cual se puede almacenar de forma persistente en ficheros binarios un conjunto de "
     "objetos TOL con estructura arbitraria. Los ficheros se organizan en un único directorio plano."
     "Esta función recupera total o parcialemente la información guardada en un almacén OIS "
     "bajo el directorio raíz (root)\n"
     "  1) El argumento address debe tener estructura @Ois.AddressStr o bien ser vacío."
     "Los elementos vacíos serán ignorados y el resto se usarán para definir "
     "el directorio donde se ubicará el OIS siguiendo esta regla organizativa:\n"
     "  root+\"/\"+address->Concept+\"/\"+address->Version+\"/\"+address->Node\n"
     "Se pretende con este argumento dotar de una mejor organización pero nada impide "
     "pasar todo el path en el argumento root y no especificar el argumento address o pasarlo vacío.\n"
     "  2) Con el argumento 'options' se puede configurar el proceso de carga. \n\n"
     "Por defecto se cargan los datos completos del OIS lo cual se podría expresar explícitamente de esta forma:\n"
     "Set {options = \n"
     "[[ \n"
     "  Set loadMode = [[Ois.Load.Mode.Data]], \n"
     "  Set showMode = \n"
     "  [[\n"
     "    Real showHrchyDepth = -1, \n"
     "    Real showHrchyMaxChilds = -1 \n"
     "  ]], \n"
     "  Set partialLoad = Empty, \n"+
        aliasSpanishDescription_+", \n"
     "  Real oisHasPriorityOnConflict = True\n"
     "]]}")+"\n\n"+
  I2("NOTE: It's strictly needed that you declare optionals arguments with proposed names to be recognized by "
     "internal process. Declaration of Set options is between keys {} to avoid create spureous global variables."
     "Options declaration order is not important.",
     "NOTA: Es estrictamente necesario que se declaren los argumentos opcionales con los nombres "
     "propuestos para que sean reconocibles por el proceso interno. Las declaración de options está entre llaves "
     "{} para evitar crear variables globales innecesarias. El orden de la declaración de las opciones es irrelevante.")+"\n\n"+
  I2("    2.1) Valid values of loadMode are :",
     "    2.1) Los valores válidos de loadMode son:")+
     "\n      2.1.1) Real Ois.Load.Mode.ShowHierarchy = "+OIS_Load_Mode_ShowHierarchy+" : "+
  I2("The objects hierarchy will be displayed in a text editor.",
     "Indica a la función Ois.Load que la jerarquía de objetos OIS se ha de "
     "mostrar en un editor de texto.")+"\n"+
     "\n      2.1.2) Real Ois.Load.Mode.Header = "+OIS_Load_Mode_Header+" : "+
  I2("Header information will be loaded from OIS directory",
     "Indica a la función Ois.Load que ha de cargar sólo el contenido de la cabecera OIS")+"\n"
     "\n      2.1.3) Real Ois.Load.Mode.Data = "+OIS_Load_Mode_Data+" : "+
  I2("TOL objects declaration, description and data will be loaded from OIS directory",
     "Se cargarán las declaraciones, descripciones y datos de los objectos TOL "
     "contenidos en el directorio OIS")+"\n\n"+
  I2("    2.2) When Real Ois.Load.Mode.ShowHierarchy has been included in loadMode you can pass a set "
     "called showMode to control showing process. "
     "      2.2.1) With showHrchyDepth you can specify the maximum depth of hierarchy that will be "
     "shown. If is negative or missing depth will be not bounded. This feature could be dangerous if there are "
     "cycled or very large structures. "
     "      2.2.2) With showHrchyMaxChilds you can specify the maximum number of childs that will be shown for each node . "
     "If is negative childs number will be not bounded. This feature could be dangerous if there are "
     "very large structures. ",
    "     2.2) Cuando Ois.Load.Mode.ShowHierarchy se incluye en loadMode es posible controlar el proceso "
    "de presentación de la jerarquía."
    "      2.2.1) Con showHrchyDepth se puede especificar la profundidad máxima de la jerarquía que se quiere representar."
    " Si es negativo o no aparece no se acotará dicha cantidad. Esto es peligroso si hay estructuras cíclicas o muy largas. "
    "      2.2.2) Con showHrchyMaxChilds Se puede especificar el máximo número de hijos que se verán para cada nodo."
    " Si es negativo o no aparece no se acotará dicha cantidad. Esto es peligroso si hay estructuras cíclicas o muy largas. ")+"\n\n"+
  I2("    2.3) When Ois.Load.Mode.Data is included in loadMode you can load full or partial OIS data "
     "with partialLoad argument, that can be empty for full loading, or else it must have the ordinals "
     "of successive elements from root set, or "
     "its names if exist, even when names are local. For example: ",
     "    2.3) Cuando OIS_Load_Mode_Data se incluye en loadMode es posible cargar los datos completos del "
     "OIS o bien especificar un camino parcial de carga mediante el argumento partialLoad que "
     "contendrá los ordinales sucesivos de los elementos desde el conjunto raíz o alternativamente "
     "los nombres de los mismos si es que tienen, incluso aunque se trate de nombres de ámbito "
     "local. Por ejemplo:")+"\n"+
     "  Set  { partialLoad = [[1,model,3]] } \n"+
  I2("will load just third element of a set called model into the first element of the OIS.",
     "cargará sólo el tercer elemento de un conjunto llamado model dentro del primer elemento del OIS.")+
  I2("WARNING: searching by name is available just if there no repeated names at same level.",
     "CUIDADO: la búsqueda por nombre sólo funciona si no hay nombres repetidos.")+"\n"+
  I2("    2.4) You can use tolSource root paths alias to check and update an OIS from a different source "
     "than original sources at building time. For example if OIS was created in other local area net and "
     "original IP is not accessible.",
     "    2.4) Se pueden usar los alias de las rutas a las raíces de los ficheros fuente de código TOL "
     "para chequera y actualizar un OIS desde una fuente distinta a la original. Por ejemplo cuando el "
     "OIS se creó en una red de área local distinta de la actual y la IP de la máquina original no está "
     "disponible.")+
  I2("    2.5) If oisHasPriorityOnConflict is false then, when a referenceable object exist already with "
     "the same name of an object inside an OIS image, the older object will be used instead of OIS one."
     "This option could be usaefull, for example, to recover old OIS images with damaged objects, as "
     "a dating that has been bounded between default dates at building time. If oisHasPriorityOnConflict "
     "is true, that's default behaviour, OIS object will be used to reproduce exactly as was built.",
     "    2.5) Si oisHasPriorityOnConflict es falso entonces, cuando exista un objeto referenciable con "
     "el mismo nombre que un objeto dentro de la imagen OIS, el objeto anterior reemplazará al del OIS."
     "Puede ser útil para recuperar OIS de versiones viejas en los que hayan podido quedar objetos "
     "deteriorados, como por ejemplo fechados recortados en las fechas por defecto de cuando se creó "
     "el OIS. Si oisHasPriorityOnConflict es cierto, que es el comportamiento por defecto, será el "
     "OIS el utilizado para garantizar que se carga exactamente lo que se almacenó. "),
     BOperClassify::Conversion_);
  void BSetOisLoad::CalcContens()
//--------------------------------------------------------------------
{
  BText& root = Text(Arg(1));
  bool doShowHierarchy     = false;
  bool doLoadData          = false;
  bool loadModeSpecified   = false;
  int  showHrchyDepth      = -1;
  int  showHrchyMaxChilds  = -1;
  BSet* add=NULL, *partial=NULL;
  BText sourceRoot = "";
  if(Arg(2)) { add = &Set(Arg(2)); }
  BOisLoader ois;
  ois.SetPath(root,add);
  if(Arg(3))
  {
    int opt, m;
    BSet& options = Set(Arg(3));
    for(opt=1; opt<=options.Card(); opt++)
    {
      if((options[opt]->Grammar()==GraReal())&&(options[opt]->Name()=="oisHasPriorityOnConflict"))
      {
        ois.SetOisHasPriorityOnConflict(Real(options[opt])!=0);
      }
      else if((options[opt]->Grammar()==GraSet())&&(options[opt]->Name()=="loadMode"))
      {
        loadModeSpecified = true;
        BSet& loadMode = Set(options[opt]);
        for(m=1; m<=loadMode.Card(); m++)
        {
          bool ok = loadMode[m]->Grammar()==GraReal();
          if(ok&&(loadMode[m]->Name()=="Ois.Load.Mode.ShowHierarchy"))
          { 
            doShowHierarchy = true; 
          }
          else if(ok && (loadMode[m]->Name()=="Ois.Load.Mode.Data"))
          { 
            doLoadData = true; 
          }
          else
          {
            WarUnkArg("Ois.Load", "options->loadMode", "Real Ois.Load.Mode.ShowHierarchy, Real Ois.Load.Mode.Header, Real Ois.Load.Mode.Data", loadMode, m);
          }
        }
      }
      else if((options[opt]->Grammar()==GraSet())&&(options[opt]->Name()=="showMode"))
      {
        BSet& showMode = Set(options[opt]);
        for(m=1; m<=showMode.Card(); m++)
        {
          bool ok = showMode[m]->Grammar()==GraReal();
          if(ok && (showMode[m]->Name()=="showHrchyDepth"))
          { 
            showHrchyDepth = (int)Real(showMode[m]); 
          }
          else if(ok && (showMode[m]->Name()=="Ois.Load.Mode.ShowHierarchy"))
          { 
            showHrchyDepth = (int)Real(showMode[m]); 
          }
          else
          {
            WarUnkArg("Ois.Load", "options->showMode", "Real showHrchyDepth, Real showHrchyMaxChilds", showMode, m);
          }
        }
      }
      else if((options[opt]->Grammar()==GraSet())&&(options[opt]->Name()=="partialLoad"))
      {
        partial = &Set(options[opt]);
      }
      else if((options[opt]->Grammar()==GraSet())&&(options[opt]->Name()=="tolSourceSearchPaths"))
      {
        ois.GetTolSourceSearchPaths("Ois.Load", options, opt);
      }
      else
      {
        WarUnkArg("Ois.Load", "options", "Set loadMode, Set showMode, Set partialLoad, Set tolSourceSearchPaths", options, opt);
      }
    }
  }
  if(!loadModeSpecified)
  {
    doLoadData = true;
  }
  bool checkIsUpdate = false;
  ois.Load(true,
           doShowHierarchy,
           checkIsUpdate,
           doLoadData,
           showHrchyDepth,
           showHrchyMaxChilds,
           partial); 
  contens_.PrepareStore(doLoadData);
  BSyntaxObject* data = NULL;
  BSyntaxObject* info = NULL;
  if(doLoadData)
  {
    BSyntaxObject* data = ois.GetData();
    if(data)
    {
      contens_.AddElement(data);
    }
  }
}  

//--------------------------------------------------------------------
  DeclareContensClass(BText, BTxtTemporary, BTextOisAutoPath);
  DefExtOpr(1, BTextOisAutoPath, "Ois.AutoPath", 1, 1, "Text",
  "(Text path)",
  I2("Returns an OIS path in a standard way.\n"
     "You can specify a usual relative or absolute path and also a default OIS "
     "abolute path begining by \"ois:/\" that will be replaced by the value of "
     "Text Ois.DefRoot variable.",
     "Devuelve un camino de OIS de una forma estándar.\n"
     "Se puede especificar un camino usual relativo o absoluto o también un camino"
     "aboluto por defecto de OIS empezando por \"ois:/\" lo cual será reemplazado"
     "por el contenido de la variable Text Ois.DefRoot"),
  BOperClassify::Conversion_);
  void BTextOisAutoPath::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = BOis::AutoPath(Text(Arg(1)));
}  

//--------------------------------------------------------------------
  DeclareContensClass(BText, BTxtTemporary, BTextOisGetModulePath);
  DefExtOpr(1, BTextOisGetModulePath, "Ois.GetModulePath", 1, 1, "Text",
  "(Text tolFile)",
  I2("Returns the OIS module path corresponding to a TOL file "
     "in a standard way.\n",
     "Devuelve el camino del módulo OIS correspondiente a un fichero "
     "TOL de una forma estándar.\n"),
  BOperClassify::Conversion_);
  void BTextOisGetModulePath::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = BOis::GetModulePath(Text(Arg(1)));
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDate, BDteTemporary, BDteOisModuleTime);
  DefExtOpr(1, BDteOisModuleTime, "Ois.GetModuleTime", 1, 1, "Text",
  "(Text tolFile)",
  I2("Returns the date and time of the standard OIS image of the "
     "module corresponding to a TOL file.\n",
     "Devuelve la fecha y la hora de la imagen OIS estándar del"
     "módulo correspondiente a un fichero TOL.\n"),
  BOperClassify::Conversion_);
  void BDteOisModuleTime::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = BOis::GetModuleTime(Text(Arg(1)));
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatOisRemoveModule);
  DefExtOpr(1, BDatOisRemoveModule, "Ois.RemoveModule", 1, 1, "Text",
  "(Text tolFile)",
  I2("Removes the standard OIS image of the module path "
     "corresponding to a TOL file.\n",
     "Destruye la imagen OIS estándar del módulo correspondiente a "
     "un fichero TOL.\n"),
  BOperClassify::Conversion_);
  void BDatOisRemoveModule::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = BOis::RemoveModule(Text(Arg(1)));
}  

//--------------------------------------------------------------------
  DeclareContensClass(BText, BTxtTemporary, BTextOisPutDefaultArchiveEngine);
  DefExtOpr(1, BTextOisPutDefaultArchiveEngine, 
               "Ois.PutDefaultArchiveEngine", 1, 1, "Text",
  "(Text archiveEngine)",
  I2("Changes current value of global variable Ois.DefaultArchiveEngine "
     "that is the default value of OIS images archive engine.\n"
     "See Ois.DefaultArchiveEngine to known about allowed values "
     "of archiveEngine are:\n"
     " \"_NONE_\"   : OIS files will be archived in a plain directory.\n"
     " \"_ZIPARC_\" : OIS files will be archived in a ZIP file.\n"
     "If any other value is passed no action will be done."
     "Returns the old value of Ois.DefaultArchiveEngine",
     "Cambia el valor de la variable global Ois.DefaultArchiveEngine que es "
     "el valor por defecto del motor de archivador de imágenes OIS.\n"
     "Los valores permitidos para archiveEngine son:\n"
     " \"_NONE_\"   : los ficheros OIS se archivarán en un directorio plano.\n"
     " \"_ZIPARC_\" : los ficheros OIS se archivarán en un fichero ZIP "
     "comprimido mediante la librería de código abierto ZipArchive, y con "
     "la extensión .oza, que se pued eabrir con cualquier archivador "
     "compatible con el formato ZIP.\n"
     "Si se pasa cualquier otro valor no se cambiará nada."
     "Se devolverá el antiguo valor de Ois.DefaultArchiveEngine."),
  BOperClassify::Conversion_);
  void BTextOisPutDefaultArchiveEngine::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = BOis::ArchiveEngine();
  BOis::PutDefaultArchiveEngine(BOis::ArchiveEngine(Text(Arg(1))));
}  

//--------------------------------------------------------------------
  DeclareContensClass(BText, BTxtTemporary, BTextOisPutDefaultSerialEngine);
  DefExtOpr(1, BTextOisPutDefaultSerialEngine, 
               "Ois.PutDefaultSerialEngine", 1, 1, "Text",
  "(Text serialEngine)",
  I2("Changes current value of global variable Ois.DefaultSerialEngine "
     "that is the default value of OIS images serialization engine.\n"
     "See Ois.DefaultSerialEngine to known about allowed values "
     "of serialEngine are:\n"
     " \"_NONE_\"  : OIS files will be serialized in a plain file.\n"
     " \"_BZIP2_\" : OIS files will be serialized in a file with independently "
     "compressed strings using open-source library BZIP, to allow indexes and "
     "accessing to internal items of the file.\n"
     "If any other value is passed no action will be done."
     "Returns the old value of Ois.DefaultArchiveEngine",
     "Cambia el valor de la variable global Ois.DefaultSerialEngine que es "
     "el valor por defecto del motor de archivador de imágenes OIS.\n"
     "Los valores permitidos para archiveEngine son:\n"
     " \"_NONE_\"   : los ficheros OIS se archivarán en un directorio plano.\n"
     " \"_BZIP2_\" : los ficheros OIS se archivarán en un fichero con cadenas "
     "de texto comprimidas independientemente unas de otras mediante la "
     "librería de código abierto BZIP2, para permitir la indexación y el "
     "acceso a puntos internos del fichero.\n"
     "Si se pasa cualquier otro valor no se cambiará nada."
     "Se devolverá el antiguo valor de Ois.DefaultSerialEngine."),
  BOperClassify::Conversion_);
  void BTextOisPutDefaultSerialEngine::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = BOis::SerialEngine();
  BOis::PutDefaultSerialEngine(BOis::SerialEngine(Text(Arg(1))));
}  


//--------------------------------------------------------------------
BSyntaxObject* OisUseModuleEvaluator(BList* arg)
//--------------------------------------------------------------------
{
  BSyntaxObject* result=NULL;
  BTimer tm;
  BText name = Text((BSyntaxObject*)Car(arg));

//BText expr = BText("IncludeText(Ois.AutoGen(\"")+name+"\"))[1]";
//result = GraSet()->EvaluateExpr(expr);

  BOisLoader oisLoader;
  oisLoader.SetModulePath(name);
  oisLoader.SetCheckSourceExists (true);
  oisLoader.SetCheckSourceSize   (true);
  oisLoader.SetCheckSourceDate   (true);
  oisLoader.SetShowCheckingTraces(false);

  bool errorWarning        = false;
  bool doShowHierarchy     = false;
  bool checkIsUpdate       = true;
  bool doLoadData          = true;
  int  showHrchyDepth      = -1;
  int  showHrchyMaxChilds  = -1;
  const BSet* partial      = NULL;
  bool oldRunningUseModule = BOis::SetRunningUseModule(true);
  bool ok = oisLoader.Load
  (
    errorWarning,
    doShowHierarchy,
    checkIsUpdate,
    doLoadData,
    showHrchyDepth,
    showHrchyMaxChilds,
    partial
  );
  if(ok)
  {
    result = oisLoader.GetData();
    oisLoader.Close();
  }
  else
  {
    oisLoader.Close();
    int oldErr = (int)TOLErrorNumber().Value();
    BOisCreator oisCreator;
    BOisCreator::current_ = &oisCreator;
    BUserSet* uData = IncludeFile(name);
    BOisCreator::current_ = NULL;
    result = uData;
    int numErr = (int)TOLErrorNumber().Value()-oldErr;
    if(numErr || !result)
    {
      Error(I2("\nOIS: Cannot create module ","\nOIS: NO se puede crear el módulo ")+
            name+
            I2(" with "," con ")+
            numErr+
            I2(" errors "," errores ")+"\n");
      BOis::RemoveModule(name);
    }
    else
    {
      oisCreator.CreateModule(name,uData);
      oisCreator.Build();
    }
  }
  BOis::SetRunningUseModule(oldRunningUseModule);
  DESTROY(arg);
  return(result);
}

static void * clone_ois_use_module_() 
{
  return new	BExternalOperator 
  (
    "Ois.UseModule",
    GraSet(),
    "Text",
    OisUseModuleEvaluator,
    1,
    1,
    "(Text tolFile)",
    I2("Loads a precompiled OIS module storing all objects of a TOL file that will be "
       "compiled just if it's needed, it's to say, if it doesn't exist or isn't updated.\n"
       "When OIS image is not rebuilt it will be loaded without compile the TOL file.\n"
       "The OIS image will be generated in the directory specified in global Text "
       "Ois.DefRoot\n"
       "To use a normal TOL file as a OIS module you could use this syntax:\n"
       "  Set Ois.Module(tolFile);\n"
       "instead of usual including order\n"
       "  Set Include(tolFile);\n"
       ,
       "Carga un módulo OIS precompilado que almacena todos los objetos de un fichero "
       "TOL que será compilado sólo si es necesario para reconstruir la imagen OIS, "
       "o sea, si no existe o no está actualizada.\n"
       "La imagen OIS se generará la imagen OIS dentro del directorio especificado "
       "en la variable global Text Ois.DefRoot.\n"
       "Para usar un fichero TOL normal como un módulo OIS la sintaxis a usar es:\n"
       " Set Ois.UseModule(tolFile);\n"
       "en lugar de la típica orden\n"
       " Set Include(tolFile);\n"),
    BOperClassify::FileIncluding_
  );
}

static BExternalOperator * ois_use_module_ = 
  (BExternalOperator*)(__delay_init((void**)&ois_use_module_,
				    &clone_ois_use_module_));


//--------------------------------------------------------------------
BSyntaxObject* OisUseCacheEvaluator(BList* arg)
//--------------------------------------------------------------------
{
  BSyntaxObject* result=NULL;
  BTimer tm;
  BText name = Text((BSyntaxObject*)Car(arg));
  if(!CheckIsFile(name)) 
  { 
    Error(I2("\nOIS: Cannot create cache of ",
             "\nOIS: No se puede crear la caché de ")+ name+
          I2(" due to file is not found",
             " porque no se encuentra el fichero")+"\n");
  }
  else
  {
    BDat caducityInMinutes = BDat::PosInf();
    BUserSet* address = (BUserSet*)GraSet()->EvaluateExpr("Copy(Empty)");
    if(Cdr(arg))
    {
      caducityInMinutes = Dat((BSyntaxObject*)Car(Cdr(arg)));
    }
    BText ozaPath = BOis::GetModulePath(name);
    bool ozaExist = CheckIsFile(ozaPath);
    bool isObsolete = !ozaExist;
    if(isObsolete)
    {
      Std(I2("\nOIS: Cannot find cache of ",
           "\nOIS: No se encuentra la caché de ")+ name +"\n");
    }
    else if(caducityInMinutes.IsUnknown() || (caducityInMinutes<=0))
    {
      Std(I2("\nOIS: Cache of ",
             "\nOIS: La caché de ")+ name +
          I2(" cannot be used with non positive or unknown caducity\n",
             " no puede ser utilizada con caducidad no positiva o desconocida\n"));
      isObsolete = true;
    }
    else
    {
      BDate tolTime = BTimer::TimeToDate(GetFileTime(name));
      BDate ozaTime = BTimer::TimeToDate(GetFileTime(ozaPath));
      isObsolete = tolTime > ozaTime;
      if(isObsolete)
      {
        Std(I2("\nOIS: Cache of ",
             "\nOIS: La caché de ")+ name +" is older than source\n");
      }
      else
      {
        BDate nowTime = DteNow();
        isObsolete = (nowTime-ozaTime)*24*60 > caducityInMinutes;
        if(isObsolete)
        {
          Std(I2("\nOIS: Cache of ",
                 "\nOIS: La caché de ")+ name +
              I2(" is lapsed\n",
                 " está caducada\n"));
        }
      }
    }
    
    if(isObsolete)
    {
      BSys::Remove(ozaPath);
      Std(I2("\nOIS: Building cache of ",
             "\nOIS: Construyendo la caché de ")+ name +" ... \n");
      BUserSet* uSet = IncludeFile(name);
      result = uSet;
      BBool ok = false;
      if(result)
      {
        BOisCreator oisCreator;
        oisCreator.Create(ozaPath,address,&(uSet->Contens()),NULL,NULL);
      //Std(BText("TRACE BDatOisCreate::CalcContens() 1\n"));
        ok = oisCreator.Build();
      }
      ok &= CheckIsFile(ozaPath);
      if(!ok)
      {
        Error(I2("\nOIS: Cannot create cache of ",
                 "\nOIS: No se puede crear la caché de ")+ name+"\n");
      }
    }
    if(!isObsolete && !result)
    {
      Std(I2("\nOIS: Loading cache of ",
             "\nOIS: Cargando la caché de ")+ name +" ... \n");
      BOisLoader oisLoader;
      oisLoader.SetPath(ozaPath,&(address->Contens()));
      oisLoader.SetCheckSourceExists (false);
      oisLoader.SetCheckSourceSize   (false);
      oisLoader.SetCheckSourceDate   (false);
      oisLoader.SetShowCheckingTraces(false);
      bool errorWarning        = false;
      bool doShowHierarchy     = false;
      bool checkIsUpdate       = false;
      bool doLoadData          = true;
      int  showHrchyDepth      = -1;
      int  showHrchyMaxChilds  = -1;
      const BSet* partial      = NULL;
      bool ok = oisLoader.Load
      (
        errorWarning,
        doShowHierarchy,
        checkIsUpdate,
        doLoadData,
        showHrchyDepth,
        showHrchyMaxChilds,
        partial
      );
      if(ok)
      {
        result = oisLoader.GetData();
        oisLoader.Close();
      }
    }
    DESTROY(address);
  }
  DESTROY(arg);
  return(result);
}

static void * clone_ois_use_cache_() 
{
  return new	BExternalOperator 
  (
    "Ois.UseCache",
    GraSet(),
    "Text Real",
    OisUseCacheEvaluator,
    2,
    2,
    "(Text tolPath, Real caducityInMinutes)",
    I2("Loads a precompiled OIS cache archive storing all objects of a TOL file that will be "
       "compiled just if cache archive not exists yet or it's obsolete or lapsed, or "
       "argument <caducityInMinutes> is non postive or unknown. \n"
       "It is considered that the cache is obsolete if it's earlier than the source file.\n"
       "Not take into account whether the source file has changed or not in reality, but only check its date.\n"
       "If the source file depend on others, they are not taken into account for purposes of verifying whether the cache is obsolete.\n"
       "It is considered that cache is lapsed when elapse more than <caducityInMinutes> minutes since its creation.\n"
       "When OIS image is not rebuilt it will be loaded without compile the TOL file.\n"
       "The OIS image will be generated in the directory specified in global Text "
       "Ois.DefRoot\n"
       ,
       "Carga un archivo de caché OIS precompilado que almacena todos los objetos de un fichero "
       "TOL que será compilado sólo si no existe, está caducado u obsoleto, o si el "
       "argumento <caducityInMinutes> es no positivo o desconocido.\n"
       "Se considera que la caché está obsoleta si su fecha es anterior a la del archivo fuente.\n"
       "No se tendrá en cuenta si el archivo fuente ha cambiado o no en realidad, sino que se comprobará únicamente su fecha.\n"
       "Si el archivo fuente depende de otros, éstos no serán tenidos en cuenta a efectos de comprobar si la caché está obsoleta.\n"
       "Se considera que la caché está caducada si han transcurrido más de <caducityInMinutes> minutos desde su creación.\n"
       "La imagen OIS se generará la imagen OIS dentro del directorio especificado "
       "en la variable global Text Ois.DefRoot.\n"
    ),
    BOperClassify::FileIncluding_
  );
}

static BExternalOperator * ois_use_cache_ = 
  (BExternalOperator*)(__delay_init((void**)&ois_use_cache_,
				    &clone_ois_use_cache_));

//--------------------------------------------------------------------
BSyntaxObject* BOisLoader::LoadFull(const BText& root)
//--------------------------------------------------------------------
{
  bool doShowHierarchy     = false;
  bool checkIsUpdate       = false;
  bool doLoadData          = true;
  bool loadModeSpecified   = false;
  int  showHrchyDepth      = -1;
  int  showHrchyMaxChilds  = -1;
  BText sourceRoot = "";
  BOisLoader ois;
  ois.SetPath(root,NULL);
  ois.Load(true,
           doShowHierarchy,
           checkIsUpdate,
           doLoadData,
           showHrchyDepth,
           showHrchyMaxChilds,
           NULL); 
  return(ois.GetData());
}
