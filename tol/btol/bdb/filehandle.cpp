/* filehandle.cpp:  Input-Output built-in TOL functions.
            GNU/TOL Language.

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


#include <tol/tol_filehandle.h>
#include <tol/tol_bout.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>

#include <errno.h>

inline char* syserr(int numErr)
{
  return(strerror(numErr));
};

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("file.cpp");


BFileDesc BFileDesc::noFileDesc_(NULL,"");


/////////////////////////////////////////////////////////////////////////////
BFileDesc::BFileHashByHandle& BFileDesc::FileHandle()
/////////////////////////////////////////////////////////////////////////////
{
  static BFileHashByHandle* fileHandle_=NULL;
  if(!fileHandle_)
  {
    fileHandle_ = new BFileHashByHandle; 
    BFileHashByHandle& fh = *fileHandle_; 
    SetEmptyKey  (fh, NULL);
    SetDeletedKey(fh, -1);
  }
  return(*fileHandle_);
};


/////////////////////////////////////////////////////////////////////////////
 BFileDesc* BFileDesc::FindFileHandle(
  int handle, bool verbose, const BText& msg, 
  BFileHashByHandle::const_iterator& found)
/////////////////////////////////////////////////////////////////////////////
{
  BFileDesc* fd;
  found = FileHandle().find(handle);
  if(found==FileHandle().end())
  { 
    fd = NULL;
  }
  else
  {
    fd = found->second;
  }
  if(!fd && verbose)
  {
    Error(I2("Invalid file descriptor ",
             "Descriptor de fichero inválido ")+handle+"\n"+msg);
  }
  return(fd);
};

/////////////////////////////////////////////////////////////////////////////
 BFileDesc* BFileDesc::FindFileHandle(
   int handle, bool verbose, const BText& msg)
/////////////////////////////////////////////////////////////////////////////
{
  BFileHashByHandle::const_iterator found;
  return(FindFileHandle(handle, verbose, msg, found));
};

/////////////////////////////////////////////////////////////////////////////
 FILE* BFileDesc::CheckFileHandle(int handle, bool verbose, const BText& msg, 
                                  BFileHashByHandle::const_iterator& found)
/////////////////////////////////////////////////////////////////////////////
{
  BFileDesc* fd = FindFileHandle(handle, verbose, msg, found);
  return((fd)?fd->file_:NULL);
};

/////////////////////////////////////////////////////////////////////////////
FILE* BFileDesc::CheckFileHandle(int handle, bool verbose, const BText& msg)
/////////////////////////////////////////////////////////////////////////////
{
  BFileHashByHandle::const_iterator found;
  return(CheckFileHandle(handle, verbose, msg, found));
};

/////////////////////////////////////////////////////////////////////////////
void BFileDesc::SysChkErrNo(bool clean, bool verbose, const BText& msg)
/////////////////////////////////////////////////////////////////////////////
{
  if(verbose && errno) 
  { 
    Error(I2("Operative System Error Message:\n",
             "Mensaje de Error del Sistema Operativo:\n")+
          syserr(errno)+"\n"+msg); 
  }
  if(clean) { errno = 0; }  
};

/////////////////////////////////////////////////////////////////////////////
BText sysErrCleanVervose_ = I2(
/////////////////////////////////////////////////////////////////////////////
"If clean is true the system error code is set to 0 to avoid repeated "
"messages.\n"
"If verbose is true and there is some non zero code error the "
"corresponding error message will be showed, ",
"Si clean es cierto el código de error del sistema se pone a 0 para "
"evitar mensajes repetidos.\n"
"Si verbose es cierto y el código de error no es cero se mostrará el "
"mensaje de error correspondiente.");

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BDat, BDatTemporary, BDatSysErrNum);
  DefExtOpr(1, BDatSysErrNum, "SysErrNum", 1, 2, "Real Real",
  "(Real clean [, Real verbose=false])",
  I2("Returns the system error code corresponding to the last happened "
     "error.\n",
     "Devuelve el código de error del sistema asociado al último error "
     "ocurrido.\n")+
  sysErrCleanVervose_,
  BOperClassify::BayesDataBase_);
  void BDatSysErrNum::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  bool clean = int(Real(Arg(1)))!=0; 
  bool verbose = false;
  if(Arg(2)) { verbose = int(Real(Arg(2)))!=0; }
  contens_ = errno;
  BFileDesc::SysChkErrNo(clean, verbose, "");
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BTxt, BTxtTemporary, BTxtSysErrMsg);
  DefExtOpr(1, BTxtSysErrMsg, "SysErrMsg", 1, 1, "Real",
  "(Real sysErrCod)",
  I2("Returns the system error message corresponding to a specified code."
     "If code is 0 then the last happened error will be "
     "returned.",
     "Devuelve el mensaje de error del sistema asociado al código "
     "especificado. Si el código es 0 entonces se devuelve el del último "
     "error ocurrido."),
  BOperClassify::BayesDataBase_);
  void BTxtSysErrMsg::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  int num = int(Real(Arg(1))); 
  if(!num) { num = errno; }
  contens_ = syserr(num);
}


/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BDat, BDatTemporary, BDatFOpen);
  DefExtOpr(1, BDatFOpen, "FOpen", 2, 4, "Text Text Real Real",
  "(Text filePath, Text access [, Real clean, Real verbose=false])",
  I2("Opens a file using C function fopen and returns an integer "
     "value, which is used to refer to the file. If un-successful, "
     "it returns 0.",
     "Abre un fichero usando la función C fopen y devueleve un "
     "entero, el cual se usará para referenciar al fichero. En caso "
     "de error devuelve 0."
     "El código de acceso especifica el modo de apertura:\n "
     "  r Abre un fichero de texto para lectura\n "
     "  w Trunca a longitud cero o crea un fichero de texto para escribir\n "
     "  a Añade abre o crea un fichero de texto para escribir al final del fichero (EOF)\n "
     "  rb Abre un fichero en modo binario para lectura\n "
     "  wb Trunca a longitud cero o crea un fichero en modo binario para escribir\n "
     "  ab Añade o crea un fichero en modo binario para escribir al final del fichero (EOF)\n "
     "  r+ Abre un fichero de texto para actualización (lectura y escritura)\n "
     "  w+ Trunca a longitud cero o crea un fichero de texto para actualización\n "
     "  a+ Añade o crea un fichero de texto para actualización, escribiendo al final del fichero (EOF)\n "
     "  r+b ó rb+ Abre un fichero en modo binario para actualización (lectura y escritura)\n "
     "  w+b ó wb+ Trunca a longitud cero o crea un fichero en modo binario para actualización\n "
     "  a+b ó ab+ Añade o crea un fichero en modo binario para actualización, escribiendo al final del fichero (EOF)\n"
  )+sysErrCleanVervose_,
  BOperClassify::BayesDataBase_);
  void BDatFOpen::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  if(CheckNonDeclarativeAction("FOpen")) { return; }
  static BText allowedAccess_ = " r w a rb wb ab r+ w+ a+ r+b w+b a+b ";
  BText& path   = Text(Arg(1));
  BText& access = Text(Arg(2));
  bool clean   = true;
  bool verbose = false;
  if(Arg(3)) { clean   = int(Real(Arg(3)))!=0; }
  if(Arg(4)) { verbose = int(Real(Arg(4)))!=0; }
  if(allowedAccess_.Find(BText(" ")+access+" ")<0)
  {
    Error(BText("[FOpen] Access mode ")+access+
    " is not one of allowed modes:(" +allowedAccess_+")");
    contens_ = 0;
    return;
  }
  int    handle = 0; 
  FILE*  file = fopen(path.String(),access.String());
  if(file)
  {
    handle = fileno(file);
    BFileDesc::FileHandle()[handle] = new BFileDesc(file, path);
  }
  else
  {
    BFileDesc::SysChkErrNo(clean, verbose, 
      I2("Fail in ","Fallo en ")+
      "FOpen(\""+path+"\",\""+access+"\")");
  }
  contens_ = handle;
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BTxt, BTxtTemporary, BTxtFGetText);
  DefExtOpr(1, BTxtFGetText, "FGetText", 2, 3, "Real Real Text",
  "(Real handle, Real maxSize [, Text delim=\"\\n\"])",
  I2("Reads a string from a file until the character delim is readed, "
     "maxSize characters are reached or file ends.",
     "Lee una cadena de un fichero hasta que se lee el caracter delim, se "
     "alcanzan maxSize caracteres o dse acaba el fichero."),
  BOperClassify::BayesDataBase_);
  void BTxtFGetText::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  if(CheckNonDeclarativeAction("FGetText")) { return; }
  int handle  = (int)Real(Arg(1));
  int maxSize = (int)Real(Arg(2));
  BText until = "\n";
  if(Arg(3)) { until=Text(Arg(3)); }
  contens_ = "";
   
  FILE* file = BFileDesc::CheckFileHandle(handle,true,
                               I2("Fail in ","Fallo en ")+"FGetText");
  if(file)
  {
    contens_.GetLine(file, maxSize, until[0]);
  }
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BDat, BDatTemporary, BDatFPutText);
  DefExtOpr(1, BDatFPutText, "FPutText", 2, 2, "Real Text",
  "(Real handle, Text txt)",
  I2("Writes a string in a open file.",
     "Escribe una cadena en un fichero abierto."),
  BOperClassify::BayesDataBase_);
  void BDatFPutText::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  if(CheckNonDeclarativeAction("FPutText")) { return; }
  int    handle = (int)Real(Arg(1));
  BText& txt    = Text(Arg(2));
  contens_ = EOF;
  FILE* file = BFileDesc::CheckFileHandle(handle,true,
                               I2("Fail in ","Fallo en ")+"FPutText");
  if(file)
  {
    contens_ = fputs(txt.String(),file);
  }
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BDat, BDatTemporary, BDatFTell);
  DefExtOpr(1, BDatFTell, "FTell", 1, 1, "Real",
  "(Real handle)",
  I2("Returns the current position of a file open with FOpen.",
     "Devuelve la posición actual de un fichero abierto con FOpen"),
  BOperClassify::BayesDataBase_);
  void BDatFTell::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  int handle  = (int)Real(Arg(1));
  FILE* file = BFileDesc::CheckFileHandle(handle,true,
    I2("Fail in ","Fallo en ")+"FTell");
  if(file)
  {
    contens_ = (double)ftello(file);
  }
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BDat, BDatTemporary, BDatFSeek);
  DefExtOpr(1, BDatFSeek, "FSeek", 2, 2, "Real Real",
  "(Real handle, Real offset)",
  I2("Sets the current position of a file open with FOpen.",
     "Mueve la posición actual de un fichero abierto con FOpen"),
  BOperClassify::BayesDataBase_);
  void BDatFSeek::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  int handle  = (int)Real(Arg(1));
  off_t offset = (off_t)Real(Arg(2));
  FILE* file = BFileDesc::CheckFileHandle(handle,true,
    I2("Fail in ","Fallo en ")+"FSeek");
  if(file)
  {
    contens_ = fseeko(file,offset,SEEK_SET);
  }
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BMat, BMatTemporary, BMatFGetNumbers);
  DefExtOpr(1, BMatFGetNumbers, "FGetNumbers", 3, 3, "Real Real Text",
  "(Real handle, Real numItems, Text numberType)",
  I2("Reads an array of numbers from a binary file.",
     "Lee un array de números desde un fichero binario.")+
     "numberType : \"double\", \"float\", \"int\", \"unsigned int\", \"short int\", \"unsigned short int\"",
  BOperClassify::BayesDataBase_);
  void BMatFGetNumbers::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  if(CheckNonDeclarativeAction("FGetNumbers")) { return; }
  int handle  = (int)Real(Arg(1));
  int numItems = (int)Real(Arg(2));
  BText& numberType = Text(Arg(3)); 
  contens_.Alloc(1,numItems);
  int result, i; 
  FILE* file = BFileDesc::CheckFileHandle(handle,true,
    I2("Fail in ","Fallo en ")+"FGetNumbers");
  if(file)
  {
    if(numberType=="double")
    {
      BArray<double> arr(numItems);
      result = fread(arr.GetBuffer(),8,numItems,file);
      if(result==numItems) for(i=0; i<numItems; i++)
      {
        contens_(0,i) = (double)arr(i); 
      }
    }
    else if(numberType=="float")
    {
      BArray<float> arr(numItems);
      result = fread(arr.GetBuffer(),4,numItems,file);
      if(result==numItems) for(i=0; i<numItems; i++)
      {
        contens_(0,i) = (double)arr(i); 
      }
    }
    else if(numberType=="int")
    {
      BArray<int> arr(numItems);
      result = fread(arr.GetBuffer(),4,numItems,file);
      if(result==numItems) for(i=0; i<numItems; i++)
      {
        contens_(0,i) = (double)arr(i); 
      }
    }
    else if(numberType=="unsigned int")
    {
      BArray<unsigned int> arr(numItems);
      result = fread(arr.GetBuffer(),4,numItems,file);
      if(result==numItems) for(i=0; i<numItems; i++)
      {
        contens_(0,i) = (double)arr(i); 
      }
    }
    else if(numberType=="short int")
    {
      BArray<short int> arr(numItems);
      result = fread(arr.GetBuffer(),2,numItems,file);
      if(result==numItems) for(i=0; i<numItems; i++)
      {
        contens_(0,i) = (double)arr(i); 
      }
    }
    else if(numberType=="unsigned short int")
    {
      BArray<unsigned short int> arr(numItems);
      result = fread(arr.GetBuffer(),2,numItems,file);
      if(result==numItems) for(i=0; i<numItems; i++)
      {
        contens_(0,i) = (double)arr(i); 
      }
    }
    if(result!=numItems)
    {
      Error(BText("[FGetNumbers] ")+
        I2("The total number of elements successfully read is ",
           "El número total de elementos leídos con éxito es ")+result+
        I2(" when it was expected ",
           " cuando se esperaban ")+numItems);
    };
  }
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BDat, BDatTemporary, BDatFEof);
  DefExtOpr(1, BDatFEof, "FEof", 1, 1, "Real",
  "(Real handle)",
  I2("Tests the end-of-file indicator for the stream pointed to by stream, "
     "returning non-zero if it is set.",
     "Comprueba el indicador de fin de fichero para descriptor especificado "
     "y devuelve cierto si se ha alcanzado."),
  BOperClassify::BayesDataBase_);
  void BDatFEof::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  if(CheckNonDeclarativeAction("FEof")) { return; }
  int handle  = (int)Real(Arg(1));
  contens_ = EOF;
  FILE* file = BFileDesc::CheckFileHandle(handle, true,
                               I2("Fail in ","Fallo en ")+"FEof");
  if(file)
  {
    contens_ = feof(file);
  }
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BDat, BDatTemporary, BDatFFlush);
  DefExtOpr(1, BDatFFlush, "FFlush", 1, 1, "Real",
  "(Real handle)",
  I2("The function FFlush forces a write of all user-space buffered data for "
     "the given output or update stream via the stream's underlying write "
     "function. The open status of the stream is unaffected."
     "If handle is null then all files will be flushed.",
     "La función fflush () fuerza de escritura de todos los datos pendientes "
     "para un fichero dado. El estado de apertura del fichero no se ve "
     "afectado. Si se pasa handle nulo se forzará la escritura de todos los "
     "ficheros abiertos."),
  BOperClassify::BayesDataBase_);
  void BDatFFlush::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  if(CheckNonDeclarativeAction("FFlush")) { return; }
  int handle  = (int)Real(Arg(1));
  contens_ = EOF;
  if(!handle)
  {
    contens_ = fflush(0);
  }
  else
  {
    FILE* file = BFileDesc::CheckFileHandle(handle, true,
                                 I2("Fail in ","Fallo en ")+"FFlush");
    if(file)
    {
      contens_ = fflush(file);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BDat, BDatTemporary, BDatFClose);
  DefExtOpr(1, BDatFClose, "FClose", 1, 3, "Real Real Real",
  "(Real handle [, Real clean, Real verbose=false])",
  I2("Closes the file associated with handle that was the result of "
     "calling FOpen. The function returns 0 if successful, "+EOF+" "
     "to indicate an error.",
     "Cierra el fichero asociado a handle que fue el resultado de "
     "una llamada a FOpen. En caso de error devuelve "+EOF+" y si "
     "todo va bien 0.")+
  sysErrCleanVervose_,
  BOperClassify::BayesDataBase_);
  void BDatFClose::CalcContens()
/////////////////////////////////////////////////////////////////////////////
{
  if(CheckNonDeclarativeAction("FClose")) { return; }
  bool clean   = true;
  bool verbose = false;
  int  handle  = (int)Real(Arg(1));
  if(Arg(2)) { clean   = int(Real(Arg(2)))!=0; }
  if(Arg(3)) { verbose = int(Real(Arg(3)))!=0; }
  contens_ = EOF;
  BFileDesc::BFileHashByHandle::const_iterator found;
  BText failMsg = I2("Fail in ","Fallo en ")+"FClose";
  FILE* file = BFileDesc::CheckFileHandle(handle, true, failMsg, found);
  if(file)
  {
    int fc = fclose(file);
    contens_ = fc;
    if(!fc)
    {
      BFileDesc::FileHandle().erase(found->first);
    }
    else
    {
      BFileDesc::SysChkErrNo(clean, verbose, failMsg);
    }
  }
}


