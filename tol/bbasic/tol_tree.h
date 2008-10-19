/* tol_tree.h: Class declaration of Syntactical Tree of GNU/TOL language.

   Copyright (C) 2003 
                 European Free Software Foundation, Inc.

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

/*! \brief Defining BList basic class, its attributes and methods prototypes
//  \brief funcionality has been implemented in lst.cpp, lsttext.cpp and
//  \brief lstalgeb.cpp
*/

#ifndef TOL_TREE_H
#define TOL_TREE_H

#include <tol/tol_list.h>

//--------------------------------------------------------------------
class TOL_API Tree 
//--------------------------------------------------------------------
{
  private:
    List* tree_;        //! List where is stored the real Tree
    List* mrFree_;      //! Pointer to most right free position of Tree

  public:
    // Constructor and Destructor methods
    Tree();
    ~Tree();

    //! Destroys all the elements of a Tree.
    BBool Destroy(); 

  // Classic get and set methods
    //! set attribute BNode* car_ of this Tree (this Tree is really a List)
    void setNode (BCore* node) 
	{
	    tree_->setCar(node);
	}

    //! set attribute List* cdr_ of this Tree (this Tree is really a List)
    void setBranch (List* branch) 
	{
	    tree_->setCdr(branch);
	}
    
    void setTree (List* theTree) 
	{
	    tree_ = theTree;
	}
    
    void setMRFree (List* pos) 
	{
	    mrFree_ = pos;
	}
    
    //! Return the node BCore stored in this branch
    BCore* getNode () 
	{
	    return tree_->car();
	}
    
    //! Return the next branch of the Tree
    List* getBranch () 
	{
	    return (tree_->cdr());
	}
    
    List* getTree () 
	{
	    return tree_;
	}
    
    List* getConstTree () const 
	{
	    return tree_;
	}
    
    List* getMRFree () 
	{
	    return mrFree_;
	}
    
    //! Determines if a BCore object is a Tree (a List)
    static BBool isBranch (BCore* aBCore) 
	{
	    return (aBCore->IsListClass());
	}
    
    //! verify if this Tree is empty
    BBool isEmpty () 
	{
	    if (!tree_) {
		return BTRUE;
	    } else {
		return BFALSE;
	    }
	}


    //! Create a Tree with \a rootElement as node and \a branch as branch
    //--------------------------------------------------
    static Tree* create (BCore* rootElement, List* branch) 

	/*! Create a Tree with \a rootElement as node of the Tree and
	 *  \a branch as branch of the Tree.
	 * \param rootElement is the node
	 * \param branch is the branch
	 * \return pointer to the new created Tree
	 */
	//--------------------------------------------------
	{
	    Tree* tRet = new Tree();
	    tRet->setTree(List::cons(rootElement, branch));
	    return tRet;
	}

    
    //! Create a Tree with one root node and one branch differently than create
    //--------------------------------------------------
    static Tree* createMonary (BCore* rootElement, List* branch) 

	/*! Create a Tree but differently than create() method.
	 *  createMonary method puts \a rootElement as node of Tree, then
	 *  create branch with \a branch as node and NIL as branch.
	 * \param rootElement is the node of the tree
	 * \param branch is the node of the branch of the tree
	 * \return pointer to the new created Tree
	 */
	//--------------------------------------------------
	{   
	    Tree* tRet = new Tree();
	    List* tree = List::cons(rootElement, List::nCons(branch));
	    if (!branch) { // Actualiza mrFree_ a la pos libre + a la derecha
		tRet->setMRFree(tree->cdr());
	    }
	    tRet->setTree(tree);
	    return tRet;
	}


    //! Create a Tree with one root node and two (left and right) branches 
    static Tree* createBinary (BCore* rootElement, 
			       List* leftBranch,
			       List* rightBranch) 
	{
	    Tree* tRet = new Tree();
	    List* tree =List::cons (rootElement,
				    List::cons (leftBranch,
						List::nCons(rightBranch)));
	    if (!rightBranch) {
		tRet->setMRFree((tree->cdr())->cdr());
	    }
	    tRet->setTree(tree);
	    return tRet;
	}

    //! Return last branch of Tree 
    List* getMostRightBranch ();

    //! Access to the left branch of the tree.
    List* getLeftBranch ();

    //! Access to the right branch of the tree.
    List* getRightBranch ();

    //! Insert Tree* parameter at the most right part of the tree
    BBool putMostRight (Tree*);

    //! Access to the n-th branch of the tree
    static List* treBranch (List*, BInt);

    //! Access to the left branch of the tree (List* parameter)
    static List* treLeft(List*);
    //! Access to the right branch of the tree (List* parameter)
    static List* treRight(List*);
    //! Access to the node getting it like a List object
    static List* treNode(List*);

    //! Returns a List with the tail of Tree
    //Tree* treBranches ();

    //! Adds new branch to the tree at specified n-th position
    //Tree* treAddBranch (Tree*, Tree*, BInt);

    //! Adds a list of branches to the tail of the tree.
    //Tree* addBranches (Tree*, Tree*);

    //! Append element to the left branch of the tree.
    //Tree* trePutLeft (Tree*, Tree*);

    //! Append element to the right branch of the tree.
    //Tree* trePutRight (Tree*, Tree*);
  DeclareClassNewDelete(Tree);
};


//--------------------------------------------------------------------
 inline List* Branch(const List* tre, BInt n)
//--------------------------------------------------------------------
{
//    return((BTree*) LstNthCar(Cdr(tre), n));
    if (tre && tre->cdr()) {
//	return ((List*) ((List*)tre)->cdr()->getNthCar(n));
	BCore* aBCore = ((List*)tre)->cdr()->getNthCar(n);
	if(aBCore==NIL) 
	     return NIL;
	else return (List*) aBCore;
    } else return NIL;
}


#endif /* TREE  */
