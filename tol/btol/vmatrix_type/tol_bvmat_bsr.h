/* tol_bvmat_bsr_parser.h: BSR: Restricted Linear Regression using 
                                 virtual matrices.
                  GNU/TOL Language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BVMAT_RLR_H
#define TOL_BVMAT_RLR_H 1

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_btext.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bvmat.h>

#include <string>
#include <vector>

namespace BysSparseReg {

///////////////////////////////////////////////////////////////////////////////
inline const BText& url_parse_bsr()
///////////////////////////////////////////////////////////////////////////////
{
  static BText aux =
  I2("You can see an example of BSR syntax at ", 
     "Puede ver un ejemplo de la sintaxis de BSR en ")+
  "http://cvs.tol-project.org/"
  "viewcvs.cgi/*checkout*/tol/stdlib/math/stat/models/bayesian/bysMcmc"
  "/test_01/parse.bsr";
  return(aux);
};

///////////////////////////////////////////////////////////////////////////////
struct doc_info
///////////////////////////////////////////////////////////////////////////////
{
  string model_name;
  string model_description;
  string session_name;
  string session_description;
  string session_authors;
};

///////////////////////////////////////////////////////////////////////////////
struct variable_info
///////////////////////////////////////////////////////////////////////////////
{
  std::string name;
  int         index;
  double      initValue;
  bool        used;

  variable_info()
  : name     (""),
    index    (0),
    initValue(BDat::Nan()),
    used     (false)
  {}
  variable_info(const variable_info& aux)
  : name     (""),
    index    (0),
    initValue(BDat::Nan()),
    used     (false)
  {
    copy(aux);
  }
  const variable_info& operator=(const variable_info& aux)
  {
    copy(aux);
    return(*this);
  }
  void copy(const variable_info& aux)
  {
    name      = aux.name;
    index     = aux.index;
    initValue = aux.initValue;
    used      = aux.used;
  }

  void show() const
  {
    Std(BText("\nvariable_info(")+
      "\n  name="+name.c_str()+", "+
      "\n  index="+index+", "+
      "\n  initValue="+initValue+", "+
      "\n  used="+(int)used+")");
  }
};

///////////////////////////////////////////////////////////////////////////////
struct missing_info
///////////////////////////////////////////////////////////////////////////////
{
  std::string name;
  int         index;
  int         row;
  int         col;
  std::string prior; 
  double      nu;
  double      sigma2;
  double      minBound;
  double      maxBound;
  bool        used;

  missing_info()
  : name     (""),
    index    (0),
    row      (0),
    col      (0),
    prior    (""),
    nu       (BDat::Nan()),
    sigma2   (BDat::Nan()),
    minBound (BDat::Nan()),
    maxBound (BDat::Nan()),
    used     (false)
  {}
  missing_info(const missing_info& aux)
  : name     (""),
    index    (0),
    row      (0),
    col      (0),
    prior    (""),
    nu       (BDat::Nan()),
    sigma2   (BDat::Nan()),
    minBound (BDat::Nan()),
    maxBound (BDat::Nan()),
    used     (false)
  {
    copy(aux);
  }
  const missing_info& operator=(const missing_info& aux)
  {
    copy(aux);
    return(*this);
  }
  void copy(const missing_info& aux)
  {
    name      = aux.name;
    index     = aux.index;
    row       = aux.row;
    col       = aux.col;
    prior     = aux.prior;
    nu        = aux.nu;
    sigma2    = aux.sigma2;
    minBound  = aux.minBound;
    maxBound  = aux.maxBound;
    used      = aux.used;
  }

  void show() const
  {
    Std(BText("\nmissing_info(")+
      "\n  name="+name.c_str()+", "+
      "\n  index="+index+", "+
      "\n  row="+row+", "+
      "\n  col="+col+", "+
      "\n  prior="+(int)prior.c_str()+", "+
      "\n  nu="+nu+", "+
      "\n  sigma2="+sigma2+", "+
      "\n  minBound="+minBound+", "+
      "\n  maxBound="+maxBound+", "+
      "\n  used="+(int)used+")");
  }
};

///////////////////////////////////////////////////////////////////////////////
struct sigma_info
///////////////////////////////////////////////////////////////////////////////
{
  std::string name;
  int         index;
  int         nzmax;

  sigma_info()
  : name (""),
    index(0),
    nzmax(0)
  {}
  sigma_info(const sigma_info& aux)
  : name (""),
    index(0),
    nzmax(0)
  {
    copy(aux);
  }
  const sigma_info& operator=(const sigma_info& aux)
  {
    copy(aux);
    return(*this);
  }
  void copy(const sigma_info& aux)
  {
    name  = aux.name;
    index = aux.index;
    nzmax = aux.nzmax;
  }

  void show() const
  {
    Std(BText("\nsigma_info(")+
      "\n  name="+name.c_str()+", "+
      "\n  index="+index+", "+
      "\n  nzmax="+nzmax+")");
  }
};


///////////////////////////////////////////////////////////////////////////////
struct noise_info
///////////////////////////////////////////////////////////////////////////////
{
  std::string      name;
  int              index;
  int              size;
  double           nu;
  std::string      sigmaName;
  int              sigmaIdx;
  double           sigmaCte;
  std::string      arimaExpr;
  std::string      covExpr; 
  bool             covIsDiag;
  BVMat            cov;
  BVMat            L;
  BVMat            Li;
  std::vector<int> equIdx;
  bool             used;
  std::string      dating;
  std::string      firstDate;
  std::string      lastDate;

  noise_info()
  : name      (""),
    index     (0),
    size      (0),
    nu        (0),
    sigmaName (""),
    sigmaIdx  (-1),
    sigmaCte  (BDat::Nan()),
    arimaExpr (""),
    covExpr   (""),
    covIsDiag (true),
    cov       (),
    L         (),
    Li        (),
    equIdx    (),
    used      (false),
    dating    (""),
    firstDate (""),
    lastDate  ("")
  {}
  noise_info(const noise_info& aux)
  : name      (""),
    index     (0),
    size      (0),
    nu        (0),
    sigmaName (""),
    sigmaIdx  (-1),
    sigmaCte  (BDat::Nan()),
    arimaExpr (""),
    covExpr   (""),
    covIsDiag (true),
    cov       (),
    L         (),
    Li        (),
    equIdx    (),
    used      (false),
    dating    (""),
    firstDate (""),
    lastDate  ("")
  {
    copy(aux);
  }
  const noise_info& operator=(const noise_info& aux)
  {
    copy(aux);
    return(*this);
  }
  void copy(const noise_info& aux)
  {
    name       = aux.name;
    index      = aux.index;
    size       = aux.size;
    nu         = aux.nu;
    sigmaName  = aux.sigmaName;
    sigmaIdx   = aux.sigmaIdx;
    sigmaCte   = aux.sigmaCte;
    arimaExpr  = aux.arimaExpr;
    covExpr    = aux.covExpr;
    covIsDiag  = aux.covIsDiag;
    cov        = aux.cov;
    L          = aux.L;
    Li         = aux.Li;
    equIdx     = aux.equIdx;
    used       = aux.used;
    dating     = aux.dating;
    firstDate  = aux.firstDate;
    lastDate   = aux.lastDate;
  }

  void show() const
  {
    Std(BText("\nnoise_info(")+
      "\n  name="+name.c_str()+", "+
      "\n  index="+index+", "+
      "\n  size="+size+", "+
      "\n  nu="+nu+", "+
      "\n  sigmaName="+sigmaName.c_str()+", "+
      "\n  sigmaIdx="+sigmaIdx+", "+
      "\n  sigmaCte="+sigmaCte+", "+
      "\n  arimaExpr="+arimaExpr.c_str()+", "+
      "\n  covExpr="+covExpr.c_str()+", "+
      "\n  used="+(int)used+", "+
      "\n  dating="+dating.c_str()+", "+
      "\n  firstDate="+firstDate.c_str()+", "+
      "\n  lastDate="+lastDate.c_str()+")");
  }
};

////////////////////////////////////////////////////////////////////////////////
//Forward types declaring are defined in tol_bvmat_impl.h
////////////////////////////////////////////////////////////////////////////////
  int Initialize();
  BStruct* ModelDefStr();
  BStruct* DocInfoStr();
  BStruct* LinearBlockStr();
  BStruct* MissingBlockStr();
  BStruct* NoiseDistribStr();
  BStruct* NoiseTimeInfo();

  int Parse(const string &          fileName,
            doc_info&               docInfo,
            vector<variable_info>&  linearInfo,
            vector<missing_info>&   inputMissingInfo,
            vector<missing_info>&   outputMissingInfo,
            vector<noise_info>&     noiseInfo,
            BVMat&                  Y, 
            BVMat&                  X,
            BVMat&                  a, 
            BVMat&                  A);

};

#endif // TOL_BVMAT_RLR_H
