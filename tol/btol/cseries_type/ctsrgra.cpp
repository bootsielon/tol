/* tsrgra.cpp: TOL Language. Time Serie Grammar's functions

   Copyright (C) 2003,2005 - Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bctsrgra.h>
//#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bprdist.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
//bool ForzeLinkerCTsrGraInternal();
bool ForzeLinkerCTsrGraVarious();

static bool forzeLinkers = //ForzeLinkerCTsrGraInternal() &&
                            ForzeLinkerCTsrGraVarious ();

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("ctsrgra.cpp");

template<>
BGrammar* BUserCTimeSeries::ownGrammar_ = NIL;

//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BUserCTimeSeries::InitInstances()
{
    BTraceInit("BUserCTimeSeries::InitInstances");
    BCTsrFromReal* unknown_ = new BCTsrFromReal(BDat::Unknown());
    unknown_->PutName("UnknownCSeries");
    unknown_->IncNRefs();
    unknown_->PutDescription("Unknown CSeries");
    unknown_->PutGlobal();
    unknown_->PutSystem(BTRUE);
    OwnGrammar()->AddObject(unknown_);
    OwnGrammar()->PutDefect(unknown_);
}

//--------------------------------------------------------------------
void BUserCTimeSeriesDo::Do ()
{
    if(IsStochastic())
    {
	FirstCTime();
	CompactData();
    }
    else if(HasDating())
	operator[](BCTime::fillNew(Dating()->getGranul(), LV, BCTime::Now()));
    else 
	operator[](BCTime::Now());
}

//--------------------------------------------------------------------
BText BUserCTimeSeriesDo::Info() const
{
    BText txt;
    if(dating_) { txt += dating_->Identify(); }
    else        { txt += "NoDating"; }
    txt += BText("[") + firstCTime_ + "," + lastCTime_ + "]";
    return(txt);
}

//--------------------------------------------------------------------
void BUserCTimeSeriesDo::PutContens(BUserCTimeSeriesDo* ser)
{
    if(!ser->Dating())
    {
	Error(I2("Source CSeries is a constant CSeries (without a CTimeSet). "
		 "The operation can not be done.",
		 "No se puede actualizar una CSeries temporal con otra que "
		 "no tiene fechado (CSeries constante)"));
    }
    else
    {
	PutFile(NIL);
	dating_ = ser->Dating();
	BCTime first = ser->FirstCTime();
	BCTime last  = ser->LastCTime ();
	int  length = ser->GetLength();
	if(!ser->IsStochastic())
	{
	    first = BCTime::fillNew(dating_->getGranul(), LV,
				    BCTime::Now().year());
	    last = BCTime::fillNew(dating_->getGranul(), HV,
				   BCTime::Now().year());

	    if(first==last) last.successor(1);
	    length = 1 + dating_->distance(first,last);
	}
	firstCTime_ = first;
	lastCTime_  = last ;
	length_    = length;
	ser->GetData(data_,first,last,length);
	PutReleased();
    }
}

//--------------------------------------------------------------------
BSyntaxObject* BUserCTimeSeriesDo::CopyContens()
{
    BUserCTimeSeriesDo* ser = NIL;
    if(!Dating())
    {
	ser = new BCTsrFromReal(GetDat(BCTime::Now()));
    }
    else
    {
	ser = new BCTsrPrimary("",Description());
	ser->PutContens(this);
    }
  
    return(ser);
}

//--------------------------------------------------------------------
void BCTsrTemporary::ReCalc ()
{
    BTmpObject<BUserCTimeSeriesDo>::ReCalc();
    data_.ReallocBuffer(0);
    flags_.calculated_ = BFALSE;
    CalcObject();
}

//--------------------------------------------------------------------
BDat BCTsrTemporary::LogicShort(bool(*comp)(const BDat&,const BDat&),
                               const BCTime& tmi) const
{
    int n = 2;
    bool ok = 1;
    BDat oldDat = CTsrR(Arg(1))[tmi];
    BDat dat;
    BUserCTimeSeries* S;
  
    if(oldDat.IsKnown())
    {
	while((S = CTsr(Arg(n))) && (dat=((*S)[tmi])).IsKnown() && ok)
	{
	    n++;
	    ok = (*comp)(oldDat, dat);
	    oldDat = dat;
	}
	if(oldDat.IsKnown()) { oldDat.PutValue(ok); }
    }

    return oldDat;
}


//--------------------------------------------------------------------
template<>
BDat BUserCTimeSeries::Compare(const BSyntaxObject* obj1,
			       const BSyntaxObject* obj2)
{
    BText name1 = obj1->Name();
    BText name2 = obj2->Name();
    return((name1<name2)?-1:(name1>name2)?1:0);
}

//--------------------------------------------------------------------
// BCTsrFromReal
//--------------------------------------------------------------------
BCTsrFromReal::BCTsrFromReal(BList* arg)
    : BCTsrTemporary(arg)
{
    datingType_ = CDATING_VOLATILE;
}


//--------------------------------------------------------------------
BCTsrFromReal::BCTsrFromReal(const BDat& dat)
    : BCTsrTemporary(NCons(new BUserDat("",dat)))
{
    datingType_ = CDATING_VOLATILE;
}

//--------------------------------------------------------------------
BDat BCTsrFromReal::operator[](const BCTime& tmi)
{
    BDat dat = Dat(Arg(1));
    return dat;
}

//--------------------------------------------------------------------
BDat BCTsrFromReal::GetDat(const BCTime& tmi)
{
    BDat dat = Dat(Arg(1));
    return dat;
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BUserCTimeSeries::FindConstant(const BText& name)
{
    BSyntaxObject* bsyntax = NIL;
    
    BUserDat* uDat = (BUserDat*)(GraReal()->FindConstant(name));
    if(uDat) {
	BUserCTimeSeries* uCTsr = new BCTsrFromReal(NCons(uDat));
	BUserCTimeSet* uCTms = 
	    (BUserCTimeSet*)(GraTimeSet()->FindVariable("C"));
	uCTsr->PutDating(uCTms);
	bsyntax = reinterpret_cast<BSyntaxObject*>(uCTsr);
    }
    
    return bsyntax;
}


//--------------------------------------------------------------------
template<>
BSyntaxObject* BUserCTimeSeries::Casting(BSyntaxObject* obj)
{
    BSyntaxObject* bsyntax = NIL;
    
    if(!obj)                           { return(NIL); }
    if(obj->Grammar()==OwnGrammar())   { return(obj); }
    if(obj->Grammar()->Name()=="Real")
    {
	obj->Do();
	BUserCTimeSeries* uCTsr = new BCTsrFromReal(NCons(obj));
	BUserCTimeSet* uCTms = 
	    (BUserCTimeSet*)(GraTimeSet()->FindVariable("C"));
	uCTsr->PutDating(uCTms);
	bsyntax = reinterpret_cast<BSyntaxObject*>(uCTsr);
    }
    
    return bsyntax;
}


//--------------------------------------------------------------------
BCTsrPrimary::BCTsrPrimary(const BText&   name,
			   const BText&   title,
			   BUserCTimeSet* dating,
			   const BCTime&  first,
			   const BCTime&  last)
    : BUserCTimeSeriesDo(name)
{
    PutDescription(title);
    PutDating(dating);
    PutFirstCTime(first);
    PutLastCTime(last);
    GetLength();
}

//--------------------------------------------------------------------
BCTsrPrimary::BCTsrPrimary(const BText&   name,
			   const BText&   title,
			   BUserCTimeSet* dating,
			   const BCTime&  first,
			   const BCTime&  last,
			   const BData&   data)
    : BUserCTimeSeriesDo(name)
{
    PutDescription(title);
    PutDating(dating);
    PutFirstCTime(first);
    PutLastCTime(last);
    GetDataBuffer()=data;
    GetLength();
}

//--------------------------------------------------------------------
BCTsrPrimary::BCTsrPrimary(const BText&   name,
			   const BText&   title,
			   BUserCTimeSet* dating,
			   const BCTime&  first,
			   const BData&   data)
    : BUserCTimeSeriesDo(name)
{
    PutDescription(title);
    PutDating(dating);
    PutFirstCTime(first);
    PutLastCTime(dating->next(first,data.Size()-1));
    PutLength(data.Size());
    GetDataBuffer()=data;
}

//--------------------------------------------------------------------
BDat BCTsrPrimary::GetDat(const BCTime& tmi)
{
    if(data_.Size()) { return(data_[GetIndex(tmi)]); }
    else             { return(BDat::Unknown()); }
}


//--------------------------------------------------------------------
void BCTsrPrimary::ReadData()
{
    if(!GetDataBuffer().Size() && File())
    {
	((BSetIncludeBDT*)File())->ReadData();
    }
}


//--------------------------------------------------------------------
class BCTsrFromYearlyTable : public BCTsrTemporary
{
private:
    bool fail_;
    int  period_;
    BText fileName_;

    void ReadData();
    
public:
    BCTsrFromYearlyTable(BList* arg);
    ~BCTsrFromYearlyTable();
    BDat operator[] (const BCTime& tmi) { return(GetDat(tmi)); }
    BDat GetDat(const BCTime& tmi);
};

//--------------------------------------------------------------------
DeclareEvaluator(BCTsrFromYearlyTable);

//--------------------------------------------------
DefExtOpr(1, BCTsrFromYearlyTable, "ReadYearlyTable", 3, 3,
	  "Text CTimeSet Real",
          I2("(Text file, CTimeSet dating, Real period)",
	     "(Text fichero, CTimeSet fechado, Real periodo)"),
          I2("Reads a time CSeries from a yearly-periodic table.",
	     "Lee una CSeries de una tabla periódica anual."),
          BOperClassify::TimeSeriesAlgebra_);

//--------------------------------------------------
BCTsrFromYearlyTable::BCTsrFromYearlyTable(BList* arg)
    : BCTsrTemporary(arg), fail_(BFALSE)
{
    int period_ = ((int) Real(Arg(3)));

    if(period_ <= 0) 
    {
	std::stringstream EN_ss, ES_ss;
	EN_ss << "Wrong value for period parameter: " << period_ << endl;
	EN_ss << "Must be positive.";
	ES_ss << "Valor erróneo para el parámetro período: " << period_;
	ES_ss << endl << "Debe ser positivo.";
	throw BException(EN_ss.str(), ES_ss.str());
    }

    PutDating(CTms(Arg(2)));
}

BCTsrFromYearlyTable::~BCTsrFromYearlyTable()
{
    if(dating_) dating_->DecNRefs();
}

//--------------------------------------------------
void BCTsrFromYearlyTable::ReadData()
{
    if(GetDataBuffer().Size() || fail_) { return; }
    fail_ = true;
    BTextCell cell;
    BText text;
    fileName_ = Text(Arg(1));
    ReadFile(text, fileName_);
    //ReadCell(cell, fileName_);
    ReadAllCells(text,cell,'\n',';');
    period_ = (int)Real(Arg(3));
    int lag = 12 / period_;
    if(cell.Size()>1)
    {
	int i, m, y;
	int year;
	int firstYear=0;
	int lastYear=0;
	BArray<int> yearTable(cell.Size()-1);
	for(i=1; i<cell.Size(); i++)
	{
	    sscanf(cell[i][0].String(), "%ld", &year);
	    if((i==1)||firstYear>year) { firstYear=year; }
	    if((i==1)|| lastYear<year) {  lastYear=year; }
	    yearTable[i-1]=year;
	}

	BCTime first= BCTime::fillNew(Dating()->getGranul(), LV, 1, firstYear);
	BCTime last = BCTime::fillNew(Dating()->getGranul(), HV, 1, lastYear);

	int len = (1+lastYear-firstYear) * period_;

	if((len>0) && first.isNormal() && last.isNormal())
	{
	    PutFirstCTime(first);
	    PutLastCTime (last);
	    PutLength    (len);
	    GetDataBuffer().ReallocBuffer(len);
	    for(i=1; i<=len/period_; i++)
	    {
		y=yearTable[i-1]-firstYear;
		for(m=1; m<=period_; m++)
		{
		    BDat dat;
		    dat.PutValue(cell[i][m]);
		    GetDataBuffer()[y*period_+(m-1)] = dat;
		}
	    }
	    fail_ = false;
	}
    }
}

//--------------------------------------------------
BDat BCTsrFromYearlyTable::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    if(!fail_)
    {
	if(!Data().HasValue()) { ReadData(); }
	dat = data_[GetIndex(tmi)];
    }
    return(dat);
}


//--------------------------------------------------------------------
const BText& BCTsrBSI::Description() const
{
    BCTimeSeries* ser = (BCTimeSeries*)this; //Cast problem with gcc
    ser->ReadData();
    return(BCTsrPrimary::Description());
}

//--------------------------------------------------
void BCTsrBSI::WriteFile(const BText& bsiFileName, BList* serList)
{
    static BChar sF = '\t';
    static BChar sR = '\n';
    BText bsdFileName = GetFilePath(bsiFileName);
    if(bsdFileName.HasName()) { bsdFileName += "/"; }
    bsdFileName += GetFilePrefix(bsiFileName)+".bsd";
    FILE* bsi = fopen(bsiFileName.String(),"wb");
    FILE* bsd = fopen(bsdFileName.String(),"wb");
    if(!bsi)
    {
	Error(I2("Cannot open for write BSI file ",
		 "No se pudo abrir para escritura el fichero BSI ")+
	      bsiFileName);
    }
    if(!bsd)
    {
	Error(I2("Cannot open for write BSD file ",
		 "No se pudo abrir para escritura el fichero BSD ")+
	      bsdFileName);
    }
    if(!bsi || !bsd) { return; }
    
    // -------------------
    // con un correcto BDat::BinWrite
    // no haria falta.
    BReal value;
    bool known;
    // -------------------
    
    int numSer = 0;
    int offset = 0;
    fwrite(&numSer,sizeof(int),1,bsi);
    while(serList)
    {
	BUserCTimeSeries* ser = CTsr(Car(serList));
	if(ser && (ser->Grammar()==OwnGrammar()))
	{
	    BText name = ser->Name();
	    BUserCTimeSet* dating = ser->Dating();
	    BText datingName = dating->Identify();
	    BCTime first = ser->FirstCTime();
	    BCTime last = ser->LastCTime();
	    
	    // Std(name+" goes from "+first+" to "+last);
	    if(!first.isNormal())
	    {
		first  = BCTime::fillNew(dating->getGranul(), LV,
					 BCTime::Now().year());
		first += dating;
		first -= dating;
	    }
	    if(!last.isNormal())
	    {
		last  = BCTime::fillNew(dating->getGranul(), HV,
					BCTime::Now().year());
		if(dating->getGranul()==YEARS) last.successor(1);

		last += dating;
		last -= dating;
	    }
	    // Std(BText(" -> from ")+first+" to "+last+" ;\n");
	    
	    if(name.HasName() && dating && first.isNormal() && last.isNormal())
	    {
		BText desc   = ser->Description();
		BText dating = ser->Dating()->Identify();
		int len = 1 + ser->Dating()->distance(first,last);
		BData data;
		ser->GetData(data,first,last,len);
		
		name.BinWrite(bsi);
		fwrite(&offset,sizeof(int),1,bsi);
		
		offset += desc.BinWrite(bsd);
		offset += datingName.BinWrite(bsd);
		offset += first.BinWrite(bsd);
		offset += last.BinWrite(bsd);
		offset += sizeof(int) + (sizeof(value) + sizeof(known)) * len;
		
		fwrite(&len, sizeof(int), 1, bsd);
		// ------------------------------------------------
		for ( int i = 0; i < len; i++ ) {
		    value = data[i].Value();
		    known = data[i].Known();
		    // aqui deberia ser data[i].BinWrite(bsd);
		    fwrite(&value, sizeof(value), 1, bsd);
		    fwrite(&known, sizeof(known), 1, bsd);
		}
		// ------------------------------------------------
		//fwrite(data.GetBuffer(), sizeof(BDat), len, bsd);
		numSer++;
	    }
	    else
	    {
		Warning(I2("Cannot save into a BSI file a CSeries without "
			   "name, dating, or initial and final dates. \n  "
			   "CSeries : ",
			   "No se puede guardar en un fichero BSI una CSeries "
			   "sin nombre, sin fechado o sin fechas de inicio "
			   "y final. \n         CSeries : ") +
			ser->Identify()+ "\n" + 
			I2("  File : ","  Fichero : ") +
			bsiFileName);
	    }
	}
	serList = Cdr(serList);
    }

    fclose(bsd);
    fseek(bsi, 0, SEEK_SET);
    fwrite(&numSer,sizeof(int),1,bsi);
    fclose(bsi);
}

//--------------------------------------------------
BList* BCTsrBSI::ReadFile(const BText& bsiFileName)
{
    static BChar sF = '\t';
    static BChar sR = '\n';
    BList* serList = NIL;
    BList* auxList = NIL;
    BText bsdFileName = GetFilePath(bsiFileName)+"/"+
	GetFilePrefix(bsiFileName)+".bsd";
    FILE* bsi = fopen(bsiFileName.String(),"rb");

    if(!bsi)
    {
	Error(I2("Cannot open for read BSI file ",
		 "No se pudo abrir para lectura el fichero BSI ")+
	      bsiFileName);
	return(serList);
    }

    BText name;
    int  offset=0, numSer=0;
    fread(&numSer,sizeof(int),1,bsi);

    for(int n=0; n<numSer; n++)
    {
	name.BinRead(bsi);
	fread(&offset,sizeof(int),1,bsi);
	BCTsrPrimary* ser = new BCTsrBSI(name, bsdFileName, offset);
	LstFastAppend(serList, auxList, ser);
    }

    fclose(bsi);
    return serList;
}

//--------------------------------------------------
void BCTsrBSI::ReadData()
{
    if(!data_.Size())
    {
	FILE* bsd = fopen(bsdFileName_.String(),"rb");
	if(!bsd)
	{
	    Error(I2("Cannot open BSD file ",
		     "No se pudo abrir el fichero BSD ")+
		  bsdFileName_ + "\n(CSERIES  = "+ Name()  +")" );
	    data_.ReallocBuffer(1);
	    return;
	}

	BUserCTimeSet* dating = NIL;
	BText datingName;
	BCTime first, last;
	int length=0;
	if(!fseek(bsd, offset_, SEEK_SET))
	{
	    // -------------------
	    // con un correcto BDat::BinWrite
	    // no haria falta.
	    BReal value;
	    bool known;
	    // -------------------
	    BText desc;
	    desc.BinRead(bsd);
	    datingName.BinRead(bsd);
	    first.BinRead(bsd);
	    last.BinRead(bsd);
	    fread(&length,sizeof(int),1,bsd);
	    PutDescription(desc);
	    data_.ReallocBuffer(length);
	    dating=CTms(GraCTimeSet()->EvaluateExpr(datingName));
	    PutDating(dating);

	    if(!(dating && first.isNormal() && last.isNormal() &&
		 dating->includes(first) && dating->includes(last)))
	    {
		Error(I2("Wrong dating or dates in BSI file ",
			 "Fechado o fechas erróneos en fichero BSI") 
		      + bsdFileName_ + " : " + datingName + "[" 
		      + first + "," + last+"]");
	    }

	    PutFirstCTime(first);
	    PutLastCTime(last);
	    // -------------------------------------
	    for ( int i=0; i<length; i++ ) {
		fread( &value, sizeof(value), 1, bsd );
		fread( &known, sizeof(known), 1, bsd );
		data_[i].PutValue( value );
		data_[i].PutKnown( known );
	    }
	    // -------------------------------------
	    // fread(data_.GetBuffer(),length,sizeof(BDat),bsd);
	}
	else
	{
	    Error(I2("Offset seek failure in BSD file ",
		     "Fallo en la bsqueda de offset en el fichero BSD ")
		  + bsdFileName_ +
		  "\n(OFFSET  = "+ offset_ +")" +
		  "\n(CSERIES = "+ Name()  +")" );
	}

	fclose(bsd);
    }
}

#endif // __USE_TC__
