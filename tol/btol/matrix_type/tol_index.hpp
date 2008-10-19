/* tol_index.hpp: Implementation for BDiagMatrix class.
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

//--------------------------------------------------------------------
template <class Any>
BInt 
BIndexed<Any>::Cmp(const void* v1, const void* v2)
//--------------------------------------------------------------------
{
    BIndexed<Any>& id1 = *((BIndexed<Any>*)v1);
    BIndexed<Any>& id2 = *((BIndexed<Any>*)v2);
    Any dif = id1.dat_-id2.dat_;
    if(dif<0) { return(-1); }
    if(dif>0) { return( 1); }
    return(0);
    
}

//--------------------------------------------------------------------
template <class Any>
void 
BIndexed<Any>::Sort(BArray< BIndexed<Any> >& index)
//--------------------------------------------------------------------
{
    index.Sort(BIndexed<Any>::Cmp);
}

//--------------------------------------------------------------------
template <class Any>
void 
BIndexed<Any>::Order(const BArray<Any >& data,
		           BArray<BInt>& direct)
//--------------------------------------------------------------------
{
    BInt i, n = data.Size();
    BArray< BIndexed<Any> > index(n);
    for(i=0; i<n; i++)
    {
	index(i).dat_ = data(i);
	index(i).pos_ = i;
    }
    BIndexed<Any>::Sort(index);
    direct.ReallocBuffer(n);
    for(i=0; i<n; i++)
    {
	BInt j = index(i).pos_;
	direct (i) = j;
    }
}

//--------------------------------------------------------------------
template <class Any>
void 
BIndexed<Any>::Order(const BArray<Any >& data,
		           BArray<BInt>& direct,
		           BArray<BInt>& inverse)
//--------------------------------------------------------------------
{
    BInt i, n = data.Size();
    BArray< BIndexed<Any> > index(n);
    for(i=0; i<n; i++)
    {
	index(i).dat_ = data(i);
	index(i).pos_ = i;
    }
    BIndexed<Any>::Sort(index);
    direct.ReallocBuffer(n);
    inverse.ReallocBuffer(n);
    for(i=0; i<n; i++)
    {
	BInt j = index(i).pos_;
	direct (i) = j;
	inverse(j) = i;
    }   
}


