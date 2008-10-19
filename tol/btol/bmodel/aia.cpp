/* aia.cpp: Functions of BAia
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

//#define TRACE_LEVEL 2

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_baia.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bmatrix.h>
#include <tol/tol_btsrgrav.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bmaia.h>

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BAia).
//--------------------------------------------------------------------
BTraceInit("aia_.cpp");
BArray<BAtom*> BOutlier::sortedOutliers_;
BDat   BOutlier::minOutlierLikelyhood_ = 4;
BList* BOutlier::instances_         = NIL;

static BPol p1 = BPol::One();
static BPol pB = BPol::X();
static BPol pD = BPol::One() - BPol::X();
static BPol pT = pD^2;

BArray<BOutlier*>  BAia::sysOutliers_;
BOutlier* BAia::pulseOut_   = NULL;
BOutlier* BAia::stepOut_    = NULL;
BOutlier* BAia::compensOut_ = NULL;

//--------------------------------------------------------------------
int BAia::StaticInit()
//--------------------------------------------------------------------
{
  if (!BAia::sysOutliers_.Size()) 
  {
    BOutlier* stOutliers_ [] =
    {
      pulseOut_   = new BOutlier("PulseOut",    p1 / p1 ),
      stepOut_    = new BOutlier("StepOut",     p1 / pD ),
      compensOut_ = new BOutlier("CompensOut",  pD / p1 )
    };
    BInt stOutliersSize_ = sizeof(stOutliers_)/sizeof(BOutlier*);
    BAia::sysOutliers_ = BArray<BOutlier*>(stOutliersSize_, stOutliers_);
  }
  return 1;
}

//--------------------------------------------------------------------
BDat& MinOutlierLikelyhood()
//--------------------------------------------------------------------
{
  return(BOutlier::minOutlierLikelyhood_);
}


//--------------------------------------------------------------------
BOutlier::BOutlier(const BText& name, const BRat& rat)

/*! BAia constructor
 */
//--------------------------------------------------------------------
:BObject(name), responseFunction_(rat), aia_(NIL), enabled_(BTRUE)
{
//Std(BText("\nCreating outlier ")+name+" = " +rat.Name());
  if(name.HasName())
  {
    instances_ = Cons((BCore*)this, instances_);
  }
  expand_.ReallocBuffer(1000);
  expand_.ReallocBuffer(0);
}


//--------------------------------------------------------------------
BOutlier::~BOutlier()

/*! BAia destructor
 */
//--------------------------------------------------------------------
{
  if(BGrammar::Instances())
  { 
    instances_=LstRemoveAtom(instances_,(BCore*)((BSyntaxObject*)(this))); 
  }
}


//--------------------------------------------------------------------
void BOutlier::CalcExpand(BInt n)

/*! BAia constructor
 */
//--------------------------------------------------------------------
{
  expand_[n] = responseFunction_.Numerator().Coef(n);
//Std(BText("\n   Expand[")+n+",0]="+expand_[n].Name());
  BInt i=n;
  if(responseFunction_.Denominator().Degree()<n)
  { 
    i = responseFunction_.Denominator().Degree(); 
  }
  for (;i>0;i--)
  {
    expand_[n] -= expand_[n-i]*responseFunction_.Denominator().Coef(i);
  //Std(BText("\n  Expand[")+n+","+i+"]="+expand_[n].Name());
  }
  expand_[n] /= responseFunction_.Denominator()[0].Coef();
}


//--------------------------------------------------------------------
BDat BOutlier::Expand(BInt n)

/*! BAia constructor
 */
//--------------------------------------------------------------------
{
  if((responseFunction_.Denominator().Degree()==0) &&
     (responseFunction_.Denominator()[0].Coef()!=0))
  {
    return(responseFunction_.Numerator().Coef(n)/
           responseFunction_.Denominator()[0].Coef());
  }
  else if((responseFunction_.Denominator()[0].Degree()==0) &&
          (responseFunction_.Denominator()[0].Coef()!=0))
  {
    BInt size = expand_.Size();
    if(n>=size)
    {
    //Std(BText("\nExpand : n = ")+n+", Size = "+ size +", MaxSize = "+expand_.MaxSize());
      if(n>=expand_.MaxSize()) { expand_.ReallocBuffer(2*(n+1)); }
      if(n>=expand_.MaxSize()) { expand_.ReallocBuffer(2*(n+1)); }
      expand_.ReallocBuffer(n+1);
      for(BInt i=size; i<=n; i++) { CalcExpand(i); }
    }
    return(expand_[n]);
  }
  return(BDat::Unknown());
}


//--------------------------------------------------------------------
BOutlier* BOutlier::Find(const BText& name)

/*! Finds an outlier called name
 */
//--------------------------------------------------------------------
{
  BOutlier* found = NULL;
  BObjectCRef aux(name);
  int f = sortedOutliers_.FindSorted(&aux,AtmCmp);
  if(f>=0) { found = (BOutlier*)sortedOutliers_[f]; }
  return(found);
}


//--------------------------------------------------------------------
BOutlier* BOutlier::Find(const BRat& rat)

/*! Finds an outlier called name
 */
//--------------------------------------------------------------------
{
  BList* lst = instances_;
  while(lst)
  {
    BOutlier* out = (BOutlier*)Car(lst);
    if(out->responseFunction_==rat) { return(out); }
    lst = Cdr(lst);
  }
  return(NIL);
}


//--------------------------------------------------------------------
void   BOutlier::PutAia(BAia* aia)

/*! Prepares the outlier for use with a model.
 */
//--------------------------------------------------------------------
{
  int i, j;
  int length = aia->N_;
  aia_ = aia;
  rat_    = responseFunction_;
  rat_*=aia_->rat_;
//Std(BText("\n   Length      = ")+length);
  BPol expand = rat_ % (length+1);
  x_.ReallocBuffer(length);
  xx_.ReallocBuffer(length);
  BDat max = 0;
  maxRelative_ = 0;
  for(i=0; i<length; i++)
  {
    x_[i]  = expand.Coef(i);
    if(max < Abs(x_[i]))
    {
      max = Abs(x_[i]);
      maxRelative_ = i;
    }
    xx_[i] = x_[i]*x_[i];
  }
  nearMaxAbs_.AllocBuffer(length);
  for(i=j=0; i<length; i++)
  {
    if(Abs(x_[i])>=.9*max)
    {
      nearMaxAbs_[j++]=i;
    }
  }
  nearMaxAbs_.AllocBuffer(j);
//Std(BText("\n   Ratio = ")+rat_.Name()+ " max (" + maxRelative_ +")="+max);
}


//--------------------------------------------------------------------
void BOutlier::Evaluate	(BInt t, BMatrix<BDat>& x, int row, BDat& sqrSum)

/*! Applys the outlier to a date an puts data in a row of a matrix
 */
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BOutlier::Evaluate");
#endif
  if(!aia_) { return; }
  BInt i, j=0;
  TRACE_SHOW_HIGH(fun,Name()+
                  "["+t+","+
                  aia_->res_->Dating()->Next(aia_->res_->FirstDate(),
                  t)+"]");
  BInt length = aia_->N_;
  sqrSum=0;
  for(i=0; i<t; i++)
  {
    x(row,i) = 0;
  }
  for(; i<length; i++, j++)
  {
    x(row,i)+=x_[j];
    sqrSum+=xx_[j];
  }
}


//--------------------------------------------------------------------
BReal BOutlier::Estimate(BInt t, BReal& w)

/*! Estimates the coefient w0 of this outlier at index t for the
 *  current model mod_ and returns true if the corresponding t-student
 *  statistic value is great or equal than minOutlierLikelihood_
 */
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BOutlier::Estimate");
#endif
  if(!aia_) { return(0); }
  t-=maxRelative_;
  BReal zx=0;
  BReal xx=0;
  BInt i=t, j=0;
  TRACE_SHOW_MEDIUM(fun,Name()+
                    "["+t+","+
                    aia_->res_->Dating()->Next(aia_->res_->FirstDate(),
                    t)+"]");
  BInt length = aia_->N_;
  for(; i<length; i++, j++)
  {
    xx+=xx_[j].Value();
    zx+=(aia_->res_->Data()[i]*x_[j]).Value();
  }
  if(xx<=0) { return(0); }
  w = zx/xx;
  TRACE_SHOW_MEDIUM(fun,BText("  *w = ")+w);
  TRACE_SHOW_MEDIUM(fun,BText("  *stDs = ")+aia_->sigma_);
  BDat sigma   = aia_->sigma_/Sqrt(xx);
  TRACE_SHOW_MEDIUM(fun,BText("  *sigma = ")+sigma.Value());
  student_ = (BDat(w)/sigma).Value();
  TRACE_SHOW_MEDIUM(fun,BText("  *Student = ")+student_);
  return(fabs(student_));
}


//--------------------------------------------------------------------
BText BOutlier::GetExpression(BInt t, BDat	w) const

/*! Substract the effect
 */
//--------------------------------------------------------------------
{
  if(!aia_) { return(""); }
  BUserTimeSet* dating = aia_->res_->Dating();
  BDate f     = dating->Next(aia_->res_->FirstDate(),t);  
  BText expression = 
    BText("InputDef(")+w+", RationExpand("+
    f.Name()+","+
    dating->Identify()+","+
    Name()+"))";
  return(expression);
}

//--------------------------------------------------------------------
BSyntaxObject* BOutlier::GetInputDef(BInt t, BDat w) const

/*! Substract the effect
 */
//--------------------------------------------------------------------
{
  if(!aia_) { return(NIL); }
  BText expression = GetExpression(t,w);
  BSyntaxObject* result = GraSet()->EvaluateExpr(expression);
//BGrammar::DoUnparseNodes() = aux;
  if(result) { result->PutDescription(expression); }
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BOutlier::RecalcResiduous(BInt t, BReal w) const

/*! Substract the effect
 */
//--------------------------------------------------------------------
{
  if(!aia_) { return(NIL); }
  t-=maxRelative_;
  BInt i=t, j=0;
  BDat sigma = 0;
  BInt length = aia_->N_;
//Std(BText("\nRecalcResiduous(")+Name()+","+t+","+w+")");
  for(; i<length; i++, j++)
  {
    aia_->res_->Data()[i]-=w*x_[j];
  //Std(BText("\n  x[")+j+"]="+x_[j].Name());
  //Std(BText("; Res[")+i+"]="+aia_->res_->Data()[i]);
  }
  aia_->sigma_ = StandardDeviation(aia_->res_->Data()).Value();
/*
Std(BText("\n   Found ") + Name() + " outlier at " + f.Name() +
" (Value = " + w + ")"+
" (T-Student = " + student_ + ")"+
" (Res error = " + aia_->sigma_ + ")");
Std(BText("\n   ") + input);
*/ 
  return(GetInputDef(t,w));
}


//--------------------------------------------------------------------
BAia::BAia(BModel* model)

/*! BAia constructor
 */
//--------------------------------------------------------------------
: res_(model->res_), rat_(model->ari_ / model->ma_), input_(NIL)
{
  Initialize();
}

//--------------------------------------------------------------------
BAia::BAia(BUserTimeSerie* res, const BRat& rat)

/*! BAia constructor
 */
//--------------------------------------------------------------------
: res_(res), rat_(rat), input_(NIL)
{
  userOutliers_ = sysOutliers_;
  Initialize();
}


//--------------------------------------------------------------------
BAia::BAia(BUserTimeSerie* res, const BRat& rat,
     const BArray<BOutlier*>& userOutliers)
    
/*! BAia constructor
 */
//--------------------------------------------------------------------
: res_(res), rat_(rat), userOutliers_(userOutliers), input_(NIL)
{
  Initialize();
}


//--------------------------------------------------------------------
void BAia::Initialize()

/*! Put aia_ member to this for each used BOutlier.
 */
//--------------------------------------------------------------------
{
  BInt o;
  BDat f   = (*res_)[res_->FirstDate()]; //Forzes residuous calculation
  N_  = res_->Data().Size();
//Std(BText("\nResiduous dates ")+res_->FirstDate().Name()+", "+
//          res_->LastDate ().Name()+", "+N_);
  sigma_    = StandardDeviation(res_->Data()).Value();
//Std("\nUsing outlier's types: ");
  for(o=0; o<userOutliers_.Size(); o++)
  {
    userOutliers_[o]->PutAia(this);
  //Std(BText("\n  ")+userOutliers_[o]->Name());
  }
}


//--------------------------------------------------------------------
bool BAia::SearchNextOutlier()
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BAia::SearchNextOutlier");
#endif
  BInt  t,o;
  BReal max    =  0;
  tFound_ = -1;
  oFound_ = -1;
  
  for(t=0; t<N_; t++)
  {
    BReal absRes = Abs(res_->Data()[t]).Value();
  //Std(BText("\n   ")+absRes+ " <=> " + max);
    if(absRes>max)
    {
      max    = absRes;
      tFound_ = t;
    }
  }
  TRACE_SHOW_LOW(fun,BText("Max absolute residuous: [")+tFound_+"]="+max);
  if(tFound_<0) { return(BFALSE); }
  max  = 0;
  
  for(o=0; o<userOutliers_.Size(); o++)
  {
    if(userOutliers_[o]->Enabled())
    {
      BReal w;
      BReal student = userOutliers_[o]->Estimate(tFound_,w);
      if(Abs(student)>max)
      {
        max  = student;
        oFound_  = o;
        wFound_  = w;
      }
    }
  }
  return(oFound_>=0);
}


//--------------------------------------------------------------------
BList* BAia::Input()
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("BAia::Input");
#endif
  if(!input_)
  {
    BInt n=0;
    BDat oldBic = 2*Log(sigma_);
    while((n<N_-2) && SearchNextOutlier()) 
    { 
      BSyntaxObject* inp = userOutliers_[oFound_]->RecalcResiduous(tFound_,wFound_);
      assert(inp);
      input_ = Cons(inp, input_);
      BDat tStd = userOutliers_[oFound_]->student_;
      BDat bic = 2*Log(sigma_) + n*Log(N_)/N_;
      BDat difBIC = bic-oldBic;
      oldBic = bic;
      TRACE_SHOW_LOW(fun,BText("\n AIA [")+n+"]"+inp->Identify()+
                     "\n *TST = "+ tStd+ 
                     "\n *SIG = "+ sigma_+ 
                     "\n *BIC = "+bic+ "(dif:"+difBIC+")");
      if((difBIC>0)||(Abs(tStd)<MinOutlierLikelyhood()))
      {
        break;
      }
      n++; 
    }
  }
  return(input_);
}



//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetAia);
DefExtOpr(1, BSetAia, "AIA", 2, 7, 
  "Serie Ratio Set Real Real Real Real",
  "(Serie ser, Ratio rat [, Set outliers,"
  " Real optMaxOrder=0,"
  " Real optMaxCrossNum=100,"
  " Real optMinNonZeroParamProb=0.99,"
  " Real optMaxEigenValueRelativeRange=100])",
  I2(
"AIA: Automatic analysis of Intervention of temporary series. \n"
"\n"
"Starting with the series of remainders <ser> of model ARIMA \n"
"given by the quotient of retardation polynomials \n"
"\n"
"<rat> = (Dif(B)*Ar(B))/Ma (B). \n"
"\n"
"This model can be considered previously or simply a model ARIMA \n"
"can be postulated anyone. The set <outliers> contains the different \n"
"forms from intervention that are wanted to explore defined by means \n"
"of objects of Ratio type with name. If they are not specified they \n"
"will use the forms by defect: \n"
"\n"
"  Ratio PulseOut   = 1/1; \n"
"  Ratio CompensOut = (1-B)/1; \n"
"  Ratio StepOut    = 1 (1-B); \n"
"\n"
"Is very recommendable to follow the nomenclature \n"
"\n"
"  Ratio <name>Out\n"
"\n"
"to be able to make use of some functions based on AIA. \n"
"\n"
"The procedure is based on taking the highest remainders in absolute \n"
"value and proving what forms explain better to such. Evidently an \n"
"exhaustive procedure of exploration of all the combinations of \n"
"inputs defined by neighboring dates to a high remainder and one of \n"
"the forms of the set <outliers> he would be impassable with as \n"
"soon as dozens of data. The procedure must therefore take the \n"
"combinations that are considered more probable and to consider \n"
"linear models to see which is indeed the best one of them. \n"
"\n"
"Once filtrates the remainders of this combination of inputs, the \n"
"process will be repeated sequentially until it is considered that \n"
"no combination new improvement the results, which will happen \n"
"when the minimum value of t-student of inputs considered does \n"
"not reach the meaning you give by the global variable \n"
"\n"
"  Real MinOutlierLikelyhood\n"
"\n"
"which one would be due to take always at least like 2, and \n"
"preferredly over 3. \n"
"\n"
"The parameter <optMaxOrder> indicates the maximum size of each \n"
"one of those combinations of inputs that will be explored. \n"
"\n"
"The parameter <optMaxCrossNum> indicates the maximum of combinations \n"
"of dimension n that will be taken as candidates to generate those \n"
"of dimension n+1 \n"
"\n"
"The greater ones are <optMaxOrder> and <optMaxCrossNum> the more \n"
"combinations will be explored and better results will be able to be \n"
"obtained when considering correlacioens between the different ones \n"
"inputs, which can need others to reach the minimum meaning. \n"
"Evidently, also it increases the computacional cost.\n"
"\n"
"Best empirical values of <optMaxOrder> are found between 1 and 4, \n"
"and for <optMaxCrossNum> between 10 and 1000.\n"
"\n"
"In order to keep compatibility with previous versions, \n"
"if <optMaxOrder> is not specified either is smaller or just as 0, a \n"
"simplified, slightly faster and enough less robust implementation \n "
"will be used.\n"
,
"AIA: Análisis de Intervención Automático de series temporales.\n"
"\n"
"Se parte de la serie de residuos <ser> del modelo ARIMA dado por el \n"
"cociente de polinomios de retardo \n"
"\n"
"  <rat> = (Dif(B)*Ar(B))/Ma(B). \n"
"\n"
"Este modelo puede haber sido estimado previamente o simplemente \n"
"se puede postular un modelo ARIMA cualquiera.\n"
"\n"
"El conjunto <outliers> contiene las diferentes formas de intervención \n"
"que se quieren explorar definidas mediante objetos de tipo Ratio \n"
"con nombre. Si no se especifican se usarán las formas por defecto:\n"
"\n"
"  Ratio PulseOut   = 1/1;\n"
"  Ratio CompensOut = (1-B)/1;\n"
"  Ratio StepOut    = 1/(1-B);\n"
"\n"
"Es muy recomendable seguir la nomenclatura Ratio <name>Out para \n"
"poder hacer uso de algunas funciones basadas en AIA.\n"
"\n"
"El procedimiento se basa en tomar los residuos más altos en valor \n"
"absoluto y probar qué formas explican mejor a los mismos. \n"
"\n"
"Evidentemente un procedimiento exhaustivo de exploración de todas\n"
"las combinaciones de inputs definidos por una fechas vecinas \n"
"a un residuo alto y una de las formas del conjunto <outliers> \n"
"sería impracticable con apenas unas docenas de datos. \n"
"\n"
"El procedimiento debe por lo tanto tomar las combinaciones que \n"
"se consideren más probables y estimar modelos lineales para ver \n"
"cuál es efectivamente la mejor de ellas. Una vez filtrados los \n"
"residuos de dicha combinación de inputs, este proceso se repetirá \n"
"secuencialmente hasta que se considere que ninguna combinación \n"
"nueva mejora los resultados, lo cual ocurrirá si el mínimo valor \n"
"de las t-student de los inputs estimados no alcanza la significación \n"
"dada por el argumento <optMinNonZeroParamProb>, o bien si los \n"
"autovalores de la matriz de covarianzas de los parámetros tienen\n"
"un rango relativo mayor que <optMaxEigenValueRelativeRange>\n"
"\n"
"El parámetro <optMaxOrder> indica el tamaño máximo de cada una de \n"
"esas combinaciones de inputs que se explorarán. \n"
"\n"
"El parámetro <optMaxCrossNum> indica el máximo de combinaciones \n"
"de dimensión n que se tomarán como candidatas para generar las \n"
"de dimensión n+1\n"
"\n"
"Cuanto mayores sean <optMaxOrder> y <optMaxCrossNum> más \n"
"combinaciones se explorarán y mejores resultados se podrán obtener\n"
"al tenerse en cuenta las correlacioens entre los diferentes \n"
"inputs, los cuáles pueden necesitar unos de otros para alcanzar\n"
"la significación mínima. Evidentemente también aumenta el coste\n"
"computacional.\n"
"\n"
"Los valores razonables de <optMaxOrder> se encuentran empíricamente\n"
"entre 1 y 4 y los de <optMaxCrossNum> entre 10 y 1000.\n"
"\n"
"\nPara guardar compatibilidad con versiones anteriores, \n"
"si no se especifica <optMaxOrder> o bien es menor o igual que 0 \n"
"se utilizará una implementación simplificada ligeramente más rápida \n"
"pero bastante menos robusta\n"
),
    BOperClassify::Sthocastic_);

//--------------------------------------------------------------------
void BSetAia::CalcContens()
//--------------------------------------------------------------------
{
//Std("\nAIA\n");
  BUserTimeSerie* res  = Tsr(Arg(1)->CopyContens());
  BRat      rat  = Rat(Arg(2));
  BArray<BOutlier*> outliers;
  if(!(res->IsStochastic()))
  {
    Error(I2("Cannot do AIA to a non stochastic series."
       "The series does not has dating or it is infinite.",
       "No se puede hacer el AIA para una serie no estocástica."
       "La serie no tiene fechado o es infinita."));
    return;
  }
  if(Arg(3))
  {
    BSet& set = Set(Arg(3));
    outliers.ReallocBuffer(set.Card());
    BInt o=0;
    for(BInt i=1; i<=set.Card(); i++)
    {
      if(set[i]->Grammar()->Name()=="Ratio")
      {
        BText id = set[i]->Identify();
        outliers[o++] = new BOutlier(id,Rat(set[i]));
      }
      else
      {
        Warning(I2("Non Ratio typed element for AIA at ",
                   "Elemento de tipo no Ratio para AIA en el ")+
                i+I2("-th element", "-ésimo elemento"));
      }
    }
    outliers.ReallocBuffer(o);
  }
  else
  {
    outliers = BAia::sysOutliers_;
  }
  if(!Arg(4))
  {
  //BTimer tm("Automatic Intervention Analysis (AIA)\n");
    BAia aia(res, rat, outliers);
    contens_.RobElement(aia.Input());
  }
  else
  {
    int  optMaxOrder                   =   2;
    int  optMaxCrossNum                = 100;
    BDat optMinNonZeroParamProb        = .99;
    BDat optMaxEigenValueRelativeRange = 100;
    if(Arg(4)) { optMaxOrder                   = (int)Real(Arg(4)); }
    if(Arg(5)) { optMaxCrossNum                = (int)Real(Arg(5)); }
    if(Arg(6)) { optMinNonZeroParamProb        = Dat(Arg(6)); }
    if(Arg(7)) { optMaxEigenValueRelativeRange = Dat(Arg(7)); }
    BMultAia aia
    (
      res, 
      rat, 
      outliers, 
      optMaxOrder, 
      optMaxCrossNum,
      optMinNonZeroParamProb,
      optMaxEigenValueRelativeRange
    );
    contens_.RobElement(aia.Input());
  }
  delete res;
}


//--------------------------------------------------------------------
class BTsrRationExpand : public BTsrDummy
//--------------------------------------------------------------------
{
private:
  BOutlier* out_;
public:
  BTsrRationExpand(BList* arg) : BTsrDummy(arg)
  {
    BRat& rat = Rat(LstNthCar(arg,3));
    out_ = BOutlier::Find(rat);
    if(!out_) { out_ = new BOutlier("", rat); }
  }
//BDat operator[] (const BDate& dte) { return(GetDat(dte)); }
  BDat GetDat(const BDate& dte);
  RedeclareClassNewDelete(BTsrRationExpand);
};



//--------------------------------------------------------------------
DeclareEvaluator(BTsrRationExpand);
DefExtOpr(1, BTsrRationExpand, "RationExpand", 3, 3, "Date TimeSet Ratio",
  I2("(Date from, TimeSet dating, Ratio rat)",
     "(Date desde, TimeSet fechado, Ratio rat)"),
  I2("Returns the expansion of a Ratio along a dating from a Date",
     "Devuelve la expansión de una función racional a lo largo de un fechado"
     "desde "),
    BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BTsrRationExpand::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(dte < Center()) { return(BDat(0.0)); }
  BInt dif = Dating()->Difference(Center(), dte);
  BDat x = out_->Expand(dif);
  return(x);
}

