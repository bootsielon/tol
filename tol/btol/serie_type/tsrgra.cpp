/* tsrgra.cpp: Time Serie Grammar's functions
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

//#define TRACE_LEVEL 4

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_btsrgra.h>
#include <tol/tol_btsrgrai.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bprdist.h>

//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerTsrGraInternal();
BBool ForzeLinkerTsrGraVarious();

static BBool forzeLinkers = ForzeLinkerTsrGraInternal() &&
ForzeLinkerTsrGraVarious ();

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("tsrgra.cpp");

template<>
BGrammar* BUserTimeSerie::ownGrammar_ = NIL;

//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BUserTimeSerie::InitInstances()
{
    BTraceInit("BUserTimeSerie::InitInstances");
    BUserTimeSerie* unknown_ = Tsr(GraSerie()->New
    (
      "UnknownSerie",
      new BTsrFromReal(BDat::Unknown())
    ));
    unknown_->PutDescription(I2("Unknown series.","Serie desconocida."));
    OwnGrammar()->PutDefect(unknown_);
}



//--------------------------------------------------------------------
void BUserTimeSerieDo::Do ()
//--------------------------------------------------------------------
{
  DoCalcObject();
  //Std(BText("\n") + Identify()+ " series doing ... ");
  if(IsStochastic())
  {
    BData dta;
    GetData(dta);
    //  Std(BText("\n IsStochastic [")+FirstDate()+","+LastDate()+"]"+Length());
    //  GetData(GetDataBuffer());
    //  Std(BText("\n IsStochastic [")+FirstDate()+","+LastDate()+"]"+Length());
    CompactData();
    //  Std(BText("\n IsStochastic [")+FirstDate()+","+LastDate()+"]"+Length());
  }
  else
  {
    operator[](DteToday());
  }
}

//--------------------------------------------------------------------
BText BUserTimeSerieDo::Info() const
//--------------------------------------------------------------------
{
  BText txt;
  if(Dating()) { txt += Dating()->Identify(); }
  else         { txt += "NoDating"; }
  txt += BText("[")+FirstDate() + "," + LastDate() + "]";
  return(txt);
}

//--------------------------------------------------------------------
void BUserTimeSerieDo::PutContens(BUserTimeSerieDo* ser)
//--------------------------------------------------------------------
{
  assert(ser);
  //Std(BText("\nBUserTimeSerieDo::PutContens() ")+ "\t" + Identify());
  if(!ser->Dating())
  {
    Error(I2("Cannot apply PutValue (:=) to a time series with another that has "
      "no dating, it's to say a constant series.",
      "No se puede aplicar PutValue (:=) a una serie temporal con otra que "
      "no tiene fechado, o sea, una serie constante."));
  }
  else
  {
    PutDating(ser->Dating());
    BDate first;
    BDate last;
    BInt  length;
    if(!ser->IsStochastic())
    {
      first  = BDate::DefaultFirst();
      last   = BDate::DefaultLast ();
      Warning(I2("Cannot apply PutValue (:=) to a time series with another that has "
        "no bounded dates, so, it will be truncated by current default dates ",
        "No se puede aplicar PutValue (:=) a una serie temporal con otra que "
        "no tiene fechas acotadas, por lo que será truncada por las fechas por "
        "defecto en curso")+": ["+first+","+last+"]");
      length = Dating()->Difference(first,last)+1;
    }
    else
    {
      first  = ser->FirstDate();
      last   = ser->LastDate ();
      length = ser->GetLength();
    }
    BData dta;
    ser->GetData(dta, first, last, length);
    assert(length==dta.Size());
    PutFirstDate(first);
    PutLastDate (last);
    PutLength   (length);
    GetDataBuffer() = dta;
    PutReleased();
  }
}


//--------------------------------------------------------------------
BSyntaxObject* BUserTimeSerieDo::CopyContens()
//--------------------------------------------------------------------
{
//Std(BText("\nBUserTimeSerieDo::CopyContens() ")+ "\t" + Identify());
  BSyntaxObject* ser = NIL;
  if(!Dating())
  {
    ser = new BTsrFromReal(GetDat(DteToday()));
  }
  else
  {
    if(!IsStochastic())
    {
      Warning(I2("Cannot copy non bounded time series!",
                 "No se puede copiar una serie no acotada"));
    }
    BTsrPrimary* pr = new BTsrPrimary("",Description());
    pr->PutContens(this);
    ser = pr;
  }
  return(ser);
}

//--------------------------------------------------------------------
BSyntaxObject* BTsrRenamed::New(const BText& name, BSyntaxObject* d)
//--------------------------------------------------------------------
{ 
//VBR: this new line is equivalent to write +x instead of x because 
//BTsrIdentity is the unary + operator. Now the renamed time serie 
//is saved from changes in new created one
  d = new BTsrIdentity(NCons(d));

//This is the original behaviour
  return((BSyntaxObject*)(new BTsrRenamed(name, d)));
}


//--------------------------------------------------------------------
void BTsrTemporary::ReCalc ()
//--------------------------------------------------------------------
{
  //static BInt level_ = 0;
  //level_ ++;
  //Std(BText("\nBTsrTemporary::ReCalc ")+level_+ "\t" + Identify());
  BTmpObject<BUserTimeSerieDo>::ReCalc();
  Realloc(0);
  //firstDate_ = BDate::Unknown();
  //lastDate_  = BDate::Unknown();
  //dating_ = NIL;
  flags_.calculated_ = BFALSE;
  CalcObject();
  //level_ --;
  
}


//--------------------------------------------------------------------
BDat BTsrTemporary::LogicShort(BBool(*comp)(const BDat&,const BDat&),
                               const BDate& dte) const
                               
/*! Applies a two real numbers comparisson method to the list of
 *  arguments in a date while this comparisson returns a true value
 */
//--------------------------------------------------------------------
{
  BInt      n      = 2;
  BBool     ok     = 1;
  BDat      oldDat = TsrR(Arg(1))[dte];
  BDat      dat;
  BUserTimeSerie* S;
  
  if(oldDat.IsKnown())
  {
    while((S = Tsr(Arg(n))) && (dat=((*S)[dte])).IsKnown() && ok)
    {
      n++;
      ok     = (*comp)(oldDat, dat);
      oldDat = dat;
    }
    if(oldDat.IsKnown()) { oldDat.PutValue(ok); }
  }
  return(oldDat);
}


//--------------------------------------------------------------------
template<>
BDat BUserTimeSerie::Compare(const BSyntaxObject* obj1,
                             const BSyntaxObject* obj2)
{
  BText name1 = obj1->Name();
  BText name2 = obj2->Name();
  return((name1<name2)?-1:(name1>name2)?1:0);
}


//--------------------------------------------------------------------
BTsrFromReal::BTsrFromReal(BList* arg)
//--------------------------------------------------------------------
    : BTsrTemporary(arg)
{
    datingType_ = DATING_VOLATILE;
}


//--------------------------------------------------------------------
BTsrFromReal::BTsrFromReal(const BDat& dat)
//--------------------------------------------------------------------
    : BTsrTemporary(NCons(new BContensDat(dat)))
{
    datingType_ = DATING_VOLATILE;
}

//--------------------------------------------------------------------
BDat BTsrFromReal::operator[] (const BDate& dte)
//--------------------------------------------------------------------
{
    BDat dat = Dat(Arg(1));
    return(dat);
}

//--------------------------------------------------------------------
BDat BTsrFromReal::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BDat dat = Dat(Arg(1));
    return(dat);
}

//--------------------------------------------------------------------
/* Returns a valid constant time serie for name.
 */
template<>
BSyntaxObject* BUserTimeSerie::FindConstant (const BText& name)
{
    BSyntaxObject* bsyntax = NIL;
    
    BUserDat* uDat = (BUserDat*)(GraReal()->FindConstant(name));
    if(uDat) {
	BUserTimeSerie* uTsr = new BTsrFromReal(NCons(uDat));
	BUserTimeSet* uTms = (BUserTimeSet*)(GraTimeSet()->FindVariable("C"));
	uTsr->PutDating(uTms);
	bsyntax = reinterpret_cast<BSyntaxObject*>(uTsr);
    }
    
    return bsyntax;
}


//--------------------------------------------------------------------
/*! Returns a valid time serie for obj.
 */
template<>
BSyntaxObject* BUserTimeSerie::Casting(BSyntaxObject* obj)
{
    BSyntaxObject* bsyntax = NIL;
    
    if(!obj)                           { return(NIL); }
    if(obj->Grammar()==OwnGrammar())   { return(obj); }
    if(obj->Grammar()==GraReal())
    {
	obj->Do();
	BUserTimeSerie* uTsr = new BTsrFromReal(NCons(obj));
	BUserTimeSet* uTms = (BUserTimeSet*)(GraTimeSet()->FindVariable("C"));
	uTsr->PutDating(uTms);
	bsyntax = reinterpret_cast<BSyntaxObject*>(uTsr);
    }
    
    return bsyntax;
}


//--------------------------------------------------------------------
BTsrPrimary::BTsrPrimary(const BText&    name,
                         const BText&  title  ,
                         BUserTimeSet* dating ,
                         const BDate&  first  ,
                         const BDate&  last   )
//--------------------------------------------------------------------
                         : BUserTimeSerieDo(name)
{
  PutDescription (title);
  PutDating      (dating, true);
  PutFirstDate   (first);
  PutLastDate    (last);
  GetLength();
}

//--------------------------------------------------------------------
BTsrPrimary::BTsrPrimary(const BText&    name,
                         const BText&  title,
                         BUserTimeSet* dating,
                         const BDate&  first,
                         const BDate&  last,
                         const BData&  data)
//--------------------------------------------------------------------
    : BUserTimeSerieDo(name)
{
    PutDescription (title);
    PutDating   (dating, true);
    PutFirstDate(first);
    PutLastDate (last);
    Realloc(data.Size());
    data_=data;
    GetLength();
}

//--------------------------------------------------------------------
BTsrPrimary::BTsrPrimary(const BText&  name,
                         const BText&  title,
                         BUserTimeSet* dating,
                         const BDate&  first,
                         const BData&  data)
                         
/*! BTsrPrimary constructor
 */
//--------------------------------------------------------------------
: BUserTimeSerieDo(name)
{
    PutDescription (title);
    PutDating   (dating, true);
    PutFirstDate(first);
    PutLastDate (dating->Next(first,data.Size()-1));
    PutLength(data.Size());
    Realloc(data.Size());
    data_=data;
}

//--------------------------------------------------------------------
BDat BTsrPrimary::GetDat(const BDate& dte)

/*! Returns the value of this serie in a date if data are charged
 */
//--------------------------------------------------------------------
{
    if(data_.Size()) { return(data_[GetIndex(dte)]); }
    else             { return(BDat::Unknown()); }
}


//--------------------------------------------------------------------
void BTsrPrimary::ReadData()

/*! Read data and dates from file if exists.
 */
//--------------------------------------------------------------------
{
//BDT series is no more lazy
}


//--------------------------------------------------------------------
class BTsrFromYearlyTable : public BTsrTemporary
//--------------------------------------------------------------------
{
private:
    BBool fail_;
    void  ReadData();
    BInt  period_;
    BText fileName_;
    
public:
    BTsrFromYearlyTable(BList* arg);
    ~BTsrFromYearlyTable();
    BDat operator[] (const BDate& dte) { return(GetDat(dte)); }
    BDat GetDat(const BDate& dte);
};
//--------------------------------------------------------------------
DeclareEvaluator(BTsrFromYearlyTable);
//--------------------------------------------------------------------
DefExtOpr(1, BTsrFromYearlyTable, "ReadYearlyTable",3,3,"Text TimeSet Real",
          I2("(Text file, TimeSet dating, Real period)",
          "(Text fichero, TimeSet fechado, Real periodo)"),
          I2("Reads a time series from a yearly-periodic table.",
          "Lee una serie de una tabla periódica anual."),
          BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BTsrFromYearlyTable::BTsrFromYearlyTable(BList* arg)
: BTsrTemporary(arg), fail_(BFALSE)
{
    PutDating(Tms(Arg(2)));
}

BTsrFromYearlyTable::~BTsrFromYearlyTable()
{
}


//--------------------------------------------------------------------
void BTsrFromYearlyTable::ReadData()
//--------------------------------------------------------------------
{
  if(Data().Size() || fail_) { return; }
  fail_ = BTRUE;
  BTextCell cell;
  BText text;
  fileName_ = Text(Arg(1));
  ReadFile(text, fileName_);
  //ReadCell(cell, fileName_);
  ReadAllCells(text,cell,'\n',';');
  period_ = (BInt)Real(Arg(3));
  BInt lag = 12 / period_;
  if(cell.Size()>1)
  {
    BInt i, m, y;
    BInt year;
    BInt firstYear=0;
    BInt lastYear=0;
    BArray<BInt> yearTable(cell.Size()-1);
    for(i=1; i<cell.Size(); i++)
    {
      sscanf(cell[i][0].String(), "%d", &year);
      if((i==1)||firstYear>year) { firstYear=year; }
      if((i==1)|| lastYear<year) {  lastYear=year; }
      yearTable[i-1]=year;
    }
    BDate first(firstYear,  1, 1);
    BDate last ( lastYear, period_, 1);
    BInt  len = (1+lastYear-firstYear)*period_;
    BDat  dat;
    if((len>0)&&first.HasValue()&&last.HasValue())
    {
      PutFirstDate(first);
      PutLastDate (last);
      PutLength       (len);
      Realloc(len);
      for(i=1; i<=len/period_; i++)
      {
        y=yearTable[i-1]-firstYear;
        for(m=1; m<=period_; m++)
        {
          BDat dat;
          dat.PutValue(cell[i][m]);
          data_[y*period_+(m-1)]=dat;
        }
      }
      fail_ = BFALSE;
    }
  }
}
//--------------------------------------------------------------------
BDat BTsrFromYearlyTable::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  if(!fail_)
  {
    if(!Data().HasValue()) { ReadData(); }
    dat = data_[GetIndex(dte)];
  }
  return(dat);
}



//--------------------------------------------------------------------
const BText& BTsrBSI::Description() const
//--------------------------------------------------------------------
{
    BTimeSerie* ser = (BTimeSerie*)this; //Cast problem with gcc
    ser->ReadData();
    return(BTsrPrimary::Description());
}


//--------------------------------------------------------------------
void BTsrBSI::WriteFile (const BText& bsiFileName, BList* serList)

/*! Writes a serie's table into a file with Bayes Serie Index format
 */
//--------------------------------------------------------------------
{
  Deprecated(I2("Files with binary format BSI (Bayes Series Index) "
                "will be unsupported from next TOL version 1.1.6\n"
                "Perhaps you should use an OIS image instead of it.",
                "Los ficheros con formato binario BSI (Bayes Series "
                "Index) no serán admitidos desde la versión 1.1.6\n"
                "Quizás debería usar una imagen OIS en su lugar."));
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
  BBool known;
  // -------------------

  BInt numSer = 0;
  BInt offset = 0;
  fwrite(&numSer,sizeof(BInt),1,bsi);
  while(serList)
  {
    BUserTimeSerie* ser = Tsr(Car(serList));
    if(ser && (ser->Grammar()==OwnGrammar()))
    {
      BText name = ser->Name();
      BUserTimeSet* dating = ser->Dating();
      BText datingName = dating->Identify();
      BDate first = ser->FirstDate();
      BDate last = ser->LastDate();
      
      //    Std(name+" goes from "+first+" to "+last);
      if(!first.HasValue())
      {
        first  = BDate::DefaultFirst();
        first += dating;
        first -= dating;
      }
      if(!last .HasValue())
      {
        last  = BDate::DefaultLast ();
        last += dating;
        last -= dating;
      }
      //    Std(BText(" -> from ")+first+" to "+last+" ;\n");
      
      if(name.HasName() && dating && first.HasValue() && last.HasValue())
      {
        BText desc   = ser->Description();
        BText dating = ser->Dating()->Identify();
        BInt  len    = 1+ser->Dating()->Difference(first,last); //ser->GetLength();
        BData data;
        ser->GetData(data,first,last,len);
        
        name.BinWrite(bsi);
        fwrite(&offset,sizeof(BInt),1,bsi);
        

        offset += desc.BinWrite(bsd);
		offset += datingName.BinWrite(bsd);
		offset += first.BinWrite(bsd);
        offset += last.BinWrite(bsd);
        offset += sizeof(BInt) + (sizeof(value)+sizeof(known))*len;
//                               sizeof(BDat)*len;
		  
        
        fwrite(&len,               sizeof(BInt), 1,   bsd);
		// ------------------------------------------------
		for ( int i = 0; i < len; i++ ) {
		  value = data[i].Value();
		  known = data[i].Known();
		  // aqui deveria ser data[i].BinWrite(bsd);
		  fwrite( &value, sizeof(value), 1, bsd );
		  fwrite( &known, sizeof(known), 1, bsd );
		}
		// ------------------------------------------------
        //fwrite(data.GetBuffer(), sizeof(BDat), len, bsd);
        numSer++;
      }
      else
      {
        Warning(I2(
          "Cannot save into a BSI file a series without name, dating, or "
          "initial and final dates. \n  Series : ",
          "No se puede guardar en un fichero BSI una serie sin nombre, "
          "sin fechado o sin fechas de inicio y final. \n         Serie : ")+
          ser->Identify()+ "\n" + I2("  File : ","  Fichero : ") +
          bsiFileName);
      }
    }
    serList = Cdr(serList);
  };
  fclose(bsd);
  fseek(bsi, 0, SEEK_SET);
  fwrite(&numSer,sizeof(BInt),1,bsi);
  fclose(bsi);
}


//--------------------------------------------------------------------
BList* BTsrBSI::ReadFile(const BText& bsiFileName)

/*! Writes a serie's table into a file with Bayes Serie Index format
 */
//--------------------------------------------------------------------
{
  Deprecated(I2("Files with binary format BSI (Bayes Series Index) "
                "will be unsupported from next TOL version 1.1.6\n"
                "Perhaps you should use an OIS image instead of it.",
                "Los ficheros con formato binario BSI (Bayes Series "
                "Index) no serán admitidos desde la versión 1.1.6\n"
                "Quizás debería usar una imagen OIS en su lugar."));
#ifdef TRACE_LEVEL
  BText fun = BText("BTsrBSI::ReadFile()");
#endif
  TRACE_SHOW_LOW(fun,"Begin");
  static BChar sF = '\t';
  static BChar sR = '\n';
  BList* serList = NIL;
  BList* auxList = NIL;
  TRACE_SHOW_MEDIUM(fun,"1");
  BText bsdFileName = GetFilePath(bsiFileName);
  TRACE_SHOW_MEDIUM(fun,"2");
  if(bsdFileName.HasName() && (bsdFileName.Last()!='/') && (bsdFileName.Last()!='\\'))
  {
    bsdFileName += "/";
  }
  TRACE_SHOW_MEDIUM(fun,"3");
  bsdFileName += GetFilePrefix(bsiFileName)+".bsd";
  TRACE_SHOW_MEDIUM(fun,"4");
  FILE* bsi = fopen(bsiFileName.String(),"rb");
  TRACE_SHOW_MEDIUM(fun,"5");
  if(!bsi)
  {
    TRACE_SHOW_MEDIUM(fun,"6");
    Error(I2("Cannot open for read BSI file ",
      "No se pudo abrir para lectura el fichero BSI ")+
      bsiFileName);
    TRACE_SHOW_LOW(fun,"End Error 1"); 
    return(serList);
  }
  TRACE_SHOW_MEDIUM(fun,"7");
  BText errCF = I2("Corrupted file","Fichero corrupto")+" "+bsiFileName+
                I2(" at function "," en la función ")+" BTsrBSI::ReadFile()"; 

  BText name;
  BInt  offset=0, numSer=0;
  TRACE_SHOW_MEDIUM(fun,"8");
  fread(&numSer,sizeof(BInt),1,bsi);
  TRACE_SHOW_MEDIUM(fun,"9");
  if(numSer<0) { Error(errCF); TRACE_SHOW_LOW(fun,"End Error 2"); return(serList); }
  TRACE_SHOW_MEDIUM(fun,"10");
  for(BInt n=0; n<numSer; n++)
  {
    TRACE_SHOW_MEDIUM(fun,"11");
    if(name.BinRead(bsi)<0) { Error(errCF); TRACE_SHOW_LOW(fun,"End Error 3"); return(serList); }
    fread(&offset,sizeof(BInt),1,bsi);
    TRACE_SHOW_MEDIUM(fun,"12");
    if(offset<0) { Error(errCF); TRACE_SHOW_LOW(fun,"End Error 4"); return(serList); }
    BTsrPrimary* ser = new BTsrBSI(name, bsdFileName, offset);
    TRACE_SHOW_MEDIUM(fun,"13");
    LstFastAppend(serList, auxList, ser);
  };
  TRACE_SHOW_MEDIUM(fun,"14");
  fclose(bsi);
  TRACE_SHOW_LOW(fun,"End Normal");
  return(serList);
}


//--------------------------------------------------------------------
void BTsrBSI::ReadData()
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("BTsrBSI::ReadData()");
#endif
  TRACE_SHOW_LOW(fun,"Begin");
  if(!data_.Size())
  {
    TRACE_SHOW_MEDIUM(fun,"1");
    const char* path = bsdFileName_.String();
    FILE* bsd = fopen(path,"rb");
    TRACE_SHOW_MEDIUM(fun,"2");
    if(!bsd)
    {
      TRACE_SHOW_MEDIUM(fun,"3");
      Error(I2("Cannot open BSD file ","No se pudo abrir el fichero BSD ")+
        bsdFileName_ + "\n(SERIE  = "+ Name()  +")" );
      TRACE_SHOW_MEDIUM(fun,"4");
      Realloc(1);
      TRACE_SHOW_LOW(fun,"ERR 1");
      return;
    }
    TRACE_SHOW_MEDIUM(fun,"5");
    BText errCF = I2("Corrupted file","Fichero corrupto")+" "+path+
                  I2(" at function "," en la función ")+" BTsrBSI::ReadData()"; 
    TRACE_SHOW_MEDIUM(fun,"6");
    BUserTimeSet* dating = NIL;
    BText datingName;
    BDate first, last;
    BInt length=0;
    TRACE_SHOW_MEDIUM(fun,"7");
    if(!fseek(bsd, offset_, SEEK_SET))
    {
      TRACE_SHOW_MEDIUM(fun,"8");
	  // -------------------
	  // con un correcto BDat::BinWrite
	  // no haria falta.
      BReal value;
      BBool known;
      // -------------------
      BText desc="";
      TRACE_SHOW_MEDIUM(fun,"9");
      if(desc      .BinRead(bsd)<0) { Error(errCF); Realloc(1); return; }
      TRACE_SHOW_MEDIUM(fun,"10");
      if(datingName.BinRead(bsd)<0) { Error(errCF); Realloc(1); return; }
      TRACE_SHOW_MEDIUM(fun,"11");
      first        .BinRead(bsd);
      TRACE_SHOW_MEDIUM(fun,"12");
      last         .BinRead(bsd);
      TRACE_SHOW_MEDIUM(fun,"13");
      fread(&length,sizeof(BInt),1,bsd);
      TRACE_SHOW_MEDIUM(fun,"14");
      if(length<0) { Error(errCF); Realloc(1); return; }
      TRACE_SHOW_MEDIUM(fun,"15");
      PutDescription(desc);
      TRACE_SHOW_MEDIUM(fun,"16");
      Realloc(length);
      TRACE_SHOW_MEDIUM(fun,"17");
      //When dating is given as a TimeSet expression withou name
      //a new object will be created and must be incremented it 
      //references to protect it and destroyed with the BTsrBSI
      dating=Tms(GraTimeSet()->EvaluateExpr(datingName));
      if(dating) { dating->IncNRefs(); }
      PutDating(dating);
      if(!(dating && first.HasValue() && last.HasValue() &&
        dating->Contain(first) && dating->Contain(last)))
      {
        Error(I2("Wrong dating or dates in BSI file ",
          "Fechado o fechas erróneos en fichero BSI") + bsdFileName_ +
          " : " + datingName + "[" + first + "," + last+"]");
      }
      TRACE_SHOW_MEDIUM(fun,"18");
      PutFirstDate(first);
      PutLastDate(last);
  	  // -------------------------------------
	    for ( int i = 0; i < length; i++ ) 
      {
	      fread( &value, sizeof(value), 1, bsd );
		    fread( &known, sizeof(known), 1, bsd );
        if(!(known==0) && !(known==1)) { Error(errCF); Realloc(1); return; }
		    data_[i].PutValue( value );
		    data_[i].PutKnown( known );
	    }
      TRACE_SHOW_MEDIUM(fun,"19");
	    // -------------------------------------
      // fread(data_.GetBuffer(),length,sizeof(BDat),bsd);
    }
    else
    {
      Error(I2("Offset seek failure in BSD file ",
        "Fallo en la bsqueda de offset en el fichero BSD ")+
        bsdFileName_+
        "\n(OFFSET = "+ offset_ +")" +
        "\n(SERIE  = "+ Name()  +")" );
    }
    TRACE_SHOW_MEDIUM(fun,"20");
    fclose(bsd);
  }
  TRACE_SHOW_LOW(fun,"End Normal");
}
