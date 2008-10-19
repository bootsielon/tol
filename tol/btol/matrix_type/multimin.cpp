/* multimin.cpp: Multidimensional minimization.
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
  \file   multimin.cpp
  \author 
  \date   Fri Mar 21 2:55:25 2003
  
  \brief  Multidimensional minimization
*/

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bcommon.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include <tol/tol_bcodgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bsetgra.h>

BTraceInit("multimin.cpp");

//--------------------------------------------------------------------
class BMultidimFunction :  public BSetTemporary
//--------------------------------------------------------------------
{
public:
    BMultidimFunction(BList * args) : BSetTemporary(args) 
	{
    Initialize();
	    flags_.calculated_ = BFALSE;
	    if (!mc_UX->NRefs())
		mc_UX->IncNRefs();
	    CheckArguments();
	}
    
  static bool Initialize()
  {
    /* Alloc the matrix for the evaluations */
    if (!mc_UX) 
    {
      mc_UX = BContensMat::New("", BMatrix<BDat>(),"UX");
      return(true);
    }
    else
    {
      return(false);
    }
  }
    virtual void CalcContens();
    
    
    static double FunctionEval(const gsl_vector *x, 
			       void * params);

    static void   GradientEval(const gsl_vector *v, 
			       void *params, 
			       gsl_vector *df);

    static void   BothFandG   (const gsl_vector *x, 
			       void *params, 
			       double *f, 
			       gsl_vector *df); 
  
    //! Pointer to the user code to evaluate in FunctionEval
    static BCode * CurrentFunction; 
    //! \a BSyntaxObject used to evaluate \a CurrentFunction
    static BUserMat * mc_UX; 
    static int dimension;
    static int numIter;
  
protected:
    BBool m_ArgsOk;  /*!< Status of the arguments */
    
    BBool CheckArguments(); 
    virtual void MultidimensionalMinimization (BArray<BDat>& p, 
					       BDat globTol, 
					       BDat linTol, 
					       BDat& iter, 
					       BDat& fret);
  
    virtual BList * AddExtraParameters(BList *);
};
//--------------------------------------------------------------------

BCode *       BMultidimFunction::CurrentFunction = NULL;
BUserMat *    BMultidimFunction::mc_UX = NULL; 
int           BMultidimFunction::dimension = 0;
int           BMultidimFunction::numIter   = 100;

void BMultidimFunction::CalcContens()
{
    BList * lst = NIL, * aux = NIL;
    if ( flags_.calculated_ )
	return;
    if (m_ArgsOk) {
	dimension       = (int)((((BUserDat*)Arg(3))->Contens()).Value());
	BText english("Starting minimization -- Dimension: "+BDat(dimension));
      BText spanish("Comenzando minimización -- Dimension: "+BDat(dimension));
        english += Name();
	english += "\n";
	spanish += Name();
	spanish += "\n";
	Std(I2(english,spanish));
	/* do minimization */
	int i,r,  n = dimension;
	
	BMatrix<BDat> *X;
	BDat * datarr;
	BArray<BDat> p(n), pf(n);
	
	X = &(((BUserMat*)Arg(2))->Contens());
	datarr = X->GetData().GetBuffer();
	for (r = 0; r < n; r++) {
	    if (datarr[r].Known())
		p(r) = datarr[r].Value();
	}
	
	double stepSize = (double)((((BUserDat*)Arg(4))->Contens()).Value());
	double lTol     = (double)((((BUserDat*)Arg(5))->Contens()).Value());
	numIter         = (int)((((BUserDat*)Arg(6))->Contens()).Value());
    
	CurrentFunction = &Code(Arg(1));
	
	mc_UX->Contens().Alloc(dimension,1);
	BDat fret, iter = numIter;
	BMat p1(n,1);
	
	MultidimensionalMinimization (p, stepSize, lTol, iter, fret);
	for (i=0; i<n; i++) p1(i, 0)=p[i];
	
	BUserMat* p0    = BContensMat::New("Point", p1);
	BUserDat* fret0 = BContensDat::New("MinimumValue", fret);
	BUserDat* iter0 = BContensDat::New("IterNumber", iter);
	
	BList* lst = Cons(p0,Cons(fret0,NCons(iter0)));
	
	lst = AddExtraParameters(lst);
	
	contens_.RobElement(lst);
	/* now I'm calculated */
	flags_.calculated_ = BTRUE;
	Std(I2("Minimization done\n","Minimización finalizada\n"));
    }
}


//--------------------------------------------------------------------
void BMultidimFunction::MultidimensionalMinimization (BArray<BDat>& p, 
						      BDat globTol, 
						      BDat linTol, 
                                                      BDat& iter, 
						      BDat& fret)
//--------------------------------------------------------------------
{
  // Inicializa estructura necesaria para minimizar
    gsl_multimin_function_fdf st_func;
    
    st_func.f   = &BMultidimFunction::FunctionEval;
    st_func.df  = &BMultidimFunction::GradientEval;
    st_func.fdf = &BMultidimFunction::BothFandG;
    st_func.n   = dimension;
    st_func.params = NIL;
    
    size_t iters = 0;
    int status; 
    int i;
    
    const gsl_multimin_fdfminimizer_type *T;
    gsl_multimin_fdfminimizer *s;
    T = gsl_multimin_fdfminimizer_conjugate_fr;
    s = gsl_multimin_fdfminimizer_alloc (T, dimension);
    
    double gTol = globTol.Value();
    double lTol = linTol.Value();
    gsl_vector *x = gsl_vector_alloc(dimension);
    for(i=0; i<dimension; i++) gsl_vector_set(x, i, p(i).Value());
    
    gsl_multimin_fdfminimizer_set (s, &st_func, x, gTol, lTol);
    do{
	iters++;
	if (!(iters%10)){
	    Std(I2("Iteration ","Iteración ")+BDat(iters)+BText("\n"));
	}
	status = gsl_multimin_fdfminimizer_iterate (s);
	if (status)
	    break;
	status = gsl_multimin_test_gradient (s->gradient, 1e-3);
    }
    while (status == GSL_CONTINUE && iters < iter);
    
    iter = iters;
    for(i=0; i<dimension; i++) p(i)=gsl_vector_get(s->x, i);
    fret = s->f;
    
    gsl_multimin_fdfminimizer_free (s);
    gsl_vector_free (x);  
}

//--------------------------------------------------------------------
BList * BMultidimFunction::AddExtraParameters(BList * lst)
//--------------------------------------------------------------------
{
    return lst;
}

//--------------------------------------------------------------------
double BMultidimFunction::FunctionEval(const gsl_vector * x, void * params)
//--------------------------------------------------------------------
{
    int r;
    BList * args;
    BSyntaxObject * eval_res;
    BDat * dat_res;
    static double last_res = 0.0;
    static BBool bUserError = BFALSE;
    double dbl_res;
    BDat * mc_XBuffer = mc_UX->Contens().GetData().GetBuffer();
    
    if (bUserError)
	return last_res;
    
    int nref = mc_UX->NRefs();
    for (r = 0; r < dimension; r++) {
	mc_XBuffer[r].PutValue(gsl_vector_get(x, r));
	mc_XBuffer[r].PutKnown();
    }
    eval_res = CurrentFunction->Evaluator(args=NCons(mc_UX));
    /* verify if eval just failed */
    if (eval_res) {
	dat_res = &(((BUserDat*)eval_res)->Contens());
	if (dat_res->Known()) {
	    dbl_res = dat_res->Value();
	} else {
	    BText msg_en("In MultidimensionalMinimization, unknown value in evaluation of ");
	    BText msg_sp("En MultidimensionalMinimization, valor desconocido en la evaluación de ");
	    msg_en.Concat(CurrentFunction->Name());
	    msg_sp.Concat(CurrentFunction->Name());
	    Warning(I2(msg_en, msg_sp));
	}
	DESTROY(eval_res);
    } else {
	// there was an error evaluating the user function, assume the last evaluation 
	bUserError = BFALSE;
	dbl_res = last_res;
    }
    return last_res = dbl_res;
}


//--------------------------------------------------------------------
void BMultidimFunction::GradientEval(const gsl_vector *v, 
				     void *params, 
				     gsl_vector *df)
//--------------------------------------------------------------------
{
    double fxh1, g, gi, fx;
    gsl_vector *y = gsl_vector_alloc(dimension);
    int i;
    
    for(i=0; i<dimension; i++) gsl_vector_set(y, i, gsl_vector_get(v, i));
    fx = FunctionEval(y, params);
    for( i=0; i<dimension; i++)
    {
	gi = gsl_vector_get(y, i);
	gi +=Distance().Value();
	gsl_vector_set(y, i, gi);
	fxh1 = FunctionEval(y, params);
	
	g = (fxh1-fx)/Distance().Value();
	gsl_vector_set(df, i, g);
	gi -=Distance().Value();
	gsl_vector_set(y, i, gi);
    }
    gsl_vector_free (y);
}



//--------------------------------------------------------------------
void   BMultidimFunction::BothFandG(const gsl_vector *x, 
				    void *params, 
				    double *f, 
				    gsl_vector *df)
//--------------------------------------------------------------------
{
    *f = FunctionEval(x, params); 
    GradientEval(x, params, df);
}


//--------------------------------------------------------------------
BBool BMultidimFunction::CheckArguments()
//--------------------------------------------------------------------
{
    /* verify arguments */
    m_ArgsOk = BTRUE;
    return m_ArgsOk;
}


//--------------------------------------------------------------------
DeclareEvaluator(BMultidimFunction);
DefExtOpr(1, BMultidimFunction, "MultidimMin", 6, 6, "Code Matrix Real Real Real Real",
          "(Code function, Matrix point, Real dim, Real stepSize, Real linTol, Real numIter)", 
          I2("Apllies Fletcher-Reeves conjugate gradient algorithm to find a multidimensional   \n"
             "point such that the scalar function takes a value which is lower than at any      \n"
             "neighboring point.   \n"
             "Arguments:                                                                        \n"
             "function  ---> function to be minimized, f R^dim ---> R,                          \n"
             "point     ---> intial point,                                                      \n"
             "dim       ---> dimension of the space function domain                             \n"
             "stepSize  ---> size of the first trial step                                       \n"
             "linTol    ---> accuracy of the line minimization                                  \n", 
             "Aplica la variante Fletcher-Reeves del algoritmo gradiente conjugado              \n"
             "para encontrar el punto donde la función alcanza un mínimo local                  \n"
             "Argumentos:                                                                       \n"
             "function  ---> función a minimizar, f R^dim ---> R,                               \n"
             "point     ---> punto inicial,                                                     \n"
             "dim       ---> dimensión del espacio dominio de la función                        \n"
             "stepSize  ---> tamaño del paso de búsqueda                                        \n"
             "linTol    ---> tolerancia para la minimización lineal                             \n"),
          BOperClassify::NumericalAnalysis_);
