/* foread.cpp: Functions of ReadForecastModel
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

#include <tol/tol_bmodel.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bout.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bsetgra.h>

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class ReadForecastModel).
//--------------------------------------------------------------------
BTraceInit("foread.cpp");
struct BBlockTitle { BInt numBlock_; BInt numRead_; const BChar* title_; };
static BBlockTitle blockTitle[] =
{
  {  0,	 1, "SERIE OUTPUT:"},
  {  1,	 2, "VARIABLES INPUT:"},
  {  2,	 2, "VARIABLES AUTOMATICAS:"},
  {  3,	 3, "TRANSFORMACION BOX-COX"},
  {  4,	 4, "DIFERENCIAS"},
  {  5,	 0, "NUMERO DE DATOS"},
  {  6,	 0, "MATRIZ DE CORRELACIONES DE LOS PARAMETROS ESTIMADOS"},
  {  7,	 5, "DENOM.				 STD	 T"},
  {  8,	 0, "SUMA TOTAL DE CUADRADOS . . . . . . . ."},
  {  9,	 0, "NUMERO TOTAL DE OBSERVACIONES . . . . ."},
  { 10,	 0, "SUMA DE CUADRADOS DE LOS RESIDUOS . . ."},
  { 11,	 0, "R-CUADRADO	 . . . . . . . . . . . . . ."},
  { 12,	 0, "NUMERO EFECTIVO DE OBSERVACIONES  . . ."},
  { 13,	 0, "VARIANZA RESIDUAL ESTIMADA	 . . . . . ."},
  { 14,	 6, "ERROR STANDARD RESIDUAL . . . . . . . ."},
  { 15,	 0, "AUTOCORRELACIONES"},
  { 16,	 0, "AUTOCORRELACIONES PARCIALES"}
};

static BInt numBlocks = sizeof(blockTitle) / sizeof(BBlockTitle);

static BArray<BText> block_(numBlocks);	 // blocks of model information

static BArray<BInt>  arrayBIntDeclarator_; // Template Declaration ?!


//--------------------------------------------------------------------
BBool BModel::ReadForecast(const BText& forecastFile)

/*! Reads blocks of information in the model file
 */
//--------------------------------------------------------------------
{
  BBool ok =
    ReadBlocks(forecastFile) &&
    ReadOutName()	     &&
    ReadVariables()	     &&
    ReadTransformation()     &&
    ReadDifference()	     &&
    ReadPolynomials()	     &&
    ReadStandardError();
//Trace(BText("Forecast File Readed ")+Inform());
  return(ok);
}


//--------------------------------------------------------------------
BBool BModel::ReadBlocks(const BText& forecastFile)

/*! Reads blocks of information in the model file
 */
//--------------------------------------------------------------------
{
  ifstream in(forecastFile.String());
  BText forecast(BDir(forecastFile).Bytes()+10);
  if(!in.good())
  {
    Error(forecastFile+I2(" cannot open"," no se puede abrir"));
    return(BFALSE);
  }
  Read(in, forecast);
  const BChar* position = forecast.String();
  const BChar* titlePos = strstr(position,blockTitle[0].title_);
  const BChar* oldPos   = NULL;

  titlePos+=strlen(blockTitle[0].title_)+1;
  BInt n, blockLength=0;
  for(n=0; n<numBlocks-2; n++)
  {
    oldPos = titlePos;
    if(n==13)
    {
      Std(n);
    }
    if((titlePos = strstr(oldPos, blockTitle[n+1].title_)))
    {
      blockLength = titlePos-oldPos-1;
      if(blockTitle[n].numRead_) { block_[n].Copy(oldPos,0,blockLength); }
      titlePos += strlen(blockTitle[n+1].title_);
//    Std(BText("\nBlock[")+n+"] <"+blockTitle[n].title_+"> = \n"+block_[n]);
    }
    else
    {
      titlePos = oldPos;
      if(n!=1)
      {
	Warning(BText(blockTitle[n+1].title_)+
		I2(" not found in "," no se encuentra en ")+
		forecastFile);
      }
      if(blockTitle[n].numRead_) { block_[n]=""; }
    }
  }
  if(oldPos) { blockLength = strlen(oldPos); }
  if(blockTitle[n].numRead_) { block_[n].Copy(oldPos,0,blockLength); }
//Std(BText("\nBlock[")+n+"] <"+blockTitle[n].title_+"> = \n"+block_[n]);
  return(BTRUE);
}


//--------------------------------------------------------------------
BBool BModel::ReadOutName()

/*! Reads
 */
//--------------------------------------------------------------------
{
    BBool ok = BTRUE;
    outName_ = block_[0];
    outName_.Replace(';',' ');
    outName_.Compact();
    if((output_ = Tsr(GraSerie()->FindOperand(outName_,false))))
    {
	    outData_.AddSerie(output_);
    }
//Std(Out()+"\nOutput : "+outName_);
    return(ok);
}


//--------------------------------------------------------------------
BBool BModel::ReadVariables()

/*! Reads
 */
//--------------------------------------------------------------------
{
  BText dating = "C";
  if(output_ && output_->Dating())
  { dating = output_->Dating()->Identify(); }
  BInt n,m;
  BUserTimeSerie* ser;
  BArray<BText> input;
  BArray<BText> automat;

  if(block_[1].HasName())
  {
    ReadAllTokens(Compact(block_[1]), input,   ';');
    ReadAllTokens(Compact(block_[2]), automat, ';');
  }
  else
  {
    ReadAllTokens(Compact(block_[2]), input,   ';');
  }
  inpName_.ReallocBuffer(input.Size()+automat.Size());

  for(n=0; n<input.Size(); n++)
  {
    inpName_[n]=input[n];
    inpName_[n].Compact();
    if((ser = Tsr(GraSerie()->EvaluateExpr(inpName_[n]))))
    {
      inData_.AddSerie(ser);
    }
  }
  for(m=0; m<automat.Size(); m++,n++)
  {
    inpName_[n]=automat[m];
    inpName_[n].Replace("PULSO",    "Pulse(");
    inpName_[n].Replace("COMPENS.", "Compens(");
    inpName_[n].Replace("ESCALON",  "Step(");
    inpName_[n]+=BText(",") + dating + ")";
    inpName_[n].Compact();
    if((ser = Tsr(GraSerie()->EvaluateExpr(inpName_[n]))))
    {
      inData_.AddSerie(ser);
    }
  }

//Std(BText("\nInp : ") + inpName_.Size());
  return(BTRUE);
}


//--------------------------------------------------------------------
BBool BModel::ReadTransformation()

/*! Reads
 */
//--------------------------------------------------------------------
{
  BBool ok = (2==sscanf(block_[3].String(),
			"%lf%lf",
			&firstTransformation_,
			&secondTransformation_));
//Std(Out()+"\nBOX-COX : "+firstTransformation_+", "+secondTransformation_);
  return(ok);
}


//--------------------------------------------------------------------
BBool BModel::ReadDifference()

/*! Reads
 */
//--------------------------------------------------------------------
{
  BBool ok = (2==sscanf(block_[4].String(),
			"%d%d",
			&regularDifference_,
			&seasonalDifference_));
//Std(Out()+"\nDif : "+regularDifference_+", "+seasonalDifference_);
  return(ok);
}


//--------------------------------------------------------------------
class BParameter
//--------------------------------------------------------------------
{
public:
  BText name_;	  //
  BText type_;	  // NUM, AR, MA, ...
  BInt	factor_;  // num of factor in a polynomial
  BInt	order_;	  // degree of this monomial
  BReal value_;	  // coefficient of this monomial
  BPol	mon_;	  // monome
public:
  BParameter() : name_(30), type_(10) {};
  BBool Read(const BText& line);
};


//--------------------------------------------------------------------
BBool BParameter::Read(const BText& line)

/*! BParameter reader
 */
//--------------------------------------------------------------------
{
    if(line.Length()<50) { return(BFALSE); }
    BChar name[128];
    BChar type[128];
    sscanf(line.String()+ 3, "%s",  name); name_ = name;
    sscanf(line.String()+27, "%s",  type); type_ = type;
    sscanf(line.String()+31, "%d",  &factor_);
    sscanf(line.String()+37, "%d",  &order_);
    sscanf(line.String()+43, "%lf", &value_);
    mon_ = BMonome<BDat>(value_, order_);
/*
  Std(BText("\n")+line);
  Std(BText("\nPARAM:(")+name_+";"+type_+";"+
  factor_+";"+order+";"+value+";"+mon_.Name()+")");
*/
    return(BTRUE);
}


//--------------------------------------------------------------------
BBool BModel::ReadPolynomials()

/*! Reads
 */
//--------------------------------------------------------------------
{
  BBool		ok     = BTRUE;
  BInt		numVar = -1;
  BInt		n;
  arFactors_.ReallocBuffer(2); arFactors_[0] = arFactors_[1] = BPol::One();
  maFactors_.ReallocBuffer(2); maFactors_[0] = maFactors_[1] = BPol::One();
  BArray<BText> line;

  ReadAllTokens(block_[7], line, '\n');
  BArray<BParameter> parameter(line.Size());
  transfer_.ReallocBuffer(inpName_.Size());
  for(n=2; (n<line.Size())&&parameter[n].Read(line[n]); n++)
  {
    if((numVar<0) && (parameter[n].type_.Find("ONST")>=0))
    {
      constant_ = parameter[n].value_;
//    Std(BText("\n+")+constant_);
    }
    if(numVar<inpName_.Size())
    {
      if((numVar<0)||
	 (parameter[n].name_!=parameter[n-1].name_) ||
	 (parameter[n].mon_.Degree()==parameter[n-1].mon_.Degree()))
      { numVar++; }
      if(numVar<inpName_.Size())
      {
	transfer_[numVar]+=parameter[n].mon_;
//	Std(BText("\n+")+transfer_[numVar].Name()+" : "+inpName_[numVar]);
      }
    }
    if(numVar>=inpName_.Size())
    {
      BInt f = 0;
      if(parameter[n].mon_.Degree() % periodicity_ == 0) { f = 1; }
      if(parameter[n].type_=="AR")
      {
	 arFactors_[f]-=parameter[n].mon_;
//	 Std(BText("\nFACTOR AR ")+f+ " -> "+arFactors_[f].Name());
      }
      else //if(parameter[n].type_=="MA")
      {
	 maFactors_[f]-=parameter[n].mon_;
//	 Std(BText("\nFACTOR MA ")+f+ " -> "+maFactors_[f].Name());
      }
    }
  }
  BInt staDeg = seasonalDifference_*periodicity_;
  if(regularDifference_)  { dif_*= (BPol::One() -  BPol::X());	       }
  if(seasonalDifference_) { dif_*= (BPol::One() - (BPol::X()^staDeg)); }
  return(ok);
}


//--------------------------------------------------------------------
BBool BModel::ReadStandardError()

/*! Reads
 */
//--------------------------------------------------------------------
{
    BReal x;
    BBool ok = (2==sscanf(block_[14].String(), "%lf", &x));
    standardError_=x;
    return(ok);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetIncludeBFM);
DefExtOpr(1, BSetIncludeBFM, "IncludeBFM", 1, 2, "Text Real",
  I2("(Text fileName, Real periodicity)",
     "(Text nombreFichero, Real periodicidad)"),
  I2("Returns a set with the model in a Bayes-Forecast Model (BFM) file.",
     "Devuelve un conjunto con el modelo del fichero con formato "
     "Bayes-Forecast Model (BFM)."),
	  BOperClassify::FileIncluding_);
//--------------------------------------------------------------------
void BSetIncludeBFM::CalcContens()
//--------------------------------------------------------------------
{
    BInt	periodicity = 0;
    if(Arg(2)) { periodicity = (BInt)Real(Arg(2)); }
    BModel mod("", TolPath(), periodicity);
    contens_.RobElement(mod.GetDefinitionList());
}
