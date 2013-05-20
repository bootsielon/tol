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

#include <stdlib.h>

#define BFSMEM_MAX_BASE_SIZE  1024

#define BFSMEM_COUNT_BY_CLASS
#ifdef BFSMEM_COUNT_BY_CLASS

  void bfsmem_add_instances(char* className, int numItems, int size);
  #define BFSMEM_ADD_INSTANCES(className,numItems,size) \
    bfsmem_add_instances(className,numItems,size)

  void bfsmem_remove_instances(char* className, int numItems, int size);
  #define BFSMEM_REMOVE_INSTANCES(className,numItems,size) \
    bfsmem_remove_instances(className,numItems,size)

  int bfsmem_num_classes();
  #define BFSMEM_NUM_CLASSES \
    bfsmem_num_classes()

  int bfsmem_class_instances(char* className);
  #define BFSMEM_CLASS_INSTANCES(className) \
    bfsmem_class_instances(className)

  double bfsmem_class_megabytes(char* className);
  #define BFSMEM_CLASS_MEGABYTES(className) \
    bfsmem_class_megabytes(className)

  void bfsmem_show_table();

#else
  #define BFSMEM_ADD_INSTANCES(className,numItems,size)
  #define BFSMEM_REMOVE_INSTANCES(className,numItems,size)
  #define BFSMEM_NUM_CLASSES
  #define BFSMEM_CLASS_INSTANCES(className)
  #define BFSMEM_CLASS_MEGABYTES(className)
  void bfsmem_show_table() {}
#endif

//--------------------------------------------------------------------
class TOL_API BFixedSizeMemoryBase
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
  static char* BFSMEM_ClassName() { return(#ANY_); } \
  static int BFSMEM_ClassSize() { return(sizeof(ANY_)); } \
  BFixedSizeMemoryBase* GetMemHandler() const \
  { \
    assert(sizeof(ANY_)<BFSMEM_MAX_BASE_SIZE); \
    return(BFSMSingleton<sizeof(ANY_)>::Handler()); \
  } \
  size_t GetSizeOf() const \
  { \
    return(sizeof(ANY_)); \
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
    BFSMEM_ADD_INSTANCES(#ANY_,1,sizeof(ANY_)); \
    register unsigned short pageNum; \
    register ANY_* obj = (ANY_*)BFSMSingleton<sizeof(ANY_)>::Handler()->New(size, pageNum); \
    obj->_bfsm_PageNum__ = pageNum; \
    return(obj); \
  } \
  static void operator delete(void* ptr) \
  { \
    BFSMEM_REMOVE_INSTANCES(#ANY_,1,sizeof(ANY_)); \
    register ANY_* obj = (ANY_*)ptr; \
    BFSMSingleton<sizeof(ANY_)>::Handler()->Delete(obj,obj->_bfsm_PageNum__); \
  } 

#define DeclareClassNewDelete(ANY_) \
 protected: \
  unsigned short _bfsm_PageNum__; \
  RedeclareClassNewDelete(ANY_);	

#define UndeclareClassNewDelete \
public:	\
  short IsAssigned() const  \
  { \
    return(-1); \
  } \
  static void* operator new(size_t size) \
  { \
    BFSMEM_ADD_INSTANCES(BFSMEM_ClassName(),1,size); \
    return(malloc(size)); \
  } \
  static void operator delete(void* ptr) \
  { \
    BFSMEM_REMOVE_INSTANCES(BFSMEM_ClassName(),1,BFSMEM_ClassSize()); \
    free(ptr); \
  } 

#endif  //TOL_BFSMEM_H
