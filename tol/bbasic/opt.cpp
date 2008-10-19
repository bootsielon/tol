/* opt.cpp: Manange the main(int argc, char *argv[]) options
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

#include <tol/tol_btext.h>
#include <tol/tol_bopt.h>

//--------------------------------------------------------------------
BOpt::BOpt(      BInt   numOptions,
	         BBool  freeLet,
	   const BText& samples,
	   const BText& copyright,
	   const BText& margin)
    
/*! Creates a option object.
 */
//--------------------------------------------------------------------
: BObject(""),
  counter_(0), optArg_(numOptions), freeLet_(freeLet),
  errorArg_(BFALSE), errorMsg_("\nERROR:\n"),
  samples_(samples), copyright_(copyright), margin_(margin)
{ }


//--------------------------------------------------------------------
BInt BOpt::PutOption(const BText& option,
		     const BText& description,
		     const BText& defValue,
	  	           BBool  parametric,
		           BBool  mandatory,
		           BBool  numeric,
		           BReal  minimum,
		           BReal  maximum)
    
/*! Initialize an option and return the option handler.
 */
//--------------------------------------------------------------------
{
  counter_++;
  BInt n=0;

  if(counter_>NumOptions()) { Error(Out()+"Too much options"); }
  else
  {
    n=counter_-1;

    PutOption	  (n, option);
    PutDescription(n, description);
    PutDefault	  (n, defValue);
    PutMandatory  (n, mandatory);
    PutAppear	  (n, BFALSE);
    PutParametric (n, parametric);
    PutNumeric	  (n, numeric);
    PutParameter  (n, BText(""));
    PutMinimum	  (n, minimum);
    PutMaximum	  (n, maximum);
  }
  return(n);
}


//--------------------------------------------------------------------
// internal functions
//--------------------------------------------------------------------


//--------------------------------------------------------------------
static BBool IsOption(const BText& arg)

/*! Returns true if arg has the option format -x[x], for examples:
 *  -x, -X, -alfa, -Alfa, -m100, -G95, etc.
 */
//--------------------------------------------------------------------
{
  if(arg.Length()<2) { return(BFALSE); }
  return((arg(0)=='-') && (arg(1)>='A') && (arg(1)<='z'));
}


//--------------------------------------------------------------------
// functions
//--------------------------------------------------------------------


//--------------------------------------------------------------------
BBool BOpt::ParseShow(const BText& commandLine)

/*! If not parse show the error and the help. As Parse() returns
 *  BTRUE it works ok.
 */
//--------------------------------------------------------------------
{
  if(! ParseArg(commandLine)) { Std(ErrorMsg()+Help()); }
  return(! ErrorArg());
}


//--------------------------------------------------------------------
BBool BOpt::ParseShow(BInt argc, BChar** argv)

/*! If not parse show the error and the help. As Parse() returns
 *	    BTRUE it works ok.
 */
//--------------------------------------------------------------------
{
  if(! ParseArg(argc,argv)) { Std(ErrorMsg()+Help()); }
  return(! ErrorArg());
}


//--------------------------------------------------------------------
BBool BOpt::ParseArg(const BText& commandLine)

/*! Parse the a command line (for example "application -r 3 -x")
 *	    Returns BTRUE it works ok.
 */
//--------------------------------------------------------------------
{
  BArray<BText> args;
  ReadAllTokens(commandLine,args,' ');
  BInt n=args.Size();
  BArray<BChar*> argv(n);
  BInt i;

  for(i=0; i<n; i++) { argv[i]=args[i].DupBuffer(); }

  if(n>0) { ParseArg(argv.Size(),argv.GetBuffer()); }
  else
  {
    AddErr(I2("Empty command line options","Mandato vacio, sin opciones"));
    AddErr(I2("Internal error","Error interno"));
  }

  for(i=0; i<n; i++) { delete argv[i]; argv[i]=NIL; }

  return(! ErrorArg());
}


//--------------------------------------------------------------------
BBool BOpt::ParseArg(BInt argc, BChar** argv)

/*! Parse the main(int argc, char *argv[]) arguments to met the
 *  options. Returns BTRUE it works ok. The programmer error, like
 *  the Bad number of options are stored as the user errors
 *  in the errorMsg_ text, one per line.
 *
 * ALGORITHM:
 *   If the programmer put the correct number of options:
 *     Read all arguments traversing argv[n] while n<argc.
 *     Assing defaults values for options that not appear and
 *	 check mandatory options.
 *     Process and check the free arguments (letFree_).
 *   Return the parse results.
 */
//--------------------------------------------------------------------
{
  if(counter_!=NumOptions())
  {
    AddErr(I2("Bad number of options","Numero erroneo de opciones"));
    AddErr(I2("Internal error","Error interno"));
  }
  else
  {
    BInt   i=0; // Option identifier
    BInt   n=0; // Counter for argv[n]
    PutName(argv[n]); n++;
    BList* lstFree=NIL;

    while(n<argc)
    {
      if(!IsOption(argv[n])) { lstFree=Cons(new BObject(argv[n]), lstFree); }
      else
      {
	if((i=FindOption(argv[n]))<0)
	{
	  AddErr(BText(argv[n])+": "+I2("Invalid option","Opcion invalida"));
	}
	else // A valid option is found
	{
	  PutAppear(i,BTRUE);
	  if(Parametric(i)) // This option need a parameter
	  {
	    if((n+1)<argc) // If there is more arguments
	    {
	      n++;
	      if(!IsOption(argv[n])) { PutParameter(i,argv[n]); }
	      else // The parametric option doesn't have paremeter
	      {
		AddErr(BText(argv[n-1])+": "+
		       I2("Parametric option without paremeter",
			  "Opcion parametrica sin parametro"));
		n--; // Recover the error
	      }
	    }
	    else
	    {
	      AddErr(BText(argv[n])+": "+
		     I2("Parametric option without paremeter",
			"Opcion parametrica sin parametro"));
	    }
	  }
	}
      }
      n++;
    }
    AssingDefaultsAndCheck();
    FreeProcessAndCheck(lstFree);
  }
  return(! ErrorArg());
}


//--------------------------------------------------------------------
BInt BOpt::FindOption(const BText& name)

/*! Returns the number (handler) of an option in the BOpt object.
 *  Returns -1 if the options doesn't exit.
 */
//--------------------------------------------------------------------
{
  BBool foundOption=BFALSE;
  BInt	c=0;

  while(c<NumOptions() && !foundOption)
  {
    if(Option(c)==name) { foundOption=BTRUE; }
    else		{ c++; }
  }

  if(foundOption) { return(c);	}
  else		  { return(-1); }
}


//--------------------------------------------------------------------
BBool BOpt::AssingDefaultsAndCheck()

/*! Assing defaults for options that not appear and check mandatory
 *  and numeric options. The numeric options check is made accesing
 *  to its value.
 */
//--------------------------------------------------------------------
{
  for(BInt i=0; i<NumOptions(); i++)
  {
    if(!Appear(i))
    {
      PutParameter(i,Default(i));
      if(Mandatory(i))
      {
	AddErr(Option(i)+": "+I2("Doesn't appear a mandatory option",
				 "Opcion obligatoria no encontrada"));
      }
    }
    if(Numeric(i)) { BReal r=RealValue(i); }
  }
  return(! ErrorArg());
}


//--------------------------------------------------------------------
BBool BOpt::FreeProcessAndCheck(BList* lstFree)

/*! Process and check the free arguments (FreeLet()). The list must
 *  be destroyed by its creator. The free arguments must be stored
 *  in the same order that are declared by the user (thus in reverse
 *  order of the list.
 */
//--------------------------------------------------------------------
{
  if(lstFree)
  {
    BInt   len=LstLength(lstFree);
    freeArg_.AllocBuffer(len);
    BObject* aux;
    BInt   f=len-1;

    ForAll(BObject, aux, lstFree) { freeArg_[f]=*aux; f--; }
    EndFor

    if(!FreeLet())
    {
      AddErr(BText(I2("Free arguments not permited: ",
		      "Paramatros libres no permitidos: "))+
	     LstText(lstFree,"[","]",", "));
    }
  }
  return(! ErrorArg());
}


//--------------------------------------------------------------------
BReal BOpt::RealValue(BInt n)

/*! Returns a numeric real from parameter. The numbers are received
 *  from the command lines as texts (BChar* argv[]). This method tries
 *  to get a real numeric from the text, if fails tries to get a text
 *  from the default value, if fails returns the minimum from the
 *  option range.
 */
//--------------------------------------------------------------------
{
  BReal	dat = 0;
  BReal realValue;
  BBool badValue=BFALSE;
  
  char * stop;

  dat = strtod(Parameter(n).String(), &stop);
  if (!*stop)
  {
    if(dat<Minimum(n) || dat>Maximum(n))
    {
      badValue=BTRUE;
      AddErr(Option(n)+": "+I2("Out of range","Fuera de rango")+Range(n));
    } else
		realValue = dat;
  }
  else
  {
    badValue=BTRUE;
    AddErr(Option(n)+": "+I2("Format error","Formato erroneo")+Range(n));
  }

  if(badValue)
  {
	  dat = strtod(Default(n).String(), &stop);
	  if (!*stop)
	  {
		  realValue=dat;
          if(realValue<Minimum(n) || realValue>Maximum(n))
		  {
	          badValue=BTRUE;
	          AddErr(Option(n)+": "+I2("Out of range","Fuera de rango")+Range(n));
	          AddErr(Option(n)+": "+I2("Internal error","Error interno"));
		  }
	  }
      else
    {
      badValue=BTRUE;
      AddErr(Option(n)+": "+I2("Format error","Formato erroneo")+Range(n));
      AddErr(Option(n)+": "+I2("Internal error","Error interno"));
    }
  }

  return(realValue);
}


//--------------------------------------------------------------------
BBool BOpt::AddErr(const BText& message)

/*! Add an error message to errorMsg_, preceding with the margin_ and
 *  terminated in \n. Allways set the errorArg_ flag to BTRUE and
 *  returns errorArg_.
 */
//--------------------------------------------------------------------
{
  errorArg_ =  BTRUE;
  errorMsg_ += margin_ + message + "\n";
  return(ErrorArg());
}

