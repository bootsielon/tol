/* modcalc.cpp: Functions of BModel.
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
#include <tol/tol_matrix.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bstat.h>
#include <tol/tol_brealfun.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bsparse.h>
#include <tol/tol_bsvdsep.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_nan.h>

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BModel).
//--------------------------------------------------------------------
BTraceInit("model.cpp");

//--------------------------------------------------------------------
void BModel::ConstructARMAFromRoots()
//--------------------------------------------------------------------
{
  BInt i, j;
  ar_ = BPol::One();
  ma_ = BPol::One();

  arParam_ = 0;
  maParam_ = 0;
  lagrangianParam_ = 0;
//TRZ(ar_);TRZ(ma_);
  for(j=i=0; i<arFactors_.Size(); i++, j++)
  {
  //TRZ(ar_);TRZ(ma_);
  //Std(BText("\nAR[")+i+"]="+arFactors_(i).Name());
    if(arFactors_(i).Degree()>0) 
    { 
      lagrangianParam_ ++; 
      ar_*=arFactors_(i);
      arParam_+=arFactors_[i].Size()-1;
    }
  }
  for(i=0; i<maFactors_.Size(); i++, j++)
  {
  //TRZ(ar_);TRZ(ma_);
  //Std(BText("\nMA[")+i+"]="+maFactors_(i).Name());
    if(maFactors_(i).Degree()>0) 
    { 
      lagrangianParam_ ++; 
      ma_*=maFactors_(i);
      maParam_+=maFactors_(i).Size()-1;
    }
  }
  ari_	  = dif_ * ar_;
  p_	    = ar_.Degree();
  q_	    = ma_.Degree();
  maxpq_  = Maximum(p_,q_);
  difDeg_ = dif_.Degree();

  teta1_ = 0.0; for(i=0; i<ma_.Size(); i++) { teta1_ += ma_(i).Coef(); }
  fi1_	 = 0.0; for(i=0; i<ar_.Size(); i++) { fi1_   += ar_(i).Coef(); }
  ari_=ar_*dif_;

/*
  TRZ(ar_);TRZ(ma_);
  Std(BText("\np       = ")+p_);
  Std(BText("\tq       = ")+q_);
  Std(BText("\tmaxpq   = ")+maxpq_);
  Std(BText("\tarParam = ")+arParam_);
  Std(BText("\tmaParam = ")+maParam_);
*/
}


//--------------------------------------------------------------------
BDat BModel::Transfor(BDat x) const
//--------------------------------------------------------------------
{
  x+=secondTransformation_;
  if(firstTransformation_!=1)
  { 
    if(firstTransformation_) { x^=firstTransformation_; }
	   else 		                  { x=Log(x); }
  }
  return(x);
}


//--------------------------------------------------------------------
BDat BModel::InvTransfor(BDat x) const
//--------------------------------------------------------------------
{
  if(firstTransformation_!=1)
  {
    if(firstTransformation_) { x^=1/firstTransformation_; }
    else		                   { x=Exp(x); }
  }
  x-=secondTransformation_;
  return(x);
}


//--------------------------------------------------------------------
static void MatFromPolB(BLowTrMatrix<BDat>& mat,
                  			   const BPol& pol, BInt n)

/*! Constructs a left triangular matrix from a B-polyn with n rows
 *  where the (i,j) element is the coefficient of B^(j-i) for each
 *  j>=i and 0 for each j<i
 */
//--------------------------------------------------------------------
{
  BInt i,j;
  mat.Alloc(n);
  for(i=0; i<n; i++) { mat(i,0) = pol.Coef(i); }
  for(i=1; i<n; i++) { for(j=1; j<=i; j++) { mat(i,j)=mat(j-i,0); } }
}


//--------------------------------------------------------------------
static void MatFromPolF(BUpTrMatrix<BDat>& mat,
			const BPol& pol, BInt n)

/*! Constructs a right triangular matrix from a F-polyn with n rows
 *  where the (i,j) element is the coefficient of B^(i-j) for each
 *  i>=i and 0 for each i<j
 */
//--------------------------------------------------------------------
{
  BInt i,j;
  mat.Alloc(n);
  for(j=0; j<n; j++) { mat(0,j) = pol.Coef(-j); }
  for(j=1; j<n; j++) { for(i=1; i<=j; i++) { mat(i,j) = mat(0,i-j); } }
}


//--------------------------------------------------------------------
BBool ApplyPolB(BArray<BDat>& ps, const BArray<BDat>& s,
		const BPol& p_, BInt deg, const BText& name, int maxTransferDegree)
//--------------------------------------------------------------------
{
  BInt j, k;
  BBool ok = BTRUE;
//Std(BText("Applying ")+p_.Name()+" to "+name+"\n");
  for(j=0; j<ps.Size(); j++)
  {
    ps(j)=0;
    for(k=0; k<p_.Size(); k++)
    {
      BInt jj = j + deg - p_(k).Degree();
      ps(j)+=s[jj]*p_(k).Coef();
//    Std(BText("    s[")+jj+"]="+s[jj].Name()+"\n");
    }
//  Std(BText("	 ps[")+j+"]="+ps[j].Name()+"\n");
    if(ok && (j>=maxTransferDegree) && ps(j).IsUnknown())
    {
      Error(name+ I2(" has unknown data at ",
		     " tiene valores desconocidos en ")+ j);
      ok  = BFALSE;
    }
  }
  return(ok);
}


//--------------------------------------------------------------------
static BBool ApplyPolB(BMat& ps, int rPs, const BMat& s, int rS,
		       const BPol& p_, BInt deg, const BText& name, int maxTransferDegree)
//--------------------------------------------------------------------
{
  BInt j, k;
  BBool ok = BTRUE;
//Std(BText("Applying ")+p_.Name()+" to "+name+"\n");
  for(j=0; j<ps.Columns(); j++)
  {
    ps(rPs,j)=0;
    for(k=0; k<p_.Size(); k++)
    {
      BInt jj = j + deg - p_(k).Degree();
      ps(rPs,j)+=s.Get(rS,jj)*p_(k).Coef();
//    Std(BText("    s[")+jj+"]="+s[jj].Name()+"\n");
    }
//  Std(BText("	 ps[")+j+"]="+ps[j].Name()+"\n");
    if(ok && (j>=maxTransferDegree) && ps(rPs,j).IsUnknown())
    {
      Error(name+ I2(" has unknown data at ",
		     " tiene valores desconocidos en ")+ j);
      ok  = BFALSE;
    }
  }
  return(ok);
}


//--------------------------------------------------------------------
BBool ApplyPolB(BMat& ps, int row, const BData& s,
		const BPol& p_, BInt deg, const BText& name, int maxTransferDegree)
//--------------------------------------------------------------------
{
  BInt j, k;
  BBool ok = BTRUE;
//Std(BText("Applying ")+p_.Name()+" to "+name+"\n");
  for(j=0; j<ps.Columns(); j++)
  {
    ps(row,j)=0;
    for(k=0; k<p_.Size(); k++)
    {
      BInt jj = j + deg - p_(k).Degree();
      ps(row,j)+=s[jj]*p_(k).Coef();
//    Std(BText("    s[")+jj+"]="+s[jj].Name()+"\n");
    }
//  Std(BText("	 ps[")+j+"]="+ps[j].Name()+"\n");
    if(ok && (j>=maxTransferDegree) && ps(row,j).IsUnknown())
    {
      Error(name+ I2(" has unknown data at ",
		     " tiene valores desconocidos en ")+ j);
      ok  = BFALSE;
    }
  }
  return(ok);
}


//--------------------------------------------------------------------
BBool BModel::InitData(const BDate& f, const BDate& l, BInt numPrev)

/*! Initializes the matrix of inputs and transformed output data.
 */
//--------------------------------------------------------------------
{
  calculatingJacobian_ = BFALSE; 
  if(aborted_) { return(BFALSE); }
  BTimer tm(I2("Data charging","Carga de datos"));
  BBool ok = BTRUE;
  BInt i, k;

  firstDate_= output_->Dating()->FirstNoLess (f);
  lastDate_ = output_->Dating()->FirstNoGreat(l);
  maxTransferDegree_ = 0;
  for(i=0; i<transfer_.Size(); i++)
  {
    BInt deg = transfer_(i).Degree();
    if(maxTransferDegree_<deg) { maxTransferDegree_ = deg; }
  }
//maxTransferDegree_+=p_;

  Std(I2("\nCharging data for output serie between ",
	 "\nCargando datos de la serie output entre ") +
	 firstDate_.Name() + I2(" and "," y ") + lastDate_.Name());

  outData_.Fill(firstDate_ , lastDate_);
  ok = outData_.Dating()!=NIL;
  if(!ok)
  {
  }
  if(ok)
  {
  //TRZ(outData_.Data());
    transOutData_.ReallocBuffer(outData_.NumDates());

    for(i=0;i<transOutData_.Size();i++)
    {
      transOutData_(i)=Transfor(outData_.Data()(0,i));
    //Std(BText("\n")<<outData_.Date(i+1)<<"; "<< outData_.Data()(0,i) <<transOutData_(i));
    }
  //TRZ(outData_.Data().Traspose());
    int h,h0;
    BDat avr = Average(transOutData_);
  //TRZ(avr);
    if(avr.IsUnknown()) { avr = 0; }
    interruptionIndex_ .ReallocBuffer(outData_.NumDates());
    interruptionValue_ .ReallocBuffer(outData_.NumDates());
    BArray<BInt> preInterruptionIndex(outData_.NumDates());
    fullData_ = transOutData_;
    BDate d = firstDate_;
    for(h=h0=i=0;i<transOutData_.Size();i++)
    {
      interruptionValue_ (i) = BDat::Unknown();
      if(transOutData_(i).IsUnknown())
      {
      //if(0)
      //if(i>p_+difDeg_)
      //if(i>difDeg_)
        if(i>=0)
	       {
        //Std(BText("\nValid Interruption at ")+d);
	         interruptionValue_  (i)   = avr;
           fullData_           (i)   = avr;
	         interruptionIndex_  (h++) = i;
	       }
	       else
	       {
           Std(BText("\nInvalid Interruption at ")+d);
	             preInterruptionIndex (h0++) = i;
           fullData_             (i)    = 0;
	       }
	       d += output_->Dating();
      }
    }
    interruptionIndex_	 .ReallocBuffer(h);
    preInterruptionIndex .ReallocBuffer(h0);

    if(h)
    {
      Warning(I2("There are ", "Hay ")+h+
	      I2(" interruptions in transformed output.",
		        " interrupciones en la serie output transformada."));
      DI_ = new double[h];
    } 
    else { DI_ = NULL; }

  //Std(BText("\ntransOutData_ = \n")+Name(transOutData_));
    systemInputParam_ = h0;
    if(ok)
    {
/*
      for(i=0;i<transOutData_.Size();i++)
      {
        Std(BText("\n")<<outData_.Date(i+1)<<"; "<< 
                         outData_.Data()(0,i) << "; " << 
                         transOutData_(i) << "; " << 
                         fullData_(i) << "; ");
      }
*/
      N_ = outData_.NumDates()-difDeg_;
      outDifData_.Alloc(1, N_);
      ApplyPolB(outDifData_, 0, fullData_, dif_, difDeg_, outName_+"_Full", 0);
      Z_ = outDifData_.T();
      initialError_ = Sqrt(CenterMoment(outDifData_.Data(),2));
      standardError_ = initialError_;
      Std(I2("\nDifferenced output norm : ",
	     "\nNorma de la serie output diferenciada : ")+initialError_);
    }
  //TRZ(outDifData_.T());

    firstForInput_ = output_->Dating()->Prev(firstDate_, maxTransferDegree_);
    lastForInput_  = output_->Dating()->Next(lastDate_,	 numPrev);
  //lastForInput_  = BDate::End();

    k=transfer_.Size();
    transfer_.ReallocBuffer(k+h0);
    inpName_.ReallocBuffer(k+h0);
    BInt ti;
    if(h0)
    {
      for(i=0; i<h0; i++)
      {
	       ti = preInterruptionIndex(i);
	       BDate d = output_->Dating()->Next(firstDate_,ti);
         Std(BText("\n Adding Interruption at ")+d);
	       transfer_(k) = BPol::One();
	       inpName_ (k) = BText("Pulse("+d+","+output_->Dating()->Name()+")");
	       BUserTimeSerie* s = Tsr(GraSerie()->EvaluateExpr(inpName_ (k)+";"));
         inData_.AddSerie(s);
	       k++;
      }
    }

    if(inData_.NumSeries())
    {
      Std(I2("\nCharging data for input series between ",
	     "\nCargando datos de series input entre ") +
	     firstForInput_.Name() + I2(" and "," y ") + lastForInput_.Name());

      inData_.Fill(firstForInput_, lastForInput_);

      ok = inData_.TestDates();
      if(ok)
      {
        BSerieGroup group;
        group.AddGroup(inData_);
        group.AddGroup(outData_);
        group.PrepareOperate();
        ok = group.Dating()!=NULL;
      }   
      if(!ok)
      {
        Error(I2("Incompatible dating on output and/or inputs series. "
                 "Estimate was unable to retrieve data for input time series.",
                 "Fechados incompatibles en las series output y/o input."
                 "No se obtuvieron los datos de las series input."));
      }
      else
      {
        inDifData_.Alloc(transfer_.Size(), inData_.NumDates()-difDeg_);
       	lastForInput_= inData_.MinLast();
      //TRZ(inData_.Data().T());
	       BInt n = 0;
	       if(transfer_.Size())
	       {
	         for(i=0; i<transfer_.Size(); i++)
	         {
	           n+=transfer_(i).Size();
	           if(!ApplyPolB(inDifData_,i,inData_.Data(),i,dif_,
		          difDeg_,inpName_(i),maxTransferDegree_))
	           { ok = BFALSE; }
	         }
	       }
      //TRZ(inDifData_.T());
      }
    }
  }
  aborted_ = !ok;
  GetInfoParam();
  return(ok);
}


//--------------------------------------------------------------------
void BModel::GetInfoParam()

/*! Checks data searching linnear relations between differenced inputs
 */
//--------------------------------------------------------------------
{
  if(aborted_) { return; }
  BInt j,k,p;
  numParam_ = 0;
  ConstructARMAFromRoots();
  CalcNonLinnearInputDifFilter(N_);
  numParam_ += arParam_ + maParam_;// + lagrangianParam_;
  numParam_ += nonLinPar_.Card();
  if(constant_.IsKnown()) { numParam_++; }
  inputParam_ = 0;
  for(j=0; j<transfer_.Size(); j++)
  {
//  Std(BText("\n  transfer_[")+j+"] = " + transfer_(j).Name());
    inputParam_ +=transfer_(j).Size();
  }
  numParam_+=inputParam_;
  BUserFunction* uFun = (BUserFunction*)(Code(nonLinFun_).Operator());
  infoParam_.ReallocBuffer(numParam_);
  p=0;
  if(constant_.IsKnown()) { infoParam_(p++) = "Constant";  }
  for(j=0; j<transfer_.Size(); j++)
  {
    for(k=0;k<transfer_(j).Size();k++)
    {
      infoParam_(p++)= BText("(B^")+transfer_(j)(k).Degree()+"):("+
                   		       inpName_(j)+")";

    }
  }
  for(j=1; j<=nonLinPar_.Card(); j++)
  {
    infoParam_(p++)=uFun->ArgName(j-1);
  }
  for(j=0; j<arFactors_.Size(); j++)
  {
    for(k=1; k<arFactors_(j).Size(); k++)
    {
      infoParam_(p++) = BText("AR factor ")+(j+1)+" degree "+
                       	arFactors_(j)(k).Degree();
    }    
  }
  for(j=0; j<maFactors_.Size(); j++)
  {
    for(k=1; k<maFactors_(j).Size(); k++)
    {
      infoParam_(p++) = BText("MA factor ")+(j+1)+" degree "+
   			                  maFactors_(j)(k).Degree();
    }
  }
/*
  for(j=0; j<infoParam_.Size(); j++)
  {
    Std(BText("\nParameter(")+j+") : "+infoParam_(j));
  }
*/
}


//--------------------------------------------------------------------
BBool BModel::CheckData()

/*! Checks data searching linnear relations between differenced inputs
 */
//--------------------------------------------------------------------
{
  if(aborted_) { return(BFALSE); }
  BTimer tm(I2("Input data checking for ",
	       "Test de datos input para ")+ inputParam_+" variables ... ");

  BInt i,j,k,r,t;
  BBool ok = BTRUE;

  BArray<BDat> MY(inputParam_);
  BInt d  = inData_.NumDates() - interruptionValue_.Size();
  BBool foundInterruptionsInitialValues_ = BFALSE;
  BInt intVal = 0;
  for(r=j=0; j<transfer_.Size(); j++)
  {
    for(k=0; k<transfer_(j).Size(); k++, r++)
    {
      MY(r) = 0;
      intVal = 0;
      for(t=0; t<interruptionValue_.Size(); t++)
      {
       if(interruptionValue_(t).IsUnknown())
        {
          BInt t0 = t+d-transfer_(j)(k).Degree();
        //Std(BText("\n")<<j<<" "<<k<<" "<<transfer_(j)(k).Degree()<<" "<<t<<" "<<t0<<" "<<inData_.NumDates());
          MY(r) += (inData_.Data()(j,t0)^2.0);
        }
        else
        {
          intVal++;
        }
      }
    //Std(BText("\nNorma((B^")+transfer_(j)(k).Degree()+")"+inpName_(j)+")^2 = "+MY(r));
    }
    if(inpName_(j)=="InterruptionsInitialValues_") 
    {
    //TRZ(BDat(interruptionIndex_.Size()));  TRZ(BDat(intVal)); TRZ(MY(r-1)); TRZ(transfer_(j));
      if(foundInterruptionsInitialValues_)
      {
        Error(inpName_(j)+I2(" is duplicated."," está duplicado."));
        continue;
      }
      if(transfer_(j).Degree())
      {
        Error(inpName_(j)+I2(" transfer function must be 1.",
                             " la función de transferencia debe ser 1."));
        continue;
      }
      foundInterruptionsInitialValues_=BTRUE;
      if((interruptionIndex_.Size()==intVal) && (MY(r)<=DEpsilon()))
      {
        Std(inpName_(j)<<I2(" is found."," se ha encontrado."));
        for(t=0; t<interruptionValue_.Size(); t++)
        {
          if(!interruptionValue_(t).IsUnknown())
          {
            BInt t0 = t+d-transfer_(j)(0).Degree();
            fullData_(t) = interruptionValue_(t) = inData_.Data()(j,t0);
          //Std(BText("\ninterruptionValue_(")<<t<<")="<<interruptionValue_(t));
          }
        }
      }
      else
      {
        Warning(inpName_(j)+
                I2(" is found but it doesn't have values just at interruptions.",
                   " se ha encontrado pero no tiene valores sólo para las interrupciones."));
      }
    }
  }
  if(inputParam_)
  {
    BMat X(N_,inputParam_);
    X.SetAllValuesTo(0);
    d  = inDifData_.Columns() - N_;
    for(r=j=0; j<transfer_.Size(); j++)
    {
      for(k=0; k<transfer_(j).Size(); k++, r++)
      {
	       for(t=0; t<X.Rows(); t++) { X(t,r) = inDifData_(j,t+d-transfer_(j)(k).Degree()); }
      }
    }
    
    BDat cor;

    BSymMatrix<BDat> M = MtMSqr(X);
    eliminated_.AllocBuffer(M.Rows());

    for(r=i=0; i<M.Rows(); i++)
    {
      eliminated_(i) = (MY(i)<=DEpsilon()) || (M(i,i)<=DEpsilon());
      BText reason;
      if(eliminated_(i)) { reason = I2("is null","es nula"); }
      else if(i<M.Rows()-systemInputParam_)
      {
        for(j=i+1; (!eliminated_(i))&&(j<M.Rows()); j++)
        {
	         cor = M(i,j) / Sqrt(M(i,i)*M(j,j));
	         eliminated_(i) = (Abs(1.0+cor-1.0) == 1);
	         if(eliminated_(i))
	         {
	           reason = I2("has unitary correlation with the variable ",
		                      "tiene correlación unitaria con la variable ")+
		          infoParam_(j);
	         }
	       //else { Std(BText("\nCor(")+infoParam_(i)+","+infoParam_(j)+"="+cor); }
        }
      }
      if(eliminated_(i))
      {
        r++;
        Std(BText("\n")+r+" ) "+
        I2("The variable ","La variable ")+infoParam_(i)+" "+
	       I2("will be eliminated because ","será eliminada porque ")+reason);
      }
    }
    }
  numParam_    -= r;
  inputParam_  -= r;

  if(!numParam_)
  {
    ok = BFALSE;
    aborted_ = BTRUE;
    Error(I2("It can not be estimated a model with no variables. "
	            "Introduce any ARMA parameter or any input series.",
	            "No se puede estimar un modelo sin variables."
	            "Introduzca algún parámetro ARMA o una serie input."));
  }
  param_.ReallocBuffer(numParam_);
  if(ok && (N_<(interruptionIndex_.Size()+numParam_)))
  {
    ok = BFALSE;
    aborted_ = BTRUE;
    Error(I2("Variables and interruptions number exceeds data number.\n"
               "The ARIMA model was NOT created.",
             "El número de variables e interrupciones excede al de datos.\n"
             "No ha sido posible crear el modelo ARIMA."));
  }
  if(ok && (N_<=p_+q_))
  {
    ok = BFALSE;
    aborted_ = BTRUE;
    Error(I2("ARIMA degrees exceeds data number.\n"
             "The ARIMA model was NOT created.",
             "Los grados ARIMA exceden al de datos.\n"
             "No ha sido posible crear el modelo ARIMA."));
  }
  return(ok);
}


//--------------------------------------------------------------------
static void InitPolyn(      BInt		 pq,
		                    const BPol&		 pi,
		                    const BPol&		 it,
		                    const BPol&		 h,
		                    const BDat*		 buf,
			                         BMatrix<BDat>&	 L,
			                         BMatrix<BDat>&	 LTPiR)
//--------------------------------------------------------------------
{
  BLowTrMatrix <BDat> Pi;  MatFromPolB(Pi, pi, pq);    //TRZ(Pi);
  BLowTrMatrix <BDat> IT;  MatFromPolB(IT, it, pq);    //TRZ(IT);
  BUpTrMatrix  <BDat> H;   MatFromPolF(H , h , pq);    //TRZ(H);
  BMatrix	     <BDat> R(pq, 1, buf);	       //TRZ(R);

  L = IT*H;

  BMatrix  <BDat> LT = L.T();			//TRZ(LT);
  BMatrix  <BDat> LTPi;
  LTPi	 = LT*Pi;				       //TRZ(LTPi);
  LTPiR = LTPi*R;				       //TRZ(LTPiR);
}


//--------------------------------------------------------------------
void BModel::CalcInputFilter(BInt N)

/*! Initializes the matrix of input and output data.
 */
//--------------------------------------------------------------------
{
  BInt i,j,k,t;
  BInt size = inData_.Data().Columns();
  inputFilter_.ReallocBuffer(size);
  systemInputFilter_.ReallocBuffer(size);
  BDat c = 0; if(constant_.IsKnown()) { c=constant_; }
  for(j=0; j<size; j++) { inputFilter_(j)=c; systemInputFilter_(j)=0; }
  if(inData_.Data().Rows())
  {
    if(inData_.NumSeries())
    {
      //Std("\nmaxTransferDegree_= "+ maxTransferDegree_);
      //Std("\nsize= "+ size);
      effectData_.Alloc(transfer_.Size(),size);
      effectData_.SetAllValuesTo(0);
      for(i=j=0; i<transfer_.Size(); i++)
      {
      //Std(BText("\n  transfer_[")+i+"] = " + transfer_(i).Name());
	       for(k=0; k<transfer_(i).Size(); k++, j++)
	       {
	         if(!eliminated_(j))
	         {
	           BInt d = transfer_(i)(k).Degree();
	           BDat c = transfer_(i)(k).Coef();
	           for(t=d; t<size; t++)
	           {
	             BDat x = inData_.Data().Get(i,t-d)*c;
	             effectData_(i,t) += x;
	           }
	         }
	       }
	       if(i<transfer_.Size()-systemInputParam_)
	       { for(t=0; t<size; t++) { inputFilter_	    (t)+=effectData_(i,t); } }
	       else
	       { for(t=0; t<size; t++) { systemInputFilter_(t)+=effectData_(i,t); } }
      }
    }
  //TRZ(inputData_);
  //TRZ(effectData_);
  //Std(BText("\ninputFilter_std err :")+FrobeniusNormU(inputFilter_));
  //Std("\ninputFilter_\n");
  //for(j=0; j<size; j++) { Std(inputFilter_[j].Name()+"\n"); }
  }
}


//--------------------------------------------------------------------
void BModel::CalcInputDifFilter(BInt N)

/*! Initializes the diferenced matrix of input and output data.
 */
//--------------------------------------------------------------------
{
  BInt i,j,k,r,t;
  if(inputParam_)
  {
    BInt size = inDifData_.Columns()-maxTransferDegree_;
    inputDifFilter_.ReallocBuffer(size);
    BDat c = 0; if(constant_.IsKnown()) { c=constant_; }
    for(j=0; j<size; j++) { inputDifFilter_(j)=c; }
    if(inData_.NumSeries())
    {
      //Std("\nmaxTransferDegree_= "+ maxTransferDegree_);
      //Std("\nsize= "+ size);
      for(r=i=0; i<transfer_.Size(); i++)
      {
      //Std(BText("\nCalcInputDifFilter	 transfer_[")+i+"] = " + transfer_(i).Name());
	       for(k=0; k<transfer_(i).Size(); k++, r++)
	       {
	         if(!eliminated_(r))
	         {
	           BInt d = transfer_(i)(k).Degree();
	           BDat c = transfer_(i)(k).Coef();
          //Std(BText("\ntransfer_(")+i+")("+k+").Coef()="+c);
	           for(t=0; t<size; t++)
	           {
	             BInt tt = t+maxTransferDegree_ - d;
	             inputDifFilter_(t) += inDifData_(i,tt)*c;
	           }
          //Std(BText("\ninputDifFilter_std err :")+FrobeniusNormU(inputDifFilter_));
	         }
	       }
      }
    }
  //Std("\ninputDifFilter_\n");
  //for(j=0; j<size; j++) { Std(inputDifFilter_[j].Name()+"\n"); }
  }
}


//--------------------------------------------------------------------
void BModel::CalcNonLinnearInputDifFilter(BInt N)

/*! Initializes the diferenced matrix of input and output data.
 */
//--------------------------------------------------------------------
{
  BInt j;
  if(nonLinPar_.Card()>0)
  {
    BInt	   size	   = N-N_+inDifData_.Columns()-maxTransferDegree_;
    BCode&	   code	   = Code(nonLinFun_);
//VBR: reverse list to pass arguments to Evaluator
    BList* arg = nonLinPar_.ToReversedList();
    BSyntaxObject* objCode = code.Evaluator(arg);
    if(!objCode || (objCode->Grammar()!=BUserTimeSerie::OwnGrammar()))
    {
      Warning(I2("Wrong non linnear input function definition and it will be "
		               "ignored",
		               "La funcion de input no lineal está mal definida y será "
		               "ignorada"));
      nonLinPar_.Delete();
    }
    else
    {
      BSerieTable nli;
      nli.AddSerie(Tsr(objCode));
      nli.Fill(firstDate_, lastForInput_);
      inDifNonLin_.ReallocBuffer(size);
      BText name = nonLinFun_->Identify()+"("+nonLinPar_.Name()+")";
//    for(j=0; j<nli.NumDates(); j++)
//    { Std(nli.Date(j).Name()+"\t"+nli.Data()[0][j].Name()+"\n"); }

      if(ApplyPolB(inDifNonLin_, nli.Data().Data(), dif_, difDeg_, name, 0))
      { for(j=0; j<size; j++) { inputDifFilter_(j)+=inDifNonLin_(j); } }
      else
      { nonLinPar_.Delete(); }
    }
  }
}


static BDat eps_ = 1.E-4;

//--------------------------------------------------------------------
static BPolyn<BDat> ReversePolyn(const BPolyn<BDat>& p)
//--------------------------------------------------------------------
{
  static BPol B = BPol::B();
  BPolyn<BDat> q;
  q.ChangeBF(p);
  return((B^p.Degree())*q);
}


//--------------------------------------------------------------------
static void SchurToVector(BArray<BDat>& v, const BPolyn<BDat>& p, int n0)
//--------------------------------------------------------------------
{
  int  n  = p.Degree();
  if(n==0) { return; }
  BDat a0 = p(0).Coef();
  BDat a  = (a0>=1.-eps_)?1-eps_:((a0<=-1.+eps_)?-1.+eps_:a0);
//BDat a  = (Abs(a0)>1.0)?1.0/a0:a0;
  BDat a_ = a/Sqrt(1.0-(a^2));
//Std(BText("\nSV v.Size()=")<<v.Size()<<";p="<<p.Name()<<
//          "; n="<<n<<"; n0="<<n0<<"; n0+n-1="<<(n0+n-1)<<
//          "; a0="<<a0<<"; a="<<a);
  if(n>1)
  {
  //Std(BText("; a_=")<<a_);
    BPolyn<BDat> q  = ReversePolyn(p);
    BPolyn<BDat> p0 = p-a*q;
    BDat aux = p0.Coef(n);
  //TRZ(q); TRZ(p0); TRZ(aux);
    for(int i=0; i<p0.Size(); i++) 
    { 
      p0(i).PutDegree(p0(i).Degree()-1); 
      int d = p0(i).Degree();
      if((d<0)||(d>n-1)) { p0(i).PutCoef(0); }
      else               { p0(i).PutCoef(p0(i).Coef()/aux); }
    }
    p0.Aggregate();
  //TRZ(p0);
    SchurToVector(v,p0,n0);
  }
  v(n0+n-1) = a_;
}


//--------------------------------------------------------------------
static void VectorToSchur(BPolyn<BDat>& p, const BArray<BDat>& v, int n, int n0)
//--------------------------------------------------------------------
{
  static BPol B = BPol::B();
  static BPol F = BPol::F();
  if(n==0) { return; }
  BDat a  = v(n0+n-1);
  BDat b  = Sqrt(1.0+(a^2));
  BDat a0 = a/b;
  BDat a_ = (a0>=1.-eps_)?1.-eps_:((a0<=-1.+eps_)?-1.+eps_:a0); 
//BDat a_ = (Abs(a0)>1.0)?1.0/a0:a0;
//Std(BText("\nVS v.Size()=")<<v.Size()<<"; v="<<Compact(Name(v))<<
//          "; n="<<n<<"; n0="<<n0<<"; n0+n-1="<<(n0+n-1)<<
//          "; a="<<a);
  if(n==1)
  {
    p = a_+B;
  //Std(BText("; a0=")<<a0<<"; a_="<<a_); 
  //TRZ(p);
  }
  else
  {
    BPolyn<BDat> p1;
    VectorToSchur(p1, v, n-1, n0);
    BPolyn<BDat> q1 = F*ReversePolyn(p1);
    BPolyn<BDat> aux1 = p1+a_*q1;
  //Std(BText("; a0=")<<a0<<"; a_="<<a_<<"; b="<<b<<"; a="<<a);
  //TRZ(p1); TRZ(q1); TRZ(aux1);
    for(int i=0; i<aux1.Size(); i++)
    {
      int d = aux1(i).Degree();
      if((d<0)||(d>n-2)) { aux1(i).PutCoef(0); }
    }
    aux1.Aggregate();
    p = a_+B*aux1+(B^n);
  //TRZ(aux1); TRZ(p);
  };
};

/*
static int Proof()
{
  int i,k;
  BPolyn<BDat> p, p_, q, q_;
  BDat::PutRealFormat("%20.15lf");
  for(k=1; k<=100; k++)
  {
  //Std(BText("\n\n///////////////////////////////////////////////////////////////////////////"));
  //Std(BText("\nProof ")<<k);
  //Std(BText("\n///////////////////////////////////////////////////////////////////////////\n"));
    int n = (int)(1+10*BUniformDist::Random01()).Value();
    BArray<BDat> r (n);
    BArray<BDat> v (n);
    BArray<BDat> v_(n);
    for(i=0; i<n; i++) 
    { 
      r(i) = (1.-eps_)*BUniformDist::Random01();
      v(i) = ATanH(r(i))*K_;
    }
  //Std(BText("\nVectorToSchur ="));
    VectorToSchur(q, v, n, 0);
  //Std(BText("\n")<<q.Name());
  //Std(BText("\nSchurToVector(")<<p.Name()+")");
    SchurToVector(v_,q,    0);
  //Std(BText("\nVectorToSchur ="));
    VectorToSchur(q_,v_,n, 0);
  //Std(BText("\n")<<p_.Name());
    p  = ReversePolyn(q );
    p_ = ReversePolyn(q_);
    BPol pErr = p-p_;
    BDat vErrMax = 0, pErrMax = 0;
    for(i=0; i<n; i++) 
    { 
      if(Abs(pErr.Coef(i))>pErrMax) { pErrMax = Abs(pErr.Coef(i)); }
      if(Abs(v(i)-v_(i)  )>vErrMax) { vErrMax = Abs(v(i)-v_(i)  ); }
    //Std(BText("\n")<<i<<"\t"<<r(i)<<"\t"<<v(i)<<"\t"<<v_(i)<<"\t"<<v(i)-v_(i)<<"\t"<<
    //    p.Coef(i+1)<<"\t"<<p_.Coef(i+1)<<"\t"<<(p-p_).Coef(i+1)<<"\t");
    }
    Std(BText("\n")<<k<<"\t"<<n<<"\t"<<vErrMax<<"\t"<<pErrMax);
  }
  return(k); 
};
    int proof_ = Proof();
*/

/*
 * Que hace esta función?
 */

//--------------------------------------------------------------------
void BModel::GetParameter()
//--------------------------------------------------------------------
{
  if(numParam_<=0) { return; }
  BInt i=0, j, k, r;
  if(constant_.IsKnown()) { param_(i++) = constant_;  }
  for(r=j=0; j<transfer_.Size(); j++)
  {
//  Std(BText("\nGetParameter  transfer_[")+j+"] = " + transfer_(j).Name());
    for(k=0;k<transfer_(j).Size();k++,r++)
    {
      if(!eliminated_(r))
      {
	       param_(i++)=transfer_(j)(k).Coef();
      }
    }
  }
  for(j=1; j<=nonLinPar_.Card(); j++)
  {
    param_(i++)=Dat(nonLinPar_[j]);
  }

  for(j=0; j<armaFactors_.Size(); j++)
  {
    BPol& p = *(armaFactors_(j));
    if(!p.Degree()) { continue; }
    if(p.Degree()==armaPeriod_(j)*(p.Size()-1))
    { 
      BPol q = p;
      for(int d = 0; d<q.Size(); d++) { q(d).PutDegree(q(d).Degree()/armaPeriod_(j)); }
      q = ReversePolyn(q);
    //Std(BText("\nGetParameter armaFactors_(")<<j<<",period="<<armaPeriod_(j)<<")="<<p.Name()); TRZ(q);
      SchurToVector(param_, q, i);
      i+=q.Degree();
    //Std(BText("\nGetParameter param_=")+Compact(Name(param_)));
    }
    else
    {
      for(k=1; k<p.Size(); k++, i++)
      {
        param_(i)=-p(k).Coef();
      }
    }
  }
/* 
  Std(BText("\nGetParameter = ")<<Compact(Name(param_)));
//TRZ(ar_);TRZ(ma_);
  for(i=0; i<arma_.factor_.Size(); i++)
  {
    bool arOk = arma_.factor_(i).ar_.IsStationary();
    bool maOk = arma_.factor_(i).ma_.IsStationary();
    Std(BText("\nStationary?[")<<(arOk?"YES":"NOT")<<"] AR("<<arma_.factor_(i).s_<<")="<<arma_.factor_(i).ar_.Name());
    Std(BText("\nStationary?[")<<(maOk?"YES":"NOT")<<"] MA("<<arma_.factor_(i).s_<<")="<<arma_.factor_(i).ma_.Name());
  }
*/
}


//--------------------------------------------------------------------
void BModel::AddRestriction(BInt& s, BInt& r, BInt i)
//--------------------------------------------------------------------
{
  if(paramAPrioriNu_.Size()>s)
  {
    if(paramAPrioriNu_(s).IsKnown()&&paramAPrioriSigma_(s).IsKnown())
    {
  //  Std(BText("\nAplicando restrición al parametro ")+ paramAPrioriName_[i]);
      paramAPrioriResiduals_(r++) =
	                     (param_(i)-paramAPrioriNu_(s))/paramAPrioriSigma_(s);
    }
    s++;
  }
}



//--------------------------------------------------------------------
void BModel::PutParameter(const BArray<BDat>& p)
//--------------------------------------------------------------------
{
  if(numParam_<=0) { return; }
  BInt i=0, j, k, r=0, s=0, t;
//Std(BText("\nPutParameters ")+ p.Size() + "\n" + Name(p));
  paramAPrioriResiduals_.ReallocBuffer(numParam_);
  param_ = p;
  if(constant_.IsKnown())
  {
    constant_ = p(i);
    AddRestriction(s, r, i);
    i++;
  }
  for(t=j=0; j<transfer_.Size(); j++)
  {
    for(k=0;k<transfer_(j).Size();k++,t++)
    {
      if(!eliminated_(t))
      {
	       transfer_(j)(k).PutCoef(p(i++));
	       AddRestriction(s, r, i);
      //Std(BText("\n") + i + " : " + " Transfer "+ j + inpName_[j]
      //    + " " + k + " " + transfer_[j][k].Coef().Name());
      }
    }
  //Std(BText("\nPutParameter  transfer_[")+j+"] = " + transfer_(j).Name());
  }
  for(j=1; j<=nonLinPar_.Card(); j++,i++)
  {
    Dat(nonLinPar_[j])=p(i);
    AddRestriction(s, r, i);
  //Std(BText("\n") + i + " : " + " NonLinnear "+ j );
  }
  for(j=0; j<armaFactors_.Size(); j++)
  {
    int d;
    BPol& aux = *armaFactors_(j);
    BPol pol = aux;
    if(!pol.Degree()) { continue; }
    if(pol.Degree()==armaPeriod_(j)*(pol.Size()-1))
    { 
      BPol q; 
    //Std(BText("\nPutParameter armaFactors_(")<<j<<",period="<<armaPeriod_(j)<<")="<<pol.Name());  
    //Std(BText("\nPutParameter p=")+Compact(Name(p))); 
      VectorToSchur(q, p, pol.Degree()/armaPeriod_(j), i);
    //TRZ(q); 
      i+=q.Degree();
      for(d = 0; d<q.Size(); d++) { q(d).PutDegree(q(d).Degree()*armaPeriod_(j)); }
    //TRZ(q);
      pol = ReversePolyn(q);
    //TRZ(pol);
    //for(i=0; i<q.Degree(); i++) { AddRestriction(s, r, i); }
    }
    else
    {
      for(k=1; k<pol.Size(); k++, i++)
      {
        pol(k).PutCoef(-p(i));
        AddRestriction(s, r, i);
      }
    }
    for(d=1; d<aux.Size(); d++) 
    { 
      int deg = aux(d).Degree();
      aux(d).PutCoef(pol.Coef(deg));
    }
  }
/*
  Std(BText("\nPutParameter = ")<<Compact(Name(p)));
//TRZ(ar_);TRZ(ma_);
  for(i=0; i<arma_.factor_.Size(); i++)
  {
    bool arOk = arma_.factor_(i).ar_.IsStationary();
    bool maOk = arma_.factor_(i).ma_.IsStationary();
    Std(BText("\nStationary?[")<<(arOk?"YES":"NOT")<<"] AR("<<arma_.factor_(i).s_<<")="<<arma_.factor_(i).ar_.Name());
    Std(BText("\nStationary?[")<<(maOk?"YES":"NOT")<<"] MA("<<arma_.factor_(i).s_<<")="<<arma_.factor_(i).ma_.Name());
  }
*/
  paramAPrioriResiduals_.ReallocBuffer(r);
  ConstructARMAFromRoots();
}

/*
//--------------------------------------------------------------------
static void ApplyDenPol(const BArray<BDat>& x,
			                     const BArray<BDat>& y,
			                     const BPol&	        den,
			                           BInt	         q)
//--------------------------------------------------------------------
{
  BInt i, j, n = y.Size()-q;
    
//Std(BText("\n\nApplying Den")+den.Name());
  for(i=q ;i<n+q;i++)
  {
    y(i) = x(i);
    for(j=1; j<den.Size(); j++)
    { y(i)-=den(j).Coef()*y[i-den(j).Degree()]; }
  }
//Std(BText("\n")+Name(y));
}
*/

/*
//--------------------------------------------------------------------
static void ApplyNumPol(const BArray<BDat>& x,
                      		const BArray<BDat>& y,
			                     const BPol&	        num,
			                           BInt	         p)
//--------------------------------------------------------------------
{
  BInt i, j, n = x.Size()-p;
    
//Std(BText("\n\nApplying num")+num.Name());
  for(i=0; i<n; i++)
  {
   	y(i) = 0;
	   for(j=0; j<num.Size(); j++)
	   { y(i)+=num(j).Coef()*x[i + p - num(j).Degree()]; }
  }
//Std(BText("\n")+Name(y));
}
*/

//--------------------------------------------------------------------
static void ApplyBRation(const BArray<BDat>& x,
                      		 const BArray<BDat>& y,
			                      const BPol&		num,
			                      const BPol&		den,
			                            BInt		p,
			                            BInt		q)
//--------------------------------------------------------------------
{
  BInt i, j;
    
//Std(BText("\n\nApplying Backward Rational")+num.Name()+" / "+ den.Name());
  for(i=q; i<y.Size(); i++)
  {
   	y(i) = 0;
	   for(j=0; j<num.Size(); j++)
	   { y(i)+=num(j).Coef()*x[i - q + p - num(j).Degree()]; }
	   for(j=1; j<den.Size(); j++)
	   { y(i)-=den(j).Coef()*y[i - den(j).Degree()]; }
  //Std(BText("\nBack[")+i+"]="+y[i].Name()+
  //    "; from x = "+x[i].Name());
  }
}


//--------------------------------------------------------------------
static void ApplyFRation(const BArray<BDat>& x,
                     			 const BArray<BDat>& y,
			                      const BPol&		       num,
			                      const BPol&		       den,
			                            BInt		        p,
			                            BInt	        	q)
//--------------------------------------------------------------------
{
  BInt i, j;
  BInt n = y.Size()-q;
    
//Std(BText("\n\nApplying Forward Rational") + num.ToBText("F")+" / "
//					     + den.ToBText("F"));
  for(i=n-1; i>=0; i--)
  {
   	y(i) = 0;
	   for(j=0; j<num.Size(); j++)
	   { y(i)+=num(j).Coef()*x[i + q - p + num(j).Degree()]; }
	   for(j=1; j<den.Size(); j++)
	   { y(i)-=den(j).Coef()*y[i + den(j).Degree()]; }
  //Std(BText("\nFor[")+i+"]="+y[i].Name()+
  //"; from x = "+x[i].Name());
  }
//Std(BText("\n")+Name(y));
}

/*
//--------------------------------------------------------------------
static void ApplyRation(const BArray<BDat>& x,
		                     	      BArray<BDat>& y,
			                     const BPol&	        num,
			                     const BPol&	        den)
//--------------------------------------------------------------------
{
  BInt p = num.Degree();
  BInt q = den.Degree();
  BInt m = Maximum(p,q);
  BInt i;
  y.ReallocBuffer(x.Size());
  for(i=0; i<y.Size(); i++) { y(i) = 0; }
//Std(BText("\nApplyRation")+num.Name()+" / "+ den.Name());
  BDat absNorm;
  do
  {
	   absNorm = 0;
	   BArray<BDat> yy = y;
   	ApplyBRation(x,y,num,den,m,m);
	   ApplyFRation(x,y,num,den,m,m);
   	for(i=0; i<y.Size(); i++)
	   {
	     BDat a = Abs(yy(i)-y(i));
	     if(a>absNorm) { absNorm = a; }
	   }
  //Std(BText("\n	 Norm ")+absNorm.Name()+"\n");
  }
  while(absNorm>BDat::Tolerance()/(1000*y.Size()));
}
*/

//--------------------------------------------------------------------
void BModel::CalcDifNoise(BInt N)
    
/*! Initializes the diferenced matrix of input and output data.
 */
//--------------------------------------------------------------------
{
  BInt j;

  ApplyPolB(outDifData_, 0, fullData_, dif_, difDeg_, outName_+"_Full", 0);
  Z_ = outDifData_.T();
  if(inputParam_)
  {
   	for(j=0; j<N; j++)
	   {
	     Z_(j,0)  -= inputDifFilter_(j);
	   }
  }
  difNoise_ = Z_.Data();
//Std(BText("\nfullData_  = ")+FrobeniusNormU(fullData_));
//Std(BText("\noutDifData_  = ")+FrobeniusNormU(outDifData_.Data()));
//Std(BText("\ninputDifFilter_  = ")+FrobeniusNormU(inputDifFilter_));
//Std(BText("\nDifferenced noise error  = ")+FrobeniusNormU(difNoise_));
   
}


//--------------------------------------------------------------------
BPol ChangeBDegree(const BPol& p, BDat x)
//--------------------------------------------------------------------
{
  BPol q = p;
  for(BInt i=0; i<q.Size(); i++)
  {
   	BReal deg = q(i).Degree();
	   q(i).PutDegree((int)Round(deg*x).Value());
  }
  q.Aggregate();
  return(q);
}


//--------------------------------------------------------------------
BPolStationaryFunction::BPolStationaryFunction(BPolyn<BDat>& pol,
		                                    		       BInt period)
//--------------------------------------------------------------------
  : BRnRFunction(pol.Size()-1)
{
  f_ = 1.0/period;
  p_ = ChangeBDegree(pol,f_);
}

//--------------------------------------------------------------------
BDat BPolStationaryFunction::GetValue()
//--------------------------------------------------------------------
{
  BDat y = p_.StationaryValue(BTRUE);
//Std(BText("\nS(")+p_.Name()+")="+(y-1));
  return(y-1);
};

//--------------------------------------------------------------------
void BPolStationaryFunction::Evaluate(BDat& y, const BArray<BDat>& x)
//--------------------------------------------------------------------
{
  for(BInt k=1; k<p_.Size(); k++) { p_(k).PutCoef(x(k-1)); }
  y = GetValue();
};

//--------------------------------------------------------------------
void BPolStationaryFunction::GetGradient(BArray<BDat>& G)
//--------------------------------------------------------------------
{
  BArray<BDat> x;
  x.ReallocBuffer(n_);
  for(BInt i=0; i<n_; i++) { x(i) = p_(i+1).Coef(); }
  Gradient(x,G);
}

static BDat nullInitResiduals_ = 0;

BDat& NullInitResiduals () { return(nullInitResiduals_); }

//--------------------------------------------------------------------
void BModel::CalcResiduous(BArray<BDat>&res, int method)
//--------------------------------------------------------------------
{
  BInt j;
  if(maxpq_==0)
  {
    A_ = Z_;
    res = A_.Data();
    //VBR: Si no hay parte ARMA la máxima verosimilitud se alcanza en 
    //     en el mínimo de la suma de cuadrados => maxLikelihoodCoef_=1
    maxLikelihoodCoef_ = 1;
    return;
  }
//BTimer tm (BText("\nCalcResiduous ..."));
  BArray<BARIMAFactor> factor(arFactors_.Size());
  for(j=0; j<factor.Size(); j++)
  {
    BInt arDeg = (arFactors_(j).Size()>1)?arFactors_(j)(1).Degree():0;
    BInt maDeg = (maFactors_(j).Size()>1)?maFactors_(j)(1).Degree():0;
    factor(j).s_   = Maximum(arDeg,maDeg);
    factor(j).dif_ = BPol::One();
    factor(j).ar_  = arFactors_(j);
    factor(j).ma_  = maFactors_(j);
  }
  arma_.PutFactors(factor);
  arma_.PutOutputData(Z_);
  if(method==1)
  {
    arma_.CalcLikelihood_Levinson();
  }
  else if(method==2)
  {
    arma_.CalcLikelihood_Almagro();
  }
  A_  = arma_.a_;
  A0_ = arma_.a0_;
  Z0_ = arma_.w0_;
  if(initValues_ && 
      arma_.likelihoodCoef_.IsKnown() && 
     (arma_.likelihoodCoef_<BDat::PosInf()))
  {
    if(!calculatingJacobian_ && interruptionIndex_.Size() && recalcInterruptions_)
    {
      CleanInterruptions(A_, recalcInterruptions_);
      CalcDifNoise(N_);
      arma_.PutOutputData(Z_);
      arma_.CalcLikelihood();
      A_  = arma_.a_;
      A0_ = arma_.a0_;
      Z0_ = arma_.w0_;
    }
    BMat Z2 = MtMSqr(Z_);
    BMat A2 = MtMSqr(Z_);
    BDat stdvDN    = (!Z2.Data().Size())?BDat::Unknown():Sqrt(Z2(0,0)/Z_.Rows());
    BDat stdErr    = (!A2.Data().Size())?BDat::Unknown():Sqrt(A2(0,0)/A_.Rows());
    BDat normMaxLH = stdErr*arma_.likelihoodCoef_;
    maxLikelihoodCoef_ = arma_.likelihoodCoef_;
  }
  res = (A_*maxLikelihoodCoef_).Data();
}



/* */
//--------------------------------------------------------------------
void BModel::CleanInterruptions(BMatrix<BDat>& A, BBool recalculate)
//--------------------------------------------------------------------
{
  BInt I = interruptionIndex_.Size();
  //Std(BText("CleanInterruptions: ")+(BText)recalculate);
  if((I>0)  && recalculate)
  {
  //BTimer tm(BText("CleanInterruptions"));
    BDat sigma = FrobeniusNormU(A.Data());

    if(sigma<standardError_) { standardError_ = sigma; }

  //Std(BText("\nCleanInterruptions sigma before = ") + sigma);

    BInt n  = interruptionValue_.Size();
    BInt N = arma_.w_.Rows();
  //BInt d = dif_.Degree();
    BInt p = ar_.Degree();
  //BInt q = ma_.Degree();

    piw_  = (ari_/ma_).Expand(n,BFALSE); 
    BInt i, j, ti, tj;
  //Std(BText("\nn=")+n+"\nN="+N+"\nd="+d+"\np="+p+"\nq="+q);
  //Std(BText("\npiw_.Size()=")<<piw_.Size());
    BMat U = A0_;
    BMat E = A_;

    PIW_ = BMat(n,I); 
    PIW_.SetAllValuesTo(0);

    for(ti=0; ti<n; ti++)
    {
      for(j=0; j<I; j++)
      {
	       tj = interruptionIndex_(j);
	       if(tj>ti) { break; }
      //PIW_(ti,j) = piw_.Coef(ti-tj);
        PIW_(ti,j) = piw_(ti-tj).Coef();
      //Std(BText("\npiw_(")<<ti<<"-"<<tj<<")="<<piw_(ti-tj).Coef());
      }
    }
    PIW_ = PIW_.Sub(n-N,0,N,I);   
    BMat CI(I,1);  CI.SetAllValuesTo(0);
    CI = CholeskiMinimumResidualsSolve(PIW_,-A_);
    BBool CI_isKnown = FrobeniusNormU(CI.Data()).IsKnown();
    BMat err = PIW_.T()*(PIW_*CI+A_);
    BDat test = err.MaxAbsNorm(); 
    if(!CI_isKnown ||(test>Sqrt(BDat::Tolerance())))
    {
      Warning(BText("There are numerical problems to calculate values for "
      "interruptions by means of Cholesky decomposition. So SVD method will "
      "be tried and process could become slower"));
      BText method = BText("Golub_Reinsch_Mod");
      BMat U(N,I), V(I,I);
      BDiagMatrix<BDat> D(I,1.0), Dp;
      gsl_SingularValueDecomposition(PIW_,U,D,V, method);
    //Std(BText("\nBModel::CleanInterruptions U=\n")+U.Name()+"\n");
    //Std(BText("\nBModel::CleanInterruptions D=\n")+D.Name()+"\n");
    //Std(BText("\nBModel::CleanInterruptions V=\n")+V.Name()+"\n");
      Dp=D.P(Sqrt(DEpsilon()));
    //Std(BText("\nBModel::CleanInterruptions Dp=\n")+Dp.Name()+"\n");
      CI=-V*Dp*(A_.T()*U).T();
    //Std(BText("\nBModel::CleanInterruptions CI=\n")+CI.T().Name()+"\n");
    //Std(BText("\nBModel::CleanInterruptions PIW_*CI=\n")+(PIW_*CI).T().Name()+"\n");
      err = PIW_.T()*(PIW_*CI+A_);
    //Std(BText("\nBModel::CleanInterruptions err=\n")+(err).T().Name()+"\n");
      test = err.MaxAbsNorm(); 
    //Std(BText("\nBModel::CleanInterruptions test=")+test+"\n");
    }
    CI_isKnown = FrobeniusNormU(CI.Data()).IsKnown();
    if(!CI_isKnown ||(test>Sqrt(BDat::Tolerance())))
    {
      Warning(I2("Cannot calculate values for interruptions due "
               "to a numerical error solving related linear programming "
               "problem by means of singular value decomposition method.",
               "No se pudieron calcular los valores de las interrupciones "
               "debido a un error de tipo numérico resolviendo el "
               "problema de programación lineal correspondiente "
               "mediante el método de descomposición de valor singular.")+
            "\n  Max(|PIW'(PIW*CI+A_)|)="+test);
    }
    if(CI_isKnown)
    {
      for(j=0; j<I; j++) { interruptionValue_[interruptionIndex_[j]] = CI(j,0); }
      BMat PIWC = PIW_*CI;
      A_  = E + PIWC;
      sigma = FrobeniusNormU(A.Data());
      //Std(BText("\nCleanInterruptions sigma after = ") + sigma);
      if(sigma.IsKnown() && 
         !gsl_isinf(sigma.Value()) && 
         !gsl_isnan(sigma.Value()) && 
         (sigma<=standardError_))
      {
        for(i=0; i< interruptionIndex_.Size(); i++)
        {
          ti = interruptionIndex_(i);
          fullData_(ti) += interruptionValue_(ti);
        //Std(BText("\nfullData(I[")+i+"]="<<ti<<")="<<fullData(ti));
        }
      }
    }
  }
}


//--------------------------------------------------------------------
void BModel::Evaluate(BArray<BDat>&res, const BArray<BDat>& p)
//--------------------------------------------------------------------
{
  BInt i;
//Std(BText("\nBEstimation::Evaluate ..."));
  if(aborted_) { return; }
//for(BInt i= 0; i<p.Size();  i++) { Std(p[i].Name()); }
  PutParameter(p);

  if(recalculateDifNoise_)
  {
  //BTimer tm2("Calculating differenced filter and noise");
    CalcInputDifFilter(N_);
    CalcNonLinnearInputDifFilter(N_);
  }
  CalcDifNoise(N_);
  /*
  if (recalcInterruptions_) {Std(BText("\n Entra recalcInterruptions_= True"));}
  else {Std(BText("\n Entra recalcInterruptions_= False"));}
  */
  CalcResiduous(res,1);
  /*
  if (recalcInterruptions_) {Std(BText("\n Sale recalcInterruptions_= True"));}
  else {Std(BText("\n Sale recalcInterruptions_= False"));}
  */
  BInt N1 = res.Size();
  BInt N2 = paramAPrioriResiduals_.Size();
  res.ReallocBuffer(N1+N2);
  for(i=N1; i<N1+N2;  i++)
  {
    res(i) = paramAPrioriResiduals_(i-N1);
  }
//Std(BText("\nBEstimation::Evaluate stationary sigma = ")+StandardDeviation(res));
}


//--------------------------------------------------------------------
void BModel::CalcNumPartDer(const BArray<BDat>& x,
                     			    const BArray<BDat>& y,
			                               BArray<BDat>& Jn,
			                               BInt          r,
			                               BInt          n,
			                               BBool         initValues,
			                               BBool         recalcInterruptions)
//--------------------------------------------------------------------
{
//BTimer tm(BText("\nCalcNumJacobian partial ")+n+" [V "+r+"] : "+infoParam_(r));
  int iter = 0;
  bool ok, end;
  BArray<BDat> res(y.Size());
  BArray<BDat> p = x;
  BDat old = p(n);
  BDat h = 1.0, dist, norm, norm0=FrobeniusNormU(y);
//BBool ok = BFALSE;
  BBool oldInitValues = initValues_;
  BBool oldRecalcInterruptions = recalcInterruptions_;
  initValues_ = initValues;
  recalcInterruptions_ = recalcInterruptions;

  do
  {
    p(n) = old*(1.0+Distance()*h);
    dist = p(n)-old;
    PutParameter(p);
    Evaluate(res,p);
    ok = arma_.likelihoodCoef_.IsKnown() && (arma_.likelihoodCoef_<BDat::PosInf());
    if(ok)
    {
      for(BInt t=0; t<y.Size(); t++)
      {
        Jn(t)=(res(t)-y(t))/dist;
      }
    }
    else
    {
      h/=2;
      iter++;
    }
    end = ok || (1.0+Distance()*h!=1.0);
  }
  while(!end);
  initValues_ = oldInitValues;
  recalcInterruptions_ = oldRecalcInterruptions;
  p(n)=old;
  PutParameter(p);
}


//--------------------------------------------------------------------
void BModel::CalcNumericalJacobian(const BArray<BDat>&  x,
	                            			   const BArray<BDat>&  y,
				                                     BMatrix<BDat>& J)
    
/*! Calculates the numerical jacobian matrix of residuous
 */
//--------------------------------------------------------------------
{
  BDat stdvDN    = Sqrt(MtMSqr(Z_ )(0,0)/Z_.Rows());
  BDat stdErr    = Sqrt(MtMSqr(A_ )(0,0)/A_.Rows());
  BDat normMaxLH = stdErr*arma_.likelihoodCoef_;
  Std(BText("\n    ")<<I2("ARIMA Evaluation","Evaluación ARIMA"));
  Std(BText(" : (stdErr = ")<<stdErr.Value()<<")");
  Std(BText(" * (|ACov|^(1/N) = ")<<Sqrt(arma_.aCovDetN_).Value()<<")");
  Std(BText(" * (Sqrt((w'*InvACov*w/A'A) = ")<<arma_.waCoef_.Value()<<")");
  Std(BText("; stdev(difNoise) = ")<<stdvDN.Value()<<")");

//Std("\nCalcResiduousJacobian ");
  BInt j, k, r=-1, t, n=0, N = y.Size();
  BArray<BDat> Y (N);

  calculatingJacobian_ = BTRUE; 
  recalculateDifNoise_ = BTRUE;

  // CALCULO DE LAS DERIVADAS PARCIALES RESPECTO A LA CONSTANTE
  if(constant_.IsKnown())
  {
    CalcNumPartDer(x, y, Y, 0, n, BFALSE, BFALSE);
    for(t=0; t<N; t++) { J(t,n)=Y(t); }
    n++;
  }
  // CALCULO DE LAS DERIVADAS PARCIALES RESPECTO A LOS INPUTS LINEALES
  if(inputParam_)
  {
    BRational <BDat> R (ar_, ma_);
    for(r=j=0; j<transfer_.Size(); j++)
    {
      for(k=0; k<transfer_(j).Size(); k++, r++)
      {
	       if(!eliminated_(r))
	       {
	         CalcNumPartDer(x, y, Y, r, n, BFALSE, BFALSE);
	         for(t=0; t<N; t++) { J(t,n)=Y(t); }
	         n++;
	       }
      }
    }
  }
  // CALCULO DE LAS DERIVADAS PARCIALES RESPECTO A LOS INPUTS NO LINEALES
  if(nonLinPar_.Card())
  {
    for(j=1; j<=nonLinPar_.Card(); j++, n++, r++)
    {
      CalcNumPartDer(x, y, Y, r, n, BTRUE, BTRUE);
      for(t=0; t<N; t++) { J(t,n)=Y(t); }
    }
  }
  recalculateDifNoise_ = BFALSE;
  // DERIVADAS PARCIALES RESPECTO A LOS PARAMETROS ARMA
  BInt n0=n;
  for(; n<n0+arParam_; n++, r++)
  {
    CalcNumPartDer(x, y, Y, r, n, BTRUE, BTRUE);
    for(t=0; t<N; t++) { J(t,n)=Y(t); }
  }
  for(; n<n0+arParam_+maParam_; n++, r++)
  {
    CalcNumPartDer(x, y, Y, r, n, BTRUE, BTRUE);
    for(t=0; t<N; t++) { J(t,n)=Y(t); }
  }
  recalculateDifNoise_ = BTRUE;
  calculatingJacobian_ = BFALSE; 
}



//--------------------------------------------------------------------
void BModel::CalcAnaliticalJacobian(const BArray <BDat>&  x,
				                                const BArray <BDat>&  y,
				                                      BMatrix<BDat>& J)

/*! Calculates the jacobian matrix of residuous
 */
//--------------------------------------------------------------------
{
  BDat stdvDN    = Sqrt(MtMSqr(Z_ )(0,0)/Z_.Rows());
  BDat stdErr    = Sqrt(MtMSqr(A_ )(0,0)/A_.Rows());
  BDat normMaxLH = stdErr*arma_.likelihoodCoef_;
  Std(BText("\n    ")<<I2("ARIMA Evaluation","Evaluación ARIMA"));
  Std(BText(" : (stdErr = ")<<stdErr.Value()<<")");
  Std(BText(" * (|ACov|^(1/N) = ")<<Sqrt(arma_.aCovDetN_).Value()<<")");
  Std(BText(" * (Sqrt((w'*InvACov*w/A'A) = ")<<arma_.waCoef_.Value()<<")");
  Std(BText("; stdev(difNoise) = ")<<stdvDN.Value()<<")");

  calculatingJacobian_ = BTRUE; 
  recalculateDifNoise_ = BTRUE;
//BTimer tm("Analytical Residuous Jacobian ");
  BInt c    = J.Columns()-arParam_-maParam_;
  BInt N    = A_.Rows();
  BDat coef1 = maxLikelihoodCoef_;
  BInt j, k, r=0, t, n=0, M = y.Size();
  BArray<BDat> Y (M);
  BMat J1(N,c);
  J1.SetAllValuesTo(0);
  BMat J2(N,arParam_+maParam_);
  J2.SetAllValuesTo(0);
//TRZ(inDifData_);
  // CALCULO DE LAS DERIVADAS PARCIALES RESPECTO A LA CONSTANTE
  if(constant_.IsKnown())
  {
    BDat cte = -constant_*fi1_/teta1_;
    for(t=0; t<M; t++) { J1(t,n)=cte; }
    n++;
  }
  // CALCULO DE LAS DERIVADAS PARCIALES RESPECTO A LOS INPUTS LINEALES
  if(inputParam_)
  {
    BRational <BDat> R (ar_, ma_);
    for(r=j=0; j<transfer_.Size(); j++)
    {
      BBool elim = BTRUE;
      BInt rOld = r;
      for(k=0; k<transfer_(j).Size(); k++, r++)
      {
	       if(!eliminated_(r))
	       {
	         elim = BFALSE;
	         break;
	       }
      }
      if(elim)
      {
	       r = rOld + transfer_(j).Size();
	       continue;
      }
      r = rOld;

      BInt T = inDifData_.Columns();

      BMatrix	<BDat> Y, X;
      X.Alloc(T,1);
      for(t = 0; t<T; t++)
      {
	       X(t,0) = inDifData_(j,t);
	       if(X(t,0).IsUnknown()) { X(t,0) = 0; }
      }
//    TRZ(X);
      if(p_ || q_) { MatBackwardDifEq(R,X,Y); }
      else	   { Y = X; }
//    TRZ(Y);
      for(k=0; k<transfer_[j].Size(); k++, r++)
      {
	       if(eliminated_(r)) { continue; }
	       BInt qjk = transfer_[j][k].Degree();
	       for(t=0; t<N; t++)
	       {
	         BInt t1 = t + Y.Rows() - N - qjk;
	         if(t1>=0) { J1(t,n) = -Y(t + Y.Rows()-N-qjk,0); }
	         else	     { J1(t,n) = 0; }
	       }
	       n++;
      }
    //TRZ(J1);
    }
  }
//BMat J10 = arma_.ILtLi_*(arma_.L_.T()*J1);
//J1 -= arma_.L_*J10;
  J1 *= coef1;

  // CALCULO DE LAS DERIVADAS PARCIALES RESPECTO A LOS INPUTS NO LINEALES
  // EL CALCULO ES NUMERICO PUES A PRIORI NO SE CONOCE LA FUNCION
  if(nonLinPar_.Card())
  {
    for(j=1; j<=nonLinPar_.Card(); j++, n++, r++)
    {
      CalcNumPartDer(x, y, Y, r, n, BFALSE, BFALSE);
      for(t=0; t<M; t++) { J2(t,n)=Y(t); }
    }
  }
  recalculateDifNoise_ = BFALSE;

  //BTimer tm("ARMA parameters analitical jacobian");
  // DERIVADAS PARCIALES RESPECTO A LOS PARAMETROS ARMA
  // EL CALCULO ES NUMERICO PUES NO SE CONOCE LA EXPRESION ANALITICA
  BInt n0=n;
  for(; n<n0+arParam_; n++, r++)
  {
    CalcNumPartDer(x, y, Y, r, n, BTRUE, BTRUE);
    for(t=0; t<M; t++) { J2(t,n-n0)=Y(t); }
  }
  for(; n<n0+arParam_+maParam_; n++, r++)
  {
    CalcNumPartDer(x, y, Y, r, n, BTRUE, BTRUE);
    for(t=0; t<M; t++) { J2(t,n-n0)=Y(t); }
  }

  J = J1 | J2;
  recalculateDifNoise_ = BTRUE;
  calculatingJacobian_ = BFALSE; 
}

//--------------------------------------------------------------------/
void BModel::IntegrateMissingValues()

/*! Integrate the missing values estimated. Before calling
 *  IntegrateMissingValues the estimated values are differenced.
 */
//--------------------------------------------------------------------/
{
#ifndef _TEST_NEW_CLEAN_  
  return;
#endif
  int i, j, ti, offset_index;
//  int size_res = arma_.L_.Rows();
  int size_res = arma_.r_.Rows();
  int size_ser = interruptionValue_.Size();
  int size_I = interruptionIndex_.Size(), size_d;
  double delta_0, sum, missing, Yti, Nti;
  BMonome<BDat> * mono;

  if (size_I && difDeg_) {
    offset_index = size_res - size_ser;
    delta_0 = dif_(0).Coef().Value();
    size_d = dif_.Size();
    // interruptionValue is differenced and filtered
    // this loop compute the integration part depending only on transOutData_
    for (i = 0; i < size_I; i++) {
      ti = interruptionIndex_(i);
      sum = 0;
      for (j = 1; j < size_d; j++) 
      {
       	mono = &dif_(j);
	       sum += mono->Coef().Value()*fullData_(ti-mono->Degree()).Value();
      }
      // now inverse filter & complete integration
      Yti = interruptionValue_(ti).Value();
      if (inputDifFilter_.Size())
      {
	       Nti = inputDifFilter_(ti+offset_index).Value();
      }
      else
      {
	       Nti = 0;
      }
      missing = (Yti + Nti - sum)/delta_0;
      //missing = (Yti - sum)/delta_0;
      fullData_(ti) = interruptionValue_(ti) = missing;
    }
  }
}

static BDat doStatistics_ = 1;

BDat& DoStatistics () { return(doStatistics_); }

static BDat doKernel_ = 0;

BDat& DoKernel () { return(doKernel_); }

//--------------------------------------------------------------------
void BModel::Statistics()
//--------------------------------------------------------------------
{
  if(aborted_) { return; }
  BDat stdvDN    = Sqrt(MtMSqr(Z_ )(0,0)/Z_.Rows());
  BDat stdErr    = Sqrt(MtMSqr(A_ )(0,0)/A_.Rows());
  BDat normMaxLH = stdErr*arma_.likelihoodCoef_;
  Std(BText("\n    ")<<I2("ARIMA Evaluation","Evaluación ARIMA"));
  Std(BText(" : (stdErr = ")<<stdErr.Value()<<")");
  Std(BText(" * (|ACov|^(1/N) = ")<<Sqrt(arma_.aCovDetN_).Value()<<")");
  Std(BText(" * (Sqrt((w'*InvACov*w/A'A) = ")<<arma_.waCoef_.Value()<<")");
  Std(BText("; stdev(difNoise) = ")<<stdvDN.Value()<<")");

  BTimer tm("BModel::Statistics()");

// Si hay interrupciones se recalcula el valor de la serie output sin
// interrupciones

  BInt p,i,j,k,t, n=numParam_, N = A_.Rows()+4; //jacobian_.Columns();

  CalcDifNoise(difNoise_.Size());

  BUserTimeSerie*  out = output_;
  BUserTimeSet*	   tms = out->Dating();
  BDate		   f   = tms->Prev(lastDate_,N-1);
  BDate		   l   = lastDate_;
  BText		   rN  = I2("Residuous of ","Residuos de ");
  BDat		   var = standardError_*standardError_;
  BDat		   sigma = 1.0/Sqrt(N);

  SwartzInfo_ = Log(var)+n*Log(N)/N;
  paramSD_.ReallocBuffer(n);

  BTimer stat(I2("Model statisics","Estadísticas del modelo"));

  ////////////////////////////////////////////////////////////////////////////
  // Covarianze Matrix (Cov/Varianze)
  ////////////////////////////////////////////////////////////////////////////
  {
    BSymMatrix<BDat> Mi;

    if(DoStatistics().Value() && jacobian_.Rows() )
    {
      BTimer tm("Covarianze Matrix SVD");
/* */
      BDat dist;
      BArray<BDat> y0, y;
      calculatingJacobian_ = BTRUE;
      recalculateDifNoise_ = BFALSE;
      initValues_          = BTRUE;
      recalcInterruptions_ = BTRUE;
      i = jacobian_.Columns()-arParam_-maParam_;
    //TRZ(jacobian_);
      CalcResiduous(y0,1);
      for(j=0; j<armaFactors_.Size(); j++)
      {
        BPol& pol = *(armaFactors_(j));
        for(k=1; k<pol.Size(); k++, i++)
        {   
          BDat old = pol(k).Coef();
          pol(k).PutCoef(old*(1.0+Distance()));
          dist = pol(k).Coef()-old;
          CalcResiduous(y,1);
          pol(k).PutCoef(old);
          for(t=0; t<y.Size(); t++)
          {
	          jacobian_(t, i)=(y(t)-y0(t))/dist;
          }
        }
      }
    //TRZ(jacobian_);
      calculatingJacobian_ = BFALSE;
      recalculateDifNoise_ = BFALSE;
      initValues_          = BTRUE;
      recalcInterruptions_ = BFALSE;
      CalcResiduous(y0,1);
//    Evaluate(residuous_, param_);
/* */
      BDiagMatrix<BDat> Dp;
      BMatrix<BDat> Vt;
      BDat fn = FrobeniusNormU(jacobian_.Data());
      BInt r  = jacobian_.Rows(), c = jacobian_.Columns();
      U_.Alloc(r, c);
      V_.Alloc(c, c);
      D_.Alloc(c);
      /*BMatrix<BDat> U_(r, c), V_(c, c);
        BDiagMatrix<BDat> D_(c, 0);*/
      if(fn.IsUnknown() || (fn<=0.0) || (fn==BDat::PosInf()))
      { 
        Warning(I2("\nJacobian of residuals couldn't be well calculated",
                   "\nEl jacobiano de los residuos no se pudo calcular correctamente"));
        return;
      }
      gsl_SingularValueDecomposition(jacobian_, U_, D_, V_,
                                     BText("Golub_Reinsch_Mod"));

      //gsl_SingularValueDecomposition(jacobian_, U_, D_, V_);
      Vt  = V_.T();
      Dp  = D_.P(Sqrt(D_.Rows()*BDat::Zero()));
    //informationMatrix_ = (V_*D_*D_*Vt)*2.0/var;
    //paramCov_	         = (V_*Dp*Dp*Vt)*0.5*var;
      informationMatrix_ = (V_*D_*D_*Vt)/var;
      paramCov_	         = (V_*Dp*Dp*Vt)*var;
      
      //  BMat test = paramCov_ * informationMatrix_;
      //  TRZ(jacobian_); TRZ(U_); TRZ(D_);	 TRZ(V_);  TRZ(D2); TRZ(Dp); TRZ(D2p);
      
      //  informationMatrix_ = MtMSqr(jacobian_);
      //  Inverse(informationMatrix_,paramCov_);
      //  TRZ(informationMatrix_); TRZ(paramCov_);
      
      BInt d = jacobian_.Columns() - D_.Columns();
      if(d)
      {
        Warning(I2("\nThere are ", "\nHay ") + (d) +
                I2(" linear combinations."," combinaciones lineales."));
      }
      else
      {
        Std(I2("\nThe system is regular.","\nEl sistema es regular."));
      }
      if(DoKernel().Value() && d)
      {
        kernel_ = Kernel(jacobian_);
        p=i=0;
        BArray<BText> names(jacobian_.Columns());
        if(constant_.IsKnown()) { names(i++) = infoParam_(p++); }
        for(p=j=0; j<transfer_.Size(); j++)
        {
          for(k=0; k<transfer_(j).Size(); k++, p++)
          {
            if(!eliminated_(p)) { names(i++) = infoParam_(p); }
          }
        }
       	BUserFunction* uFun = (BUserFunction*)(Code(nonLinFun_).Operator());
        for(j=1; j<=nonLinPar_.Card(); j++)
        {
          names(i++) = infoParam_(p++);
        }
       	for(j=0; j<arParam_+maParam_; j++)
        {
          names(i++) = infoParam_(p++);
        }
        WriteRowNullEquations(kernel_.T(), names);
      }
    }
  }

  //--------------------------------------------------------
  // Correlation Matrix and Standard Deviation of parameters
  //--------------------------------------------------------
  {
    n = paramCov_.Rows();
//  BTimer tm("Correlation Matrix and Standard Deviation of parameters");
    paramCor_ = BSymMatrix<BDat>(n);
    for(i=0; i<n; i++)
    {
      if(paramCov_(i,i)==0)
      {
	/*
#ifdef _MSC_VER
	       paramSD_(i)=INFINITY;
	       paramCor_(i,i)=NAN;
#else
	       paramSD_(i)=1.0/0.0;
	       paramCor_(i,i)=0.0/0.0;
#endif
	*/
	       paramSD_(i)=GSL_POSINF;
	       paramCor_(i,i)=GSL_NAN;
      }
      else
      {
	       paramSD_(i)=Sqrt(paramCov_.Get(i,i));
	       paramCor_(i,i)=1;
      }
      for(j=0; j<i; j++)
      {
	       paramCor_(j,i)=paramCor_(i,j)=paramCov_(i,j)/(paramSD_(i)*paramSD_(j));
      }
    }
    // TRZ(paramCor_);
//  for(i=0; i<n; i++) { paramSD_[i]*=standardError_; }
  }

  //-------------------------------------------------------
  // Autocorrelation Matrix and Box-Pierce-Ljung Statistics
  //-------------------------------------------------------
  aCorNum_ = N/4;
  BData acor(aCorNum_), pacor(aCorNum_);
  {
//  BTimer tm("Autocorrelation Matrix and Box-Pierce-Ljung Statistics");
    if(periodicity_>0)
    {
      if(aCorNum_< 3*periodicity_) { aCorNum_ =	 3*periodicity_; }
      if(aCorNum_>10*periodicity_) { aCorNum_ = 10*periodicity_; }
    }
    else
    {
      if(aCorNum_>100) { aCorNum_ = 100; }
    }
    if(aCorNum_>N/2) { aCorNum_ = N/2; }
    resACor_ = BMat(4,aCorNum_);
    BInt  p = arParam_;
  //BInt  q = maParam_;
    AutoCor(residuous_,acor,aCorNum_);
    BDat Q = 0;
    for(i=0; i<aCorNum_; i++)
    {
      resACor_(0,i)=i+1;
      resACor_(1,i)=acor(i);
      resACor_(2,i)=sigma;
      Q += ((resACor_(1,i)^2)/BDat(N-i-1));
      resACor_(3,i)=Q*BDat(N*(N+2));
    }
  }

  //-------------------------------------------------------
  // Partial Autocorrelation Matrix
  //-------------------------------------------------------
  {
//  BTimer tm("Partial Autocorrelation Matrix");
    resPACor_=BMat(3,aCorNum_);
    PartAutoCor(acor,pacor);

    for(i=0; i<aCorNum_; i++)
    {
      resPACor_(0,i)=i+1;
      resPACor_(1,i)=pacor(i);;
      resPACor_(2,i)=sigma;
    }

    resACor_  = resACor_ .T();
    resPACor_ = resPACor_.T();
  }
}
