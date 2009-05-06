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

//--------------------------------------------------------------------
   class TOL_API BStackManager

// PURPOSE: Stack manager class
// Stores TOL local objects in a super-fast searcheable way with very
// low cost of inserting and deleting
// Objects are stored in a classical stack and also a dicionary tree.
//--------------------------------------------------------------------
{
public:

  //! Error manager function
  static void SendError(const BText& msg)
  {
    Error(BText("[Stack Manager FATAL]\n")+msg);
  }

  // Internal definitions
  static short numAllowedChar_;
  static int   branchPoolSize_;
  static short branchPoolLen_;

public:
  // Internal classes

  class BDictionaryEntryNode
  /* Dictionary entry node.
   * References to an object and the next entry node in reversed creating order
   */
  {
  public:
    BSyntaxObject*        obj_;  //!<Object reference
    BDictionaryEntryNode* next_; //!<Next entry node

    BDictionaryEntryNode(BSyntaxObject* obj=NULL, BDictionaryEntryNode* next=NULL)
    : obj_(obj), next_(next)
    {}
    DeclareClassNewDelete(BDictionaryEntryNode);
  };
  class BDictionaryEntry
  /* Dictionary entry stack.
   * Each dictionary node has a stack of entries corresponding to objects with
   * the same name.
   */
  {
  public:
    BDictionaryEntryNode* heap_;
    BDictionaryEntry() : heap_(NULL) {}

    //! Enters a new dictionary entry node at stack top level
    void Push(BSyntaxObject* obj)
    {
      heap_ = new BDictionaryEntryNode(obj, heap_);
    }
    //! Releases the dictionary entry node at stack top level
    void Pop()
    {
      if(!heap_)
      { SendError("Heap is already empty. Cannot delete any object."); }
      BDictionaryEntryNode* next = heap_->next_;
      delete heap_;
      heap_ = next;
    }
  };
  class BDictionaryNode
  /* Dictionary node.
   * Each dictionary node has a stack of entries corresponding to objects with
   * the same name in reversed creating order.
   * The name referred by a node is the string corresponding to the path from
   * the root node to this node.
   * Each node has an array of branches, one for each posible
   * symbol.
   * Non matched branches are pointing to NULL.
   * When all entries of a node are deletd the node survives even all its
   * branches arfe empty because it's very probably the path will be matched
   * again in future and used memory size is not significant.
   */
  {
  private:
    //!<Stores the branch position correspondent to each valid character
    static short charPos_ [256];
    //!<Stores all valid chacracters in branch order
    static char  allowed_ [256];
    //!<Cache for fast memory assignation
    static BArray<BDictionaryNode*> branchPool_;
    //!<Current position in assignation pool
    static int currentBranch_;
  public:
    BDictionaryEntry entry_;
    BDictionaryNode* branch_;

    //!<Nodes are empty at building time
    BDictionaryNode()
    : entry_(),
      branch_ (NULL)
      
    {}
    //! Destructor is unreachable by design
   ~BDictionaryNode()
    {
      SendError("Cannot delete a dictionary node.");
    }

    //! Initializes the dictionary static members
    static bool Initialize();
    //! Creates the array of branches
    void CreateBranches();
    //! Finds recursively the dictionary node correspondent of a name begining
    //! from n-th character
    BDictionaryNode* Find(const BText& name, short n);
  };
  class BStackNode
  /* Stack node.
   * Stores references to all objects in reversed creating order pointing also
   * the dictionary node where the object has its correspondent entry in order
   * to delete it from the dictionary without any searching cost.
   * When an object is added increments its number of references.
   * When an object is deleetd decrements its number of references.
   */
  {
  public:
    BSyntaxObject*   obj_;   //!< Referenced object
    BStackNode*      next_;  //!< Next stack node
    BDictionaryNode* dNode_; //!< Referenced dictionary entry node

    BStackNode(BSyntaxObject*   obj   = NULL,
               BStackNode*      next  = NULL,
               BDictionaryNode* dNode = NULL)
    : obj_(obj), next_(next), dNode_(dNode)
    {}
    DeclareClassNewDelete(BStackNode);
  };

private:
  //! Heap of the stack of all available objects in reversed creating order
  static BStackNode*      stack_;
  //! Dictionary root
  static BDictionaryNode* root_;
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
