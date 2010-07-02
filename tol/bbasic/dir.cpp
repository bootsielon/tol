/* dir.cpp: BDir (dir) Operating systems directories and files information.
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

/*!
 *
 * BDir: Operating systems directories and files information
 * WARNING: In HP-UX the header /usr/include/sys/dir.h is not ansi prototyped
 *	    and was modified by ASC as following:
 *	    Original:
 *	      extern  DIR *opendir();
 *	      extern  struct direct *readdir();
 *	      ...
 *	      extern int closedir();
 *	    Modification:
 *	      #ifdef BDIR { BYS ASC Prototype }
 *	      extern "C"
 *	      {
 *		extern DIR *opendir(const char *);
 *		extern struct direct *readdir(DIR *);
 *		extern int closedir(DIR *);
 *	      }
 *	      #else { Original code }
 *		extern DIR *opendir();
 *		extern struct direct *readdir();
 *		extern int closedir();
 *	      #endif
 */	      

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bdir.h>
#include <tol/tol_blist.h>

#if defined(UNIX)
#  include <errno.h>
#else
#  include <windows.h>
#endif

#include <sys/types.h>
#if HAVE_UTIME_H
#  include <utime.h>
#elif HAVE_SYS_UTIME_H
#  include <sys/utime.h>
#endif

BTraceInit("dir.cpp");

//--------------------------------------------------------------------
// general functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BText BDir::GetCurrent()

/*! Returns a text with the current directory.
 */
//--------------------------------------------------------------------
{
  BChar d[DIRMAXNAMLEN];
  getcwd(d,DIRMAXNAMLEN);
  return(BText(d));
}


//--------------------------------------------------------------------
void BDir::ChangeCurrent(const BText& d)

/*! Changes the current directory (like the command cd).
 * \param d new directory 
 */
//--------------------------------------------------------------------
{ 
//Std(BText("\nChanging current directory from \n  ")+GetCurrent()+
//          "\nto \n  "+d+"\n");
  chdir(d.String());
}


//--------------------------------------------------------------------
// internal functions
//--------------------------------------------------------------------
#if defined(_MSC_VER)

//--------------------------------------------------------------------
static BList* DirFiles(const BText& dirName)

/*! Returns a new sorted list with BTexts, each BText contens a name
 *  of one directory file. The caller function is responsible of
 *  BList and its BText destruction. 
 * \param  dirName name of the directory
 * \return A new sorted list,
 * \return a new sorted list, NIL if the directory doesn't exits.
 */
//--------------------------------------------------------------------
{
  BList* lstFil=NIL;
  BList* lstSort=NIL;
  struct _finddata_t sfile;
  long hFile;
  BText files_filter( dirName+"\\*.*" );

  if ((hFile=_findfirst(files_filter.String(),&sfile)) != -1L) {
    do {
      lstFil = Cons( new BObject(sfile.name), lstFil );
    } while ( _findnext(hFile, &sfile) == 0 );
    _findclose(hFile);
    lstSort = LstSort(lstFil);
    DESTROY(lstFil);
  }
  return lstSort;
}

#else // !(_MSC_VER || UNIX) ==> is BORLAND

//--------------------------------------------------------------------
static BText GetEntryName(DIR* fpDir)

/*! Read file names from directory pointed to and return the file
 *  name, if there aren't more entries in dir returns an empty
 *  file name.
 *   - fpAuxDir is a file pointer to directory declared as BChar*
 *     for casting problems between UNIX and WINDOWS.
 *   - fileName is a location to store file name.
 *   - struct direct *dirEntry holds directory entries.
 * \param fpDir is a file pointer to directory 
 * \return the file name, empty if there aren't more entries
 */
//--------------------------------------------------------------------
{
  static struct direct *dirEntry;
  BText	 fileName;
  BBool	 endCycle=BFALSE;
  BBool	 ok=BTRUE;

  while(! endCycle)
  {
    if((dirEntry=readdir(fpDir))==(struct direct *)NULL)
    {
      endCycle=BTRUE;
      ok=BFALSE; // There aren't more files
    }
    else
    #ifdef UNIX
    if(dirEntry->d_ino != 0)
    #endif // UNIX
    {
      endCycle=BTRUE;
      ok=BTRUE; // One more file is found
    }
  }

  if(ok) { fileName = dirEntry->d_name; }

  return(fileName);
}


//--------------------------------------------------------------------
static BList* DirFiles(const BText& dirName)

/*! Returns a new sorted list with BTexts, each BText contens a name
 *  of one directory file. The caller function is responsible of
 *  BList and its BText destruction. Return NIL if the directory
 *  doesn't exits.
 * \param dirName name of the directory
 * \return a new list with the names of the directories, NIL if the 
 *        directory doesn't exits 
 */
//--------------------------------------------------------------------
{
  DIR*	 fpDir;
  BList* lstFil=NIL;
  BList* lstSort=NIL;
  BChar* nam = (char*)dirName.String();
  if(nam && (fpDir=opendir(nam)))
  {
    BText fileName(GetEntryName(fpDir));
    while(fileName.HasName())
    {
      lstFil=Cons(new BObject(fileName), lstFil);
      fileName = GetEntryName(fpDir);
    }
    closedir(fpDir);
  }

  lstSort=LstSort(lstFil);
  DESTROY(lstFil);

  return(lstSort);
}

#endif // _MSC_VER

#ifdef _MSC_VER
//#define stat _stat
#define S_IFREG _S_IFREG
#define S_IFDIR _S_IFDIR
#endif

//--------------------------------------------------------------------
BBool BDir::CheckIsDir(const BText& dirName)

/*! Verifies if \a dirName is a directory.
 * \param dirName name of directory
 * \return true if is a directory
 */
//--------------------------------------------------------------------
{
  struct stat buffer;
  char last = dirName.Last();
  #ifdef UNIX
  if(last=='/')
  #else
  if(last=='/' || last=='\\')
  #endif
  {
    BText dirName_(dirName.String(),dirName.Length()-1);
    return(CheckIsDir(dirName_));
  }
  if ( dirName.Length() && stat( dirName.String(), &buffer ) != -1 &&
       buffer.st_mode & S_IFDIR )
    return BTRUE;
  return BFALSE;
}

//--------------------------------------------------------------------
BBool CheckIsFile(const BText& fileName)

/*! Verifies if \a fileName is a file.
 * \param fileName name of file
 * \return true if is a file
 */
//--------------------------------------------------------------------
{
  struct stat buffer;
  
  if ( stat(fileName.String(), &buffer) != -1 &&
       buffer.st_mode & S_IFREG )
    return BTRUE;
  return BFALSE;
}

//--------------------------------------------------------------------
BInt GetFileSize(const BText& fileName)

/*! Returns the size in bytes of a file. This internal functions
 *  assumes that the file exists and is a file (see CheckIsFile()).
 * \param fileName name of file
 * \return size of a file
 */
//--------------------------------------------------------------------
{
  struct stat fileStat;
  BInt	 nBytes=0; // fileStat type is off_t that is long and BInt is long

  stat(fileName.String(), &fileStat);
  nBytes=(BInt)fileStat.st_size;

  return(nBytes);
}


//--------------------------------------------------------------------
time_t GetFileTime(const BText& fileName)

/*! Returns the last modification time of a file. This internal
 *  functions assumes that the file exists and is a file (see
 *  CheckIsFile()). This time is measured in secs since
 *  00:00:00 GMT, Jan 1, 1970
 * \param fileName name of file
 * \return seconds from the last modification    
 */
//--------------------------------------------------------------------
{
  struct stat fileStat;
  BInt	 mTime=0; // fileStat type is time_t that is long and BInt is long

  stat(fileName.String(), &fileStat);
  mTime=(BInt)fileStat.st_mtime;

  return(mTime);
}

//--------------------------------------------------------------------
void SetFileTime(const BText& fileName, time_t time)

/*! Puts the last modification and access time of a file
 * \param fromFile file of whith the date is taken
 * \param new date/time 
 */
//--------------------------------------------------------------------
{
  struct utimbuf ut;
  ut.actime  = time; 
  ut.modtime = time; 
  utime(fileName.String(),&ut);
};

//--------------------------------------------------------------------
void TouchAsFile(const BText& fromFile, const BText& toFile)

/*! Puts the last modification time of first file in second file
 * \param fromFile file of whith the date is taken
 * \param toFile file of whith the date is modified
 */
//--------------------------------------------------------------------
{
  SetFileTime(toFile,GetFileTime(toFile));
}


//--------------------------------------------------------------------
// externs
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BBool IsAbsolutePath(const BText& path)

/*! Indicates if path is a an absolute path or not.
 * \param path route of a directory or file
 * \return Returns true if is a an absolute path or zero if relative
 */
//--------------------------------------------------------------------
{
  #ifdef UNIX
  return(path.Get(0)==SLASH);
  #else
  BChar c0 = path.Get(0);
  BChar c1 = path.Get(1);
  BBool ok = (c0==SLASH)||(c0=='/')||(c1==':');
//Std(BText("\nSLASH = ")+SLASH+"\tc0 = "+c0+"\tc1 = "+c1+"\tok = "+ok+"\tpath = "+path);
  return(ok);
  #endif
}


//--------------------------------------------------------------------
  bool IsSlash(char ch)
//--------------------------------------------------------------------
{
  bool isSlash = (ch=='/') || (ch=='\\');
  return(isSlash);
};

//--------------------------------------------------------------------
  BText ReplaceSlash(const BText& path)
//--------------------------------------------------------------------
{
#ifdef UNIX
  return(Replace(path,"\\","/"));
#else
  return(Replace(path,"/","\\"));
#endif
}

//--------------------------------------------------------------------
  BText RemoveLastSlash(const BText& path, bool& endSlash)
//--------------------------------------------------------------------
{
  BText p = path;
  endSlash = false;
  endSlash = IsSlash(p.Last());
  while(IsSlash(p.Last()))
  {
    p.PutLength(p.Length()-1);
  }
  return(p);
}

//--------------------------------------------------------------------
  BText RemoveLastSlash(const BText& path)
//--------------------------------------------------------------------
{
  bool endSlash;
  return(RemoveLastSlash(path,endSlash));
}

//--------------------------------------------------------------------
BText GetAbsolutePath(const BText& path)

/*! Extracts the path of file.
 * \param path_ is a relative or absolute route of a file or directory
 * \return the path of file
 */
//--------------------------------------------------------------------
{
  bool endSlash;
  BText p = RemoveLastSlash(path,endSlash);
  if(!IsAbsolutePath(p))
  {
    BText cur = BDir::GetCurrent();
    if(IsSlash(cur.Last())) { cur.PutLength(cur.Length()-1); }
//  Trace(cur+" es el directorio en curso ");
    if(cur.HasName()) { p = cur+"/"+p; }
    p.Replace('\\', '/' );
  }
  if(BDir::CheckIsDir(p)) 
  {
    BText oldDir = BDir::GetCurrent();
    BDir::ChangeCurrent(p);
    p = BDir::GetCurrent();;
    BDir::ChangeCurrent(oldDir);
  }
  else if(CheckIsFile(p)) 
  {
    BText fp = GetFilePath(p);
    fp = GetAbsolutePath(fp);
    if(!IsSlash(fp.Last())) { fp+="/"; }
    p = fp+GetFileName(p);
  }
  if(endSlash  && !IsSlash(p.Last()))
  {
    p += "/";
  }
  p.Replace("\\","/");
  return(p);
}

//--------------------------------------------------------------------
BText GetStandardAbsolutePath(const BText& path_)

/*! Transform the path to an standard way
 *  for example 
 *    /folder_1/folder_1_1/..
 *  is replaced by 
 *    /folder_1
 * \param path_ is a relative or absolute route of a file or directory
 * \return the path in an standard way
 */
//--------------------------------------------------------------------
{
  bool endSlash = IsSlash(path_.Last());
  BText path = GetAbsolutePath(path_);
#ifdef WIN32
  #define maxPathLength_ 10000
  static char fName[maxPathLength_];
  TCHAR** lppPart={NULL};
  int  len = GetFullPathName(path.String(), maxPathLength_-1, fName, lppPart);
  path.Copy(fName,len);
  path.Replace("\\","/");
#else
  //VBR: hay que buscar alguna forma de estandarizar el path en linux
  //eliminando cosas inútiles como /./
#endif
  if(endSlash && !IsSlash(path.Last()))
  {
    path += "/";
  }
  return(path);
}


//--------------------------------------------------------------------
BText GetFilePath(const BText& nameFile)

/*! Returns the path of file.
 * \param nameFile name of file
 * \return the path of file
 */
//--------------------------------------------------------------------
{
  BText path("");

  BInt l = nameFile.Length()-1;
  while((l>=0) && (nameFile.Get(l)!='\\') && (nameFile.Get(l)!='/')) { l--; }
  path.Copy(nameFile,0,l);
  if(path.Get( path.Length() - 1 )==':') path += "\\";
  return path;
}


//--------------------------------------------------------------------
BText GetFileName(const BText& path)

/*! Extracts the name of file path. Is a text function that don't 
 *  makes any control about the file type (file or dir) or its
 *  existence.
 * \param path route of a file
 * \return the name of file path
 */
//--------------------------------------------------------------------
{
  BText name("");
  BText p = Replace(path,'\\','/');
  if(path.HasName())
  {
    BArray<BText> token;
    ReadAllTokens(p,token,'/');
    name=token[token.Size()-1];
  }
  return(name);
}


//--------------------------------------------------------------------
BText GetFileExtension(const BText& path)

/*! Extracts the extension of file path. Is a text function that 
 *  don't makes any control about the file type (file or dir) or its
 *  existence.
 * \param path route of a file
 * \return the extension of file path
 */
//--------------------------------------------------------------------
{
  BText extension("");

  if(path.HasName())
  {
    BText name=GetFileName(path);
    BArray<BText> token;
    ReadAllTokens(name,token,'.');
    BInt n=token.Size();
    if(n>1) { extension=token[n-1]; }
  }
  return(extension);
}


//--------------------------------------------------------------------
BText GetFilePrefix(const BText& path)

/*! Extracts the prefix of a file pathh. The prefix is the name of 
 *  the file without extension (prefix.ext). Is a text function that
 *  don't makes any control about the file type (file or dir) or 
 *  its existence.
 * \param path route of a file
 * \return the prefix of file path
 */
//--------------------------------------------------------------------
{
  BText prefix("");

  if(path.HasName())
  {
    BText name=GetFileName(path);
    BArray<BText> token;
    ReadAllTokens(name,token,'.');
    prefix=token[0];
  }
  return(prefix);
}


//--------------------------------------------------------------------
// constructors
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BDir::BDir(const BText& name)

/*! Creates a operating system directory object. At the begining
 *  assumes an existing empty directory exist_(BTRUE), empty_(BTRUE),
 *  isFile_(BFALSE), nBytes_(0), numFiles_(0) and numDirs_(0).
 *  The BDir object only contains names of files and subdirectories,
 *  the . and .. directories are not stored because always appears.
 * \param name name of the raiz directory   
 */
//--------------------------------------------------------------------
: BObject(name),
  exist_(BTRUE), 
  empty_(BTRUE), 
  isFile_(BFALSE), 
  dirName_(), 
  fileName_(), 
  numFiles_(0), 
  numDirs_(0),
  nBytes_(0),
  mTime_(0)
{

  BArray<BObject> entryName;	  // All entries excluding . and ..
  BArray<BInt>	isFile;		  // BTRUE if the entries is a file
  BArray<BInt>	bSize;		  // Size in n bytes of each entry
  BArray<time_t>	mTime;		  // Modification time in n secs of each entry
  BInt		numEntries=0;
  BInt		e=0, f=0, d=0;	  // Entries, files & dirs counters
  BList*	lstFil=NIL;
  BInt		l=0;

  if(CheckIsFile(name))
  { // Exits as a file
    isFile_=BTRUE;
    nBytes_=GetFileSize(name);
    mTime_ =GetFileTime(name);
  }
  else
  {
    lstFil=DirFiles(name);
    l=LstLength(lstFil);
    if(l==0)
    {
      exist_=BFALSE;	  // Doesn't exits
    }
    else if(l<=2) {		   }		     // Only . and ..
    else	  { empty_=BFALSE; numEntries=l-2; } // Full of files
  }

  if(! empty_)
  {
    BObject* auxObj;

    entryName.AllocBuffer(numEntries);
    isFile.AllocBuffer(numEntries);
    bSize.AllocBuffer(numEntries);
    mTime.AllocBuffer(numEntries);

    ForAll(BObject,auxObj,lstFil)
    {
      if((auxObj->Name()!=".") && (auxObj->Name()!=".."))
      {
	entryName[e]=*auxObj;

	BText subName(BText("")+name+"/"+entryName[e].Name());

	if(BDir::CheckIsDir(subName))
	{
	  isFile[e]=BFALSE;
	  bSize[e]=0;
	  mTime[e]=0;
	  numDirs_++;
	}
	else
	{
	  isFile[e]=BTRUE;
	  bSize[e]=GetFileSize(subName);
	  mTime[e]=GetFileTime(subName);
	  numFiles_++;
	}

	e++;
      }
    }
    EndFor

    DESTROY(lstFil);

    dirName_.AllocBuffer(numDirs_);
    fileName_.AllocBuffer(numFiles_);
    fileBytes_.AllocBuffer(numFiles_);
    fileTime_.AllocBuffer(numFiles_);

    for(e=0; e<numEntries; e++)
    {
      if(isFile[e])
      {
	fileName_ [f] = entryName[e];
	fileBytes_[f] = bSize[e];
	fileTime_ [f] = mTime[e];
	f++;
      }
      else
      {
	dirName_  [d] = entryName[e];
	d++;
      }
    }
  }
}

