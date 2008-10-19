/* tol_classify.h: Declaration of classification methods

   Copyright (C) 2007 
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

/*! \brief Defining Declaration of classification methods
//  \brief funcionality has been implemented in classify.cpp
*/

#ifndef TOL_CLASSIFY_H
#define TOL_CLASSIFY_H

#include <vector>
#include <set>

//--------------------------------------------------------------------
class BClassifier
//--------------------------------------------------------------------
{
 public:
  BClassifier(int n);
  virtual int Relation(int i, int j, bool& error) const = 0; 
  void ClassifyByEquivalenceRelation();
  void ClassifyBySymmetricRelation();
  void ClassifyByBinaryRelation();
  void ClassifyByPartialOrder();
  void Classify(const char* relationType);
  int  GetNumberOfElements() const { return(n_); }
  int  GetNumberOfClasses() const;
  int  GetSizeOfClass(int i) const;
  int  GetFirstElementOfClass(int i);
  int  GetNextElementOfClass(int i);
  bool Error() const { return(error_); }

 protected:
  int n_; //Number of elements of the set to be classified
  int numEval_;
  bool error_;
  std::vector< std::set<int> > classes_;
  std::vector< std::set<int>::const_iterator > iterator_;

  void EvalReflexiveRelation(bool symmetric, bool transitive);
  void TransitiveClossure();

};
  

#endif /* TOL_CLASSIFY_H  */
