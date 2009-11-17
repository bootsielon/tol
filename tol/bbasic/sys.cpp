/* sys.cpp: System calls functions - GNU/TOL Language.

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

#include <tol/tol_bsys.h>
#include <tol/tol_bdir.h>
#include <tol/tol_btimer.h>
#include <tol/tol_init.h>
#include <ctype.h>

#ifdef UNIX
#  include <errno.h>
#else // WINDOWS
#  include <windows.h>
#  include <process.h>
#  include <fcntl.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <io.h>
#  include <stdio.h>
#endif

//--------------------------------------------------------------------
// initialization
//   Global variables (static in BSys)
//--------------------------------------------------------------------
BTraceInit("sys.cpp");
#ifdef   UNIX
  BBool BSys::unix_=BTRUE;
  BText BSys::editor_("emacs ");
#else // WINDOWS
  BBool BSys::unix_=BFALSE;
  BText BSys::editor_("c:\\windows\\notepad.exe");
#endif

BOpenProc BSys::fileEditor_=NIL;
BOpenProc BSys::tableEditor_=NIL;
BOpenProc BSys::chartEditor_=NIL;

#ifdef UNIX
BText BSys::hostName_=BSys::GetEnv("HOSTNAME");
#else // WINDOWS
BText BSys::hostName_=BSys::GetEnv("COMPUTERNAME");
#endif

BDate* BSys::session_initTime_ = NULL;

//--------------------------------------------------------------------
static BText Buil_TolAppData_()

//! Returns the standard system directory to store application data
//--------------------------------------------------------------------
{
  static BText tolAppData_;
  if(!tolAppData_.HasName())
  {
# if _WINDOWS
    tolAppData_ = BSys::GetEnv("APPDATA")+"/tol/";
# else
    tolAppData_ = BSys::GetEnv("HOME")+"/.tol/";
#  endif
    BSys::MkDir(tolAppData_+"syslog", true);
    BSys::MkDir(tolAppData_+"OIS", true);
    BSys::MkDir(tolAppData_+"tests_results",true);
    tolAppData_ = GetStandardAbsolutePath(tolAppData_);
    int ll = tolAppData_.Length();
    if ( ll && tolAppData_.String()[ ll - 1 ] != '/' ) {
      tolAppData_ += '/';
    }
  }
  return(tolAppData_);
}


//--------------------------------------------------------------------
// Implements on windows gcc standard function gettimeofday 
//--------------------------------------------------------------------
#ifndef __GETTIMEOFDAY_C
#define __GETTIMEOFDAY_C

#if defined(_MSC_VER) || defined(_WINDOWS_)
   #include <time.h>
   #if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
         struct timeval 
         {
            long tv_sec;
            long tv_usec;
         };

         struct timezone {
             int tz_minuteswest;     /* minutes west of Greenwich */
             int tz_dsttime;         /* type of DST correction */
         };

   #endif 
   int gettimeofday(struct timeval* tv, struct timezone* ) 
   {
      union {
         long long ns100;
         FILETIME ft;
      } now;
     
      GetSystemTimeAsFileTime (&now.ft);
      tv->tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
      tv->tv_sec  = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
     return (0);
   }
#else
   #include <sys/time.h>
#endif 
#endif /* __GETTIMEOFDAY_C */


//--------------------------------------------------------------------
  const BDate& BSys::SessionInitTime()
//--------------------------------------------------------------------
{
  return(*session_initTime_);
};

//--------------------------------------------------------------------
  double BSys::SessionTime()
//Return the time elapsed since program started
//--------------------------------------------------------------------
{
  static bool    initialized = false;
  static timeval initTimeEval_;
  static timeval currentTimeEval_;
  if(!initialized)
  {
    session_initTime_ = new BDate(DteNow());
    gettimeofday(&initTimeEval_, NULL);
    initialized = true;
    return(0.0);
  }
  else
  {
    gettimeofday(&currentTimeEval_, NULL);
    return((currentTimeEval_.tv_sec  - initTimeEval_.tv_sec)+
           (currentTimeEval_.tv_usec - initTimeEval_.tv_usec)/1.E6);
  }
}

//--------------------------------------------------------------------
BText BSys::TolAppData()

//! Returns the standard system directory to store application data
//--------------------------------------------------------------------
{
  return(Buil_TolAppData_());
}

//--------------------------------------------------------------------
BBool BSys::MkDir(const BText& dir_, bool force)

/*! Create a directory named \a dir.
 * \param dir Name of directory to be created
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  BText dir = dir_;
  while(IsSlash(dir.Last()))
  {
    dir.PutLength(dir.Length()-1);
  }
  if(BDir::CheckIsDir(dir)) { return(true); }
#ifdef UNIX
  if(force) { return(BSys::System(BText("mkdir -p \"")+dir+"\"")); }
  else      { return(!mkdir(dir.String(),8*8*8-1)); }
#else
  if(force) { BSys::WinSystem(BText("cmd.exe /D /E:ON /C ")+"mkdir \""+dir+"\"",0,1); }
  else      { return(!_mkdir(dir.String())); }
#endif
}

//--------------------------------------------------------------------
BBool BSys::RmDir(const BText& dir)

/*! Create a directory named \a dir.
 * \param dir Name of directory to be created
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  if(!BDir::CheckIsDir(dir)) { return(true); }
#ifdef UNIX
  return(BSys::System(BText("rm -r -f ")+dir));
#else
  return(BSys::WinSystem(BText("cmd.exe /D /E:ON /C ")+"rmdir /S/Q  \""+dir+"\"",0,1));
#endif
}


//--------------------------------------------------------------------
void BSys::DeleteTmpFiles(BDir& dir, BReal lag, BText prefix)

/*! Deletes each file in directory \a dir, that begins with text
 *  \a prefix and hasn't been modified for last \a lag hours.
 * \param dir Directory where files are deleted
 * \param lag Number of seconds since last update
 * \param prefix Text to match begging of deleted files
 */
//--------------------------------------------------------------------
{
  if(lag<0) { lag = 0; }
  lag*=3600;

  time_t now = time(NIL);
  Std(I2("\nDeleting temporary files (", "\nBorrando ficheros temporales (")
       + dir.NumFiles() + ") ... ");
  BInt deleted=0;
  BText path(1024);
  BText msg = I2(" has been deleted.", " ha sido borrado.");
  for(BInt n=0; n<dir.NumFiles(); n++)
  {
    time_t t   = dir.FileTime(n);
    time_t dif = now - t;
    if(dif>=lag)
    {
      deleted++;
      BText fName = dir.FileName(n);
      if(!prefix.HasName() || fName.BeginWith(prefix))
      {
  path = dir.Name()+"/"+fName;
  //  Std(BText("\n") + n + ") " + dir.FileName(n) + msg);
  remove(path.String());
      }
    }
  }
  Std(BText("")+deleted+I2("\n files deleted.", "\n ficheros borrados."));
}


//--------------------------------------------------------------------
BText BSys::TempNam(const BText& outputDir,
        const BText& prefix,
        const BText& ext,
              BInt   len)

/*! Creates a unique temporal file name in specific directory.
 *  tempnam() returns new memory. If paht is empty or the program
 *  don't has privileges over de given directory (path) the common
 *  /usr/tmp directory is used.
 *         Examples:<pre>
 *             path       prefix   result
 *                                 /usr/tmp/AAAa22026
 *             /usr/base           /users/base/BAAa22026
 *             /usr/base  tol      /users/base/tolCAAa22026
 *                        tol      /usr/tmp/tolDAAa22026</pre>
 * \param outputDir Directory of file name
 * \param prefix Prefix of file name
 * \param ext Extension of file name
 * \param len Length of file name
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BSys::TempNam");
  static BInt fileNumber_ = -1;
  BText fileName;
  BText dir = outputDir;
//Std(BText("\nBSys::TempNam 1 dir = ")+dir);
  if(!dir.HasName()) { dir= TolAppData()+"tmp/"; }
//Std(BText("\nBSys::TempNam 2 dir = ")+dir);

//Std("\nBSys::TempNam("); Std(outputDir+", "+prefix+", "+ext+", "+len+")");
  if(!BDir::CheckIsDir(dir))
  {
  //Std(BText("\nBSys::TempNam 4 dir = ")+dir);
#ifdef UNIX
    mkdir(dir, 01777);
#else
    mkdir(dir);
#endif
  }
//Std(BText("\nBSys::TempNam 5 dir = ")+dir);
  BInt attempts = 0;
  BText pathFile;
  do
  {
    attempts ++;
    if(len<4) { len=4; }
    BInt base = (BInt)pow(10.0,(double)len); //(BInt)(BDat(10)^BDat(len)).Value();
    BInt r = LongRandom(base-1);
    BText number = BText("")+r;
    if(len<number.Length()) { number = number.SubString(0,len-1); }
    fileName = prefix+number+"."+ext;
    pathFile = dir+fileName;
//  Std(pathFile + " is File " + CheckIsFile(pathFile));
  }
  while(CheckIsFile(pathFile));
//Std(BText("\nTempNam = ") + fileName + " after " + attempts+ " attempts.");
//SumPartialTime;
  return(pathFile);
}



//--------------------------------------------------------------------
BText BSys::GetEnv(const BText& envVar)

/*! Returns the value of an existing environment variable. getenv()
 *  returns existing memory that can't be deleted.
 * \param envVar Name of environment variable
 */
//--------------------------------------------------------------------
{
  BChar* aux=getenv(envVar.String());
  BText   tmpName=aux;
  return(tmpName);
}


//--------------------------------------------------------------------
BBool BSys::System(const BText& command)

/*! Operating system call. Executes the command at the operating
 *  system using the C++ system() function. Returns true when works
 *  ok, if fails emit a error message and returns false.
 * \param command Command to execute
 */
//--------------------------------------------------------------------
{
  BInt errNum = system(command.String());
  BBool err=(errNum!=0);
  if(err)
  {
    Error(BText("\n[")+errno+"]=" + strerror(errno) + "\n" +
    I2("Cannot execute the command: ",
       "No se pudo ejecutar el mandato: ") + "\n" + command);
  }
  return(!err);
}


#ifndef UNIX
//--------------------------------------------------------------------
BBool BSys::WinExecuteFile(const BText& fileName)

/*! Executes a file in windows
 * \param fileName Name of file to execute
 */
//--------------------------------------------------------------------
{
  HINSTANCE instance = ShellExecute
  (
    0,                 // handle to parent window
    "open",            // string that specifies operation to perform
    fileName.String(), // filename string
    NIL,               // string that specifies executable-file parameters
    NIL,               // string that specifies default directory
    SW_SHOWNORMAL      // whether file is shown when opened
  );
  return(true);
}

//--------------------------------------------------------------------
static void OSError(const BText& msg)
//--------------------------------------------------------------------
{
  Error(I2("Operative system error \n",
           "Error del Sistema Operativo \n")+msg);
};

//--------------------------------------------------------------------
static bool ShowLastError(const BText& action, bool force)
//--------------------------------------------------------------------
{
  DWORD dw = GetLastError(); 
  if(dw || force)
  {
    LPVOID lpMsgBuf;
    FormatMessage
    (
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, 
      NULL 
    );
    OSError(action + "\n" + (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
  };
  return(dw==0);
};

//--------------------------------------------------------------------
  BBool BSys::WinSystem  (const BText& command, BInt showMode, bool wait)
//--------------------------------------------------------------------
{
  SetLastError(0);
  STARTUPINFO         startupInfo;
  PROCESS_INFORMATION piProcInfo;
  BText errNam = BSys::TempNam();
  BText cmdRedirected = command;
  if(command.BeginWith("cmd ")) { cmdRedirected += BText(" 2> ")+errNam; }
  char* cmd = (char*)cmdRedirected.String();
  GetStartupInfo(&startupInfo);
  startupInfo.dwFlags &= STARTF_USESHOWWINDOW;
  startupInfo.wShowWindow = showMode;
  bool ok = true; 
  int result = 0;

  if (!CreateProcess(NULL, cmd, NULL, NULL,
       FALSE, 0, NULL, NULL, &startupInfo,
       &piProcInfo)) 
  {
    ShowLastError(I2("Creating process \n", 
                     "Creando el proceso \n") +
                  command, true);
    ok = false;
  }
  else
  {
    if(wait)
    {
      result=WaitForSingleObject(piProcInfo.hProcess, INFINITE);
      if( result != WAIT_OBJECT_0 )
      {
        ShowLastError(I2("Waiting for result of process \n", 
                         "Esperando el resultado del proceso \n") +
                      command, true);
        ok = false;
      }
      else
      {
        result = CloseHandle(piProcInfo.hProcess);
        if( result == 0 )
        {
          ShowLastError(I2("Closing process \n", 
                           "Cerrando el proceso \n") +
                        command, true);
          ok = false;
        }
        BText errMsg(16*1024);
        errMsg.PutLength(0);
        if(CheckIsFile(errNam))        ReadFile(errMsg,errNam);
        if(errMsg.HasName())
        {
          OSError(errMsg);
          ok = false;
        }
        remove(errNam.String());
      }
    }
    else if(wait)
    {
      result=WaitForSingleObject(piProcInfo.hProcess, 0);
      if( result != WAIT_OBJECT_0 )
      {
        ShowLastError(I2("Waiting for result of process \n", 
                         "Esperando el resultado del proceso \n") +
                      command, true);
        ok = false;
      }
    }
  }
  return(ok);
}


//--------------------------------------------------------------------
//BBool BSys::WinSystem  (const BText& command, BInt showMode)

/*! Windows operating system call.
 * /
//--------------------------------------------------------------------
{
  BInt errNum = WinExec(command.String(),showMode);
  
  BBool err=(errNum<=31);
  BText msg;
  switch(errNum)
  {
    case 0:
    {
      msg = I2("The system is out of memory or resources.",
               "El sistema no tiene memoria o recursos");
      break;
    }
    case ERROR_BAD_FORMAT:
    {
      msg = I2("The .EXE file is invalid","El fichero .EXE no es válido.");
      break;
    }
    case ERROR_FILE_NOT_FOUND:
    {
      msg = I2("The specified file was not found.",
               "No se encuentra el fichero especificado");
      break;
    }
    case ERROR_PATH_NOT_FOUND :
    {
      msg = I2("The specified path was not found.",
               "No se encuentra el camino especificado.");
    }
  }
  if(err)
  {
    Error(I2("Executing commad \n", "Ejecutando el comando \n") +
              command + "\n" + msg);
  }
  return(!err);
}
*/

#else


#endif



//--------------------------------------------------------------------
BBool BSys::Print(const BText& fileName)

/*! Prints a file in the default printer. Operating systems:<ul>
 *      <li>UNIX: print the file with lp command, options:<ul>
 *  <li>-onb: option no banner (don't work).
 *  <li>-s: suppress messages from lp such as: request ID is...
 *  <li>-c: Make copies of the files to be printed immediately when
 *            lp is invoked (and then can be deleted).</ul>
 *      <li>WINDOWS: .</ul>
 * \param fileName Name of file to be printed
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
#ifdef   UNIX
  BText command("lp -c ");
  return(BSys::System(command+fileName));
#else // WINDOWS
  ShellExecute(
    0,                 // handle to parent window
    "print",           // string that specifies operation to perform
    fileName.String(), // filename string
    Editor().String(), // string that specifies executable-file parameters
    NIL,               // string that specifies default directory
    SW_SHOWNORMAL      // whether file is shown when opened
   );
  return(BTRUE);
#endif
}


//--------------------------------------------------------------------
BBool BSys::Remove(const BText& fileName)

/*! Removes a file given its name.
 * \param fileName Name of file to be removed
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  return(!remove(fileName.String()));
}

//--------------------------------------------------------------------
BBool BSys::Copy(const BText& origin, const BText& target)

/*! Copies \a origin file(s) to \a target. 
 *  Operating systems:<ul>
 *      <li>UNIX:    copies using cp f t.
 *      <li>WINDOWS: copies using copy f t.</ul>
 * \param origin Name of source(s) file(s). If there are multiple sources,
 *               target must be a directory
 * \param target Name of destination file or directory
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
#ifdef   UNIX
  BText copyCom("cp ");
#else // WINDOWS
  BText copyCom("copy ");
#endif
  BText command(copyCom + origin + " " + target);
  return(BSys::System(command));
}

//--------------------------------------------------------------------
BBool BSys::Cat(const BArray<BText>& origin, const BText& target)

/*! Cats a set of files into target. 
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  static const int bufferSize_ = 1024*1024;
  static char buffer_[bufferSize_];
  FILE* in;
  FILE* out = fopen(target.String(),"wb");
  if(!out)
  {
    Error(I2("Cannot open for write file ",
             "No se pudo abrir para escritura el fichero ")+
          target);
    return(false);
  }
  int i, numBytes, size, pos;
  for(i=0; i<origin.Size(); i++)
  {
    size = GetFileSize(origin[i]);
    in = fopen(origin[i].String(),"rb");
    if(!in)
    {
      Error(I2("Cannot open for read file ",
               "No se pudo abrir para lectura el fichero ")+
            origin[i]);
      return(false);
    }
    pos = 0;
    while(pos<size)
    {
      numBytes = bufferSize_;
      if(numBytes>size-pos) { numBytes = size-pos; }
      fread (&buffer_,1,numBytes,in);
      fwrite(&buffer_,1,numBytes,out);
      pos += numBytes;
    }
    fclose(in);
  }
  fclose(out);
  return(true);
}

//--------------------------------------------------------------------
BBool BSys::PrintUnlink(const BText& fileName)

/*! Prints a file in the default printer and removes this file from
 *  disk. At UNIX the -c option must be set.
 * \param fileName Name of file to be printed
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
//return(BSys::Print(fileName) && BSys::Unlink(fileName));
  return(BSys::Print(fileName));
}


//--------------------------------------------------------------------
BBool BSys::Unlink(const BText& fileName)

/*! Removes a file from disk. Doesn't emit any error message.
 * \param fileName Name of file to be removed
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  BBool ok=BTRUE;
  if(unlink(fileName.String())<0) { ok=BFALSE; }
  return(ok);
}


//--------------------------------------------------------------------
BBool BSys::Edit(const BText& fileName, BInt typeIndex)

/*! Edits a file in the default editor. Edition is sent in parallel.
 * \param fileName Name of file to be edited
 * \param typeIndex
 * \return Returns true on success, or false if an error occurred.(On windows
 *         always returns true).
 */
//--------------------------------------------------------------------
{
  if(!BSys::Editor().HasName()) { return(BFALSE); }
#ifdef   UNIX
  return(BSys::System(BSys::Editor()+" "+fileName+" &"));
#else // WINDOWS
  if (fileEditor_)
  {
//  Std(BText("\nCalling ")+fileName+ " "+BInt(fileEditor_));
    (*fileEditor_)(fileName, typeIndex);
  }
  else
  {
    ShellExecute(
      0,                 // handle to parent window
      "open",            // string that specifies operation to perform
      fileName.String(), // filename string
      Editor().String(), // string that specifies executable-file parameters
      NIL,               // string that specifies default directory
      SW_SHOWNORMAL      // whether file is shown when opened
     );
  }
  return(BTRUE);
#endif
}


//--------------------------------------------------------------------
BBool BSys::EditTable(const BText& fileName, BInt typeIndex)

/*! Edits a file in the default table editor. Edition is sent in
 *  parallel.
 * \param fileName Name of file to be edited
 * \param typeIndex
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  if(!BSys::Editor().HasName()) { return(BFALSE); }
#ifdef   UNIX
  return(Edit(fileName,0));
#else // WINDOWS
  if (tableEditor_)
  {
    (*tableEditor_)(fileName, typeIndex);
  }
  else { return(Edit(fileName, typeIndex)); }
  return(BTRUE);
#endif
}


//--------------------------------------------------------------------
BBool BSys::EditChart(const BText& fileName, BInt typeIndex)

/*! Edits a file in the default chart editor. Edition is sent in
 *  parallel.
 * \param fileName Name of file to be edited
 * \param typeIndex
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  if(!BSys::Editor().HasName()) { return(BFALSE); }
#ifdef   UNIX
  return(Edit(fileName,0));
#else // WINDOWS
  if (chartEditor_)
  {
    (*chartEditor_)(fileName, typeIndex);
  }
  else { return(Edit(fileName, typeIndex)); }
  return(BTRUE);
#endif
}


//--------------------------------------------------------------------
BBool BSys::EditUnlink(const BText& fileName, BInt typeIndex)

/*! Edits a file in the default editor and removes this file from
 *  disk. It's usefull for temporary files. The edition is send in
 *  parallel those the file is removed before the end of edition.
 * \param fileName Name of file to be edited
 * \param typeIndex
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
//return(BSys::Edit(fileName) && BSys::Unlink(fileName));
  return(BSys::Edit(fileName, typeIndex));
}


//--------------------------------------------------------------------
BBool BSys::EditB(const BText& fileName)

/*! Edits a file in the default editor. Edition is a blocking
 *  process.
 * \param fileName Name of file to be edited
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
#ifdef   UNIX
  return(BSys::System(BSys::Editor()+fileName));
#else // WINDOWS
  ShellExecute(
    0,                  // handle to parent window
    "open",             // string that specifies operation to perform
    fileName.String(),  // filename string
    NIL,                // string that specifies executable-file parameters
    Editor().String(),  // string that specifies default directory
    SW_SHOWNORMAL       // whether file is shown when opened
  );
  return(BTRUE);
#endif
}


//--------------------------------------------------------------------
BBool BSys::EditNew()

/*! Edits a new file in the default editor. Edition is sent in
 *  parallel.
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  return(BSys::System(BSys::Editor()+" &"));
}


//--------------------------------------------------------------------
BBool BSys::EditText(const BText& text)

/*! Edits a text using a temporary file in the default editor and
 *  removes this temporaty file from disk. It's usefull for 
 *  large texts. The edition is sent in parallel, those the file is
 *  removed before the end of edition.
 * \param text Text to be edited
 * \return Returns true on success, or false if an error occurred
 */
//--------------------------------------------------------------------
{
  BText fileName=TempNam();
  OverWrite(fileName,text);
  return(BSys::EditUnlink(fileName,0));
}


//--------------------------------------------------------------------
void BSys::Unix2Dos(const BText& fileName)

/*! Converts a text file from UNIX format to DOS format
 * \param fileName Name of file to be converted
 */
//--------------------------------------------------------------------
{
  BText text;
  {
    ifstream input(fileName.String());
    BDir     file (fileName);
    BInt     length=file.Bytes();
    if(input.good())
    {
      text.ReallocateBuffer(length+10);
      Read(input, text, length+10, '\0');
      text.Replace("\n", "\r\n");
    }
  }
  if(text.HasName()) { OverWrite(fileName,text); }
}


//--------------------------------------------------------------------
void BSys::Dos2Unix(const BText& fileName)

/*! Converts a text file from DOS format to UNIX format
 * \param fileName Name of file to be converted
 */
//--------------------------------------------------------------------
{
  BText text;
  {
    ifstream input(fileName.String());
    BDir     file (fileName);
    BInt     length=file.Bytes();
    if(input.good())
    {
      text.ReallocateBuffer(length+10);
      Read(input, text);
      text.Replace('\r', ' ');
      text.Replace(BChar(6), ' ');
    }
  }
  if(text.HasName()) { OverWrite(fileName,text); }
}


#ifndef UNIX
//--------------------------------------------------------------------
unsigned long _stdcall threadMain(void *arg)
//--------------------------------------------------------------------
{
  char* buf = (char*) arg;
//Std(BText("\n")+buf);
  BSys::System(buf);
  delete(buf);
  DWORD lpExitCode;
  GetExitCodeThread
  (
    GetCurrentThread(), // handle to the thread
    &lpExitCode          // address to receive termination status
  );
  ExitThread(lpExitCode);
  return(lpExitCode);
}
#endif


//--------------------------------------------------------------------
BBool BSys::ChildProcess(const BText& command)
//--------------------------------------------------------------------
{
#ifndef    UNIX

//Std(BText("\nClient Question : \n")+command+"\n");

  SECURITY_ATTRIBUTES    sa =
  {
    sizeof(SECURITY_ATTRIBUTES),   // structure size
    0,                             // No security descriptor
    TRUE,                          // Thread handle is inheritable
  };
  char* arg = new char[command.Length()+1];
  strncpy(arg, command.String(), command.Length()+1);
  DWORD    threadId;
  HANDLE hThread = (HANDLE)CreateThread
  (
    &sa,                    // Thread security
    4096,                   // Thread stack size
    threadMain,              // Thread starting address
    (void *)arg,            // Thread start argument
    CREATE_SUSPENDED,       // Create in suspended state
    &threadId               // Thread ID.
  );

  if(hThread == INVALID_HANDLE_VALUE)
  {
    //MessageBox(0, "Thread Creation Failed", "Error", MB_OK);
  Error("Thread Creation Failed");
    return(BFALSE);
  }
  ResumeThread(hThread);
  printf("Created thread with an ID of %u\n", threadId);
#endif
  return(BTRUE);
}

//--------------------------------------------------------------------
BInt LongRandom(BInt max)

/*! Generates a pseudo-random series of uniformely distributed in
 *  [0,1] and independent real numbers using the multiplicative
 *  congruential method IBM System/360 Uniform Random Number
 *  Generator. See
 *
 *  Simulation and Montecarlo Method. Page 25.
 *  Wiley series in Probability and Mathematical Statistics.
 * \param max Indicates the superior limit of the random number
 * \return Return the random number
 */
//--------------------------------------------------------------------
{
    static unsigned long startRand_ = clock();
    startRand_ = (16807*startRand_)%2147483647;
    BReal r = BReal(startRand_)/BReal(2147483646.0);
    if(startRand_==0) { startRand_ = clock(); }
    return(BInt(r*BReal(max)));
}

