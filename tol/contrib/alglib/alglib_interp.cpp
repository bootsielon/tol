/* alglib_interp.cpp: Header file for interface between C++ and C for 
                      AlgLib and GNU/TOL Language.

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

/*!
  \file   alglib_interp.cpp
  \author vdebuen@tol-project.org
  \date   2007-08-21

  \brief  API of TOL for AlgLib interpolation functions  
*/


#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <math.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_btsrgra.h>

#include "polinterpolation.h"
#include "ratinterpolation.h"
#include "spline3.h"
#include "leastsquares.h"

BTraceInit("alglib_interp.cpp");


//--------------------------------------------------------------------
  enum AlgLibType
//--------------------------------------------------------------------
{
  ALT_None                   = 0,
  ALT_Constant               = 1,
  ALT_BarycentricRational    = 2,
  ALT_LinearSpline           = 3,
  ALT_CubicSpline            = 4,
  ALT_AkimaSpline            = 5,
  ALT_SplineLeastSquares     = 6,
  ALT_ChebyshevLeastSquares  = 7,
  ALT_PolynomialLeastSquares = 8,
  ALT_NumOptions             = 9 
};

//--------------------------------------------------------------------
  class AlgLib_Method_Info
//--------------------------------------------------------------------
{
 public:
  AlgLibType type_;
  BText      name_;
  int        minPoints_;
  BText      url_;
  static AlgLib_Method_Info algLibTypeName_[ALT_NumOptions];

  AlgLib_Method_Info(AlgLibType type, const BText& name, int minPoints, const BText& url)
  : type_      (type),
    name_      (name),
    minPoints_ (minPoints),
    url_       (url)
  { }

 ~AlgLib_Method_Info() {}

  static AlgLibType getType(const BText&  typeName)
  {
    BText tn = ToLower(typeName);
    AlgLibType t=ALT_None;
    int i;
    for(i=ALT_Constant+1; i<ALT_NumOptions; i++)
    {
      if(tn==ToLower(algLibTypeName_[i].name_))
      { 
        t = (AlgLibType)i; 
        break;
      }
    }
    return(t);
  }

};

#define ALMetInf(NAME,MINPOINTS,URL) AlgLib_Method_Info(ALT_##NAME, #NAME, MINPOINTS, BText("http://www.alglib.net/interpolation/")+URL )

  AlgLib_Method_Info AlgLib_Method_Info::algLibTypeName_[ALT_NumOptions] =
{
  ALMetInf(None,                    0, "" ),
  ALMetInf(Constant,                1, "" ),
  ALMetInf(BarycentricRational,     2, "rational.php" ),
  ALMetInf(LinearSpline,            2, "spline3.php" ),
  ALMetInf(CubicSpline,             3, "spline3.php" ),
  ALMetInf(AkimaSpline,             5, "spline3.php" ),
  ALMetInf(SplineLeastSquares,      2, "linearleastsquares.php" ),
  ALMetInf(ChebyshevLeastSquares,   2, "linearleastsquares.php" ),
  ALMetInf(PolynomialLeastSquares,  2, "linearleastsquares.php" )
};

//--------------------------------------------------------------------
  class AlgLib_Interpolation_Handler
//--------------------------------------------------------------------
{
 public:
  int pointsNumber_;
  int order_;
  AlgLibType type_; 
  ap::real_1d_array x_;
  ap::real_1d_array f_;
  ap::real_1d_array workSpace_;
  double constant_;

  AlgLib_Interpolation_Handler()
  : pointsNumber_(0), order_(0), x_(), f_(), workSpace_(), type_(ALT_None), constant_(BDat::Nan())
  {
  }
  AlgLib_Interpolation_Handler(AlgLibType t, 
                            const double* x, 
                            const double* f,
                            int pointsNumber,
                            int order,
                            bool copy,
                            bool checkConstant)
  : pointsNumber_(0), order_(0), x_(), f_(), workSpace_(), type_(ALT_None), constant_(BDat::Nan())
  {
    Set(t,x,f,pointsNumber,order,copy, checkConstant);
  }
  AlgLib_Interpolation_Handler(const BText&  t, 
                               const double* x, 
                               const double* f,
                               int pointsNumber,
                               int order,
                               bool copy,
                               bool checkConstant)
  : pointsNumber_(0), order_(0), x_(), f_(), workSpace_(), type_(ALT_None), constant_(BDat::Nan())
  {
    Set(t,x,f,pointsNumber,order,copy, checkConstant);
  }
  AlgLib_Interpolation_Handler(const BText&  t, 
                               const double* x, 
                               const double* f,
                                     double* workSpace,
                               int           pointsNumber,
                               int           workSize,
                               int           order,
                               bool          copy)
  : pointsNumber_(0), order_(0), x_(), f_(), workSpace_(), type_(ALT_None), constant_(BDat::Nan())
  {
    SetWorkSpace(t,x,f,workSpace,pointsNumber,workSize,order,copy);
  }

 ~AlgLib_Interpolation_Handler()
  {
    Reset();
  }

  void Reset()
  {
    if(x_.getcontent())
    {
      x_.setbounds(0,-1);
      f_.setbounds(0,-1);
      workSpace_.setbounds(0,-1);
    }
  }

  void SetData(int           pointsNumber, 
               int           order, 
               const double* x, 
               const double* f,
               bool          copy)
  {
    pointsNumber_ = pointsNumber;
    order_ = order;
    x_.setcontent(0,pointsNumber-1,x, copy);
    f_.setcontent(0,pointsNumber-1,f, copy);
  }

  void SetWorkSpaceData(int     pointsNumber, 
                        int     order, 
                        int     workSize, 
                        double* workSpace,
                        bool    copy)
  {
    pointsNumber_ = pointsNumber;
    order_ = order;
    workSpace_.setcontent(0, workSize-1, workSpace, copy);
  }

  bool CheckInsufPoints(AlgLibType t, int& pointsNumber)
  {
    if(pointsNumber<AlgLib_Method_Info::algLibTypeName_[t].minPoints_)
    {
      Error(I2("Cannot use AlgLib Interpolation method ",
               "No se puede usar AlgLib Interpolation de tipo ")+AlgLib_Method_Info::algLibTypeName_[t].name_+
            I2(" with less than ",
               " con menos de ") + AlgLib_Method_Info::algLibTypeName_[t].minPoints_ +
            I2(" evaluation points",
               " puntos de evaluación")+".\n"+
            I2("See more on ",
               "Más información en ")+AlgLib_Method_Info::algLibTypeName_[t].url_);
      type_ = ALT_None;
      return(false);
    }
    else
    {
      return(true);
    }
  }


  bool Set(AlgLibType    t,  
           const double* x, 
           const double* f,
           int           pointsNumber,
           int           order,
           bool          copy,
           bool          checkConstant)
  {
    if(!f || !x) { return(false); }
    if(t==ALT_None) { return(false); }
    Reset();
    int j;
    double xMin = BDat::PosInf();
    double xMax = BDat::NegInf();
    for(j=1; j<pointsNumber; j++) 
    {
      xMin = Minimum(xMin, x[j]);
      xMax = Maximum(xMax, x[j]);
    }
    if(checkConstant)
    {
      bool is_constant = true;
      for(j=1; is_constant && (j<pointsNumber); j++) 
      {
        is_constant = f[j]==f[j-1];
      }
      if(is_constant) { t = ALT_Constant; }
    }
    type_ = t;
    switch(type_)
    {
      case ALT_Constant:
      {
        constant_ = f[0];
        break;
      }
      case ALT_BarycentricRational:
      {
        order = pointsNumber;
        if(!CheckInsufPoints(t,pointsNumber)) { return(false); }
        SetData(pointsNumber,order,x,f,copy);
        int d=3;
        buildfloaterhormannrationalinterpolant(x_,pointsNumber_,d,workSpace_);
        assert(workSpace_.getcontent());
        break;
      }
      case ALT_LinearSpline:
      {
        order = pointsNumber;
        if(!CheckInsufPoints(t,pointsNumber)) { return(false); }
        SetData(pointsNumber,order,x,f,copy);
        buildlinearspline(x_,f_,pointsNumber_,workSpace_);
        assert(workSpace_.getcontent());
        break;
      }
/*
      case ALT_HermiteSpline:
      {
        if(!CheckInsufPoints(t,pointsNumber)) { return(false); }
        SetData(pointsNumber,x,f,copy);
        buildhermitespline(x_,f_,pointsNumber_,workSpace_);
        break;
      }
*/
      case ALT_CubicSpline:
      {
        if(!CheckInsufPoints(t,pointsNumber)) { return(false); }
        SetData(pointsNumber,order,x,f,copy);
        buildcubicspline(x_,f_,pointsNumber_,0,0,0,0,workSpace_);
        assert(workSpace_.getcontent());
        break;
      }
      case ALT_AkimaSpline:
      {
        order = pointsNumber;
        if(!CheckInsufPoints(t,pointsNumber)) { return(false); }
        SetData(pointsNumber,order,x,f,copy);
        buildakimaspline(x_,f_,pointsNumber_,workSpace_);
        assert(workSpace_.getcontent());
        break;
      }
      case ALT_SplineLeastSquares:
      {
        if(!CheckInsufPoints(t,pointsNumber)) { return(false); }
        SetData(pointsNumber,order,x,f,copy);
        ap::real_1d_array weights;
        weights.setbounds(0,pointsNumber-1);
        for(j=0; j<pointsNumber; j++) { weights(j)=1.0; }
        buildsplineleastsquares(x_, f_, weights, xMin, xMax, pointsNumber, order, workSpace_);
        assert(workSpace_.getcontent());
        break;
      }
      case ALT_ChebyshevLeastSquares:
      {
        if(!CheckInsufPoints(t,pointsNumber)) 
        { return(false); }
        SetData(pointsNumber,order,x,f,copy);
        ap::real_1d_array weights;
        weights.setbounds(0,pointsNumber-1);
        for(j=0; j<pointsNumber; j++) { weights(j)=1.0; }
        buildchebyshevleastsquares(x_, f_, weights, xMin, xMax, pointsNumber, order, workSpace_);
        assert(workSpace_.getcontent());
        break;
      }
      case ALT_PolynomialLeastSquares:
      {
        if(!CheckInsufPoints(t,pointsNumber)) { return(false); }
        SetData(pointsNumber,order,x,f,copy);
        ap::real_1d_array weights;
        weights.setbounds(0,pointsNumber-1);
        for(j=0; j<pointsNumber; j++) { weights(j)=1.0; }
        buildpolynomialleastsquares(x_, f_, pointsNumber, order, workSpace_);
        assert(workSpace_.getcontent());
        break;
      }
      default :
      { 
        Error(I2("Unimplimented or unknown AlgLib Interpolation method ",
                 "Método AlgLib Interpolation no implementado o desconocido ")+
              AlgLib_Method_Info::algLibTypeName_[t].name_);
        return(false); 
      }
    }
  }

  bool Set(const BText&  typeName, 
           const double* x, 
           const double* f,
           int           pointsNumber,
           int           order,
           bool          copy,
           bool          checkConstant)
  {
    AlgLibType t=AlgLib_Method_Info::getType(typeName);
    return(Set(t,x,f,pointsNumber,order,copy,checkConstant));
  }

  bool SetWorkSpace(AlgLibType    t,  
                    const double* x, 
                    const double* f,
                          double* workSpace,
                    int           pointsNumber,
                    int           workSize,
                    int           order,
                    bool          copy)
  {
    if(t==ALT_None)  { return(false); }
    Reset();
    type_ = t;
    switch(type_)
    {
      case ALT_Constant:
      {
        constant_ = workSpace[0];
        break;
      }
      case ALT_BarycentricRational:
      {
        SetData(pointsNumber,order,x,f,copy);
        SetWorkSpaceData(pointsNumber,order, workSize, workSpace, copy);
        break;
      }
      case ALT_LinearSpline:
      case ALT_CubicSpline:
      case ALT_AkimaSpline:
      case ALT_SplineLeastSquares:
      case ALT_ChebyshevLeastSquares:
      case ALT_PolynomialLeastSquares:
      {
        SetWorkSpaceData(pointsNumber,order, workSize, workSpace, copy);
        break;
      }
      default :
      {
        Error(I2("Unimplimented or unknown AlgLib Interpolation method ",
                 "Método AlgLib Interpolation no implementado o desconocido ")+
                 AlgLib_Method_Info::algLibTypeName_[t].name_);
        return(false); 
      }
    }
  }

  bool SetWorkSpace(const BText&  typeName, 
                    const double* x, 
                    const double* f,
                    const double* workSpace,
                    int           pointsNumber,
                    int           workSize,
                    int           order,
                    bool          copy)
  {
    AlgLibType t=AlgLib_Method_Info::getType(typeName);
    return(SetWorkSpace(t,x,f,workSpace,pointsNumber,workSize,order,copy));
  }

  double Evaluate(double t) const
  {
    switch(type_)
    {
      case ALT_Constant:
        return(constant_);
      case ALT_BarycentricRational:
        return(barycentricinterpolation(x_, f_, workSpace_, pointsNumber_, t));
      case ALT_LinearSpline:
      case ALT_CubicSpline:
      case ALT_AkimaSpline:
      case ALT_SplineLeastSquares:
        return(splineinterpolation(workSpace_, t));
      case ALT_ChebyshevLeastSquares:
        return(calculatechebyshevleastsquares(order_, workSpace_, t));
      case ALT_PolynomialLeastSquares:
        return(calculatechebyshevleastsquares(order_, workSpace_, t));
      default :
        return(BDat::Nan());
    }
  }
};

static BText AlgLibMoreAbout = 
  I2("Valid interpolation types for argument <interpType> are:\n",
     "Los tipos de interpolación admisibles para el argumento <interpType> son:\n")+
  "  \"BarycentricRational\"\n"+
  "  \"LinearSpline\"\n"+
  "  \"CubicSpline\"\n"+
  "  \"AkimaSpline\"\n"+
  "  \"SplineLeastSquares\"\n"+
  "  \"ChebyshevLeastSquares\"\n"+
  "  \"PolynomialLeastSquares\"\n"+
  I2("More details in","Más detalles en")+
  " http://www.alglib.net/interpolation";

static BText AlgLibEvalDescOptions_ = I2
(
  "If <mode> is  0 evaluates the interpolation function in <a>.\n"
  "If <mode> is  1 evaluates the derivative of the interpolation function in <a>.\n"
  "If <mode> is -1 evaluates the integral of the interpolation function between <a> and <b>.\n"
  "NOTE: At this moment just first option has been implemented.\n",
  "Si <modo> es  0 evalúa la función de interpolación en <a>\n"
  "Si <modo> es  1 evalúa la derivada de la función de interpolación en <a>\n"
  "Si <modo> es -1 evalúa la integral de la función de interpolación entra <a> f <b>\n"
  "NOTA: Por el momento sólo se ha implementado la primera opción.\n"
)+AlgLibMoreAbout;

//--------------------------------------------------------------------
class AlgLib_Interpolation_Tol_Eval: public BExternalOperator
//--------------------------------------------------------------------
{
private:
  AlgLib_Interpolation_Handler handler_; 
public:
  AlgLib_Interpolation_Tol_Eval(const BText& name, 
                                const BText& typeName, 
                                const double* x, 
                                const double* f,
                                int   pointsNumber,
                                int   order,
                                bool  copy)
  : BExternalOperator
    (
      name, GraReal(), "Real", NIL, 1, 1,
      I2("(Real a)",
         "(Real a)"),
		  I2("AlgLib interpolation functions handler created with TOL function Alglib.Interp.Scalar(...).\n",
         "Manejador de funciones de interpolación de AlgLib creado con la función TOL Alglib.Interp.Scalar(...)\n")+
      AlgLibEvalDescOptions_,
		  BOperClassify::NumericalAnalysis_
    ),
    handler_(typeName, x, f, pointsNumber, order, copy, true)
  {
    
  }
 ~AlgLib_Interpolation_Tol_Eval() {}

  BSyntaxObject* Evaluator(BList* arg) const
  { 
    BList* lst = arg;
    double a,f;
    a  = Real(lst->Car()); lst = lst->Cdr();
    f = handler_.Evaluate(a); 
/*
    int    mode = 0;
    else if(mode==1)
    {
      f = handler_.Derivative(a); 
    }
    else if(mode==2)
    {
      f = handler_.Derivative2(a); 
    }
    else if(mode==-1)
    {
      if(!lst)
      {
        Error(I2("Argument <b> of integration upper bound is mandatory when <mode> is -1",
                 "El argumento <b> de límite superior de integración es obligatorio cuando <modo> es -1"));
      }
      double b = Real(lst->Car());
      f = handler_.Integral(a,b); 
    }
*/
    BContensDat* result = new BContensDat("",f);
    DESTROY(arg);
    return(result);
  }
 private:
  DeclareClassNewDelete(AlgLib_Interpolation_Tol_Eval);
};

static BText  warning_non_ois_storable_function =
  I2("\nWARNING: This function is not storable in OIS!\n",
     "\nCUIDADO: ¡Esta función no es almacenable en OIS!\n");

static BText alglib_vector_interp_order_copy_description_ = I2
(
  "Some methods are not exactly interpolations but uniform aproximations "
  "estimated by least squares over a speficic family of functions which "
  "dimension is given by optional argument <order>. If <order> is not set "
  "or is non positive or method is not a least squares one, then <order> "
  "will be set as columns of <x> and <f> that are the same, of course.\n"

  "If optional argument <copy> is true then a copy of matrices <x> and <f> will "
     "be used, else it will use directly these matrices and user must ensure "
     "they are alive while interpolation vectorial function handler were in use."
     "This feature can be usefull to save memory working with very large matrices "
     "but must be used carefully.\n"

  ,
  "Algunos métodos no son exáctamente de interpolación sino de aproximación "
  "uniforme estimada por mínimos cuadrados sobre una familia específica de "
  "funciones de dimensión igual al argumento opcional <order>. Si no se "
  "pasa este argumento o no es positivo o el método no es de mínimos cuadrados "
  "sino de interpolación exacta, entonces se usará el número de columnas de "
  "las matrices <x> e <f> que ha de ser lógicamente el mismo.\n"

  "Si el argumento opcional <copy> es cierto entonces se usará internamente "
     "una copia de las matrices <x> e <f> pero en caso contrario se usarán "
     "directamente y será responsabilidad del usaurio asegurar que dichas "
     "matrices permanecen vivas mientras se usa el manejador. Esta "
     "característica puede ser útil para ahorrar memoria cuando se manejan "
     "matrices muy grandes pero debe emplearse con sumo cuidado.\n"
);

//--------------------------------------------------------------------
DeclareContensClass(BCode, BCodeTemporary, BCodeAlgLibInterp);
DefExtOpr(1, BCodeAlgLibInterp, "AlgLib.Interp.Scalar", 3, 5, "Text Matrix Matrix Real Real",
	  "(Text interpType, Matrix x, Matrix f [, Real order=0, Real copy=false])",
	  I2("Creates a AlgLib interpolation function handler.\n",
	   "Crea un manejador de funciones de interpolación de AlgLib.\n")+
    alglib_vector_interp_order_copy_description_+
    warning_non_ois_storable_function+
    AlgLibMoreAbout,
	  BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BCodeAlgLibInterp::CalcContens()
//--------------------------------------------------------------------
{
  BText& typeName = Text(Arg(1));
  DMat&  x_   = (DMat&)Mat(Arg(2));
  DMat&  f_   = (DMat&)Mat(Arg(3));
  int pointsNumber = f_.Data().Size(); //Number of points to evaluate
  int order        = pointsNumber;
  if(Arg(4)) 
  { 
    order = (int)Real(Arg(4)); 
  }
  bool copy = true;
  if(Arg(5)) 
  { 
    copy = Real(Arg(5))!=0; 
  }
  const BArray<double>& x = x_.Data();
  const BArray<double>& f = f_.Data();
  AlgLib_Interpolation_Tol_Eval* opr;
  opr = new AlgLib_Interpolation_Tol_Eval("", typeName, 
                                          x.Buffer(), f.Buffer(), 
                                          pointsNumber, order, copy);
  contens_.PutOperator(opr);
}


//--------------------------------------------------------------------
static BText AlgLibVInterpDesc(const BText& genFun)
//--------------------------------------------------------------------
{
  return(I2("AlgLib interpolation vectorial functions handler "
            "created with TOL function ",
            "Manejador de funciones vectoriales de interpolación "
            "de AlgLib creado con la función TOL ")+
         genFun+"(...).\n"+AlgLibEvalDescOptions_);
};

//--------------------------------------------------------------------
class AlgLib_Vector_Interpolation_Tol_Eval: public BExternalOperator
//--------------------------------------------------------------------
{
private:
  BArray <AlgLib_Interpolation_Handler> handler_; 

public:
  AlgLib_Vector_Interpolation_Tol_Eval(const BText& name, 
                                       const BText& typeName, 
                                       const double* x,
                                       const double* f,
                                       int pointsNumber,
                                       int order,
                                       int funcsNumber,
                                       bool x_is_common,
                                       bool copy,
                                       bool getWorkSpace,
                                       BMatrix<double>& workSpace)
  : BExternalOperator
    (
      name, GraMatrix(), "Real Real Real", NIL, 1, 3,
      "(Real mode, Real a [, Real b])",
		  AlgLibVInterpDesc("AlgLib.Interp.Vector"),
		  BOperClassify::NumericalAnalysis_
    ),
    handler_(funcsNumber)
  {
    Set(name, typeName, x, f, pointsNumber, order, funcsNumber, 
        x_is_common, copy, getWorkSpace, workSpace);
  }

  AlgLib_Vector_Interpolation_Tol_Eval(const BText& name, 
                                       const BText& typeName, 
                                       const double* x,
                                       const double* f,
                                       double* workSpace,
                                       int pointsNumber,
                                       int workSize,
                                       int order,
                                       int funcsNumber,
                                       bool x_is_common,
                                       bool copy)
  : BExternalOperator
    (
      name, GraMatrix(), "Real Real Real", NIL, 1, 3,
      "(Real mode, Real a [, Real b])",
		  AlgLibVInterpDesc("AlgLib.Interp.Vector.LoadWorkSpace"),
		  BOperClassify::NumericalAnalysis_
    ),
    handler_(funcsNumber)
  {
    SetWorkSpace(name, typeName, x, f, workSpace, pointsNumber, workSize, order, funcsNumber, x_is_common, copy);
  }

 ~AlgLib_Vector_Interpolation_Tol_Eval() {}

  void Set(const BText& name, 
           const BText& typeName, 
           const double* x,
           const double* f,
           int pointsNumber,
           int order,
           int funcsNumber,
           bool x_is_common,
           bool copy,
           bool getWorkSpace,
           BMatrix<double>& workSpace)
  {
    int i;
    const double* x_, *f_;
    int workSize=-1;
    AlgLibType type = AlgLib_Method_Info::getType(typeName);
    for(i=0; i<handler_.Size(); i++)
    {
      x_ = (x_is_common)?x:x+i*pointsNumber;
      f_ = f+i*pointsNumber;
      handler_[i].Set(type, x_, f_, pointsNumber, order, copy, false);
      assert(handler_[i].workSpace_.getcontent());
      if(getWorkSpace)
      {
        if(workSize<0)
        {
          workSize = 1+handler_[i].workSpace_.gethighbound()-handler_[i].workSpace_.getlowbound();
          workSpace.Alloc(funcsNumber,workSize);
        }
        assert(workSize == 1+handler_[i].workSpace_.gethighbound()-handler_[i].workSpace_.getlowbound());
        workSpace.GetData().GetBuffer()[0]=handler_[i].type_;
        memcpy(workSpace.GetData().GetBuffer()+i*workSize, 
               handler_[i].workSpace_.getcontent(),
               workSize*sizeof(double));
      }
    }
  }

  void SetWorkSpace(const BText& name, 
                    const BText& typeName, 
                    const double* x,
                    const double* f,
                    double* workSpace,
                    int pointsNumber,
                    int workSize,
                    int order,
                    int funcsNumber,
                    bool x_is_common,
                    bool copy)
  {
    int i;
    const double* x_, *f_;
    double *w_;
    AlgLibType type = AlgLib_Method_Info::getType(typeName);
    for(i=0; i<handler_.Size(); i++)
    {
      x_ = (x==NULL)?x:((x_is_common)?x:x+i*pointsNumber);
      f_ = (f==NULL)?f:(f+i*pointsNumber);
      w_ = workSpace+i*workSize;
      handler_[i].SetWorkSpace(type, x_, f_, w_, pointsNumber, workSize, order, copy);
    }
  }

  BSyntaxObject* Evaluator(BList* arg) const
  { 
    BList* lst = arg;
    int    mode, i;
    double a;
    BMat f(handler_.Size(),1);
    mode = (int)Real(lst->Car()); lst = lst->Cdr();
    a    =      Real(lst->Car()); lst = lst->Cdr();
    if(mode==0)
    {
      for(i=0; i<handler_.Size(); i++)
      {
        f(i,0) = handler_[i].Evaluate(a); 
      }
    }
/*
    else if(mode==1)
    {
      for(i=0; i<handler_.Size(); i++)
      {
        f(i,0) = handler_[i].Derivative(a); 
      }
    }
    else if(mode==2)
    {
      for(i=0; i<handler_.Size(); i++)
      {
        f(i,0) = handler_[i].Derivative2(a); 
      }
    }
    else if(mode==-1)
    {
      if(!lst)
      {
        Error(I2("Argument <b> of integration upper bound is mandatory when <mode> is -1",
                 "El argumento <b> de límite superior de integración es obligatorio cuando <modo> es -1"));
      }
      double b = Real(lst->Car());
      for(i=0; i<handler_.Size(); i++)
      {
        f(i,0) = handler_[i].Integral(a,b); 
      }
    }
*/
    DESTROY(arg);
    return(new BContensMat("",f));
  }
 private:
  DeclareClassNewDelete(AlgLib_Vector_Interpolation_Tol_Eval);
};


static BText alglib_vector_interp_description_ = I2
(
  "Each row of matrices <x> and <f> is referred to a real to real function.\n"
  "If <x> has just one row then it will be take for each row of <f>\n"
  ,
	"Cada fila de las matrices <x> e <f> se refiere a una función de real a real.\n"
  "Si <x> sólo tiene una fila se tomará la misma para todas las filas de <f>\n."
) + alglib_vector_interp_order_copy_description_;

static BText warning_ois_storement = I2
(
  "In order to store all needed information you can use "
  "AlgLib.Interp.Vector.CreateWorkSpace and then you can restore it "
  "and apply to AlgLib.Interp.Vector.LoadWorkSpace"
  ,
  "Para poder almacenar toda la información necesaria use "
  "AlgLib.Interp.Vector.CreateWorkSpace y entonces podrá restaurarla "
  "posteriormente y aplicarla a AlgLib.Interp.Vector.LoadWorkSpace"
);

//--------------------------------------------------------------------
DeclareContensClass(BCode, BCodeTemporary, BCodeAlgLibVectorInterp);
DefExtOpr(1, BCodeAlgLibVectorInterp, "AlgLib.Interp.Vector", 3, 6, 
  "Text Matrix Matrix Real Real Code",
	"(Text interpType, Matrix x, Matrix f [, Real order=0, Real copy=false, Code interp=?])",
	I2("Creates a AlgLib interpolation vectorial function handler",
     "Crea un manejador de funciones vectoriales de interpolación de AlgLib")+".\n"+
  alglib_vector_interp_description_+
  I2("If an existent vectorial interpolator function is passed as the "
     "optional argument <interp> then it will be modified instead of create a new one",
     "Si se pasa una función de interpolación vectorial en el argumento opcional "
     "<interp>, entonces se modificará éste, en lugar de crear uno nuevo")+".\n"+
  warning_non_ois_storable_function+"\n"+
  warning_ois_storement+
  AlgLibMoreAbout,
	BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BCodeAlgLibVectorInterp::CalcContens()
//--------------------------------------------------------------------
{
  BText& typeName = Text(Arg(1));
  DMat&  x_   = (DMat&)Mat(Arg(2));
  DMat&  f_   = (DMat&)Mat(Arg(3));
  const BArray<double>& x = x_.Data();
  const BArray<double>& f = f_.Data();
  BMatrix<double> workSpace;
  AlgLib_Vector_Interpolation_Tol_Eval* opr;
  bool x_is_common = x_.Rows()==1;
  int pointsNumber = f_.Columns(); //Number of points to evaluate
  int funcsNumber  = f_.Rows();    //Number of functions
  int order        = pointsNumber;
  if(Arg(4)) 
  { 
    order = (int)Real(Arg(4)); 
  }
  bool copy = true;
  if(Arg(5)) 
  { 
    copy = Real(Arg(5))!=0; 
  }
  if(!Arg(6))
  {
    opr = new AlgLib_Vector_Interpolation_Tol_Eval
    ( "", typeName, x.Buffer(), f.Buffer(), pointsNumber, order, funcsNumber, x_is_common, copy, false, workSpace);
  }
  else
  {
    BCode& code = Code(Arg(6));
    opr = (AlgLib_Vector_Interpolation_Tol_Eval*)code.Operator();
    opr->Set
    ( "", typeName, x.Buffer(), f.Buffer(), pointsNumber, order, funcsNumber, x_is_common, copy, false, workSpace);
  }
  contens_.PutOperator(opr);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetAlgLibVectorInterpCreateWorkSpace);
DefExtOpr(1, BSetAlgLibVectorInterpCreateWorkSpace, 
  "AlgLib.Interp.Vector.CreateWorkSpace", 3, 5, 
  "Text Matrix Matrix Real Real",
	"(Text interpType, Matrix x, Matrix f [, Real order=0, Real copy=false])",
	I2("Creates the working space needed for evaluate a AlgLib "
     "interpolation vectorial function handler.\n",
     "Crea el espacio de trabajo necesario para evaluar un manejador "
     "de funciones vectoriales de interpolación de AlgLib.\n")+
  alglib_vector_interp_description_+
  AlgLibMoreAbout,
	BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BSetAlgLibVectorInterpCreateWorkSpace::CalcContens()
//--------------------------------------------------------------------
{
  BText& typeName = Text(Arg(1));
  DMat&  x_   = (DMat&)Mat(Arg(2));
  DMat&  f_   = (DMat&)Mat(Arg(3));
  const BArray<double>& x = x_.Data();
  const BArray<double>& f = f_.Data();
  bool x_is_common = x_.Rows()==1;
  int pointsNumber = f_.Columns(); //Number of points to evaluate
  int funcsNumber  = f_.Rows();    //Number of functions
  int order        = pointsNumber;

  BMatrix<double> workSpace;
  if(Arg(4)) 
  { 
    order = (int)Real(Arg(4)); 
  }
  bool copy = true;
  if(Arg(5)) 
  { 
    copy = Real(Arg(5))!=0; 
  }
  AlgLib_Vector_Interpolation_Tol_Eval* opr = 
    new AlgLib_Vector_Interpolation_Tol_Eval
    ( "", typeName, x.Buffer(), f.Buffer(), pointsNumber, order, funcsNumber, x_is_common, copy, true, workSpace);
  AlgLibType type = AlgLib_Method_Info::getType(typeName);
  BList *lst = NULL, *aux=NULL;
  BGrammar::DecLevel();
  contens_.PrepareStore(7);
  BContensText* typeName_     = new BContensText ("",typeName,         ""); 
  BContensDat*  pointsNumber_ = new BContensDat  ("",pointsNumber,     ""); 
  BContensDat*  funcsNumber_  = new BContensDat  ("",funcsNumber,      ""); 
  BContensDat*  order_        = new BContensDat  ("",order,            ""); 
  BContensMat*  workSpace_    = new BContensMat  ("",(BMat&)workSpace, ""); 
  typeName_    ->PutName("typeName"); 
  pointsNumber_->PutName("pointsNumber"); 
  funcsNumber_ ->PutName("funcsNumber"); 
  order_       ->PutName("order"); 
  workSpace_   ->PutName("workSpace"); 
  contens_.AddElement(typeName_);
  contens_.AddElement(pointsNumber_);
  contens_.AddElement(funcsNumber_);
  contens_.AddElement(order_);
  contens_.AddElement(workSpace_);
  if(type == ALT_BarycentricRational)
  {
    BContensMat*  x__ = new BContensMat  ("",(BMat&)x_, ""); 
    BContensMat*  f__ = new BContensMat  ("",(BMat&)f_, ""); 
    x__   ->PutName("x"); 
    f__   ->PutName("f"); 
    contens_.AddElement(x__);
    contens_.AddElement(f__);
  }
  BGrammar::IncLevel();
//contens_.PutOperator(opr);
}


//--------------------------------------------------------------------
DeclareContensClass(BCode, BCodeTemporary, BCodeAlgLibVectorInterpLoadWorkSpace);
DefExtOpr(1, BCodeAlgLibVectorInterpLoadWorkSpace, 
  "AlgLib.Interp.Vector.LoadWorkSpace", 1, 1, 
  "Set",
	"(Set workSpace)",
	I2("Creates a AlgLib interpolation vectorial function handler "
     "from the set resulting of",
     "Crea un manejador de funciones vectoriales de interpolación "
     "de AlgLib a partir del conjunto resultado de la función ")+
     "AlgLib.Interp.Vector.CreateWorkSpace.\n"+
  warning_non_ois_storable_function+"\n"+
  warning_ois_storement,
	BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BCodeAlgLibVectorInterpLoadWorkSpace::CalcContens()
//--------------------------------------------------------------------
{
  BSet& wi = Set(Arg(1));
  BText& typeName     =        Text(wi["typeName"]);
  int    pointsNumber = (int)  Real(wi["pointsNumber"]);
  int    funcsNumber  = (int)  Real(wi["funcsNumber"]);
  int    order        = (int)  Real(wi["order"]);
  DMat&  workSpace    = (DMat&)Mat (wi["workSpace"]);
  AlgLibType type = AlgLib_Method_Info::getType(typeName);
  const double* x = NULL;
  const double* f = NULL;
        double* w = workSpace.GetData().GetBuffer();
  bool  x_is_common = false;

  if(type == ALT_BarycentricRational)
  {
    DMat&  x_ = (DMat&)Mat (wi["x"]);
    DMat&  f_ = (DMat&)Mat (wi["f"]);
    x = x_.Data().Buffer();
    f = f_.Data().Buffer();
    x_is_common = x_.Rows()==1;
  }
  AlgLib_Vector_Interpolation_Tol_Eval* opr;
  opr = new AlgLib_Vector_Interpolation_Tol_Eval
  ( "", typeName, x, f, w, pointsNumber, workSpace.Columns(), order, funcsNumber, x_is_common, false);
  contens_.PutOperator(opr);
}


