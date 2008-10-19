/* txtgra.cpp: Text grammars functions of GNU/TOL language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

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

/*
 * OJO!!! este valor no debe redefinirse en ninguno de los .h que se incluyan
 * a continuacion.
 */
#define DEFAULT_INIT_PRIORITY 10

//#define USETRAZEOBJECTS

#include <tol/tol_btxtgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bdir.h>
#include <tol/tol_list.h>
#include <tol/tol_bseed.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bdtegra.h>
#ifdef __USE_TC__
#  include <tol/tol_bctmigra.h>
#endif
#include <tol/tol_bsetgra.h>
#include <tol/tol_btmsgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bmoninfo.h>

#if defined(_MSC_VER)
#  include <process.h>
#endif

#include <locale.h>

/*! key used to force that Sub be initialized before than Sub of matrix
 *  the default is 100 (see default argument for key in delay_init.h)
 */
#define KEY_INIT 10

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("txtgra.cpp");
template<>
BGrammar* BGraContensBase<BText>::ownGrammar_ = NIL;

DefineContensCommonOperators(BText, "Text");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BText>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}


//--------------------------------------------------------------------
/*! Time in local format class
 */
class BTxtTime: public BSystemText
{
public:
    // Constructors and destructors: inline
    BTxtTime()
	: BSystemText("Time","",
		      I2("Returns the actual time in local format.",
			 "Devuelve la hora actual en el formato local."))  { }
    ~BTxtTime() { }
    void	 CalcContens() {}
    BText& Contens() { return(contens_ = BTimer::Text()); }
};

//--------------------------------------------------------------------
/*! Process Time Report
 */
class BTxtProcessTime: public BSystemText
{
public:
    // Constructors and destructors: inline
    BTxtProcessTime()
	: BSystemText("ProcessTime","",
		      I2("Returns the actual time in local format.",
			 "Devuelve la hora actual en el formato local."))  { }
    ~BTxtProcessTime() { }
    void	 CalcContens() {}
    BText& Contens()
	{
	    BTimer::PrintProcess();
	    return(contens_ = BTimer::Text());
	}
};


#ifdef UNIX
#else // WINDOWS
#include <windows.h>

//--------------------------------------------------------------------
static BText& BGetSystemDirectory()
//--------------------------------------------------------------------
{
    static BText SystemDirectory(1024,'\0');
    BInt len = GetSystemDirectory
	(
	    SystemDirectory.Buffer(),
	    SystemDirectory.Size()
	    );
    SystemDirectory.PutLength(len);
    return(SystemDirectory);
}

//--------------------------------------------------------------------
static BText& BGetWindowsDirectory()
//--------------------------------------------------------------------
{
    static BText WindowsDirectory(1024,'\0');
    BInt len = GetWindowsDirectory
	(
	    WindowsDirectory.Buffer(),
	    WindowsDirectory.Size()
	    );
    WindowsDirectory.PutLength(len);
    return(WindowsDirectory);
}

#endif // WINDOWS

static BText tol_lc_numeric_("C");
BText& TOL_lc_numeric() { return tol_lc_numeric_; }

static BText jacobianMethod_("Analytical");
BText& JacobianMethod() { return(jacobianMethod_); }

BText & GCFFile()
{
  static BText gcfFile("");

  return gcfFile;
}


static BSystemText* editor_ = NULL;

static char cpp_compiler     [ 64];
static char cpp_compiler_desc[256];

void SetCppCompiler()
{
  static bool aux_;
  if(!aux_)
  {
#   if defined(__GNUC__)
      sprintf(cpp_compiler,      "GCC_%d",__GNUC__);
      sprintf(cpp_compiler_desc, "%s", 
              "The GNU Compiler Collection (http://gcc.gnu.org)\n"
              "This is a fully open source sofware.");
#   elif defined(_MSC_VER)
      sprintf(cpp_compiler, "MSVC_%d",_MSC_VER);
      sprintf(cpp_compiler_desc, "%s", 
              "The Microsoft Softaware Visual C++ (http://msdn.microsoft.com/visualc).\n"
              "This is a propietary softaware although there is a free limited license of type EULA.");
#   endif
    aux_ = true;
  }
}

char* CppCompiler()
{
  return(cpp_compiler);
}

char* CppCompilerDesc()
{
  return(cpp_compiler_desc);
}



//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraContensBase<BText>::InitInstances()    
{
    BTraceInit("BGraContens<BTxt>::InitInstances");
    
    BSystemText* nullString_ = new BSystemText("NULL", "",
					       I2("Null string.",
						  "Cadena vacía."));
    
    new BParamText
      ("TOL_LC_NUMERIC", TOL_lc_numeric(),
       I2("LC_NUMERIC used to format output number values. "
	  "LC_NUMERIC is part of the locale OS configuration.\n"
	  "\"locale\" is a character string containing the "
	  "information required to format according to each country "
	  "settings. A  locale  name  is  typically  of the form "
	  "language[_territory][.code-set][@modifier], where language "
	  "is an ISO 639 language code, territory is an ISO 3166 "
	  "country code, and codeset is a character set or encoding "
	  "identifier like ISO-8859-1 or UTF-8.\n"
	  "Under Windows, use a language string such \"English\", or "
	  "\"Spanish\". Under Linux and Unix systems, use the format "
	  "explained above. For Spanish: es_ES.UTF-8, or es_ES.ISO8859-1"
	  "\n"
	   ,
	  "LC_NUMERIC usado para formatear la salida de valores numéricos."
	  "LC_NUMERIC es parte de la configuración de localización "
	  "geográfica del Sistema Operativo\n."
	  "La \"localización\" es una cadena de caracteres que "
	  "contiene la identificación del lugar geográfica necesaria para "
	  "dar el formato acorde las especificaciones de cada país.\n"
	  "Una localización tiene normalmente la siguiente forma: "
	  "lenguage[_territorio][.code-set][@modificador], donde lenguaje "
	  "es un código de lenguaje acorde con la norma ISO 639, "
	  "territorio un código de la norma ISO 3166 de especificación "
	  "de país, y el codeset es el conjunto de codificación de "
	  "caracteres tales como el ISO-8859-1 o el UTF-8.\n"
	  "Bajo Windows, se recomienda usar las cadenas de identificación "
	  "de lenguaje expresadas de namenra natural: p. ej: "
	  "\"English\", or \"Spanish\".\n"
	  ));

    new BParamText("GCFFile", GCFFile(),
                   I2("GCF file to be used by Chart methods",
                      "Archivo GCF a ser usado por los métodos Chart"));

    BText JMN = "JacobianMethod";
    BText JMD =
	I2("Jacobian's calculation method can be Analytical or Numerical",
	   "El método de cálculo del Jacobiano puede ser Analítico o Numérico");
    
    BParamText* JM = new BParamText(JMN, jacobianMethod_, JMD);
    
    BSystemText* TolAppDataPath_ =new BSystemText("TolAppDataPath", BSys::TolAppData(),
		  I2("Returns the standard system directory to store application data.",
		     "Devuelve el directorio estándar del sistema para almacenar "
         "datos de la aplicación."));

#ifdef UNIX
#else // WINDOWS
#  include <windows.h>
    
    BText& SearchPaths();
    BParamText* searchPaths = new BParamText
	("SearchPaths", SearchPaths(),
	 I2("TOL searching paths separated by ';'.",
	    "Directorio de búsqueda de ficheros TOL"));
    
    BSystemText* systemDir_ = new BSystemText
	("SystemDirectory", BGetSystemDirectory(),
	 I2("Windows System Directory. For sample, on Windows 95, it should "
	    "be c:\\windows\\system; and on Windows NT, c:\\winnt\\system32",
	    "Directorio del Sistema Operativo. Por ejemplo, en Windows 95, "
	    "suele ser c:\\windows\\system; y en Windows NT, "
	    "c:\\winnt\\system32"));
    
    BSystemText* windowsDir_ = new BSystemText
	("WindowsDirectory", BGetWindowsDirectory(),
	 I2("Windows Directory. For sample, on Windows 95, it should "
	    "be c:\\windows; and on Windows NT, c:\\winnt",
	    "Directorio de Windows. Por ejemplo, en Windows 95, "
	    "suele ser c:\\windows; y en Windows NT, c:\\winnt"));
    
#endif // WINDOWS

    BSystemText* system_ = new BSystemText("OSName", SYSTEM,
					   I2("Operative System Name.",
					      "Nombre del Sistema Operativo."));

    SetCppCompiler();
    BSystemText* compiler_ = new BSystemText("CppCompiler", cpp_compiler,
					   I2("C++ Compiler identification TOL has been generated with.",
					      "Identificador del compilador C++ con el que se ha generado TOL.")+
                          "\n"+cpp_compiler_desc);
    
    BSystemText* language_ = new BSystemText
      ("Language", I2("ENGLISH","CASTELLANO"),
       I2("User language.","Idioma de usuario."));
    
    BSystemText* tempDir_ = new BSystemText
	("TmpDir", GetFilePath(BSys::TempNam("")),
	 I2("Temporary stockage directory.",
	    "Directorio para almacenamiento temporal."));
    
    editor_= new BSystemText("Editor", BSys::Editor(), I2("Default editor.","Editor por defecto."));
 

   BTxtTime* timeTxt_ = new BTxtTime;

#ifdef USETRAZEOBJECTS
   BTxtProcessTime* processTimeTxt_ = new BTxtProcessTime;
#endif

    OwnGrammar()->PutDefect(nullString_);
}



//--------------------------------------------------------------------
/*! Returns a valid constant BUserText for name.
 */
template<>
BSyntaxObject* BGraContensBase<BText>::FindConstant(const BText& name)
{
    BUserText* userText = NIL;
    
    if(name.HasName() && (name.Get(0)=='\"'))
    {
	BText txt = name.String()+1;
//	if(txt.Last()=='\"') { txt.PutLength(txt.Length()-1); }
	userText = new BContensText(txt);
    }
    return((BSyntaxObject*)userText);
}


//--------------------------------------------------------------------
/*! Returns a valid text for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BText>::Casting(BSyntaxObject* obj)
{
    if(!obj)			     { return(NIL); }
    if(obj->Grammar()==OwnGrammar()) { return(obj); }
    return(NIL);
}


//--------------------------------------------------------------------
// algebraic temporary class declarations
//--------------------------------------------------------------------


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtI2);
DefIntOpr(1, BTxtI2, "I2", 2, 2,
	  "(Text eng, Text spa)",
	  I2("Returns the first text or the second according to if the "
       "current language is English or Spanish.",
	     "Devuelve el primer texto o el segundo según sea el idioma "
       "en curso el inglés o el español."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtI2::CalcContens()
//--------------------------------------------------------------------
{
  contens_=I2(Text(Arg(1)),Text(Arg(2)));
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BGetEnv);
DefIntOpr(1, BGetEnv, "GetEnv", 1, 1,
	  I2("(Text environmentVar)",
	     "(Text varDeEntorno)"),
	  I2("Get the value of enviroment variable given as argument.",
	     "Obtiene el valor de la variable de entorno especificada "
	     "como argumento"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BGetEnv::CalcContens()
//--------------------------------------------------------------------
{
    BText varName = Text(Arg(1));
    contens_ = BSys::GetEnv(varName);
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BGetRngName);
DefIntOpr(1, BGetRngName, "GetRandomName", 1, 1,
	  "(Text empty)",
	  I2("Returns the name of the default random generator.",
	     "Retorna el nombre del generador aleatorio por omisión"),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BGetRngName::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = gsl_rng_name(BProbDist::rng());
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BPutEditor);
DefIntOpr(1, BPutEditor, "PutEditor", 1, 1,
	  I2("(Text fileName)",
	     "(Text nombreFichero)"),
	  I2("Puts a new default editor for text format methods and returns the "
	     "current editor.",
	     "Cambia el editor por defecto para metodos con formato de texto "
	     "y devuelve el que había hasta el momento."),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BPutEditor::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = BSys::Editor();
    BText editorPath = Text(Arg(1));
    editor_->Contens()=editorPath;
    BSys::PutEditor(editorPath);
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BPutLanguage);
DefIntOpr(1, BPutLanguage, "PutLanguage", 1, 1,
	  I2("(Text language)",
	     "(Text idioma)"),
	  I2("Sets the current human language of TOL messages and returns the previously one."
       "Admited languages are \"ENGLISH\" and \"SPANISH\"",
	     "Cambia el idioma usado por TOL en los mensajes y devuelve el que había hasta el momento."
       "Los idiomas admitidos son \"INGLES\" y \"CASTELLANO\" ó \"ESPAÑOL\" "),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BPutLanguage::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = I2("ENGLISH","CASTELLANO");
  BText& lang = Text(Arg(1));
       if(lang=="ENGLISH"    ) { BText::PutLanguage(BENGLISH); }
  else if(lang=="INGLES"     ) { BText::PutLanguage(BENGLISH); }
  else if(lang=="SPANISH"    ) { BText::PutLanguage(BSPANISH); }
  else if(lang=="CASTELLANO" ) { BText::PutLanguage(BSPANISH); }
  else if(lang=="ESPAÑOL"    ) { BText::PutLanguage(BSPANISH); }
  else 
  {
    Warning(I2("PutLanguage function cannot use unknown language ",
               "La función PutLanguage no puede usar el idioma desconocido \"")+lang+"\"\n"+
            I2("Admited languages are \"ENGLISH\" and \"SPANISH\"",
               "Los idiomas admitidos son \"INGLES\" y \"CASTELLANO\" ó \"ESPAÑOL\" "));
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtSum);
DefExtOpr(1, BTxtSum, "+", 2, 2, "Text Text",
	  "txt1 + txt2  {Text txt1, Text x2}",
	  I2("Concats two text objects.",
	     "Concatena dos objetos de tipo texto. "),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtSum::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Text(Arg(1)) + Text(Arg(2));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtSetSum);
DefExtOpr(1, BTxtSetSum, "SetSum", 1, 1, "Set",
	  I2("(Set texts)",
	     "(Set textos)"),
	  I2("Concats all elements of a set of texts.",
	     "Concatena todos los elementos de un conjunto de textos."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtSetSum::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  contens_ = "";
  bool ok = true;
  for(BInt n=1; n<=set.Card(); n++)
  {
	  if(set[n]->Grammar()==GraText()) { contens_ += Text(set[n]); }
    else                             { ok = false; }
  }
  if(!ok) 
  { 
    Error(I2("There are non Text objects in the set argument of "
             "function Text SetSum",
             "Hay objetos de tipo distinto de Text en el conjunto "
             "argumento de la llamada a la función Text SetSum"));
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtConcat);
DefExtOpr(1, BTxtConcat, "<<", 2, 2, "Text Anything",
	  "txt1 << x2  {Text txt1, Anything x2}",
	  I2("Concats two text objects."
	     "If the second object is not a text, converts it to text before "
	     "effecting the operation.",
	     "Concatena dos objetos de texto. "
	     "Si el segundo objeto no es un texto, lo convierte a texto antes de "
	     "efectuar la operación."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtConcat::CalcContens()
//--------------------------------------------------------------------
{
    if(Arg(2)->Grammar()==Grammar()) {
	contens_ = Text(Arg(1)) + Text(Arg(2));
    } else {
	contens_ = Text(Arg(1)) + Arg(2)->Dump();
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtCompact);
DefIntOpr(1, BTxtCompact, "Compact", 1, 1,
	  "(Text txt)",
	  I2("Compacts a text substituting any combination of white characters "
	     "by an only character of space.",
	     "Compacta un texto sustituyendo cualquier combinación de caracteres "
	     "blancos por un sólo carácter de espacio."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtCompact::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Text(Arg(1));
    contens_.Compact();
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtReverse);
DefIntOpr(1, BTxtReverse, "Reverse", 1, 1,
	  "(Text txt)",
	  I2("Reverses a text.",
	     "Invierte el orden de los caracteres de un texto."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtReverse::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Text(Arg(1));
    contens_.Reverse();
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtReplace);
DefIntOpr(1, BTxtReplace, "Replace", 3, 3,
	  I2("(Text txt, Text old, Text new)",
	     "(Text txt, Text viejo, Text nuevo)"),
	  I2("Replaces a substring by another one into a text.",
	     "A lo largo de un texto reemplaza una subcadena por otra."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtReplace::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Text(Arg(1));
    contens_.Replace(Text(Arg(2)),Text(Arg(3)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtReplaceTable);
DefExtOpr(1, BTxtReplaceTable, "ReplaceTable", 2, 3, "Text Set Real",
	  I2("(Text txt, Set table, [Real loops=0])",
	     "(Text txt, Set tabla, [Real loops=0])"),
	  I2("Constructs a Text based on \"Text txt\" by applying all "
	     "replacements given in \"Set table\". Elements of \"Set "
	     "table\" must be Set objects, each of them with two Text "
	     "elements: [[\"OldText\",\"NewText\"]]. The elements of "
	     "Set table are sorted by oldText before being sequentially "
	     "applied .Parameter \"Real loops\" is optional. If it is not "
	     "used, replacements are applied repeatedly until no substitution "
	     "is possible. Using no third parameter could get an infinite "
	     "loops.\n\nExamples:\n\n"
	     "1) Automata to recognize three different regular expressions\n"
	     "AC*DF | AC*E | BF\n"
	     "Each number in \"Set Automata\" is an state. If the result "
	     "of a given input string is an output string with value \"4\" "
	     "then the Automata recognize that input string:\n\n"
	     "Set Automata =\n"
	     "[[ [[\"A\", \"12\"]], // Entry \"A\" -> from State 1 to 2\n"
	     "   [[\"B\", \"13\"]], // Entry \"B\" -> from State 1 to 3\n"
	     "   [[\"C\", \"22\"]], // Entry \"C\" -> from State 2 to 2\n"
	     "   [[\"D\", \"23\"]], // Entry \"D\" -> from State 2 to 3\n"
	     "   [[\"E\", \"24\"]], // Entry \"E\" -> from State 2 to 4\n"
	     "   [[\"F\", \"34\"]], // Entry \"F\" -> from State 3 to 4\n"
	     "   [[\"12\", \"2\"]], // migration from State 1 to 2\n"
	     "   [[\"13\", \"3\"]], // migration from State 1 to 3\n"
	     "   [[\"22\", \"2\"]], // migration from State 1 to 2\n"
	     "   [[\"23\", \"3\"]], // migration from State 2 to 3\n"
	     "   [[\"24\", \"4\"]], // migration from State 2 to 4\n"
	     "   [[\"34\", \"4\"]]  // migration from State 3 to 4\n"
	     "]];\n"
	     "Text instr = \"ACCCDF\";\n"
	     "Text outstr = ReplaceTable(instr,automata);\n"
	     "Result: outstr == \"4\"\n\n"
	     "2) Octal numbers to binary numbers conversion (using 3th param)"
	     ":\n\n"
	     "Set oct2bin =\n"
	     "[[ [[\"0\", \"000\"]],\n"
	     "   [[\"1\", \"001\"]],\n"
	     "   [[\"2\", \"010\"]],\n"
	     "   [[\"3\", \"011\"]],\n"
	     "   [[\"4\", \"100\"]],\n"
	     "   [[\"5\", \"101\"]],\n"
	     "   [[\"6\", \"110\"]],\n"
	     "   [[\"7\", \"111\"]]\n"
	     "]];\n"
	     "Text octstr = \"371\";\n"
	     "Text binstr = ReplaceTable(octstr,oct2bin,1);\n\n"
	     "Result: binstr == \"011111001\"\n",
	     "Crea un Text aplicando a Text txt las sustituciones de "
	     "Set table. Los elementos de Set table tienen que ser objetos "
	     "Set formados por pares de Text: [[\"TextoAntiguo\","
	     "\"TextoNuevo\"]]. Los elementos del Set table son ordenados "
	     "alfabéticamente por \"TextoAntiguo\" antes de que se apliquen "
	     "secuencialmente las sustituciones. El parámetro Real loops es "
	     "opcional, si no se utiliza, las sustituciones se aplican "
	     "reiteradamente hasta que no es posible ninguna sustitución "
	     "más. No especificar el tercer argumento puede provocar bucles "
	     "infinitos.\n\nEjemplos:\n\n"
	     "1) Autómata que reconoce tres expresiones regulares\n"
	     "AC*DF | AC*E | BF\n"
	     "Cada número en el Set Automata es un estado. Si el resultado "
	     "de una cadenada de entrada dada es una cadena de salida con "
	     "valor \"4\" entonces el autómota reconoce la cadena de "
	     "entrada:\n\n"
	     "Set Automata =\n"
	     "[[ [[\"A\", \"12\"]], // Entry \"A\" -> from State 1 to 2\n"
	     "   [[\"B\", \"13\"]], // Entry \"B\" -> from State 1 to 3\n"
	     "   [[\"C\", \"22\"]], // Entry \"C\" -> from State 2 to 2\n"
	     "   [[\"D\", \"23\"]], // Entry \"D\" -> from State 2 to 3\n"
	     "   [[\"E\", \"24\"]], // Entry \"E\" -> from State 2 to 4\n"
	     "   [[\"F\", \"34\"]], // Entry \"F\" -> from State 3 to 4\n"
	     "   [[\"12\", \"2\"]], // migration from State 1 to 2\n"
	     "   [[\"13\", \"3\"]], // migration from State 1 to 3\n"
	     "   [[\"22\", \"2\"]], // migration from State 1 to 2\n"
	     "   [[\"23\", \"3\"]], // migration from State 2 to 3\n"
	     "   [[\"24\", \"4\"]], // migration from State 2 to 4\n"
	     "   [[\"34\", \"4\"]]  // migration from State 3 to 4\n"
	     "]];\n"
	     "Text instr = \"ACCCDF\";\n"
	     "Text outstr = ReplaceTable(instr,Automata);\n"
	     "Result: outstr == \"4\"\n\n"
	     "2) Corrección ortográfica (no utiliza el 3er. parámetro):\n\n"
	     "Set ortTab = \n"
	     "[[ [[\"nrr\", \"nr\"]], // despues de n una sola r\n"
	     "   [[\"vr\",  \"br\"]], // la r no va con la v\n"
	     "   [[\"vl\",  \"bl\"]], // la l no va con la v\n"
	     "   [[\"nb\",  \"mb\"]], // m antes de b\n"
	     "   [[\"np\",  \"mp\"]]  // m antes de p\n"
	     "]];\n"
	     "Text muyMal = \"El envlema de Enrrique enrrrrrriquecía "
	     "anplia y vravamente\";\n"
	     "Text ortRep = ReplaceTable(muyMal, ortTab);\n\n"
	     "Resultado: ortRep == \"El emblema de Enrique enriquecía "
	     "amplia y bravamente\"\n\n"
	     "3) Conversión de base octal a binaria (utiliza el 3er. "
	     "parámetro):\n\n"
	     "Set oct2bin =\n"
	     "[[ [[\"0\", \"000\"]],\n"
	     "   [[\"1\", \"001\"]],\n"
	     "   [[\"2\", \"010\"]],\n"
	     "   [[\"3\", \"011\"]],\n"
	     "   [[\"4\", \"100\"]],\n"
	     "   [[\"5\", \"101\"]],\n"
	     "   [[\"6\", \"110\"]],\n"
	     "   [[\"7\", \"111\"]]\n"
	     "]];\n"
	     "Text octstr = \"371\";\n"
	     "Text binstr = ReplaceTable(octstr,oct2bin,1);\n\n"
	     "Resultado: binstr == \"011111001\"\n"),
	  BOperClassify::Text_);
//--------------------------------------------------------------------
void BTxtReplaceTable::CalcContens()
//--------------------------------------------------------------------
{
    BSet& set = Set(Arg(2));
    int loops = 0;
    if(Arg(3)) loops = (int) Real(Arg(3));
    
    contens_ = Text(Arg(1));
    if(set.Card())
    {
	BArray<BParam> param;
	param.ReallocBuffer(set.Card());
	BInt m=0, n=1;
	for(; n<=set.Card(); n++)
	{
	    if(set[n]->Grammar()==GraSet())
	    {
		BSet& setn = Set(set[n]);
		if((setn.Card()==2)&&
		   (setn[1]->Grammar()==GraText())&&
		   (setn[2]->Grammar()==GraText()))
		{
		    BText txt1 = Text(setn[1]);
		    BText txt2 = Text(setn[2]);
		    // check txt2 in param[?] to verify not appear there
		    char jump = 0; // "1" if txt2 was introduced in txt1 before
		    for (int j=0; j<m && !jump; j++) {
			if(param[j].Name()==txt2) jump=1;
		    }
		    if (!jump) {
			param[m] = BParam(txt1, txt2);
		    }
		    m++;
		}
	    }
	}
	param.ReallocBuffer(m);
	BSeed seed(param);
	seed.Replace(contens_, loops);
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtChar);
DefExtOpr(1, BTxtChar, "Char", 1, 1, "Real",
	  "(Real n)",
	  I2("Returns the n-th ASCII character.",
	     "Devueve el enésimo caráter ASCII."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtChar::CalcContens()
//--------------------------------------------------------------------
{
    BChar ch = (BChar)Real(Arg(1));
    contens_.Copy(ch);
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtRepeatText);
DefExtOpr(1, BTxtRepeatText, "Repeat", 2, 2, "Text Real",
	  I2("(Text txt, Real n)",
	     "(Text txt, Real n)"),
	  I2("Returns the given text n times repeated.",
	     "Devueve el texto dado repetido n veces."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtRepeatText::CalcContens()
//--------------------------------------------------------------------
{
    BText txt = Text(Arg(1));
    BInt	n   = (BInt)Real(Arg(2));
    contens_.ReallocateBuffer(txt.Length()*n+1);
    contens_ = "";
    for(BInt i=0; i<n; i++) { contens_ += txt;  }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtSubString);
DefExtOpr(1, BTxtSubString, "Sub", 3, 3, "Text Real Real",
	  I2("(Text txt, Real from, Real until)",
	     "(Text txt, Real desde, Real hasta)"),
	  I2("Returns the substring of a text between two bounds, both included.",
	     "Devueve la subcadena comprendida entre dos límites ambos incluídos."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtSubString::CalcContens()
//--------------------------------------------------------------------
{
    contens_.Copy(Text(Arg(1)),(BInt)Real(Arg(2))-1,(BInt)Real(Arg(3))-1);
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BFirstTxtToUpper);
DefExtOpr(1, BFirstTxtToUpper, "FirstToUpper", 1, 2, "Text Real",
	  "(Text txt [, Real restToLower=FALSE])",
	  I2("Changes the first character of a text to upper case and makes lower "
	     "case the rest if restToLower is TRUE.",
	     "Cambia el primer caracter a mayúsculas y el resto a minúsculas si "
       "restToLower es CIERTO."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BFirstTxtToUpper::CalcContens()
//--------------------------------------------------------------------
{
  contens_=Text(Arg(1));
  if(Arg(2) && Real(Arg(2))) { contens_.ToLower(); }
  contens_.PutChar(0,(BChar)toupper(contens_.Get(0)));
}



//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtToUpper);
DefIntOpr(1, BTxtToUpper, "ToUpper", 1, 1,
	  "(Text txt)",
	  I2("Changes a text to upper case characters.",
	     "Cambia todos los caracteres a mayúsculas."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtToUpper::CalcContens()
//--------------------------------------------------------------------
{
    contens_=ToUpper(Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtToLower);
DefIntOpr(1, BTxtToLower, "ToLower", 1, 1,
	  "(Text txt)",
	  I2("Changes a text to upper case characters.",
	     "Cambia todos los caracteres a minúsculas."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtToLower::CalcContens()
//--------------------------------------------------------------------
{ 
    contens_=ToLower(Text(Arg(1))); 
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtToName);
DefIntOpr(1, BTxtToName, "ToName", 1, 1,
	  "(Text txt)",
	  I2("Changes a text to identifier valid characters.",
	     "Cambia todos los caracteres no válidos como identificador por "
	     "el carácter '_'."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtToName::CalcContens()
//--------------------------------------------------------------------
{
    contens_=ToName(Text(Arg(1)));
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtName);
DefExtOpr(1, BTxtName, "Name",   1, 1, "Anything",
	  "(Anything var)",
	  I2("Returns the name of a variable.",
	     "Devuelve el nombre de una variable."),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtName::CalcContens()
//--------------------------------------------------------------------
{ 
    contens_=Arg(1)->Name(); 
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtIdentify);
DefExtOpr(1, BTxtIdentify, "Identify",   1, 1, "Anything",
	  "(Anything var)",
	  I2("Returns the name of a variable if it has a name. Else returns its "
	     "description.",
	     "Devuelve el nombre de una variable si lo tiene. Si no devuelve su "
	     "descripción"),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtIdentify::CalcContens()
//--------------------------------------------------------------------
{ 
    contens_=Arg(1)->Identify(); 
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtDescription);
DefExtOpr(1, BTxtDescription, "Description",	 1, 1, "Anything",
	  "(Anything var)",
	  I2("Returns the description of a variable",
	     "Devuelve la descripción de una variable"),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtDescription::CalcContens()
//--------------------------------------------------------------------
{
  BSyntaxObject* arg1 = Arg(1);
  contens_=arg1->Description();
  if(!contens_.HasName() && 
      (arg1->Grammar()==GraCode()) && 
      (arg1->Mode()==BOBJECTMODE))
  {
    BUserCode* uCode = UCode(arg1);
    const BSpecialFunction* spf = uCode->Contens().SpecialFunction();
    if(spf)
    {
      contens_=spf->Description();
    }
    else
    {
      BOperator* opr = uCode->Contens().Operator();
      if(opr)
      {
        contens_=opr->Description();
        if(!contens_.HasName()) { contens_ = opr->Expression(); }
      }
      else
      {
        contens_="NOT FOUND";
      }
    }
  }
  if(!contens_.HasName()) { contens_ = arg1->Expression(); }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtExpression);
DefExtOpr(1, BTxtExpression, "Expression",	 1, 1, "Anything",
	  "(Anything var)",
	  I2("Returns the TOL expression of a variable",
	     "Devuelve la expressión TOL de una variable"),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtExpression::CalcContens()
//--------------------------------------------------------------------
{
  if((Arg(1)->Grammar()==GraCode()) && (Arg(1)->Mode()==BOBJECTMODE))
  {
	  BUserCode* uCode = UCode(Arg(1));
	  BOperator* opr = uCode->Contens().Operator();
	  contens_=opr->Expression();
  }
  else
  {
	  contens_=Arg(1)->Expression();
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtGrammar);
DefExtOpr(1, BTxtGrammar, "Grammar",	 1, 1, "Anything",
	  "(Anything var)",
	  I2("Returns the name of the grammar of a variable."
	     "If it does not exist any object with that name returns the empty chain."
	     "\nIf there are several objects with the same name in different "
	     "grammars only it is returned one of them .",
	     "Devuelve el nombre de la gramática de una variable. \n"
	     "Si no existe ningún objeto con ese nombre devuelve la cadena vacía. \n"
	     "Si hay varios objetos con el mismo nombre en diferentes gramáticas "
	     "sólo se devuelve una de ellas."),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtGrammar::CalcContens()
//--------------------------------------------------------------------
{
  BSyntaxObject* arg = Arg(1);
  if(arg->Mode()==BSTRUCTMODE)
  {
    contens_ = "Struct";
  }
  else
  {
    BGrammar* gra = arg->Grammar();
    contens_ = "";
    if(gra) { contens_ = gra->Name(); }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtGetTOLPath);
DefExtOpr(1, BTxtGetTOLPath, "GetTOLPath",   1, 1, "Anything",
	  "(Anything var)",
	  I2("Returns the TOLPath of variable."
	     "If it does not exist any object with that name returns the empty "
	     "chain. \n"
	     "If there are several objects with the same name in different grammars "
	     "only it is returned one of them .",
	     "Devuelve el camino TOL de una variable. \n"
	     "Si no existe ningún objeto con ese nombre devuelve la cadena vacía. \n"
	     "Si hay varios objetos con el mismo nombre en diferentes gramáticas "
	     "sólo se devuelve uno de ellas."),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtGetTOLPath::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Arg(1)->TolPath();
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtGetSourcePath);
DefExtOpr(1, BTxtGetSourcePath, "GetSourcePath",   1, 1, "Anything",
	  "(Anything var)",
	  I2("Returns the path of the file where the variable was created."
	     "If it does not exist any object with that name returns the empty "
	     "chain. \n"
	     "If there are several objects with the same name in different grammars "
	     "only it is returned one of them .",
	     "Devuelve el camino del fichero donde fue creada la variable. \n"
	     "Si no existe ningún objeto con ese nombre devuelve la cadena vacía. \n"
	     "Si hay varios objetos con el mismo nombre en diferentes gramáticas "
	     "sólo se devuelve uno de ellas."),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtGetSourcePath::CalcContens()
//--------------------------------------------------------------------
{
  contens_ =  Arg(1)->SourcePath();
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtArguments);
DefExtOpr(1, BTxtArguments, "Arguments",   1, 3, "Code Real Text",
	  I2("(Code function "
	     "[, Real argNum=0, Text infoType={\"\";\"TYPE\";\"NAME\"} ])])",
	     "(Code función "
	     "[, Real numArg=0, Text tipoInfo={\"\";\"TYPE\";\"NAME\"} ])"),
	  I2("Returns the arguments description for a function.\nThe 2nd "
	     "parameter argNum might be used to specify from which argument "
	     "we want to get data\n. The 3th parameter infoType might be "
	     "to specify what data we want: the parameter Type or the "
	     "parameter Name\n",
	     "Devuelve la descripción de los argumentos de una función.\n"
	     "El segundo parámetro puede ser usado para especificar el "
	     "número del argumento del cual queremos obtener información\n"
	     "El tercer parámetro puede ser utilizado para especificar que "
	     "dato queremos: el tipo del parámetro o su nombre\n"),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtArguments::CalcContens()
//--------------------------------------------------------------------
{
    BUserCode* uCode = UCode(Arg(1));

    BOperator* opr   = GetOperator(uCode);
    if(!opr) {
	contens_ = "error";
	return;
    }

    int   n = 0; if(Arg(2)) { n = (BInt)Real(Arg(2)); }
    BText i = ""; 
    if(Arg(3)) { i = Text(Arg(3)); }

    if(!n) {
	contens_=opr->Arguments();
    } else {
	int j = -1;
	if(i.ToUpper()=="TYPE") { j = 0; }
	if(i.ToUpper()=="NAME") { j = 1; }
	if(j>=0) { contens_ = opr->ArgTable()[n-1][j]; }
	else     { contens_ = opr->ArgTable()[n-1][0] + " " 
		       + opr->ArgTable()[n-1][1]; 
	}
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtCodeGrammar);
DefExtOpr(1, BTxtCodeGrammar, "CodeGrammar",	 1, 1, "Code",
	  "(Code function)",
	  I2("Returns the name of the grammar returned by a function."
	     "If it does not exist any function with that name returns the empty "
	     "chain.\n If there are several functions with the same name in "
	     "different grammars only it is returned one of them .",
	     "Devuelve el nombre de la gramática devuelta por una función. \n"
	     "Si no existe ningúna función con ese nombre devuelve la cadena vacía. "
	     "\nSi hay varias funciones con el mismo nombre en diferentes "
	     "gramáticas sólo se devuelve una de ellas."),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtCodeGrammar::CalcContens()
//--------------------------------------------------------------------
{
    BUserCode* uCode = UCode(Arg(1));
    BOperator* opr   = uCode->Contens().Operator();
    BGrammar*  gra   = NIL;
    if(opr) { gra = opr->Grammar(); }
    if(gra) { contens_ = gra->Name(); }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtStructName);
DefExtOpr(1, BTxtStructName, "StructName",   1, 1, "Set",
	  "(Set s)",
	  I2("Returns the name of the structure of a set. If it has no structure "
	     "then returns the null string.",
	     "Devuelve el nombre de la estructura de un conjunto. Si no la tiene "
	     "devuelve la cadena vacía."),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BTxtStructName::CalcContens()
//--------------------------------------------------------------------
{
    BStruct* str = Set(Arg(1)).Struct(true);
    if(str) { contens_ = str->Name(); }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BPutTableRealFormat);
DefExtOpr(1, BPutTableRealFormat, "PutTableRealFormat", 1, 2, "Anything Real",
	  I2("(Anything lengthOrFormatText       [, Real precision=0])",
	     "(Anything longitudOCadenaDeFormato [, Real precision=0])"),
	  I2("Change the default real format. There are two posible uses in "
	     "order to allow backward compatibility:\n" 
	     " (1.- Correct)    PutTableRealFormat(Text format)\n"
	     " (2.- Deprecated) PutTableRealFormat(Real length, "
	     "Real precision)\n"
	     "Returns the old format string. See FormatReal function to "
	     "know more about number formating."
	     ,
	     "Cambia el formato de números reales por defecto. Hay dos "
	     "posibles usos para garantizar la compatibilidad hacia atrás:\n"
	     " (1.- Correcto) PutTableRealFormat(Text formato)\n"
	     " (2.- Obsoleto) PutTableRealFormat(Real longitud, "
	     "Real precision)\n"
	     "Devuelve la cadena de formato que se acaba de reemplazar."
	     "Mirar la función FormatReal para saber más acerca de formatos "
	     "de números."),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
/*! \sa BSeriesTable::CreateTextTable (srg.cpp)
 */
void BPutTableRealFormat::CalcContens()
{
    contens_ = BTableFormat::RealFormat();
    if(Arg(2))
	BTableFormat::PutRealFormat((int) Real(Arg(1)), (int) Real(Arg(2)));
    else BTableFormat::PutRealFormat(Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BPutTableDateFormat);
DefExtOpr(1, BPutTableDateFormat, "PutTableDateFormat",  1, 2, "Anything Text",
	  I2("(Anything lengthOrFormatText       [, Text format=\"\"])",
	     "(Anything longitudOCadenaDeFormato [, Text format=\"\"])"),
	  I2("Change the default date format for tables. There are two posible uses in "
	     "order to allow backward compatibility:\n" 
	     " (1.- Correct) PutTableDateFormat(Text format)\n"
	     " (2.- Deprecated) PutTableDateFormat(Real length, Text format)\n"
	     "Returns the old format string. See FormatDate fuunction to "
	     "know more about date formating."
	     ,
	     "Cambia el formato de fechas por defecto en las tablas. Hay dos "
	     "posibles usos para garantizar la compatibilidad hacia atrás:\n"
	     " (1.- Correcto) PutTableDateFormat(Text formato)\n"
	     " (2.- Obsoleto) PutTableDateFormat(Real longitud, "
	     "Text formato)\n"
	     "Devuelve la cadena de formato que se acaba de reemplazar."
	     "Mirar la función FormatFate para saber más acerca de formatos "
	     "de fecha."),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BPutTableDateFormat::CalcContens()
{
    contens_ = BTableFormat::DateFormat().Format();
    if(Arg(2))
    {
	BTableFormat::PutFirstLength((int) Real(Arg(1)));
	BTableFormat::PutDateFormat(BDateFormat(Text(Arg(2))));
    }
    else
    {
	BTableFormat::PutDateFormat(BDateFormat(Text(Arg(1))));
    }
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BPutRealFormat);
DefExtOpr(1, BPutRealFormat, "PutRealFormat", 1, 1, "Text",
	  I2("(Text format)",
	     "(Text formato)"),
	  I2("Change the default real format. ( C and C++ string format ) ",
	     "Cambia el formato de números reales por defecto."
	     "( Cadenas de formato C y C++ )."),
	  BOperClassify::Conversion_);

//--------------------------------------------------------------------
void BPutRealFormat::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = BDat::RealFormat();
    BDat::PutRealFormat(Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtFormatReal);
DefExtOpr(1, BTxtFormatReal, "FormatReal", 1, 2, "Real Text",
	  I2("(Real value [, Text format])",
	     "(Real valor [, Text formato])"),
	  I2("Converts a real number in text with a given format. "
	     "If no format is given, default format is used, '%lf'.\n"
	     "  value: Number to convert.\n"
	     "  format: Format to apply.\n"
	     "   %[flags] [width] [.precision] [Size Prefixes] [type]\n"
	     "    [flags]\n"
	     "     - : Left align the result within the given field width.\n"
	     "         Default right align.\n"
	     "     + : Prefix the output value with a sign (+ or -).\n"
	     "     0 : If width is prefixed with 0, zeros are added until the"
	     " minimum width is reached.\n"
	     "         If 0 and - appear, the 0 is ignored. Default no padding.\n"
	     "    [width]\n"
	     "     n : Number of digitos of the whole part.\n" 
	     "    [.precision]\n" 
	     "     m : Number of digits of the part decimal.\n"
	     "    [Size Prefixes]\n"
	     "     l -> Long.\n"
	     "     L -> Long.\n" 
	     "    [type]\n"
	     "     e : Signed value having the form [-]d.dddd e [sign]ddd\n"
	     "     E : Identical to the e format except that E rather than e"
	     " introduces the exponent.\n"
	     "     f : Signed value having the form [-]dddd.dddd.\n"
	     "     g : Signed value printed in f or e format, whichever is more"
	     " compact for the given value and precision.\n"
	     "         The e format is used only when the exponent of the value"
	     " is less than -4 or greater than or equal to the"
	     " precision argument.\n"
	     "	       Trailing zeros are truncated, and the decimal point"
	     " appears only if one or more digits follow it.\n"
	     "     G : Identical to the g format, except that E, rather than e,"
	     " introduces the exponent (where appropriate)."
	     ,
	     "Convierte un número real a texto con un formato dado. Si no se da"
	     " se utiliza el formato por defecto, '%lf'.\n"
	     "  valor: Número a convertir.\n"
	     "  formato: Formato a aplicar.\n"
	     "   %[flags] [width] [.precision] [Size Prefixes] [type]\n"
	     "    [flags]\n"
	     "     - : Alinea el resultado a obtenido a la izquierda del ancho"
	     "         especificado. Por defecto a la derecha.\n"
	     "     + : Indica el signo del número en la salida (+ o -).\n"
	     "     0 : Rellena el ancho del prefijo con ceros, si este es mayor"
	     " que la parte entera. Sí 0 y - aparecen, 0 es ignorado.\n"
	     "         Por defecto no aparecen.\n"
	     "    [width]\n"
	     "     n : Número de dígtos de la parte entera.\n" 
	     "    [.precision]\n" 
	     "     m : Número de díagitos de la parte decimal.\n"
	     "    [Size Prefixes]\n"
	     "     l -> Long.\n"
	     "     L -> Long.\n" 
	     "    [type]\n"
	     "     e : Valor con signo con la forma [-]d.dddd e [sign]ddd\n"
	     "     E : Igual que el formato e. Se reemplaza E por e.\n"
	     "     f : Valor con signo con la forma [-]dddd.dddd.\n"
	     "     g : Utiliza el formato f o e, el mas apropiado en cada caso.\n"
	     "         El formato e solo se utilza si el exponente es menor que"
	     " -4, o mayor o igual que la precisión del argumento.\n"
	     "     G : Igual que le formato g, exceptuando que se reemplaza"
	     " E por E.\n."
	     ),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BTxtFormatReal::CalcContens()
//--------------------------------------------------------------------
{
  BDat	dat = Dat(Arg(1));
  BText format = BDat::RealFormat();
  if(Arg(2)) { format = Text(Arg(2)); }
    contens_ = dat.Format(format);
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BPutDateFormat);
DefExtOpr(1, BPutDateFormat, "PutDateFormat", 1, 1,
	  "Text",
	  I2("(Text format)",
	     "(Text formato)"),
	  I2("Change the default date format.",
	     "Cambia el formato de fechas por defecto."),
	  BOperClassify::Conversion_);

//--------------------------------------------------------------------
void BPutDateFormat::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = BDateFormat::Defect().Format();
    BDateFormat::Defect() =  BDateFormat(Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BGetDumpFile);
DefExtOpr(1, BGetDumpFile, "GetDumpFile", 1, 1, "Real",
	  "(Real unused)",
	  I2("Returns the default dump file path.",
	     "Devuelve la ruta del fichero de volcado por defecto."),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BGetDumpFile::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = BOut::DumpFile();
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BPutDumpFile);
DefExtOpr(1, BPutDumpFile, "PutDumpFile", 1, 2, "Text Real",
	  I2("(Text format [, Real rewrite=TRUE])",
	     "(Text formato [, Real reescribir=CIERTO])"),
	  I2("Change the default dump file. If the second parameter is TRUE then "
	     "the file is rewrited, else the new messages will be append to the "
	     "end of the file, if this exists.",
	     "Cambia el fichero de volcado por defecto. Si el segundo parámetro es "
	     "CIERTO el fichero se sobreescribe y en otro caso los mensajes se "
	     "añaden al final del fichero, si es que éste existe."),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BPutDumpFile::CalcContens()
//--------------------------------------------------------------------
{
  BBool rewrite = BTRUE;
  if(Arg(2)) { rewrite = (BBool)Real(Arg(2)); }
  contens_ = BOut::DumpFile();
  BOut::PutDumpFile(Text(Arg(1)),rewrite);
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtFormatDate);
DefExtOpr(1, BTxtFormatDate, "FormatDate",	1, 2, "Date Text",
	  I2("(Date date [, Text format])",
	     "(Date fecha [, Text formato])"),
	  I2("Converts a date in text with a format."
	     "If no format are given default date format is used"
		 " (%cy%Ym%md%d%uh%hi%is%s).\n"
		 "  date : Date to convert.\n"
		 "  format : format to apply.\n"
         "    u : If an element does not exist, it does not show it.\n" 
         "    c : Show nonexisting elements.\n" 
         "    Y : Long year (yyyy).\n"
         "    y : Short year (yy).\n"
         "    m : Month (mm).\n"
         "    n : Short month name (mmm).\n"
         "    N : Long month name (mmmm).\n"
         "    d : Day of month (dd).\n"
         "    w : Short week day name (ddd).\n"
         "    W : Long week day name (dddd).\n"
         "    h : Hour.\n"
         "    i : Minute.\n"
         "    s : Second.\n",
	     "Convierte una fecha en texto con un formato dado."
	     "Si no se da el formato se utiliza el formato de fechas por defecto."
		 " (%cy%Ym%md%d%uh%hi%is%s).\n"
		 "  fecha : Fecha a convertir.\n"
		 "  formato : Formato a aplicar.\n"
		 "    u : Quita del resultado los elementos no existentes.\n"
         "    c : Fuerza la aparición de elementos no existentes.\n"
         "    Y : Año largo (yyyy).\n"
         "    y : Año corto (yy).\n"
         "    m : Mes en número (mm).\n"
         "    n : Nombre del mes corto (mmm).\n"
         "    N : Nombre del mes largo (mmmm).\n"
         "    d : Día del mes (dd).\n"
         "    w : Nombre corto del día de la semana (ddd).\n"
         "    W : Nombre largo del día de la semana (dddd).\n"
         "    h : Hora.\n"
         "    i : Minuto.\n"
         "    s : Segundo.\n"),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BTxtFormatDate::CalcContens()
//--------------------------------------------------------------------
{
    BDate dte  = Date(Arg(1));
    BDateFormat fmt = BDateFormat::Defect();
    if(Arg(2)) { fmt = BDateFormat(Text(Arg(2))); }
    contens_ = fmt.DateToText(dte);
}

#ifdef __USE_TC__
//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtFromCTime);
DefExtOpr(
    1, BTxtFromCTime, "TextFromCTime", 2, 2, "CTime Text",
    "(CTime tmi, Text format)",
    I2("Formats the given CTime according to the format specification "
       "given in the 2nd. parameter Text format.\n\n"
       "Ordinary characters placed in the format string are copied to "
       "the output Text without conversion. Conversion specifiers are "
       "introduced by a `%' character, and are replaced in s as "
       "follows:\n\n"
       "%a   The abbreviated weekday name according to the current locale.\n\n"
       "%A   The full weekday name according to the current locale.\n\n"
       "%b   The abbreviated month name according to the current locale.\n\n"
       "%B   The full month name according to the current locale.\n\n"
       "%c   The preferred date and time representation for the current "
       "locale.\n\n"
       "%C   The century number (year/100) as a 2-digit integer. (SU)\n\n"
       "%d   The day of the month as a decimal number (range 01 to 31).\n\n"
       "%D   Equivalent  to %m/%d/%y. (For Americans only. Americans should "
       "note that in other countries %d/%m/%y is rather common. This "
       "means that in international context this format is ambiguous "
       "and should not be used.) (SU)\n\n"
       "%e   Like %d, the day of the month as a decimal number, but a "
       "leading zero is replaced by a space. (SU)\n\n"
       "%E   Modifier: use alternative format, see below. (SU)\n\n"
       "%F   Equivalent to %Y-%m-%d (the ISO 8601 date format). (C99)\n\n"
       "%G   The ISO 8601 year with century as a decimal number. "
       "The 4-digit year  corresponding to the ISO week number (see %V). "
       "This has the same format and value as %y, except that if the ISO "
       "week number belongs to the previous or next year, that year is used "
       "instead.(TZ)\n\n"
       "%g   Like %G, but without century, i.e., with a 2-digit year "
       "(00-99). (TZ)\n\n"
       "%h   Equivalent to %b. (SU)\n\n"
       "%H   The hour as a decimal number using a 24-hour clock (range 00 "
       "to 23).\n\n"
       "%I   The hour as a decimal number using a 12-hour clock (range 01 "
       "to 12).\n\n"
       "%j   The day of the year as a decimal number (range 001 to 366).\n\n"
       "%k   The hour (24-hour clock) as a decimal number (range 0 to 23); "
       "single digits are preceded by a blank. (See also %H.) (TZ)\n\n"
       "%l   The hour (12-hour clock) as a decimal number (range 1 to 12); "
       "single digits are preceded by a blank. (See also %I.) (TZ)\n\n"
       "%m   The month as a decimal number (range 01 to 12).\n\n"
       "%M   The minute as a decimal number (range 00 to 59).\n\n"
       "%n   A newline character. (SU)\n\n"
       "%O   Modifier: use alternative format, see below. (SU)\n\n"
       "%p   Either  `AM' or `PM' according to the given time value, or the "
       "corresponding strings for the current locale. Noon is treated as "
       "`pm' and midnight as `am'.\n\n"
       "%P   Like %p but in lowercase: `am' or `pm' or a corresponding "
       "string for the current locale.(GNU)\n\n"
       "%r   The time in a.m. or p.m. notation. In the POSIX locale this is "
       "equivalent to `%I:%M:%S %p'. (SU)\n\n"
       "%R   The time in 24-hour notation (%H:%M).(SU) For a version "
       "including the seconds, see %T below.\n\n"
       "%s   The number of seconds since the Epoch, i.e., since 1970-01-01 "
       "00:00:00 UTC. (TZ)\n\n"
       "%S   The second as a decimal number (range 00 to 60). (The range "
       "is up to 60 to allow for occasional leap seconds.)\n\n"
       "%t   A tab character. (SU)\n\n"
       "%T   The time in 24-hour notation (%H:%M:%S). (SU)\n\n"
       "%u   The day of the week as a decimal, range 1 to 7, Monday being 1. "
       "See also %w. (SU)\n\n"
       "%U   The week number of the current year as a decimal number, range "
       "00 to 53, starting with the first Sunday as the first day of week 01."
       " See also %V and %W.\n\n"
       "%V   The ISO 8601:1988 week number of the current year as a decimal "
       "number, range 01 to 53, where week 1 is the first week that has at "
       "least 4 days in the current year, and with Monday as the first day "
       "of the week. See also %U and %W. (SU)\n\n"
       "%w   The day of the week as a decimal, range 0 to 6, Sunday being 0. "
       "See also %u.\n\n"
       "%W   The week number of the current year as a decimal number, range "
       "00 to 53, starting with the first Monday as the first day of week "
       "01.\n\n"
       "%x   The preferred date representation for the current locale "
       "without the time.\n\n"
       "%X   The preferred time representation for the current locale "
       "without the date.\n\n"
       "%y   The year as a decimal number without a century (range 00 to "
       "99).\n\n"
       "%Y   The year as a decimal number including the century.\n\n"
       "%z   The time-zone as hour offset from GMT. Required to emit "
       "RFC822-conformant dates (using \"%a, %d %b %Y %H:%M:%S %z\").(GNU)\n\n"
       "%Z   The time zone or name or abbreviation.\n\n"
       "%+   The date and time in date(1) format. (TZ)\n\n"
       "%%   A literal `%' character.\n\n"
       "Some conversion specifiers can be modified by preceding them by the "
       "E or O modifier to indicate that an alternative format should be "
       "used. If the alternative format or specification does not exist for "
       "the current locale, the behaviour will be as if the unmodified "
       "conversion specification were used. (SU) The Single Unix "
       "Specification mentions %Ec, %EC, %Ex,  %EX, %Ry, %EY, %Od, %Oe, "
       "%OH, %OI, %Om, %OM, %OS, %Ou, %OU, %OV, %Ow, %OW, %Oy, where the "
       "effect of the O modifier is to use alternative numeric symbols "
       "(say,  roman  numerals), and that of the E modifier is to use a "
       "locale-dependent alternative representation."
       ,
       "Da formato al parámetro CTime de acuerdo con el formato especificado "
       "en el parámetro format. Los caracteres normales de la cadena de "
       "formato se copian en s sin conversión ninguna. Los especificadores "
       "de conversión se introducen con un carácter de porcentaje `%', y se "
       "reem plazan en s como sigue:\n\n"
       "%a   El nombre abreviado del día de la semana según la localización "
       "en curso.\n\n"
       "%A   El nombre del día de la semana, completo, según la localización "
       "en curso.\n\n"
       "%b   El nombre abreviado del mes segúnn la localización en curso.\n\n"
       "%B   El nombre completo del mes según la localización en curso.\n\n"
       "%c   La representación preferida de fecha y hora para la "
       "localización en curso.\n\n"
       "%C   El siglo (año/100) como un entero de 2 dígitos. (SU)\n\n"
       "%d   El día del mes como un número en base diez (en el rango de 01 "
       "a 31).\n\n"
       "%D   Equivalente a %m/%d/%y. (Sip - sólo para norteamericanos. Los "
       "norteamericanos deben darse cuenta que en otros países %d/%m/%y es "
       "bastante común. Esto significa que en un contexto internacional "
       "este formato es ambiguo y no se debería usar). (SU)\n\n"
       "%e   Como %d, el día del mes como un número decimal, pero un cero "
       "inicial se reemplaza por un espacio.(SU)\n\n"
       "%E   Modificador; use formato alternativo, ver más abajo. (SU)\n\n"
       "%F   Equivalente a %Y-%m-%d (el formato de fecha de ISO 8601). "
       "(C99)\n\n"
       "%G   El  año con siglo como un número decimal según el estándar "
       "ISO 8601. El año de 4 dígitos correspondiente al número de la semana "
       "ISO (ver %V).  Éste tiene el mismo formato y valor que %y, salvo que "
       "si el número de la semana ISO pertenece al año anterior o siguiente, "
       "ese año se utiliza en su lugar. (TZ).\n\n"
       "%g   Como %G, pero sin siglo, es decir, con un año de 2 dígitos "
       "(00-99). (TZ)\n\n"
       "%h   Equivalente a %b. (SU)\n\n"
       "%H   La hora como un número en base diez en formato de 24 horas "
       "(en el rango de 00 a 23).\n\n"
       "%I   La hora como un número en base diez en formato de 12 horas (en "
       "el rango de 01 a 12).\n\n"
       "%j   El día juliano (día del año) como un número en base diez (en "
       "el rango de 001 a 366).\n\n"
       "%k   La hora (en un reloj de 24 horas) como un número decimal (en "
       "el rango de 0 a 23); los dígitos individuales son precedidos por un "
       "blanco. (Ver también %H). (TZ)\n\n"
       "%l   La hora (en un reloj de 12 horas) como un número decimal (en "
       "el rango de 1  a  12); los dígitos individuales son precedidos por "
       "un blanco. (Ver tambíen %I). (TZ).\n\n"
       "%m   El mes como un número en base diez (en el rango de 01 a 12).\n\n"
       "%M   El minuto como un número en base diez (en el rango de 00 a "
       "59).\n\n"
       "%n   Un carácter de nueva línea. (SU)\n\n"
       "%O   Modificador; use un formato alternativo, ver más abajo. (SU)\n\n"
       "%p   O `AM' (\"Ante Meridiem\", antes del mediodía) o `PM' (\"Post "
       "Meridiem\", después del mediodía) de acuerdo con el valor del "
       "tiempo dado, o la cadena de caracteres correspondiente  según la "
       "localización en curso. El mediodía se trata como `pm' y la media "
       "noche como `am'.\n\n"
       "%P   Como %p pero en letras minúsculas: `am' o `pm' o una cadena "
       "correspondiente para la localización actual. (GNU)\n\n"
       "%r   El tiempo en notación a.m. o p.m. En la localización POSIX "
       "esto es equivalente a `%I:%M:%S %p'. (SU)\n\n"
       "%R   El tiempo en notación de 24 horas (%H:%M). (SU) Para una "
       "versión que incluya los segundos, ver %T más abajo.\n\n"
       "%s   El número de segundos desde la época, es decir, desde "
       "1970-01-01 00:00:00 UTC. (TZ)\n\n"
       "%S   El segundo como un número decimal (en el rango de 00 a 61)\n\n"
       "%t   Un carácter tabulador. (SU)\n\n"
       "%T   El tiempo en notación de 24 horas (%H:%M:%S). (SU)\n\n"
       "%u   El día de la semana como un número decimal, en el rango de 1 a "
       "7, siendo 1 el Lunes. Ver también %w. (SU)\n\n"
       "%U   El número de la semana del año actual como un número en base "
       "decimal, en el rango de 00 a 53, empezando por el primer domingo "
       "como el primer día de la primera semana. Ver también %V y %W.\n\n"
       "%W   El  número de la semana del año actual como un número decimal "
       "según el estandar ISO 8601:1988, donde la semana 1 es la primera "
       "semana que tiene al menos 4 días del año actual y el lunes como el "
       "primer día de la semana. Ver también %U y %W. (SU)\n\n"
       "%w   El día de la semana como un número decimal, en el rango de 0 "
       "a 6, siendo el domingo el cero. Ver también %u.\n\n"
       "%x   La  representación preferida de la fecha (sin la hora) para "
       "la localización en curso.\n\n"
       "%X   La representación preferida de la hora (sin la fecha) para la "
       "localización en curso.\n\n"
       "%y   El año como un número en base diez sin la centuria (en el "
       "rango de 00 a 99).\n\n"
       "%Y   El año como un número en base diez, incluyendo la centuria.\n\n"
       "%z   El huso horario como las horas de diferencia respecto a GMT."
       "Necesario para emitir fechas conformes a RFC822 (usando \"%a, %d %b "
       "%Y %H:%M:%S %z\"). (GNU)\n\n"
       "%Z   La zona horaria, nombre o abreviatura.\n\n"
       "%+   La fecha y hora en el formato de date(1). (TZ)\n\n"
       "%%   Un carácter de porcentaje literal, '%'.\n\n"
       "Algunos indicadores de conversión se pueden modificar precediéndolos "
       "por un modificador E u O para indicar que se debe usar un formato "
       "alternativo.  Si no existen formatos o especificaciones alternativos "
       "para la localización actual, el comportamiento quedará como si se "
       "usara la especificación de conversión sin modificar. (SU) `The "
       "Single Unix Specification' menciona %Ec, %EC, %Ex, %EX, %Ry, %EY, "
       "%Od, %Oe, %OH, %OI, %Om, %OM, %OS,  %Ou,  %OU,  %OV, %Ow,  %OW, %Oy, "
       "donde el efecto del modificador O es el de usar símbolos numéricos "
       "alternativos (digamos, números romanos) y el del modificador E es el "
       "de usar un representación alternativa dependiente de la "
       "localización."), BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BTxtFromCTime::CalcContens()
{
    BCTime tmi  = CTime(Arg(1));
    BText fmt = Text(Arg(2));
    contens_ = tmi.format(fmt.Buffer()).c_str();
}
#endif // __USE_TC__

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtFormatMatrix);
DefExtOpr(1, BTxtFormatMatrix, "FormatMatrix",    1, 3,
	  "Matrix Text Text",
	  I2("(Matrix M [, Text colSep=\";\", Text rowSep=NL])",
	     "(Matrix M [, Text colSep=\";\", Text rowSep=NL])"),
	  I2("Converts a matrix in text with a given format. If no format is"
	     " given, default format is used.",
	     "Convierte una matriz a texto con un formato dado. Si no se da"
	     " se utiliza el formato por defecto."),
	  BOperClassify::Conversion_);
void BTxtFormatMatrix::CalcContens()
//--------------------------------------------------------------------
{
    BMat	mat = Mat(Arg(1));
    BText format = BDat::RealFormat();
    BText colSep = ";";
    BText rowSep = "\n";
    if(Arg(2)) { colSep = Text(Arg(2)); }
    if(Arg(3)) { rowSep = Text(Arg(3)); }
    contens_ = "";
    for(BInt i=0; i<mat.Rows(); i++)
    {
	if(i>0) { contens_ += rowSep; }
	for(BInt j=0; j<mat.Columns(); j++)
	{
	    if(j>0) { contens_ += colSep; }
	    contens_ += mat(i,j);
	}
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtFormatPolyn);
DefExtOpr(1, BTxtFormatPolyn, "FormatPolyn",	  1, 1,
	  "Polyn",
	  I2("(Polyn P)",
	     "(Polyn P)"),
	  I2("Converts a polynomial in text with a given format. If no format is"
	     " given, default format is used.",
	     "Convierte un polinomio a texto con un formato dado. Si no se da"
	     " se utiliza el formato por defecto."),
	  BOperClassify::Conversion_);

//--------------------------------------------------------------------
void BTxtFormatPolyn::CalcContens()
//--------------------------------------------------------------------
{
//  BPol  pol = Pol(Arg(1));
//  contens_ = pol.Name();
    contens_ = Arg(1)->Dump();
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtDatingName);
DefExtOpr(1, BTxtDatingName, "DatingName",   1, 1, "Serie",
	  "(Serie ser)",
	  I2("Returns the dating name of a serie.",
	     "Devuelve el nombre del fechado de una serie."),
	  BOperClassify::Conversion_);

//--------------------------------------------------------------------
void BTxtDatingName::CalcContens()
//--------------------------------------------------------------------
{
    BUserTimeSerie* ser = Tsr(Arg(1));
    BUserTimeSet*	  tms = ser->Dating();
    BText		  name;
    if(tms) { contens_ = tms->Identify(); }
    else	  { contens_ = ""; }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtListOfDates);
DefExtOpr(1, BTxtListOfDates, "ListOfDates",	 1, 3, "TimeSet Date Date",
	  I2("(TimeSet dating [, Date from=DefFirst, Date until=DefLast])",
	     "(TimeSet fechado [, Date desde=DefFirst, Date hasta=DefLast])"),
	  I2("Returns a comma separated text width all dates of a dating between"
	     "two given dates.",
	     "Devuelve un texto separado por comas con todas las fechas de un "
	     "fechado entre dos fechas dadas."),
	  BOperClassify::Conversion_);

//--------------------------------------------------------------------
void BTxtListOfDates::CalcContens()
//--------------------------------------------------------------------
{
    BUserTimeSet* tms = Tms(Arg(1));
    BDate		f   = BDate::DefaultFirst();
    BDate		l   = BDate::DefaultLast ();
    if(Arg(2)) { f = Date(Arg(2)); }
    if(Arg(3)) { l = Date(Arg(3)); }
    contens_ = tms->ListOfDates(f,l,", ",0);
}



//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtShowFile);
DefExtOpr(1, BTxtShowFile, "ShowFile",   1, 1, "Text",
	  I2("(Text fileName)",
	     "(Text nombreFichero)"),
	  I2("Shows a text file of valid extension (TOL,BDT,BST,BMT,...)",
	     "Muestra un fichero de texto de extensión válida (TOL,BDT,BST,BMT,...)"),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtShowFile::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Text(Arg(1));
    BSys::Edit(contens_,1);
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtReadFile);
DefExtOpr(1, BTxtReadFile, "ReadFile",   1, 1, "Text",
	  I2("(Text fileName)",
	     "(Text nombreFichero)"),
	  I2("Returns the contents of a text file. If the file not exists returns "
	     "the void string and displays a warning message.",
	     "Devuelve el contenido de un fichero de texto. Si el fichero no existe "
	     " devuelve la cadena vacía y muestra un mensaje de advertencia."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtReadFile::CalcContens()
//--------------------------------------------------------------------
{
    ReadFile(contens_, Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtWriteFile);
DefExtOpr(1, BTxtWriteFile, "WriteFile",   2, 2, "Text Text",
	  I2("(Text fileName, Text string)",
	     "(Text nombreFichero, Text cadena)"),
	  I2("Writes a text in a file. If the file not exists returns "
	     "the void string and displays a warning message; else it just returns "
	     "the file name.",
	     "Escribe una cadena en un fichero. Si el fichero no existe "
	     "devuelve la cadena vacía y muestra un mensaje de advertencia."
	     "Si existe devuelve el propio nombre del fichero."),
	  BOperClassify::Text_);

//--------------------------------------------------------------------
void BTxtWriteFile::CalcContens()
//--------------------------------------------------------------------
{
    BText fileName = Text(Arg(1));
    if(OverWrite(fileName,Text(Arg(2))))
    {
	contens_ = fileName;
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtAppendFile);
DefExtOpr(1, BTxtAppendFile, "AppendFile",   2, 2, "Text Text",
	  I2("(Text fileName, Text string)",
	     "(Text nombreFichero, Text cadena)"),
	  I2("Appends a text at the end of a file. If the file not exists returns "
	     "the void string and displays a warning message; else it just returns "
	     "the file name.",
	     "Añade una cadena al final un fichero. Si el fichero no existe "
	     "devuelve la cadena vacía y muestra un mensaje de advertencia."
	     "Si existe devuelve el propio nombre del fichero."),
	  BOperClassify::Text_);
void BTxtAppendFile::CalcContens()
//--------------------------------------------------------------------
{
    BText fileName = Text(Arg(1));
    if(AppendWrite(fileName,Text(Arg(2))))
    {
	contens_ = fileName;
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtPeriodicNull);
DefExtOpr(1, BTxtPeriodicNull, "PeriodicNull", 2, 2, "Serie Real",
	  I2("(Serie ser, Real period)",
	     "(Serie ser, Real periodo)"),
	  I2("Returns the expression of periodic time intervals that a serie take "
	     "null value in. You can to obtain the time set using the function Eval.",
	     "Devuelve la expresion de los intervalos temporales periódicos en los "
	     "que se anula una serie. Se puede obtener el conjunto temporal usando "
	     "la funcion Eval."),
	  BOperClassify::TimeSeriesAlgebra_);

//--------------------------------------------------------------------
void BTxtPeriodicNull::CalcContens()
//--------------------------------------------------------------------
{
    contens_="";
    BUserTimeSerie* s = Tsr (Arg(1));	      if(!s)          { return; }
    BUserTimeSet* tms = s->Dating();	      if(!tms)	      { return; }
    BInt	  per = (BInt)Real(Arg(2));   if(per<=1)      { return; }
    BInt	    n = 0;
    BInt	nulls = 0;
    BDate       first = s->FirstDate(); if(!first.HasValue()) { return; }
    BDate        last = s-> LastDate(); if(!last. HasValue()) { return; }
    BHash	 hash; tms->GetHashBetween(hash, first, last);
    BInt	  len = hash.Size();	if(len<=per)          { return; }
    for(; n<=len; n++)
    {
	BDate dte; dte.PutHash(hash[n]);
	BDat x = (*s)[dte];
	if(x==0) { nulls++; }
	else
	{
	    if(nulls >= per)
	    {
		BInt extract	 = (BInt)(floor((double)nulls/per)*per);
		BInt firstClosed = n-extract;
		BInt lastClosed	 = n-1;
		if(contens_.HasName()) { contens_ += " + "; }
		contens_ += BText("In(")+
		    HashToDte(hash[firstClosed]).Name() + ", " +
		    HashToDte(hash[lastClosed ]).Name() + ")";
	    }
	    nulls = 0;
	}
    }
}


//--------------------------------------------------------------------
void Tokenizer(BSet& set, const BText& txt, BChar sep)
//--------------------------------------------------------------------
{
  BArray<BText> tok;
  ReadAllTokens(txt,tok,sep);
  set.PrepareStore(tok.Size());
//Std(BText("\nTokenizer TARCE 1"));
  for(BInt i=0; i<tok.Size(); i++)
  {
    set.AddElement(new BContensText(tok[i]));
  }
//Std(BText("\nTokenizer TARCE 2"));
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetReadAllTokens);
DefExtOpr(1, BSetReadAllTokens, "Tokenizer", 2, 2,  "Text Text",
	  I2("(Text string, Text separator)",
	     "(Text cadena, Text separador)"),
	  I2(".",
	     "Divide un texto en cada una de las partes separadas por el caracter "
	     "de separacion dado."),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetReadAllTokens::CalcContens()
//--------------------------------------------------------------------
{
  BText& txt = Text(Arg(1));
  BText& sep = Text(Arg(2));
  Tokenizer(contens_,txt,sep.Get(0));
}

//--------------------------------------------------------------------
void GetTableFromText(BSet& set, const BText& txt, BChar linSep, BChar celSep)
//--------------------------------------------------------------------
{
    BList* lst = NIL;
    BList* aux = NIL;
    BList* lstA = NIL;
    BList* auxA = NIL;
    BArray< BArray<BText> > cell;
    
    ReadAllCells(txt,cell,linSep,celSep);
    for(BInt i=0; i<cell.Size(); i++)
    {
	lstA = NIL;
	auxA = NIL;
	for(BInt j=0; j<cell[0].Size(); j++)
	{
	    LstFastAppend(lstA,auxA,new BContensText(cell[i][j]));
	}
	LstFastAppend(lst,aux,NewSet (lstA,NIL,BSet::Generic));
	//LstFastAppend(lst,aux,NewSet ("", "",lstA));
    }
    set.RobStruct(lst,NIL,BSet::Table);
    //set.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetReadAllCells);
DefExtOpr(1, BSetReadAllCells, "GetTableFromText", 3, 3,  "Text Text Text",
	  I2("(Text string, Text rowSeparator, Text cellSeparator)",
	     "(Text cadena, Text separadorDeLinea, Text separadorDeCelda)"),
	  I2(".",
	     "."),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetReadAllCells::CalcContens()
//--------------------------------------------------------------------
{
    BText txt = Text(Arg(1));
    BText lin = Text(Arg(2));
    BText cel = Text(Arg(3));
    GetTableFromText(contens_,txt,lin.Get(0),cel.Get(0));
}


//--------------------------------------------------------------------
unsigned long GenerateNextRandom(unsigned long rand)
//--------------------------------------------------------------------
{
    return((16807*rand)%2147483647);
};


union Byte4_Or_Long
{
    unsigned long _LongValue;
    char _Byte4Value[4];
};

//--------------------------------------------------------------------
void XORRandomCodify(BInt key,  BText& input)
//--------------------------------------------------------------------
{
    unsigned long rand = key;
    int i;
    BBool end = BFALSE;
    Byte4_Or_Long aux;
    
    for(i=0; i<7+key%17; i++) { rand = GenerateNextRandom(rand); }
    for(int k=0; k<input.Length(); k+=4)
    {
	rand = GenerateNextRandom(rand);
	aux._LongValue = rand;
	for(i=0; (i<4) && (i+k)<input.Length(); i++)
	{
	    input.PutChar(i+k,input(i+k) ^ aux._Byte4Value[i]);
	}
    }
};


//--------------------------------------------------------------------
void XORRandomCodifyFile(BInt key,
			 const BText& inputName,
			 const BText& outputName)
//--------------------------------------------------------------------
{
    unsigned long rand = key;
    int i;
    BBool end = BFALSE;
    Byte4_Or_Long aux;
    char inp, out;
    FILE* input  = fopen(inputName .String(),"rb");
    FILE* output = fopen(outputName.String(),"wb");
    
    if(!input)
    {
	Error(I2("Cannot open file ","No se pudo abrir el fichero ")+
	      inputName);
	return;
    }
    if(!output)
    {
	Error(I2("Cannot open file ","No se pudo abrir el fichero ")+
	      outputName);
	return;
    }
    
    for(i=0; i<7+key%17; i++) { rand = GenerateNextRandom(rand); }
    while(!feof(input))
    {
	rand = GenerateNextRandom(rand);
	aux._LongValue = rand;
	for(i=0; (i<4) && !feof(input); i++)
	{
	    fread(&inp, sizeof(char), 1, input);
	    if(!feof(input))
	    {
		out = inp ^ aux._Byte4Value[i];
		fwrite(&out, sizeof(char), 1, output);
	    }
	}
    }
    fclose(input);
    fclose(output);
};


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtEncryptFile);
DefExtOpr(1, BTxtEncryptFile, "ENCRYPT_FILE", 3, 3,  "Real Text Text",
	  I2("(Real key, Text inputFileName, Text outputFileName)",
	     "(Real clave, Text nombreFicheroEntrada, Text nombreFicheroSalida)"),
	  I2("",""),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BTxtEncryptFile::CalcContens()
//--------------------------------------------------------------------
{
    unsigned long startRand = BInt(Real(Arg(1)));
    BText inputName  = Text(Arg(2));
    BText outputName = Text(Arg(3));
    contens_ = outputName;
    XORRandomCodifyFile(startRand, inputName, outputName);
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtEncrypt);
DefExtOpr(1, BTxtEncrypt, "ENCRYPT", 2, 2,  "Real Text",
	  I2("(Real key, Text input)",
	     "(Real clave, Text entrada)"),
	  I2("",""),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BTxtEncrypt::CalcContens()
//--------------------------------------------------------------------
{
    unsigned long startRand = BInt(Real(Arg(1)));
    contens_ = Text(Arg(2));
    XORRandomCodify(startRand, contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtGetFilePath);
DefExtOpr(1, BTxtGetFilePath, "GetFilePath",	 1, 1, "Text",
	  I2("(Text filePath)",
	     "(Text caminoFichero)"),
	  I2("Returns the path of the owner directory of a file.\n"
	     "Sample: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "returns \n"
	     "\"c:/windows\"\n"
	     "GetFileName(\"../../notepad.exe\") \n"
	     "returns \n"
	     "\"../..\"\n",
	     "Devuelve el camino del directorio en el que se encuentra un fichero.\n"
	     "Ejemplo: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "devuelve \n"
	     "\"c:/windows\"\n"
	     "GetFileName(\"../../notepad.exe\") \n"
	     "devuelve \n"
	     "\"../..\"\n"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BTxtGetFilePath::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = GetFilePath(Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtGetFileName);
DefExtOpr(1, BTxtGetFileName, "GetFileName",	 1, 1, "Text",
	  I2("(Text filePath)",
	     "(Text caminoFichero)"),
	  I2("Returns the name of a file without path but with extension.\n"
	     "Sample: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "returns \n"
	     "\"notepad.exe\"\n",
	     "Devuelve el nombre de un fichero sin el camino pero con extensión.\n"
	     "Ejemplo: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "devuelve \n"
	     "\"notepad.exe\"\n"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BTxtGetFileName::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = GetFileName(Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtGetFilePrefix);
DefExtOpr(1, BTxtGetFilePrefix, "GetFilePrefix",   1, 1, "Text",
	  I2("(Text filePath)",
	     "(Text caminoFichero)"),
	  I2("Returns the name of a file without path nor extension.\n"
	     "Sample: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "returns \n"
	     "\"notepad\"\n",
	     "Devuelve el nombre de un fichero sin el camino ni extensión.\n"
	     "Ejemplo: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "devuelve \n"
	     "\"notepad\"\n"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BTxtGetFilePrefix::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = GetFilePrefix(Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtGetFileExtension);
DefExtOpr(1, BTxtGetFileExtension, "GetFileExtension",   1, 1, "Text",
	  I2("(Text filePath)",
	     "(Text caminoFichero)"),
	  I2("Returns the extension of a file.\n"
	     "Sample: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "returns \n"
	     "\"exe\"\n",
	     "Devuelve la extension de un fichero.\n"
	     "Ejemplo: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "devuelve \n"
	     "\"exe\"\n"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BTxtGetFileExtension::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = GetFileExtension(Text(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtGetAbsolutePath);
DefExtOpr(1, BTxtGetAbsolutePath, "GetAbsolutePath",	 1, 1, "Text",
	  I2("(Text filePath)",
	     "(Text caminoFichero)"),
	  I2("Returns the absolute path of the owner directory of a file.\n"
	     "Sample: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "returns \n"
	     "\"c:/windows\"\n"
	     "GetFileName(\"../../notepad.exe\") \n"
	     "returns \n"
	     "\"c:/windows\"\n",
	     "Devuelve el camino absoluto del directorio en el que se encuentra un "
	     "fichero.\n"
	     "Ejemplo: \n"
	     "GetFileName(\"c:/windows/notepad.exe\") \n"
	     "devuelve \n"
	     "\"c:/windows\"\n"
	     "GetFileName(\"../../notepad.exe\") \n"
	     "devuelve \n"
	     "\"c:/windows\"\n"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BTxtGetAbsolutePath::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = GetAbsolutePath(Text(Arg(1)));
}



//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtFormatSerSet);
DefExtOpr(1, BTxtFormatSerSet, "FormatSerSet",    3, 5,
	  "Set Text Set Date Date",
	  I2("(Set series , Text template, Set rules [, Date from, Date until] )",
	     "(Set series , Text plantilla, Set reglas [, Date desde, Date hasta] )"),
	  I2(".",
	     "Dado un conjunto de series escribe para cada fecha el texto resultante "
	     "de aplicar un conjunto de reglas a una plantilla dada. Una regla se "
	     "define mediante un conjunto que contiene por este orden un tag"),
	  BOperClassify::Conversion_);

//--------------------------------------------------------------------
void BTxtFormatSerSet::CalcContens()
//--------------------------------------------------------------------
{
    BInt i,j;
    BSet& set = Set(Arg(1));
    BDate f = BDate::Unknown();
    BDate l = BDate::Unknown();
    if(Arg(4)) { f = Date(Arg(4)); }
    if(Arg(5)) { l = Date(Arg(5)); }
    BSerieTable t;
    t.AddSet(set);
    t.Fill(f,l);
    if(!t.TestDates()) { return; }

    BText templateText = Text(Arg(2));
    BSet& rul	       = Set(Arg(3));
    BArray<BText> tag(rul.Card());
    BArray<BInt>	col(rul.Card());
    BArray<BText> fmt(rul.Card());
    BText txt;
    
    for(j=0; j<rul.Card(); j++)
    {
	BBool ok = rul[j+1]->Grammar()==GraSet();
	if(ok)
	{
	    BSet& r = Set(rul[j+1]);
	    ok = (r.Card()==3)&&
		(r[1]->Grammar()==GraText())&&
		(r[2]->Grammar()==GraReal())&&
		(r[3]->Grammar()==GraText());
	    if(ok)
	    {
		tag(j) =       Text(r[1]);
		col(j) = (BInt)Real(r[2]);
		fmt(j) =       Text(r[3]);
	    }
	}
	if(!ok)
	{
	    Error(I2("Bad rules definition for FormatSerSet function",
		     "Definición de reglas errónea para la función FormatSerSet"));
	    return;
	}
    }
    BArray<BParam> param(rul.Card());
    for(i=0; i<t.NumDates(); i++)
    {
	for(j=0; j<rul.Card(); j++)
	{
	    if(col(j)==0)
	    {
		txt = t.Date(i+1).GetText(BDateFormat(fmt(j)));
	    }
	    else
	    {
		txt = t.Dat(col(j),i+1).Format(fmt(j));
	    }
	    param(j) = BParam(tag(j), txt);
	}
	BSeed seed(param);
	txt = templateText;
	seed.Replace(txt);
	contens_ += txt;
    }
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BDatGetPID);
DefExtOpr(1, BDatGetPID, "GetPID", 0, 1, "Anything",
	  I2("(Anything dummy)","(Anything dummy)"),
	  I2("Returns current TOL process' PID .",
	     "Devuelve el identificador de proceso tol actual"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BDatGetPID::CalcContens()
//--------------------------------------------------------------------
{
    char buffer[100];
    sprintf( buffer, "%d", getpid() );
    contens_ = buffer;
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTextParseError);
DefExtOpr(1, BTextParseError, "ParseError", 1, 1, "Text",
	  I2("(Text TOL_Expression)","(Text expression_TOL)"),
	  I2("Returns the posible parsing errors of a TOL expression or an "
	     "empty string if it's correct",
	     "Devuelve los posibles errores sintácticos de una expresión TOL o bien "
	     "la cadena vacía si es correcta"),
	  BOperClassify::System_);
void BTextParseError::CalcContens()
//--------------------------------------------------------------------
{
    BText expr = Text(Arg(1));
    BBool stat = BOut::ErrorHci();
    BOut::PutErrorHci(BFALSE);
    Tree* tree = BParser::DefaultParser()->Parse(expr);
    contens_ = BParser::DefaultParser()->MessageError();
    BOut::PutErrorHci(stat);
    DESTROY(tree);
}

#ifdef __USE_TC__

DeclareContensClass(BText, BTxtTemporary, BTextGranularity);
DefExtOpr(1, BTextGranularity, "Granularity", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns granularity associated with Time given as argument",
	     "Devuelve la granularidad asociada al Time pasado como "
	     "parámetro"),
	  BOperClassify::Conversion_);
//--------------------------------------------------
void BTextGranularity::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    std::string base = BCTime::editedGranul(uTmi.getGranul());
    contens_ = BText(base.c_str());
}

#endif /* __USE_TC__ */
