/* tol_bdir.h: BDir(dir)class and functions header
               GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or(at your option)
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

/*! \class BDir.h
 * \brief Defining BDir basic class, its attributes and methods prototypes
 * \brief functionality has been implemented in dir.cpp, dirtext.cpp and
 * \brief dircomma.cpp
 */         

#ifndef TOL_BDIR_H
#define TOL_BDIR_H 1

#include <tol/tol_bobject.h>
#include <tol/tol_barray.h>

#if HAVE_DIRENT_H
#  include <dirent.h>
#  define NAMLEN(dirent)strlen((dirent)->d_name)
#else
#  define dirent direct
#  define NAMLEN(dirent)(dirent)->d_namlen
#  if HAVE_SYS_NDIR_H
#    include <sys/ndir.h>
#  endif
#  if HAVE_SYS_DIR_H
#    include <sys/dir.h>
#  endif
#  if HAVE_NDIR_H
#    include <ndir.h>
#  endif
#endif

#ifdef __MINGW32__
#  define SLASH	 '/'
#  define TSLASH "/"
#  define DIRMAXNAMLEN MAX_PATH
#elif UNIX
#  define SLASH	 '/'
#  define TSLASH "/"
#  define DIRMAXNAMLEN MAXNAMLEN
#else
#  define SLASH	 '\\'
#  define TSLASH "\\"
#  ifdef _MSC_VER
#    include <stdlib.h>
#    define DIRMAXNAMLEN _MAX_FNAME
#  else
#    define DIRMAXNAMLEN MAXPATH
#  endif
#endif

#ifndef _MSC_VER
#  define direct dirent
#else
#  include <direct.h>
#endif

#if _MSC_VER
#  include <io.h>
#endif  // _MSC_VER
#include <sys/stat.h>

//--------------------------------------------------------------------
class BDir: public BObject

/*! Operating system directories. This object gives the following
 *  information:
 *    - Files: Only files.
 *    - Dirs : Siubdirectories inside the directory.
 *             The own directory . and its parent .. is allways omitted.
 */
//--------------------------------------------------------------------
{
private:
  BBool           exist_;     //!< BTRUE if the directory(or file)exists
  BBool           isFile_;    //!< BTRUE if is a file, not a directory
  BInt            nBytes_;    //!< Size if is a file, 0 id is a directory
  time_t          mTime_;     //!< Last modification time measured in seconds
  BBool           empty_;     //!< BFALSE if the directory only has . and ..
  BArray<BObject> dirName_;   //!< All subdirectories excluding . and ..
  BArray<BObject> fileName_;  //!< All files
  BArray<BInt>	  fileBytes_; //!< Size of all files
  BArray<time_t>  fileTime_;  //!< Modification time of all files
  BInt            numFiles_;  //!< Number of files
  BInt            numDirs_;   //!< Number of subdirectories excluding . and ..

public:
  // Constructors and destructors:
  //! Create a new BDir
  BDir(const BText& dirName);
  //! Destroy a BDir
 ~BDir(){ }

  // Access and manipulation: inline
  //! \a fileName_ returns elements n-th of the attribute 
  BText FileName(BInt n) const { return(fileName_[n].Name()); }
  //! \a fileTime_ returns elements n-th of the attribute
  BInt	FileBytes(BInt n) const { return(fileBytes_[n]); }
  //! \a fileTime_ returns elements n-th of the attribute
  time_t FileTime(BInt n) const { return(fileTime_[n]); }
  //! \a dirName_ returns elements n-th of the attribute
  BText DirName(BInt n)	 const { return(dirName_[n].Name()); }
  //! return \a numFiles_ attribute
  BInt	NumFiles() const { return(numFiles_); }
  //! return \a numDirs_ attribute
  BInt	NumDirs() const { return(numDirs_); }
  //! return \a nBytes_ attribute
  BInt	Bytes() const { return(nBytes_); }
  //! return \a mTime_ attribute
  time_t Time() const { return(mTime_); }
  //! return \a exist_ attribute
  BBool Exist() const { return(exist_); }
  //! return \a empty_ attribute
  BBool Empty() const { return(empty_); }
  //! indicates if it exist and he is a directory
  BBool IsDir() const { return(exist_ && !isFile_); }
  //! indicates if it exist and he is a file
  BBool IsFile() const { return(exist_ &&  isFile_); }

  // Functions:
  //! finds for the name of a file
  BInt	FindFile(const BText& fileName) const;
  //! finds for the name of a directory
  BInt	FindDir(const BText& dirName ) const;
  //! Transform in text a directory object
  BText Text(BBool sizes=BFALSE, BInt columns=3, BBool recursive=BFALSE);
  //! Execute an instruction to all the archives of the directory
  BInt	Command(BBool trace, BBool recursive,
		const BText& command, const BText& pattern);

  // General:
  //! Get current directory 
  static BText GetCurrent();
  //! Change the current directory
  static void  ChangeCurrent(const BText& d);

  static BBool CheckIsDir(const BText& dirName);
  DeclareClassNewDelete(BDir);
};


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
//! Verifies if he is a directory 
//BBool CheckIsDir(const BText& dirName);
//! Verifies if he is a file
TOL_API BBool CheckIsFile(const BText& fileName);
//! Gets the path of file
TOL_API BText GetFilePath(const BText& nameFile);
//! Gets the name of file path 
TOL_API BText GetFileName(const BText& path);
//! Gets the extension of file
TOL_API BText GetFileExtension(const BText& path);
//! Gets the name of file without extension 
TOL_API BText GetFilePrefix(const BText& path);
//! Gets an absolute path of file 
TOL_API BText GetAbsolutePath(const BText& path);
//! Gets the path of file in an standard way
TOL_API BText GetStandardAbsolutePath(const BText& path_);
//! Indicates if path is absolute or relative
TOL_API BBool IsAbsolutePath(const BText& path);
//! Gets the size of a file
TOL_API BInt   GetFileSize(const BText& fileName);
//! Gets the last modification time of a file
TOL_API time_t GetFileTime(const BText& fileName);
//! Gets the last modification and access time of a file
TOL_API void   SetFileTime(const BText& fileName, time_t time);
//! Pust the last modification time of first file in second file 
TOL_API void TouchAsFile(const BText& fromFile,
					                          const BText& toFile);
//! Gets the path in the operative system style
TOL_API BText SysPath	        (const BText& path);
//! Returns true for both / and \ slashes
TOL_API bool  IsSlash         (char ch);
//! Replace all slashes by standard one for operative system
TOL_API BText ReplaceSlash    (const BText& path);
//! Removes last slash and puts true endSlash if paths end at an slash
TOL_API BText RemoveLastSlash (const BText& path, bool& endSlash);
//! Removes last slash if paths end at an slash
TOL_API BText RemoveLastSlash (const BText& path);

#endif // TOL_BDIR_H
