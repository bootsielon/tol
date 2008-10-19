/* tol_bsymboltable.h: Stores and manages the global symbols table of TOL 
       for GNU/TOL Language.

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

#ifndef TOL_BSYMBOLTABLE_H
#define TOL_BSYMBOLTABLE_H 1

#include <tol/tol_bnameblock.h>

//these warning disabling macros don't work from included headers
#ifdef _MSC_VER
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#endif

//--------------------------------------------------------------------
// Fordward declarations
//--------------------------------------------------------------------
class BList;


//--------------------------------------------------------------------
//Antecedents:
//1º) The class of an object is defined by its grammar and its mode
//    that defines a BObjClassify key.
//2º) BObjByClassHash is a hash_map by BObjClassify of symbols
//Both classes are declared in tol_bgrammar.h
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//Global symbols can be queried by name, by class or by class and name
//So, they will be indexed in a crossed structure od hash_map's
//declared here:
//
//1º) A hash_map by BObjClassify of hash_map's by name of symbols
typedef 
  hash_map_by_unikey<BObjClassify,BSyntaxObject*>::dense_ 
  BObjByClassHash;
//
//2º) A hash_map by name of hash_map's by BObjClassify of symbols
typedef 
  hash_map_by_name<BObjByClassHash*>::dense_ 
  BObjByNameClassHash;
//--------------------------------------------------------------------


//--------------------------------------------------------------------
class TOL_API BSymbolTable

/*! Stores and manages the global symbols table of TOL
 */
//--------------------------------------------------------------------
{
 private:
  //A hash_map by BObjClassify of hash_map's by name of symbols
  BObjByClassNameHash symbolsByClassName_;
  //A hash_map by name of hash_map's by BObjClassify of symbols
  BObjByNameClassHash symbolsByNameClass_;

 public:
  BSymbolTable();
 ~BSymbolTable() {}

 //Adds a new symbol
  int Add(BSyntaxObject* obj);

  //Eliminates an stored  symbol
  int Kill  (BSyntaxObject* obj);

  //Returns the symbol with the specified class and name
  BSyntaxObject* Search(const BObjClassify& oc, const BText& name) const;

  //Returns the stored hash_map of all symbols with a specified name
  BObjByClassHash* Search(const BText& name) const;

  //Creates a list of all symbols with a specified class and
  //concats it to the given list
  BList* Select(BList* lst, const BObjClassify& oc) const;

  //Return true if a pair of objects can be simultaneously global
  static bool AreCompatible(const BSyntaxObject* a, 
                            const BSyntaxObject* b);

  //Return true if an of object can be simultaneously global with
  //an object of specified class
  static bool AreCompatible(const BSyntaxObject* a, const BObjClassify&  oc);

  //Returns lexical information about all objects with specified name
  void LexInfo(BText& info, const BText& name) const;

  //Returns lexical information about all objects with specified name and mode
  void LexInfo(BText& info, const BText& name, int mode) const;

  //Shows in standard output a report of current status
  void Dump  () const;
};
 
#endif // TOL_BSYMBOLTABLE_H
