/* tmsgrav.cpp: Various Time Set Grammar's functions. 
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_btmsgrav.h>
#include <tol/tol_bsys.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_blanguag.h>

//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerTmsGraVarious() { return(BTRUE); }

//--------------------------------------------------------------------
// static members
//--------------------------------------------------------------------
BTraceInit("tmsgrav.cpp");
BDat  BTmsAbortable::maxIter_    = 100000;
BDat  BTmsAbortable::outOfRange_ =  10000;


template<>
BArray<BCacheInfo> BTmsStaticCached<BTmsTemporary, BI_NONE>::cacheInfo_(1);

//--------------------------------------------------------------------
// MACROS for cached successor and predecessor
//--------------------------------------------------------------------
#define USE_CACHE_FOR_SUCC 2


#ifdef USE_CACHE_FOR_SUCC

#if(USE_CACHE_FOR_SUCC==2)
# define PREPARE_CACHE(dte)                                           \
if(!BuildingCache() && !Cache().Size() && BeginCache().IsUnknown() && \
   (dte>=BDate::DefaultFirst()) && (dte<=BDate::DefaultLast()))       \
{                                                                     \
  ForceCache();                                                       \
} 
#elif(USE_CACHE_FOR_SUCC==1)
# define PREPARE_CACHE(dte) 
#endif                                                          

/* */
#define CACHED_SUCCESSOR(dte)                                  \
  if(!BuildingCache() && Cache().Size() && dte.HasValue())     \
  {                                                            \
    BBool okHash = false;                                      \
    BInt  p;                                                   \
    BDate d = HashSuccessor(dte.Hash(), okHash, p);            \
    if(okHash) { return d; }                                   \
  }                                          

#define CACHED_PREDECESSOR(dte)                                \
  if(!BuildingCache()&& Cache().Size() && dte.HasValue())      \
  {                                                            \
    BBool okHash = false;                                      \
    BInt  p;                                                   \
    BDate d = HashPredecessor(dte.Hash(), okHash, p);          \
    if(okHash) { return d; }                                   \
  }                                          
/* * /
#define CACHED_SUCCESSOR(dte)                                              \
  if(!BuildingCache() && !BeginCache().IsUnknown() && dte.HasValue())      \
  {                                                                        \
    BBool okHash = false;                                                  \
    BInt  p;                                                               \
    BDate d;                                                               \
    if(Cache().Size()) { d = HashSuccessor(dte.Hash(), okHash, p); }       \
    if(okHash) { return d; }                                               \
    else if((BeginCache()<=dte)&&(EndCache()>=dte)) { dte = EndCache(); }  \
  }                                          

#define CACHED_PREDECESSOR(dte)                                              \
  if(!BuildingCache() && !BeginCache().IsUnknown() && dte.HasValue())        \ 
  {                                                                          \ 
    BBool okHash = false;                                                    \
    BInt  p;                                                                 \
    BDate d;                                                                 \
    if(Cache().Size()) { d = HashPredecessor(dte.Hash(), okHash, p); }       \
    if(okHash) { return d; }                                                 \
    else if((BeginCache()<=dte)&&(EndCache()>=dte)) { dte = BeginCache(); }  \
  }                                          
/* */
   
#else
  #define CACHED_SUCCESSOR
  #define CACHED_PREDECESSOR
#endif


#define EnsureLimitsSuccessor(dte)                                      \
  if(dte  .IsUnknown () || dte.IsTheEnd  ()) { return(dte); }           \
  if(Inf().IsTheBegin() && dte.IsTheBegin()) { return(dte); }           \
  if(dte< Inf())                             { dte = Inf()-1.0; }       \
  if(dte>=Sup())                             { return(BDate::End()); }

#define EnsureLimitsPredecessor(dte)                                    \
  if(dte  .IsUnknown() || dte.IsTheBegin()) { return(dte); }            \
  if(Inf().IsTheEnd () && dte.IsTheEnd  ()) { return(dte); }            \
  if(dte> Sup())                            { dte = Sup()+1; }          \
  if(dte<=Inf())                            { return(BDate::Begin()); }


//--------------------------------------------------------------------
// non const date reference
//--------------------------------------------------------------------
static BDate& NCDate(const BDate& dte)
{
    BDate* ncDate = (BDate *)(&dte);
    return(*ncDate);
}


//--------------------------------------------------------------------
// Functions for ensurement of progression and regression of timeset's
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  static BText InterruptMsg()
//--------------------------------------------------------------------
{
  return(I2("\nCalculations will be interrumped.",
	          "\nEl cálculo será interrrumpido."));
}


//--------------------------------------------------------------------
  static void UnknownFoundMsg(const BDate& d0)
//--------------------------------------------------------------------
{
  Error(I2("Unknown date found while TimeSet cache building as "
		         "successor of",
           "Se encontró la fecha desconocida construyendo la cache de "
           "TimeSet, como el sucesor de ")+ d0 +InterruptMsg());
};

//--------------------------------------------------------------------
  static void NonProgressiveMsg(const BDate& d0, const BDate& d1)
//--------------------------------------------------------------------
{
  Error(I2("Non progressive TimeSet. Successor of ",
		         "Conjunto temporal no progresivo. El sucesor de ")+ d0 +
		      I2(" cannot be "," no puede ser ") + d1 +InterruptMsg());
};

//--------------------------------------------------------------------
  static bool TestDatingProgress(const BDate& d0, const BDate& d1)
//--------------------------------------------------------------------
{
  bool ok = true;
	 if(d1.IsUnknown()) 
  { 
    UnknownFoundMsg   (d0    ); 
    ok = false; 
  }
		if(d1<=d0)
  { 
    if(d1.HasValue()) { NonProgressiveMsg (d0, d1); }
    ok = false; 
  }
  return(ok);
}


//--------------------------------------------------------------------
  static void NonRegressiveMsg(const BDate& d0, const BDate& d1)
//--------------------------------------------------------------------
{
  Error(I2("Non regressive TimeSet. Successor of ",
		         "Conjunto temporal no progresivo. El predecesor de ")+ d0 +
		      I2(" cannot be "," no puede ser ") + d1 +InterruptMsg());
};

//--------------------------------------------------------------------
  static bool TestDatingRegress(const BDate& d0, const BDate& d1)
//--------------------------------------------------------------------
{
  bool ok = true;
	 if(d1.IsUnknown()) 
  { 
    UnknownFoundMsg   (d0    ); 
    ok = false; 
  }
	if(d1>=d0)
  { 
    if(d1.HasValue()) { NonRegressiveMsg (d0, d1); }
    ok = false; 
  }
  return(ok);
}


//--------------------------------------------------------------------
  static bool TestDatingMoving(const BDate& d0, const BDate& d1, BInt m)
//--------------------------------------------------------------------
{
  bool ok = true;
       if(m>0) { ok = TestDatingProgress(d0,d1); }
  else if(m<0) { ok = TestDatingRegress (d0,d1); }
  else if(d0!=d1)
  {
    Error(I2("Undefined 0-successor of ",
		           "No está definido el 0-sucesor de ")+ d0 +
		        I2("\nInvalid result ","\nResultado inválido ") + d1 +InterruptMsg());  
    ok = false;
  }
  return(ok);
}


//--------------------------------------------------------------------
  void BTmsCached::FreeCache() 
//--------------------------------------------------------------------
{ 
//Std(BText("\n")+GetIdentify()+"->FreeCache["+Cache().Size()+"] ("<<beginCache_<<","<<endCache_<<")\n");
  hashUsedKBytes_ -= double(Cache().MaxSize()*sizeof(BReal))/1024.0;
  cacheInfo_.cache_.DeleteBuffer(); 
  cacheInfo_.beginCache_ = BDate::Unknown();
  cacheInfo_.endCache_   = BDate::Unknown();
}

//--------------------------------------------------------------------
  const BDate& BTmsAbortable::EnsureNotAbortedSuccessor(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(abortedMessageSended_) 
  { 
  //Std(BText("\nabortSucc[")+abortSuccFirst_+","+abortSuccLast_+"]");
    if(!abortSuccFirst_.IsUnknown() && (abortSuccFirst_<=dte)) 
    { return(BDate::End()); }
    if(!abortPredFirst_.IsUnknown() && (abortPredFirst_<=dte) && (dte<abortPredLast_)) 
    { return(abortPredLast_); }
  }
/* * /
  else
  {
    if(!abortSuccFirst_.IsUnknown() && (abortSuccFirst_<=dte) && (dte<abortSuccLast_)) 
    { return(abortSuccLast_); }
    if(!abortPredFirst_.IsUnknown() && (abortPredFirst_<=dte) && (dte<abortPredLast_)) 
    { return(abortPredLast_); }
  }
/* */
  return(dte);
}

//--------------------------------------------------------------------
  const BDate& BTmsAbortable::EnsureNotAbortedPredecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(abortedMessageSended_) 
  { 
  //Std(BText("\nabortPred[")+abortPredFirst_+","+abortPredLast_+"]");
    if(!abortPredFirst_.IsUnknown() && (dte<=abortPredLast_)) 
    { return(BDate::Begin()); }
    if(!abortSuccFirst_.IsUnknown() && (abortSuccFirst_< dte) && (dte<=abortSuccLast_)) 
    { return(abortSuccFirst_); }
  }
/* * /
  else
  {
    if(!abortPredFirst_.IsUnknown() && (abortPredFirst_ < dte) && (dte<=abortPredLast_)) 
    { return(abortPredFirst_); }
    if(!abortSuccFirst_.IsUnknown() && (abortSuccFirst_ < dte) && (dte<=abortSuccLast_)) 
    { return(abortSuccFirst_); }
  }
/* */
  return(dte);
}

//--------------------------------------------------------------------
  void BTmsAbortable::AbortSaveInfo(const BDate& d1, 
                                    const BDate& d2,
                                    short sign) const
//--------------------------------------------------------------------
{
  if(!abortedMessageSended_) { return; }
  BTmsAbortable* T = (BTmsAbortable*)this;
  if(sign<0)
  {
    if(abortPredFirst_.IsUnknown() || abortPredLast_.IsUnknown())
    {
      T->abortPredFirst_  = d2;
      T->abortPredLast_   = d1;
    }
    else 
    {
      if(abortPredLast_ <d1) { T->abortPredLast_  = d1; }
    //if(abortPredFirst_>d2) { T->abortPredFirst_ = d2; }
    }
  }
  else
  {
    if( abortSuccFirst_.IsUnknown() || abortSuccLast_.IsUnknown() )
    {
      T->abortSuccFirst_  = d1;
      T->abortSuccLast_   = d2;
    }
    else 
    {
      if(abortSuccFirst_>d1) { T->abortSuccFirst_ = d1; }
    //if(abortSuccLast_< d2) { T->abortSuccLast_  = d2; }
    }
  }
}

//--------------------------------------------------------------------
  void BTmsAbortable::AbortMessageMaxIter(const BText& function, 
                                          const BDate& d1, 
                                          const BDate& d2,
                                          short sign) const
//--------------------------------------------------------------------
{
  if(!abortedMessageSended_)
  {
    int N = (int)MaxIter().Value();
    Warning(function +
            I2(" has been aborted after ",
               " ha sido abortado después de ")+N+
            I2(" iterations for TimeSet ",
               " iteraciones para el TimeSet ")+Identify()+
            I2(" operating between dates "," operando entre las fechas [")+d1+","+d2+"]"+"\n"+
            I2("Bounded TimeSet resulting behaviour will be assumed.",
               "Se asumirá en los resultados el comportamiento de TimeSet acotado."));
    ((BTmsAbortable*)(this))->abortedMessageSended_ = true;
  }
  AbortSaveInfo(d1,d2, sign);
}

//--------------------------------------------------------------------
  void BTmsAbortable::AbortMessageOutOfRange(const BText& function, 
                                             const BDate& d1, 
                                             const BDate& d2,
                                             short sign) const
//--------------------------------------------------------------------
{
  if(!abortedMessageSended_)
  {
    int M = (int)OutOfRange().Value();
    Warning(function +
            I2(" has been aborted because it has fall out of calculation range ",
               " ha sido abortado porque se ha salido del rango de cálculo ")+
          //" ["+CurrentCalcFirstDate()+ ","+CurrentCalcLastDate()+"] "+
            M+
            I2(" times ",
               " veces ")+ 
            I2(" for TimeSet ",
               " para el TimeSet ")+Identify()+
            I2(" operating between dates "," operando entre las fechas [")+d1+","+d2+"]"+"\n"+
            I2("Bounded TimeSet resulting behaviour will be assumed.",
               "Se asumirá en los resultados el comportamiento de TimeSet acotado."));
    ((BTmsAbortable*)(this))->abortedMessageSended_ = true;
  }
  AbortSaveInfo(d1,d2,sign);
}


//--------------------------------------------------------------------
// Bounded time set's
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DefExtOpr(1, BTmsOneDay, "Day", 1, 1, "Date",
  "(Date d)",
  I2("Returns the time set compound of an only given day.",
     "Devuelve el conjunto temporal compuesto de un único día dado."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
  BTmsOneDay::BTmsOneDay(BList* arg)
//--------------------------------------------------------------------
: BTmsCached(arg), base_(1)
{
  date_=Date(Arg(1));
  if(Arg(2)) { base_=Real(Arg(2)); }
}

//--------------------------------------------------------------------
BBool BTmsOneDay::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{ return(dte == date_); }


//--------------------------------------------------------------------
BDate BTmsOneDay::Successor(const BDate& dte) const
//--------------------------------------------------------------------
{
       if(dte.IsUnknown()) { return(dte); }
  else if(dte <  date_)    { return(date_); }
  else                     { return(BDate::End()); }
}

//--------------------------------------------------------------------
BDate BTmsOneDay::Predecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
       if(dte.IsUnknown()) { return(dte); }
  else if(dte >  date_ )   { return(date_); }
  else                     { return(BDate::Begin()); }
}

//--------------------------------------------------------------------
DefExtOpr(1, BTmsYear, "Y", 1, 1, "Real",
  "(Real n)",
  I2("Returns the time set of all the dates of the umpteenth year.",
     "Devuelve el conjunto temporal de todas las fechas del enésimo año."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsYear::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{ return(dte.Year() == year_); }

//--------------------------------------------------------------------
BDate BTmsYear::Successor(const BDate& dte) const
//--------------------------------------------------------------------
{
       if(dte.IsUnknown()) { return(dte); }
  else if(dte <  first_)   { return(first_); }
  else if(dte >= last_ )   { return(BDate::End()); }
  BDate suc=dte; 
  suc.PutFraction(0);
  suc.IncDay(1); 
  return(suc); 
}

//--------------------------------------------------------------------
BDate BTmsYear::Predecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
       if(dte.IsUnknown()) { return(dte); }
  else if(dte >  last_ )   { return(last_); }
  else if(dte <= first_)   { return(BDate::Begin()); }
  BDate pre=dte; 
  pre.PutFraction(0);
  if(!dte.Fraction()) { pre.IncDay(-1); }
  return(pre);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTmsInterval, "In", 2, 3, "Date Date TimeSet",
  I2("(Date from, Date until [, TimeSet units_ = Daily])",
     "(Date desde, Date hasta [, TimeSet unidades = Diario])"),
  I2("Returns the time set of all the dates between two given dates..",
     "Devuelve el conjunto temporal de todas las fechas "
     "entre dos fechas dadas."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BTmsInterval::BTmsInterval(BList* arg)
//--------------------------------------------------------------------
: BTmsCached(arg)
{
  if(Arg(3)) { units_= Tms(Arg(3)); }
  else       { units_=Tms(OwnGrammar()->FindVariable("C")); }
  first_ = units_->FirstNoLess(Date(Arg(1)));
  last_  = units_->FirstNoGreat(Date(Arg(2)));
  if(first_>last_) throw exception();
}

//--------------------------------------------------------------------
BBool BTmsInterval::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{ return((first_ <= dte) && (dte <= last_) && units_->Contain(dte)); }
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BDate BTmsInterval::Successor(const BDate& dte) const
//--------------------------------------------------------------------
{
       if(dte.IsUnknown()) { return(dte); }
  else if(dte <  first_)   { return(first_); }
  else if(dte >= last_ )   { return(BDate::End()); }
  else                     { BDate suc = dte; suc+=units_; return(suc); }
}

//--------------------------------------------------------------------
BDate BTmsInterval::Predecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
       if(dte.IsUnknown()) { return(dte); }
  else if(dte >  last_ )   { return(last_); }
  else if(dte <= first_)   { return(BDate::Begin()); }
  else                     { BDate pre = dte; pre-=units_; return(pre); }
}


//--------------------------------------------------------------------
// Aritmetic operation with time set's
//--------------------------------------------------------------------



//--------------------------------------------------------------------
DefIntOpr(1, BTmsUnion, "+", 2, 2,
  "tms1 + tms2  {TimeSet tms1, TimeSet tms2}",
  I2("Returns the union of both time sets.",
     "Devuelve la unión de ambos conjuntos temporales."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsUnion::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{ 
  return(a_->Contain(dte) || b_->Contain(dte)); 
}

//--------------------------------------------------------------------
  BDate BTmsUnion::CalcInf() const
//--------------------------------------------------------------------
{
  return(Minimum(a_->Inf(), b_->Inf()));
}

//--------------------------------------------------------------------
  BDate BTmsUnion::CalcSup() const
//--------------------------------------------------------------------
{
  return(Maximum(a_->Sup(), b_->Sup()));
}

//--------------------------------------------------------------------
  void BTmsUnion::CalcHashBetween (BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  double F = first.Hash();
  double L = last .Hash();
  BHash A; a_->GetHashBetween(A, first, last);
  BHash B; b_->GetHashBetween(B, first, last);
  if(!A.Size() && !B.Size())
  {
  //if(a_->NRefs()==1) { a_->FreeCache(); }
  //if(b_->NRefs()==1) { b_->FreeCache(); }
    hash.DeleteBuffer();
    return;
  }
  hash.AllocBuffer(A.Size()+B.Size());
  int i, j, k;
  double cmp;
  for(i=j=k=0; (i<A.Size()) && (j<B.Size()); )
  {
    cmp = A[i]-B[j];
         if(cmp<0) { hash[k++] = A[i++]; }
    else if(cmp>0) { hash[k++] = B[j++]; }
    else           { hash[k++] = B[j++]; i++; }
  }  
  while(i<A.Size()) { hash[k++] = A[i++]; }
  while(j<B.Size()) { hash[k++] = B[j++]; }

  hash.ReallocBuffer(k);
//if(a_->NRefs()==1) { a_->FreeCache(); }
//if(b_->NRefs()==1) { b_->FreeCache(); }
}


//--------------------------------------------------------------------
BDate BTmsUnion::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsSuccessor(dte);
  CACHED_SUCCESSOR(dte);
  BDate d=BDate::End();
  BDate aSuc = a_->Successor(dte); if(aSuc.IsUnknown()) { return(aSuc); }
  BDate bSuc = b_->Successor(dte); if(bSuc.IsUnknown()) { return(bSuc); }
       if (aSuc.IsTheEnd()) { d = bSuc; }
  else if (bSuc.IsTheEnd()) { d = aSuc; }
  else if(aSuc <  bSuc)     { d = aSuc; }
  else		                  { d = bSuc; }
  return (d);
}

//--------------------------------------------------------------------
BDate	 BTmsUnion::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsPredecessor(dte);
  CACHED_PREDECESSOR(dte);
  BDate d=BDate::Begin();
  BDate aPre = a_->Predecessor(dte); if(aPre.IsUnknown()) { return(aPre); }
  BDate bPre = b_->Predecessor(dte); if(bPre.IsUnknown()) { return(bPre); }
       if(aPre.IsTheBegin()) { d = bPre; }
  else if(bPre.IsTheBegin()) { d = aPre; }
  else if(aPre > bPre)       { d = aPre; }
  else		                   { d = bPre; }
  return (d);
}



//--------------------------------------------------------------------
DefIntOpr(1, BTmsIntersection, "*", 2, 2,
  "tms1 * tms2  {TimeSet tms1, TimeSet tms2}",
  I2("Returns the intersection of both time sets.",
     "Devuelve la intersección de ambos conjuntos temporales."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsIntersection::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{ 
  return(a_->Contain(dte) && b_->Contain(dte)); 
}

//--------------------------------------------------------------------
  BDate BTmsIntersection::CalcInf() const
//--------------------------------------------------------------------
{
  return(Maximum(a_->Inf(), b_->Inf()));
}

//--------------------------------------------------------------------
  BDate BTmsIntersection::CalcSup() const
//--------------------------------------------------------------------
{
  return(Minimum(a_->Sup(), b_->Sup()));
}

//--------------------------------------------------------------------
  void BTmsIntersection::CalcHashBetween (BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  double F = first.Hash();
  double L = last .Hash();
  BHash A; a_->GetHashBetween(A, first, last);
  BHash B; b_->GetHashBetween(B, first, last);
  if(!A.Size() || !B.Size())
  {
  //if(a_->NRefs()==1) { a_->FreeCache(); }
  //if(b_->NRefs()==1) { b_->FreeCache(); }
    hash.DeleteBuffer();
    return;
  }
  BHash* A__ = &A;
  BHash* B__ = &B;
  double costA = A.Size()*log((double)B.Size());
  double costB = B.Size()*log((double)A.Size());
  if(costA>costB) { A__ = &B; B__ = &A; }
  BHash& A_ = *A__;
  BHash& B_ = *B__;
  hash.AllocBuffer(A_.Size());
  int i, j;
  double a;
  for(i=j=0; i<A_.Size(); i++)
  {
    a = A_[i];
    if(B_.FindSorted(a,BRealOrderCriterium)>=0)
    {
      hash[j++] = a;
    }
  }  
  hash.ReallocBuffer(j);
//if(a_->NRefs()==1) { a_->FreeCache(); }
//if(b_->NRefs()==1) { b_->FreeCache(); }
}


//--------------------------------------------------------------------
BDate	BTmsIntersection::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = EnsureNotAbortedSuccessor(dte_);
  EnsureLimitsSuccessor(dte);
  PREPARE_CACHE(dte);
  CACHED_SUCCESSOR(dte);
  int iter, n, N = (int)MaxIter().Value(), M = (int)OutOfRange().Value();
  BDate curLimit = CurrentCalcLastDate();
  if(!curLimit.HasValue()) { curLimit = BDate::DefaultLast(); }
  for(n=iter=0; iter<N; iter++)
  {
    dte = EnsureNotAbortedSuccessor(dte);
    if(dte.IsUnknown()||dte.IsTheEnd()) { return(dte); }
    if(dte>=Sup()) { return(BDate::End()); }

    if(curLimit<dte) 
    { 
      if(n>M)
      {
        AbortMessageOutOfRange("BTmsIntersection::Successor",dte_,dte,1); 
        return(BDate::End());
      }
      else
      {
        n++;
      }
    }
    BDate aSuc = a_->Successor(dte);
    if(aSuc.IsUnknown()||aSuc.IsTheEnd()) 
    { AbortSaveInfo(dte_, aSuc, 1); return(aSuc); }
    BDate bSuc = b_->Successor(dte);
    if(bSuc.IsUnknown()||bSuc.IsTheEnd()) 
    { AbortSaveInfo(dte_, bSuc, 1); return(bSuc); }
    if(aSuc==bSuc)	       
    { AbortSaveInfo(dte_, aSuc, 1); return(aSuc); }
    BBool ac = a_->Contain(bSuc);
    BBool bc = b_->Contain(aSuc);
         if( ac && !bc)    
    { AbortSaveInfo(dte_, bSuc, 1); return(bSuc); }
    else if( bc && !ac)    
    { AbortSaveInfo(dte_, aSuc, 1); return(aSuc); }
    else if( ac &&  bc)    
    { BDate d = Minimum(aSuc,bSuc); AbortSaveInfo(dte_, d, 1); return(d); } 
    else                   { dte =  Maximum(aSuc,bSuc);  }
  }
  AbortMessageMaxIter("BTmsIntersection::Successor",dte_,dte,1);
  return(BDate::End());
}

//--------------------------------------------------------------------
BDate BTmsIntersection::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = EnsureNotAbortedPredecessor(dte_);
  EnsureLimitsPredecessor(dte);
  PREPARE_CACHE(dte);
  CACHED_PREDECESSOR(dte);
  int iter, n, N = (int)MaxIter().Value(), M = (int)OutOfRange().Value();
  BDate curLimit = CurrentCalcFirstDate();
  if(curLimit==BDate::Begin()) { curLimit = BDate::DefaultFirst(); }
  for(n=iter=0; iter<N; iter++)
  {
    dte = EnsureNotAbortedPredecessor(dte);
    if(dte.IsUnknown()||dte.IsTheBegin()) { return(dte); }
    if(dte<=Inf()) { return(BDate::Begin()); }
    if(curLimit>dte) 
    { 
      if(n>M)
      {
        AbortMessageOutOfRange("BTmsIntersection::Predecessor",dte_,dte,-1); 
        return(BDate::Begin());
      }
      else
      {
        n++;
      }
    }
    BDate aPred = a_->Predecessor(dte);
    if(aPred.IsUnknown()||aPred.IsTheBegin()) { AbortSaveInfo(dte_, aPred, -1); return(aPred); }
    BDate bPred = b_->Predecessor(dte);
    if(bPred.IsUnknown()||bPred.IsTheBegin()) { AbortSaveInfo(dte_, bPred, -1); return(bPred); }
    if(aPred==bPred)	      { AbortSaveInfo(dte_, aPred, -1); return(aPred); }
    BBool ac = a_->Contain(bPred);
    BBool bc = b_->Contain(aPred);
         if( ac && !bc)    { AbortSaveInfo(dte_, bPred, -1); return(bPred); }
    else if( bc && !ac)    { AbortSaveInfo(dte_, aPred, -1); return(aPred); }
    else if( ac &&  bc)    { BDate d = Maximum(aPred,bPred); AbortSaveInfo(dte_, d, -1); return(d); } 
    else                   { dte =  Minimum(aPred,bPred);  }
  }
  AbortMessageMaxIter("BTmsIntersection::Predecessor",dte_,dte,-1);
  return(BDate::Begin());
}


//--------------------------------------------------------------------
DefIntOpr(1, BTmsDifference, "-", 2, 2,
  "tms1 - tms2  {TimeSet tms1, TimeSet tms2}",
  I2("Returns the difference between both time sets.",
     "Devuelve la diferencia entre ambos conjuntos temporales."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsDifference::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  return(a_->Contain(dte) && !(b_->Contain(dte))); 
}

//--------------------------------------------------------------------
  BDate BTmsDifference::CalcInf() const
//--------------------------------------------------------------------
{
  return(a_->Inf());
}

//--------------------------------------------------------------------
  BDate BTmsDifference::CalcSup() const
//--------------------------------------------------------------------
{
  return(a_->Sup());
}


//--------------------------------------------------------------------
  void BTmsDifference::CalcHashBetween (BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  double F = first.Hash();
  double L = last .Hash();
  BHash A; a_->GetHashBetween(A, first, last);
  BHash B; b_->GetHashBetween(B, first, last);
  hash.AllocBuffer(A.Size());
  int i, j;
  double a;
  for(i=j=0; i<A.Size(); i++)
  {
    a = A[i];
    if(B.FindSorted(a,BRealOrderCriterium)<0)
    {
      hash[j++] = a;
    }
  }  
  hash.ReallocBuffer(j);
//if(a_->NRefs()==1) { a_->FreeCache(); }
//if(b_->NRefs()==1) { b_->FreeCache(); }
}

//--------------------------------------------------------------------
BDate	 BTmsDifference::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = EnsureNotAbortedSuccessor(dte_);
  EnsureLimitsSuccessor(dte);
  PREPARE_CACHE(dte);
  CACHED_SUCCESSOR(dte);
  BDate suc;
  int iter, n, N = (int)MaxIter().Value(), M = (int)OutOfRange().Value();
  BDate curLimit = CurrentCalcLastDate();
  if(!curLimit.HasValue()) { curLimit = BDate::DefaultLast(); }
  if(a_!=b_)
  {
    for(n=iter=0; iter<N; iter++)
    {
      dte = EnsureNotAbortedSuccessor(dte);
      if(dte.IsUnknown()||dte.IsTheEnd()) { return(dte); }
      if(dte>=Sup()) { return(BDate::End()); }
      if(curLimit<dte) 
      { 
        if(n>M)
        {
          AbortMessageOutOfRange("BTmsDifference::Successor",dte_,dte,1); 
          return(BDate::End());
        }
        else
        {
          n++;
        }
      }
      suc = a_->Successor(dte);
      if(suc.HasValue() && (b_->Contain(suc)))
      {
        if(!TestDatingMoving(dte,suc,1)) { AbortSaveInfo(dte_, suc, 1); return(suc); }
        dte = suc;
      }
      else
      {
        AbortSaveInfo(dte_, suc, 1); return(suc);
      }
    }
  }
  else
  {
    return(BDate::End());
  }
  AbortMessageMaxIter("BTmsDifference::Successor",dte_,dte,1);
  return(BDate::End());
}


//--------------------------------------------------------------------
BDate BTmsDifference::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = EnsureNotAbortedPredecessor(dte_);
  EnsureLimitsPredecessor(dte);
  PREPARE_CACHE(dte);
  CACHED_PREDECESSOR(dte);
  BDate pre;
  int iter, n, N = (int)MaxIter().Value(), M = (int)OutOfRange().Value();
  BDate curLimit = CurrentCalcFirstDate();
  if(curLimit==BDate::Begin()) { curLimit = BDate::DefaultFirst(); }
  if(a_!=b_)
  {
    for(n=iter=0; iter<N; iter++)
    {
      dte = EnsureNotAbortedPredecessor(dte);
      if(dte.IsUnknown()||dte.IsTheBegin()) { return(dte); }
      if(dte<=Inf()) { return(BDate::Begin()); }
      if(curLimit>dte) 
      { 
        if(n>M)
        {
          AbortMessageOutOfRange("BTmsDifference::Predecessor",dte_,dte,-1); 
          return(BDate::Begin());
        }
        else
        {
          n++;
        }
      }
      pre = a_->Predecessor(dte);
      if(pre.HasValue() && (b_->Contain(pre)))
      {
        if(!TestDatingMoving(dte,pre,-1)) { AbortSaveInfo(dte_, pre, 1); return(pre); }
        dte = pre;
      }
      else
      {
        AbortSaveInfo(dte_, pre, 1); return(pre);
      }
    }
  }
  else
  {
    return(BDate::Begin());
  }
  AbortMessageMaxIter("BTmsDifference::Predecessor",dte_,dte,-1);
  return(BDate::Begin());
}


//--------------------------------------------------------------------
DefIntOpr(1, BTmsAllUnion, "Union", 2, 0,
  "(TimeSet tms1, TimeSet tms2 [, TimeSet tms3, ... ])",
  I2("Returns the union of all given time sets.",
     "Devuelve la unión de todos los conjuntos temporales dados."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsAllUnion::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  BBool inc = BFALSE;
  for(BInt n=1; !inc && (n<=NumArgs()); n++)
  {
    inc = Tms(Arg(n))->Contain(dte);
  }
  return(inc);
}

//--------------------------------------------------------------------
  BDate BTmsAllUnion::CalcInf() const
//--------------------------------------------------------------------
{
  BDate d, inf;
  for(BInt n=1; (inf!=BDate::Begin()) && (n<=NumArgs()); n++)
  {
    d = Tms(Arg(n))->Inf();
    if((n==1) || (d<inf)) { inf = d; }
  }
  return(inf);
}

//--------------------------------------------------------------------
  BDate BTmsAllUnion::CalcSup() const
//--------------------------------------------------------------------
{
  BDate d, sup;
  for(BInt n=1; (sup!=BDate::End()) && (n<=NumArgs()); n++)
  {
    d = Tms(Arg(n))->Sup();
    if((n==1) || (d>sup)) { sup = d; }
  }
  return(sup);
}

//--------------------------------------------------------------------
void BTmsAllUnion::CalcHashBetween(BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  double F = first.Hash();
  double L = last .Hash();
  BArray<BHash> H (NumArgs());
  int i, j, m;
  int sh = 0;
  for(m=0; m<H.Size(); m++)
  {
    Tms(Arg(m+1))->GetHashBetween(H [m], first, last);
    sh += H[m].Size();
  }
  if(!sh) 
  { 
  //for(m=0; m<H.Size(); m++) { if(Tms(Arg(m+1))->NRefs()==1) { Tms(Arg(m+1))->FreeCache(); } }
    hash.DeleteBuffer(); 
    return; 
  }
  BHash aux1(sh);

  for(m=j=0; m<H.Size(); m++)
  {
    for(i=0; i<H[m].Size(); i++)
    {
      aux1[j++] = H[m][i];
    }
  }  
  aux1.Sort(BRealOrderCriterium);
  BHash aux2(sh);
  for(i=j=0; i<sh; i++)
  {
    if(!i || (aux1[i]>aux1[i-1]))
    {
      aux2[j++] = aux1[i];
    }
  }
  aux2.ReallocBuffer(j);
  hash = aux2;
//for(m=0; m<H.Size(); m++) { if(Tms(Arg(m+1))->NRefs()==1) { Tms(Arg(m+1))->FreeCache(); } }
}


//--------------------------------------------------------------------
BDate BTmsAllUnion::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsSuccessor(dte);
  CACHED_SUCCESSOR(dte);
  BDate d, nSuc;
  for(BInt n=1; n<=NumArgs(); n++)
  {
   	nSuc = Tms(Arg(n))->Successor(dte);
    if(nSuc.IsUnknown()) { return(nSuc); }
    if((n==1) || (nSuc<d)) { d = nSuc; }
  }
//BText msg("\nTmsAllUnion::Successor(");
//Std(msg+dte+")="+d);
  return(d);
}



//--------------------------------------------------------------------
BDate BTmsAllUnion::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsPredecessor(dte);
  CACHED_PREDECESSOR(dte);
  BDate d, nSuc;
  for(BInt n=1; n<=NumArgs(); n++)
  {
   	nSuc = Tms(Arg(n))->Predecessor(dte);
    if(nSuc.IsUnknown()) { return(nSuc); }
    if((n==1) || (nSuc>d)) { d = nSuc; }
  }
//BText msg("\nTmsAllUnion::Predecessor(");
//Std(msg+dte+")="+d);
  return(d);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTmsAllIntersection, "Intersection", 2, 0,
  "(TimeSet tms1, TimeSet tms2 [, TimeSet tms3, ... ])",
  I2("Returns the intersection of all given time sets.",
     "Devuelve la intersección de todos los conjuntos temporales dados."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsAllIntersection::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  BBool inc = BTRUE;
  for(BInt n=1; inc && (n<=NumArgs()); n++)
  {
  	inc = Tms(Arg(n))->Contain(dte);
  }
  return(inc);
}

//--------------------------------------------------------------------
  BDate BTmsAllIntersection::CalcInf() const
//--------------------------------------------------------------------
{
  BDate d, inf;
  for(BInt n=1; n<=NumArgs(); n++)
  {
    d = Tms(Arg(n))->Inf();
    if((n==1) || (d>inf)) { inf = d; }
  }
  return(inf);
}

//--------------------------------------------------------------------
  BDate BTmsAllIntersection::CalcSup() const
//--------------------------------------------------------------------
{
  BDate d, sup;
  for(BInt n=1;n<=NumArgs(); n++)
  {
    d = Tms(Arg(n))->Sup();
    if((n==1) || (d<sup)) { sup = d; }
  }
  return(sup);
}

//--------------------------------------------------------------------
void BTmsAllIntersection::CalcHashBetween(BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  double F = first.Hash();
  double L = last .Hash();
  BArray<BHash>  H  (NumArgs());
  BArray<double> SH (NumArgs());
  int m, n;
  double lsh = 0;
  bool ok = true;
  for(m=0; m<H.Size(); m++)
  {
    Tms(Arg(m+1))->GetHashBetween(H[m], first, last);
    SH[m] = H[m].Size();
    if(!SH[m])
    {
      ok = false;
      break;
    }
    lsh  += log(SH[m]);
  }
  if(ok)
  {
    double costMin;
    for(m=0; m<H.Size(); m++)
    {
      double aux = SH[m]*lsh/log(SH[m]);
      if(!m || (aux<costMin))
      {
        n = m;
        costMin = aux;
      }
    }
    hash.AllocBuffer(H[n].Size());
    int i, j;
    double a;
    for(i=j=0; i<H[n].Size(); i++)
    {
      a = H[n][i];
      bool all = true;
      for(m=0; all & (m<H.Size()); m++)
      {
        if(m!=n)
        {
          all = H[m].FindSorted(a,BRealOrderCriterium)>=0;
        }
      }
      if(all)
      {
        hash[j++] = a;
      }
    }
    hash.ReallocBuffer(j);
  }
  else
  {
    hash.DeleteBuffer();
  }  
//for(m=0; m<H.Size(); m++) { if(Tms(Arg(m+1))->NRefs()==1) { Tms(Arg(m+1))->FreeCache(); } }
}


//--------------------------------------------------------------------
BDate BTmsAllIntersection::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = EnsureNotAbortedSuccessor(dte_);
  EnsureLimitsSuccessor(dte);
  PREPARE_CACHE(dte);
  CACHED_SUCCESSOR(dte);
  int iter, i, j, n, N = (int)MaxIter().Value(), M = (int)OutOfRange().Value();
  BArray<BDate> D(NumArgs());
  BDate curLimit = CurrentCalcLastDate();
  if(!curLimit.HasValue()) { curLimit = BDate::DefaultLast(); }
  for(n=iter=0; iter<N; iter++)
  {
    dte = EnsureNotAbortedSuccessor(dte);
    if(dte.IsUnknown()||dte.IsTheEnd()) { return(dte); }
    if(dte>=Sup()) { return(BDate::End()); }
    if(curLimit<dte) 
    { 
      if(n>M)
      {
        AbortMessageOutOfRange("BTmsAllIntersection::Successor",dte_,dte,1); 
        return(BDate::End());
      }
      else
      {
        n++;
      }
    }
    BDate maxD, minOkD;
    for(i=0; i<NumArgs(); i++)
    {
      D[i] = Tms(Arg(i+1))->Successor(dte); 
      if(D[i].IsUnknown()||D[i].IsTheEnd()) { AbortSaveInfo(dte_, D[i], 1); return(D[i]); }  
      if(!i || (D[i]>maxD)) { maxD = D[i]; }
    }
    for(i=0; i<NumArgs(); i++)
    {
      BBool ok = true, isSet=false;
      for(j=0; j<NumArgs(); j++)
      {
        if((j!=i) && !(ok=Tms(Arg(j+1))->Contain(D[i]))) { break; }
      }
      if(ok)
      {
        if(!isSet || (D[i]<minOkD)) 
        { 
          minOkD = D[i]; 
          isSet  = true;
        }
      }
    }
    if(minOkD.HasValue()) { AbortSaveInfo(dte_, minOkD, 1); return(minOkD); }
    else                  
    { 
    //Std(BText("\nBTmsAllIntersection::Succcessor step(")+iter+") "+dte+" -> "+maxD);
      dte = maxD; 
    }
  }
  AbortMessageMaxIter("BTmsAllIntersection::Successor",dte_,dte,1);
  return(BDate::End());
}

//--------------------------------------------------------------------
BDate BTmsAllIntersection::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = EnsureNotAbortedPredecessor(dte_);
  EnsureLimitsPredecessor(dte);
  PREPARE_CACHE(dte);
  CACHED_PREDECESSOR(dte);
  int iter, i, j, n, N = (int)MaxIter().Value(), M = (int)OutOfRange().Value();
  BArray<BDate> D(NumArgs());
  BDate curLimit = CurrentCalcFirstDate();
  if(curLimit==BDate::Begin()) { curLimit = BDate::DefaultFirst(); }
  for(n=iter=0; iter<N; iter++)
  {
    dte = EnsureNotAbortedPredecessor(dte);
    if(dte.IsUnknown()||dte.IsTheBegin()) { return(dte); }
    if(dte<=Inf()) { return(BDate::Begin()); }
    if(curLimit>dte) 
    { 
      if(n>M)
      {
        AbortMessageOutOfRange("BTmsAllIntersection::Predecessor",dte_,dte,-1); 
        return(BDate::Begin());
      }
      else
      {
        n++;
      }
    }
    BDate minD, maxOkD;
    for(i=0; i<NumArgs(); i++)
    {
      D[i] = Tms(Arg(i+1))->Predecessor(dte); 
      if(D[i].IsUnknown()||D[i].IsTheBegin()) { AbortSaveInfo(dte_, D[i],-1); return(D[i]); }  
      if(!i || (D[i]<minD)) { minD = D[i]; }
    }
    for(i=0; i<NumArgs(); i++)
    {
      BBool ok = true, isSet=false;;
      for(j=0; j<NumArgs(); j++)
      {
        if((j!=i) && !(ok=Tms(Arg(j+1))->Contain(D[i]))) { break; }
      }
      if(ok)
      {
        if(!isSet || (D[i]>maxOkD)) 
        { 
          maxOkD = D[i]; 
          isSet  = true;
        }
      }
    }
    if(maxOkD.HasValue()) { AbortSaveInfo(dte_, maxOkD,-1); return(maxOkD); }
    else                  
    { 
      //Std(BText("\nBTmsAllIntersection::Predecessor step(")+iter+") "+dte+" -> "+minD);
      dte = minD; 
    }
  }
  AbortMessageMaxIter("BTmsAllIntersection::Predecessor",dte_,dte,-1);
  return(BDate::Begin());}


//--------------------------------------------------------------------
// TimeSet traslation functions
//--------------------------------------------------------------------



//--------------------------------------------------------------------
DefExtOpr(1, BTmsPeriodical, "Periodic", 2, 3, "Date Real TimeSet",
  I2("(Date D, Real P [, TimeSet units_ = Daily])",
     "(Date D, Real P [, TimeSet unidades = Diario])"),
  I2("Returns the periodic time set, compound by a date D and all its "
     "virements to a number of given multiple P within a given unit's "
     "dating.",
     "Devuelve el conjunto temporal periódico, compuesto por una fecha D y "
     "todas sus translaciones a un número de fechas múltiplo de P dentro del "
     "fechado de unidades dado."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  BTmsPeriodical::BTmsPeriodical(BList* arg)
//--------------------------------------------------------------------
: BTmsCached(arg)
{
  period_=(BInt)Real(Arg(2));
  if(period_<0) 
  { 
    period_=-period_; 
    Warning(I2("Negative period is not allowed for TimeSet Periodic function.",
               "No se permite periodo negativo para la función TimeSet Periodic.")+
            I2(BText("Positive value ")+period_+" will be used",
               BText("Se usará el valor positive ")+period_)); 
  }
  if(period_==0)
  {
    Warning(I2("Null period for TimeSet Periodic function.",
               "Periodo nulo para la función TimeSet Periodic."));
    centerIsInUnits_ = units_->Contain(center_);
  }
  if(Arg(3)) { units_= Tms(Arg(3)); }
  else       { units_= Tms(OwnGrammar()->FindVariable("C")); }
  center_ = Date(Arg(1));
  if(!units_->Contain(center_))
  {
    center_ = center_+units_;
    Warning(I2("Reference <center> date for TimeSet Periodic must be into traslation base <units> dating.",
               "La fecha de referencia <centro> de la función TimeSet Periodic debe pertencer al fechado base de la traslación.")+
            Date(Arg(1)).Name()+" "+I2("will be replaced","será reemplazada por")+" "+center_.Name());
  }
}

//--------------------------------------------------------------------
BBool BTmsPeriodical::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(dte.IsUnknown()) { return(false); }
  if(!period_)
  { return(centerIsInUnits_ && (dte == center_)); }
  if(units_->Contain(dte))
  {
    int dif = units_->Difference(dte,center_);  
    if(dif<0) { dif+=(abs(dif/period_)+1)*period_; }
  	BBool inc = !(dif%period_);
	  if(inc) { NCDate(center_) = dte; }
	  return(inc);
  }
  else
  { return(BFALSE); }
}

//--------------------------------------------------------------------
  BDate BTmsPeriodical::CalcInf() const
//--------------------------------------------------------------------
{
       if(period_)          { return(BDate::Begin()); }
  else if(centerIsInUnits_) { return(center_);        }
  else                      { return(BDate::End());   }  
}

//--------------------------------------------------------------------
  BDate BTmsPeriodical::CalcSup() const
//--------------------------------------------------------------------
{
       if(period_)          { return(BDate::End());   }
  else if(centerIsInUnits_) { return(center_);        }
  else                      { return(BDate::Begin()); } 
}


//--------------------------------------------------------------------
  void BTmsPeriodical::CalcHashBetween (BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  double F = first.Hash();
  double L = last .Hash();
  int i, j=0;
  BDate  fU = Minimum(first,center_);
  BDate  lU = Maximum(last, center_);
  BHash  U;  units_ ->GetHashBetween(U, fU, lU);
  int    US = U.Size();
  double C  = center_.Hash();
  int    pC = U.FindSorted(C,BRealOrderCriterium);
  int    p0 = pC % period_;
  hash.AllocBuffer(1+US/period_);
  for(i=p0; i<US; i+=period_)
  {
    if((U[i]<F) || (U[i]>L)) { continue; }
    hash[j++] = U[i];
  }
//if(units_->NRefs()==1) { units_->FreeCache(); }
  hash.ReallocBuffer(j);
}

//--------------------------------------------------------------------
BDate BTmsPeriodical::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsSuccessor(dte);
  if(!period_)
  {
    if(centerIsInUnits_ && (dte<center_)) { return(center_); }
    else                                  { return(BDate::End()); }
  }
  if(!(dte.HasValue())) { return(dte); }
//Std(BText("\nBTmsPeriodical::Successor  (")+dte+") { ");
  BDate suc, u = units_->Successor(dte);

  BInt	dif = units_->Difference(u,center_);
//Std(BText("Difference(")+suc+","+center_+") = "+dif);
  if(dif<0) { dif+=(abs(dif/period_)+1)*period_; }
  dif = dif%period_;
//dif = (dif%period_ + period_) % period_;
//Std(BText(" -> ")+dif);
  suc = units_->Next(u,dif);
//Std(BText(" } = ")+suc);

/*
  BInt	dif = units_->Difference(suc,center_);
  if(dif<0) { dif+=(abs(dif/period_)+1)*period_; }
//Std(BText("Difference(")+suc+","+center_+") = "+dif);
  dif = dif%period_;
//Std(BText(" -> ")+dif);
  suc = units_->Next(suc,dif?dif:period_);
*/

/*
  dif = units_->Difference(suc,center_);  
  if(dif<0) { dif+=(abs(dif/period_)+1)*period_; }
  dif = dif%period_;
  assert(dif==0);
*/
  return(suc);
}

//--------------------------------------------------------------------
BDate BTmsPeriodical::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsPredecessor(dte);
  if(!period_)
  {
    if(centerIsInUnits_ && (dte >center_)) { return(center_); }
    else                                   { return(BDate::Begin()); }
  }
  if(!(dte.HasValue())) { return(dte); }
//Std(BText("\nBTmsPeriodical::Predecessor(")+dte+") { ");
  BDate pre, u = units_->Predecessor(dte);

  BInt	dif = units_->Difference(center_,u);
//Std(BText("Difference(")+pre+","+center_+") = "+dif);
  if(dif<0) { dif+=(abs(dif/period_)+1)*period_; }
  dif = dif%period_;
//dif = (dif%period_ + period_) % period_;
//Std(BText(" -> ")+dif);
  pre = units_->Prev(u,dif);
//Std(BText(" } = ")+pre);

/*
  BInt	dif = units_->Difference(center_,pre);
  if(dif<0) { dif+=(abs(dif/period_)+1)*period_; }
//Std(BText("Difference(")+pre+","+center_+") = "+dif);
  dif = dif%period_;
//Std(BText(" -> ")+dif);
  pre = units_->Prev(pre,dif?dif:period_);
//Std(BText(" } = ")+pre);
*/

/*
  dif = units_->Difference(pre,center_);  
  if(dif<0) { dif+=(abs(dif/period_)+1)*period_; }
  dif = dif%period_;
  assert(dif==0);
*/
  return(pre);
}


//--------------------------------------------------------------------
// Auxiliar functions to Succ and Range
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  inline BBool SuccSetStatus(BUserTimeSet* center_, 
                             BInt          n,
                             BUserTimeSet* units_, 
                             const BDate&  u,
                                   BDate&  u0,
                                   BDate&  u1,
                                   BDate&  c)
//--------------------------------------------------------------------
{
//Std(BText("\nSuccSetStatus(")+center_->Identify()+","+n+","+units_->Identify()+","+u+")");
  BBool inc;
  if(n>0)
  {
    u1 = units_ ->Next(u, -n+1);
    u0 = units_ ->Predecessor(u1);
    c  = center_->Predecessor(u1);
    inc = c.HasValue() && (c>=u0);
  }
  else if(n<0)
  {
    u1 = units_ ->Next(u, -n-1);
    u0 = units_ ->Successor  (u1);
    c  = center_->Successor  (u1);
    inc = c.HasValue() && (c<=u0);
  }  
/* * /
  if(n>0)
  { 
    printf("\n  \t%ld\tn\t%ld\tx\t%.0lf\tu0\t%.0lf\t<=\tc\t%.0lf\t<\tu1\t%.0lf\t",(int)inc, n,u.Hash(),u0.Hash(),c.Hash(),u1.Hash());
  } 
  else if(n<0)
  { 
    printf("\n  \t%ld\tn\t%ld\tx\t%.0lf\tu0\t%.0lf\t>=\tc\t%.0lf\t>\tu1\t%.0lf\t",(int)inc, n,u.Hash(),u0.Hash(),c.Hash(),u1.Hash());
  } 
  else
  { 
    printf("\n  \t%ld\tn\t%ld\tx\t%.0lf\tc\t%.0lf ",(int)inc, n,u.Hash(),c.Hash());
  } 
/* */
  return(inc);
}


//--------------------------------------------------------------------
  inline BBool SuccIncludes(BUserTimeSet* center_, 
                            int           n, 
                            BUserTimeSet* units_, 
                            BUserTimeSet* icu_,
                            const BDate&  x)
//--------------------------------------------------------------------
{
  BDate u0, u1, c;
  BBool inc = (n==0)?icu_->Contain(x):SuccSetStatus(center_, n, units_, x, u0, u1, c);

  return(inc);
}

/*
//--------------------------------------------------------------------
  inline BDate SuccSuccessor(BUserTimeSet* center_, 
                             BInt          n,
                             BUserTimeSet* units_, 
                             BUserTimeSet* icu_, 
                             const BDate&  x,
                                   BDate&  u,
                                   BDate&  u0,
                                   BDate&  u1,
                                   BDate&  c)
//--------------------------------------------------------------------
{
  if(x.IsUnknown() || x.IsTheEnd ()) { return(x); }
  if(n==0)  { return(icu_->Successor(x)); }
//Std(BText("\nSuccSuccessor(")+center_->Identify()+","+n+","+units_->Identify()+","+dte+")");
  u = x;
  for(int k=0; k<BTmsAbortable::GetOutOfRange(); k++)
  {
    BDate old = u;
    u = units_->Successor(u);  
    if(u.IsUnknown() || u.IsTheEnd ()) { return(u); }
    assert(old<u);
    if(SuccSetStatus(center_, n, units_, u, u0, u1, c)) { return(u); }
  }
  return(BDate::End());
}


//--------------------------------------------------------------------
  inline BDate SuccPredecessor(BUserTimeSet* center_, 
                               BInt          n,
                               BUserTimeSet* units_, 
                               BUserTimeSet* icu_, 
                               const BDate&  x,
                                     BDate&  u,
                                     BDate&  u0,
                                     BDate&  u1,
                                     BDate&  c)
//--------------------------------------------------------------------
{
  if(x.IsUnknown () || x.IsTheBegin()) { return(x); }
  if(n==0)  { return(icu_->Predecessor(x)); }
//Std(BText("\nSuccPredecessor(")+center_->Identify()+","+n+","+units_->Identify()+","+dte+")");
  u = x;
  for(int k=0; k<BTmsAbortable::GetOutOfRange(); k++)
  {
    BDate old = u;
    u = units_->Predecessor(u);  
    if(u.IsUnknown() || u.IsTheBegin ()) { return(u); }
    assert(old>u);
    if(SuccSetStatus(center_, n, units_, u, u0, u1, c)) { return(u); }
  }
  return(BDate::Begin());
}
*/

/* */
//--------------------------------------------------------------------
  inline BDate SuccSuccessor(BUserTimeSet* center_, 
                             BInt          n,
                             BUserTimeSet* units_, 
                             BUserTimeSet* icu_, 
                             const BDate&  x,
                                   BDate&  u,
                                   BDate&  u0,
                                   BDate&  u1,
                                   BDate&  c)
//--------------------------------------------------------------------
{
  if(x.IsUnknown() || x.IsTheEnd ()) 
  { return(x); }
  if(n==0)  { return(icu_->Successor(x)); }
//Std(BText("\nSuccSuccessor(")+center_->Identify()+","+n+","+units_->Identify()+","+dte+")");
  
  u = units_->Successor(x);  
  if(u.IsUnknown() || u.IsTheEnd ()) 
  { return(u); }
  
  if(!SuccSetStatus(center_, n, units_, u, u0, u1, c))
  {
    BDate u01 = (n>0)?u1:u0;
    c = center_->Successor(u01);
    if(c.IsUnknown () || c.IsTheEnd()) 
    { return(c); }
    u = units_->Next(c, +n);  
  //assert(u>x); 
  }
  BDate old;
  while(u<=x)
  {
    c = center_->Successor(old=c);
  //if(old==c) { return(BDate::End()); }
    if(c.IsUnknown () || c.IsTheEnd()) 
    { return(c); }
    u = units_->Next(c, +n);  
    if(old==u) { return(BDate::End()); }
  }
//Std(BText("\nSuccSuccessor(")+center_->Identify()+","+n+","+units_->Identify()+","+dte+")="+u);
  return(u); 

}


//--------------------------------------------------------------------
  inline BDate SuccPredecessor(BUserTimeSet* center_, 
                               BInt          n,
                               BUserTimeSet* units_, 
                               BUserTimeSet* icu_, 
                               const BDate&  x,
                                     BDate&  u,
                                     BDate&  u0,
                                     BDate&  u1,
                                     BDate&  c)
//--------------------------------------------------------------------
{
  if(x.IsUnknown () || x.IsTheBegin()) 
  { return(x); }
  if(n==0)  { return(icu_->Predecessor(x)); }
//Std(BText("\nSuccPredecessor(")+center_->Identify()+","+n+","+units_->Identify()+","+dte+")");
  u = units_->Predecessor(x);  
  if(u.IsUnknown () || u.IsTheBegin()) 
  { return(u); }
  if(!SuccSetStatus(center_, n, units_, u, u0, u1, c))
  {
    BDate u01 = (n>0)?u1:u0;
    c = center_->Predecessor(u01);
    if(c.IsUnknown () || c.IsTheBegin()) 
    { return(c); }
    u = units_->Next(c, +n);  
  //assert(u<x); 
  }
  BDate old;
  while(u>=x)
  {
    c = center_->Predecessor(old=c);
    if(c.IsUnknown () || c.IsTheBegin()) 
    { return(c); }
    u = units_->Next(c, +n);  
    if(old==u) { return(BDate::Begin()); }
  }
//Std(BText("\nSuccSuccessor(")+center_->Identify()+","+n+","+units_->Identify()+","+dte+")="+u);
  return(u); 
}
/* */

//--------------------------------------------------------------------
DefExtOpr(1, BTmsSuccessor, "Succ", 2, 3, "TimeSet Real TimeSet",
  I2("(TimeSet T, Real n [, TimeSet units_ = Daily])",
     "(TimeSet T, Real n [, TimeSet unidades = Diario])"),
  I2("Returns the time set T, moved n dates along the given unit's dating, "
     "toward the future, if n is positive, or toward the past, if n is "
     "negative. If n is zero returns the own T.",
     "Devuelve el conjunto temporal T, trasladado n fechas a lo largo del "
     "fechado de unidades dado, hacia el futuro, si n es positivo, o hacia "
     "el pasado, si n es negativo. Si n es cero devuelve el propio T."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
  BTmsSuccessor::BTmsSuccessor(BList* arg)
//--------------------------------------------------------------------
: BTmsAbortable(arg)
{
  center_	 =Tms(Arg(1));
  displacement_=(BInt)Real(Arg(2));
  if(Arg(3)) { units_= Tms(Arg(3)); }
  else       { units_=Tms(OwnGrammar()->FindVariable("C")); }
  if(displacement_==0) { icu_ = new BTmsIntersection(center_, units_); }
  else                 { icu_ = NULL; }
}


//--------------------------------------------------------------------
BBool BTmsSuccessor::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
//Std(BText("\n[")+Identify() + "]::Includes(" +dte+")");
  if(!(units_->Contain(dte))) { return(BFALSE); }
  return(SuccIncludes(center_,displacement_,units_,icu_,dte));
}

//--------------------------------------------------------------------
  BDate BTmsSuccessor::CalcInf() const
//--------------------------------------------------------------------
{
  if(!displacement_) { return(icu_->Inf()); }
  BDate inf = units_->Next(center_->Inf(),displacement_);
  inf = Maximum(inf, units_->Inf());
  return(inf);
}

//--------------------------------------------------------------------
  BDate BTmsSuccessor::CalcSup() const
//--------------------------------------------------------------------
{
  if(!displacement_) { return(icu_->Sup()); }
  BDate sup = units_->Next(center_->Sup(),displacement_);
  sup = Minimum(sup, units_->Sup());
  return(sup);
}

//--------------------------------------------------------------------
  void BTmsSuccessor::CalcHashBetween (BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  double F = first.Hash();
  double L = last .Hash();
  int i, j=0, n=displacement_, pC;
  BDate fU = units_->Next(first,(n>0)?-n:-1);
  BDate lU = units_->Next(last, (n<0)?-n: 1);
  if(fU<units_->Inf()) { fU = units_->Inf(); }
  if(lU>units_->Sup()) { lU = units_->Sup(); }
  BDate fUC = center_->Predecessor(fU);
  BDate lUC = center_->Successor  (lU);
  BHash C; center_->GetHashBetween(C, fUC, lUC);
  if(!C.Size()) 
  { 
  //if(center_->NRefs()==1) { center_->FreeCache(); }
    hash.DeleteBuffer();
    return; 
  }
  BHash U; units_ ->GetHashBetween(U, fU, lU);
  int US = U.Size();
  hash.AllocBuffer(US);
  BDate fU_ = HashToDte(U[0   ])-units_;
  BDate lU_ = HashToDte(U[US-1])-units_;
//BDate fU_ = (fU.Hash()<U[0   ])?fU:BDate::End  ();
//BDate lU_ = (lU.Hash()<U[US-1])?lU:BDate::Begin();
  double u0;
  for(i=0; i<US; i++)
  {
    if((U[i]<F) || (U[i]>L)) { continue; }
    u0 = (i-n<0)?fU_.Hash():(i-n>=US)?lU_.Hash():U[i-n];
    if( ((n> 0) && ((pC=HashPred(C,U[i-n+1]))>=0) && (C[pC]>=u0)) ||
        ((n< 0) && ((pC=HashSucc(C,U[i-n-1]))>=0) && (C[pC]<=u0)) ||
        ((n==0) && ((pC=C.FindSorted(U[i],BRealOrderCriterium))>=0))    )  
    {
      hash[j++] = U[i];
    }
  }  
  hash.ReallocBuffer(j);
//if(units_ ->NRefs()==1) { units_ ->FreeCache(); }
//if(center_->NRefs()==1) { center_->FreeCache(); }
}


//--------------------------------------------------------------------
BDate BTmsSuccessor::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsSuccessor(dte);
  PREPARE_CACHE(dte);
  CACHED_SUCCESSOR(dte);
  BDate u, u0, u1, c;
  return(SuccSuccessor(center_, displacement_, units_, icu_, dte, u, u0, u1, c));

}

//--------------------------------------------------------------------
BDate BTmsSuccessor::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsPredecessor(dte);
  PREPARE_CACHE(dte);
  CACHED_PREDECESSOR(dte);
  BDate u, u0, u1, c;
  return(SuccPredecessor(center_, displacement_, units_, icu_, dte, u, u0, u1, c));
}


//--------------------------------------------------------------------
BDate BTmsSuccessor::Next(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  if(icu_) { return(icu_->Next(dte,nth));	 }
  return(BTmsTemporary::Next(dte,nth));
}

//--------------------------------------------------------------------
BDate BTmsSuccessor::Prev(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  if(icu_) { return(icu_->Prev(dte,nth));	 }
  return(BTmsTemporary::Prev(dte,nth));
}


//--------------------------------------------------------------------
DefExtOpr(1, BTmsRangeSuc, "Range", 3, 4, "TimeSet Real Real TimeSet",
  I2("(TimeSet T, Real N0, Real N1 [, TimeSet units_ = Daily])",
     "(TimeSet T, Real N0, Real N1 [, TimeSet unidades = Diario])"),
  I2("Returns the union of all the virements (see the function Succ) of the "
     "time set T, for each entire number from N0 until N1. Obviously, if "
     "N0>N1 is obtained the empty time set.",
     "Devuelve la unión de todas las translaciones (vea la función Succ) del "
     "conjunto temporal T, para cada número entero desde N0 hasta N1. "
     "Obviamente, si N0>N1 se obtiene el conjunto temporal vacío."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
  BTmsRangeSuc::BTmsRangeSuc(BList* arg)
//--------------------------------------------------------------------
: BTmsAbortable(arg)
{
  center_=Tms(Arg(1));
  from_  =(BInt)Real(Arg(2));
  until_ =(BInt)Real(Arg(3));
  if(from_>until_) 
  { 
    Warning(I2("Function TimeSet Range between","La función TimeSet Range entre")+
            " "+from_+" "+I2("and", "y")+" "+until_+" "+
            I2("will return empty TimeSet", "devolverá el conjunto vacío")); 
  }
  if(Arg(4)) { units_= Tms(Arg(4)); }
  else       { units_= Tms(OwnGrammar()->FindVariable("C")); }
  if((from_<=0) && (until_>=0)) { icu_ = new BTmsIntersection(center_, units_); }
  else                 { icu_ = NULL; }
}

//--------------------------------------------------------------------
  void BTmsRangeSuc::GetChildHashes(const BDate& first, 
                                    const BDate& last,
                                          BDate& fU,
                                          BDate& lU,
                                          BDate& fU_,
                                          BDate& lU_,
                                          BHash& C,
                                          BHash& U,
                                          int&   US)
//--------------------------------------------------------------------
{
  fU = units_->Next(first,(until_>0)?-until_:-1);
  lU = units_->Next(last, (from_ <0)?-from_ : 1);
  BDate fC = fU, lC = lU;
  if(fU<=units_ ->Inf()) { fU = units_ ->Inf(); fC = center_->Predecessor(fU); }
  if(lU>=units_ ->Sup()) { lU = units_ ->Sup(); lC = center_->Successor  (lU); }
  center_->GetHashBetween(C, fC, lC);
  units_ ->GetHashBetween(U, fU, lU);
  US = U.Size();
  fU_ = HashToDte(U[0   ])-units_;
  lU_ = HashToDte(U[US-1])+units_;
};

//--------------------------------------------------------------------
BBool BTmsRangeSuc::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(from_>until_) { return(false); }
  if(!(units_->Contain(dte))) { return(BFALSE); }
//Std(BText("\nBTmsRangeSuc::Includes(")+dte+")");
  BInt	k;
  BBool inc=false;
  for(k = from_; (k<=until_) && !inc; k++)
  {
    inc = SuccIncludes(center_,k,units_,icu_,dte);
  }

//Std(BText("\n  BTmsRangeSuc::Includes(")+dte+")"+" = "+int(inc));
  return(inc);
}

//--------------------------------------------------------------------
  BDate BTmsRangeSuc::CalcInf() const
//--------------------------------------------------------------------
{
//Std(BText("\nCalculating BTmsRangeSuc::CalcInf ")+GetIdentify());
  if(from_>until_) { return(BDate::End()); }
  BDate d, inf, cInf = center_->Inf(), uInf = units_->Inf();
  if(cInf.IsTheBegin()) { return(uInf); }
  for(int k = from_; (inf!=BDate::Begin()) && (k<=until_); k++)
  {
    if(!k) { d = icu_->Inf(); }
    else   { d = units_->Next(cInf,k); }
    if((k==from_) || (d<inf)) { inf = d; }
  //Std(BText("\n  k=")+k+" inf="+inf);
  }
  inf = Maximum(inf, units_->Inf());
//Std(BText("\n  ")+k+" inf="+inf);
  return(inf);
}

//--------------------------------------------------------------------
  BDate BTmsRangeSuc::CalcSup() const
//--------------------------------------------------------------------
{
//Std(BText("\nCalculating BTmsRangeSuc::CalcSup ")+GetIdentify());
  if(from_>until_) { return(BDate::Begin()); }
  BDate d, sup, cSup = center_->Sup(), uSup = units_->Sup();
  if(cSup.IsTheEnd()) { return(uSup); }
  for(int k = from_; (sup!=BDate::End()) && (k<=until_); k++)
  {
    if(!k) { d = icu_->Sup(); }
    else   { d = units_->Next(cSup,k); }
    if((k==from_) || (d>sup)) { sup = d; }
  //Std(BText("\n  k=")+k+" sup="+sup);
  }
  sup = Minimum(sup, uSup);
//Std(BText("\n  ")+k+" sup="+sup);
  return(sup);
}


//--------------------------------------------------------------------
  void BTmsRangeSuc::CalcHashBetween (BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  if(from_>until_) { hash.DeleteBuffer(); return; }
  double F = first.Hash();
  double L = last .Hash();
//Std(BText("\nCalculating ")+GetIdentify()+"->GetHashBetween("+first+","+last+")");

  int i, j=0, n, pC, US;
  BDate fU,lU,fU_,lU_;
  BHash C, U;
  GetChildHashes(first, last, fU, lU, fU_, lU_, C, U, US);

  if(!C.Size() || !US) 
  { 
  //if(center_->NRefs()==1) { center_->FreeCache(); }
  //if(units_ ->NRefs()==1) { units_ ->FreeCache(); }
    hash.DeleteBuffer();
    return; 
  }
  hash.AllocBuffer(US);
  double u0;
  for(i=0; i<US; i++)
  {
    if((U[i]<F) || (U[i]>L)) { continue; }
    bool none = true;
    for(n=from_; none && (n<=until_); n++)
    {
      u0 = (i-n<0)?fU_.Hash():(i-n>=US)?lU_.Hash():U[i-n];
      if( ((n> 0) && ((pC=HashPred(C,U[i-n+1]))>=0) && (C[pC]>=u0)) ||
          ((n< 0) && ((pC=HashSucc(C,U[i-n-1]))>=0) && (C[pC]<=u0)) ||
          ((n==0) && ((pC=C.FindSorted(U[i],BRealOrderCriterium))>=0))     )
      {
        none = false;
      }
/* * /
      int inc = !none;
      if(n>0)
      { 
        printf("\nBTmsRangeSuc::GetHashBetween\t%ld\tn\t%ld\tx\t%.0lf\tpC\t%ld\tu0\t%.0lf\t<=\tc\t%.0lf\t<\tu1\t%.0lf",inc,n,U[i],pC,u0,C[pC],U[i-n+1]);
      } 
      else if(n<0)
      { 
        printf("\nBTmsRangeSuc::GetHashBetween\t%ld\tn\t%ld\tx\t%.0lf\tpC\t%ld\tu0\t%.0lf\t>=\tc\t%.0lf\t>\tu1\t%.0lf",inc,n,U[i],pC,u0,C[pC],U[i-n-1]);
      } 
      else
      { 
        printf("\nBTmsRangeSuc::GetHashBetween\t%ld\tn\t%ld\tx\t%.0lf\tpC\t%ld\tc\t%.0lf",inc,n,U[i],pC,C[pC]);
      } 
/* */
    }
    if(!none)
    { 
      hash[j++] = U[i];
    }
  }  
  hash.ReallocBuffer(j);
//if(units_ ->NRefs()==1) { units_ ->FreeCache(); }
//if(center_->NRefs()==1) { center_->FreeCache(); }
}


/*
#define NumClk_ 77
static double clk_[NumClk_];
void ShowClk()
{
  for(int n=0; n<NumClk_; n++) 
  {
    printf("\nShowClk[%ld]=%.15lg",n,clk_[n]);
  }
};
void NullClk()
{
  for(int n=0; n<NumClk_; n++) 
  {
    clk_[n] = 0;
  }
};
*/

//--------------------------------------------------------------------
BDate BTmsRangeSuc::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsSuccessor(dte);
  if(from_>until_) { return(BDate::End()); }
  PREPARE_CACHE(dte);
  CACHED_SUCCESSOR(dte);
//Std(BText("\nCalculating ")+GetIdentify()+"->Successor("+dte+")");
  if(!from_ && !until_) { return(icu_->Successor(dte));  }

  BDate d, dn;
  BInt	 n;
  BDate u, u0, u1, c;
  for(n=from_; n<=until_; n++)
  {
  //double clk0 = clock();
    dn = SuccSuccessor(center_, n, units_, icu_, dte, u, u0, u1, c);
  //double clk1 = clock();
  //clk_[n-from_] += clk1-clk0;
    if(dn.IsUnknown()) { return(dn); }
    if((n==from_) || (dn<d)) { d = dn; }
  //Std(BText("\n  n=")+n+"; dn="+dn+"; d="+d);
  }
  return(d);
}

//--------------------------------------------------------------------
BDate BTmsRangeSuc::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = dte_;
  EnsureLimitsPredecessor(dte);
  if(from_>until_) { return(BDate::Begin()); }
  PREPARE_CACHE(dte);
  CACHED_PREDECESSOR(dte);
//Std(BText("\nCalculating ")+GetIdentify()+"->Predecessor("+dte+")");
  if(!from_ && !until_) { return(icu_->Predecessor(dte));  }
  BDate d, dn;
  BInt	 n; 
  BDate u, u0, u1, c;
  for(n=from_; n<=until_; n++)
  {
  //double clk0 = clock();
    dn = SuccPredecessor(center_, n, units_, icu_, dte, u, u0, u1, c);
  //double clk1 = clock();
  //clk_[n-from_] += clk1-clk0;
    if(dn.IsUnknown()) { return(dn); }
    if((n==from_) || (dn>d)) { d = dn; }
  //Std(BText("\n  n=")+n+"; dn="+dn+"; d="+d);
  }
  return(d);
}


//--------------------------------------------------------------------
BDate BTmsRangeSuc::Next(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  if(from_>until_) { return(BDate::End()); }
  if(!from_ && !until_) { return(icu_->Next(dte,nth));  }
  return(BTmsTemporary::Next(dte,nth));
}

//--------------------------------------------------------------------
BDate BTmsRangeSuc::Prev(const BDate& dte, BInt nth) const
//--------------------------------------------------------------------
{
  if(from_>until_) { return(BDate::Begin()); }
  if(!from_ && !until_) { return(icu_->Prev(dte,nth));  }
  return(BTmsTemporary::Prev(dte,nth));
}


//--------------------------------------------------------------------
// External conversion functions of time set's
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DefExtOpr(1, BTmsDatesOfSet, "DatesOfSet", 1, 1, "Set",
  "(Set s)",
  I2("Returns the time set compound all over the dates of an ordinary set "
     "of dates.",
     "Devuelve el conjunto temporal compuesto por todas las fechas de "
     "un conjunto ordinario de fechas."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BTmsDatesOfSet::BTmsDatesOfSet(BList* arg)
//--------------------------------------------------------------------
: BTmsCached(arg)
{
  BSet& set = Set(Arg(1));
  BArray<BDate>dates(set.Card());
  BSyntaxObject* obj = NULL;
  int i,j;
  for(i=j=0; i<dates.Size(); i++)
  {
    obj = set[i+1];
    if(obj && (obj->Grammar()==GraDate()))
    {
      dates[j++] = Date(obj);
    }
    else if(obj)
    {
      Error(BText("Invalid argument for DatesOfSet: ")+obj->Grammar()->Name()+
            " "+obj->Identify()+ "cannot be converted to a Date object");
    }
  }
  dates.ReallocBuffer(j);
  dates.Sort(DateCmp);
  cacheInfo_.cache_.AllocBuffer(dates.Size()); 
  for(i=j=0; i<dates.Size(); i++)
  {
    if((i==0)||(dates[i]!=dates[i-1]))
    {
      cacheInfo_.cache_[j++] = dates[i].Hash();
    }
  }
  cacheInfo_.cache_.ReallocBuffer(j); 
  int s = cacheInfo_.cache_.Size();
  if(s)
  {
    cacheInfo_.beginCache_.PutHash(cacheInfo_.cache_[0  ]);
    cacheInfo_.endCache_  .PutHash(cacheInfo_.cache_[s-1]);
  }
  else
  {
    cacheInfo_.beginCache_ = BDate::Begin();
    cacheInfo_.endCache_   = BDate::End();
  }

//for(i=0; i<dates_.Size(); i++)  { Std(BText("\nDate[")+(i+1)+"]="+dates_[i]); }
}


//--------------------------------------------------------------------
BTmsDatesOfSet::BTmsDatesOfSet(BHash& h)
//--------------------------------------------------------------------
: BTmsCached()
{
  cacheInfo_.cache_ = h;
  int s = cacheInfo_.cache_.Size();
  if(s)
  {
    cacheInfo_.beginCache_.PutHash(cacheInfo_.cache_[0  ]);
    cacheInfo_.endCache_  .PutHash(cacheInfo_.cache_[s-1]);
  }
  else
  {
    cacheInfo_.beginCache_ = BDate::Begin();
    cacheInfo_.endCache_   = BDate::End();
  }
}


//--------------------------------------------------------------------
  BDate BTmsDatesOfSet::CalcInf() const 
//--------------------------------------------------------------------
{ 
  int s = Cache().Size();
  return(s?HashToDte(Cache()[0]):BDate::End()); 
}

//--------------------------------------------------------------------
  BDate BTmsDatesOfSet::CalcSup() const 
//--------------------------------------------------------------------
{ 
  int s = Cache().Size();
  return(s?HashToDte(Cache()[s-1]):BDate::Begin()); 
}

//--------------------------------------------------------------------
BBool BTmsDatesOfSet::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  BInt s=Cache().Size();
  if(!s) { return(false); }
  int n = Cache().FindSorted(dte.Hash(),BRealOrderCriterium);
  return(n>=0);
}

//--------------------------------------------------------------------
BDate BTmsDatesOfSet::Successor(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(dte.IsUnknown()) { return(dte); }
//CACHED_SUCCESSOR(dte);

  BInt s=Cache().Size();
  if(!s) { return(BDate::End()); }
  double h = dte.Hash();
  if(h>=Cache()[s-1]) { return(BDate::End()); }
  if(h< Cache()[0  ]) { return(HashToDte(Cache()[0])); }
  int p = HashSucc(Cache(),h);
  return(HashToDte(Cache()[p]));
}

//--------------------------------------------------------------------
BDate BTmsDatesOfSet::Predecessor(const BDate& dte) const
//--------------------------------------------------------------------
{
  if(dte.IsUnknown()) { return(dte); }
//CACHED_PREDECESSOR(dte);
  BInt s=Cache().Size();
  if(!s) { return(BDate::Begin()); }
  double h = dte.Hash();
  if(h<=Cache()[0  ]) { return(BDate::Begin()); }
  if(h> Cache()[s-1]) { return(HashToDte(Cache()[s-1])); }
  int p = HashPred(Cache(),h);
  return(HashToDte(Cache()[p]));
}


//--------------------------------------------------------------------
  void BTmsDatesOfSet::GetHashBetween (BHash& hash, BDate first, BDate last) const
//--------------------------------------------------------------------
{
  if(!first.HasValue()) { first = BDate::DefaultFirst(); }
  if(! last.HasValue()) {  last = BDate::DefaultLast (); }  
  if(first<Inf()) { first = Inf(); }
  if(last >Sup()) { last  = Sup(); }
  first = FirstNoLess  (first);
  last	= FirstNoGreat (last );
  double hf = first.Hash();
  double hl = last .Hash();
  int f = Cache().FindSorted(hf,BRealOrderCriterium);
  int l = Cache().FindSorted(hl,BRealOrderCriterium);
  int n;
  hash.AllocBuffer(l-f+1);
  for(n=f; n<=l; n++) { hash[n-f] = Cache()[n]; }
}

//--------------------------------------------------------------------
DefExtOpr(1, BTmsOfSerie, "SerTms", 1, 1, "Serie",
  "(Serie ser)",
  I2("Returns the time set of all the dates of the dating of a series in "
     "those which this is not annulled.",
     "Devuelve el conjunto temporal de todas las fechas del fechado de una "
     "serie en las que ésta no se anula."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
BBool BTmsOfSerie::Includes(const BDate& dte) const
//--------------------------------------------------------------------
{
  BUserTimeSerie* ser = Tsr(Arg(1));
  return(ser->Dating() &&
    	   ser->Dating()->Contain(dte) &&
	       (
	         (dte<ser->FirstDate()) ||
	         (dte>ser->LastDate ()) ||
	         (*ser)[dte].Value()));
}

//--------------------------------------------------------------------
  BDate BTmsOfSerie::CalcInf() const 
//--------------------------------------------------------------------
{ 
  return(Tsr(Arg(1))->FirstDate()); 
}

//--------------------------------------------------------------------
  BDate BTmsOfSerie::CalcSup() const 
//--------------------------------------------------------------------
{ 
  return(Tsr(Arg(1))->LastDate ()); 
}

//--------------------------------------------------------------------
  void BTmsOfSerie::GetHashBetween (BHash& hash, BDate first, BDate last) const
//--------------------------------------------------------------------
{
  BUserTimeSerie* ser = Tsr(Arg(1));
  BHash aux;
  ser->Dating()->GetHashBetween(aux,  first, last);
  BTmsTemporary::GetHashBetween(hash, first, last);
}


//--------------------------------------------------------------------
BDate BTmsOfSerie::Successor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = EnsureNotAbortedSuccessor(dte_);
  EnsureLimitsSuccessor(dte);
  PREPARE_CACHE(dte);
  CACHED_SUCCESSOR(dte);
  int iter, n, N = (int)MaxIter().Value(), M = (int)OutOfRange().Value();
  BDate curLimit = CurrentCalcLastDate();
  if(!curLimit.HasValue()) { curLimit = BDate::DefaultLast(); }
//Std(Out()+"\nBTmsOfSerie::Successor of "+ser->Identify()+" "+dte);
  BUserTimeSerie* ser = Tsr(Arg(1));
  for(n=iter=0; iter<N; iter++)
  {
    dte = EnsureNotAbortedSuccessor(dte);
    if(dte.IsUnknown()||dte.IsTheEnd()) { return(dte); }
    if(dte>=Sup()) { return(BDate::End()); }

    if(curLimit<dte) 
    { 
      if(n>M)
      {
        AbortMessageOutOfRange("BTmsOfSerie::Successor",dte_,dte,1); 
        return(BDate::End());
      }
      else
      {
        n++;
      }
    }
  	dte += ser->Dating();
    if(Contain(dte)) { return(dte); }
    if(dte>ser->LastDate()) { return(BDate::End()); }
  }
  AbortMessageMaxIter("BTmsOfSerie::Successor",dte_,dte,1);
  return(BDate::End());
}

//--------------------------------------------------------------------
BDate BTmsOfSerie::Predecessor(const BDate& dte_) const
//--------------------------------------------------------------------
{
  BDate dte = EnsureNotAbortedPredecessor(dte_);
  EnsureLimitsPredecessor(dte);
  PREPARE_CACHE(dte);
  CACHED_PREDECESSOR(dte);
  int iter, n, N = (int)MaxIter().Value(), M = (int)OutOfRange().Value();
  BDate curLimit = CurrentCalcFirstDate();
  if(curLimit==BDate::Begin()) { curLimit = BDate::DefaultFirst(); }
  BUserTimeSerie* ser = Tsr(Arg(1));
  for(n=iter=0; iter<N; iter++)
  {
    dte = EnsureNotAbortedPredecessor(dte);
    if(dte.IsUnknown()||dte.IsTheBegin()) { return(dte); }
    if(dte<=Inf()) { return(BDate::Begin()); }
    if(curLimit>dte) 
    { 
      if(n>M)
      {
        AbortMessageOutOfRange("BTmsOfSerie::Predecessor",dte_,dte,-1); 
        return(BDate::Begin());
      }
      else
      {
        n++;
      }
    }
  	dte -= ser->Dating();
    if(Contain(dte)) { return(dte); }
    if(dte>ser->LastDate()) { return(BDate::End()); }
  }
  AbortMessageMaxIter("BTmsOfSerie::Predecessor",dte_,dte,-1);
  return(BDate::Begin());
}



//--------------------------------------------------------------------
// Miscallanea funcions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTextTimeSetTestCoherence);
DefExtOpr(1, BTextTimeSetTestCoherence, "TestCoherenceOfTimeSet", 1, 7, "TimeSet Real Date Date Real Real Real",
          "(TimeSet tms [, Real verboseMode=0, Date from=DefFirst, Date until=DefLast, Real maxTimeSim=120, Real numSim=1000, Real maxLag=10])",
          I2("Tests coherence of different TimeSet definition methods in a specified interval\n"
             "This function gives back text OK if everything has gone either or a text warning that no date "
             "of the interval belongs to him or in its case a message detailing the detected incoherence.\n"
             "Allowed values for verboseMode are \n"
             "  0: None displaying\n"
             "  1: Only displays steps description\n"
             "  2: Strongly detailed displaying\n"
             "  3: Fully detailed displaying",
             "Comprueba la coherencia de los métodos de definición de un TimeSet en el intervalo especificado\n"
             "Esta función devuelve el texto OK si todo ha ido bien o bien un texto avisando de que ninguna "
             "fecha del intervalo le pertenece o en su caso un mensaje detallando la incoherencia detectada.\n"
             "Los valores permitidos para verboseMode son \n"
             "  0: No muestra nada\n"
             "  1: Muestra sólo la descripción de los pasos\n"
             "  2: Muestra la mayoría de los detalles\n"
             "  3: Muestra todos los detalles"),
          BOperClassify::Conversion_);
void BTextTimeSetTestCoherence::CalcContens()
//--------------------------------------------------------------------
{
  BUserTimeSet* tms    = Tms (Arg(1));
  int           verbose= 0;
  BDate         from   = BDate::DefaultFirst();
  BDate         until  = BDate::DefaultLast ();
  double        mxTmSm = 120.0;
  int           numSim = 1000;
  int           maxLag = 10;
  if(Arg(2)) { verbose = (int)Real(Arg(2)); }
  if(Arg(3)) { from    =      Date(Arg(3)); }
  if(Arg(4)) { until   =      Date(Arg(4)); }
  if(Arg(5)) { mxTmSm  =      Real(Arg(5)); }
  if(Arg(6)) { numSim  = (int)Real(Arg(6)); }
  if(Arg(7)) { maxLag  = (int)Real(Arg(7)); }

  contens_ = tms->TestCoherence(verbose, from, until, mxTmSm, numSim, maxLag);
}
