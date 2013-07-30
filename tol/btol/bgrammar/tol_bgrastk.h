/* tol_bgrastk.h: Local Stack manager for GNU/TOL Language.

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

#ifndef TOL_STACK_MANAGER_H
#define TOL_STACK_MANAGER_H 1

#include <tol/tol_bsyntax.h>

class BStruct;
class BClass;

//--------------------------------------------------------------------
   class TOL_API BStackManager

// PURPOSE: Stack manager class
// Stores TOL local objects in a super-fast searcheable way with very
// low cost of inserting and deleting
// Objects are stored in a classical stack and also a dicionary tree.
//--------------------------------------------------------------------
{
private:
  //! Number of currently stack size
  static int currentEntries_;
public:
  //! Initializes the stack handler
  static bool Initialize();
  //! Enters a new object in both stack and dictionary structures
  static void Push(BSyntaxObject* obj);
  //! Removes the object on the top of stack and its correspondent
  //! dictionary entry node that must be on the top of referenced dictionary
  //! node. If object is not the except one then destroy it
  static void Pop (BSyntaxObject* except);
  //Changes the name of a local object
  static void ChangeName(BSyntaxObject* obj, const BText& newName);
  //! Removes objects from the stack top until stack size is the specified one
  //! All released objects but except one will be deleted
  static void DestroyUntil(int size, BSyntaxObject* except=NULL);
  //! Returns the last created object with specified name if exists or null else
  static BSyntaxObject* Find      (const BText& name);
  //! Returns the last created struct with specified name if exists or null else
  static BStruct*       FindStruct(const BText& name);
  //! Returns the last created user class with specified name if exists or null else
  static BClass*        FindClass(const BText& name);
  //! Returns the current stack size
  static int CurrentEntries() { return(currentEntries_); }

};

#endif // TOL_STACK_MANAGER_H
