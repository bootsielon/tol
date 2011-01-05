 /* bout.h: Implementing an environment independent output behavior
            for GNU/TOL Language.

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

/*! \class BOut
//  \brief Implementing an environment independent output behavior
           Functionality has been implemented in out.cpp
*/

#ifndef TOL_BOUT_H
#define TOL_BOUT_H 1

#include <tol/tol_btext.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BDat;
TOL_API BDat& TOLErrorNumber();
TOL_API BDat& TOLWarningNumber();

//--------------------------------------------------------------------
// macroes
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
//! pointer to user defined function
typedef void (*BHciWriter)(const BText&);
typedef void (*BHciWriterEx)(const BText&, void*);


//--------------------------------------------------------------------
class TOL_API BOut

/*! Static class that implements an environment independent output
 *  device for errors, warnings, traces and messages.
 *  Text terminal and log-files are implemented and other outputs
 *  like Windows or X-Windows styles (Human-Computer Interface) are
 *  supported by external function's pointer.
 */
//--------------------------------------------------------------------
{
private:
  //! Boolean to indicate if class has been initialized
  static BBool	  initialized_; 
  //! Boolean to indicate if class is currently enabled
  static BBool	  enabled_; 
  //! Text object used to concatenate messages
  static BText	  out_;
  //! Log file's name
  static BText	  dumpFile_; 
  //! Log file
  static FILE*	  file_;
  //! Boolean that indicates if standard messages must be sent to hciWriter
  static BBool	  stdHci_;
  //! Boolean that indicates if log messages must be sent to logHciWriter
  static BBool    logHci_;
  //! Boolean that indicates if trace messages must be sent to hciWriter
  static BBool	  traceHci_;
  //! Boolean that indicates if error messages must be sent to hciWriter
  static BBool	  errorHci_;
  //! Boolean that indicates if warning messages must be sent to hciWriter
  static BBool	  warningHci_;
  //! indicates if an INFO Level message must be sent to hciWriter
  static BBool	  infoHci_;
  //! Boolean that indicates if standard messages must be sent to terminal
  static BBool	  stdTerm_;
  //! Boolean that indicates if trace messages must be sent to terminal
  static BBool	  traceTerm_;
  //! Boolean that indicates if error messages must be sent to terminal
  static BBool	  errorTerm_;
  //! Boolean that indicates if warning messages must be sent to terminal
  static BBool	  warningTerm_;
  //! indicates if an INFO Level message must be sent to terminal
  static BBool	  infoTerm_;
  //! Send a message to specified outputs
  static void Write    (const BText& txt, BBool hci, BBool term);
  //! Pointer to user defined function to write TOL messages
  static BHciWriter hciWriter_;
  static BHciWriter logHciWriter_;
  static BHciWriterEx hciWriterEx_;
  static void       *hciWriterExData_;
  static BHciWriterEx logHciWriterEx_;
  static void       *logHciWriterExData_;

  static BText errorOpenTag_; static BText errorCloseTag_;
  static BText warningOpenTag_; static BText warningCloseTag_;

public:
  // Access and manipulation: inline

  //! Disable output
  static bool IsEnabled() { return(initialized_ && enabled_); }
  //! Disable output
  static bool Disable();
  //! Enable output
  static bool Enable();
  //! Initializes class BOut
  static bool  InitializeClass  ();
  //! Flushes all output streams
  static void  FlushAll();
  //! Returns the actual log file name
  static const BText& DumpFile	  ();
  //! Returns true if a log file is available
  static       BBool  HasDumpFile ();
  //! Disables the log file output
  static       BBool  PutDumpFile ();
  //! Changes the log file
  static       BBool  PutDumpFile (const BText& name);
  //! Changes the log file. New file can be overwritten or not.
  static       BBool  PutDumpFile (const BText& name, BBool reset);
  //! Sends the text to Standard output managed by hciWriter_ attribute
  static       void   HciWrite	  (const BText& txt);
  //! Sends the text to Log Output managed by logHciWriter_ attribute
  static       void   LogHciWrite (const BText& txt);
  //! Sets hciWriter 
  static       void   PutHciWriter(BHciWriter h) { 
      hciWriter_=h; 
      hciWriterEx_=NULL; 
  }
  static       void   PutHciWriterEx(BHciWriterEx h, void *ClientData) { 
      hciWriter_=NULL; 
      hciWriterEx_=h;
      hciWriterExData_ = ClientData;
  }
  static       void   PutLogHciWriter(BHciWriter h) { 
      logHciWriter_=h; 
      logHciWriterEx_=NULL; 
  }
  static       void   PutLogHciWriterEx(BHciWriterEx h, void *ClientData) { 
      logHciWriter_=NULL; 
      logHciWriterEx_=h;
      logHciWriterExData_ = ClientData;
  }

  //! Returns the actual hciWriter
  static       BHciWriter HciWriter () { return(hciWriter_); }

  //! Sets boolean value for errorHci_
  static void PutErrorHci(BBool hci);
  //! Sets boolean value for warningHci_
  static void PutWarningHci(BBool hci);
  //! Sets boolean value for traceHci_
  static void PutTraceHci(BBool hci);
  //! Sets boolean value for stdHci_
  static void PutStdHci(BBool hci);
  //! Sets boolean value for infoHci_
  static void PutInfoHci(BBool hci);
  //! Sets boolean value for logHci_
  static void PutLogHci(BBool hci);
  //! Sets boolean value for errorTerm_
  static void PutErrorTerm(BBool term);
  //! Sets boolean value for warningTerm_
  static void PutWarningTerm(BBool term);
  //! Sets boolean value for traceTerm_
  static void PutTraceTerm(BBool term);
  //! Sets boolean value for stdTerm_
  static void PutStdTerm(BBool term);
  //! Sets boolean value for infoTerm_
  static void PutInfoTerm(BBool term);
  //! Sets boolean value for all terms
  static void PutAllHci(BBool hci);
  //! Sets boolean value for all hci
  static void PutAllTerm(BBool term);

  static void setErrorTags(const char* openTag, const char * closeTag);
  static void setWarningTags(const char *openTag, const char *closeTag);

  //! Gets boolean value of errorHci_
  static BBool ErrorHci();
  //! Gets boolean value of errorHci_
  static BBool WarningHci();
  //! Gets boolean value of traceHci_
  static BBool TraceHci();
  //! Gets boolean value of stdHci_
  static BBool StdHci();
  //! Gets boolean value of stdHci_
  static BBool InfoHci();
  //! Gets boolean value of stdHci_
  static BBool LogHci();
  //! Gets boolean value of errorTerm_
  static BBool ErrorTerm();
  //! Gets boolean value of traceTerm_
  static BBool WarningTerm();
  //! Gets boolean value of traceTerm_
  static BBool TraceTerm();
  //! Gets boolean value of stdTerm_
  static BBool StdTerm();
  //! Gets boolean value of infoTerm_
  static BBool InfoTerm();

  //! Returns a cleaned BText reference to concatenate message
  friend TOL_API BText& Out();
  //! Sends an error message to all current available outputs
  friend TOL_API void  Error(const BText&);
  //! Sends a warning message to all current available outputs
  friend TOL_API void Warning(const BText&);
  //! Sends a deprecated message to all current available outputs
  friend TOL_API void Deprecated(const BText&);
  //! Sends a standard message to all current available outputs
  friend TOL_API void Std(const BText&);
  //! Sends a trace message to all current available outputs
  friend TOL_API void Trace(const BText&);
  //! Write the message given as parameter into the Dump File
  friend TOL_API void Dump(const BText&);
  //! Sends an Info level message to available outputs
  friend TOL_API void Info(const BText&);
  //! Sends an error message to all current available outputs
  //when it occurs a non declarative action while Ois.UseModule
  //is running
  //friend TOL_API bool CheckNonDeclarativeAction(const BText&);
  
  friend class TC_MsgMan;
};

bool CheckNonDeclarativeAction(const BText&);

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
void HCIWriteString(const char* msg);
void WriteDumpFile(const char* msg);


//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define TRZ(A) Trace(BText(#A" = \n")+A.Name())

//! Returns a cleaned BText reference to concatenate message
TOL_API BText& Out();
//! Sends an error message to all current available outputs
TOL_API void  Error(const BText&);
//! Sends a warning message to all current available outputs
TOL_API void Warning(const BText&);
//! Sends a deprecated message to all current available outputs
TOL_API void Deprecated(const BText&);
//! Sends a standard message to all current available outputs
TOL_API void Std(const BText&);
//! Sends a trace message to all current available outputs
TOL_API void Trace(const BText&);
//! Write the message given as parameter into the Dump File
TOL_API void Dump(const BText&);
//! Sends an Info level message to available outputs
TOL_API void Info(const BText&);

#endif // TOL_BOUT_H
