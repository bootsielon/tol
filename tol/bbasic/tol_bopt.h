/* tol_bopt.h: Manage the options passed to tol line command interpreter.
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

#ifndef TOL_BOPT_H
#define TOL_BOPT_H

#include <tol/tol_bobject.h>
#include <tol/tol_blist.h>

//--------------------------------------------------------------------
class BArg: public BObject

/*! Manange the main(int argc, char *argv[]) options, tha name of the
 *  each object is its application name (argv[0]). Default option
 *  only are sound for parametric options.
 */
//--------------------------------------------------------------------
{
 public:
    BText description_; //!< Options description
    BBool mandatory_;	//!< BTRUE if the option is mandatory
    BBool appear_;	//!< BTRUE if the option appear in command line
    BBool parametric_;	//!< BTRUE if the option is parametric
    BBool numeric_;	//!< BTRUE if need a numeric parameter
    BText parameter_;	//!< If is parametric the command line parameter
    BText default_;	//!< Default value when parameter doesn't appear
    BReal minimum_;	//!< Minimum value for numeric parameter
    BReal maximum_;	//!< Maximum value for numeric parameter
    
 public:
    // Constructors and destructors:
    BArg():BObject(), description_(), mandatory_(BFALSE), appear_(BFALSE),
	parametric_(BFALSE), numeric_(BFALSE), parameter_(),
	default_(), minimum_(0), maximum_(0) {}
    ~BArg() {}
    
    // Text functions:
    BText Range();
    BText Help(const BText& prefix);
    BText State(const BText& prefix);
};


//--------------------------------------------------------------------
class BOpt: public BObject

/*! Manange the main(int argc, char *argv[]) options, tha name of the
 *  each object is its application name (argv[0]). Default option
 *  only are sound for parametric options.
 */
//--------------------------------------------------------------------
{
private:
  BInt		counter_;   //!< Internal counter
  BArray<BArg>	optArg_;    //!< All options: -r, -s, etc.
  BArray<BObject> freeArg_;   //!< Other non -x parameters
  BBool		freeLet_;   //!< BTRUE if must be several non -x parameters
  BBool		errorArg_;  //!< Error in arguments flag
  BText		errorMsg_;  //!< Text with the errors found
  BText		samples_;   //!< Text with uses samples for user help
  BText		copyright_; //!< Text with uses samples for user help
  BText		margin_;    //!< For indent the texts

public:
  // Constructors and destructors:
  BOpt(BInt  numOptions=0,
       BBool freeLet=BFALSE,
       const BText& samples="",
       const BText& copyright="Bayes Inference S.A. (1996)",
       const BText& margin="  ");
 ~BOpt() { }

  // Access to option arguments: inline
  BText Option	   (BInt n)const { return(optArg_[n].Name()); }
  BText Description(BInt n)const { return(optArg_[n].description_); }
  BBool Mandatory  (BInt n)const { return(optArg_[n].mandatory_); }
  BBool Appear	   (BInt n)const { return(optArg_[n].appear_); }
  BBool Parametric (BInt n)const { return(optArg_[n].parametric_); }
  BBool Numeric	   (BInt n)const { return(optArg_[n].numeric_); }
  BText Parameter  (BInt n)const { return(optArg_[n].parameter_); }
  BReal RealValue  (BInt n);
  BInt	IntValue   (BInt n)	 { return((BInt)RealValue(n));	}
  BText Default	   (BInt n)const { return(optArg_[n].default_); }
  BReal Minimum	   (BInt n)const { return(optArg_[n].minimum_); }
  BReal Maximum	   (BInt n)const { return(optArg_[n].maximum_); }
  BText Range	   (BInt n)const { return(optArg_[n].Range()); }

  // Manipulation of option arguments: inline
  void PutOption     (BInt n, const BText& v) { optArg_[n].PutName(v); }
  void PutDescription(BInt n, const BText& v) { optArg_[n].description_=v; }
  void PutMandatory  (BInt n, BBool v)	      { optArg_[n].mandatory_  =v; }
  void PutAppear     (BInt n, BBool v)	      { optArg_[n].appear_     =v; }
  void PutParametric (BInt n, BBool v)	      { optArg_[n].parametric_ =v; }
  void PutNumeric    (BInt n, BBool v)	      { optArg_[n].numeric_    =v; }
  void PutParameter  (BInt n, const BText& v) { optArg_[n].parameter_  =v; }
  void PutDefault    (BInt n, const BText& v) { optArg_[n].default_    =v; }
  void PutMinimum    (BInt n, BReal v)	      { optArg_[n].minimum_    =v; }
  void PutMaximum    (BInt n, BReal v)	      { optArg_[n].maximum_    =v; }

  BInt		NumOptions()   const { return(optArg_.Size()); }
  BText		FreeArg(BInt n)const { return(freeArg_[n].Name()); }
  BBool		FreeLet()      const { return(freeLet_); }
  BInt		FreeNum()      const { return(freeArg_.Size()); }
  BArray<BObject> FreeArray()    const { return(freeArg_); }
  BBool		ErrorArg()     const { return(errorArg_); }
  BText		ErrorMsg()     const { return(errorMsg_); }
  BText		Samples()      const { return(samples_); }
  BText		Copyright()    const { return(copyright_); }

  // Manipulation:
  BInt PutOption(const BText& option,
                 const BText& description="",
                 const BText& defValue="",
                 BBool parametric=BFALSE,
                 BBool mandatory=BFALSE,
                 BBool numeric=BFALSE,
                 BReal minimum=0,
                 BReal maximum=0);

  // Functions:
  BBool ParseShow(const BText& commandLine); // If not parse show errors
  BBool ParseShow(BInt argc, BChar** argv); //
  BBool ParseArg(const BText& commandLine);
  BBool ParseArg(BInt argc, BChar** argv);  // The same as the main()
  BInt	FindOption(const BText& name);
  BBool AssingDefaultsAndCheck();
  BBool FreeProcessAndCheck(BList* lstFree);
  BBool AddErr(const BText& message);

  // Text functions:
  BText Help();
  BText State();
};


#endif // TOL_BOPT_H
