/* srg.cpp: BSerieGroup functions.
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

#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_butil.h>
#include <tol/tol_bdir.h>
#include <tol/tol_barray.h>
#include <tol/tol_bmatrix.h>

//--------------------------------------------------------------------
// Static global class variable for BUserTimeSerie list of instances.
//--------------------------------------------------------------------
BTraceInit("srg.cpp");

BData BDataLine::emptyData_;
BDateFormat  BDataLine::dateFormat_("%c%Y/%m/%d%u:%h:%i:%s");

BDateFormat BTableFormat::dateFormat_("%c%Y/%m/%d%u:%h:%i:%s");
BText	    BTableFormat::realFormat_ = "%.16lg";
BAlignType  BTableFormat::align_      =BLEFT;
BChar	    BTableFormat::fill_	      =' ';
const char*	    BTableFormat::sep_	      =" ; ";
BInt	    BTableFormat::firstLength_=12;
BInt	    BTableFormat::restLength_ =18;
BDat	    BSerieTable::formatBDT_=BFALSE;


//--------------------------------------------------------------------
BSerieGroup::BSerieGroup()
//--------------------------------------------------------------------
    :
dating_(NIL),
firstDate_(),
lastDate_(),
maxFirst_(),
minLast_(),
numDates_(0),
serArr_(0),
checkDating_(BFALSE),
volatileDating_(false)
{
}

//--------------------------------------------------------------------
  void BSerieGroup::AddList (BList* lst) 
//--------------------------------------------------------------------
{ 
  for(; lst; lst=Cdr(lst)) 
  { 
    BSyntaxObject* obj = (BSyntaxObject*)Car(lst);
    if(obj->Grammar()==GraSerie())
    {
      AddSerie(Tsr(obj)); 
    }
  } 
}

//--------------------------------------------------------------------
  void BSerieGroup::AddSet  (const BSet& set) 
//--------------------------------------------------------------------
{ 
  for(int n=1; n<=set.Card(); n++) 
  { 
    BSyntaxObject* obj = set[n];
    if(obj->Grammar()==GraSerie())
    {
      AddSerie(Tsr(obj)); 
    }
  } 
}

//--------------------------------------------------------------------
  void BSerieGroup::AddGroup(const BSerieGroup& group)
//--------------------------------------------------------------------
{ 
  for(int n=1; n<=group.NumSeries(); n++) 
  { 
    AddSerie(group[n]); 
  } 
}


//--------------------------------------------------------------------
void BSerieGroup::PrepareOperate()
//--------------------------------------------------------------------
{
  BInt	n;
  dating_      = NIL;
  numDates_    = 0;
  BDate first, last;
  maxFirst_ = BDate::Begin();
  minLast_ = BDate::End();
  if(!(NumSeries())) { return; }
  for(n=1; n<=NumSeries(); n++)
  {
    BUserTimeSerie* ser = Serie(n);
	  if(ser && (ser->Grammar()==GraSerie()))
	  {
      if(ser->IsEmpty()) 
      { 
        maxFirst_ = BDate::End();
        minLast_ = BDate::Begin();
        break;
      } 
	    first  = ser->FirstDate();
	    last   = ser->LastDate();
	    //Std(BText("\nSerie ")+n+" ("+first+","+last+")");
	    if(first.HasValue())
	    {
        if(!maxFirst_.HasValue()||(maxFirst_<first)) { maxFirst_=first; }
	    }
	    if(last.HasValue())
	    {
		    if(!minLast_.HasValue()||(minLast_>last )) {    minLast_=last; }
	    }
	    //Std(BText(" -> ")+n+" ("+maxFirst_+","+minLast_+")");
	  }
  }
    
//Std(BText(" MinMax =")+n+" ("+maxFirst_+","+minLast_+")");
  BUserTimeSet* newDating = NIL;
  BUserTimeSet* dating    = NIL;
  BBool         allNull   = BTRUE;
  int serieDatVol = 0; // if exist a serie with VOLATILE dating_, its index

  dating_      = NIL;
  checkDating_ = BTRUE;

  for(n=1; (n<=NumSeries())&&(checkDating_); n++)
  {
    BUserTimeSerie* ser = Serie(n);
	  if(ser->getDatingType()==DATING_VOLATILE) serieDatVol = n;
	  if(ser && (ser->Grammar()==GraSerie()) && n!=serieDatVol)
	  {
	    newDating = ser->Dating();
	    if(newDating)
	    {
		    allNull = BFALSE;
		    if(!dating) { dating = newDating; }
				checkDating_ = (dating == newDating) ||
		                   (dating->HasName() && 
		                   (dating->Name() == newDating->Name())) ||
		                   (dating->Description().HasName() &&
		                   (dating->Description() == newDating->Description()));
        if(!checkDating_)  
        {
          checkDating_ = dating->IsCompatibleWith(*newDating, maxFirst_, minLast_);
        }
	    }
	  }
  }
  if(!dating && serieDatVol) 
  {
	  dating = Serie(serieDatVol)->Dating();
	  volatileDating_ = true;
	  checkDating_ = BTRUE;
  }
  if(checkDating_) { dating_ = dating; }
}

//--------------------------------------------------------------------
void BSerieGroup::PrepareFill(const BDate& f, const BDate& l)

/*! BSerieGroup constructor.
 */
//--------------------------------------------------------------------
{
  checkDating_ = BFALSE;
  BInt n;
  if(!NumSeries()) { return; }
  checkDating_ = BTRUE;
  dating_      = NIL;
  firstDate_   = f;
  lastDate_    = l;
  numDates_    = 0;

  //Calculates the minimum first and maximum last dates of all series
  if(!firstDate_.HasValue() || !lastDate_.HasValue())
  {
	  BUserTimeSerie* ser = Serie(1);
	  BDate  first = ser->FirstDate();
	  BDate  last	 = ser->LastDate ();
	  firstDate_ = first;
	  lastDate_  = last;
	  maxFirst_  = first;
	  minLast_   = last;
		for(n=2; n<=NumSeries(); n++)
	  {
	    if(Serie(n) && (Serie(n)->Grammar()==GraSerie()))
	    {
		    ser = Serie(n);
		    first  = ser->FirstDate();
		    last   = ser->LastDate();
    		if(first.HasValue()) 
        {
          if(!firstDate_.HasValue() || (firstDate_>first)) { firstDate_ = first; }
          if(!maxFirst_ .HasValue() || (maxFirst_ <first)) { maxFirst_  = first; }
		    }
        if(last.HasValue()) 
        {
          if(!lastDate_.HasValue() || (lastDate_<last )) { lastDate_=last; }
          if(!minLast_. HasValue() || (minLast_ >last )) { minLast_ =last; }
		    }
	    }
	  }
  }
  else
  {
	  maxFirst_ = firstDate_;
	  minLast_  = lastDate_;
  }
  //Tests that all series have the same dating
  BUserTimeSet* newDating = NIL;
  BUserTimeSet* dating    = NIL;
  BBool	  allNull   = BTRUE;
  int serieDatVol = 0;
  dating_      = NIL;
  checkDating_ = BTRUE;
  for(n=1; (n<=NumSeries())&&(checkDating_); n++)
  {
    if(Serie(n)->getDatingType()==DATING_VOLATILE) { serieDatVol = n; }
	  if(Serie(n) && (Serie(n)->Grammar()==GraSerie()) && (n!=serieDatVol))
	  {
	    newDating = Serie(n)->Dating();
	    if(newDating)
	    {
		    allNull = BFALSE;
		    if(!dating) { dating = newDating; }
    		checkDating_ = (dating == newDating) ||
		                   (dating->HasName() &&
		                   (dating->Name() == newDating->Name())) ||
		                   (dating->Description().HasName() &&
		                   (dating->Description() == newDating->Description()));
		    if(!checkDating_)
        {
            checkDating_=dating->IsCompatibleWith(*newDating,firstDate_,lastDate_);
	      }
	    }
    }
    if(!dating && serieDatVol) 
    {
	    dating = Serie(serieDatVol)->Dating();
	    volatileDating_ = true;
	    checkDating_ = BTRUE;
    }
    if(!dating) { checkDating_ = BFALSE; }
    if(checkDating_)
    {
	    dating_ = dating;
	    if(dating_ && firstDate_.HasValue() && lastDate_.HasValue())
	    {
	      firstDate_ = dating_->FirstNoLess (firstDate_);
	      lastDate_	 = dating_->FirstNoGreat(lastDate_);
	    }
    }
    else if(!allNull)
    {
	    dating_ = NIL;
	    BText serTxt = "\nSeries:\n";
	    for(n=1; n<=NumSeries(); n++)
	    {
	      if(Serie(n) && (Serie(n)->Grammar()==GraSerie()))
	      {
		      serTxt += BText("  ") + Serie(n)->Identify()+
		                I2(" has the dating ", " tiene el fechado ")+
		                Serie(n)->Dating()->Identify()+
		                ";\n";
	      }
	    }
	    Error(I2("Cannot operate series with incompatible dating.",
		           "No se puede operar con series de fechado incompatible.")+
	          serTxt);
	    return;
    }
  }
}

//--------------------------------------------------------------------
BBool BSerieGroup::TestDates()
//--------------------------------------------------------------------
{
    if(!Dating())
    {
	Error(I2("Wrong dating for grouping series.",
		 "Fechado erróneo en grupo de series."));
	return(BFALSE);
    }
    
    if(!MaxFirst().HasValue() || !MinLast().HasValue() ||
       (MaxFirst()>MinLast()))
    {
	Error(I2("Wrong dates for grouping series.",
		 "Fechas erróneas en grupo de series."));
	return(BFALSE);
    }
    return(BTRUE);
}


//--------------------------------------------------------------------
void BTableFormat::PutRealFormat  (BInt len, BInt dec)
//--------------------------------------------------------------------
{
    BText lenTxt; lenTxt.Copy(len,"%2ld");
    BText decTxt; decTxt.Copy(dec,"%ld");
    realFormat_  = BText("%")+lenTxt+"."+decTxt+"lf";
    restLength_  = len;
}


//--------------------------------------------------------------------
BInt BTableFormat::Length(BInt col)
//--------------------------------------------------------------------
{ return((col>0)?restLength_:firstLength_); }


//--------------------------------------------------------------------
BText BTableFormat::Format(const BText& txt, BInt col)
//--------------------------------------------------------------------
{
    BText compact = Compact(txt);
    return(compact.Pad(align_, Length(col), fill_));
}


//--------------------------------------------------------------------
BText BTableFormat::Format(const BDat& dat, BInt col)
//--------------------------------------------------------------------
{
    BText compact = Compact(dat.Format(realFormat_));
    return(compact.Pad(align_, Length(col), fill_));
}

//--------------------------------------------------------------------
void BSerieTable::Fill(const BDate& f,  const BDate& l)

/*! BSerieTable constructor.
 */
//--------------------------------------------------------------------
{
  BInt n;
  if(!(NumSeries())) { return; }
  PrepareFill(f, l);
  if(!(Dating())) { return; }
  if(!firstDate_.HasValue())
  {
	  firstDate_ = Dating()->FirstNoLess(BDate::DefaultFirst());
  }
  if(!lastDate_ .HasValue())
  {
	  lastDate_  = Dating()->FirstNoGreat(BDate::DefaultLast ());
  }
  dating_->GetHashBetween(hash_, firstDate_, lastDate_);
  numDates_  = hash_.Size();
  data_.Alloc(NumSeries(),numDates_);
  date_.ReallocBuffer(numDates_);
  for(n=0; n<numDates_; n++) { date_[n].PutHash(hash_[n]); }
  BArray< BData > data(NumSeries());
  data_.Alloc(NumSeries(), numDates_);
  for(n=1; n<=NumSeries(); n++)
  {
    BUserTimeSerie* ser = Serie(n);
    BData& dt = data(n-1); 
  //dt.AllocBuffer(numDates_);
    dt.Replicate(BDat::Unknown(),numDates_);
	  if(ser && (ser->Grammar()==GraSerie()))
	  {
	    ser->GetData(dt, firstDate_, lastDate_, numDates_);
	  }
    void* dest = data_.GetData().GetBuffer()+(n-1)*numDates_;
    const void* sour = dt.Buffer();
    int size = numDates_*sizeof(BDat);
    if(dest && sour) { memcpy(dest,sour,size); }
  }
}

//--------------------------------------------------------------------
void WriteAsBDC (BUserTimeSerie* serie, ostream& os)

/*! Writes a serie into a file with Bayes Data Column format
 */
//--------------------------------------------------------------------
{
    if(!(serie->Dating())) { return; }
    BText serNameTxt	   = serie->Name();
    BText serDescTxt	   = BText("\"")+ serie->Description() + "\"";
    BText serDatingTxt	   = serie->Dating()->Identify();
    const BChar* serName   = serNameTxt.String();
    const BChar* serDesc   = serDescTxt.String();
    const BChar* serDating = serDatingTxt.String();
    BDate first            = serie->FirstDate();
    BDate last             = serie->LastDate();
    if(!first.HasValue()) { first = BDate::DefaultFirst(); }
    if(!last .HasValue()) { last  = BDate::DefaultLast (); }
    
    BHash hash; serie->Dating()->GetHashBetween(hash, first, last);
    BInt numDates = hash.Size();
    BData data;
    serie->GetData(data, first, last, numDates);
    
    os<<"#"<<serName<<"\n"
      <<"ACTUALIZACION "<<serName<<" ES UNA Serie temporal primaria FIN\n"
      <<"VALOR DE ("<<serName<<") Fechado = "<<serDating<<"\n"
      <<"VALOR DE ("<<serName<<") Titulo  = "<<serDesc<<"\n"
      <<"VALOR DE ("<<serName<<") Fuente  = "<<"\"TOL\""<<"\n"
      <<"&DATOS SUSTITUCION\n"
      <<first.Name()<<"\n";
    
    for(BInt m=0; m<numDates; m++)
    {
	if(m>0) { os << ",\n"; }
	os<< data[m].Value();
    }
    os << ";\n";
}


//--------------------------------------------------------------------
void BSerieTable::WriteAsBDC (ostream& os)

/*! Writes a serie's table into a file with Bayes Data Column format
 */
//--------------------------------------------------------------------
{
    if(!Dating()) { return; }
    BInt n, m;
    if((numDates_<=0)&&(NumSeries()<=0)) { return; }
    BText serDatingTxt = Dating()->Name();
    const char* nameDating = serDatingTxt.String();
    for(n=1; n<=NumSeries(); n++)
    {
	BText serNameTxt = Serie(n)->Name();
	const BChar* serName = serNameTxt.String();
	BText serDescTxt = BText("\"")+ Serie(n)->Description() + "\"";
	const BChar* serDesc = serDescTxt.String();
	BDate first = Date(1);
	os<<"#"<<serName<<"\n";
	os<<"ACTUALIZACION "<<serName<<" ES UNA Serie temporal primaria FIN\n";
	os<<"VALOR DE ("<<serName<<") Fechado = "<<nameDating<<"\n";
	os<<"VALOR DE ("<<serName<<") Titulo  = "<<serDesc<<"\n";
	os<<"VALOR DE ("<<serName<<") Fuente  = "<<"\"TOL\""<<"\n";
	os<<"&DATOS SUSTITUCION\n";
	os<<first.Name()<<"\n";
	for(m=1; m<=numDates_; m++)
	{
	    if(m>1) { os<< ",\n"; }
	    os<<TextData(n,m);
	}
	os << ";\n";
    }
}


//--------------------------------------------------------------------
void BSerieTable::WriteAsBDT (ostream& os, const BText& header)

/*! Writes a serie's table into a file with Bayes Data Table format
 */
//--------------------------------------------------------------------
{
    if(!Dating()) { return; }
    BInt n, m;
    if((numDates_<=0)&&(NumSeries()<=0)) { return; }
    if(header.HasName())
    {
	os << header.String() << "\n";
    }
    else
    {
	os << Dating()->Identify().String() << "; ";
	for(n=1; n<=NumSeries(); n++) { os << Name(n).String() << "; "; }
	os << "\n";
    }
    for(m=1; m<=numDates_; m++)
    {
	os << BTableFormat::DateFormat().DateToText(Date(m)) << "; ";
	for(n=1; n<=NumSeries(); n++)
	{
	    os << TextData(n,m).String() << "; ";
	}
	os << "\n";
    }
}


//--------------------------------------------------------------------
void Std (BSerieTable& dt)

/*! Writes a serie's table into a file with Bayes Data Table format
 */
//--------------------------------------------------------------------
{
    if(!dt.Dating()) { return; }
    BInt n, m;
    if((dt.numDates_<=0)&&(dt.NumSeries()<=0)) { return; }
    Std(dt.Dating()->Identify() + "; ");
    for(n=1; n<=dt.NumSeries(); n++) { Std(dt.Name(n) + "; "); }
    Std("\n");
    for(m=1; m<=dt.numDates_; m++)
    {
	Std(BTableFormat::DateFormat().DateToText(dt.Date(m)) + "; ");
	for(n=1; n<=dt.NumSeries(); n++)
	{
	    Std(dt.TextData(n,m)+"; ");
	}
	Std("\n");
    }
}

//--------------------------------------------------------------------
BInt BSerieTable::CreateTextTable(BArray< BArray<BText> >& txt) const
//--------------------------------------------------------------------
{
    if(!Dating()) { return(-1); }
    BInt sepLength = strlen(BTableFormat::Sep());
    BInt totalLength = 0;
    BArray< BInt > max;
    if((numDates_<=0)&&(NumSeries()<=0)) { return(0); }
    BInt n, m;
    max.ReallocBuffer(NumSeries()+1);
    txt.ReallocBuffer(numDates_+1);
    for(m=0; m<=numDates_; m++) { txt[m].ReallocBuffer(NumSeries()+1); }
    
    n=m=0;
    txt[m][n] = Dating()->Identify();
    txt[n][n].Compact();
    max[n]    = txt[m][n].Length();
    for(n=1; n<=NumSeries(); n++)
    {
	txt[m][n] = Name(n);
	txt[m][n].Compact();
	max[n] = txt[m][n].Length();
    }
    for(m=1; m<=numDates_; m++)
    {
	n=0;
	txt[m][n] = BTableFormat::DateFormat().DateToText(Date(m));
	txt[m][n].Compact();
	if(max[n]<txt[m][n].Length()) { max[n]=txt[m][n].Length(); }
	for(n=1; n<=NumSeries(); n++)
	{
	    txt[m][n] = Dat(n,m).Format(BTableFormat::RealFormat());
	    txt[m][n].Compact();
	    if(max[n]<txt[m][n].Length()) { max[n]=txt[m][n].Length(); }
	}
    }
    
    for(m=0; m<=numDates_; m++)
    {
	for(n=0; n<=NumSeries(); n++)
	{
	    txt[m][n]=txt[m][n].Pad(BTableFormat::Align(),
				    1+max[n],
				    BTableFormat::Fill());
	    txt[m][n]+=BTableFormat::Sep();
	}
	txt[m][NumSeries()]+="\n";
    }
    
    for(n=0; n<=NumSeries(); n++)
    {
	max[n]+=sepLength;
	totalLength+=max[n];
    }
    max[NumSeries()]   +=2;
    totalLength*= (numDates_+1);
    return(totalLength);
}


//--------------------------------------------------------------------
ostream& operator << (ostream& os, BSerieTable& st)
//--------------------------------------------------------------------
{
    if(!st.Dating()) { return(os); }
    BArray< BArray <BText> > cells;
    BInt n, m;
    if(BSerieTable::FormatBDT()!=0)
    {
	st.CreateTextTable(cells);
	for(m=0; m<=st.numDates_; m++)
	{ for(n=0; n<=st.NumSeries(); n++) { os << cells[m][n].String(); } }
    }
    else
    {
	st.WriteAsBDT(os);
    }
    return(os);
}



//--------------------------------------------------------------------
static BBool ReSizeData(const BText&	         file,
			      BArray<BDataLine>& data,
			      BUserTimeSet*      dating,
			      BDate&	         firstDate,
			      BDate&	         lastDate,
			      BBool	         enableWarning)
//--------------------------------------------------------------------
{
  if((data.Size()<=0) || !dating)  { return(BFALSE); }
  data.Sort(BDataLineOrderCriterium);
  firstDate.PutHash(data[0].Hash());
  lastDate .PutHash(data[data.Size()-1].Hash());

  if(!firstDate.HasValue() || !lastDate.HasValue())
  {
    Error(BDT+file+" "+I2("Wrong dates.", "Fechas no validas"));
    return(BFALSE);
  }
  if(!dating->Contain(firstDate)) { firstDate+=dating; }
  if(!dating->Contain(lastDate))  { lastDate -=dating; }

  BHash hash(data.Size()*10);
  BDate date	 = firstDate;
  BInt	d	 = 0;
  BInt	n	 = 0;
  dating->GetHashBetween(hash, firstDate, lastDate);
  BArray<BDataLine> newData(hash.Size());
  for(d=0; d<hash.Size(); d++)
  {
    while(hash[d]>data[n].Hash())
    {
      if(enableWarning)
      {
	Warning(BDT+file+" "+I2("Date ","Fecha ")+
		BDataLine::DateFormat().DateToText(HashToDte(data[n].Hash()))+
		I2(" out of dating "," fuera del fechado ")+dating->Name());
      }
      n++;
    }
    if(hash[d] == data[n].Hash()) { newData[d] = data[n++]; }
    else			  { newData[d] = BDataLine(hash[d]); }
  }
  data = newData;
  return(BTRUE);
}


//--------------------------------------------------------------------
BBool ReadBDT(const BText&	          file,
	            BDat&		  fillValue,
	            BArray<BDataLine>&    data,
	            BArray<BTsrPrimary*>& serie,
	            BBool		  enableWarning)
//--------------------------------------------------------------------
{
  if((data.Size()<=0) || !serie[0] || !(serie[0]->Dating()))
  { return(BFALSE); }

  BUserTimeSet* dating = serie[0]->Dating();
  BDate firstDate, lastDate;
  if(!ReSizeData(file,data, dating, firstDate, lastDate, enableWarning))
  { return(BFALSE); }

  BBool datesOk = BTRUE;
  for(BInt m = 0; m<serie.Size(); m++)
  {
    BUserTimeSerie* ser = serie[m];
    if(!ser)
    {
      Error(BDT + file +" "+ I2("Null serie.", "Serie nula."));
      return(BFALSE);
    }
    if( (m>0) && (ser->Dating()!=serie[m-1]->Dating()) )
    {
      Error(BDT+file+" "+
	    I2(" Wrong dating."," Fechado no valido."));
      return(BFALSE);
    }
    if(ser->FirstDate()!=firstDate)
    {
      datesOk=!(ser->FirstDate().HasValue());
      ser->PutFirstDate(firstDate);
    }
    if(ser->LastDate()!=lastDate)
    {
      datesOk=!(ser->LastDate().HasValue());
      ser->PutLastDate(lastDate);
    }
    if(!datesOk && enableWarning)
    {
      Warning(BDT+file+" "+
	      I2("First or last dates have been modified for serie ",
		 "Las fechas inicial o final han sido modificadas "
		 "para la serie ")+
	      ser->Name());
      datesOk = BTRUE;
    }
    ser->Alloc(data.Size());

    for(BInt n=0; n<data.Size(); n++)
    {
      if(data[n].Data().Size())
      { ser->PutBufDat(n,data[n][m]); }
      else
      { ser->PutBufDat(n,fillValue); }
    }
  }
  return(BTRUE);
}


//--------------------------------------------------------------------
void BDataLine::PutLine(const BArray<BText>& line)

/*! BDataLine
 */
//--------------------------------------------------------------------
{
    BText txt = line[0];
    txt.Compact();
    BDate dte = dateFormat_.TextToDate(txt);
    hash_ = dte.Hash();
    for(BInt n=0; n<Data().Size(); n++) { Data()[n].PutValue(line[n+1]); }
    
}


//--------------------------------------------------------------------
BBool ReadBDTText(BDat&		   fillValue,
		  BText&		   txtData,
		  BArray<BTsrPrimary*>& serie,
		  BBool		   enableWarning)
//--------------------------------------------------------------------
{
    BTextCell	    cell;
    ReadAllCells(txtData, cell, '\n', ';');
    BArray<BData>	    data    (cell.Size());
    BArray<BDataLine> dataLine(cell.Size());
    for(BInt n = 0; n<cell.Size(); n++)
    {
	data[n].ReallocBuffer(cell[n].Size()-1);
	dataLine[n].PutData(data[n]);
	dataLine[n].PutLine(cell[n]);
    }
    return(ReadBDT(" ",fillValue, dataLine, serie, enableWarning));
}

//--------------------------------------------------------------------
   BBool BDTFindDate(istream& file,
		     BInt     lineMax,
		     BChar    sep,
		     BReal    searched,
		     BReal&   fromHash,
		     BReal&   toHash,
		     BInt&    fromPos,
		     BInt&    toPos)

// PURPOSE:
//
//--------------------------------------------------------------------
{
  BInt	n = 0;
  BText line;
  BText dateTxt(65);
  BReal hash;
  BDate dte;
  BInt	pos;
  BInt	oldFromHash = (BInt)fromHash;
  BInt	oldToHash   = (BInt)toHash;
  do
  {
    pos = file.tellg();
    GetLine(file,line,64,sep);
    line.Compact();
    dte = ConstantDate(line);
    GetLine(file,line,lineMax,'\n');
  }
  while(!file.eof() && !dte.HasValue());
  if(!dte.HasValue()) { return(BFALSE); }
  hash = dte.Hash();
//  Std(BText("\nBDTFindDate [")+fromPos +" "+pos +" "+toPos +"] "+
//			    "[" +fromHash+" "+hash+" "+toHash+"] "+dte);
  if(hash == searched)
  {
    file.seekg(pos,ios::beg);
    return(BTRUE);
  }
  else if(hash < searched)
  {
    fromPos  = pos;
    fromHash = hash;
  }
  else //if(hash > searched)
  {
    toPos  = pos;
    toHash = hash;
  }

  if((oldFromHash == fromHash) && (oldToHash == toHash))
  {
    file.seekg(pos,ios::beg);
    return(BTRUE);
  }

  file.seekg((toPos+fromPos)/2,ios::beg);
  GetLine(file,line,lineMax,'\n');
  return(BDTFindDate(file, lineMax, sep, searched,
		     fromHash, toHash, fromPos, toPos));
}


//--------------------------------------------------------------------
  static bool BDTUnexpectedEndOfLine(int line, const BText& fileName)
//--------------------------------------------------------------------
{
  Error(I2("Unexpected end of line ","Inesperado fin de línea ")+(line+2)+
        I2(" reading BDT file "," leyendo el fichero BDT " )+fileName);
  return(false);
}

//--------------------------------------------------------------------
  static bool BDTTooLargeCellValue(char* buf, int line, const BText& fileName)
//--------------------------------------------------------------------
{
  Error(I2("Too large cell value '",
           "Valor de celda demasiado largo '")+buf+"'"+
        I2(" in line "," en la línea ")+(line+2)+
        I2(" reading BDT file "," leyendo el fichero BDT " )+fileName);
  return(false);
}

#define MaxItemLength 128

//--------------------------------------------------------------------
BBool ReadBDTFile(const BText&	              fileName,
		        BDat&		      fillValue,
 		        istream& 	      file,
		        BArray<BTsrPrimary*>& serie,
		        BBool		      enableWarning,
		        BDate		      first,
		        BDate		      last,
		        BChar		      sep,
		        BBool		      hasHeader)
//--------------------------------------------------------------------
{
  BChar		    buf [MaxItemLength+1];
  BText		    txt(MaxItemLength+1);
  BBool		    go	       = BTRUE;
  BBool		    end	       = BFALSE;
  BBool		    knownDates = first.HasValue() && last.HasValue();
  BInt		    rowNum     = 200;
  BArray<BReal>	    hash    (rowNum);
  BArray<BData>	    data    (rowNum);
  BArray<BDataLine> dataLine(rowNum);
  BInt		    n=0, k=0, m=0, s;
  BText		    line;
  BReal		    firstHash;
  BReal		    lastHash;
  BInt		    size = BDir(fileName).Bytes();
  BInt		    lineMax = MaxItemLength*(1+serie.Size());
  if(knownDates)
  {
    firstHash = first.Hash();
    lastHash  = last .Hash();
    BReal fH = firstHash;
    BReal tH = lastHash;
    BInt  fP = 0;
    BInt  tP = size;
    BDTFindDate(file, lineMax, sep, fH, fH, tH, fP, tP);
  }
  do
  {
    GetLine(file,line, lineMax,'\n');
    if(n==data.Size())
    {
      data.ReallocBuffer(n*2);
      hash.ReallocBuffer(n*2);
    }
  //Std(BText("\nBDT line[")+(n+1)+"]="+line);
    const BChar* p = line.String();
    m=0;
    for(k=0; p[m]&&(p[m]!=sep)&&(k<MaxItemLength); m++, k++) { buf[k]=p[m]; }
    buf[k]='\0';
    if(k==MaxItemLength) { return(BDTTooLargeCellValue(buf,n,fileName)); }
    txt.Copy(buf,k);
    txt.Compact();
    if(txt.HasName()) 
    { 
      if(!p[m++]) { return(BDTUnexpectedEndOfLine(n,fileName)); }
      BDate dte = ConstantDate(txt);
      if(dte.HasValue())
      {
        hash[n] = dte.Hash();
      //Std(BText("\t'")+txt+"'->"+dte);
        if(knownDates)
	      {
	        go  = (hash[n] >= firstHash);
	        if(go) { end = (hash[n] >  lastHash ); }
	      }
	      if(go && !end)
	      {
	        data[n].ReallocBuffer(serie.Size());
	        for(s = 0; s<serie.Size(); s++)
	        {
            for(k=0; p[m]&&(p[m]!=sep)&&(k<MaxItemLength); m++, k++) { buf[k]=p[m]; }
            buf[k]='\0';
            if(k==MaxItemLength) { return(BDTTooLargeCellValue(buf,n,fileName)); }
            txt.Copy(buf,k);
            txt.Compact();
            data[n][s].PutValue(txt);
          //Std(BText("\t'")+txt+"'->"+data[n][s]);
            if(!p[m++] && (m<serie.Size()-1)) { return(BDTUnexpectedEndOfLine(n,fileName)); }
          }
          for(; p[m] && m<line.Length(); m++) 
          { 
            if((p[m]=='/')&&(p[m+1]=='/')) { break; }
            if(p[m]!=' ') 
            { 
              BText unexpected(p+m,1);
              Warning(I2("Skiped unexpected symbols '",
                         "Descartados símbolos inesperados '")+unexpected+
                      I2("' after data line ",
                         "' después de los datos de la línea ")+(n+2)+
                      I2(" reading BDT file "," leyendo el fichero BDT " )+fileName+
                      I2("\nYou can use // to insert commentaries at the end of a data line.",
                         "\nSe puede usar // para insertar comentarios al final de una línea de datos."));
            }
          }

          n++;
        }
      }
    }
  }
  while(!file.eof() && !end);
  data.ReallocBuffer(n);
  hash.ReallocBuffer(n);
  dataLine.ReallocBuffer(n);
  for(n = 0; n<data.Size(); n++)
  { dataLine[n].PutLine(hash[n],data[n]); }
  return(ReadBDT(fileName,fillValue, dataLine, serie, enableWarning));
}

//--------------------------------------------------------------------
   void PeriodicTable(	    BUserTimeSerie* ser,
			    BUserTimeSet*   tms,
		      const BDate&	    first,
		      const BDate&	    last,
			    BInt	    period,
		      const BText&	    header,
		      const BText&	    fmt,
		      const BText&	    fileName)
//--------------------------------------------------------------------
{
  ofstream out;
  BData data;
  if(fileName!="Std") { out.open(fileName.String()); }
  BInt	N = ser->Dating()->Difference(first,last);
  ser->GetData(data, first, last, N);
  BDate dte = first;
  BInt	n;
  WriteFileOrStd(fileName, out, (ser->Identify()+";"));
  WriteFileOrStd(fileName, out, header);
  BDateFormat format(fmt);
  for(n=0;n<N;n++)
  {
    if(!(n%period))
    {
      WriteFileOrStd(fileName, out, (BText("\n")+format.DateToText(dte)+";"));
      dte+=tms;
    }
    WriteFileOrStd(fileName, out, (data[n].Name()+";"));
  }
  if(fileName!="Std") { out.close(); }
}


//--------------------------------------------------------------------
   void PeriodicTable(	    BUserTimeSerie* ser,
			    BUserTimeSet*   tms,
			    BInt	    period,
		      const BText&	    header,
		      const BText&	    fmt,
		      const BText&	    fileName)
//--------------------------------------------------------------------
{
  ofstream out;
  if(fileName!="Std") { out.open(fileName.String()); }
  BDate	       first = ser->FirstDate();
  BDate	       last  = ser->LastDate();
  if(!first.HasValue()) { first = BDate::DefaultFirst(); }
  if(!last .HasValue()) { last	= BDate::DefaultLast(); }
  first = tms->FirstNoGreat(first);
  last	= tms->FirstNoLess (last);
  PeriodicTable(ser,tms,first,last,period,header,fmt,fileName);
}




static BText monthHead_ = I2
(
  "Jan; Feb; Mar; Apr; May; Jun; Jul; Aug; Sep; Oct; Nov; Dec;",
  "Ene; Feb; Mar; Abr; May; Jun; Jul; Ago; Sep; Oct; Nov; Dic;"
);

static BText weekDayHead_ = I2
(
  "Mon; Tue; Wed; Thu; Fri; Sat; Sun;",
  "Lun; Mar; Mie; Jue; Vie; Sab; Dom;"
);

//--------------------------------------------------------------------
   void PeriodicTable(	    BUserTimeSerie* ser,
		      const BText&	    fileName)
//--------------------------------------------------------------------
{

  BUserTimeSet*	 tms	= ser->Dating();
  BText		 dating = tms->Identify();
  dating.Compact();

  if((dating=="C")||(dating=="Daily")||(dating=="Diario"))
  {
    PeriodicTable(ser,Tms("Weekly"),7,weekDayHead_,"%Y/%m/%d",fileName);
  }
  else if((dating=="D(1)")||(dating=="Monthly")||(dating=="Mensual"))
  {
    PeriodicTable(ser,Tms("Yearly"),12,monthHead_,"%Y",fileName);
  }
  else if((dating=="Bimonthly")||(dating=="Bimensual"))
  {
    PeriodicTable(ser,Tms("Yearly"),6,"I;II;III;IV;V;VI;","%Y",fileName);
  }
  else if((dating=="Quarterly")||(dating=="Trimestral"))
  {
    PeriodicTable(ser,Tms("Yearly"),4,"I;II;III;IV;","%Y",fileName);
  }
  else if((dating=="Thirdannual")||(dating=="Cuatrimestral"))
  {
    PeriodicTable(ser,Tms("Yearly"),3,"I;II;III;","%Y",fileName);
  }
  else if((dating=="HalfYearly")||(dating=="Semestral"))
  {
    PeriodicTable(ser,Tms("Yearly"),2,"I;II;","%Y",fileName);
  }
  else if((dating=="Yearly")||(dating=="Anual"))
  {
    PeriodicTable(ser,Tms("Yearly"),1,I2("Data;","Dato;"),"%Y",fileName);
  }
  else
  {
    BDTWriteFile1(ser,fileName);
  }
}

//--------------------------------------------------------------------
  void BDTWriteFile1(BUserTimeSerie* ser, const BText& fileName)
//--------------------------------------------------------------------
{
  BSerieTable table;
  table.AddSerie(ser);
  table.Fill();
  {
    ofstream out;
    if(fileName!="Std") { out.open(fileName.String()); }
    WriteFileOrStd(fileName, out, table);
    if(fileName!="Std") { out.close(); }
  }
}

//--------------------------------------------------------------------
  void BDTWriteFileL(BList* ser, const BText& fileName)
//--------------------------------------------------------------------
{
  BSerieTable table;
  table.AddList(ser);
  table.Fill();
  {
    ofstream out;
    if(fileName!="Std") { out.open(fileName.String()); }
    WriteFileOrStd(fileName, out, table);
    if(fileName!="Std") { out.close(); }
  }
}





