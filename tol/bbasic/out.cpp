/* out.cpp: functions and variables for controlling outputs of
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
#  include <win_tolinc.h>
#endif

#include <tol/tol_bout.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bdat.h>
#include <tol/tol_boper.h>
#include <tol/tol_ois.h>

//--------------------------------------------------------------------
void HCIWriteString(const char* msg)
//--------------------------------------------------------------------
{
  Std(msg);
}

//-------------------------------------------------------------------
void WriteDumpFile(const char* msg) 
//-------------------------------------------------------------------
{ 
  Dump(msg); 
}

//--------------------------------------------------------------------
static BText StartDumpFile()
/*! PURPOSE: Returns a name for the log file. 
 */
//--------------------------------------------------------------------
{
  BText prefix = BSys::TolAppData()+"syslog/start";
  BText start = prefix+".log";
  BDir f(start);
  BText startSession;
  if(f.Exist())
  {
    BBool ok = BFALSE;
    for(BInt i=0; !ok; i++)
    {
      startSession = start;
      if(i) { startSession = prefix+"_"+i+".log"; }
      BDir fi(startSession);
      if(fi.Exist())
      {
        ok = BSys::Remove(startSession);
      }
      else
      {
        ok = BTRUE;
      }
      if(ok)
      {
        FILE* startFile = fopen(startSession.String(),"w");
        ok = startFile!=NULL;
        if(startFile) { fclose(startFile); }
      }
    }
    if(!ok) { start = ""; }
    else    { start = startSession; }
  }
  return(start);
};


//--------------------------------------------------------------------
// inicialization
//   Global variables (static in the class BoBBut)
//--------------------------------------------------------------------
BTraceInit("out.cpp");
BHciWriter BOut::hciWriter_;
BHciWriter BOut::logHciWriter_;
BBool      BOut::initialized_;
BBool      BOut::enabled_;
BBool      BOut::stdHci_;
BBool      BOut::logHci_;
BBool      BOut::traceHci_;
BBool      BOut::errorHci_;
BBool      BOut::warningHci_;
BBool      BOut::infoHci_;
BBool      BOut::stdTerm_;
BBool      BOut::traceTerm_;
BBool      BOut::errorTerm_;
BBool      BOut::warningTerm_;
BBool      BOut::infoTerm_;
BText      BOut::out_;
BText      BOut::dumpFile_(StaticInit());
FILE*      BOut::file_;
BText      BOut::errorOpenTag_(StaticInit());
BText      BOut::errorCloseTag_(StaticInit());
BText      BOut::warningOpenTag_(StaticInit());
BText      BOut::warningCloseTag_(StaticInit());


DefIsAlreadyInitilialized(BOut);

//--------------------------------------------------------------------
    bool BOut::InitializeClass()
/*! PURPOSE: Initializes class BOut. Looks for a not used log file.
 */
//--------------------------------------------------------------------
{
  if(!IsAlreadyInitilialized(BOut)) 
  {
    BText::InitializeClass();
    BOut::initialized_ = BTRUE;
    BOut::enabled_     = BTRUE;
    BOut::stdHci_      = BTRUE;
    BOut::logHci_      = BTRUE;
    BOut::traceHci_    = BTRUE;
    BOut::errorHci_    = BTRUE;
    BOut::warningHci_  = BTRUE;
    BOut::infoHci_     = BTRUE;
    BOut::stdTerm_     = BTRUE;
    BOut::traceTerm_   = BTRUE;
    BOut::errorTerm_   = BTRUE;
    BOut::warningTerm_ = BTRUE;
    BOut::infoTerm_    = BTRUE;
    BOut::out_         = "";
    BOut::dumpFile_    = "";
    BOut::file_        = NIL;
    BOut::hciWriter_   = NIL;
    BOut::logHciWriter_ = NIL;
    BOut::errorOpenTag_    = "<E>\nERROR: ";
    BOut::errorCloseTag_   = "</E>\n";
    BOut::warningOpenTag_  = "<W>\nWarning: ";
    BOut::warningCloseTag_ = "</W>\n";

    if(!HasDumpFile()) 
    { 
      PutDumpFile(StartDumpFile(),BTRUE); 
    }
  }
  return(true);
}


//--------------------------------------------------------------------
void BOut::setErrorTags(const char *openTag, const char *closeTag) 
//--------------------------------------------------------------------
{ 
  errorOpenTag_ = openTag; 
  errorCloseTag_ = closeTag; 
}

//--------------------------------------------------------------------
void BOut::setWarningTags(const char *openTag, const char *closeTag) 
//--------------------------------------------------------------------
{ 
  warningOpenTag_ = openTag; 
  warningCloseTag_ = closeTag; 
}

//--------------------------------------------------------------------
bool BOut::Disable()
//--------------------------------------------------------------------
{
  BBool old = enabled_;
  enabled_=BFALSE;
  return(old!=0);
}

//--------------------------------------------------------------------
bool BOut::Enable()
//--------------------------------------------------------------------
{
  BBool old = enabled_;
  enabled_=BTRUE;
  return(old!=0);
}

//--------------------------------------------------------------------
BDat& TOLErrorNumber()
//--------------------------------------------------------------------
{
  static BDat aux_ = 0;
  return(aux_);
};

//--------------------------------------------------------------------
BDat& TOLWarningNumber()
//--------------------------------------------------------------------
{
  static BDat aux_ = 0;
  return(aux_);
};


//--------------------------------------------------------------------
void  BOut::FlushAll()
/*! PURPOSE: Flushes all output streams
 */
//--------------------------------------------------------------------
{
  #ifdef UNIX 
  fflush(NULL);
  #else 
  flushall();
  #endif
}

/*! PURPOSE: Sets boolean value for errorHci_ */
void  BOut::PutErrorHci(BBool hci)  { errorHci_ = hci; }
/*! PURPOSE: Sets boolean value for warningHci_ */
void  BOut::PutWarningHci(BBool hci)  { warningHci_ = hci; }
/*! PURPOSE: Sets boolean value for traceHci_ */
void  BOut::PutTraceHci(BBool hci)  { traceHci_ = hci; }
/*! PURPOSE: Sets boolean value for stdHci_ */
void  BOut::PutStdHci(BBool hci)  { stdHci_ = hci; }
/*! PURPOSE: Sets boolean value for infoHci_ */
void  BOut::PutInfoHci(BBool hci)  { infoHci_ = hci; }
/*! PURPOSE: Sets boolean value for stdHci_ */
void  BOut::PutLogHci(BBool hci)  { logHci_ = hci; }
/*! PURPOSE: Sets boolean value for errorTerm_ */
void  BOut::PutErrorTerm(BBool term) { errorTerm_ = term; }
/*! PURPOSE: Sets boolean value for warningTerm_ */
void  BOut::PutWarningTerm(BBool term) { warningTerm_ = term; }
/*! PURPOSE: Sets boolean value for traceTerm_ */
void  BOut::PutTraceTerm(BBool term) { traceTerm_ = term; }
/*! PURPOSE: Sets boolean value for stdTerm_ */
void  BOut::PutStdTerm(BBool term) { stdTerm_ = term; }
/*! PURPOSE: Sets boolean value for infoTerm_ */
void  BOut::PutInfoTerm(BBool term) { infoTerm_ = term; }
/*! PURPOSE: Sets boolean value for all hci flags */
void  BOut::PutAllHci(BBool hci)
{ stdHci_ = traceHci_ = errorHci_ = warningHci_ = infoHci_ = hci; }
/*! PURPOSE: Sets boolean value for all term flags */
void  BOut::PutAllTerm(BBool term)
{ stdTerm_ = traceTerm_ = errorTerm_ = warningTerm_ = infoTerm_ = term; }

/*! PURPOSE: Gets boolean value of errorHci_ */
BBool BOut::ErrorHci() { return(errorHci_); }
/*! PURPOSE: Gets boolean value of warningHci_ */
BBool BOut::WarningHci() { return(warningHci_); }
/*! PURPOSE: Gets boolean value of traceHci_ */
BBool BOut::TraceHci() { return(traceHci_); }
/*! PURPOSE: Gets boolean value of stdHci_ */
BBool BOut::StdHci() { return(stdHci_); }
/*! PURPOSE: Gets boolean value of stdHci_ */
BBool BOut::InfoHci() { return(infoHci_); }
/*! PURPOSE: Gets boolean value of errorTerm_ */
BBool BOut::ErrorTerm() { return(errorTerm_); }
/*! PURPOSE: Gets boolean value of errorTerm_ */
BBool BOut::WarningTerm() { return(warningTerm_); }
/*! PURPOSE: Gets boolean value of traceTerm_ */
BBool BOut::TraceTerm() { return(traceTerm_); }
/*! PURPOSE: Gets boolean value of stdTerm_ */
BBool BOut::StdTerm() { return(stdTerm_); }
/*! PURPOSE: Gets boolean value of infoTerm_ */
BBool BOut::InfoTerm() { return(infoTerm_); }


//--------------------------------------------------------------------
BText& Out()

/*! PURPOSE: Returns a cleaned BText reference to concatenate messages
 */
//--------------------------------------------------------------------
{
  BOut::out_.PutLength(0);
  return(BOut::out_);
}


//--------------------------------------------------------------------
BBool BOut::HasDumpFile()

/*! PURPOSE: Returns true if a log file is avalaible.
 */
//--------------------------------------------------------------------
{ 
  return (dumpFile_.HasName()); 
}



//--------------------------------------------------------------------
const BText& BOut::DumpFile()

/*! PURPOSE: Returns the actual log file name
 */
//--------------------------------------------------------------------
{ 
  return(dumpFile_); 
}


//--------------------------------------------------------------------
BBool BOut::PutDumpFile()

/*! PURPOSE: Disables the log file output.
 */
//--------------------------------------------------------------------
{
  if(file_)        { fclose(file_);  }
  if(HasDumpFile()) { dumpFile_ = ""; }
  return(BFALSE);
}

//--------------------------------------------------------------------
BBool BOut::PutDumpFile(const BText& name)
/*! PURPOSE: Changes the log file to another whose name is given by \a name.
 * \param name Name of new log file
 */
//--------------------------------------------------------------------
{
  return PutDumpFile(name, BTRUE);
}

//--------------------------------------------------------------------
BBool BOut::PutDumpFile(const BText& name, BBool reset)

/*! Changes the log file to another whose name is given by \a name.
 *  New file can be overwritten or not deppending on \a reset.
 * \param name Name of new log file
 * \param reset Boolean to indicate if log file must be overwritten or not
 */
//--------------------------------------------------------------------
{
  BText path = GetAbsolutePath(name);
  BText old = dumpFile_;
  if(HasDumpFile() && (path != dumpFile_))
  {
    Std(I2("\nOutput dump file will be set to ",
           "\nEl fichero de volcado de salida se ubica en ")+
           "\n"+path+"\n");
  }
  if(file_)  { fclose(file_);  }
  file_ = fopen(name.String(),reset?"w":"a");
  if(file_)
  {
    dumpFile_ = path;
    return(BTRUE);
  }
  else
  {
    Std(I2("\nCannot open for write file ",
           "\nNo se puede abrir para escritura el fichero ")+
           "\n"+path+"\n");
    file_ = fopen(old.String(),"a");
    Std(I2("\nOutput dump file will be restored to ",
           "\nEl fichero de volcado de salida se restaura a ")+
           "\n"+old+"\n");
    return(BTRUE);
  }
}


//--------------------------------------------------------------------
void Error(const BText& txt)

/*! Sends an error message \a txt to all current avalaible outputs
 * \param txt Text to send
 */
//--------------------------------------------------------------------
{
  if(!BOut::IsEnabled()) { return; }
  /*BText info =  "\nERROR: ";
    info += txt + "\n";*/
  TOLErrorNumber()+=1;
  BText info = 
    BOut::errorOpenTag_ + TOLErrorNumber().Format("[%.0lf] ") + txt + 
    BOut::errorCloseTag_;
  BOut::Write(info, BOut::errorHci_, BOut::errorTerm_);
  BUserFunction::ShowCallStack();
}


//--------------------------------------------------------------------
void Warning(const BText& txt)

/*! Sends a warning message to all current avalaible outputs
 * \param txt Text to send
 */
//--------------------------------------------------------------------
{
  if(!BOut::IsEnabled()) { return; }
  /*BText info =  "\nWARNING: ";
  info += txt + "\n";*/
  TOLWarningNumber()+=1;
  BText info = 
    BOut::warningOpenTag_ + TOLWarningNumber().Format("[%.0lf] ") + txt + 
    BOut::warningCloseTag_;
  BOut::Write(info, BOut::warningHci_, BOut::warningTerm_);
}

//--------------------------------------------------------------------
void Deprecated(const BText& txt)

/*! Sends a deprecated message to all current avalaible outputs
 * \param txt Text to send
 */
//--------------------------------------------------------------------
{
  if(!BOut::IsEnabled()) { return; }
  BText info = I2("\nDeprecated: ","\nDesaprobado: ");
  info += txt + "\n";
  BOut::Write(info, BOut::warningHci_, BOut::warningTerm_);
}

//--------------------------------------------------------------------
void Std(const BText& txt)

/*! Sends a standard message \a txt to all current avalaible outputs
 * \param txt Text to send
 */
//--------------------------------------------------------------------
{
  if(!BOut::IsEnabled()) { return; }
  BOut::Write(txt, BOut::stdHci_, BOut::stdTerm_);
}

//--------------------------------------------------------------------
/*! Sends \a txt message if the Info Output Level is active.
 */
void Info(const BText& txt)
{
  if(!BOut::IsEnabled()) { return; }
  BOut::Write(txt, BOut::infoHci_, BOut::infoTerm_);
}

//--------------------------------------------------------------------
static BText SystemInfo()

/*! Returns actualized system information.
 */
//--------------------------------------------------------------------
{
  return(BText("Time = ")    + BTimer::Text() +
  "\tCores = "  + BCore::NCore());
}


//--------------------------------------------------------------------
void Trace(const BText& txt)

/*! Sends a trace message \a txt to all current avalaible outputs
 * \param txt Text to send
 */
//--------------------------------------------------------------------
{
  if(!BOut::IsEnabled()) { return; }
  BText info = BText("\nTRACE ") +SystemInfo() + "\n" + txt + "\n";
  BOut::Write(info, BOut::traceHci_, BOut::traceTerm_);
}

//-------------------------------------------------------------------
void Dump(const BText& txt)
{
  if(!BOut::IsEnabled()) { return; }
  if(BOut::HasDumpFile() && BOut::file_) 
  {
    fprintf(BOut::file_, "%s", txt.String());
    fflush(BOut::file_);
  }
}

//--------------------------------------------------------------------
void BOut::Write(const BText& txt, BBool hci, BBool term)

/*! Sends a message \a txt to specified outputs (user defined and/or
 *  terminal). If log file is defined, message is also written to it.
 *  If exists an user defined funcion to write (hciWriter_), message
 *  isn't sent to standard output despite of \a term param is true.
 * \param txt Text to send
 * \param hci Boolean that indicates if message must be send to hci output
 * \param term Boolean that indicates if message must be send to term output
 */
//--------------------------------------------------------------------
{
  if(!BOut::IsEnabled()) { return; }
  if(hci)
  {
   HciWrite(txt);
  }
  if(term)
  {
    if(!hciWriter_)
    {
      BChar* str = (BChar*)(txt.String());
      printf("%s",str);
//    flushall(); ASC: NO EXISTE EN UNIX
      fflush(stdout);
    }
  }
  if(BOut::HasDumpFile()&&file_)
  {
    fprintf(file_,"%s",txt.String());
    fflush (file_);
  }
}


//--------------------------------------------------------------------
void BOut::HciWrite(const BText& txt)

/*! PURPOSE: If there is a hci-writer sends the text \a txt to it.
 * \param txt  Text to send to the hci-writer
 */
//--------------------------------------------------------------------
{
  if(!BOut::IsEnabled()) { return; }
  if(hciWriter_)    { (*hciWriter_)(txt); }
}


//--------------------------------------------------------------------
void BOut::LogHciWrite(const BText& txt)
/*! If there is a logHciWriter sends the text \a txt to it.
 * \param txt  Text to send to the hci-writer
 */
//--------------------------------------------------------------------
{
  if(logHciWriter_) { (*logHciWriter_)(txt); } 
  else              { Std(txt); }
}


//--------------------------------------------------------------------
  bool CheckNonDeclarativeAction(const BText& msg)
//--------------------------------------------------------------------
{
  if(!BOis::RunningUseModule()) { return(false); }
  Error(BText("OIS: ")+
   "Sorry, non declarative actions are forbidden while Ois.UseModule "
   "is running. Cannot use \n"+msg);
  return(true);
}
