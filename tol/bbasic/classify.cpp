/* classify.cpp: Implements classification methods

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

#include <tol/tol_classify.h>
#include <tol/tol_bout.h>

//#define TRACE_LEVEL 4

#ifndef TRACE_LEVEL
  #define TraceClasses(level,classes_,identify,iter)
#else
//--------------------------------------------------------------------
void TraceClasses
(
  int level,
  const BText& identify,
  int iter,
  std::vector< std::set<int> >& classes_
)
//--------------------------------------------------------------------
{
  int i;
  std::set<int>::iterator h;
  Std(identify+" Classes for iteration "<<iter<<" :\n");
  if(level<=TRACE_LEVEL)
  {
    for(i=0; i<classes_.size(); i++)
    {
      Std(BText("  class[")<<i<<"] = ");
      for(h=classes_[i].begin(); h!=classes_[i].end(); h++)
      {
        Std(BText("")<<(*h)<<", ");
      }
      Std("\n");
    }
  }
}
//--------------------------------------------------------------------
void TraceClasses
(
  int level,
  const BText& identify,
  int iter,
  std::vector< int >& unitary_
)
//--------------------------------------------------------------------
{
  int i;
  std::set<int>::iterator h;
  Std(identify+" Unitary classes for iteration "<<iter<<" :\n_");
  if(level<=TRACE_LEVEL)
  {
    for(i=0; i<unitary_.size(); i++)
    {
      Std(BText("  class[")<<i<<"] = "<<unitary_[i]<<"\n_");
    }
  }
}
#endif

//--------------------------------------------------------------------
BClassifier::BClassifier(int n) 
//--------------------------------------------------------------------
: n_       (n), 
  numEval_ (0),
  error_   (false),
  classes_ (0),
  iterator_(0)
   
{}

//--------------------------------------------------------------------
void BClassifier::EvalReflexiveRelation(bool symmetric, bool transitive)
//--------------------------------------------------------------------
{
  TRACE_SHOW_LOW("BClassifier::EvalReflexiveRelation()", "begin");
  int i, j, m;
  std::vector< std::set<int> > classes(n_);
  std::vector< bool > classified(n_);
  std::set<int>::const_iterator k;
  std::set<int>::iterator       h;
  for(i=0; i<n_; i++)
  {
    classes[i].insert(i);
    classified[i] = false;
  }
  TraceClasses(2,"classes",0,classes);
  for(i=m=0; i<n_; i++)
  {
    if(!classified[i])
    {
      classified[i]=true;
      for(j=i+1; !error_ && (j<n_); j++)
      {
        if(!classified[j])
        {
          TRACE_SHOW_MEDIUM("Checking relation for pair ", 
                            BText(" (")+i+","+j+")");
          numEval_++;
          int match = Relation(i,j,error_);
          if(error_) { return; }
          else if(!symmetric)
          {
            TRACE_SHOW_MEDIUM("Checking relation for fliped pair ", 
                              BText(" (")+j+","+i+")");
            numEval_++;
            match = Relation(j,i,error_);
            if(error_) { return; }
          }
          if(match)
          {
            classes[i].insert(j);
            if(transitive)
            {
              classes[j].clear();
              classified[j]=true;
            }
          }
        }
      }
      m++;
    }
  }
  TraceClasses(2,"classes",1,classes);
  classes_.resize(m);
  for(i=m=0; i<n_; i++)
  {
    if(!classes[i].empty())
    {
      classes_[m++] = classes[i];
    }
  }
  TraceClasses(1,"classes_",2,classes_);
  TRACE_SHOW_LOW("BClassifier::EvalReflexiveRelation", "end");
  iterator_.resize(classes_.size());
}

//--------------------------------------------------------------------
void BClassifier::TransitiveClossure()
//--------------------------------------------------------------------
{
  if(error_) { return; }
  TRACE_SHOW_LOW("BClassifier::TransitiveClossure", "begin");
  //                    unused
  int i, j, i_, j_, cnu /*, iter=0*/;
  int m = classes_.size();
  int changes;
  std::vector< std::set<int> > classes;
  std::vector< int > nonEmpty_;
  std::vector< int > nonEmptyNew_;
  std::set<int>::iterator h;
  std::set<int>::iterator k;
  for(i=cnu=0; i<m; i++)
  {
    if(classes_[i].size()>=1) 
    { cnu++; }
  }
  classes     .resize(cnu);
  nonEmpty_   .resize(cnu);
  nonEmptyNew_.resize(cnu);
  for(i=cnu=0; i<m; i++)
  {
    if(classes_[i].size()>=1) 
    { 
      classes  [cnu] = classes_[i]; 
      nonEmpty_[cnu] = cnu; 
      cnu++;
    }
  }
  TraceClasses(2,"nonEmpty_",0,nonEmpty_);
  do
  {
    TraceClasses(3,"classes",iter++,classes);
    changes = 0;
    for(i_=m=0; i_<(int)nonEmpty_.size(); i_++)
    {
      i = nonEmpty_[i_];
      if(!classes[i].empty())
      {
        nonEmptyNew_[m++] = i;
        for(j_=i_+1; j_<(int)nonEmpty_.size(); j_++)
        {
          j = nonEmpty_[j_];
          if(!classes[j].empty())
          {
            bool hasIntersect = false;
            if(classes[i].size()<classes[j].size())
            {
              for(h=classes[i].begin(); h!=classes[i].end(); h++)
              {
                k = classes[j].find(*h);
                if(k!=classes[j].end())
                {
                  hasIntersect = true;
                  break;
                }
              }
            }
            else
            {
              for(h=classes[j].begin(); h!=classes[j].end(); h++)
              {
                k = classes[i].find(*h);
                if(k!=classes[i].end())
                {
                  hasIntersect = true;
                  break;
                }
              }
            }
            if(hasIntersect) 
            { 
              for(h=classes[j].begin(); h!=classes[j].end(); h++)
              {
                classes[i].insert(*h);
              }
              changes++;
              classes[j].clear();
            }
          }
        }
      }
    }
    nonEmpty_.resize(m);
    for(i_=0; i_<(int)nonEmpty_.size(); i_++)
    {
      nonEmpty_[i_] = nonEmptyNew_[i_];
    }
  } while(changes);
  classes_.clear();
  classes_.resize(nonEmpty_.size());
  for(i_=j=0; i_<(int)nonEmpty_.size(); i_++)
  {
    i = nonEmpty_[i_];
    classes_[j++] = classes[i];
  }
  TraceClasses(1,"classes_",iter++,classes_);
  TRACE_SHOW_LOW("BClassifier::ClassifyBySymmetricRelation", "end");
  iterator_.resize(classes_.size());
}

//--------------------------------------------------------------------
void BClassifier::ClassifyByEquivalenceRelation()
//--------------------------------------------------------------------
{
  EvalReflexiveRelation(true,true);
}

//--------------------------------------------------------------------
void BClassifier::ClassifyBySymmetricRelation()
//--------------------------------------------------------------------
{
  EvalReflexiveRelation(true,false);
  TransitiveClossure();
}

//--------------------------------------------------------------------
void BClassifier::ClassifyByBinaryRelation()
//--------------------------------------------------------------------
{
  EvalReflexiveRelation(false,false);
  TransitiveClossure();
}

//--------------------------------------------------------------------
void BClassifier::ClassifyByPartialOrder()
//--------------------------------------------------------------------
{
/*
  BArray<int> idx_(n_);
  int i;
  for(i=0; i<n_; i++) { idx_[i] = i; }
*/
}

//--------------------------------------------------------------------
  void BClassifier::Classify(const char* relationType)
//--------------------------------------------------------------------
{
  BText rt = ToLower(relationType);
  if(rt == "equivalence")
  {
    ClassifyByEquivalenceRelation();
  }
  else if(rt == "symmetric")
  {
    ClassifyBySymmetricRelation();
  }
  else if(rt == "binary")
  {
    ClassifyByBinaryRelation();
  }
  else
  {
    ::Error(I2("Unknown or non implemented relation type for Classify",
               "Tipo de relación desconocido o no implementado para "
               "la función Classify")+
            " "+relationType);
    error_ = true;
  }
};

//--------------------------------------------------------------------
int BClassifier::GetNumberOfClasses() const
//--------------------------------------------------------------------
{
  return((int)classes_.size());
}

//--------------------------------------------------------------------
int BClassifier::GetSizeOfClass(int i) const
//--------------------------------------------------------------------
{
  return((int)classes_[i].size());
}

//--------------------------------------------------------------------
int BClassifier::GetFirstElementOfClass(int i)
//--------------------------------------------------------------------
{
  iterator_[i] = classes_[i].begin();
  return(*iterator_[i]);
}

//--------------------------------------------------------------------
int BClassifier::GetNextElementOfClass(int i)
//--------------------------------------------------------------------
{
  iterator_[i]++;
  return(*iterator_[i]);
}

/* */
