/* BAtom: BAtom basic class, attributes and methods.
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

#include <tol/tol_batom.h>
#include <tol/tol_bout.h>

BTraceInit("atm.cpp");


#ifdef _DEBUG
//#define TRACE_MEMORY_ATM
#endif

#ifdef TRACE_MEMORY_ATM
#  if (defined(UNIX) || (defined(_MSC_VER) && (_MSC_VER>=1300)))
#    include <typeinfo>
#  else
#    include <typeinfo.h>
#  endif
#endif

#ifdef TRACE_MEMORY_ATM
#include <tol/tol_init.h>
  ostream& Tracer()
  {
    static ofstream tracer_("TOL_ATM_TRACES.txt");
    static int num_trace_ = 0;
    if(!num_trace_)
    {
      tracer_ << "num_trz\tmsg_trz\taction\ttypeid\taddress\tname\trefs\tsystem\tdead";
    }
    return(tracer_ << "\n" << (++num_trace_) << "\t");
  }
#endif

/*! Constructor. It initializes \a nRefs_ to zero
*/
BAtom::BAtom() : BCore(), nRefs_(0)
{
  #ifdef TRACE_MEMORY_ATM
  if(TOLHasBeenInitialized())
  {
    char s[64];
    sprintf(s,"%08lx",int(this));
    const char* nm = "";
    Tracer()<<"ATM_TRZ\tCreated atom\t"<<typeid(*this).name()<<"\tx"<<s
            <<"\t"<<nm<<"\t"<<0<<"\t?\t?";
  }
  #endif
}

/*! Destructor.
*/
BAtom::~BAtom()
{
}


//--------------------------------------------------------------------
BBool BAtom::Destroy()

/*! Destroys it self if it has no reference.
 */
//--------------------------------------------------------------------
{
  if(nRefs_<0)
  {
    Error(I2(" FATAL : Object with negative references : ",
             " FATAL : Objecto con referencias negativas : ")+ Name());
  }
  if(nRefs_ == 0)
  {
    #ifdef TRACE_MEMORY_ATM
  //if(TOLHasBeenInitialized())
    {
      char s[64];
      sprintf(s,"%08lx",int(this));
      BText name = Name();
      name.Replace("\"","'");
      name = BText("\"")+name+"\"";
      const char* nm = name.String();
      if(0 && !System() && !IsDeadObj() && Name().HasName())
      {
        Tracer()<<"ATM_TRZ\tDestroyed atom\t"<<typeid(*this).name()<<"\tx"<<s
            <<"\t"<<nm<<"\t"<<nRefs_<<"\t"<<System()<<"\t"<<IsDeadObj();
      }
    }
    #endif
	  delete this;
	  return(BTRUE);
  }
  else
  {
    #ifdef TRACE_MEMORY_ATM
  //if(TOLHasBeenInitialized())
    {
      char s[64];
      sprintf(s,"%08lx",int(this));
      BText name = Name();
      name.Replace("\"","'");
      name = BText("\"")+name+"\"";
      const char* nm = name.String();
      if(0 && !System() && !IsDeadObj() && Name().HasName())
      {
        Tracer()<<"ATM_TRZ\tNON destroyed atom\t"<<typeid(*this).name()<<"\tx"<<s
            <<"\t"<<nm<<"\t"<<nRefs_<<"\t"<<System()<<"\t"<<IsDeadObj();
      }
    }	  
    #endif
    return(BFALSE);
  }
}


//--------------------------------------------------------------------
BText BAtom::Dump() const

/*! Indicate the name of the atom 
 * \return return the name of the atom
 */ 
//--------------------------------------------------------------------
{
    return(Name());
}


//--------------------------------------------------------------------
BBool BAtom::HasName() const

/*! It indicates the name of it self.
 * \return True if Name() has one charecter at least, False isn't
 */
//--------------------------------------------------------------------
{
    return(Name().HasName());
}


//--------------------------------------------------------------------
BBool BAtom::IsEqualThan(const BAtom& atm1, const BAtom& atm2)

/*! Indicates if the two atoms have the same name.
 * \param atm1 first atom to compare
 * \param atm2 second atom to compare
 * \return return True the two atoms have the same name, False isn't
 */  
//--------------------------------------------------------------------
{
    return (atm1.Name()==atm2.Name());
}


//--------------------------------------------------------------------
BBool BAtom::IsMinorThan(const BAtom& atm1, const BAtom& atm2)

/*! Indicates if the name of an atom is minor who the one of another atom. 
 * \param atm1 first atom to compare
 * \param atm2 second atom to compare
 * \return return True if the name of the first atom is less than the
 *         name of the second atom, False isn't 
 */
//--------------------------------------------------------------------
{
    return (atm1.Name() < atm2.Name()); 
}


//--------------------------------------------------------------------
BBool BAtom::IsMayorThan(const BAtom& atm1, const BAtom& atm2)

/*! Indicates if the name of an atom is greater who the one of another atom. 
 * \param atm1 first atom to compare
 * \param atm2 second atom to compare
 * \return return True if the name of the first atom is great then the
 *         name of the second atom.
 */
//--------------------------------------------------------------------
{
    return (atm1.Name() > atm2.Name()); 
}

//--------------------------------------------------------------------
BText& operator << (BText& txt, const BAtom* atm)

/*! Concats its name after a text.
 * \param txt text to concatenate
 * \param atm atom from which we obtain the name
 * \return the text concatenated with the name of the atom
 */
//--------------------------------------------------------------------
{
    return(txt << atm->Name()); 
}


//--------------------------------------------------------------------
BInt AtmCmp(const void* v1, const void* v2)

/*! Compair two atom receiving the double pointers.
 * \param v1 first atom to caompare
 * \param v2 second atom to compare
 * \return returns an integer less than, equal to, or greather then zero,
 *         if the name of \a v1is found, respectively, to be less then, to
 *         match, or be greater then name of \a v2
 */
//--------------------------------------------------------------------
{
    BAtom* atm1 = *((BAtom**)v1);
    BAtom* atm2 = *((BAtom**)v2);
    return(StrCmp(atm1->Name(), atm2->Name()));
}


//--------------------------------------------------------------------
BInt AtmRCmp(const void* v1, const void* v2)

/*! Compair two atom receiving the pointers.
 * \param v1 first atom to compare
 * \param v2 second atom to compare
 * \return returns an integer less than,
 */
//--------------------------------------------------------------------
{
    BAtom* atm1 = (BAtom*)v1;
    BAtom* atm2 = (BAtom*)v2;
    return(StrCmp(atm1->Name(), atm2->Name()));
}
