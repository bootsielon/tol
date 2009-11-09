/* tol_hash_map.h: using hash map templates in an standard way.
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

//! \file tol_hash_map.h

#ifndef TOL_HASH_MAP_H
#define TOL_HASH_MAP_H 1

#include <tol/tol_bcommon.h>
#include <limits>

//--------------------------------------------------------------------
// An explicit typedef of const char* is needed to handle with 
// hash_map by name templates
//--------------------------------------------------------------------
typedef const char* BCharP;


//--------------------------------------------------------------------
   inline BCharP name_del_key()
// the standard deletion key for hash_map by name
//--------------------------------------------------------------------
{
  static BCharP aux_ = "";
  return(aux_);
}

//--------------------------------------------------------------------
class TOL_API BUniKey
//--------------------------------------------------------------------
{
protected:
public:
  BUniKey() {}
  virtual ~BUniKey() {}
  virtual size_t UniqueKey() const = 0;
};

//--------------------------------------------------------------------
//google::dense_hash_map and google::sparse_hash_map are sgi-like 
//hash_map template implementations
//--------------------------------------------------------------------
#if (__USE_HASH_MAP__==__HASH_MAP_GOOGLE__)
#include <google/dense_hash_map>
#include <google/sparse_hash_map>
using google::dense_hash_map;
using google::sparse_hash_map;


//--------------------------------------------------------------------
//string equality comparisson structure needed for hash_map by name
//--------------------------------------------------------------------
namespace std {
template<> struct equal_to<BCharP>
{
  bool operator()(BCharP s1, BCharP s2) const
  {
    return (s1 == s2) || (s1 && s2 && (strcmp(s1, s2) == 0));
  }
};
template<> struct equal_to<BIntPair>
{
  bool operator()(BIntPair s1, BIntPair s2) const
  {
    return (s1.r_==s2.r_) && (s1.c_==s2.c_);
  }
};
}

struct hash_str
{
  size_t operator()(const char* s) const 
  {
    unsigned long h=0, k=1; 
    for ( ; *s; ++s)
    {
      h = 5*h + *s + k;
      h = (16807*h)%2147483647;
      k++;
    }
    return((size_t)h);
  }
};

struct hash_BIntPair
{
  size_t operator()(const BIntPair& s) const 
  {
    size_t aux= *((size_t*)&s);
    return(aux);
  }
};

struct hash_double
{
  size_t operator()(const double& s) const 
  {
    size_t aux= *((size_t*)&s);
    return(aux);
  }
};

//--------------------------------------------------------------------
//this template struct allow define generic typedef of hash_map
//by name and int.
//--------------------------------------------------------------------
template<class _K>
struct hash_unikey
{
  size_t operator()(const _K& uk) const 
  { 
    return(uk.UniqueKey());
  }
};
template<class _K>
struct equal_to_unikey
{
  bool operator()(const _K& uk1, const _K& uk2) const
  {
    return(uk1.UniqueKey()==uk2.UniqueKey());
  }
};

template<class _K, class _T>
struct hash_map_by_unikey
{
  //dense hash_map indexed by unique key
  typedef dense_hash_map
  <
    _K,
    _T,
    hash_unikey<_K>,
    equal_to_unikey<_K>
  >dense_;
  //sparse hash_map indexed by unique key
  typedef sparse_hash_map
  <
    _K,
    _T,
    hash_unikey<_K>,
    equal_to_unikey<_K>
  >sparse_;
};

template<class _T>
struct hash_map_by_int
{
  //dense hash_map indexed by int
  typedef dense_hash_map
  <
    int,
    _T
  >dense_;
  //sparse hash_map indexed by int
  typedef sparse_hash_map
  <
    int,
    _T
  >sparse_;
};

template<class _T>
struct hash_map_by_size_t
{
  //dense hash_map indexed by size_t
  typedef dense_hash_map
  <
    size_t,
    _T
  >dense_;
  //sparse hash_map indexed by size_t
  typedef sparse_hash_map
  <
    size_t,
    _T
  >sparse_;
};

template<class _T>
struct hash_map_by_double
{
  //dense hash_map indexed by double
  typedef dense_hash_map
  <
    double,
    _T,
    hash_double,
    std::equal_to<double>
  >dense_;
  //sparse hash_map indexed by double
  typedef sparse_hash_map
  <
    double,
    _T,
    hash_double,
    std::equal_to<double>
  >sparse_;
};

template<class _T>
struct hash_map_by_BIntPair
{
  //dense hash_map indexed by BIntPair
  typedef dense_hash_map
  <
    BIntPair,
    _T,
    hash_BIntPair,
    std::equal_to<BIntPair>
  >dense_;
  //sparse hash_map indexed by BIntPair
  typedef sparse_hash_map
  <
    BIntPair,
    _T,
    hash_BIntPair,
    std::equal_to<BIntPair>
  >sparse_;
};

template<class _T>
struct hash_map_by_name
{
  //dense hash_map indexed by name
  typedef dense_hash_map
  <
    BCharP,
    _T,
    hash_str,
    std::equal_to<BCharP>
  >dense_;
  //sparse hash_map indexed by name
  typedef sparse_hash_map
  <
    BCharP,
    _T,
    hash_str,
    std::equal_to<BCharP>
  >sparse_;
};

//An key to identify empty cells must be defined for each hash_map 
//object before to insert any key.
#define SetEmptyKey(HASH_MAP,KEY_)   HASH_MAP.set_empty_key  (KEY_)

//An key to identify deleted cells must be defined for each hash_map 
//object before to erase any key.
#define SetDeletedKey(HASH_MAP,KEY_) HASH_MAP.set_deleted_key(KEY_)


//--------------------------------------------------------------------
//stdext::hash_map is a Microsoft-like dense hash_map template 
//implementation. No sparse implementation is available.
//--------------------------------------------------------------------
#elif (__USE_HASH_MAP__==__HASH_MAP_MSVC__)
#include <ext/hash_map>
using stdext::hash_map;

template<> struct less<BCharP>
{
  bool operator()(const BCharP & x, const BCharP & y) const 
  {
    if ( strcmp(x, y) < 0) { return(true); }
    return(false);
  }
};


//--------------------------------------------------------------------
//this template struct allows define generic typedef of hash_map
//by name and int.
//sparse types are defined as dense ones to maintain compatibility
//with google API
//--------------------------------------------------------------------

template<class _K, class _T>
struct hash_map_by_unikey
{
  //dense hash_map indexed by unique key
  typedef hash_map
  <
    _K,
    _T,
    stdext::hash_compare <_K, less<_K> >
  >dense_;
  //sparse hash_map indexed by unique key
  typedef hash_map
  <
    _K,
    _T,
    stdext::hash_compare <_K, less<_K> >
  >sparse_;
};
template<class _T>
struct hash_map_by_int
{
  //dense hash_map indexed by int
  typedef hash_map
  <
    int,
    _T
  >dense_;
  //sparse hash_map indexed by int
  typedef hash_map
  <
    int,
    _T
  >sparse_;
};

template<class _T>
struct hash_map_by_size_t
{
  //dense hash_map indexed by size_t
  typedef hash_map
  <
    size_t,
    _T
  >dense_;
  //sparse hash_map indexed by size_t
  typedef hash_map
  <
    size_t,
    _T
  >sparse_;
};

template<class _T>
struct hash_map_by_name
{
  //dense hash_map indexed by name
  typedef hash_map
  <
    BCharP,
    _T,
    stdext::hash_compare <BCharP, less<BCharP> >
  >dense_;
  //sparse hash_map indexed by name
  typedef hash_map
  <
    BCharP,
    _T,
    stdext::hash_compare <BCharP, less<BCharP> >
  >sparse_;
};

//MSVC hash_map doesn't use any key to identify empty or deleted 
//cells, but these macroes are defined to maintain compatibility
#define SetEmptyKey(HASH_CLASS,KEY_)
#define SetDeletedKey(HASH_CLASS,KEY_)

#elif (__USE_HASH_MAP__==__HASH_MAP_GCC__ ) || (__USE_HASH_MAP__==__HASH_MAP_ICC__ )

#include <ext/hash_map>
namespace std {
using __gnu_cxx::hash_map;

//--------------------------------------------------------------------
//string equality comparisson structure needed for hash_map by name
//--------------------------------------------------------------------
template<> struct equal_to<BCharP>
{
  bool operator()(BCharP s1, BCharP s2) const
  {
    return (s1 == s2) || (s1 && s2 && (strcmp(s1, s2) == 0));
  }
};

//--------------------------------------------------------------------
//this template struct allow define generic typedef of hash_map
//by name and int.
//--------------------------------------------------------------------
template<class _K>
struct hash_unikey
{
  size_t operator()(const _K& uk) const 
  { 
    return(uk.UniqueKey());
  }
};
template<class _K>
struct equal_to_unikey
{
  bool operator()(const _K& uk1, const _K& uk2) const
  {
    return(uk1.UniqueKey()==uk2.UniqueKey());
  }
};

//--------------------------------------------------------------------
//this template struct allows define generic typedef of hash_map
//by name and int.
//sparse types are defined as dense ones to maintain compatibility
//with google API
//--------------------------------------------------------------------

template<class _K, class _T>
struct hash_map_by_unikey
{
  //dense hash_map indexed by unique key
  typedef hash_map
  <
    _K,
    _T,
    hash_unikey<_K>,
    equal_to_unikey<_K>
  >dense_;
  //sparse hash_map indexed by unique key
  typedef hash_map
  <
    _K,
    _T,
    hash_unikey<_K>,
    equal_to_unikey<_K>
  >sparse_;
};

template<class _T>
struct hash_map_by_int
{
  //dense hash_map indexed by int
  typedef hash_map
  <
    int,
    _T
  >dense_;
  //sparse hash_map indexed by int
  typedef hash_map
  <
    int,
    _T
  >sparse_;
};

template<class _T>
struct hash_map_by_size_t
{
  //dense hash_map indexed by size_t
  typedef hash_map
  <
    size_t,
    _T
  >dense_;
  //sparse hash_map indexed by size_t
  typedef hash_map
  <
    size_t,
    _T
  >sparse_;
};

template<class _T>
struct hash_map_by_name
{
  //dense hash_map indexed by name
  typedef hash_map
  <
    BCharP,
    _T
  >dense_;
  //sparse hash_map indexed by name
  typedef hash_map
  <
    BCharP,
    _T
  >sparse_;
};

}

// CHECK THIS!!!!!
//MSVC hash_map doesn't use any key to identify empty or deleted 
//cells, but these macroes are defined to maintain compatibility
#define SetEmptyKey(HASH_CLASS,KEY_)
#define SetDeletedKey(HASH_CLASS,KEY_)

#else
#error 'No hash_map implementation configured'
#endif //__USE_HASH_MAP__

#endif // TOL_HASH_MAP_H
