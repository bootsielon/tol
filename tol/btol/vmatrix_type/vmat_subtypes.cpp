/* vmat_subtypes.cpp: BVMat: Identification of subtypes and related methods
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bvmat_impl.h>


////////////////////////////////////////////////////////////////////////////////
// Static Types
////////////////////////////////////////////////////////////////////////////////
typedef hash_map_by_name <BVMat::ECode      >::dense_ BHashCodeName;
typedef hash_map_by_name <BVMat::ECholSolSys>::dense_ BHashCSysName;
typedef hash_map_by_name <BVMat::ECholFacOri>::dense_ BHashCFacName;
typedef hash_map_by_int  <BVMat::StrDefCode*>::dense_ BHashDefCode;
typedef hash_map_by_int  <BVMat::StrConvert*>::dense_ BHashConvert;
typedef hash_map_by_int  <BVMat::StrProduct*>::dense_ BHashProduct;
typedef hash_map_by_int  <BVMat::StrCholSol*>::dense_ BHashCholSol;
typedef hash_map_by_int  <BVMat::StrCholFac*>::dense_ BHashCholFac;
       
////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////
static BHashCodeName* hashCodeName_ = NULL;
static BHashCSysName* hashCSysName_ = NULL;
static BHashCFacName* hashCFacName_ = NULL;
static BHashDefCode*  hashDefCode_  = NULL;
static BHashConvert*  hashConvert_  = NULL;
static BHashProduct*  hashProduct_  = NULL;
static BHashCholSol*  hashCholSol_  = NULL;
static BHashCholFac*  hashCholFac_  = NULL;

static bool InitialiceHashes()
{
  static bool done_ = false;
  if(!done_)
  {
    done_ = true;
    hashCodeName_ = new BHashCodeName;
    hashCSysName_ = new BHashCSysName;
    hashCFacName_ = new BHashCFacName;
    hashDefCode_  = new BHashDefCode;
    hashConvert_  = new BHashConvert;
    hashProduct_  = new BHashProduct;
    hashCholSol_  = new BHashCholSol;
    hashCholFac_  = new BHashCholFac;
  }
  return(done_);
}

BHashCodeName& HashCodeName() 
{ InitialiceHashes();  return(*hashCodeName_); }
BHashCSysName& HashCSysName() 
{ InitialiceHashes(); return(*hashCSysName_); }
BHashCFacName& HashCFacName() 
{ InitialiceHashes(); return(*hashCFacName_); }
BHashDefCode&  HashDefCode() 
{ InitialiceHashes(); return(*hashDefCode_ ); }
BHashConvert&  HashConvert()  
{ InitialiceHashes(); return(*hashConvert_ ); }
BHashProduct&  HashProduct()
{ InitialiceHashes(); return(*hashProduct_ ); }
BHashCholSol&  HashCholSol()
{ InitialiceHashes(); return(*hashCholSol_ ); }
BHashCholFac&  HashCholFac()
{ InitialiceHashes(); return(*hashCholFac_ ); }

////////////////////////////////////////////////////////////////////////////////
// BVMat members
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
int BVMat::defCodeId(EEngineType engine, ECellType cell, EStoreType store)
////////////////////////////////////////////////////////////////////////////////
{
  static int baseEngine_ = 256;
  static int baseCell_   = 16*baseEngine_;
  return(              (1+(int)engine)
         + baseEngine_*(1+(int)cell)
         + baseCell_  *(1+(int)store));
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::codePairId(ECode left, ECode right)
////////////////////////////////////////////////////////////////////////////////
{
  static int base_ = 256*256;
  return(        (1+(int)left)
         + base_*(1+(int)right));
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::cholSolId(ECode left, ECode right, ECholSolSys sys)
////////////////////////////////////////////////////////////////////////////////
{
  static int base_ = 256;
  return((int)sys + base_*codePairId(left, right));
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::cholFacId(ECode code, ECholFacOri ori)
////////////////////////////////////////////////////////////////////////////////
{
  static int base_ = 16;
  return((int)ori + base_*(1+(int)code));
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::defCodeAdd(StrDefCode& s)
////////////////////////////////////////////////////////////////////////////////
{
  s.id = defCodeId(s.engine, s.cell, s.store);
  HashDefCode()[s.id] = &s;
  return(s.id);
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::convertAdd(StrConvert& s)
////////////////////////////////////////////////////////////////////////////////
{
  s.id = codePairId(s.left, s.right);
  HashConvert()[s.id] = &s;
  return(s.id);
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::productAdd(StrProduct& s)
////////////////////////////////////////////////////////////////////////////////
{
  s.id = codePairId(s.left, s.right);
  HashProduct()[s.id] = &s;
  return(s.id);
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::cholSolAdd(StrCholSol& s)
////////////////////////////////////////////////////////////////////////////////
{
  s.id = cholSolId(s.left, s.right, s.sys);
  HashCholSol()[s.id] = &s;
  return(s.id);
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::cholFacAdd(StrCholFac& s)
////////////////////////////////////////////////////////////////////////////////
{
  s.id = cholFacId(s.code, s.ori);
  HashCholFac()[s.id] = &s;
  return(s.id);
};

////////////////////////////////////////////////////////////////////////////////
int BVMat::initializeIdentifiers()
////////////////////////////////////////////////////////////////////////////////
{
  static bool done_ = 0;
  if(!done_)
  {

    int i;
    done_ = true;
    InitialiceHashes();
/*
#define CHOLMOD_OK            ( 0) // success 
#define CHOLMOD_NOT_INSTALLED (-1) // failure: method not installed 
#define CHOLMOD_OUT_OF_MEMORY (-2) // failure: out of memory 
#define CHOLMOD_TOO_LARGE     (-3) // failure: integer overflow occured 
#define CHOLMOD_INVALID       (-4) // failure: invalid input 
#define CHOLMOD_NOT_POSDEF    ( 1) // warning: matrix not pos. def. 
#define CHOLMOD_DSMALL        ( 2) // warning: D for LDL’ or diag(L) or LL’ has tiny absolute value 
*/
    assert(engineTypeName_numOpt==2);
    engineTypeName_[EET_Blas   ] = new BText("Blas");
    engineTypeName_[EET_Cholmod] = new BText("Cholmod");

    assert(cellTypeName_numOpt==2);
    cellTypeName_[ECT_Real   ] = new BText("Real");
    cellTypeName_[ECT_Complex] = new BText("Complex");

    assert(storeTypeName_numOpt==4);
    storeTypeName_[EST_Dense  ] = new BText("Dense");
    storeTypeName_[EST_Sparse ] = new BText("Sparse");
    storeTypeName_[EST_Factor ] = new BText("Factor");
    storeTypeName_[EST_Triplet] = new BText("Triplet");

    assert(codeName_numOpt==5);
    codeName_[ESC_undefined   ] = new BText("Undefined");
    codeName_[ESC_blasRdense  ] = new BText("Blas.R.Dense");
    codeName_[ESC_chlmRsparse ] = new BText("Cholmod.R.Sparse");
    codeName_[ESC_chlmRfactor ] = new BText("Cholmod.R.Factor");
    codeName_[ESC_chlmRtriplet] = new BText("Cholmod.R.Triplet");

    assert(cSysName_numOpt==9);
    cSysName_[ECSS_none  ] = new BText("None");
    cSysName_[ECSS_PtLLtP] = new BText("PtLLtP");
    cSysName_[ECSS_LtP   ] = new BText("LtP");
    cSysName_[ECSS_PtL   ] = new BText("PtL");
    cSysName_[ECSS_LLt   ] = new BText("LLt");
    cSysName_[ECSS_Lt    ] = new BText("Lt");
    cSysName_[ECSS_L     ] = new BText("L");
    cSysName_[ECSS_Pt    ] = new BText("Pt");
    cSysName_[ECSS_P     ] = new BText("P");

    assert(cFacName_numOpt==4);
    cFacName_[ECFO_none] = new BText("None");
    cFacName_[ECFO_X   ] = new BText("X");
    cFacName_[ECFO_XtX ] = new BText("XtX");
    cFacName_[ECFO_XXt ] = new BText("XXt");


    BHashCodeName& hashCodeName = *hashCodeName_;
    BHashCSysName& hashCSysName = *hashCSysName_;
    BHashCFacName& hashCFacName = *hashCFacName_;
    BHashDefCode&  hashDefCode  = *hashDefCode_;
    BHashConvert&  hashConvert  = *hashConvert_;
    BHashProduct&  hashProduct  = *hashProduct_;
    BHashCholSol&  hashCholSol  = *hashCholSol_;
    BHashCholFac&  hashCholFac  = *hashCholFac_;

    SetEmptyKey  (hashCodeName, NULL);
    SetEmptyKey  (hashCSysName, NULL);
    SetEmptyKey  (hashCFacName, NULL);
    SetEmptyKey  (hashDefCode,  0);
    SetEmptyKey  (hashConvert,  0);
    SetEmptyKey  (hashProduct,  0);
    SetEmptyKey  (hashCholSol,  0);
    SetEmptyKey  (hashCholFac,  0);

    assert(defCode_numOpt==5);
    defCode_[1] = StrDefCode
      (0, ESC_blasRdense,   EET_Blas,    ECT_Real, EST_Dense);
    defCode_[2] = StrDefCode
      (0, ESC_chlmRsparse,  EET_Cholmod, ECT_Real, EST_Sparse);
    defCode_[3] = StrDefCode
      (0, ESC_chlmRfactor,  EET_Cholmod, ECT_Real, EST_Factor);
    defCode_[4] = StrDefCode
      (0, ESC_chlmRtriplet, EET_Cholmod, ECT_Real, EST_Triplet);
    
    for(i=1; i<defCode_numOpt; i++)
    {
      ECode c = defCode_[i].code;
      HashCodeName()[ CodeName(c) ] = c;
      defCodeAdd(defCode_[i]);
    }
    
    assert(convert_numOpt==8);
    convert_[1] = StrConvert
      (0, ESC_chlmRsparse,  ESC_blasRdense,   bRd2cRs);
    convert_[2] = StrConvert
      (0, ESC_chlmRtriplet, ESC_blasRdense,   bRd2cRt);
    convert_[3] = StrConvert
      (0, ESC_chlmRsparse,  ESC_chlmRfactor,  cRf2cRs);
    convert_[4] = StrConvert
      (0, ESC_chlmRsparse,  ESC_chlmRtriplet, cRt2cRs);
    convert_[5] = StrConvert
      (0, ESC_blasRdense,   ESC_chlmRsparse,  cRs2bRd);
    convert_[6] = StrConvert
      (0, ESC_blasRdense,   ESC_chlmRtriplet, cRt2bRd);
    convert_[7] = StrConvert
      (0, ESC_chlmRtriplet, ESC_chlmRsparse,  cRs2cRt);
    
    for(i=1; i<convert_numOpt; i++) { convertAdd(convert_[i]); }

    assert(product_numOpt==5);
    product_[1] = StrProduct
      (0, ESC_blasRdense,   ESC_blasRdense,   bRd_bRd_prod);
    product_[2] = StrProduct
      (0, ESC_blasRdense,   ESC_chlmRsparse,  bRd_cRs_prod);
    product_[3] = StrProduct
      (0, ESC_chlmRsparse,  ESC_blasRdense,   cRs_bRd_prod);
    product_[4] = StrProduct
      (0, ESC_chlmRsparse,  ESC_chlmRsparse,  cRs_cRs_prod);
    
    for(i=1; i<product_numOpt; i++) { productAdd(product_[i]); }

    assert(cholSol_numOpt==25);
    cholSol_[1] = StrCholSol
      (0, ESC_blasRdense,   ESC_blasRdense,  ECSS_PtLLtP, bRd_bRd_cholSolLLt);
    cholSol_[2] = StrCholSol
      (0, ESC_blasRdense,   ESC_blasRdense,  ECSS_LLt,    bRd_bRd_cholSolLLt);
    cholSol_[3] = StrCholSol
      (01, ESC_blasRdense,   ESC_blasRdense, ECSS_LtP,    bRd_bRd_cholSolLt);
    cholSol_[4] = StrCholSol
      (0, ESC_blasRdense,   ESC_blasRdense,  ECSS_PtL,    bRd_bRd_cholSolL);
    cholSol_[5] = StrCholSol
      (01, ESC_blasRdense,   ESC_blasRdense, ECSS_Lt,     bRd_bRd_cholSolLt);
    cholSol_[6] = StrCholSol
      (0, ESC_blasRdense,   ESC_blasRdense,  ECSS_L,      bRd_bRd_cholSolL);
    cholSol_[7] = StrCholSol
      (01, ESC_blasRdense,  ESC_blasRdense,  ECSS_Pt,     bRd_bRd_cholSolId);
    cholSol_[8] = StrCholSol
      (0, ESC_blasRdense,   ESC_blasRdense,  ECSS_P,      bRd_bRd_cholSolId);
    cholSol_[9] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_blasRdense,  ECSS_PtLLtP, cRs_bRd_cholSolPtLLtP);
    cholSol_[10] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_blasRdense,  ECSS_LLt,    cRs_bRd_cholSolLLt);
    cholSol_[11] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_blasRdense,  ECSS_LtP,    cRs_bRd_cholSolLtP);
    cholSol_[12]= StrCholSol
      (0, ESC_chlmRfactor,  ESC_blasRdense,  ECSS_PtL,    cRs_bRd_cholSolPtL);
    cholSol_[13] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_blasRdense,  ECSS_Lt,     cRs_bRd_cholSolLt);
    cholSol_[14] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_blasRdense,  ECSS_L,      cRs_bRd_cholSolL);
    cholSol_[15] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_blasRdense,  ECSS_Pt,     cRs_bRd_cholSolPt);
    cholSol_[16] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_blasRdense,  ECSS_P,      cRs_bRd_cholSolP);
    cholSol_[17] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_chlmRsparse, ECSS_PtLLtP, cRs_cRs_cholSolPtLLtP);
    cholSol_[18] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_chlmRsparse, ECSS_LLt,    cRs_cRs_cholSolLLt);
    cholSol_[19] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_chlmRsparse, ECSS_LtP,    cRs_cRs_cholSolLtP);
    cholSol_[20] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_chlmRsparse, ECSS_PtL,    cRs_cRs_cholSolPtL);
    cholSol_[21] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_chlmRsparse, ECSS_Lt,     cRs_cRs_cholSolLt);
    cholSol_[22] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_chlmRsparse, ECSS_L,      cRs_cRs_cholSolL);
    cholSol_[23] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_chlmRsparse, ECSS_Pt,     cRs_cRs_cholSolPt);
    cholSol_[24] = StrCholSol
      (0, ESC_chlmRfactor,  ESC_chlmRsparse, ECSS_P,      cRs_cRs_cholSolP);

    for(i=1; i<cholSol_numOpt; i++) { cholSolAdd(cholSol_[i]); }    

    HashCSysName()[ CSysName(ECSS_PtLLtP) ] = ECSS_PtLLtP;
    HashCSysName()[ CSysName(ECSS_LLt)    ] = ECSS_LLt;
    HashCSysName()[ CSysName(ECSS_LtP )   ] = ECSS_LtP;
    HashCSysName()[ CSysName(ECSS_PtL)    ] = ECSS_PtL;
    HashCSysName()[ CSysName(ECSS_Lt )    ] = ECSS_Lt;
    HashCSysName()[ CSysName(ECSS_L  )    ] = ECSS_L;
    HashCSysName()[ CSysName(ECSS_Pt )    ] = ECSS_Pt;
    HashCSysName()[ CSysName(ECSS_P  )    ] = ECSS_P;
  
    assert(cholFac_numOpt==7);
    cholFac_[1] = StrCholFac(1, ESC_blasRdense,  ECFO_X,   bRd_choFac_X);
    cholFac_[2] = StrCholFac(2, ESC_blasRdense,  ECFO_XtX, bRd_choFac_XtX);
    cholFac_[3] = StrCholFac(3, ESC_blasRdense,  ECFO_XXt, bRd_choFac_XXt);
    cholFac_[4] = StrCholFac(4, ESC_chlmRsparse, ECFO_X,   cRs_choFac_X);
    cholFac_[5] = StrCholFac(5, ESC_chlmRsparse, ECFO_XtX, cRs_choFac_XtX);
    cholFac_[6] = StrCholFac(6, ESC_chlmRsparse, ECFO_XXt, cRs_choFac_XXt);
    
    for(i=1; i<cholFac_numOpt; i++) { cholFacAdd(cholFac_[i]); }    

    HashCFacName()[ CFacName(ECFO_X  )] = ECFO_X;
    HashCFacName()[ CFacName(ECFO_XtX)] = ECFO_XtX;
    HashCFacName()[ CFacName(ECFO_XXt)] = ECFO_XXt;
  }
  return(done_);
}


////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::EngineTypeNames()
////////////////////////////////////////////////////////////////////////////////
{
  static BText aux = "";
  if(aux=="")
  {
    int i;
    for(i=0; i<engineTypeName_numOpt; i++)
    {
      if(i>0) { aux+=", "; }
      aux += *engineTypeName_[i];
    }
  }
  return(aux);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::CellTypeNames()
////////////////////////////////////////////////////////////////////////////////
{
  static BText aux = "";
  if(aux=="")
  {
    int i;
    for(i=0; i<cellTypeName_numOpt; i++)
    {
      if(i>0) { aux+=", "; }
      aux += *cellTypeName_[i];
    }
  }
  return(aux);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::StoreTypeNames()
////////////////////////////////////////////////////////////////////////////////
{
  static BText aux = "";
  if(aux=="")
  {
    int i;
    for(i=0; i<storeTypeName_numOpt; i++)
    {
      if(i>0) { aux+=", "; }
      aux += *storeTypeName_[i];
    }
  }
  return(aux);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::CodeNames()
////////////////////////////////////////////////////////////////////////////////
{
  static BText aux = "";
  if(aux=="")
  {
    int i;
    for(i=0; i<codeName_numOpt; i++)
    {
      if(i>0) { aux+=", "; }
      aux += *codeName_[i];
    }
  }
  return(aux);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::CSysNames()
////////////////////////////////////////////////////////////////////////////////
{
  static BText aux = "";
  if(aux=="")
  {
    int i;
    for(i=0; i<cSysName_numOpt; i++)
    {
      if(i>0) { aux+=", "; }
      aux += *cSysName_[i];
    }
  }
  return(aux);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::CFacNames()
////////////////////////////////////////////////////////////////////////////////
{
  static BText aux = "";
  if(aux=="")
  {
    int i;
    for(i=0; i<cFacName_numOpt; i++)
    {
      if(i>0) { aux+=", "; }
      aux += *cFacName_[i];
    }
  }
  return(aux);
}


////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::EngineTypeName(EEngineType et)
////////////////////////////////////////////////////////////////////////////////
{
  return(*engineTypeName_[et]);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::CellTypeName(ECellType ct)
////////////////////////////////////////////////////////////////////////////////
{
  return(*cellTypeName_[ct]);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::StoreTypeName(EStoreType st)
////////////////////////////////////////////////////////////////////////////////
{
  return(*storeTypeName_[st]);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::CodeName(ECode sc)
////////////////////////////////////////////////////////////////////////////////
{
  return(*codeName_[sc]);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::CSysName(ECholSolSys cs)
////////////////////////////////////////////////////////////////////////////////
{
  return(*cSysName_[cs]);
}

////////////////////////////////////////////////////////////////////////////////
const BText& BVMat::CFacName(ECholFacOri cf)
////////////////////////////////////////////////////////////////////////////////
{
  return(*cFacName_[cf]);
}

////////////////////////////////////////////////////////////////////////////////
  BVMat::ECode BVMat::FindCodeName(const BText& codeName)
////////////////////////////////////////////////////////////////////////////////
{
  BHashCodeName::const_iterator fc = HashCodeName().find(codeName);
  if(fc!=HashCodeName().end())
  {
    return(fc->second);
  }
  else
  {
    return(ESC_undefined);
  }
}

////////////////////////////////////////////////////////////////////////////////
  BVMat::ECholSolSys BVMat::FindCSysName(const BText& cSysName)
////////////////////////////////////////////////////////////////////////////////
{
  BHashCSysName::const_iterator fc = HashCSysName().find(cSysName);
  if(fc!=HashCSysName().end())
  {
    return(fc->second);
  }
  else
  {
    return(ECSS_none);
  }
}
  
////////////////////////////////////////////////////////////////////////////////
  BVMat::ECholFacOri BVMat::FindCFacName(const BText& cfacName)
////////////////////////////////////////////////////////////////////////////////
{
  BHashCFacName::const_iterator fc = HashCFacName().find(cfacName);
  if(fc!=HashCFacName().end())
  {
    return(fc->second);
  }
  else
  {
    return(ECFO_none);
  }
}

  
////////////////////////////////////////////////////////////////////////////////
  const BVMat::StrDefCode* BVMat::FindDefCode(EEngineType engine, 
                                              ECellType cell, 
                                              EStoreType store)
////////////////////////////////////////////////////////////////////////////////
{
  int id = defCodeId(engine, cell, store);
  BHashDefCode::const_iterator fc = HashDefCode().find(id);
  if(fc!=HashDefCode().end())
  {
    return(fc->second);
  }
  else
  {
    return(NULL);
  }
}

////////////////////////////////////////////////////////////////////////////////
  const BVMat::StrConvert* BVMat::FindConvert(ECode left, ECode right)  
////////////////////////////////////////////////////////////////////////////////
{
  int id = codePairId(left, right);
  BHashConvert::const_iterator fc = HashConvert().find(id);
  if(fc!=HashConvert().end())
  {
    return(fc->second);
  }
  else
  {
    return(NULL);
  }
}

////////////////////////////////////////////////////////////////////////////////
  const BVMat::StrProduct*  BVMat::FindProduct(ECode left, ECode right)
////////////////////////////////////////////////////////////////////////////////
{
  int id = codePairId(left, right);
  BHashProduct::const_iterator fc = HashProduct().find(id);
  if(fc!=HashProduct().end())
  {
    return(fc->second);
  }
  else
  {
    return(NULL);
  }
}

////////////////////////////////////////////////////////////////////////////////
  const BVMat::StrCholSol* BVMat::FindCholSol(ECode left, ECode right, 
                                              ECholSolSys sys)
////////////////////////////////////////////////////////////////////////////////
{
  int id = cholSolId(left, right, sys);
  BHashCholSol::const_iterator fc = HashCholSol().find(id);
  if(fc!=HashCholSol().end())
  {
    return(fc->second);
  }
  else
  {
    return(NULL);
  }
}

////////////////////////////////////////////////////////////////////////////////
  const BVMat::StrCholFac* BVMat::FindCholFac(ECode code, ECholFacOri ori)
////////////////////////////////////////////////////////////////////////////////
{
  int id = cholFacId(code, ori);
  BHashCholFac::const_iterator fc = HashCholFac().find(id);
  if(fc!=HashCholFac().end())
  {
    return(fc->second);
  }
  else
  {
    return(NULL);
  }
}

////////////////////////////////////////////////////////////////////////////////
  const BText& BVMat::AvailConvert()
////////////////////////////////////////////////////////////////////////////////
{
  static BText aux_;
  if(!aux_.HasName())
  {
    BHashConvert::const_iterator iter;
    const StrConvert* convert;
    for(iter=HashConvert().begin(); iter!=HashConvert().end(); iter++)
    {
      convert = iter->second;
      aux_ += CodeName(convert->left) + 
             I2(" to ", " a ") + 
             CodeName(convert->right)+"\n";
    }
  }
  return(aux_);
} 
  
////////////////////////////////////////////////////////////////////////////////
  const BText& BVMat::AvailProduct()
////////////////////////////////////////////////////////////////////////////////
{
  static BText aux_;
  if(!aux_.HasName())
  {
    BHashProduct::const_iterator iter;
    const StrProduct* product;
    for(iter=HashProduct().begin(); iter!=HashProduct().end(); iter++)
    {
      product = iter->second;
      aux_ += CodeName(product->left) + 
             " * " + 
             CodeName(product->right)+"\n";
    }
  }
  return(aux_);
} 

