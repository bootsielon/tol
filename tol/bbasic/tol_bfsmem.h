/* tol_bfsmem.h: BGrammar (gra) class and functions header
                   of GNU/TOL language.

   Copyright (C) 2003-2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BFSMEM_H
#define TOL_BFSMEM_H 1

#define BFSMEM_MAX_BASE_SIZE  1024

#include <stdlib.h>

//--------------------------------------------------------------------
class BFixedSizeMemoryBase
//--------------------------------------------------------------------
{
public:
  BFixedSizeMemoryBase() {}
  virtual ~BFixedSizeMemoryBase() {}

  virtual void* New(size_t size, unsigned short& pageNum) = 0;
  virtual void Delete(void* obj_, unsigned short pageNum) = 0;
  virtual short IsAssigned(const void* obj_, unsigned short pageNum) = 0;
  virtual short IsAssigned(const void* obj_) = 0;
  static BFixedSizeMemoryBase* Instance(int baseSize);
};

//--------------------------------------------------------------------
  template<int _baseSize_>
  class BFSMSingleton
//--------------------------------------------------------------------
{
public:
  static BFixedSizeMemoryBase* Handler()
  {
    static BFixedSizeMemoryBase* handler_ = 
      BFixedSizeMemoryBase::Instance(_baseSize_);
    return(handler_);
  }
};

#define RedeclareClassNewDelete(ANY_) \
public:	\
  BFixedSizeMemoryBase* GetMemHandler() const \
  { \
    assert(sizeof(ANY_)<BFSMEM_MAX_BASE_SIZE); \
    return(BFSMSingleton<sizeof(ANY_)>::Handler()); \
  } \
  unsigned short GetPageNum() const \
  { \
    return(this->_bfsm_PageNum__); \
  } \
  short IsAssigned() const  \
  { \
    return(BFSMSingleton<sizeof(ANY_)>::Handler()->IsAssigned(this,this->_bfsm_PageNum__)); \
  } \
  static void* operator new(size_t size) \
  { \
    register unsigned short pageNum; \
    register ANY_* obj = (ANY_*)BFSMSingleton<sizeof(ANY_)>::Handler()->New(size, pageNum); \
    obj->_bfsm_PageNum__ = pageNum; \
    return(obj); \
  } \
  static void operator delete(void* ptr) \
  { \
    register ANY_* obj = (ANY_*)ptr; \
    BFSMSingleton<sizeof(ANY_)>::Handler()->Delete(obj,obj->_bfsm_PageNum__); \
  } 

#define DeclareClassNewDelete(ANY_) \
 protected: \
  unsigned short _bfsm_PageNum__; \
  RedeclareClassNewDelete(ANY_);	

#define UndeclareClassNewDelete \
public:	\
  static void* operator new(size_t size) \
  { \
    return(malloc(size)); \
  } \
  static void operator delete(void* ptr) \
  { \
    free(ptr); \
  } 

#endif  //TOL_BFSMEM_H
