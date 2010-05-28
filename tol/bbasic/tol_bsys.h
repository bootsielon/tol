/* tol_bsys.h: System calls functions.
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

/*! \class BSys
//  \brief Implementing system calls functions
           Functionality has been implemented in sys.cpp
*/

#ifndef TOL_BSYS_H
#define TOL_BSYS_H 1

#include <tol/tol_bobject.h>
#include <tol/tol_bout.h>
#include <tol/tol_blist.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
typedef void (* BOpenProc) (const BText&, BInt);

//--------------------------------------------------------------------
// constants
//--------------------------------------------------------------------
#define SYSDEFECTPREFIX "TOL"

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BDir;
class BDate;


//--------------------------------------------------------------------
class BSys

/*! System calls. It provides methods to access system services like:
 *  environment variables access, system command execution, temporal
 *  file name generation, making directories, removing, coping and
 *  printing files. It also manages predefined files, tables and 
 *  charts editors by function pointers.
 */
//--------------------------------------------------------------------
{
private:
  //! boolean to indicate if operating system is unix
  static BBool	unix_;
  //! Name of default editor
  static BText	editor_;
  //! Pointer to file editor
  static BOpenProc fileEditor_;
  //! Pointer to table editor
  static BOpenProc tableEditor_;
  //! Pointer to chart editor
  static BOpenProc chartEditor_;
  //! hostname
  static BText hostName_;
  static BDate* session_initTime_;

public:
  // Access and manipulation: inline
  //! Returns if operating system is Unix
  static       BBool  Unix()   { return(unix_); }
  //! Returns the default editor
  static const BText& Editor() { return(editor_); }
  //! Return hostname_
  static const BText& HostName() { return(hostName_); }
  //! Sets the default editor
  static       void   PutEditor(const BText& e) { editor_=e; }

  // Utilities
  //! Sets file editor
  static void	PutFileEditor (BOpenProc editor) { fileEditor_	= editor; }
  //! Sets table editor
  static void	PutTableEditor(BOpenProc editor) { tableEditor_ = editor; }
  //! Sets chart editor
  static void	PutChartEditor(BOpenProc editor) { chartEditor_ = editor; }

  //! Returns the standard system directory to store application data
  static BText	TolAppData();
  //! Creates a directory
  static BBool	MkDir(const BText& dir, bool force=false);
  static BBool  RmDir(const BText& dir);

  //! Deletes all files in a directory that starts with a text and that hasn't 
  //! been modified for the last \a lag hours.
  static void	DeleteTmpFiles(BDir& dir, BReal lag, BText prefix);
  //! Creates a unique temporal file name in specific directory
  static BText	TempNam	   (const BText& outputDir="",
			    const BText& prefix=SYSDEFECTPREFIX,
			    const BText& ext="",
				  BInt	 len=8);
  //! Gets an environment variable
  static BText	GetEnv	   (const BText& envVar);
  //! Executes a shell command
  static BBool	System	   (const BText& command);
  
#ifndef	  UNIX
  static BBool	WinExecuteFile (const BText& fileName);
  static BBool	WinSystem  (const BText& command,
				                    BInt	showMode, 
                            bool  wait=BFALSE);
#endif
  static BBool ChildProcess(const BText& command);
  //! Prints a file in the default printer.
  static BBool Print(const BText& fileName);
  //! Removes a file
  static BBool Remove(const BText& fileName);
  //! Copies \a origin file(s) to \a target
  static BBool Copy(const BText& origin, const BText& target);
  static BBool Cat(const BArray<BText>& origin, const BText& target);

  //! Prints a file in the default printer and removes this file from disk.
  static BBool	PrintUnlink(const BText& fileName);
  //! Removes a file from disk
  static BBool	Unlink	   (const BText& fileName);
  //! Returns a list with names of files of directory \a dirName
  static BList* DirFiles   (const BText& dirName);
  //! Edits a file in the default editor
  static BBool	Edit	   (const BText& fileName, BInt typeIndex);
  //! Edits a file in the default table editor
  static BBool	EditTable  (const BText& fileName, BInt typeIndex);
  //! Edits a file in the default chart editor
  static BBool	EditChart  (const BText& fileName, BInt typeIndex);
  //! Edits a file in the default editor and removes this file from disk
  static BBool	EditUnlink (const BText& fileName, BInt typeIndex);
  //! Edits a file in the default editor. Edition is a blocking process
  static BBool	EditB	   (const BText& fileName);
  //! Edits a new file in the default editor
  static BBool	EditNew	   ();
  //! Edits a text using a temporary file in the default editor
  static BBool	EditText   (const BText& text);
  //! Converts a text file from UNIX format to DOS format
  static void	Unix2Dos   (const BText& fileName);
  //! Converts a text file from DOS format to UNIX format
  static void	Dos2Unix   (const BText& fileName);
  //! Returns current available memory in bytes
  static double SessionAvailMem();
  //! Return the time elapsed since program started
  static double	SessionTime();
  static const BDate& SessionInitTime();
  //Make inactive waiting until they have passed a given number of milliseconds
  static void SleepMilliSeconds(unsigned int milliseconds);
  //Opens a file and locks it
  //If cannot do it then sleeps and retries again until time is out
  static FILE* FOpenAndLock(
   const char *filename,
   const char *mode,
   unsigned int timeOutInMilliseconds,
   char* errorMessage);
  //Unlocks a file and close it
  static bool FUnlockAndClose(FILE* file, const char *filename);
};

//! Generate a pseudo-random series of uniformely distributed in [0,1]
BInt LongRandom(BInt max);

#endif // TOL_BSYS_H
