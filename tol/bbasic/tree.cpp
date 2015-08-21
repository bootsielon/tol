/* tree.cpp: Implements syntactic Tree behavior of GNU/TOL language.

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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_tree.h>
#include <tol/tol_list.h>
#include <tol/tol_bout.h>

//--------------------------------------------------------------------
Tree::Tree ()
//--------------------------------------------------------------------
    : tree_(NIL), mrFree_(NIL) 
{}


//--------------------------------------------------------------------
Tree::~Tree () 
//--------------------------------------------------------------------
{
    tree_=NIL;
    mrFree_=NIL;
}


//--------------------------------------------------------------------
BBool Tree::Destroy() 
//--------------------------------------------------------------------
{
    BBool destroyied = BTRUE;
    if (tree_) {
	List* lstTree = tree_;
	delete this;
	if (lstTree) lstTree->Destroy();
    }
    return destroyied;
}


//--------------------------------------------------------------------
List* Tree::getLeftBranch () 

/*! Return the branch at the left side of the Tree
 */
//--------------------------------------------------------------------
{
    if (tree_ && tree_->cdr() && !(tree_->cdr()->empty()))
	return ((List*) (tree_->cdr())->car());
    else return NIL;
}


//--------------------------------------------------------------------
List* Tree::getRightBranch () 

/*! Return the branch at the right side of the Tree
 */
//--------------------------------------------------------------------
{
    if (tree_ && tree_->cdr() && tree_->cdr()->cdr() &&
	!(tree_->cdr()->cdr()->empty()))
	return ((List*) ((tree_->cdr())->cdr())->car());
    else return NIL;
}


//--------------------------------------------------------------------
List* Tree::getMostRightBranch () 

/*! Advance throw BTree (BList) until the end to returns last branch.
 * \param tre Tree (BList)
 * \return Last element of \a tre
 */	    
//--------------------------------------------------------------------
{
    List* rightBranch = NULL;
    if (!isEmpty()) 
      {
      rightBranch = tree_;
      while (rightBranch->cdr()) 
        {
        rightBranch = rightBranch->cdr();
        }
      }
    return (rightBranch);
}


//--------------------------------------------------------------------
BBool Tree::putMostRight (Tree* aTree) 

/*! If tre is null returns b, else adds b to the most right branch of
 *  tre and returns tre.
 * \param tre Syntactical Tree (Implemented as a BList) destination
 * \param b Tree (as a BList) source. It'll be added to \a tre parameter
 */
//--------------------------------------------------------------------
{
  // unused
  //List* right = NIL;
  if(isEmpty()) 
  {
	  tree_ = aTree->getTree();
	  setMRFree(aTree->getMRFree());
  } 
  else if(!mrFree_) 
  {
	  setMRFree(aTree->getMRFree());
  }
  else
  {
	  mrFree_->setCar(aTree->getTree());
	  mrFree_ = aTree->getMRFree();
  }
  return (BTRUE);
}


//--------------------------------------------------------------------
List* Tree::treBranch (List* l, BInt n) 

/*! Return n-th branch of Tree.
 *  That n-th branch returned has to be a List*.
 *  \a n param is incremented by needs of lstNthCdr List method
 * \param n Depth of branch that will be returned
 * \return List at n-th branch depth
 * \sa BList::Car, BList::LstNthCdr
 */
//--------------------------------------------------------------------
{
  List* lRes;
  if(!(l->empty())) {
    if (n==0) return l;
    else {
      lRes = l->getNthCdr(n+1);
      return lRes ? (List*) lRes->car() : NIL;
    }
  } else {
    return NIL;
  }
}


//--------------------------------------------------------------------
List* Tree::treLeft (List* lst) 

/*! Access to the left branch of the tree.
 * \param lst Syntactic Tree stored in List
 * \return Left branch in the tree
 * \sa BList::Car, BList::Cdr getLeftBranch
 */
//--------------------------------------------------------------------
{
    if (lst && lst->cdr() && lst->cdr()->car())
	 return (List*) lst->cdr()->car();
    else return NIL;
}


//--------------------------------------------------------------------
List* Tree::treRight (List* lst) 

/*! Access to the left branch of the tree.
 * \param lst Syntactic Tree stored in List
 * \return Left branch in the tree
 * \sa BList::Car, BList::Cdr getRightBranch
 */
//--------------------------------------------------------------------
{
    if (lst && lst->cdr() && lst->cdr()->cdr() && lst->cdr()->cdr()->car())
	return (List*) lst->cdr()->cdr()->car();
    else return NIL;
}


//--------------------------------------------------------------------
List* Tree::treNode (List* lst) 

/*! Access to the node as a List.
 * \param lst Syntactic Tree stored in List
 * \return Node as a List
 */
//--------------------------------------------------------------------
{
    if (lst && lst->car())
	return (List*) lst->car();
    else return NIL;
}
