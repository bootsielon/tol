/* tol_bcommon.h: Common MACRO descriptions to all sources of 
                  GNU/TOL Language

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

#ifndef TOL_BCOMMON_H
#define TOL_BCOMMON_H 1

#if HAVE_CONFIG_H
#  include <tol/config.h>
#endif

/* Test for GCC > 4.3.0
   
   #if GCC_VERSION > 40300
*/
#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 10000 \
                               + __GNUC_MINOR__ * 100 \
                               + __GNUC_PATCHLEVEL__)
#else
#define GCC_VERSION 0
#endif

#ifndef OPENFLAG 
#  if (defined(UNIX) || (defined(_MSC_VER) && (_MSC_VER>=1300)))
#    define OPENFLAG ios::in
#  else
#    define OPENFLAG ios::nocreate
#  endif
#endif

#ifdef __cplusplus
#  if ((defined(__GNUC__) && (__GNUC__>2)) || (defined(_MSC_VER) && (_MSC_VER>=1300)))
#  include <string>
using namespace std;
#    include <iostream>
#    include <fstream>
#    include <iosfwd>
#  else
#    include <iostream.h>
#    include <ostream.h>
#    include <fstream.h>
#    include <string>
#  endif
#  include <sstream>
#  include <exception>
#endif

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#if HAVE_STRING_H
#  include <string.h>
#endif

#if HAVE_STRINGS_H
#  include <strings.h>
#endif

#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# endif

# if HAVE_TIME_H
#  include <time.h>
# endif

#if defined(__GNUC__)
#define _FILE_OFFSET_BITS 64
#elif defined(WIN32)
#define fseeko _fseeki64
#define ftello _ftelli64
#define off_t  __int64
#else
#define fseeko fseek
#define ftello ftell
typedef long off_t;

#endif


#undef BEGIN_DECLS
#undef END_DECLS
#ifdef __cplusplus
#  define BEGIN_DECLS extern "C" {
#  define END_DECLS   }
#else
#  define BEGIN_DECLS /* empty */
#  define END_DECLS   /* empty */
#endif /* __cplusplus */

#ifdef STATIC_BUILD
#   define DLLIMPORT
#   define DLLEXPORT
#else
#   if (defined(WIN32) && (defined(_MSC_VER) || (__BORLANDC__ >= 0x0550) || (defined(__GNUC__) && defined(__declspec)))) || (defined(MAC_TCL) && FUNCTION_DECLSPEC)
#	define DLLIMPORT __declspec(dllimport)
#	define DLLEXPORT __declspec(dllexport)

#if (defined(_MSC_VER) && (_MSC_VER<1400))
# error "TOL cannot be built with versions of MSVC prior to .NET 2005" 
#endif

#if (defined(_MSC_VER))
//Disable paranoid warning in Visual C++ 7: '<' : no coinciden signed/unsigned
#pragma warning(disable : 4018)
#endif

//no suitable definition provided for explicit template instantiation request
#pragma warning(disable : 4661)
//disable warnings on extern before template instantiation
#pragma warning(disable : 4231)
// identifier was truncated to '255' characters in the browser information
#pragma warning(disable : 4786)
#   else
#	define DLLIMPORT
#	define DLLEXPORT
#   endif
#endif

#ifdef TOL_API
#   undef TOL_API
#endif /* TOL_API */

#ifdef TOLDLL_EXPORTS
#    define TOL_API DLLEXPORT
#else
#    define TOL_API DLLIMPORT
#endif

/* Basic Types, constants, and Macros */

#define BChar char
#define BInt  int
#define BReal double
#define BBool unsigned char
#define BAny  void*

#define MAXBINT	 2147483647
#define MINBINT -2147483647

typedef BInt (*BOrderCriterium)(const BAny, const BAny);
enum BSign { BPOSITIVE=1, BNULL=0, BNEGATIVE = -1 };

#define BASCENDENT  BPOSITIVE
#define BUNORDERED  BNULL
#define BDESCENDENT BNEGATIVE
#define BTRUE	'\1'
#define BFALSE	'\0'

#define BINTMIN -2147483648
#define BINTMAX	 2147483648

#ifndef NIL
#  define NIL 0	  /* Pointer to nothing */
#endif

#ifdef UNIX
#  define BINT64 long long int
#else
#  define BINT64 __int64
#endif


/* MACROS: There are the following traces:
     TRACE_LEVEL, TRACE_MEMORY, TRACE_INIT, TRACE_BPARSER.
*/


/*You can define TRACE_LEVEL to filter some debuging traces and show the 
  rest ones. There are specific macroes for four main levels:
   -LEVEL 1 : TRACE_SHOW_LOW      begin and end of a function
   -LEVEL 2 : TRACE_SHOW_MEDIUM   intermediate lines out of cycles
   -LEVEL 3 : TRACE_SHOW_HIGH     nonnested cycles
   -LEVEL 4 : TRACE_SHOW_TOP      nested cycles
  For higher specific levels you can use directly TRACE_SHOW_LEVEL 
  You can define it at first line of a .cpp to allow it just in that file or 
  alternatively you can define it in your makefile or project, but then all
  files will allow traces and result could be too verbose.
  If TRACE_LEVEL is no defined no trace will be generated at compiling time.
*/
#ifdef  TRACE_LEVEL
#  define TRACE_SHOW_LEVEL(LEVEL,FUN,MSG) \
  if(LEVEL<=TRACE_LEVEL) \
  { Std(BText("TRACE\t")+LEVEL+"\t"+FUN+"\t"+MSG+"\n"); }
#else
#  define TRACE_SHOW_LEVEL(LEVEL,FUN,MSG) 
#endif
#define TRACE_SHOW_LOW(FUN,MSG)     TRACE_SHOW_LEVEL(1,FUN,MSG)
#define TRACE_SHOW_MEDIUM(FUN,MSG)  TRACE_SHOW_LEVEL(2,FUN,MSG)
#define TRACE_SHOW_HIGH(FUN,MSG)    TRACE_SHOW_LEVEL(3,FUN,MSG)
#define TRACE_SHOW_TOP(FUN,MSG)     TRACE_SHOW_LEVEL(4,FUN,MSG)


/*Macroes for ordered initialization of static variables
*/
#ifdef __cplusplus

/* /////////////////////////////////////////////////////////////////////// */
  template <class Derived, class Base>
  class BCast
/* /////////////////////////////////////////////////////////////////////// */
{
public:
  static Derived* c(Base* base)
  {
  #ifndef NDEBUG
    Derived* cst = dynamic_cast<Derived*>(base);
    assert((!cst)==(!base));
    return(cst);
  #else
    return(static_cast<Derived*>(base));
  #endif
  }
};

/* /////////////////////////////////////////////////////////////////////// */
  class BStaticInit
/* /////////////////////////////////////////////////////////////////////// */
{
public:
  BStaticInit() { }
 ~BStaticInit() { }
};

/* /////////////////////////////////////////////////////////////////////// */
inline const BStaticInit& StaticInit()
/* /////////////////////////////////////////////////////////////////////// */
{
  static BStaticInit aux_;
  return(aux_);
};


#ifndef NDEBUG
#define TraceInitilializing(CLASS) \
  printf("\nInitializing class "#CLASS);
#else
#define TraceInitilializing(CLASS) 
#endif

#define DefIsAlreadyInitilialized(CLASS) \
  static bool CLASS##_IsAlreadyInitilialized_() \
{ \
  static bool isAlreadyInitilialized_ = false; \
  if(!isAlreadyInitilialized_) \
  { \
    TraceInitilializing(CLASS) \
    isAlreadyInitilialized_ = true; \
    return(false); \
  } \
  else \
  { \
    return(true); \
  } \
}

#define IsAlreadyInitilialized(CLASS) \
  CLASS##_IsAlreadyInitilialized_() 

bool InitializeOrderedClasses();
# if(defined(TRACE_INIT)&&(TRACE_INIT>1))
#  define BTraceInit(sourceFile) \
    static bool  TraIniOrdCls__ =InitializeOrderedClasses(); \
    static BInt	 TraIniInt__    =(BInt)printf("\n%s: <return>",sourceFile); \
    static BChar TraIniChar__   =(BChar)getchar();
# elif(defined(TRACE_INIT)&&(TRACE_INIT<=1))
#  define BTraceInit(sourceFile) \
    static bool  TraIniOrdCls__ =InitializeOrderedClasses(); \
    static BInt	 TraIniInt__    =(BInt)printf("\n%s: <return>",sourceFile); 
# else
#  define BTraceInit(sourceFile) \
    static bool  TraIniOrdCls__ =InitializeOrderedClasses(); 
# endif
#endif

/* End Basic Types, constants, and Macros */


/* Macro constants definitions from btolconst.h */
#define	TOLFILE  1
#define	PRJFILE  2
#define	BDTFILE  3
#define	BMTFILE  4
#define	BSTFILE  5
#define	BDCFILE  6

/* Macro definition from tol_blanguag.h */
#ifdef UNIX
#  define SYSTEM "UNIX"
#else
#  define SYSTEM "WINDOWS"
#endif

TOL_API const char * GetCompilationDate();
TOL_API const char * GetCompilationTime();

#ifdef HAVE_LIMITS_H
#  include <limits.h>
#endif

#ifdef HAVE_FLOAT_H
#  include <float.h>
#endif

#if (defined(WIN32) && defined(_MSC_VER))
#if !defined(strcasecmp)
#define strcasecmp  _stricmp
#endif
#if !defined(strncasecmp)
#define strncasecmp _strnicmp
#endif
#if !defined(snprintf)
#define snprintf _snprintf
#endif
#endif

#ifndef HAVE_ROUND
#ifdef __cplusplus
  inline double round(double a) {
    double f, i;
    f = modf(a, &i);
    return (a>0) ? (i+(f>0.5)) : (i-(f<-0.5));
  }
#else
  double round(double a);
#endif
  
#endif

struct TOL_API BIntPair { BInt r_; BInt c_; };
  
/* /////////////////////////////////////////////////////////////////////// */
#ifdef DARWIN

long double  fabsl(long double x);
#  ifdef __cplusplus
inline long double fabsl(long double x) { return (long double) fabs((double)(x)); }
#  else
#    define fabsl(x) ((long double) fabs((double)(x)))
#  endif

long double  powl(long double x, long double y);
#  ifdef __cplusplus
inline long double powl(long double x, long double y) { return (long double) pow((double)(x), (double)(y)); }
#  else
#    define powl(x, y) ((long double) pow((double)(x), (double)(y)))
#  endif

long double  sqrtl(long double x);
#  ifdef __cplusplus
inline long double sqrtl(long double x) { return (long double) sqrt((double)(x)); }
#  else
#    define sqrtl(x) ((long double) sqrt((double)(x)))
#  endif

#endif // DARWIN
/* /////////////////////////////////////////////////////////////////////// */

#include <tol/tol_memhandler.h>

#endif /*! TOL_BCOMMON_H */
