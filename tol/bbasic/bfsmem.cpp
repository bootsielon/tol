/* bfsmem.cpp: BGrammar (gra) class and functions header
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bcommon.h>

#if(__USE_POOL__==__POOL_BFSMEM__)

#include <tol/tol_bfsmem.h>
#include <tol/tol_bout.h>
#include <tol/tol_init.h>

BTraceInit("bfsmem.cpp");

#define BFSMEM_PAGESIZE_MAX  65535
#define BFSMEM_def_grow_       4.0
#define BFSMEM_def_initPgSz_    64
#define BFSMEM_def_initPgNm_     4

#define Page(p) (*(page__[p])) 

#ifndef NDEBUG
  #define BFSMEM_DEBUG_PAGE
  #define BFSMEM_DEBUG_ASSIGNED
//#define BFSMEM_TRACE
#endif

typedef unsigned char  BYTE;
typedef unsigned short BYTE2;

static BYTE  pow2_[8] = { 128, 64, 32, 16, 8, 4, 2, 1};
static BYTE _pow2_[8] = {~128,~64,~32,~16,~8,~4,~2,~1};
static unsigned short byte_pos_;
static BYTE           bit_pos_;

//----------------------------------------------------------------------------
  bool GetBit(const BYTE* v, int k)
//----------------------------------------------------------------------------
{
  byte_pos_ = k/8;
  bit_pos_  = k%8;
  return((v[byte_pos_] & pow2_[bit_pos_])!=0);
}

//----------------------------------------------------------------------------
   void PutBitTrue(BYTE* v, unsigned short k)
//----------------------------------------------------------------------------
{
  byte_pos_ = k/8;
  bit_pos_  = k%8;
  v[byte_pos_] |= pow2_[bit_pos_];
};

//----------------------------------------------------------------------------
   void PutBitFalse(BYTE* v, unsigned short k)
//----------------------------------------------------------------------------
{
  byte_pos_ = k/8;
  bit_pos_  = k%8;
  v[byte_pos_] &= _pow2_[bit_pos_];
};

//--------------------------------------------------------------------
  class BSTLMemoryHandler: public BFixedSizeMemoryBase
//--------------------------------------------------------------------
{
 public:
  BSTLMemoryHandler() 
  : BFixedSizeMemoryBase() {}
 ~BSTLMemoryHandler() {}
  void* New(size_t size, unsigned short& pageNum)
  {
    pageNum = 65535;
    return(malloc(size));
  };
  void Delete(void* obj_, unsigned short pageNum)
  {
    if(TOLHasBeenEnded()) { return; }
    free(obj_);
  }
  short IsAssigned(const void* obj_)
  {
    return((obj_==NULL)?false:-1);
  }
  short IsAssigned(const void* obj_, unsigned short pageNum)
  {
    return((obj_==NULL)?false:-1);
  }
  static BFixedSizeMemoryBase* Handler()
  {
    static BSTLMemoryHandler* handler_ = new BSTLMemoryHandler;
    return(handler_);
  }
};

//--------------------------------------------------------------------
class BFixedSizeMemoryHandler: public BFixedSizeMemoryBase

/*! Implements a memory handler for elementary but very fast and cheap
 *  assignation and deletion of individual objects of class Any.
 *  Vectorial operations new[] and delete[] are not allowed.
 *
 *  Each new needed page will have size equal to previous page 
 *  multiplied by _growingFactor_ where _growingFactor_ must be a real 
 *  number in [1,10].
 *
 *  The more _growingFactor_ uses the more memory but is the more 
 *  faster. Programmer must to select its strategy setting value of
 *  _growingFactor_ For lower sizes of Any and expected massive 
 *  assignement its a good idea to use _growingFactor_ = 2, 
 *  in order to maintain a low number of pages and get a faster page 
 *  search used in New
 *
 *  Parameter _initPageSize_ specifies the size of first page. It's
 *  prefered select it in order to _initPageSize_*sizeof(Any) were a
 *  power of 2. It must be a medium range number between 2^8 and 2^16
 *
 *  _initPageNum_ it's not an important parameter and the best choice 
 *  is to leave it with default parameter
 *
 */
//--------------------------------------------------------------------
{
private: 

  //------------------------------------------------------------------
  class BPage
  /*! A page is the minimum block of elements taht is required to the 
   *  system. Allows recycling all previously deleted items avoiding
   *  internal fragmentation.
   */
  //------------------------------------------------------------------
  {
  public:
    BYTE2  baseSize_;
    BYTE*  pool_;         //!< Available addresess
    BYTE*  poolLimit_;    //!< Limit of available addresess
    BYTE2* avail_;        //!< All current available items
    BYTE*  isAssigned_;   //!< All current available items

    BYTE2  pageSize_;   //!< Number of allocated objects
    BYTE2  available_;  //!< Number of current available items
    //!BPage constructor: At building time cache_ must be full available 
    BPage(BYTE2 baseSize, BYTE2 pageSize);
    //!BPage destructor release dynamical memory
   ~BPage();
    //!Returns an available address
    void* New();
    //!Makes available an address
    void Delete(BYTE* obj);
    int Index(BYTE* address);
    void* Address(int p);
    bool Includes(BYTE* address);
    bool IsAssigned(const BYTE* address);
  };

public:
  //Data members
  float _growingFactor_;
  int   _initPageSize_;
  int   _initPageNum_;
  BYTE2  baseSize_;
  BPage** page__;        //!< Pages store
  BYTE2 lastAllocPg_;    //!< Number of currently allocable pages
  BYTE2 lastPage_;       //!< Number of currently assigned pages
  int initPageNum_;      //!< Initial number of pages 
  int initPageSize_;     //!< Initial size of first page
  int pgNm_;             //!< Current used page number
  int lastDelPgNm_;      //!< page number where last deleting was made
  int maxAllowed_;       //!< Current maximum number of assignable objects along all pages
  int assigned_;         //!< Current assigned objects along all pages 
  int threshold_;        //!< Thresold of assigned objects before to create a new page
  int percentThreshold_; //!< =100*Thresold/pageSize_ must be between 50% and 95%
  float growingFactor_;  //!< Ensures that _growingFactor_ belongs to [1,10]

public:
  /*! BFixedSizeMemoryHandler constructor: Ensures that all parameters 
   *  are into its respective intervals and builds all data members to 
   *  store the first empty page
   */ 
  BFixedSizeMemoryHandler(BYTE2 baseSize, BYTE percentThreshold = 90);
  virtual ~BFixedSizeMemoryHandler();

  /*  Returns an available address from a non full page creting a new page 
   *  if threshold_ is exceeded. If current page is full use it, else search
   *  for the first non full page in reverse order because older objects
   *  are deleted with the less probability.
   */
  void* New(size_t size, BYTE2& pageNum);
 
  /*! Deletes a previously assigned object. If object doesn't belong to current 
   *  page then finds the owner page by binary search.
   */ 
  void Delete(void* obj_, BYTE2 pageNum);
  short IsAssigned(const void* obj_, BYTE2 pageNum);
  int Search(const void* obj);
  short IsAssigned(const void* obj);

  void ShowStats();
  static BFixedSizeMemoryHandler* Instance(int baseSize);
};


//--------------------------------------------------------------------
BFixedSizeMemoryBase** BFSMEM_BuildInstances()
//--------------------------------------------------------------------
{
  static BFixedSizeMemoryBase** instances_ = NULL;
  if(!instances_)
  {
    instances_ = new BFixedSizeMemoryBase*[BFSMEM_MAX_BASE_SIZE];
    for(int i=0; i<BFSMEM_MAX_BASE_SIZE; i++)
    {
      instances_[i] = new BFixedSizeMemoryHandler(i+1);
    }
  }
  return(instances_);
}

//--------------------------------------------------------------------
void BFSMEM_FreeInstances()
//--------------------------------------------------------------------
{
  BFixedSizeMemoryBase** instances_ = BFSMEM_BuildInstances();
  for(int i=0; i<BFSMEM_MAX_BASE_SIZE; i++)
  {
    delete instances_[i];
  }
  delete [] instances_;
}


//--------------------------------------------------------------------
BFixedSizeMemoryBase* BFixedSizeMemoryBase::Instance(int baseSize)
//--------------------------------------------------------------------
{
  static BFixedSizeMemoryBase** instances_ = BFSMEM_BuildInstances();
  assert(baseSize>=1);
  if(baseSize>BFSMEM_MAX_BASE_SIZE)
  {
    return(BSTLMemoryHandler::Handler());
  }
  else
  {
    return(instances_[baseSize-1]);
  }
}

//--------------------------------------------------------------------
BFixedSizeMemoryHandler::BPage::BPage(BYTE2 baseSize, 
                                      BYTE2 pageSize)
//--------------------------------------------------------------------
: baseSize_ (baseSize),
  pageSize_ (pageSize), 
  available_(pageSize)
{
# ifdef BFSMEM_DEBUG_PAGE
  assert(pageSize_<=BFSMEM_PAGESIZE_MAX);
# endif
  pool_ = new BYTE[pageSize_*baseSize_]; 
  memset(pool_,0,pageSize_*baseSize_);
  avail_ = new BYTE2[pageSize_];
# ifdef BFSMEM_DEBUG_PAGE
  assert(pool_);
  assert(avail_);
# endif
  isAssigned_ = new BYTE[1+pageSize_/8];
# ifdef BFSMEM_DEBUG_ASSIGNED
  assert(isAssigned_);
# endif
  memset(isAssigned_,0,1+pageSize_/8);
  poolLimit_ = pool_+pageSize_*baseSize_;
  for(available_=0; available_<pageSize_; available_++)
  {
    avail_[available_]=available_;
  } 
# ifdef BFSMEM_TRACE
  printf("\nBPage(%u,%u)",
         baseSize_, 
         pageSize);
# endif
}

//--------------------------------------------------------------------
BFixedSizeMemoryHandler::BPage::~BPage()
//!BPage destructor release dynamical memory
//--------------------------------------------------------------------
{
/*
# ifdef BFSMEM_DEBUG_PAGE
  assert(pool_);
  assert(avail_);
# endif
  delete[]pool_;
  delete[]avail_;
# ifdef BFSMEM_DEBUG_ASSIGNED
  assert(isAssigned_);
# endif
# ifndef NDEBUG
  delete[]isAssigned_;  //!< All current available items
*/
}

//--------------------------------------------------------------------
void* BFixedSizeMemoryHandler::BPage::New() 
//!Returns an available address
//--------------------------------------------------------------------
{
  BYTE2 p = avail_[--available_];
# ifdef BFSMEM_DEBUG_PAGE
  assert(available_<pageSize_);
  assert(p>=0);
  assert(p<pageSize_);
# endif
# ifdef BFSMEM_DEBUG_ASSIGNED
  assert(!GetBit(isAssigned_,p));
# endif
  PutBitTrue(isAssigned_,p);
# ifdef BFSMEM_TRACE
  printf("\nBPage(%u,%u)::New()->%ld",
         baseSize_, 
         pageSize_, 
         avail_[available_]);
# endif
  return(pool_+(p*baseSize_));
}

//--------------------------------------------------------------------
void BFixedSizeMemoryHandler::BPage::Delete(BYTE* address)
//!Makes available an address
//--------------------------------------------------------------------
{
  BYTE2 p = (address-pool_)/baseSize_;
# ifdef BFSMEM_DEBUG_PAGE
  assert(available_<pageSize_);
  assert(p>=0);
  assert(p<pageSize_);
# endif
# ifdef BFSMEM_DEBUG_ASSIGNED
  assert(GetBit(isAssigned_,p));
# endif
  PutBitFalse(isAssigned_,p);
# ifdef BFSMEM_TRACE
  printf("\nBPage(%u,%u)::Delete(%ld)",
         baseSize_, 
         pageSize_, 
         p); div
# endif
  avail_[available_++] = p; 
};

//--------------------------------------------------------------------
int BFixedSizeMemoryHandler::BPage::Index(BYTE* address)
//--------------------------------------------------------------------
{
  if(address< pool_)      { return(-1); }
  if(address>=poolLimit_) { return(-2); }
  return((address-pool_)/baseSize_);
}

//--------------------------------------------------------------------
void* BFixedSizeMemoryHandler::BPage::Address(int p)
//--------------------------------------------------------------------
{
  return(pool_+p*baseSize_);
}

//--------------------------------------------------------------------
bool BFixedSizeMemoryHandler::BPage::Includes(BYTE* address)
//--------------------------------------------------------------------
{
  return((address>=pool_)&&(address<poolLimit_));
};

//--------------------------------------------------------------------
bool BFixedSizeMemoryHandler::BPage::IsAssigned(const BYTE* address)
//--------------------------------------------------------------------
{
  if((address<pool_)||(address>=poolLimit_)) { return(false); }
  int dif = int(address-pool_);
  int pos = dif / baseSize_;
  if(pos*baseSize_!=dif) { return(false); }
  return(GetBit(isAssigned_,pos));
}

//--------------------------------------------------------------------
BFixedSizeMemoryHandler::BFixedSizeMemoryHandler
(
  BYTE2 baseSize, 
  BYTE  percentThreshold
)
//--------------------------------------------------------------------
: baseSize_           (baseSize),
  percentThreshold_   (percentThreshold),
  page__              (NULL),
  lastPage_           (0), 
  lastAllocPg_        (0), 
  pgNm_               (0),
  lastDelPgNm_        (0),
  assigned_           (0)
{
  assert(baseSize<=BFSMEM_MAX_BASE_SIZE);
  _growingFactor_   = BFSMEM_def_grow_;
  _initPageSize_    = BFSMEM_def_initPgSz_/baseSize_;
  _initPageNum_     = BFSMEM_def_initPgNm_;
  initPageNum_      = _initPageNum_;
  initPageSize_     = _initPageSize_;
  growingFactor_    = _growingFactor_;
  percentThreshold_ = percentThreshold;
  if(growingFactor_    <   1) { growingFactor_    =   1; }
  if(growingFactor_    >  10) { growingFactor_    =  10; }
  if(percentThreshold_ <  50) { percentThreshold_ =  50; }
  if(percentThreshold_ >  95) { percentThreshold_ =  95; }
  if(initPageNum_      <   1) { initPageNum_      =   1; }
  if(initPageNum_      > 256) { initPageNum_      = 256; }
  if(initPageSize_     <   1) { initPageSize_     =   1; }
  if(initPageSize_     >= BFSMEM_PAGESIZE_MAX) { initPageSize_  = BFSMEM_PAGESIZE_MAX; }
  lastAllocPg_   = initPageNum_;
  page__         = new BPage*[lastAllocPg_];
  page__[0]      = new BPage(baseSize_,initPageSize_);
  lastPage_      = 1;
  pgNm_          = 0;
  threshold_     = (Page(0).pageSize_*percentThreshold_)/100;
  maxAllowed_    = Page(0).pageSize_;
}

//--------------------------------------------------------------------
BFixedSizeMemoryHandler::~BFixedSizeMemoryHandler()
//--------------------------------------------------------------------
{
}


//--------------------------------------------------------------------
void* BFixedSizeMemoryHandler::New(size_t size, BYTE2& pageNum)
/*  Returns an available address from a non full page creting a new page 
 *  if threshold_ is exceeded. If current page is full use it, else search
 *  for the first non full page in reverse order because older objects
 *  are deleted with the less probability.
 */
//--------------------------------------------------------------------
{
  assert(size==baseSize_);
  static void* address=NULL;
  static BPage* pg;
  address = NULL;
  assigned_++;
  pg = page__[pgNm_];
  if(pg->available_) 
  { 
    address = (pg->New());
  }
  else if(Page(lastDelPgNm_).available_) 
  { 
    address = (Page(pgNm_=lastDelPgNm_).New());
  }
  else if(assigned_<threshold_)
  {
    int i;
    int maxAvail = 0;
    int maxAvailPage = -1;
    for(i=lastPage_-1; i>=0; i--)
    { 
      if(Page(i).available_>maxAvail)
      {
        maxAvail = Page(i).available_;
        maxAvailPage = i;
        if(maxAvail>=lastPage_) { break; }
      }
    }
    if(maxAvailPage>=0) 
    { address = (Page(pgNm_=maxAvailPage).New()); }
  }
  if(!address)
  {
    if(lastPage_==lastAllocPg_)
    {
      BPage** old  = page__;
      lastAllocPg_ = lastAllocPg_*2;
      page__       = new BPage*[lastAllocPg_];
      memcpy(page__,old,sizeof(BPage*)*lastPage_);
      delete [] old;
    } 
    int oldPageSize = Page(lastPage_-1).pageSize_;
    int newPageSize = int(oldPageSize*growingFactor_);
    if(newPageSize>=BFSMEM_PAGESIZE_MAX) 
    { 
      newPageSize = BFSMEM_PAGESIZE_MAX; 
    }
    page__[pgNm_=lastPage_] = new BPage(baseSize_,newPageSize);
    lastPage_++;
    threshold_  += (Page(pgNm_).pageSize_*percentThreshold_)/100;
    maxAllowed_ += Page(pgNm_).pageSize_;
    address = (Page(pgNm_).New());
  }
# ifdef BFSMEM_DEBUG_PAGE
  assert((pgNm_>=0)&&(pgNm_<lastPage_));
# endif
  pageNum = pgNm_;
  assert(address);
  return(address);
}  

//--------------------------------------------------------------------
void BFixedSizeMemoryHandler::Delete(void* obj,BYTE2 pageNum)
/*! Deletes a previously assigned object. If object doesn't belong to current 
 *  page then finds the owner page by binary search.
 */ 
//--------------------------------------------------------------------
{
  if(TOLHasBeenEnded()) { return; }
  assert(obj);
# ifdef BFSMEM_DEBUG_PAGE
  assert((pgNm_>=0)&&(pgNm_<lastPage_));
# endif
  assigned_--;
# ifdef BFSMEM_DEBUG_ASSIGNED
  assert(assigned_>=0);
# endif
  Page(lastDelPgNm_=pageNum).Delete((BYTE*)obj);
}

//--------------------------------------------------------------------
int BFixedSizeMemoryHandler::Search(const void* obj)

/*! Search the page number for a given address and returns it
 *  if exists.
 *  If not exists return -1
 */ 
//--------------------------------------------------------------------
{
  BYTE* x = (BYTE*)obj;
  int a = 0;
  int b = lastPage_;
  if((x<page__[a]->pool_)||(x>=page__[b]->poolLimit_)) { return(-1); }
  int c = -1;
  while(a<b)
  {
    if(a==b  ) { c=-1; }
    if(a==b+1) 
    { 
           if(x< page__[a]->poolLimit_) { c=a; }
      else if(x>=page__[b]->pool_     ) { c=b; }
      else                              { return(-1); }
    }
    else
    {
      c = (a+b)/2;
      if(x<page__[c]->poolLimit_) { b=c; }
      else                        { a=c; }
    }
  }
  assert((x>=page__[c]->pool_)||(x<page__[c]->poolLimit_));
  return(c);
}


//--------------------------------------------------------------------
short BFixedSizeMemoryHandler::IsAssigned(const void* obj)
/*! Returns true if the object is still assigned in some page.
 */ 
//--------------------------------------------------------------------
{
  if(obj==NULL) { return(false); }
  return(IsAssigned(obj,Search(obj)));
}

//--------------------------------------------------------------------
short BFixedSizeMemoryHandler::IsAssigned(const void* obj, 
                                         BYTE2 pageNum)
/*! Returns true if the object is still assigned in specified page.
 */ 
//--------------------------------------------------------------------
{
  if((pageNum<0)||(pageNum>=lastPage_)) { return(false); }
  return(Page(pageNum).IsAssigned((const BYTE*)obj));
}

//--------------------------------------------------------------------
void BFSMEM_ShowStatsOfAllInstances()
//--------------------------------------------------------------------
{
  BFixedSizeMemoryBase** instances_ = BFSMEM_BuildInstances();
  int i;
  int usedObjects = 0;
  int totalSize = 0;
  int usedSize  = 0;
  BFixedSizeMemoryHandler* h;
  for(i=0; i<BFSMEM_MAX_BASE_SIZE; i++)
  {
    h = ((BFixedSizeMemoryHandler*)instances_[i]);
    usedObjects += h->assigned_;
    totalSize += h->maxAllowed_*h->baseSize_;
    usedSize += h->assigned_*h->baseSize_;
  }
  printf("\nStatistics of use of Fixed Size Memory Handlers:");
  printf("\n  Allocated Objects = %d",         usedObjects);
  printf("\n  Size Avgerage     = %.3lf bytes", double(usedSize*1.0/usedObjects));
  printf("\n  Reserved Size     = %d bytes",   totalSize);
  printf("\n  Used Size         = %d bytes",   usedSize);
  printf("\n  Used Percent      = %.2lf%%",     double(usedSize*100.0/totalSize));
  printf("\n\n"
         "GrowFactor  "
         "InitPgSize  "
         " InitPgNum  "
         "  BaseSize  "
         "TotalItems  "
         " TotalSize  "
         " %%UsedSize  ");
  for(i=0; i<BFSMEM_MAX_BASE_SIZE; i++)
  {
    h = ((BFixedSizeMemoryHandler*)instances_[i]);
    if(h->assigned_) { h->ShowStats(); }
  }
}

//--------------------------------------------------------------------
void BFixedSizeMemoryHandler::ShowStats()
//--------------------------------------------------------------------
{
  if(lastPage_)
  {
    printf("\n"
           "% 10.4f |"
           "% 10d |"
           "% 10d |"
           "% 10u |"
           "% 10d |"
           "% 10ld |"
           "% 10.2lf%% |",
             _growingFactor_,
             _initPageSize_,
             _initPageNum_,
             baseSize_, 
             maxAllowed_,
             maxAllowed_*baseSize_,
             double(assigned_*100.0)/double(maxAllowed_));
/*
    for(i=0; i<lastPage_; i++)
    { 
      printf("\n  Page[%ld] avail %ld of %ld",
             i,
             Page(i).available_,
             Page(i).pageSize_);
    }
*/
  }
}

#endif
