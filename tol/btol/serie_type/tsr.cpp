/* tsr.cpp: Messages that support algebra.
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

#include <tol/tol_btmser.h>
#include <tol/tol_bout.h>
#include <tol/tol_butil.h>
#include <tol/tol_bsetgra.h>

BTraceInit("tsr.cpp");

BDat  BTimeSerie::globalizeSeries_ = 1;
BDat& BTimeSerie::GlobalizeSeries() { return(globalizeSeries_); }


//--------------------------------------------------------------------
BTimeSerie::BTimeSerie()
//--------------------------------------------------------------------
:	data_      (),
	dating_    (NIL),
#if(__USE_POOL__==__POOL_BFSMEM__)
  datingMai_(NULL),
#endif    
	firstCache_(BDate::Unknown()),
	firstDate_ (BDate::Begin()),
	lastDate_  (BDate::End()),
	length_    (0),
	datingType_(DATING_FIXED),
  compacted_ (false)
{
}


# if(__USE_POOL__==__POOL_BFSMEM__)
//--------------------------------------------------------------------
  void BTimeSerie::CheckRefDating()
//--------------------------------------------------------------------
{
  if(!dating_) { return; }
//VBR: Si se usa el manejador de memoria es posible detectar cuando
//     un objeto ya ha sido borrado para evitar accesos inválidos
//Bugs afectados: 224, 428, 444, 456
  short assigned = datingMai_.IsAssigned();
  assert(assigned>=0);
  if(assigned==1)
  {
  //Std(BText("\n[BTimeSerie::CheckRefDating()] Decrementing references for dating ")+
  //    "["+TxtFmt((int)this,"%X")+","+TxtFmt((int)dating_,"%08X")+"] ");
    assert(dating_);
    assert(!dating_->System());
    assert(!dating_->NameBlock());
    BText name = dating_->Name();
    int level = dating_->Level();
    int nrefs = dating_->NRefs();
  //assert(level || !name.HasName());
    assert(nrefs>0);
  //Std(BText(" '")+dating_->Identify()+"' NRef="<<dating_->NRefs());
    dating_->DecNRefs();
    if(!dating_->NRefs()) 
    { 
    //Std(BText(" DELETED"));
      delete dating_; 
      dating_ = NULL; 
      datingMai_.SetAddress(NULL);
    }
  }
}
# else
#   define CheckRefDating()
# endif

//--------------------------------------------------------------------
BTimeSerie::~BTimeSerie()
//--------------------------------------------------------------------
{
  CheckRefDating();
}


//--------------------------------------------------------------------
void BTimeSerie::PutDating(BUserTimeSet* dating, bool forzeRef) 
//--------------------------------------------------------------------
{ 
  if(dating == dating_) { return; }
  CheckRefDating();
  dating_ = dating;
  if(dating_ && !dating_->System() && !dating_->NameBlock() && 
    (forzeRef || dating_->Level() || !dating_->HasName()))
  {
#   if(__USE_POOL__==__POOL_BFSMEM__)
    datingMai_.SetAddress(dating_);
  //Std(BText("\n[BTimeSerie::PutDating] Incrementing references for dating ")+
  //    "["+TxtFmt((int)this,"%X")+","+TxtFmt((int)dating_,"%08X")+"] ");
  //Std(BText("  '")+dating_->Identify()+"' NRef="<<dating_->NRefs());
#   endif
    dating_->IncNRefs(); 
  }
}


//--------------------------------------------------------------------
BUserTimeSet* BTimeSerie::Dating() const

/*! Appends this BTimeSerie to a binary file.
 */
//--------------------------------------------------------------------
{
  BTimeSerie* ser = (BTimeSerie*)this; //Cast problem with gcc
  ser->ReadData();
  return(dating_);
}

//--------------------------------------------------------------------
BDate BTimeSerie::FirstCache() const
//--------------------------------------------------------------------
{
  BTimeSerie* ser = (BTimeSerie*)this; //Cast problem with gcc
  return(firstCache_);
}

//--------------------------------------------------------------------
BDate BTimeSerie::FirstDate() const
//--------------------------------------------------------------------
{
  BTimeSerie* ser = (BTimeSerie*)this; //Cast problem with gcc
  ser->ReadData();
  ser->CompactData();
  return(firstDate_);
}


//--------------------------------------------------------------------
BDate BTimeSerie::LastDate () const
//--------------------------------------------------------------------
{
  BTimeSerie* ser = (BTimeSerie*)this; //Cast problem with gcc
  ser->ReadData();
  ser->CompactData();
  return(lastDate_);
}

//--------------------------------------------------------------------
void BTimeSerie::PutFirstCache(const BDate&  firstCache) 
//--------------------------------------------------------------------
{
	if(Dating() && firstCache.HasValue()) 
  { 
	  firstCache_ = Dating()->FirstNoLess(firstCache); 
	}
}

//--------------------------------------------------------------------
void BTimeSerie::PutFirstDate(const BDate&  firstDate) 
//--------------------------------------------------------------------
{
	if(Dating() && firstDate.HasValue()) 
  { 
	  firstDate_ = Dating()->FirstNoLess(firstDate); 
	}
}

//--------------------------------------------------------------------
void BTimeSerie::PutLastDate(const BDate&  lastDate) 
//--------------------------------------------------------------------
{
  if(Dating()&&lastDate.HasValue())
  { 
    lastDate_ = Dating()->FirstNoGreat(lastDate); 
  }
}

//--------------------------------------------------------------------
void BTimeSerie::CompactData()
//--------------------------------------------------------------------
{
  if(compacted_) { return; }
  if(Dating() && data_.Size())
  {
    BInt n, n0=0, n1=data_.Size()-1, m1=0, d=0;
    while((n1 >= 0) && !data_[n1].IsKnown()) { n1--; m1++; d++; }
    while((n0 < n1) && !data_[n0].IsKnown()) { n0++; d++; }
    if((d>0)&&(d<data_.Size()))
    {
      length_ = n1-n0+1;
      for(n=0; n<length_; n++) { data_[n] = data_[n+n0]; }
      Realloc(length_);
      firstDate_ = Dating()->Next(firstDate_, n0);
      lastDate_  = Dating()->Prev(lastDate_,  m1);
    }
    compacted_ = true;
  }
}


//--------------------------------------------------------------------
BInt BTimeSerie::BinWrite(FILE* fil)
       
/*! Appends this BTimeSerie to a binary file.
 */
//--------------------------------------------------------------------
{
  length_ = GetLength();
  (*this)[FirstDate()];
  BInt	items = FirstDate().BinWrite(fil) +
		LastDate().BinWrite(fil) +
		fwrite(&length_,	     sizeof(BInt), 1,	    fil) +
		fwrite(Data().Buffer(), sizeof(BDat), length_, fil);
  return(items);
}


//--------------------------------------------------------------------
BInt BTimeSerie::BinRead(FILE* fil)

/*! Reads this BTimeSerie from a binary file.
 */
//--------------------------------------------------------------------
{
  BInt	items = firstDate_.BinRead(fil) +
		lastDate_. BinRead(fil) +
		fread(&length_, sizeof(BInt), 1, fil);
  Realloc(length_);
  items+=  fread(data_.GetBuffer(),  sizeof(BDat), length_, fil);
  return(items);
}

//--------------------------------------------------------------------
BInt BTimeSerie::GetIndex(const BDate& dte) const

/*! Tranforms a date in the corresponding index from the first known
 *  date of the serie.
 */
//--------------------------------------------------------------------
{
  BTimeSerie* ser = (BTimeSerie*)this; //Cast problem with gcc
  ser->ReadData();
  if(!Dating()) { return(-1); }
  BInt index = -1;

  if(FirstDate().HasValue()) index = Dating()->Difference(FirstDate(),dte);

  return(index);
}


//--------------------------------------------------------------------
BDate BTimeSerie::IndexToDate(BInt index) const

/*! tranforms an index in the corresponding date from the first known
 *	    date of the serie.
 */
//--------------------------------------------------------------------
{
  BDate dte;
  if(Dating() && FirstDate().HasValue())
  { dte = Dating()->Next(FirstDate(),index); }
  return(dte);
}


//--------------------------------------------------------------------
BInt	 BTimeSerie::GetLength()

/*! If this serie is stochastic, returns the number of its datas.
 *  Else, it returns -1.
 */
//--------------------------------------------------------------------
{
  if(!Dating()) { return(length_ = -1); }
  if(!length_)
  {
    if(IsStochastic())
    { length_= 1+Dating()->Difference(FirstDate(), LastDate()); }
    else
    { length_=0; }
  }
  if(length_<0) { length_=0; }
  return(length_);
}

//--------------------------------------------------------------------
BDat BTimeSerie::operator[](const BDate& dte)

/*! Returns all known data of the serie.
 */
//--------------------------------------------------------------------
{
  BDat  dat;
  BHash hash;
  if(!Dating()) { return(BDat::Unknown()); }
  if(!GetLength())
  {
    dat = GetDat(dte);
  }
  else if(Data().HasValue()) { dat = data_[GetIndex(dte)]; }
  else
  {
    Realloc(length_);
	  Dating()->GetHashBetween(hash, FirstDate(), LastDate());
	  BInt n=0;
	  BReal h = dte.Hash();
	  for(; n<length_; n++)
	  {
	    data_[n] = GetDat(HashToDte(hash[n]));
	    if(hash[n]==h) { dat = data_[n]; }
	  }
    compacted_ = false;
	  CompactData();
  }
  return(dat);
}


//--------------------------------------------------------------------
void BTimeSerie::PutDat(const BDate& dte, const BDat& dat)

/*! Returns all known data of the serie.
 */
//--------------------------------------------------------------------
{
  BHash hash;
  if(!GetLength() || !Dating()) { return; }
  if(!Data().HasValue())
  {
    Realloc(length_);
    Dating()->GetHashBetween(hash, FirstDate(), LastDate());
    BInt n=0;
    BReal h = dte.Hash();
    for(; n<length_; n++)
    {
      data_[n] = GetDat(HashToDte(hash[n]));
    }
    CompactData();
  }
  data_[GetIndex(dte)] = dat;
}



//--------------------------------------------------------------------
void BTimeSerie::GetData(      BData& data,
			 const BDate& firstD,
			 const BDate& lastD,
			       BInt   length)

/*! Returns all data of the serie between firstDate and lastDate.
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BTimeSerie::GetData");
  ReadData();
  compacted_ = false;
  CompactData();
  BData aux0;
  bool autoData = (&data==&data_);
  BData& dta = autoData?aux0:data;

  if(!Dating())
  {
    if(length)
    {
      dta.ReallocBuffer(length);
      BDat dat = GetDat(firstD);
      for(BInt k=0; k<length; k++) { dta[k]=dat; }
      assert(!autoData);
    }
    return;
  }
  BInt n=0, m=0;
  BDate first = firstD;
  BDate last  = lastD;
  if(!first.HasValue()) { first = BDate::DefaultFirst(); }
  if(!last.HasValue())	{ last	= BDate::DefaultLast();	 }
  if(Dating())
  {
    if(length<=0) { length = Dating()->Difference(first, last)+1; }
    if(length>0)
    {
      dta.ReallocBuffer(length);
      BDate fc = FirstDate(), fl=LastDate();
      if(!fc.HasValue() && Data().HasValue()) 
      { 
        fc = FirstCache(); 
        fl = Dating()->Next(fc,Data().Size()-1);
        if((fc!=BDate::DefaultFirst())||(fl!=BDate::DefaultLast()))
        { 
          PutFirstCache(fc=BDate::Unknown());
          Delete();
        }
      } 
      BBool cacheIsOk = Data().HasValue();
      if(cacheIsOk)
      {
        BInt ini=0;
        ini = Dating()->Difference(fc, first); 
	      for((n=ini),(m=0); m<length; n++,m++)
        {
	        if((n>=0)&&(n<Data().Size()))
          {
	          dta[m]=Data()[n];
          }
	        else
          {
	          dta[m]=BDat::Unknown();
          }
        }
      }
      else
      {
	      BHash hash; Dating()->GetHashBetween(hash, first, last);
	      for(n=0;n<hash.Size();n++)
        {
	        dta[n]=(*this)[HashToDte(hash[n])];
        }
        if(!hash.Size()) { dta.ReallocBuffer(0); }
      }
    }
    if(autoData) 
    { 
      data_ = dta; 
      compacted_ = false;
      CompactData();
      if(!FirstDate().HasValue())
      {
        PutFirstCache(first);
      }
    }
  }
  else
  {
    Error(I2("Dating, first date or last date are not well defined "
	     "in call to \nGetData(",
	     "El fechado, la fecha inicial o la final no estan bien definidas"
	     "en la llamada a \nGetData(")+
	     first.Name() + ", " + last.Name() + ")\n");
  }
//SumPartialTime;
}


