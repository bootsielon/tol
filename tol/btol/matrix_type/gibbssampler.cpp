/* gibbssampler.cpp: Gibbs sampler implementation.
                     GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])
   Author: Jorge Suit Perez Ronda <jsperez@bayesinf.com>

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
  \file   gibbssampler.cpp
  \author Jorge Suit Perez Ronda
  \date   Fri Apr  4 07:56:05 2003
  
  \brief  Gibbs sampler
*/

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_gsl.h>
#include <tol/tol_arms.h>
#include <tol/tol_bprdist_internal.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_btxtgra.h>

BTraceInit("gibbssampler.cpp");

class FullConditional : public RealGSLFunctionR1{
public:
  static FullConditional * New( BSet * args, BUserMat * condpar );
  FullConditional( BCode * code, BUserSet * data, BUserMat * condpar);
  virtual ~FullConditional();
  virtual double Sample() = 0; 
  BUserMat * ConditionalParameters() {
    return m_CondParameters;
  }
  BUserDat * PreviousSampleDat() {
    return &m_PrevSample;
  }
  void SetPreviousSampleValue( double value ) {
    m_ptrPrevSampleDat->PutValue(value);
  }
  double PreviousSample() {
    return m_ptrPrevSampleDat->Value();
  }
protected:
  //! External allocated matrix argument of size dim-1
  /*! 
      This address is shared among all FullConditional used 
      in the Gibbs Sampler. Is responsability of the caller
      allocate it.
  */

  BUserMat * m_CondParameters;
  BContensDat m_PrevSample;
  BDat * m_ptrPrevSampleDat;
};

class FullConditionalDirectSampler : public FullConditional {
public:
  FullConditionalDirectSampler( BCode * code, BUserSet * data, BUserMat * condpar );
  virtual ~FullConditionalDirectSampler();
  virtual double Sample();
protected:
  virtual BList * BuildArgs();
};

class FullConditionalDensity : public FullConditional {
public:
  static double StaticLogEval( double x, void * instance );
  FullConditionalDensity( BCode * code, BUserSet * data, BUserMat * condpar, 
			  double left, double right, int islog = 1);
  virtual ~FullConditionalDensity();
  virtual double Sample();
protected:
  BUserDat * XArg() {
    return &m_UDat;
  }
  virtual BList * BuildArgs();
  double m_Left, m_Right;
  int m_IsLog;
};

FullConditional * FullConditional::New( BSet * args, BUserMat * condpar )
{
  FullConditional * ptrObj = NULL;
  BSyntaxObject * syn;
  BCode * ucode;
  BUserSet * uset;
  BText english("Creating full conditional object: ");
  BText spanish("Creando objeto densidad condicional: ");
  
  int card = args->Card();

  if (card == 2 || card == 5) {
    // could be a direct sampler specification.
    syn = (*args)[1];
    if (syn->Grammar() == GraCode()) {
      ucode = &(UCode(syn)->Contens());
      syn = (*args)[2];
      if (syn->Grammar() == GraSet()) {
	uset = USet(syn);
      } else {
	english += "invalid data set member, must be of type Set";
	spanish += "miembro dato inválido, debe ser de tipo Set";
	Error(I2(english,spanish));
	return NULL;
      }
    } else {
      english += "invalid code member, must be of type Code";
      spanish += "miembro código inválido, debe ser de tipo Code";
      Error(I2(english,spanish));
      return NULL;
    }
    if (card == 5) {
      // could be a density sampler specification.
      BDat * dat;
      double values[3];
      const char * txt_en[] = { "invalid left bound, must be a known Real",
				"invalid right bound, must be a known Real",
				"invalid density type, must be 0 or 1"};
      const char * txt_sp[] = { "cota inferior inválida, debe ser un Real conocido",
				"cota superior inválida, debe ser un Real conocido",
				"tipo de densidad inválida, debe ser 0 o 1"};

      for (int i=3; i<=5; i++) {
	syn = (*args)[i];
	if (syn->Grammar() == GraReal()) {
	  dat = &((UDat(syn))->Contens());
	  if (dat->IsKnown()) {
	    values[i-3] = dat->Value();
	  } else {
	    english += txt_en[i-3];
	    spanish += txt_sp[i-3];
	    Error(I2(english,spanish));
	    return NULL;
	  }
	}
      }
      if (values[0] >= values[1]) {
	english += "invalid interval, left bound must be lower than right bound";
	spanish += "intervalo inválido, cota inferior debe ser menor que cota superior";
	Error(I2(english,spanish));
	return NULL;	
      }
      if (ucode->Operator()->MinArg()!=4 &&
	  ucode->Operator()->MaxArg()!=4) {
	english += "invalid argument number for code argument, should be 4";
	spanish += "número de argumentos inválidos para argumento código: debe ser 4";
	Error(I2(english,spanish));
	return NULL;
      }
      if (ucode->Operator()->GrammarForArg(1) != GraReal()) {
	english += "invalid type ";
	english += ucode->Operator()->GrammarForArg(1)->Name();
	english += " for first argument of code argument: should be Real";
	spanish += "tipo inválido ";
	spanish += ucode->Operator()->GrammarForArg(1)->Name();
	spanish += " en el primer argumento del argumento código: debe ser Real";
	Error(I2(english,spanish));
	return NULL;
      }
      if (ucode->Operator()->GrammarForArg(2) != GraMatrix()) {
	english += "invalid type ";
	english += ucode->Operator()->GrammarForArg(2)->Name();
	english += " for second argument of code argument: should be Matrix";
	spanish += "tipo inválido ";
	spanish += ucode->Operator()->GrammarForArg(2)->Name();
	spanish += " en el segundo argumento del argumento código: debe ser Matrix";
	Error(I2(english,spanish));
	return NULL;
      }
      if (ucode->Operator()->GrammarForArg(3) != GraReal()) {
	english += "invalid type ";
	english += ucode->Operator()->GrammarForArg(3)->Name();
	english += " for third argument of code argument: should be Real";
	spanish += "tipo inválido ";
	spanish += ucode->Operator()->GrammarForArg(3)->Name();
	spanish += " en el tercer argumento del argumento código: debe ser Real";
	Error(I2(english,spanish));
	return NULL;
      }
      if (ucode->Operator()->GrammarForArg(4) != GraSet()) {
	english += "invalid type ";
	english += ucode->Operator()->GrammarForArg(4)->Name();
	english += " for fourth argument of code argument: should be Set";
	spanish += "tipo inválido ";
	spanish += ucode->Operator()->GrammarForArg(4)->Name();
	spanish += " en el cuarto argumento del argumento código: debe ser Set";
	Error(I2(english,spanish));
	return NULL;
      }
      return new FullConditionalDensity(ucode, uset, condpar, values[0], values[1], int(values[2])); 
    } else {
      if (ucode->Operator()->MinArg()!=3 &&
	  ucode->Operator()->MaxArg()!=3) {
	english += "invalid argument number for code argument, should be 4";
	spanish += "número de argumentos inválidos para argumento código: debe ser 4";
	Error(I2(english,spanish));
	return NULL;
      }
      if (ucode->Operator()->GrammarForArg(1) != GraMatrix()) {
	english += "invalid type ";
	english += ucode->Operator()->GrammarForArg(1)->Name();
	english += " for first argument of code argument: should be Matrix";
	spanish += "tipo inválido ";
	spanish += ucode->Operator()->GrammarForArg(1)->Name();
	spanish += " en el primer argumento del argumento código: debe ser Matrix";
	Error(I2(english,spanish));
	return NULL;
      }
      if (ucode->Operator()->GrammarForArg(2) != GraReal()) {
	english += "invalid type ";
	english += ucode->Operator()->GrammarForArg(2)->Name();
	english += " for second argument of code argument: should be Real";
	spanish += "tipo inválido ";
	spanish += ucode->Operator()->GrammarForArg(2)->Name();
	spanish += " en el segundo argumento del argumento código: debe ser Real";
	Error(I2(english,spanish));
	return NULL;
      }
      if (ucode->Operator()->GrammarForArg(3) != GraSet()) {
	english += "invalid type ";
	english += ucode->Operator()->GrammarForArg(3)->Name();
	english += " for third argument of code argument: should be Set";
	spanish += "tipo inválido ";
	spanish += ucode->Operator()->GrammarForArg(3)->Name();
	spanish += " en el tercer argumento del argumento código: debe ser Set";
	Error(I2(english,spanish));
	return NULL;
      }
      return new FullConditionalDirectSampler(ucode, uset, condpar);
    }
  }
  // bad sampler specification.
  english += "invalid sampler specification, must be a set of 2 or 5 elements";
  spanish += "especificación de sampler inválido, debe ser un conjunto de 2 o 5 elementos";
  Error(I2(english,spanish));
  return NULL;
}

FullConditional::FullConditional( BCode * code, BUserSet * data, BUserMat * condpar )
  : RealGSLFunctionR1(code, data), m_CondParameters(condpar), m_PrevSample(BDat(0.0))
{
  m_PrevSample.IncNRefs();
  m_ptrPrevSampleDat = &m_PrevSample.Contens();
}

FullConditional::~FullConditional()
{
}

FullConditionalDirectSampler::FullConditionalDirectSampler( BCode * code, BUserSet * data, BUserMat * condpar )
  : FullConditional(code, data, condpar)
{
}

FullConditionalDirectSampler::~FullConditionalDirectSampler()
{
}

double FullConditionalDirectSampler::Sample()
{
  // return the evaluation of the user code.
  // verify if previous user code error
  if (!UserError()) {
    BSyntaxObject * eval_res;
    BDat * dat_res;

    eval_res = UserCode()->Evaluator(BuildArgs());
    if (eval_res) {
      dat_res = &((UDat(eval_res))->Contens());
      if (dat_res->Known()) {
	// save last evaluation
	LastResult() = dat_res->Value();    
      } else {
	BText msg_en("In FullConditionalDirectSampler::Sample, unknown sample in evaluation of ");
	BText msg_sp("En FullConditionalDirectSampler::Sample, muestra desconocida en la evaluación de ");
	msg_en.Concat(UserCode()->Name());
	msg_sp.Concat(UserCode()->Name());
	Warning(I2(msg_en, msg_sp));
	msg_en = "ConditionalParameters = ";
	msg_en += ConditionalParameters()->Dump();
	Warning(msg_en);
	msg_en = "PreviousSample = ";
	msg_en += PreviousSampleDat()->Dump();
	Warning(msg_en);
	Warning(UserData()->Dump());
      }
      DESTROY(eval_res);
    } else {
      /* there was an error evaluating the user function,
	 assume the last evaluation */
      UserError() = 1;
    }
  }
  SetPreviousSampleValue(LastResult());
  return LastResult();  
}

BList * FullConditionalDirectSampler::BuildArgs()
{
  return Cons(ConditionalParameters(),
	      Cons(PreviousSampleDat(),
		   Cons(UserData(),NIL)));
}

double FullConditionalDensity::StaticLogEval( double x, void * instance )
{
  return log(RealGSLFunction::StaticEvalReal(x,instance));
}

FullConditionalDensity::FullConditionalDensity( BCode * code, BUserSet * data, BUserMat * condpar,
						double left, double right, int islog )
  : FullConditional(code, data, condpar), m_Left(left), m_Right(right), m_IsLog(islog)
{
  /* set a valid initial value */
  SetPreviousSampleValue( (m_Left+m_Right)/2 );
}

FullConditionalDensity::~FullConditionalDensity()
{
}

double FullConditionalDensity::Sample()
{
  double (*ptrFunction)(double, void *);
  double xsamp;
  double xprev;
  int ninit = 4;
  int domet = 1;
  int arms_status;

  ptrFunction = m_IsLog ? RealGSLFunction::StaticEvalReal : FullConditionalDensity::StaticLogEval;
  xprev = PreviousSample();
  arms_status = arms_simple(ninit, &m_Left, &m_Right, ptrFunction, this, domet, &xprev, &xsamp);

  if (arms_status > 0) {
    BText english("In FullConditionalDensity::Sample ");
    BText spanish("En FullConditionalDensity::Sample ");
    char buffer[128];

    sprintf(buffer, "error code = %d", arms_status);
    english += buffer;
    sprintf(buffer, "código de error = %d", arms_status);
    spanish += buffer;
    Error(I2(english,spanish));
    return xprev;
  }
  SetPreviousSampleValue(xsamp);
  return xsamp;
}

BList * FullConditionalDensity::BuildArgs()
{
  return Cons(XArg(),
	      Cons(ConditionalParameters(),
		   Cons(PreviousSampleDat(),
			Cons(UserData(),NIL))));
}

DeclareContensClass(BMat, BMatTemporary, BMatGibbsSampler);
DefExtOpr(1, BMatGibbsSampler, "GibbsSampler", 3, 3, "Set Real Real",
	  "(Set Conditionals, Real BurningPeriod, Real SampleSize)",
	  I2("Muestreo multidimensional mediante Gibbs sampler\n\n"
	     "Argumentos:\n\n"
	     "   Conditionals --> especificación de las densidades de cada uno de los parámetros\n"
	     "                    condicionados al resto de los parámetros. Una densidad condicional\n"
	     "                    la podemos definir de dos formas:\n"
	     "                    1- Sabemos generar las muestras del parámetro condicionado. La densidad\n"
	     "                       se define como un conjunto formado por el generador de muetras de la\n"
	     "                       densidad y un conjunto de datos externos. Ejemplo: [[RNormalX, Mydata]]\n"
             "                       El prototipo del generador debe ser:\n"
	     "                           Real (Matrix CondParameters, Real Previous, Set data)\n\n"
             "                           CondParameters -> valor de los parámetros condicionantes.\n"
             "                           Previous       -> valor previo del parámetro condicionado.\n"
             "                           data           -> datos globales, necesarios para la evaluación\n"
	     "                                             del generador.\n\n"
	     "                    2- Solo sabemos evaluar la densidad (hasta un factor constante) o el\n"
             "                       logaritmo de la densidad (menos un término constante). La densidad se\n"
             "                       define por un evaluador de la misma, un intervalo de evaluación y un flag\n"
	     "                       indicando si se evalua la densidad o el logaritmo de la densidad,\n"
	     "                       Ejemplo: [[LogNormalX,MyData,-100,100,1]]. El prototipo del generador debe ser:\n"
	     "                           Real (Real X, Matrix CondParameters, Real Previous, Set data)\n\n"
	     "                           X -> valor del dominio donde evaluar la densidad.\n"
             "                           Resto de parametros descritos en 1-\n"
	     "  BuringPeriod --> Número de muestra a descartar inicialmente.\n"
	     "  SampleSize   --> Tamaño de muestras a generar. El valor de retorno de la función es una matriz donde\n"
	     "                   las filas son la muetras de la variable y las columnas los valores de los parámetros.\n\n"
	     "Ejemplo:\n\n",
	     "Muestreo multidimensional mediante Gibbs sampler\n\n"
	     "Argumentos:\n\n"
	     "   Conditionals --> especificación de las densidades de cada uno de los parámetros\n"
	     "                    condicionados al resto de los parámetros. Una densidad condicional\n"
	     "                    la podemos definir de dos formas:\n"
	     "                    1- Sabemos generar las muestras del parámetro condicionado. La densidad\n"
	     "                       se define como un conjunto formado por el generador de muetras de la\n"
	     "                       densidad y un conjunto de datos externos. Ejemplo: [[RNormalX, Mydata]]\n"
             "                       El prototipo del generador debe ser:\n"
	     "                           Real (Matrix CondParameters, Real Previous, Set data)\n\n"
             "                           CondParameters -> valor de los parámetros condicionantes.\n"
             "                           Previous       -> valor previo del parámetro condicionado.\n"
             "                           data           -> datos globales, necesarios para la evaluación\n"
	     "                                             del generador.\n\n"
	     "                    2- Solo sabemos evaluar la densidad (hasta un factor constante) o el\n"
             "                       logaritmo de la densidad (menos un término constante). La densidad se\n"
             "                       define por un evaluador de la misma, un intervalo de evaluación y un flag\n"
	     "                       indicando si se evalua la densidad o el logaritmo de la densidad,\n"
	     "                       Ejemplo: [[LogNormalX,MyData,-100,100,1]]. El prototipo del generador debe ser:\n"
	     "                           Real (Real X, Matrix CondParameters, Real Previous, Set data)\n\n"
	     "                           X -> valor del dominio donde evaluar la densidad.\n"
             "                           Resto de parametros descritos en 1-\n"
	     "  BuringPeriod --> Número de muestra a descartar inicialmente.\n"
	     "  SampleSize   --> Tamaño de muestras a generar. El valor de retorno de la función es una matriz donde\n"
	     "                   las filas son la muetras de la variable y las columnas los valores de los parámetros.\n\n"
	     "Ejemplo:\n\n"),
	  BOperClassify::Statistic_);

void BMatGibbsSampler::CalcContens()
{
  BText english("In GibbsSampler ");
  BText spanish("En GibbsSampler ");
  int burning;
  int sample_size;
  int card;
  BSet * setspec;

  setspec = &((USet(Arg(1)))->Contens());
  card = setspec->Card();
  if (card <= 1) {
    english += "invalid conditionals specification, should be a set of at least two elements";
    spanish += "especificación de condicionales inválidas, debe ser un conjunto de al menos dos elementos";
    Error(I2(english,spanish));
    contens_.Alloc(1,1);
    return;
  }
  burning = (int)(((UDat(Arg(2)))->Contens()).Value());
  if (burning <= 0) {
    english += "invalid burning period, should be positive";
    spanish += "período de quemado inválido, debe ser positivo";
    Error(I2(english,spanish));
    contens_.Alloc(1,1);
    return;
  }
  sample_size = (int)(((UDat(Arg(3)))->Contens()).Value());
  if (sample_size <= 0) {
    english += "invalid sample size, should be positive";
    spanish += "tamaño de muestra inválido, debe ser positivo";
    Error(I2(english,spanish));
    contens_.Alloc(1,1);
    return;
  }

  int conds_num = 0;
  int i;
  BSyntaxObject * syn;

  FullConditional ** conds = new (FullConditional*[card]);
  BContensMat condparameters(BMatrix<BDat>(card,1));
  BDat * condpar_buffer = condparameters.Contens().GetData().GetBuffer();
  condparameters.IncNRefs();
  
  for (i = 0; i < card; i++) {
    syn = (*setspec)[i+1];
    if (syn->Grammar() != GraSet()) {
      english += "conditional specification should be a Set";
      spanish += "la espacificación de una condicional debe ser un Set";
      Error(I2(english,spanish));
      break;
    }
    if (!(conds[i] = FullConditional::New(&((USet(syn))->Contens()), &condparameters)))
      break;
    ++conds_num;
  }

  if (i==card) {
    // do sampling
    contens_.Alloc(sample_size, card);
    // may be we should use external initials values for the parameters
    // for now we set the initial parameters to 0's
    
    for (i=0; i<card; ++i) {
      /* each FullConditional has now a valid initial value */
      //conds[i]->SetPreviousSampleValue(0.0);
      condpar_buffer[i].PutKnown();
    }
    

    double sample_value;
    BDat * cell;
    int total_sim = burning+sample_size;
    int j,k;
    BText msg_en("Burning ");
    BText msg_sp("Descartando ");
    msg_en += burning;
    msg_en += " samples ...\n";
    msg_sp += burning;
    msg_sp += " muestras ...\n";
    Std(I2(msg_en,msg_sp));
    for (i=0; i<burning; ++i) {
      // shift parameters
      for (j=1; j < card; ++j)
	condpar_buffer[j-1].PutValue(conds[j]->PreviousSample());
      // sample parameters
      for (j=0; j < card; ++j)
	condpar_buffer[j].PutValue(conds[j]->Sample());
    }
    msg_en = "Generating ";
    msg_en += sample_size;
    msg_en += " samples ...\n";
    msg_sp = "Generando ";
    msg_sp += sample_size;
    msg_sp += " muestras ...\n";
    Std(I2(msg_en,msg_sp));    
    // main sampling loop
    for (k=0; i < total_sim; ++i,++k) {
      if (!(k%100)) {
	printf("Sampling %d\n", k);
      }
      // shift parameters
      for (j=1; j < card; ++j)
	condpar_buffer[j-1].PutValue(conds[j]->PreviousSample());
      // sample parameters
      for (j=0; j < card; ++j) {
	sample_value = conds[j]->Sample();
	condpar_buffer[j].PutValue(sample_value);
	cell = &contens_(k,j);
	cell->PutValue(sample_value);
      }
    }
  } else {
    contens_.Alloc(1,1);
  }

  // release conditionals

  for (i = 0; i < conds_num; i++)
    delete conds[i];
  delete []conds;
}

/*
 * class MH_workspace --
 *
 *   class defining and implementing a workspace to be shared among all the
 *   blocks used in a Metropolis-Hasting. The main purpose of this class is to
 *   encapsulate the iteration over the blocks of parameters and updating from
 *   one block to other.
 *   
 */

class MH_workspace 
{
public:
  /*
   * MH_workspace --
   *
   *   alloc space for workspace vectors given the size of the join vector of
   *   parameters.
   */
  MH_workspace(size_t v_size, size_t blocks, BSet * extra)
    : umat_my(BMat::Unknown()),
      umat_other(BMat::Unknown()),
      umat_all(BMat::Unknown()),
      udat_id_ws(BDat(0))
    {
      umat_my.IncNRefs();
      umat_other.IncNRefs();
      umat_all.IncNRefs();
      udat_id_ws.IncNRefs();
      size = v_size;
      number_of_blocks = blocks;
      log_pi_values = new double[blocks];
      log_pi_flags = new int[blocks];
      for (int i = 0; i < blocks; i++) {
        log_pi_flags[i] = 0;
        log_pi_values[i] = 0.0;
      }
      mat_my = &((UMat(&umat_my))->Contens());
      mat_my->Alloc(size,1);
      buffer_my = mat_my->GetData().GetBuffer();
      ptr_umat_my = &umat_my;
      mat_other = &((UMat(&umat_other))->Contens());
      mat_other->Alloc(size,1);
      buffer_other = mat_other->GetData().GetBuffer();
      mat_all = &((UMat(&umat_all))->Contens());
      mat_all->Alloc(size,1);
      buffer_all = mat_all->GetData().GetBuffer();
      blk_prev_idx = -1;
      blk_prev_size = 0;
      init_extra(extra);
    }

  /*
   * ~MH_workspace --
   */
  
  ~MH_workspace() 
    {
      if (size_extra) {
        for (size_t i=0; i < size_extra; i++) {
          extra_data0[i]->DecNRefs();
          DESTROY(extra_data0[i]);
        }
        /* extra_data1 should be set to NULL */
        delete []extra_data0;
        delete []extra_data1;
        MH_workspace::vector_of_ws.erase(MH_workspace::vector_of_ws.begin()+id_ws);
      }
      /* WS could contain references to my local objects, so DecNRefs should
       * go after releasing the workspace */
      umat_my.DecNRefs();
      umat_other.DecNRefs();
      umat_all.DecNRefs();
      udat_id_ws.DecNRefs();
      delete []log_pi_values;
      delete []log_pi_flags;
    }
  
  BUserMat * params() 
    {
      return ptr_umat_my;
    }
  BUserMat * saved_state()
    {
      return &umat_my;
    }
  BMat * mat_my_params()
    {
      return mat_my;
    }
  BUserMat * other_params()
    {
      return &umat_other;
    }
  BMat * mat_other_params()
    {
      return mat_other;
    }
  BUserMat * all_params()
    {
      return &umat_all;
    }
  BMat * mat_all_params()
    {
      return mat_all;
    }

  BUserDat * id_workspace()
    {
      return &udat_id_ws;
    }

  /*
   * build_args1 --
   * 
   */

  BList * build_args1(BSyntaxObject * data)
    {
      BList * args;

      args = Cons(params(),
                  Cons(other_params(),
                       Cons(all_params(),
                            Cons(data,NULL))));
      if (size_extra)
        args = LstAppend(args, id_workspace());
      return args;
    }
  
  /*
   *  prepare --
   *
   *    resize the workspace vectors according to the size of current block.
   *    copy from all_params my currents values.
   */

  void prepare(int blk_idx, size_t blk_size)
    {
      int r1, r2;
      
      /* resize current parameters */
      mat_my->Alloc(blk_size, 1);
      /* copy my values from mat_all */
      /* why not use memcpy instead of this for? */
      for (r1 = 0, r2 = blk_idx; r1 < blk_size; ++r1, ++r2) {
        buffer_my[r1] = buffer_all[r2];
      }
      /* update other (conditioning) parameters */
      mat_other->Alloc(size-blk_size,1);
      /* copy previous block */
      if (blk_prev_size) { /* copy only if it precedes me*/
        /* r1 control the loop size */
        for (r1 = 0, r2 = blk_prev_idx; r1 < blk_prev_size; ++r1, ++r2) {
          buffer_other[r2] = buffer_all[r2];
        }
      } else {
        r2 = 0;
      }
      /* now r1 keep track of the position in buffer_other */
      r1 = r2;
      
      /*  here r2 == blk_idx (wrt. buffer_all)
       *  now from r2+blk_size until size (wrt. buffer_all)
       *  copy rest of parameters into buffer_other
       */

      r2 += blk_size; /* skip buffer_my inside buffer_all */
      if (r2==size) {
        /* this is the last block, so the next prepare will start in the first
         * block */
        blk_prev_idx = -1;
        blk_prev_size = 0;
      } else {
        for (; r2 < size; r1++,r2++) {
          buffer_other[r1] = buffer_all[r2];
        }
        blk_prev_idx = blk_idx;
        blk_prev_size = blk_size;
      }
    }
  
  /*
   * copy_params --
   *
   *   copy the vector sample into all_params in place of the block blk_id.
   *   returns the index of the first params of the next block. Sample must be
   *   the same size of the current state.
   */
  
  int copy_params(BMat * sample, int blk_idx, size_t blk_size)
    {
      if (sample->Rows() != blk_size && sample->Columns() != 1) {
        char str[64];
        sprintf(str, "%d", blk_size);
        Error(I2(BText("Invalid sample dimension should be a column vector of size ")+str,
                 BText("Dimension de muestra inválida, debe ser un vector columna de tamaño ")+str));
        return 0;
      }
      BDat * buffer_sample;

      buffer_sample = sample->GetData().GetBuffer();
      /* copy sample contents into all_params */
      /* why not use memcpy ? */
      int rs, rb;
      for (rs=0, rb = blk_idx; rs < blk_size; rs++, rb++)
        buffer_all[rb] = buffer_sample[rs];
      return rb;
    }

  /*
   * change_state --
   *
   *   Change the current state temporally, should be used in combination
   *   with restore_state.
   */

  int change_state(BUserMat * sample, int blk_idx, size_t blk_size)
    {
      if (copy_params(&(sample->Contens()), blk_idx, blk_size)) {
        ptr_umat_my = sample;
        mat_my = &(params()->Contens());
        buffer_my = mat_my->GetData().GetBuffer();
        return 1;
      } else
        return 0;
    }
  
  /*
   * restore_state --
   *
   *   restore my_params into block blk_idx of all_params. Should be used in
   *   combination with change_state. (Not well documented.)
   */
  
  int restore_state(int blk_idx, size_t blk_size)
    {
      /* should assert it use in combination of change_state and also blk_idx
       * and blk_size */
      
      /* restore umat_my == saved_state() as the current state */
      ptr_umat_my = saved_state();
      mat_my = &(params()->Contens());
      buffer_my = mat_my->GetData().GetBuffer();
      /* why not use memcpy ? */
      int rs, rb;
      for (rs=0, rb = blk_idx; rs < blk_size; rs++, rb++)
        buffer_all[rb] = buffer_my[rs];
      return rb;
    }

  int logpi_is_precomputed(int blk_id) 
    {
      assert(blk_id >= 0 && blk_id < number_of_blocks);
      
      return log_pi_flags[blk_id];
    }

  double logpi(int blk_id)
    {
      assert(blk_id >= 0 && blk_id < number_of_blocks);
      
      return log_pi_values[blk_id];
    }

  void save_logpi(int blk_id, double v)
    {
      assert(blk_id >= 0 && blk_id < number_of_blocks);
      
      log_pi_values[blk_id] = v;
      log_pi_flags[blk_id] = 1;
    }

  int dimension() 
    {
      return size;
    }

  /*
   * get_extra --
   */
  BSyntaxObject * get_extra(int i)
    {
      if (i < 0 || i >= size_extra)
        return NULL;
      return extra_data0[i];
    }
  
  /*
   * put_extra --
   */
  int put_extra(int i, BSyntaxObject * so)
    {
      if (i < 0 || i >= size_extra)
        return -1; /* invalid so index */
      if (extra_data1[i]) {
        /* release reference to the previously saved object */
        extra_data1[i]->DecNRefs();
        DESTROY(extra_data1[i]);
      }
      extra_data1[i] = so;
      so->IncNRefs();
      return 0;
    }
  
  /*
   * sync_extra
   */
  void sync_extra()
    {
      for (size_t i=0; i < size_extra; i++) {
        if (extra_data1[i]) {
          /* release extra0 */
          extra_data0[i]->DecNRefs();
          DESTROY(extra_data0[i]);
          extra_data0[i] = extra_data1[i];
          extra_data1[i] = NULL;
        }
      }
    }

  /*
   * clear_extra --
   */
  void clear_extra()
    {
      for (size_t i=0; i < size_extra; i++) {
        if (extra_data1[i]) {
          extra_data1[i]->DecNRefs();
          DESTROY(extra_data1[i]);
        }
      }
    }

  /*
   * get_id_extra --
   */
  std::vector<MH_workspace*>::size_type get_id_ws()
    {
      return id_ws;
    }

  static
  MH_workspace * get_ws_ptr(int id)
    {
      std::vector<MH_workspace*>::size_type i;

      if (id >= 0) {
        i = std::vector<MH_workspace*>::size_type(id);
        if (i < MH_workspace::vector_of_ws.size())
          return MH_workspace::vector_of_ws[i];
      }
      return NULL;
    }
  
  static
  void mask_pointer(void * ptr, char buffer[], char mask[])
    {
      char buffer_[sizeof(void*)+1];
      
      *((void**)&buffer_) = ptr;
      for (register unsigned int i=0; i < sizeof(void*); i++) {
        if (buffer_[i]=='\0') {
          buffer[i] = 'A';
          mask[i] = 1;
        } else {
          buffer[i] = buffer_[i];
          mask[i] = 0;
        }
      }
      buffer[sizeof(void*)] = '\0';
    }

  static
  void * umask_pointer(const char buffer[], const char mask[])
    {
      char buffer_[sizeof(void*)+1];
      
      for (unsigned int i=0; i < sizeof(void*); i++) {
        if (mask[i])
          buffer_[i] = '\0';
        else
          buffer_[i] = buffer[i];
      }
      return *((void**)&buffer_);
    }
  
protected:
  
  void init_extra(BSet * extra)
    {
      if (extra && (size_extra = extra->Card())) {
        extra_data0 = new BSyntaxObject*[size_extra];
        extra_data1 = new BSyntaxObject*[size_extra];
        for (size_t i = 0; i < size_extra; i++) {
          extra_data0[i] = (*extra)[i+1];
          extra_data0[i]->IncNRefs();
          extra_data1[i] = NULL;
        }
        MH_workspace::vector_of_ws.push_back(this);
        id_ws = MH_workspace::vector_of_ws.size()-1;
        udat_id_ws.Contens().PutValue(double(id_ws));
      } else {
        size_extra = 0;
        extra_data0 = NULL;
        extra_data1 = NULL;
      }
    }

private:
  size_t size;
  size_t number_of_blocks;
  /* precomputed values of log pi(x) for every block */
  double * log_pi_values;
  /* flag indicating if a block has being precomputed */
  int * log_pi_flags;
  /* idx & size of previous block sampled */
  int blk_prev_idx;
  size_t blk_prev_size;
  /* vector for parameters of current block */
  BContensMat umat_my;
  BMat * mat_my;
  BDat * buffer_my;
  /* temporary reference used between copy_sample & restore */
  BUserMat * ptr_umat_my;
  
  /* vector for conditioning parameters */
  BContensMat umat_other;
  BMat * mat_other;
  BDat * buffer_other;
  
  /* vector for all parameters */
  BContensMat umat_all;
  BMat * mat_all;
  BDat * buffer_all;
  /* extra data for this workspace */
  size_t size_extra;
  BSyntaxObject ** extra_data0; /* actual extra wokspace */
  BSyntaxObject ** extra_data1; /* temporal extra workspace */
  std::vector<MH_workspace*>::size_type id_ws;
  BContensDat udat_id_ws;
  static
  std::vector<MH_workspace*> vector_of_ws; 
};

std::vector<MH_workspace*> MH_workspace::vector_of_ws;

int MHWS_put_extra(int id_mh_ws, int idx_so, BSyntaxObject * so)
{
  int ret;
  
  MH_workspace* mh_ws = MH_workspace::get_ws_ptr(id_mh_ws);
  if (mh_ws) {
    ret =  mh_ws->put_extra(idx_so, so);
  } else {
    ret = -2; /* invalid MH_WS index */
  }
  return ret;
}

int MHWS_get_extra(int id_mh_ws, int idx_so, BSyntaxObject * &so)
{
  int ret;

  MH_workspace* mh_ws = MH_workspace::get_ws_ptr(id_mh_ws);
  if (mh_ws) {
    so = mh_ws->get_extra(idx_so);
    ret = so? 0/* ok */ : -1/* inv. so index */; 
  } else {
    so = NULL;
    ret = -2; /* invalid MH_WS index */
  }
  return ret;
}

/*
 * class MH_blk_sampler --
 *
 *   class defining and implementing a block sampler for Metropolis-Hasting.
 *   
 */

class MH_blk_sampler {
public:
  MH_blk_sampler() 
    {
      count_accepted = 0;
      blk_id = -1;
      blk_start = -1;
      blk_size = 0;
      blk_status = 0;
      drawer_type = direct;
      is_symmetric = 1;
      target_density = NULL;
      kernel_drawer = NULL;
      kernel_density = NULL;
      data = NULL;
      mat_initial = NULL;
    }
  MH_blk_sampler(const MH_blk_sampler & sampler) 
    {
      count_accepted = sampler.count_accepted;
      blk_id = sampler.blk_id;
      blk_start = sampler.blk_start;
      blk_size = sampler.blk_size;
      blk_status = sampler.blk_status;
      drawer_type = sampler.drawer_type;
      is_symmetric = sampler.is_symmetric;
      target_density = sampler.target_density;
      kernel_drawer = sampler.kernel_drawer;
      kernel_density = sampler.kernel_density;
      data = sampler.data;
      mat_initial = sampler.mat_initial;
    }
  MH_blk_sampler & operator=(const MH_blk_sampler & sampler) 
    {
      count_accepted = sampler.count_accepted;
      blk_id = sampler.blk_id;
      blk_start = sampler.blk_start;
      blk_size = sampler.blk_size;
      blk_status = sampler.blk_status;
      drawer_type = sampler.drawer_type;
      is_symmetric = sampler.is_symmetric;
      target_density = sampler.target_density;
      kernel_drawer = sampler.kernel_drawer;
      kernel_density = sampler.kernel_density;
      data = sampler.data;
      mat_initial = sampler.mat_initial;
      return *this;
    }
  
  MH_blk_sampler(int id, /* identifier of the block */
                 int idx_start, /* position within all_param of this block */
                 BSet & blk_def /* block definition */, int has_ws);
  
  /* draw a sample for the current iteration. Each drawer should update
   * my_param with the new sample generated. my_param & other_param are
   * resized according to the size of this block.
   * Returns: -1 -> ok (reject sample), 0 -> error, 1 -> ok (accept sample) */
  int draw(MH_workspace & workspace)
    {
      BUserMat * sample = NULL;
      
      workspace.prepare(blk_start, blk_size);
      //printf("--- drawer_type = %d\n", drawer_type);
      switch (drawer_type) {
          case direct:
            blk_status = draw_direct(workspace, sample);
            break;
          case rwalk:
            blk_status = draw_mh_rwalk(workspace, sample);
            break;
          case general:
            blk_status = draw_mh_generic(workspace, sample);
            break;
          default:
            blk_status = 0;
      }
      //printf("--- blk_status %d\n", blk_status);
      if (blk_status == 1) {
        assert(sample);
        ++count_accepted;
        if (!workspace.copy_params(&(sample->Contens()),blk_start, blk_size)) {
          blk_status = 0;
          workspace.clear_extra();
        } else
          workspace.sync_extra();         
        DESTROY(sample);
      } else
        workspace.clear_extra();        
      return blk_status;
    }
  
  /* block's status. instance construction was ok. */
  int status()
    {
      return blk_status;
    }

  int id()
    {
      return blk_id;
    }
  
  int idx_start()
    {
      return blk_start;
    }

  size_t size() 
    {
      return blk_size;
    }

  BMat * initial_state()
    {
      return mat_initial;
    }
  
  void reset_count_accepted()
  {
    count_accepted = 0;
  }
  
  size_t get_count_accepted()
  {
    return count_accepted;
  }
 
  
protected:
  /* depending on the blk_def draw will choose one of the following drawers */
  
  /* draw_direct --
   *
   *   Direct sampler
   *   
   *   Returns:
   *   -1 -> ok (reject sample), 0 -> error, 1 -> ok (accept sample)
   */
  int draw_direct(MH_workspace & workspace, BUserMat * & sample);
  
  /*
   * draw_mh_rwalk --
   *
   *   random walk kernel
   *   q(x,y) = q1(y-x)
   *   y = x + z
   *   z ~ q1
   *
   *   Returns:
   *   -1 -> ok (reject sample), 0 -> error, 1 -> ok (accept sample)
   */
  int draw_mh_rwalk(MH_workspace & workspace, BUserMat * & sample);
  
  /*
   * draw_mh_generic --
   *
   *   Generic Metropolis-Hastings.
   *
   *   Returns:
   *   -1 -> ok (reject sample), 0 -> error, 1 -> ok (accept sample)
   */
  int draw_mh_generic(MH_workspace & workspace, BUserMat * & sample);

  /*
   *  sample_from_kernel --
   *
   *    Generate a sample from q(x,.)
   */
  
  BUserMat *  sample_from_kernel(MH_workspace & workspace);
  
  /*
   * evaluate_target --
   *
   *   Evaluate the target density log pi(x) at the current state.
   */
  
  int evaluate_target(MH_workspace & workspace, double & result);

  /*
   *  evaluate_kernel --
   *
   *    Evaluate the kernel log q(x,y) for the candidate sample y. The current
   *    state is workspace.
   */
  
  int evaluate_kernel(MH_workspace & workspace, BUserMat * y, double & result);

private:
  size_t count_accepted;
  int blk_id;    /* identifier of the block */
  int blk_start; /* position within all_param of this block */
  size_t blk_size;
  int blk_status; /*-1 -> rejected, 0 ->error, 1 -> accepted */
  enum drawer_t {
    direct=0, rwalk=1, general=2
  } drawer_type;
  /* is the kernel symmetric? */
  int is_symmetric;
  /* density to draw from, if "direct" then it is the drawer code, in other
   * case it is the target density evaluator */
  BCode * target_density;
  /* drawer from the kernel distribution */
  BCode * kernel_drawer;
  /* evaluator for the kernel distribution */
  BCode * kernel_density;
  /* user data set specific for this block */
  BUserSet * data;
  /* initial state */
  BMat * mat_initial;
  /* */
};

/*
 * MH_blk_sampler::MH_blk_sampler --
 *
 *   Constructor. The block definition is:
 *
 *   ["size"] = size of the block, must be > 0.
 *   
 *   ["type"] = 0 -> Direct Sampler, 1 -> Random Walk, 2 -> generic
 *
 *   ["initial"] = column matrix of ["size"] dimension
 *
 *   ["data"] = block specific data, normally hyperparameters.
 *
 *   ["sampler"] = Matrix UserCode(Matrix X, Matrix CX, Matrix X "+" CX, Set
 *   blk_data [, Real ID_workspace]) is a direct sampler. In case of ["type"] == 0 then this the
 *   code that provide the samples for the block. In other case this the
 *   kernel sampler genrating the candidates for the MH loop.
 *
 *   If ["type"] != 0 then we should have the following extra arguments:
 *
 *     ["target"] = Real TargetDenCode(Matrix X, Matrix CX, Matrix X "+" CX, Set blk_data [, Real ID_workspace])
 *     Is the log-density evaluator = log pi(x) -- the log of target density.
 *
 *     ["symmetric"] = 0 -> kernel is non-symmetric, 1 -> kernel is symmetric
 *
 *   If ["symmetric"] = 0 then one more argument is required
 *
 *     ["kernel"] = Real KernelDenCode(Matrix Y, Matrix X, Matrix CX, Matrix X "+" CX, Set blk_data [, Real ID_workspace])
 *     Is the log-kernel evaluator = log q(x,y) -- the log of the kernel density evaluator
 *
 *   In order to be flexible I will not care about the size of blk_de, at
 *   least 5 elements are required. If "type"!=0 2 more elements are
 *   needed. And if "symmetric"==0 then 1 more is also needed.
 *
 */

#define MH_size    1
#define MH_type    2
#define MH_init    3
#define MH_data    4
#define MH_drawer  5
#define MH_target  6
#define MH_sym     7
#define MH_kernel  8

MH_blk_sampler::MH_blk_sampler(int id, /* identifier of the block */
                               int idx_start, /* position within all_param
                                               * of this block */
                               BSet & blk_def, /* block definition */
                               int has_ws)
{
  char str[64], str1[4];
  BSyntaxObject * ptr_so;
  BDat * ptr_dat;
  int card;

  count_accepted = 0;
  blk_status = 0;
  card = blk_def.Card();
  if ( card < 5) {
    sprintf(str, "%d", id);
    Error(I2(BText("A minimun of 5 elements are required in definition of block ")+str,
             BText("Se necesitan como mínimo 5 elementos en la definición del bloque ")+str));
    return;
  }
  /* extract and validate "size" argument */
  ptr_so = blk_def[MH_size];
  if (ptr_so->Grammar() != GraReal()) {
    sprintf(str, "%d", id);
    Error(I2(BText("'size' element of a block definition must be a Real, at block ")+str,
             BText("Elemento 'size' en la definición de un bloque debe ser Real, en bloque ")+str));
    return;
  }
  ptr_dat = &((UDat(ptr_so))->Contens());
  int _size = int(ptr_dat->Value());
  if (!ptr_dat->Known() || _size <= 0) {
    sprintf(str, "%d", id);
    Error(I2(BText("Block size must be > 0, at block ")+str,
             BText("El tamaño del bloque debe ser > 0, en bloque ")+str));
    return;
  }
  blk_size = size_t(_size);
  /* end "size" argument */
  
  /* extract and validate "type" argument */
  ptr_so = blk_def[MH_type];
  if (ptr_so->Grammar() != GraReal()) {
    sprintf(str, "%d", id);
    Error(I2(BText("'type' element of a block definition must be a Real, at block ")+str,
             BText("Elemento 'type' en la definición de un bloque debe ser Real, en bloque ")+str));
    return;
  }
  ptr_dat = &((UDat(ptr_so))->Contens());
  int type = int(ptr_dat->Value());
  if (!ptr_dat->Known() || (type != 0 && type != 1 && type != 2)) {
    sprintf(str, "%d", id);
    Error(I2(BText("Block 'type' should be 0, 1 or 2, at block ")+str,
             BText("Tipo de bloque ('type') debe ser 0, 1 ó 2, en bloque ")+str));
    return;
  }
  drawer_type = drawer_t(type);
  /* end "type" argument */
  
  /* extract and validate "initial" argument */
  ptr_so = blk_def[MH_init];
  mat_initial = (ptr_so->Grammar()==GraMatrix())?&((UMat(ptr_so))->Contens()):NULL;
  if (!mat_initial ||
      (mat_initial->Rows()!=blk_size || mat_initial->Columns()!=1)) {
    sprintf(str, "%d", id);
    Error(I2(BText("'initial' element of a block definition must be a column Matrix the same size as the block, at block ")+str,
             BText("Elemento 'initial' en la definición de un bloque debe ser una Matrix columna del mismo tamaño que el bloque, en bloque ")+str));
    return;
  }
  /* end "initial" argument */

  /* extract and validate "data" argument */
  ptr_so = blk_def[MH_data];
  if (ptr_so->Grammar() != GraSet()) {
    sprintf(str, "%d", id);
    Error(I2(BText("'data' element of a block definition must be a Set, at block ")+str,
             BText("Elemento 'data' en la definición de un bloque debe ser Set, en bloque ")+str));
    return;
  }
  data = USet(ptr_so);
  /* end "data" argument */

  /* extract and validate "drawer" argument */
  ptr_so = blk_def[MH_drawer];
  kernel_drawer = (ptr_so->Grammar() == GraCode())?&((UCode(ptr_so))->Contens()):NULL;
  if (!kernel_drawer || kernel_drawer->Operator()->Grammar() != GraMatrix()) {
    sprintf(str, "%d", id);
    Error(I2(BText("'drawer' element of a block definition must be a Code returning a Matrix, at block ")+str,
             BText("Elemento 'drawer' en la definición de un bloque debe ser Code que retorne una Matriz, en bloque ")+str));
    return;
  }
  int extra = has_ws?1:0;
  if (kernel_drawer->Operator()->MinArg() != (4+extra) && kernel_drawer->Operator()->MaxArg() != (4+extra)) {
    sprintf(str, "%d", id);
    sprintf(str1, "%d",4+extra);
    Error(I2(BText("Invalid argument # for '")+kernel_drawer->Name()+"', should be "+str1+", at block "+str,
             BText("# de argumentos inválidos para '")+kernel_drawer->Name()+"', debe ser "+str1+", en bloque "+str));
    return;
  }
  int i;
  const char * arg[] = {"1", "2", "3", "4"};
  
  for (i = 0; i < 3; i++) {
    if (kernel_drawer->Operator()->GrammarForArg(i+1) != GraMatrix()) {
      sprintf(str, "%d", id);
      Error(I2(BText("Invalid type for argument ")+arg[i]+" in '"+kernel_drawer->Name()+"', should be Matrix, at block "+str,
               BText("Tipo inválido de argumento ")+arg[i]+" en '"+kernel_drawer->Name()+"', debe ser Matrix, en bloque "+str));
      return;
    }
  }
  if (kernel_drawer->Operator()->GrammarForArg(4) != GraSet()) {
    sprintf(str, "%d", id);
    Error(I2(BText("Invalid type for argument 4 in '")+kernel_drawer->Name()+"', should be Set, at block "+str,
          BText("Tipo inválido de argumento 4 en '")+kernel_drawer->Name()+"', debe ser Set, en bloque "+str));
    return;
  }
  if (has_ws)
    if (kernel_drawer->Operator()->GrammarForArg(5) != GraReal()) {
      sprintf(str, "%d", id);
      Error(I2(BText("Invalid type for argument 5 in '")+kernel_drawer->Name()+"', should be Real, at block "+str,
               BText("Tipo inválido de argumento 5 en '")+kernel_drawer->Name()+"', debe ser Real, en bloque "+str));
      return;
    }
  
  /* end "drawer" argument */
  
  if (drawer_type == direct) {
    target_density = NULL;
    is_symmetric = 1;
    kernel_density = NULL;
  } else {
    if (card < 7) {
      sprintf(str, "%d", id);
      Error(I2(BText("A minimun of 7 elements are required in a non-direct block ")+str,
               BText("Se necesitan como mínimo 7 elementos en bloque no 'direct'")+str));
      return;
    }
    
    /* extract and validate "target" argument == target_density */
    ptr_so = blk_def[MH_target];
    target_density = (ptr_so->Grammar() == GraCode())?&((UCode(ptr_so))->Contens()):NULL;
    if (!target_density || target_density->Operator()->Grammar() != GraReal()) {
      sprintf(str, "%d", id);
      Error(I2(BText("'target' element of a block definition must be a Code returning a Real, at block ")+str,
               BText("Elemento 'target' en la definición de un bloque debe ser Code que retorne un Real, en bloque ")+str));
      return;
    }
    if (target_density->Operator()->MinArg() != (4+extra) && target_density->Operator()->MaxArg() != (4+extra)) {
      sprintf(str, "%d", id);
      sprintf(str1, "%d",4+extra);
      Error(I2(BText("Invalid argument # for '")+target_density->Name()+"', should be "+str1+", at block "+str,
               BText("# de argumentos inválidos para '")+target_density->Name()+"', debe ser "+str1+", at block "+str));
      return;
    }
    for (i = 0; i < 3; i++) {
      if (target_density->Operator()->GrammarForArg(i+1) != GraMatrix()) {
        sprintf(str, "%d", id);
        Error(I2(BText("Invalid type for argument ")+arg[i]+" in '"+target_density->Name()+"', should be Matrix, at block "+str,
                 BText("Tipo inválido de argumento ")+arg[i]+" en '"+target_density->Name()+"', debe ser Matrix, en bloque "+str));
        return;
      }
    }
    if (target_density->Operator()->GrammarForArg(4) != GraSet()) {
      sprintf(str, "%d", id);
      Error(I2(BText("Invalid type for argument 4 in '")+target_density->Name()+"', should be Set, at block "+str,
               BText("Tipo inválido de argumento 4 en '")+target_density->Name()+"', debe ser Set, en bloque "+str));
      return;
    }
    if (has_ws)
      if (target_density->Operator()->GrammarForArg(5) != GraReal()) {
        sprintf(str, "%d", id);
        Error(I2(BText("Invalid type for argument 5 in '")+target_density->Name()+"', should be Real, at block "+str,
                 BText("Tipo inválido de argumento 5 en '")+target_density->Name()+"', debe ser Real, en bloque "+str));
        return;
      }
    /* end "target" argument */
    
    /* extract and validate "symmetric" argument */
    ptr_so = blk_def[MH_sym];
    if (ptr_so->Grammar() != GraReal()) {
      sprintf(str, "%d", id);
      Error(I2(BText("'symmetric' element of a block definition must be a Real, at block ")+str,
               BText("Elemento 'symmetric' en la definición de un bloque debe ser Real, en bloque ")+str));
      return;
    }
    ptr_dat = &((UDat(ptr_so))->Contens());
    is_symmetric = int(ptr_dat->Value());
    if (!ptr_dat->Known() || (is_symmetric != 0 && is_symmetric != 1)) {
      sprintf(str, "%d", id);
      Error(I2(BText("'symmetric' element of a block definition should be 0 or 1, at block ")+str,
               BText("Elemento 'symmetric' en la definición de un bloque debe ser 0 ó 1, en bloque ")+str));
      return;
    }
    /* end "symmetric" argument */
    
    if (is_symmetric)
      kernel_density = NULL;
    else {
      /* extract and validate "kernel" argument */
      ptr_so = blk_def[MH_kernel];
      kernel_density = (ptr_so->Grammar() == GraCode())?&((UCode(ptr_so))->Contens()):NULL;
      if (!kernel_density || kernel_density->Operator()->Grammar() != GraReal()) {
        sprintf(str, "%d", id);
        Error(I2(BText("'kernel' element of a block definition must be a Code returning a Real, at block ")+str,
                 BText("Elemento 'kernel' en la definición de un bloque debe ser Code que retorne un Real, en bloque ")+str));
        return;
      }
      if (kernel_density->Operator()->MinArg() != (5+extra) && kernel_density->Operator()->MaxArg() != (5+extra)) {
        sprintf(str, "%d", id);
        sprintf(str1, "%d",5+extra);
        Error(I2(BText("Invalid argument # for '")+kernel_density->Name()+"', should be "+str1+", at block "+str,
                 BText("# de argumentos inválidos para '")+kernel_density->Name()+"', debe ser "+str1+", at block "+str));
        return;
      }
      for (i = 0; i < 4; i++) {
        if (kernel_density->Operator()->GrammarForArg(i+1) != GraMatrix()) {
          sprintf(str, "%d", id);
          Error(I2(BText("Invalid type for argument ")+arg[i]+" in '"+kernel_density->Name()+"', should be Matrix, at block "+str,
                   BText("Tipo inválido de argumento ")+arg[i]+" en '"+kernel_density->Name()+"', debe ser Matrix, en bloque "+str));
          return;
        }
      }
      if (kernel_density->Operator()->GrammarForArg(5) != GraSet()) {
        sprintf(str, "%d", id);
        Error(I2(BText("Invalid type for argument 5 in '")+kernel_density->Name()+"', should be Set, at block "+str,
                 BText("Tipo inválido de argumento 5 en '")+kernel_density->Name()+"', debe ser Set, en bloque "+str));
        return;
      }
      if (has_ws)
        if (kernel_density->Operator()->GrammarForArg(6) != GraReal()) {
          sprintf(str, "%d", id);
          Error(I2(BText("Invalid type for argument 6 in '")+kernel_density->Name()+"', should be Real, at block "+str,
                   BText("Tipo inválido de argumento 6 en '")+kernel_density->Name()+"', debe ser Real, en bloque "+str));
          return;
        }
      /* end "kernel" argument */
    }
  }
  blk_status = 1;
  blk_id = id;
  blk_start = idx_start;
}

/*
 * MH_blk_sampler::draw_direct --
 * 
 */

int MH_blk_sampler::draw_direct(MH_workspace & workspace, BUserMat * & sample)
{
  BSyntaxObject * so_sample;
  BList * args;

  args = workspace.build_args1(data);
  
  /* do not destroy args because Evaluator does it */
  so_sample = kernel_drawer->Evaluator(args);
  if (!so_sample) {
    char str[64];
    /* and error ocurred in the user code */
    sprintf(str, "%d", blk_id);
    Error(I2(BText("Failed draw_direct evaluation for block ")+str,
             BText("Fallo en la evaluación de draw_direct para el bloque ")+str));
    return 0;
  }
  sample = UMat(so_sample);
  return 1;
}

/* MH_blk_sampler::draw_mh_rwalk --
 * 
 *   random walk kernel
 *   q(x,y) = q1(y-x)
 *   y = x + z
 *   z ~ q1
 */

int MH_blk_sampler::draw_mh_rwalk(MH_workspace & workspace,
                                  BUserMat * & sample)
{
  return 0;
}

/*
 * MH_blk_sampler::draw_mh_general --
 * 
 *   general MH.
 *   pi is the target density.
 *   q is the proposal density.
 */

int MH_blk_sampler::draw_mh_generic(MH_workspace & workspace,
                                    BUserMat * & sample)
{  
  BUserMat * ptr_umat_y;
  double log_pi_y, log_pi_x, log_q_yx, log_q_xy, log_alpha_xy;

  sample = NULL;
  /* generate y from q(x,.) and u from U(0,1) */
  ptr_umat_y = sample_from_kernel(workspace);
  if (!ptr_umat_y)
    return 0;
  /* lock ptr_umat_y in order to protect it from being released when
   * evaluating with ->Evaluator */
  ptr_umat_y->IncNRefs();
  /* log pi(x) could be precomputed in MH_workspace */
  if (workspace.logpi_is_precomputed(blk_id)) {
    log_pi_x = workspace.logpi(blk_id);
  } else {
    /* evaluate log pi(x) */
    if (!evaluate_target(workspace,log_pi_x)) {
      ptr_umat_y->DecNRefs();
      DESTROY(ptr_umat_y);
      return 0;
    }
    /* and store it into worspace */
    workspace.save_logpi(blk_id, log_pi_x);
  }
  /* evaluate log q(x,y), only if necessary */
  if (!is_symmetric && !evaluate_kernel(workspace, ptr_umat_y, log_q_xy)) {
    ptr_umat_y->DecNRefs();
    DESTROY(ptr_umat_y);
    return 0;    
  }
  /* set y as the current state */
  if (!workspace.change_state(ptr_umat_y, blk_start, blk_size)) {
    ptr_umat_y->DecNRefs();
    DESTROY(ptr_umat_y);
    return 0;
  }
  /* evaluate log pi(y) */
  if (!evaluate_target(workspace,log_pi_y)) {
    ptr_umat_y->DecNRefs();
    DESTROY(ptr_umat_y);
    return 0;
  }
  /* evaluate log q(y,x), only if necessary */
  if (!is_symmetric &&
      !evaluate_kernel(workspace, workspace.saved_state(), log_q_yx)) {
    ptr_umat_y->DecNRefs();
    DESTROY(ptr_umat_y);
    workspace.restore_state(blk_start, blk_size);
    return 0;
  }
  /* from now on is save to unlock ptr_umat_y */
  ptr_umat_y->DecNRefs();
  /* restore MH state x */
  workspace.restore_state(blk_start, blk_size);
  /* evaluate log alpha(x,y) */
  log_alpha_xy = log_pi_y - log_pi_x;
  /*printf("pi(y) = %g, pi(x) = %g, a(x,y) = %g\n",
    log_pi_y, log_pi_x, log_alpha_xy);*/
  
  if (!is_symmetric) {
    log_alpha_xy += log_q_yx - log_q_xy;
  }
  int result;
  
  /* here we can test with an eps */
  if (log_alpha_xy < 0) {
    /* generate a random number from U(0,1) */
    double log_u = log(gsl_rng_uniform_pos(getGslRng()));
    //printf("log(u) = %g\n", log_u);
    if (log_u > log_alpha_xy) {
      /* this DESTROY set ptr_umat_y to NULL */
      DESTROY(ptr_umat_y);
      assert(ptr_umat_y==NULL);
      result = -1;
    } else {
      workspace.save_logpi(blk_id, log_pi_y);
      result = 1;
    }
  } else {
    /* ptr_umat_y has being accepted, so save computed log_pi_y */
    workspace.save_logpi(blk_id, log_pi_y);
    result = 1;
  }
  sample = ptr_umat_y;
  return result;
}

/*
 *  MH_blk_sampler::sample_from_kernel --
 *
 *    Generate a sample from q(x,.)
 */

BUserMat *  MH_blk_sampler::sample_from_kernel(MH_workspace & workspace)
{
  BSyntaxObject * so_y;
  BList * args;

  args = workspace.build_args1(data);
  /* do not destroy args because Evaluator does it */
  so_y = kernel_drawer->Evaluator(args);
  if (!so_y) {
    char str[64];
    /* an error ocurred in the user code */
    sprintf(str, "%d", blk_id);
    Error(I2(BText("Failed kernel_drawer evaluation for block ")+str,
             BText("Fallo en la evaluación de kernel_drawer para el bloque ")+str));
    return NULL;
  }
  return UMat(so_y);
}

/*
 *  MH_blk_sampler::evaluate_target --
 *
 *    Evaluate the target density log pi(x) at the current state.
 */

int MH_blk_sampler::evaluate_target(MH_workspace & workspace, double & result)
{
  BSyntaxObject * so_dat;
  BDat * ptr_dat;
  BList * args;
  int ok;
  char str[64];

  args = workspace.build_args1(data);
  /* do not destroy args because Evaluator does it */
  so_dat = target_density->Evaluator(args);
  if (so_dat) {
    ptr_dat = &((UDat(so_dat))->Contens());
    if (ptr_dat->IsKnown()) {
      result = ptr_dat->Value();
      ok = 1;
    } else {
      sprintf(str, "%d", blk_id);
      Error(I2(BText("Unknown value in evaluation of 'target_density' for block ")+str,
               BText("Valor desconocido en evaluación de 'target_density' para el bloque ")+str));
      ok = 0;
    }
    DESTROY(so_dat);
  } else {
    /* an error ocurred in the user code */
    sprintf(str, "%d", blk_id);
    Error(I2(BText("Failed target_density evaluation for block ")+str,
             BText("Fallo en la evaluación de target_density para el bloque ")+str));
    ok = 0;
  }  
  return ok;
}

/*
 *  MH_blk_sampler::evaluate_kernel --
 *
 *    evaluate the kernel q(x,y) for the candidate sample y. The current
 *    state x is workspace.
 */

int MH_blk_sampler::evaluate_kernel(MH_workspace & workspace,
                                    BUserMat * y, double & result)
{
  BSyntaxObject * so_dat;
  BDat * ptr_dat;
  BList * args;
  int ok;
  char str[64];
  
  args = workspace.build_args1(data);
  args = Cons(y, args);  
  /* do not destroy args because Evaluator does it */
  so_dat = kernel_density->Evaluator(args);
  if (so_dat) {
    ptr_dat = &((UDat(so_dat))->Contens());
    if (ptr_dat->IsKnown()) {
      result = ptr_dat->Value();
      ok = 1;
    } else {
      sprintf(str, "%d", blk_id);
      Error(I2(BText("Unknown value in evaluation of 'kernel_density' for block ")+str,
               BText("Valor desconocido en evaluación de 'kernel_density' para el bloque ")+str));
      ok = 0;
    }
    DESTROY(so_dat);
  } else {
    /* an error ocurred in the user code */
    sprintf(str, "%d", blk_id);
    Error(I2(BText("Failed 'kernel_density' evaluation for block ")+str,
             BText("Fallo en la evaluación de 'kernel_density' para el bloque ")+str));
    ok = 0;
  }  
  return ok;
}

/**
 *  MH_monitor -- implement an external monitor to the Metropolis-Hastings. It
 *  is defined by a Set with the following structure:
 *
 *    type - type of monitor, could be: "csv", "ucode"
 *
 *    name - name of the monitor, its value depend on the type. If
 *           type is "csv" then this is the name of the file.
 *
 *    frequency - number of samples to monitor.
 *
 *    discard - 1 ==> discard the previous samples and start again.
 *              0 ==> do not discard samples.
 *
 *    ucode - is opcional and could provide an function to be
 *            invoked depending on frequency.
 */

class MH_monitor 
{
public:
  typedef enum {
    csv, ucode
  } montype;

  MH_monitor(BSet * mondef, BMatrix<BDat> *samples);
  ~MH_monitor() 
    {
      if (_name)
        delete []_name;
      if (_out) {
        fclose(_out);
      }
    }

  int good() 
    {
      return !error;
    }
  
  int should_discard() 
    {
      return _discard;
    }

  size_t frequency()
    {
      return _freq;
    }
  
  void on_error() 
    {
    }
  size_t new_sample()
    {
      size_t c;
      
      ++_received;
      if (error)
        return _received;
      
      if (_received == _freq) {
        supervise();
        if (!_discard) {
          _c0 += _freq;
        }
        c = _c0;
        _received = 0;
      } else {
        c = _c0 + _received;
      }
      return c;
    }

protected:
  void supervise();
  
private:
  FILE* _out;
  BMatrix<BDat> * _samples;
  montype _type;
  char * _name;
  size_t _freq;
  int _discard;
  int _c0;
  int _received;
  int error;
};

#define MH_MON_TYPE     1
#define MH_MON_NAME     2
#define MH_MON_FREQ     3
#define MH_MON_DISCARD  4
#define MH_MON_UCODE 5

#define MH_MON_RETERROR                                                 \
do {                                                                    \
  error = 1;                                                            \
  _discard = 0;                                                         \
  _name = NULL;                                                         \
  _samples = NULL;                                                      \
  _out = NULL;                                                          \
  return;                                                               \
} while(0)

MH_monitor::MH_monitor(BSet * mondef, BMatrix<BDat> *samples)
{
  BSyntaxObject * so;
  BText * ptr_text;
  BDat * ptr_dat;
  int i;

  _c0 = 0;
  _received = 0;

  if (!mondef) {
    MH_MON_RETERROR;
  }
  
  if (mondef->Card() < 4) {
    Warning(I2(BText("'MH Monitor' definition should be at least 4 element Set. Going on without 'MH Monitor'"),
            BText("La definición del 'MH Monitor' debe ser un Set de al menos 4 elementos. Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;
  }
  /* Monitor type */
  so = (*mondef)[MH_MON_TYPE];
  if (so->Grammar() != GraText()) {
    Warning(I2(BText("type of 'MH Monitor' should be a Text value. Going on with out 'MH Monitor'"),
            BText("El tipo del 'MH Monitor' debe ser un valor Text. Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;    
  }
  ptr_text = &((UText(so))->Contens());
  if (!strcasecmp(ptr_text->Buffer(), "csv")) {
    _type = csv;
  } else if (!strcasecmp(ptr_text->Buffer(), "ucode")) {
    _type = ucode;
  } else {
    Warning(I2(BText("type of 'MH Monitor' should be a \"csv\". Going on with out 'MH Monitor'"),
            BText("El tipo del 'MH Monitor' debe ser \"csv\". Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;    
  }
  
  /* Monitor name */
  so = (*mondef)[MH_MON_NAME];
  if (so->Grammar() != GraText()) {
    Warning(I2(BText("name of 'MH Monitor' should be a Text value. Going on with out 'MH Monitor'"),
            BText("El nombre del 'MH Monitor' debe ser un valor Text. Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;    
  }
  ptr_text = &((UText(so))->Contens());
  if (!ptr_text->Length()) {
    Warning(I2(BText("name of 'MH Monitor' should be a non empty Text value. Going on with out 'MH Monitor'"),
               BText("El nombre del 'MH Monitor' debe ser un valor Text no vacío. Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;
  }
  _name = new char[ptr_text->Length()+1];
  strcpy(_name, ptr_text->Buffer());

  /* Monitor frequency */
  so = (*mondef)[MH_MON_FREQ];
  if (so->Grammar() != GraReal()) {
    Warning(I2(BText("frequency of 'MH Monitor' should be a Real value. Going on with out 'MH Monitor'"),
            BText("La frecuencia del 'MH Monitor' debe ser un valor Real. Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;    
  }
  ptr_dat = &((UDat(so))->Contens());
  i = int(ptr_dat->Value());
  if (!ptr_dat->Known() || i <= 0) {
    Warning(I2(BText("frequency of 'MH Monitor' should be a positive Real value. Going on with out 'MH Monitor'"),
            BText("La frecuencia del 'MH Monitor' debe ser un valor Real positivo. Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;    
  }
  _freq = size_t(i);

  /* Monitor discard */
  so = (*mondef)[MH_MON_DISCARD];
  if (so->Grammar() != GraReal()) {
    Warning(I2(BText("field discard of 'MH Monitor' should be a Real value. Going on with out 'MH Monitor'"),
            BText("El campo discard del 'MH Monitor' debe ser un valor Real. Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;    
  }
  ptr_dat = &((UDat(so))->Contens());
  i = int(ptr_dat->Value());
  if (!ptr_dat->Known() || (i != 0 && i != 1)) {
    Warning(I2(BText("field discard of 'MH Monitor' should be 0 or 1. Going on with out 'MH Monitor'"),
            BText("El campo discard del 'MH Monitor' debe ser 0 ó 1. Continuando sin 'MH Monitor'")));
    MH_MON_RETERROR;    
  }
  _discard = i;
  if (_type == csv) {
    _out = fopen(_name, "a");
    if (!_out) {
      Warning(I2(BText("could not open file") + _name + ". Going on with out 'MH Monitor'",
                 BText("no se pudo abrir el archivo") + _name + ". Continuando sin 'MH Monitor'"));
      MH_MON_RETERROR;    
    }
  } else
    _out = NULL;
  _samples = samples;
  
  error = 0;
}

void MH_monitor::supervise()
{
  if (error) return;
  if (_type == csv) {
    int i, r, c;

    for (i = 0, c = _c0; i < _received; i++, ++c) {
      for (r = 0; r < _samples->Rows(); r++)
        fprintf(_out, "%g,", (*_samples)(r,c).Value());
      fprintf(_out, "\n");
    }
  }
}

DeclareContensClass(BMat, BMatTemporary, BMatMetropolisHastings);
DefExtOpr(1, BMatMetropolisHastings, "MetropolisHastings",3, 5, "Set Real Real Set Set",
  "(Set BlockDef, Real Burning, Real Size [,Set Workspace, Set Monitor])",
  I2("Metropolis-Hastings algorithm to simulate a multivariate distribution. Use a 'block-at-a-time' algorithm.\n\n"
  "Arguments:\n\n"
  "   Burning - number of samples to discard initially\n"
  "      Size - samples size to generate. The result is a Matriz with Size columns and the same number of rows as parameters are simulated\n"
  "  BlockDef - Set defining each block of the join parameter vector. One block is defined in turn by a Set.\n\n"
  "  A block definition has the following structure:\n\n"
  "    [1] - size of the block, must be > 0\n"
  "    [2] - type of sampling to be used for this block, could be: 0 -> direct sampler, 1 -> random walk, 2 -> generic MH.\n"
  "    [3] - column vector as initial state for this block\n"
  "    [4] - block's specific data given as a Set\n"
  "    [5] - Matrix SamplerCode(Matrix X, Matrix CX, Matrix Join, Set blk_data [, Real ID_workspace])\n"
  "          Sampler for this block. If [2] is 0 this code is the full conditional sampler.\n"
  "          In other case this is the kernel sampler that generates candidates for the MH loop.\n"
  "          The arguments are the following:\n"
  "                 X - is the current vector value for this block\n"
  "                CX - is the join vector but this block\n"
  "              Join - is the join vector of parameters\n"
  "          blk_data - is the block's specific data\n"
  "          ID_workspace - identifier to be used in MHSetWorkspace or MHGetWorkspace as needed\n\n"
  "    If [2] is not 0 then we should have the following extra arguments:\n\n"
  "      [6] - Matrix TargetDenCode(Matrix X, Matrix CX, Matrix join, Set blk_data [, Real ID_workspace])\n"
  "            Evaluate log pi(x) -- the log of the target density\n"
  "      [7] - Real value indicating if the kernel q(x,y) is symmetric,\n"
  "            could be 0 -> non-symmetric or 1 -> symmetric\n\n"
  "    If [7] is 0 then one more argument is required:\n\n"
  "      [8] - Real KernelDenCode(Matrix Y, Matrix X, Matrix CX, Matrix join, Set blk_data [, Real ID_workspace])\n"
  "            Evaluate log q(x,y) -- the log of the kernel density evaluator\n\n"
  "  Workspace - is a Set that will be handled by the user Code in each block.\n"
  "              Each user Code invoked by the MH will receive a workspace identifier\n"
  "              that can be used to modify this Set. The workspace identifier is the\n"
  "              argument ID_workspace of the user Code.\n"
  "  Monitor - Define a monitor of the MH's chain. It is a set with at least 4 elements:\n"
  "            [1] - type of the Monitor, it is a Text with the possible values: \"csv\".\n"
  "            [2] - name of the monitor. If type is \"csv\" it's a file name where to write the chain.\n"
  "            [3] - frecuency of updating the monitor. It is an integer value.\n"
  "            [4] - discard previous set of sample. It is 0 or 1. 0 means that the result matrix.\n"
  "                  will contain the whole chain. 1 means that the number of samples in the result matrix will be the value of 'frequency' instead of 'size'.\n\n"   
  "Example:\n\n",
    "Algoritmo Metropolis-Hastings para simular una distribución multivariada. Usa un algoritmo 'block-at-a-time'\n\n"
    "Argumentos:\n\n"
    "   Burning - número de muestras a descartar inicialmente\n"
    "      Size - número de muestras a generar. El resultado es una matriz con Size columnas y tantas filas como parámetros se simulen.\n"
    "  BlockDef - Conjunto que define cada uno de los bloques en los que se particiona el vector de parametros conjunto.\n\n"
    "  La definción de un bloque es a su vez un Set con la siguiente structura:\n\n"
    "    [1] - tamaño del bloque, debe ser > 0\n"
    "    [2] - tipo de muestreo a utilizar en el bloque, debe ser: 0 -> sampler directo, 1 -> 'random walk', 2 -> MH genérico.\n"
    "    [3] - estado inicial del bloque en forma de vector columna\n"
    "    [4] - datos específicos del bloque\n"
    "    [5] - Matrix SamplerCode(Matrix X, Matrix CX, Matrix join, Set blk_data [, Real ID_workspace])\n"
    "          Generador para este bloque. Si [2] es 0 entonces este código es el generador para la densidad condicional completa del bloque.\n"
    "          En cualquier otro caso es el código que genera candidatos a ser considerados en el test MH de este bloque.\n"
    "          Tiene los siguientes argumentos:\n"
    "                 X - es el valor actual de los paramétros para este bloque\n"
    "                CX - es el vector conjunto excepto este bloque\n\n"
    "              join - es el vector conjunto de parámetros\n"
    "          blk_data - son datos espcíficos del bloque\n\n"
  "          ID_workspace - identificador a ser usado en MHSetWorkspace o MHGetWorkspace según se necesite\n\n"
    "    Si [2] no es 0 entonces necesitamos 2 argumentas extras:\n\n"
    "      [6] - Matrix TargetDenCode(Matrix X, Matrix CX, Matrix join, Set blk_data [, Real ID_workspace])\n"
    "            Evalúa log pi(x) -- el log de la densidad objetivo\n"
    "      [7] - valor Real que indica si el kernel q(x,y) is simétrico,\n"
    "            puede ser 0 -> no simétrico or 1 -> simétrico\n\n"
    "    Si [7] es 0 entonces necesitamos un argumento más:\n\n"
    "      [8] - Real KernelDenCode(Matrix Y, Matrix X, Matrix CX, Matrix join, Set blk_data [, Real ID_workspace])\n"
    "            Evalúa log q(x,y) -- el log de la densidad generadora de candidatos\n\n"
  "  Workspace - Es un Set que sera manipulado por el Code  de usuario en cada bloque.\n"
  "              Cada Code de usuario invocado por el MH recibirá un identificador de workspace\n"
  "              que puede utilizarse para modificar este Set. El identificador de workspace es\n"
  "              el argumento ID_workspace del Code de usuario.\n"
  "  Monitor - Define a monitor of the MH's chain. It is a set with at least 4 elements:\n"
  "            [1] - tipo de Monitor, es un texto que puede tomar los valores: \"csv\".\n"
  "            [2] - nombre del monitor. Si el tipo es \"csv\", es el nombre de un archivo donde se escribirá la cadena.\n"
  "            [3] - frecuencia de actualizació del monitor. Es un valor entero que indica cada cuantas muestras se actualiza el Monitor.\n"
  "            [4] - discartar bloque de muestras previo. Puede tomar los valores 0 ó 1. 0 indica que la matriz resultante contendrá toda la muestra.\n"
  "                  1 significa que la matriz resultante contendrá un número de muestras igual al valor de 'frequency' en lugar de 'size'.\n\n"   
    "Ejemplo:\n\n"),
  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BMatMetropolisHastings::CalcContens()
//--------------------------------------------------------------------
{
  BSet *block_defs, *ws_def, *monitor_def;
  BSyntaxObject * so_block, *so;
  char str[256];
  
  int burning, size;
  int card;
  size_t blk_start;
  
  block_defs = &((USet(Arg(1)))->Contens());
  card =  block_defs->Card();
  /* workspace definition */
  so = Arg(4);
  if (so)
    ws_def = &Set(so);
  else
    ws_def = NULL;
  if (card > 0) {
    MH_blk_sampler * mh_blocks = new MH_blk_sampler[card];
    size_t i;
    blk_start = 0;
    
    for (i = 0; i < card; i++) {
      so_block = (*block_defs)[i+1];
      if (so_block->Grammar() != GraSet()) {
        sprintf(str, "%d", i);
        Error(I2(BText("Block definition must be a Set, at block ")+str,
                 BText("La definición de un bloque debe ser un Set, en bloque ")+str));
        return;
      }
      mh_blocks[i] = MH_blk_sampler(i, blk_start,
                                    (USet(so_block))->Contens(), ws_def?ws_def->Card():0);
      if (!mh_blocks[i].status()) {
        sprintf(str, "%d", i);
        Error(I2(BText("Could no initialize block ")+str,
                 BText("Fallo en la inicialización del bloque ")+str));
        return;
      }
      
      blk_start += mh_blocks[i].size();
    }
    /* blk_start contains the dimension of the distribution and i the number of
     * blocks */
    MH_workspace ws(blk_start, i, ws_def);
    /* set initial state */
    int j;
    for (j = 0; j < card; j++) {
      int init_st = ws.copy_params(mh_blocks[j].initial_state(),
                                   mh_blocks[j].idx_start(),
                                   mh_blocks[j].size());
      assert(init_st);
    }
    BDat * ptr_dat = &((UDat(Arg(2)))->Contens());
    burning = int(ptr_dat->Value());
    if (!ptr_dat->Known() || burning <= 0) {
      Warning(I2("In MetropolisHastings, Burning parameter should be > 0, assuming 100",
                 "En MetropolisHastings, el parámetro Burning debe ser > 0, asumiendo 100"));
      burning = 100;
    }
    ptr_dat = &((UDat(Arg(3)))->Contens());
    size = int(ptr_dat->Value());
    if (!ptr_dat->Known() || size <= 0) {
      Warning(I2("In MetropolisHastings, Size parameter should be > 0, assuming 100",
                 "En MetropolisHastings, el parámetro Size debe ser > 0, asumiendo 100"));
      size = 100;
    }
    int error;

    //printf("Ejecutando un burning de tamaño %d\n", burning);
    
    /* burning period */
    for (i = 0; i < size_t(burning); i++) {
      error = 0;
      for (j = 0; j < card; j++)
        if (!mh_blocks[j].draw(ws)) {
          error = 1;
          break;
        }
      if (error) break;
    }
    /* mostrar acceptacion despues del burning */
    /*
    Std("After burning period :");
    for (j = 0; j < card; j++) {
      sprintf(str, "Block %d had %d samples accepted",
              j, mh_blocks[j].get_count_accepted()); 
      Std(str);
      mh_blocks[j].reset_count_accepted();
    }
    */
    //printf("Finalizado el burning con %s\n", error?"fallo":"éxito");    
    if (!error) {
      so = Arg(5);
      if (so)
        monitor_def = &Set(so);
      else
        monitor_def = NULL;
      MH_monitor monitor(monitor_def, &contens_);
      
      contens_.Alloc(ws.dimension(),
                     monitor.should_discard()?monitor.frequency():size);
      /* number of samples accepted */
      size_t number_of_accepted = 0;
      /* flag indicating if last sampled has being rejected */
      int is_accepted;
      int draw_status;
      size_t c;
      
      /* sampling period */
      c = 0;
      for (i = 0; i < size_t(size); i++) {
        error = 0;
        is_accepted = 0;
        for (j = 0; j < card; j++) {
          if (!(draw_status=mh_blocks[j].draw(ws))) {
            error = 1;
            break;
          }
          if (draw_status==1)
            is_accepted = 1;
        }
        if (error) {
          monitor.on_error();
          break;
        }
        /* copy last sample */
        for (int r = 0; r < contens_.Rows(); r++)
          contens_(r,c) = (*ws.mat_all_params())(r,0);
        c = monitor.new_sample();
        if (is_accepted)
          ++number_of_accepted;
      }
      /*
      Std("After sampling period :");
      for (j = 0; j < card; j++) {
        sprintf(str, "Block %d had %d samples accepted",
                j, mh_blocks[j].get_count_accepted()); 
        Std(str);
      }
      */
      /*
      sprintf(str, "%d", number_of_accepted);
      Std(BText("In Metropolis-Hastings ") + str + " samples where accepted\n");
      */
    }
  } else {
    Error(I2("In MetropolisHastings, BlockDef could not be empty",
             "En MetropolisHastings, BlockDef no puede ser vacío"));
  }
}
