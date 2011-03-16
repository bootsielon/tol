/* tol_bcore.h: Global variables, constructors, destructors and functions.
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

/*! \class BCore
//  \brief Defining BCore basic class, its attributes and methods prototypes
//  \brief funcionality has been implemented in cor.cpp
*/ 

#ifndef TOL_BCORE_H
#define TOL_BCORE_H 1

#include <tol/tol_bcommon.h>

//--------------------------------------------------------------------
class TOL_API BCore

/*! Minimun constitution element.
 */
//--------------------------------------------------------------------
{
 private:
    static BInt  nCore_;	    //!< Global core counter
 public:
    // Constructor and destructors: inline
    //! Create a new BCore 
    BCore();
    //! Destroy a BCore
    virtual ~BCore();
    // Access and manipulation: inline
    virtual BBool IsListClass()const=0;
    virtual BBool Destroy()=0;
    //! return \a nCore_ attribute 
    static  BInt	NCore();

    //This method will return 
    //  1 if object is still assigned
    //  0 if object was not assigned or has been deleted
    // -1 if there is no information about this object
    //A valid Memory Handler must be used to have assignation 
    //information in inherited classes to have non -1 result.
    //See on tol_bfsmem.h a sample of Memory Handler
    virtual short IsAssigned() const { return(-1); }
#if (__USE_POOL__==__POOL_BFSMEM__)
    virtual BFixedSizeMemoryBase* GetMemHandler() const { return(NULL); }
    virtual unsigned short  GetPageNum   () const { return(0); }
    virtual size_t GetSizeOf() const { return(0); } 
    struct TOL_API MemAssignInfo
    {
      BCore*                address_;
      size_t                size_;
      unsigned short        pageNum_;

      MemAssignInfo(BCore* address);
      void SetAddress(BCore* address);
      short IsAssigned();
      void SafeDestroy();
    };
#else
    struct TOL_API MemAssignInfo
    {
      short IsAssigned() { return(-1); }
      void SafeDestroy() { }
      MemAssignInfo(BCore* adress) { }
    };
#endif
};


//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define DESTROY(A) if(A) { A->Destroy(); A=NIL; }
#define SAFE_DESTROY(A__,B__)   \
if(A__)                         \
{                               \
  if(B__) (B__)->IncNRefs();    \
  A__->Destroy();               \
  if((B__)) (B__)->DecNRefs();  \
  A__=NIL;                      \
}

#endif // TOL_BCORE_H
