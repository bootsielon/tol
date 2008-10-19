/* tolgsl_interp.cpp: Header file for interface between C++ and C for gsl
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

/*!
  \file   tolgsl_interp.cpp
  \author vdebuen@tol-project.org
  \date   2007-08-21

  \brief  API of TOL for GSL interpolation functions  
*/


#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <math.h>
#include <tol/tol_gsl.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_btsrgra.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

BTraceInit("tolgsl_interp.cpp");


//--------------------------------------------------------------------
  class GSL_Interpolation_Handler
//--------------------------------------------------------------------
{
 public:
  gsl_interp_accel *acc;
  gsl_spline *spline;
  double constant_;

  GSL_Interpolation_Handler()
  : acc(NULL), spline(NULL), constant_(BDat::Nan())
  {
  }
  GSL_Interpolation_Handler(const gsl_interp_type *t, 
                            const double* x, 
                            const double* y,
                            int N)
  : acc(NULL), spline(NULL), constant_(BDat::Nan())
  {
    Set(t,x,y,N);
  }
  GSL_Interpolation_Handler(const BText& t, 
                            const double* x, 
                            const double* y,
                            int N)
  : acc(NULL), spline(NULL), constant_(BDat::Nan())
  {
    Set(t,x,y,N);
  }

 ~GSL_Interpolation_Handler()
  {
    Reset();
  }

  void Reset()
  {
    if(spline)
    {
      gsl_spline_free       (spline);
      gsl_interp_accel_free (acc);  
      spline = NULL;
      acc    = NULL;
    }
  }

  void Set(const gsl_interp_type *t,  
           const double* x, 
           const double* y,
           int N)
  {
    Reset();
    int j;
    bool is_constant = true;
    for(j=1; is_constant && (j<N); j++) 
    {
      is_constant = y[j]==y[j-1];
    }
    if(!is_constant)
    {
      acc = gsl_interp_accel_alloc ();
      spline = gsl_spline_alloc (t, N);
      gsl_spline_init (spline, x, y, N);
    }
    else
    {
      constant_ = y[0];
    }
  }

  void Set(const BText& typeName, 
           const double* x, 
           const double* y,
           int N)
  {
    BText tn = ToLower(typeName);
    const gsl_interp_type *t= NULL;
         if(tn=="linear")           { t = gsl_interp_linear; }
    else if(tn=="polynomial")       { t = gsl_interp_polynomial; }
    else if(tn=="cspline")          { t = gsl_interp_cspline; }
    else if(tn=="cspline_periodic") { t = gsl_interp_cspline_periodic; }
    else if(tn=="akima")            { t = gsl_interp_akima; }
    else if(tn=="akima_periodic")   { t = gsl_interp_akima_periodic; }
    Set(t,x,y,N);
  }

  double Evaluate(double x) const
  {
    return(spline?gsl_spline_eval (spline, x, acc):constant_);
  }

  double Derivative(double x) const
  {
    return(spline?gsl_spline_eval_deriv (spline, x, acc):0);
  }

  double Derivative2(double x) const
  {
    return(spline?gsl_spline_eval_deriv2 (spline, x, acc):0);
  }

  double Integral(double a, double b) const
  {
    return(spline?gsl_spline_eval_integ (spline, a, b, acc):(b-a)*constant_);
  }

};


//--------------------------------------------------------------------
class GSL_Interpolation_Tol_Eval: public BExternalOperator
//--------------------------------------------------------------------
{
private:
  GSL_Interpolation_Handler handler_; 
public:
  GSL_Interpolation_Tol_Eval(const BText& name, 
                             const BText& type, 
                             const double* x, 
                             const double* y,
                             int N)
  : 
    BExternalOperator
    (
      name, 
      GraReal(), 
      "Real Real Real",
		  NIL,
		  1,
		  3,
      I2("(Real mode, Real a [, Real b])",
         "(Real modo, Real a [, Real b])"),
		  I2("GSL interpolation functions handler created with TOL function gsl_interp(...).\n"
         "If <mode> is  0 evaluates the interpolation function in <a>.\n"
         "If <mode> is  1 evaluates the derivative of the interpolation function in <a>.\n"
         "If <mode> is -1 evaluates the integral of the interpolation function between <a> and <b>.\n"
         "More details in http://www.gnu.org/software/gsl/manual/html_node/Interpolation.html",
         "Manejador de funciones de interpolación de GSL creado con la función TOL gsl_interp(...)\n"
         "Si <modo> es  0 evalúa la función de interpolación en <a>\n"
         "Si <modo> es  1 evalúa la derivada de la función de interpolación en <a>\n"
         "Si <modo> es -1 evalúa la integral de la función de interpolación entra <a> y <b>\n"
         "Más detalles en http://www.gnu.org/software/gsl/manual/html_node/Interpolation.html"),
		  BOperClassify::NumericalAnalysis_
    ),
    handler_(type, x, y, N)
  {
    
  }
 ~GSL_Interpolation_Tol_Eval() {}

  BSyntaxObject* Evaluator(BList* arg) const
  { 
    BList* lst = arg;
    int    mode;
    double a,b,f;
    mode = (int)Real(lst->Car()); lst = lst->Cdr();
    a    =      Real(lst->Car()); lst = lst->Cdr();
    if(mode==0)
    {
      f = handler_.Evaluate(a); 
    }
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
      b = Real(lst->Car());
      f = handler_.Integral(a,b); 
    }
    BContensDat* result = new BContensDat("",f);
    DESTROY(arg);
    return(result);
  }
 private:
  DeclareClassNewDelete(GSL_Interpolation_Tol_Eval);
};

//--------------------------------------------------------------------
DeclareContensClass(BCode, BCodeTemporary, BCodeGslInterp);
DefExtOpr(1, BCodeGslInterp, "gsl_interp", 3, 3, "Text Matrix Matrix",
	"(Text interpType, Matrix x, Matrix y)",
  I2("Creates a GSL interpolation function handler\n"
     "Argument interpType must be one of this:\n"
     "[[\"linear\",\"polynomial\",\"cspline\",\"cspline_periodic\",\"akima\",\"akima_periodic\"]]\n"
     "See more on:",
     "Crea un manejador de funciones de interpolación de GSL.\n"
     "El argumento <interpType> debe ser alguno de estos:\n"
     "[[\"linear\",\"polynomial\",\"cspline\",\"cspline_periodic\",\"akima\",\"akima_periodic\"]]\n"
     "Ver más información en:")+"\n"
     "http://www.gnu.org/software/gsl/manual/html_node/Interpolation.html",
	  BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BCodeGslInterp::CalcContens()
//--------------------------------------------------------------------
{
  BText& type = Text(Arg(1));
  DMat&  x_   = (DMat&)Mat(Arg(2));
  DMat&  y_   = (DMat&)Mat(Arg(3));
  const BArray<double>& x = x_.Data();
  const BArray<double>& y = y_.Data();
  GSL_Interpolation_Tol_Eval* opr;
  opr = new GSL_Interpolation_Tol_Eval("", type, 
                                       x.Buffer(), y.Buffer(), 
                                       x.Size());
  contens_.PutOperator(opr);
}


//--------------------------------------------------------------------
class GSL_Vector_Interpolation_Tol_Eval: public BExternalOperator
//--------------------------------------------------------------------
{
private:
  BArray <GSL_Interpolation_Handler> handler_; 

public:
  GSL_Vector_Interpolation_Tol_Eval(const BText& name, 
                                    const BText& type, 
                                    const double* x,
                                    const double* y,
                                    int N,
                                    int m,
                                    bool x_is_common)
  : 
    BExternalOperator
    (
      name, 
      GraMatrix(), 
      "Real Real Real",
		  NIL,
		  1,
		  3,
      I2("(Real mode, Real a [, Real b])",
         "(Real modo, Real a [, Real b])"),
		  I2("GSL interpolation vectorial functions handler created with TOL function gsl_interp(...).\n"
         "If <mode> is  0 evaluates the interpolation function in <a>.\n"
         "If <mode> is  1 evaluates the derivative of the interpolation function in <a>.\n"
         "If <mode> is -1 evaluates the integral of the interpolation function between <a> and <b>.\n"
         "More details in http://www.gnu.org/software/gsl/manual/html_node/Interpolation.html",
         "Manejador de funciones vectoriales de interpolación de GSL creado con la función TOL gsl_interp(...)\n"
         "Si <modo> es  0 evalúa la función de interpolación en <a>\n"
         "Si <modo> es  1 evalúa la derivada de la función de interpolación en <a>\n"
         "Si <modo> es -1 evalúa la integral de la función de interpolación entra <a> y <b>\n"
         "Más detalles en http://www.gnu.org/software/gsl/manual/html_node/Interpolation.html"),
		  BOperClassify::NumericalAnalysis_
    ),
    handler_(m)
  {
    Set(name, type, x, y, N, m, x_is_common);
  }

 ~GSL_Vector_Interpolation_Tol_Eval() {}

  void Set(const BText& name, 
           const BText& type, 
           const double* x,
           const double* y,
           int N,
           int m,
           bool x_is_common)
  {
    int i;
    const double* x_, *y_;
    for(i=0; i<handler_.Size(); i++)
    {
      x_ = (x_is_common)?x:x+i*N;
      y_ = y+i*N;
      handler_[i].Set(type, x_, y_, N);
    }
  }

  BSyntaxObject* Evaluator(BList* arg) const
  { 
    BList* lst = arg;
    int    mode, i;
    double a,b;
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
      b = Real(lst->Car());
      for(i=0; i<handler_.Size(); i++)
      {
        f(i,0) = handler_[i].Integral(a,b); 
      }
    }
    DESTROY(arg);
    return(new BContensMat("",f));
  }
 private:
  DeclareClassNewDelete(GSL_Vector_Interpolation_Tol_Eval);
};

//--------------------------------------------------------------------
DeclareContensClass(BCode, BCodeTemporary, BCodeGslVectorInterp);
DefExtOpr(1, BCodeGslVectorInterp, "gsl_vector_interp", 3, 4, 
  "Text Matrix Matrix Code",
	"(Text interpType, Matrix x, Matrix y [, Code interp])",
	I2("Creates a GSL interpolation vectorial function handler.\n"
     "Each row of matrices <x> and <y> is referred to a real to real function.\n"
     "If <x> has just one row then it will be take for each row of <y>\n"
     "If an existent vectorial interpolator function is passed as the "
     "optional argument <interp> then it will be modified instead of create a new one.\n"
     "Argument interpType must be one of this:\n"
     "[[\"linear\",\"polynomial\",\"cspline\",\"cspline_periodic\",\"akima\",\"akima_periodic\"]]\n"
     "See more on:",
	   "Crea un manejador de funciones vectoriales de interpolación de GSL.\n"
     "Cada fila de las matrices <x> e <y> se refiere a una función de real a real.\n"
     "Si <x> sólo tiene una fila se tomará la misma para todas las filas de <y>\n."
     "Si se pasa una función de interpolación vectorial en el argumento opcional "
     "<interp>, entonces se modificará éste, en lugar de crear uno nuevo.\n"
     "El argumento <interpType> debe ser alguno de estos:\n"
     "[[\"linear\",\"polynomial\",\"cspline\",\"cspline_periodic\",\"akima\",\"akima_periodic\"]]\n"
     "Ver más información en:")+"\n"
     "http://www.gnu.org/software/gsl/manual/html_node/Interpolation.html",
	BOperClassify::NumericalAnalysis_);
//--------------------------------------------------------------------
void BCodeGslVectorInterp::CalcContens()
//--------------------------------------------------------------------
{
  BText& type = Text(Arg(1));
  DMat&  x_   = (DMat&)Mat(Arg(2));
  DMat&  y_   = (DMat&)Mat(Arg(3));
  const BArray<double>& x = x_.Data();
  const BArray<double>& y = y_.Data();
  GSL_Vector_Interpolation_Tol_Eval* opr;
  bool x_is_common = x_.Rows()==1;
  if(!Arg(4))
  {
    opr = new GSL_Vector_Interpolation_Tol_Eval
    (
      "", 
      type, 
      x.Buffer(), 
      y.Buffer(), 
      y_.Columns(), 
      y_.Rows(),
      x_is_common
    );
  }
  else
  {
    BCode& code = Code(Arg(4));
    opr = (GSL_Vector_Interpolation_Tol_Eval*)code.Operator();
    opr->Set
    (
      "", 
      type, 
      x.Buffer(), 
      y.Buffer(), 
      y_.Columns(), 
      y_.Rows(),
      x_is_common
    );
  }
  contens_.PutOperator(opr);
}

