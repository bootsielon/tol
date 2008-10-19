/* tms.cpp: Other implementation functions of BTimeSet class and
            deriving classes.
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

#include <tol/tol_btmset.h>
#include <tol/tol_bout.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bdir.h>
#include <tol/tol_btmsgra.h>

//#define BTimeSetHashStat
BTraceInit("tms.cpp");

double BTimeSet::hashUsedKBytes_ = 0; 


//--------------------------------------------------------------------
// BTimeSet functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  static BText InterruptMsg()
//--------------------------------------------------------------------
{
		return(I2("\nCalculations will be interrumped.",
		          "\nEl cálculo será interrrumpido."));
}

//--------------------------------------------------------------------
  static void UnknownFoundMsg(const BText& id, const BDate& d0)
//--------------------------------------------------------------------
{
  Error(I2("Unknown date found while TimeSet",
           "Se encontró la fecha desconocida construyendo la cache de TimeSet")+
        " "+id+" "+
        I2("cache building as successor of",
           " como el sucesor de ")+ d0 +InterruptMsg());
};

//--------------------------------------------------------------------
  static void NonProgressiveMsg(const BText& id, const BDate& d0, const BDate& d1)
//--------------------------------------------------------------------
{
  Error(I2("Non progressive TimeSet","Conjunto temporal no progresivo")+
        " "+id+"\n"+
        I2("Successor of ",
		       "El sucesor de ")+ d0 +
		    I2(" cannot be "," no puede ser ") + d1 +InterruptMsg());
};

//--------------------------------------------------------------------
  static bool TestHashProgress(const BText& id, BReal old, BReal h, const BDate& d1)
//--------------------------------------------------------------------
{
  BDate d0; 
  d0.PutHash(old);
  bool ok = true;
  if(d1.IsUnknown()) 
  { 
    UnknownFoundMsg   (id, d0    ); 
    ok = false; 
  }
  else if(h<=old)	       
  { 
    NonProgressiveMsg (id, d0, d1); 
    ok = false; 
  }
  else if(!d1.HasValue()) 
  { 
    ok=false; 
  }
  return(ok);
}

#ifndef NDEBUG
//--------------------------------------------------------------------
  static void CheckCache(const BTimeSet* tms)
// Checks if internal cache is well formed
//--------------------------------------------------------------------
{
  BDate beg = tms->BeginCache();
  BDate end = tms->EndCache();
  BDate inf = tms->Inf();
  BDate sup = tms->Sup();
  BBool isEmpty = (inf==BDate::End()) && (sup==BDate::Begin());
  if(!isEmpty)
  {
    assert(!beg.HasValue() || (beg>=inf));
    assert(!end.HasValue() || (end<=sup));
  }
};
#else
#define CheckCache(tms)
#endif

//--------------------------------------------------------------------
  BTimeSet::BTimeSet() 
//--------------------------------------------------------------------
: hashStatTimer_        (), 
  inf_                  (BDate::Unknown()),
  sup_                  (BDate::Unknown()),
  currentCalcFirstDate_ (BDate::Unknown()),
  currentCalcLastDate_  (BDate::Unknown())
{
}

//--------------------------------------------------------------------
const BDate& BTimeSet::Inf() const 
//--------------------------------------------------------------------
{ 
  if(inf_.IsUnknown()) 
  { ((BTimeSet*)this)->inf_ = CalcInf(); } 
  return(inf_); 
}

//--------------------------------------------------------------------
const BDate& BTimeSet::Sup() const 
//--------------------------------------------------------------------
{ 
  if(sup_.IsUnknown()) 
  { ((BTimeSet*)this)->sup_ = CalcSup(); } 
  return(sup_); 
}

//--------------------------------------------------------------------
  void BTimeSet::ForceCache(BDate first, BDate last) const
//--------------------------------------------------------------------
{
  BTimeSet* NCThis = (BTimeSet*)(this);
  BHash hash;
  bool old = ForcingCache();
  NCThis->PutForcingCache(true);
  GetHashBetween(hash, first, last);
  NCThis->PutForcingCache(old);
}

//--------------------------------------------------------------------
void BTimeSet::PutCache(const BHash& hash,
                        const BDate& begin, 
                        const BDate& end)
//--------------------------------------------------------------------
{
  int s = hash.Size();
#ifndef NDEBUG
  assert(s);
  BDate a  = HashToDte(hash[0  ]);
  BDate b  = HashToDte(hash[s-1]);
  BDate a1 = Predecessor(a);
  BDate b1 = Successor  (b);
  assert(a    .HasValue());
  assert(b    .HasValue());
  assert(begin.HasValue());
  assert(end  .HasValue());
  assert(begin <=a);
  assert(begin > a1);
  assert(end   >=b);
  assert(end   < b1);
#endif
#ifdef BTimeSetHashStat
  static FILE* stat_ = NULL;
  if(!stat_)
  {
    BText path = BDir::GetCurrent()+"/"+"BTimeSet.Hash.Stat.bst";
    stat_ = fopen(path.String(),"w");
    assert(stat_);
    Std(BText("\nCreated BTimeSetHashStat file ")+path.String()+"\n");
    fprintf(stat_,"%s","BTimeSetCacheStatStr;TimeSetExpr;Force;NRefs;Calls;First;Last;CacheLength;OldCacheKBytes;CacheKBytes;CumAllCacheKBytes;MiliSeconds;Speed;\n");
  }
#endif
  double msec  = BTimer::ClockToMiliSecond(hashStatTimer_.Clk());
  double mem   = hash.Size()*sizeof(double)/1024.0;
  double speed = mem/msec;
  BUserTimeSet* u = (BUserTimeSet*)this;
  if(
    //true ||             
      ForcingCache()        || 
      (msec>0)              || 
      (u->NRefs()>1)        ||
      (CacheCallsNumber()>1)
    )
  {
    PutBeginCache(begin);
    PutEndCache(end);
    double oldKB = double(Cache().MaxSize()*sizeof(double))/1024.0;
    SetCache(hash);
  //Std(BText("\n")+GetIdentify()+"->PutCache["<<Cache().Size()<<"]\n");
    double newKB = double(Cache().MaxSize()*sizeof(double))/1024.0;
    hashUsedKBytes_ += newKB-oldKB;
#ifdef BTimeSetHashStat
    BText id = GetIdentify();
    if(id.Length()>255) { id = id.SubString(0,251)+" ..."; }
    fprintf(stat_,
						";\"%s\";%d;%ld;%ld;%.6lf;%.6lf;%ld;%.15lg;%.15lg;%.15lg;%.15lg;%.15lg;\n",
						id.String(), ForcingCache(), u->NRefs(), cacheCallsNumber_, 
						hash[0],hash[hash.Size()-1], Cache().Size(), oldKB, newKB, 
						hashUsedKBytes_, msec, speed);
#endif
  }
  CheckCache(this);
}


//--------------------------------------------------------------------
void BTimeSet::GetHashBetween(BHash& hash, BDate first, BDate last) const
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
  BTimeSet* NCThis = (BTimeSet*)(this);
  NCThis->PutBuildingCache(BTRUE);
  NCThis->AddCacheCallsNumber();
  NCThis->hashStatTimer_.Reset();
  const BHash& cache = NCThis->Cache();
  assert(&cache);
  BDate f = first;
  BDate l = last;
  int oldCacheSize = cache.Size();
  BDate oldC1, oldCS;
  if(oldCacheSize)
  {
    oldC1 = HashToDte(cache[0]);
    oldCS = HashToDte(cache[oldCacheSize-1]);
  }
  if(first<Inf()) { first = Inf(); }
  if(last >Sup()) { last  = Sup(); }
  if(!first.HasValue()) { first = BDate::DefaultFirst(); }
  if(! last.HasValue()) {  last = BDate::DefaultLast (); }  
  NCThis->currentCalcFirstDate_ = first;
  NCThis->currentCalcLastDate_  = last;
  BDate bc = BeginCache();
  BDate ec = EndCache  ();
  if(!bc.HasValue() || (bc>first)) { bc = first; }
  if(!ec.HasValue() || (ec<last )) { ec = last;  }

//Std(BText("\n")+GetIdentify()+"->GetHashBetween("<<f<<","<<l<<")\n");
//Std(BText("\n  Interval:(")<<first<<","<<last<<")\n");
//Std(BText("\n  OldCache:[")<<oldCacheSize<<"] ("<<BeginCache()<<","<<EndCache()<<") \n"); 
//if(oldCacheSize) { Std(BText(" ->{")<<oldC1<<","<<oldCS<<") \n"); }
  if(!first.HasValue() || !last.HasValue() || (first>last))
  {
    hash.DeleteBuffer();
  }
  else
  {
    first = FirstNoLess(first);
    if(first.HasValue()) { last	= FirstNoGreat (last ); }
    if(!first.HasValue() || !last.HasValue() || (first>last))
    {
      hash.DeleteBuffer();
    }
    else
    {
      double hFirst = first.Hash();
      double hLast  = last .Hash();
      int S = cache.Size();
      if(!S)
      {
        NCThis->CalcHashBetween(hash, first, last);
        NCThis->PutCache(hash, bc, ec);
      }
      else 
      { 
        BDate  pc1;
        BDate  c1   = HashToDte(cache[0  ]);
        BDate  cS   = HashToDte(cache[S-1]);
        BDate  min  = Minimum(c1,first);
        BDate  max  = Maximum(cS,last);
        double hC1  = cache[0];
        double hCS  = cache[S-1];
        double hMin = min.Hash();
        double hMax = max.Hash();
        BHash result;
        const BHash *resultRef=&cache;
        BHash left(0), right(0);
        if((hFirst<hC1)||(hCS<hLast))
        {
          if(hMin<hC1)
          {
            pc1 = Predecessor(c1);
            assert(min<=pc1);
            NCThis->CalcHashBetween(left, min, pc1);
          }
          if(hMax>hCS)
          {
            BDate cS_ = Successor(cS);
            NCThis->CalcHashBetween(right, cS_, max);
          }
          result.AllocBuffer(left.Size()+cache.Size()+right.Size());
          int k, n;
          for(n=k=0; k<left .Size(); k++, n++) { result[n] = left [k]; }
          for(  k=0; k<cache.Size(); k++, n++) { result[n] = cache[k]; }
          for(  k=0; k<right.Size(); k++, n++) { result[n] = right[k]; }

          NCThis->PutCache(result, bc, ec);
          resultRef = &result;
        }
        const BHash& rr=*resultRef;
        int nFirst = rr.FindSorted(hFirst, BRealOrderCriterium);
        int nLast  = rr.FindSorted(hLast,  BRealOrderCriterium);
        assert((nFirst>=0)&&(nLast>=0)&&(nLast>=nFirst));
        hash.AllocBuffer(nLast-nFirst+1);
        memcpy(hash.GetBuffer(),rr.Buffer()+nFirst,hash.Size()*sizeof(double));
      }
    }
  }

  if(ForcingCache() && !cache.Size())
  {
    if(BeginCache().IsUnknown() || (CurrentCalcFirstDate()<BeginCache())) { NCThis->PutBeginCache(CurrentCalcFirstDate()); }
    if(EndCache()  .IsUnknown() || (CurrentCalcLastDate() >EndCache()  )) { NCThis->PutEndCache  (CurrentCalcLastDate ());  }
  }
//Std(BText("\n  NewCache:[")<<cache.Size()<<"] ("<<BeginCache()<<","<<EndCache()<<")");
//if(cache.Size()) { Std(BText(" ->{")<<HashToDte(cache[0])<<","<<HashToDte(cache[cache.Size()-1])<<") \n"); }
  NCThis->currentCalcFirstDate_  = BDate::Unknown();
  NCThis->currentCalcLastDate_   = BDate::Unknown();
  NCThis->PutBuildingCache(BFALSE);
}

//--------------------------------------------------------------------
void BTimeSet::CalcHashBetween(BHash& hash, BDate first, BDate last)
//--------------------------------------------------------------------
{
  CheckCache(this);
//VBR: Linux compilers GCC and Intel don't initilize static variables in methods
//In this case static is still not needed and will be skiped
//static int sizeAvr = 100;
  int sizeAvr = 100;
  hash.ReallocBuffer(sizeAvr);
  BDate dte = first;
  int n;
  double h	= first.Hash(), hLast = last.Hash();
  bool end = false;
  for(n=0; !end; )
  {
    if(n>=hash.Size()) { hash.ReallocBuffer(2*n); }
    hash[n++]=h;
    if(!(end=(h>=hLast)))
    {
    //Std(BText("\nhash Successor(")+dte+") = ");
      dte+=this;
    //Std(BText("")+dte);
      BReal old = h;
      h=dte.Hash();
      if(!TestHashProgress(GetIdentify(), old, h, dte)) { break; }
    //Std(BText("\nnewHash[")+n+"]="+BDat(h).Format("%.6lf"));
    }
  }
  hash.ReallocBuffer(n);
  sizeAvr = Minimum(10,(int)(.95*sizeAvr+.05*n));
}

//--------------------------------------------------------------------
BBool BTimeSet::Contain (const BDate& dte) const
//--------------------------------------------------------------------
{
  CheckCache(this);
  BBool ok;
  if(BuildingCache())
  {
    ok = Includes(dte); 
  }
  else
  {
    const BHash& cache = Cache();
    BBool okHash = false;
    BInt  pos;
    ok = HashIncludes(dte.Hash(), okHash, pos);
    if(!okHash) 
    { 
      ok = Includes(dte); 
    }
  }
  return(ok);
}

//--------------------------------------------------------------------
BInt BTimeSet::HashFindDate(BReal h, BBool& okHash) const
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
  if(BeginCache().IsUnknown() || EndCache().IsUnknown() ) 
  { 
    okHash = false; 
    return(-1); 
  }
  const BHash& cache = Cache();
  okHash = (BeginCache().Hash()<=h) && (h<=EndCache().Hash());
  BInt n = -1;
  if(cache.Size() && okHash)
  {
	  n = cache.FindSorted(h, BRealOrderCriterium);
  }
  return(n);
}


//--------------------------------------------------------------------
BBool BTimeSet::HashIncludes(BReal h, BBool& okHash, BInt& pos) const
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
  pos = HashFindDate(h, okHash);
  return(pos!=-1);
}


//--------------------------------------------------------------------
  int BTimeSet::HashSucc(const BHash& hash, BReal h)
//--------------------------------------------------------------------
{
  int S = hash.Size();
  if(S&&(h<hash[0])) { return(0); }
  bool okHash = S && (h<hash[S-1]);
  int pos = -1;
  if(okHash)
  {
	  BInt m=hash.Size()-1, n=0, i=0, k;
	  for(; (m-n)>1 ; i++)
	  {
	    k = (m+n) / 2;
	    if(h>=hash[k]) { n = k; }
	    else	         { m = k; }
   	}
    pos = n+1;
    assert((hash[n]<=h)&&(h<hash[pos]));
  }
  return(pos);
}


//--------------------------------------------------------------------
  int BTimeSet::HashPred(const BHash& hash, BReal h)
//--------------------------------------------------------------------
{
  int S = hash.Size();
  if(S && (h>hash[S-1])) { return(S-1); }
  bool okHash = S && (h>hash[0]);
  int pos = -1;
  BDate d;
  if(okHash)
  {
	  BInt m=hash.Size()-1, n=0, i=0, k;
	  for(; (m-n)>1 ; i++)
	  {
	    k = (m+n) / 2;
	    if(h<=hash[k]) { m = k; }
	    else	         { n = k; }
	  }
    pos = m-1;
    assert((hash[pos]<h)&&(h<=hash[m]));
  }
  return(pos);
}


//--------------------------------------------------------------------
BDate BTimeSet::HashSuccessor(BReal h, BBool& okHash, BInt& pos) const
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
  const BHash& cache = Cache();
  okHash = !BuildingCache() && cache.Size() && 
           (h>=cache[0]) && (h<cache[cache.Size()-1]);
  BDate d;
  if(okHash)
  {
    pos = HashSucc(cache, h);
    d.PutHash(cache[pos]);
  }
  return(d);
}


//--------------------------------------------------------------------
BDate BTimeSet::HashPredecessor(BReal h, BBool& okHash, BInt& pos) const
//--------------------------------------------------------------------
{
  CheckCache(this);
  const BHash& cache = Cache();
  okHash =  !BuildingCache() && cache.Size() && 
            (h>cache[0]) && (h<=cache[cache.Size()-1]);
  BDate d;
  if(okHash)
  {
    pos = HashPred(cache, h);
    d.PutHash(cache[pos]);
  }
  return(d);
}

//--------------------------------------------------------------------
BBool BTimeSet::IsCompatibleWith(const BTimeSet& tms,
				 BDate	   first,
				 BDate	   last	 ) const
//--------------------------------------------------------------------
{
  CheckCache(this);
  if(!first.HasValue()) { first = BDate::DefaultFirst(); }
  if(!last .HasValue()) { last	= BDate::DefaultLast (); }
  BDate d1  = FirstNoGreat(first);
  BDate d2 = tms.FirstNoGreat(first);
  while(d1<=last)
  {
    d1 = Successor(d1);
    d2 = tms.Successor(d2);
    if(d1!=d2) { return(BFALSE); }
  }
  return(BTRUE);
}


//--------------------------------------------------------------------
BDate BTimeSet::FirstNoLess(const BDate& dte) const

/*! Returns the first date belonging to the set, from "dte"onward.
 */
//--------------------------------------------------------------------
{
  CheckCache(this);
  if(!dte.HasValue() || Contain(dte)) 
  { 
    return(dte); 
  }
  else				                  
  { 
    return(Successor(dte)); 
  }
}


//--------------------------------------------------------------------
BDate BTimeSet::FirstNoGreat(const BDate& dte) const

/*! Returns the first date belonging to the set, from "dte" backwards.
 */
//--------------------------------------------------------------------
{
  CheckCache(this);
  if(!dte.HasValue() || Contain(dte)) 
  { 
    return(dte); 
  }
  else				
  { 
    return(Predecessor(dte)); 
  }
}


//--------------------------------------------------------------------
BInt BTimeSet::Difference(const BDate& dte1,
			  const BDate& dte2) const

/*! Determines the number of days between two dates in the time set.
 *  This is an specific implementation tha can be used only when the
 *  time set is known to be an numerable set (this ensures that
 *  Next() message is allways defined).
 *  This implementation is called by the Difference() virtual
 *  function of most of predefined time sets: BTmsWeekDay, BTmsMonth,
 *  BTmsDayMonth, ... besause all thay are numerable sets.
 *
 * ALGORITHM:
 *   dte <- dte1 or next(dte1) if dte1 is not included in the time set.
 *  while dte < dte2:
 *  - count the date.
 *  - pass to the next date in the time set.
 */
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
  if(!(dte1.HasValue()&&dte2.HasValue())) { return(0); }
  if (dte1>dte2)			  { return(-Difference(dte2,dte1)); }
  if (dte1==dte2)			  { return(0); }
  if(!(dte1.HasValue()&&dte2.HasValue())) { return(-1); }
  if(dte1>Sup()) { return(0); }
  if(dte2<Inf()) { return(0); }
  if((Inf()==BDate::End())&&(Sup()==BDate::Begin())) { return(0); }
  if(!Cache().Size() && !BeginCache().IsUnknown() && (BeginCache()<=dte1) && (dte2<=EndCache())) { return(0); }
//Std(BText("\nCalculating Difference(")+dte1+", "+dte2+")");
  BDate d1 = FirstNoLess (dte1);
  BDate d2 = FirstNoGreat(dte2);

//Std(BText("\nCalculate between(")+d1+", "+d2+")");

  BInt dif=0;
  BBool ok = BFALSE;
  if (d1>d2)
  {
    return(dif);
  }
  if (d1==d2)
  {
    dif = 0;
    ok = BTRUE;
  }
  else if(Cache().HasValue())
  {
    BReal h1 = d1.Hash();
    BReal h2 = d2.Hash();
    if((h1>=Cache()[0])&&(h2<=Cache()[Cache().Size()-1]))
    {
      BInt n1 = Cache().FindSorted(h1, BRealOrderCriterium);
      BInt n2 = Cache().FindSorted(h2, BRealOrderCriterium);
    //Std(BText("\nFinding in hashed time set (") + h1 + ", " +h2+") -> ("+n1 + ", " +n2+") -> (" +Cache()[n1]+", "+Cache()[n2]+")");
      if((n1>=0)&&(n2>=0))
      {
	      dif = n2-n1;
	      ok = BTRUE;
      }
      else
      {
	      Error(I2(" FATAL : Difference function fail for TimeSet",
		             " FATAL : Fallo en función diferencia de TimeSet")+
                 " "+GetIdentify()+" ");
	      return(0);
      }
    }
  }
  if(!ok)
  {
    BTimeSet* tms = (BTimeSet*)this; //Cast problem with gcc
  //Std(BText("\nGetting hash for time set between ") + d1 + " and " +d2);
    BHash hash; tms->GetHashBetween(hash, d1, d2);
    dif = hash.Size()-1;
  //Std(BText(" = ")+dif);
  }
//Std(BText("\nDifference(")+d1+", "+d2+")="+dif);
  if((d1!=dte1)||(d2!=dte2)) { dif++; }
//Std(BText("\nDifference(")+dte1+", "+dte2+")="+dif);
  return(dif);
}


//--------------------------------------------------------------------
BInt BTimeSet::PeriodsBetween(const BDate& dte1,
			      const BDate& dte2) const

/*! Determines the number of periods between two dates in the time
 *  set. Periods are determined by the min date of a period.
 *  This message can only be used when the time set is known to be an
 *  numerable set (this ensures that Next() message is allways
 *  defined).
 *  This message is used when a time set has semantic of dated, and
 *  in this case, the time set always must be numerable.
 *
 * ALGORITHM:
 *   dte <- dte1.
 *   while dte < dte2:
 *   - count the date.
 *   - pass to the next date in the time set.
 */
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
  if(!(dte1.HasValue()&&dte2.HasValue())) { return(-1); }
  if (dte1>dte2)			  { return(-1); }
  BDate d1 = FirstNoLess (dte1);
  BDate d2 = FirstNoGreat(dte2);
  BInt p;
  BBool ok = BFALSE;
  if(Cache().HasValue())
  {
    BReal h1 = d1.Hash();
    BReal h2 = d2.Hash();
    if((h1>=Cache()[0])&&(h2<=Cache()[Cache().Size()-1]))
    {
      ok = BTRUE;
      BInt n1 = Cache().FindSorted(h1, BRealOrderCriterium);
      BInt n2 = Cache().FindSorted(h2, BRealOrderCriterium);
      p = n2-n1+1;
    }
  }
  if(!ok)
  {
    BTimeSet* tms = (BTimeSet*)this; //Cast problem with gcc
    BHash hash; tms->GetHashBetween(hash, d1, d2);
    p = hash.Size();
  }
  if(d2==dte2) { p--; }

  return(p);
}

//--------------------------------------------------------------------
BDate BTimeSet::Next(const BDate& dte, BInt nth) const

/*! Determines the following date in any time.
 */
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
  if(nth == 0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  if(nth <  0) { return(Prev(dte, -nth)); }
  const BHash& cache = Cache();

  BInt  N      = cache.Size();
  BReal hash   = dte.Hash();
  BBool okHash = !BuildingCache() && Cache().Size() && 
                 (hash>=cache[0]) && (hash<cache[N-1]);
  BDate d(dte);
  if(okHash)
  {
    BInt pos;
    d = HashSuccessor(hash, okHash, pos);
    nth--;
    if(pos+nth<N)
    {
      d.PutHash(cache[pos+nth]);
      nth = 0;
    }
    else
    {
      d.PutHash(cache[N-1]);
      nth -= (N-1-pos);
    }
  }
  for ( ; !d.IsUnknown() && ! d.IsTheEnd() && (nth>0); nth--)
  { 
 
    d = Successor(d); 
  }
  return(d);
}



//--------------------------------------------------------------------
BDate BTimeSet::Prev(const BDate& dte, BInt nth) const

/*! Determines the previous date in any time set.
 */
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
  if(nth == 0) { return((!dte.HasValue()||Includes(dte))?dte:BDate::Unknown()); }
  if(nth <  0) { return(Next(dte, -nth)); }
  const BHash& cache = Cache();
  BInt  N      = cache.Size();
  BReal hash   = dte.Hash();
  BBool okHash = !BuildingCache() && cache.Size() && 
                 (hash>cache[0]) && (hash<=cache[N-1]);
  BDate d(dte);
  if(okHash)
  {
    BInt pos;
    d = HashPredecessor(hash, okHash, pos);
    nth--;
    if(pos-nth>=0)
    {
      d.PutHash(cache[pos-nth]);
      nth = 0;
    }
    else
    {
      d.PutHash(cache[0]);
      nth -= pos;
    }
  }

  for ( ; !d.IsUnknown() && ! d.IsTheBegin() && (nth>0); nth--)
	 { 
    d = Predecessor(d); 
  }
 	return(d);
}


//--------------------------------------------------------------------
BText BTimeSet::ListOfDates(BDate dte1, BDate dte2, BText sep, BInt jump)

/*! Returns a BText that contains all the dates of this time set
 *  between dte1 and dte2
 */
//--------------------------------------------------------------------
{
  assert(!BuildingCache());
  CheckCache(this);
    if(!dte1.HasValue() || !dte2.HasValue() || (dte1>dte2)) { return("()"); }
    BDate dte = FirstNoLess(dte1);
    BText txt = "(";
    for(BInt n=0; (dte.HasValue()) && (dte<=dte2); n++)
    {
	if(n>0) { txt += sep; }
	txt += dte.Name();
	if ((jump>0) && !(n % jump)) { txt << "\n"; }
	dte = Successor(dte);
    }
    txt += ")";
    return(txt);
}


//--------------------------------------------------------------------
  const BText& BTimeSet::TestError(const BText& cod, 
                                   const BText& msg, 
                                   const BText& interval)
//--------------------------------------------------------------------
{
  static BText txt;
  txt = BText("E")+cod+" [TimeSet TestCoherence FAIL. "+GetIdentify()+"] : "+msg;
  Error(txt);
  FreeCache(); 
  return(txt);
}

//--------------------------------------------------------------------
  const BText& BTimeSet::TestAborted(const BText& cod, 
                                     const BText& msg, 
                                     const BText& interval)
//--------------------------------------------------------------------
{
  static BText txt;
  txt = BText("A")+cod+" [TimeSet TestCoherence ABORTED. "+GetIdentify()+"] : "+msg;
  Warning(txt);
  FreeCache(); 
  return(txt);
}

#define CheckTestAborted(t0,maxTimeSim,interval) \
if(AbortedMessageSended()) \
{ \
  return(TestAborted("0001",msg=BText("aborted ")+maxTimeSim+" seconds.",interval)); \
} \
else if(time(0)-t0>maxTimeSim) \
{ \
  return(TestAborted("0002",msg=BText("exceeded maximum simulation time of ")+maxTimeSim+" seconds.",interval)); \
} 

//--------------------------------------------------------------------
  BText BTimeSet::TestCoherence(      int    verbose, 
                                const BDate& first, 
                                const BDate& last,
                                      double maxTimeSim, 
                                      int    numSim, 
                                      int    maxLag)
//--------------------------------------------------------------------
{
  double t0 = (double)time(0);
  BText id = GetIdentify();
  BText interval = BText("[")+first+","+last+"]";
  BText msg = BText("E0000 [TimeSet TestCoherence SUCCESS for ")+id+"] "+interval;
  int n,I,S,N;
  BDate suc, pre, old, dte = first;
  BHash included, cachedHash; 

  FreeCache(); 
  if(verbose>0)
  Std(BText("\n  TestCoherenceOfTimeSet Step 1: Building included-based cache between ")+interval+"... ");
  N = (int)(last-first)+1;
  included.ReallocBuffer(N);
  for(n=0; dte<=last; dte=dte+1)
  {
    if(Includes(dte)) 
    { 
      included[n++] = dte.Hash(); 
      if(verbose>1)
      Std(BText("\n    included[")+n+"]="+dte);
    } 
    else
    {
      if(verbose>2)
      Std(BText("\n    NOT included ")+dte);
    }
    CheckTestAborted(t0,maxTimeSim,interval);
  }
  included.ReallocBuffer(I=n);

  if(verbose>0)
  Std(BText("\n  TestCoherenceOfTimeSet Step 2: Testing integrity of GetHashBetween method among ")+n+" found dates ... ");
  GetHashBetween(cachedHash, first,last);
  CheckTestAborted(t0,maxTimeSim,interval);
  if(verbose>0)
  Std(BText("\n    BTimeSetHashUsedKBytes = ")+hashUsedKBytes_/1024+" Kb\n");
  S = cachedHash.Size();
  if(!S && !I) { return(msg+" but is EMPTY in interval "); }
  for(n=0; (n<S)&&(n<I); n++)
  {
    dte = HashToDte(cachedHash[n]);
    if(dte.Hash()!=included[n])
    {
      return(TestError("0202",msg=BText("inconsistent methods Includes and GetHashBetween at ")+dte,interval));
    }
    if(verbose>1)
    Std(BText("\ncachedHash[")+n+"]="+dte+" is OK");
    CheckTestAborted(t0,maxTimeSim,interval);
  }
  if(S>I) 
  { 
    if(verbose>1)
    for(; n<S; n++)
    {
      dte = HashToDte(cachedHash[n]);
      Std(BText("\ncachedHash[")+n+"]="+dte+" is NOT included");
      CheckTestAborted(t0,maxTimeSim,interval);
    }
    return(TestError("0203",msg=BText("inconsistent methods Includes and GetHashBetween that returns ")+S+" dates instead of "+I,interval));
  }
  if(S<I) 
  { 
    if(verbose>1)
    for(; n<I; n++)
    {
      dte = HashToDte(included[n]);
      Std(BText("\nincluded[")+n+"]="+dte+" is NOT cached");
      CheckTestAborted(t0,maxTimeSim,interval);
    }
    return(TestError("0204",msg=BText("inconsistent methods Includes and GetHashBetween that returns ")+S+" dates instead of "+I,interval));
  }
/*
  void NullClk();
  void ShowClk();
  NullClk();
*/

  FreeCache();
  if(verbose>0)
  Std(BText("\n  TestCoherenceOfTimeSet Step 3: Testing integrity of method Successor ... "));
  dte = HashToDte(cachedHash[0]);
  for(n=1; n<S; n++)
  {
  //if((id=="aux")&&(n==9))
  //printf("");
  	dte = Successor(old=dte);
    if(verbose>1)
    Std(BText("\n    Successor(")+old+")="+dte);
    if(dte.HasValue() && !Includes(dte))
    {
      return(TestError("0301",msg=BText("inconsistent methods Includes and Successor at (")+old+","+dte+")",interval));
    }
    if(dte.Hash()!=cachedHash[n])
    {
      return(TestError("0302",msg=BText("inconsistent methods GetHashBetween and Successor at ")+old+"->"+dte+"!="+HashToDte(cachedHash[n]),interval));
    }
  //Std(BText("\nSuccessor(")+old+") = "+dte);
    CheckTestAborted(t0,maxTimeSim,interval);
  }
//ShowClk();

  FreeCache();
  if(verbose>0)
  Std(BText("\n  TestCoherenceOfTimeSet Step 4: Testing integrity of method Predecessor ... "));
  dte = HashToDte(cachedHash[S-1]);
  for(n=S-2; n>=0; n--)
  {
  //if(n==17)
  //printf("");
  	dte = Predecessor(old=dte);
    if(verbose>1)
    Std(BText("\n    Predecessor(")+old+")="+dte);
    if(dte.HasValue() && !Includes(dte))
    {
      return(TestError("0401",msg=BText("inconsistent methods Includes and Predecessor at (")+old+","+dte+")",interval));
    }
    if(dte.Hash()!=cachedHash[n])
    {
      return(TestError("0302",msg=BText("inconsistent methods GetHashBetween and Predecessor at ")+old+"->"+dte+"!="+HashToDte(cachedHash[n]),interval));
    }
  //Std(BText("\nPredecessor(")+old+") = "+dte);
  //ShowClk();
    CheckTestAborted(t0,maxTimeSim,interval);
  }
//NullClk();

  FreeCache();
  if(verbose>0)
  Std(BText("\n  TestCoherenceOfTimeSet Step 5: Testing integrity of method Difference ... "));
  int d = Difference(HashToDte(cachedHash[0]),HashToDte(cachedHash[S-1]));
  CheckTestAborted(t0,maxTimeSim,interval);
  if(d!=S-1)
  {
    return(TestError("0501",msg=BText("inconsistent methods GetHashBetween and Difference that returns ")+d+" instead of "+(S-1),interval));
  }
  FreeCache();
  if(verbose>0)
  Std(BText("\n  TestCoherenceOfTimeSet Step 6: Testing progression and regression at random dates ... "));
  double idxFirst  = first.Index();
  double idxLast   = last .Index();
  double idxRange  = idxLast - idxFirst;
  
  double rnd, h, s;
  int    lag, pos,p1;
  if(maxLag>=S) { maxLag = S-1; }
  for(n=0; n<numSim; n++)
  {
  //if(n==44)
  //printf("");
    double idx0, idx1, r = BUniformDist::Random01().Value(); 
    lag = (int)round(maxLag*(2.0*r-1.0));
    r = BUniformDist::Random01().Value();
   
    if(lag<0) 
    {
      idx0 = HashToDte(included[-lag]).Index();
      idx1 = last.Index();
    }
    else if(lag>0) 
    {
      idx0 = first.Index();
      idx1 = HashToDte(included[S-lag-1]).Index();
    }
    else
    {
      idx0 = first.Index();
      idx1 = last.Index();
    }
    rnd = idx0*r+idx1*(1.0-r);
    dte.PutIndex(rnd); 
    h   = dte.Hash();
  //NullClk();
    suc = Next(dte, lag);
  //ShowClk();
    s   = suc.Hash();
    if(verbose>1)
    Std(BText("\n    ")+n+") Next("+dte+","+lag+")="+suc);
    if(lag>0)
    { 
      if(suc.IsUnknown())
      {
        return(TestError("0602",msg=BText("method (")+lag+")-Next returns unkown date for "+dte,interval));
      }
      else if(suc<=dte)
      {
        return(TestError("0603",msg=BText("method (")+lag+")-Next is non progressive for "+dte+"->"+suc,interval));
      }
      else if((h>cachedHash[0]) && (h<cachedHash[S-1]))
      { 
        p1  = HashSucc(included, h);
        pos = p1+lag-1; 
        if(p1<0)
        {
          return(TestError("0604",msg=BText("method HashSucc returns unkown date for ")+dte,interval));
        }
        else if((pos>=S) && !suc.IsTheEnd())
        {
          return(TestError("0605",msg=BText("inconsistent methods (")+lag+")-Next and HashSucc for "+dte+"->"+suc+"!="+HashToDte(included[pos]),interval));
        }
        else if((pos<S) && (included[pos]!=s))
        {
          return(TestError("0606",msg=BText("inconsistent methods (")+lag+")-Next and HashSucc for "+dte+"->"+suc+"!="+HashToDte(included[pos]),interval));
        }
      }
    }
    else if(lag<0)
    {
      if(suc.IsUnknown())
      {
        return(TestError("0607",msg=BText("method (")+lag+")-Next returns unkown date for "+dte,interval));
      }
      else if(suc>=dte)
      {
        return(TestError("0608",msg=BText("method (")+lag+")-Next is non regressive for "+dte+"->"+suc,interval));
      }
      else if((h>cachedHash[0]) && (h<cachedHash[S-1]))
      { 
        p1 = HashPred(included, h); 
        pos = p1+lag+1;
        if(p1<0)
        {
          return(TestError("0609",msg=BText("method HashPred returns unkown date for ")+dte,interval));
        }
        else if((pos<0) && !suc.IsTheBegin())
        {
          return(TestError("0611",msg=BText("inconsistent methods (")+lag+")-Next and HashPred for "+dte+"->"+suc+"!="+HashToDte(included[pos]),interval));
        }
        else if((pos>=0) && (included[pos]!=s))
        {
          return(TestError("0611",msg=BText("inconsistent methods (")+lag+")-Next and HashPred for "+dte+"->"+suc+"!="+HashToDte(included[pos]),interval));
        }
      }
    }
    CheckTestAborted(t0,maxTimeSim,interval);
  }
  FreeCache(); 
  return(msg);
}


