/* modprev.cpp: Functions of BModel
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
#include <tol/tol_bstat.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bstruct.h>

BTraceInit("modprev.cpp");

//--------------------------------------------------------------------
BTsrPrimary* BModel::TransPrev(const BDate&	 first,
			       const BDate&	 last,
			             BInt	 num,
			             BMatrix<BDat>& aCov)
//--------------------------------------------------------------------
{

    BInt h,i,j,k,l;
    BInt		   dp	 = p_+difDeg_;
    BUserTimeSerie*  out	 = output_;
    BUserTimeSet*	   tms	 = out->Dating();	  //TRZ((*tms));
    BDate		   fi	 = tms->Successor(last);  //TRZ(fi);
    BDate		   la	 = tms->Next(last,num);	  //TRZ(la);
    BText		   pN	 = "Transformed prevision of ";
    BRat		   psiR	 = ma_ / ari_;
    BPol		   psi	 = psiR % num;
    BDat		   var	 = standardError_*standardError_;
    BArray<BDat>	   DFi(dp+1);
    BArray<BDat>	   Psi(num+1);
    
    for(i=0; i<=dp;  i++) { DFi[i] = -ari_.Coef(i); }
    for(i=0; i<=num; i++) { Psi[i] =  psi .Coef(i); }
    if(!firstDate_.HasValue() || (fi<firstDate_) || (last>lastDate_))
    {
	InitData(first,last,num);
	CheckData();
	CalcInputDifFilter(N_);
	CalcNonLinnearInputDifFilter(N_);
	CalcDifNoise(N_);
	standardError_ = FrobeniusNorm(residuous_);
	var = standardError_*standardError_;
    };
    
/*
  Std(BText("\n\n Residuous (")+residuous_.Size()+")");
  for(i=0;i<residuous_.Size();i++) { Std(residuous_[i].Name()); }

  Std(BText("\n\n Noise (")+difNoise_.Size()+")");
  for(i=0;i<difNoise_.Size();i++) { Std(difNoise_[i].Name()); }
  
  Std(BText("\n\n inputDifFilter_ (")+inputDifFilter_.Size()+")");
  for(i=0;i<inputDifFilter_.Size();i++) { Std(inputDifFilter_[i].Name()); }
  
  Std(BText("\n\n standardError_ = ")+standardError_);
*/
    BTsrPrimary* ser = new BTsrPrimary("", pN + out->Identify(), tms, fi, la);
    ser->PutName(out->Identify()+"TransPrev");
    BArray<BDat>difPrev(num);
    ser->Realloc(num);
    BInt t0 = residuous_.Size()-tms->Difference(lastDate_,last);
    
    for(h=1; h<=num; h++)
    {
	difPrev[h-1]=0.0;
	for(i=1;i<=p_;i++)
	{
	    if(i<h) { difPrev[h-1]+=Fi_[i]*difPrev[-1+h-i]; }
	    else    { difPrev[h-1]+=Fi_[i]*difNoise_ [t0+h-i]; }
	}
	for(i=h;i<=q_;i++)
	{
	    difPrev[h-1]-=Teta_[i]*residuous_[t0+h-i];
	}
    }
    for(h=1; h<=num; h++)
    {
	difPrev[h-1]+=inputDifFilter_[t0+h];
    }
    
//Std(BText("\n\n difPrev (")+difPrev.Size()+")");
//for(i=0;i<difPrev.Size();i++) { Std(difPrev[i].Name()); }
    
    BArray<BDat>& tran = fullData_;
    t0+=tran.Size()-difNoise_.Size();
    for(h=1; h<=num; h++)
    {
      BDat aux = difPrev[h-1];
	for(k=1;k<dif_.Size();k++)
	{
	    j = h-dif_[k].Degree();
	    if(j>0) { aux-=ser->Data()[j-1] *dif_[k].Coef(); }
	    else    { aux-=tran[t0+j]*dif_[k].Coef(); }
	}
	ser->PutBufDat(h-1,aux);
    }
    
    aCov.Alloc(num,num);
    for(l=1;l<=num;l++)
    {
	for(k=l;k<=num;k++)
	{
	    aCov(l-1,k-1) = 0;
	    for(i=0;i<l;i++)
	    {
		aCov(l-1,k-1) += Psi(i)*Psi(i+k-l);
	    }
	    aCov(l-1,k-1) *= var;
	    aCov(k-1,l-1)  = aCov(l-1,k-1);
	}
    }
    return(ser);
}


//--------------------------------------------------------------------
void BModel::PrevisionSeries(const BText&	    serieName,
			     const BUserTimeSerie*  transPrev,
			     const BMatrix<BDat>&   aCov,
			           BUserTimeSerie** prev,
			           BUserTimeSerie** transError,
			           BUserTimeSerie** error,
			           BUserTimeSerie** upperBand,
			           BUserTimeSerie** lowerBand)
//--------------------------------------------------------------------
{
  BUserTimeSet* tms = transPrev->Dating();
  BDate		f   = transPrev->FirstDate();
  BDate		l   = transPrev->LastDate();
  BInt		num = transPrev->Data().Size();

  *prev = new BTsrPrimary("",BText("Prevision of ")+serieName,
			  tms,f,l);
  (**prev).PutName(BText("Prev")+serieName);
  (**prev).Realloc(num);

  *transError = new BTsrPrimary("",BText("Transformated error of ")+serieName,
				tms,f,l);
  (**transError).PutName(BText("TransError")+serieName);
  (**transError).Realloc(num);

  *error = new BTsrPrimary("",BText("Error of ")+serieName,
			   tms,f,l);
  (**error).PutName(BText("Error")+serieName);
  (**error).Realloc(num);

  *upperBand = new BTsrPrimary("",BText("Upper band error of ")+serieName,
			       tms,f,l);
  (**upperBand).PutName(BText("UpperBand")+serieName);
  (**upperBand).Realloc(num);

  *lowerBand = new BTsrPrimary("",BText("Lower band error of ")+serieName,
			       tms,f,l);
  (**lowerBand).PutName(BText("LowerBand")+serieName);
  (**lowerBand).Realloc(num);

  for(BInt i=0; i<num; i++)
  {
    BDat trPre = (*transPrev).Data()[i];
    BDat pre   = InvTransfor(trPre);
    BDat trErr = Sqrt(aCov(i,i));
    BDat err   = InvTransfor(trErr);
    (**prev	 ).PutBufDat(i,pre);
    (**transError).PutBufDat(i,trErr);
    (**error	 ).PutBufDat(i,err);
    (**upperBand ).PutBufDat(i,InvTransfor(trPre+trErr));
    (**lowerBand ).PutBufDat(i,InvTransfor(trPre-trErr));
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetPrevision);
DefExtOpr(1, BSetPrevision, "TransPrev", 4, 4, "Set Date Date Real",
  I2("(Set model, Date from, Date until, Real previsionNumber)",
     "(Set modelo, Date desde, Date hasta, Real numeroPrevisiones)"),
  I2("Returns the prevision serie and its covarianze matrix of a model from "
     "a date with a determined long",
     "Devuelve la serie de previsiones y su matriz de covarianzas de un "
     "modelo a partir de una fecha y con una longitud determinada."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetPrevision::CalcContens()
//--------------------------------------------------------------------
{
    BStruct* str = FindStruct("NormalDistSerie");
    
    BMat	 mat;
    BSet&	 set   = Set (Arg(1));
    BDate&       first = Date(Arg(2));
    BDate&       last  = Date(Arg(3));
    BInt	 num   = (BInt)Real(Arg(4));
    BModel       mod   = set;
    BTsrPrimary* prev  = mod.TransPrev(first, last, num, mat);
    BUserMat*    uMat  = BContensMat::New("",mat,"Cov");
    contens_.RobStruct(Cons(prev, NCons(uMat)),str, BSet::Structured);
}
