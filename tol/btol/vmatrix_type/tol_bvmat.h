/* tol_bvmat.h: BVMat: Virtual matrix handler class.
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

#ifndef TOL_BVMAT_H
#define TOL_BVMAT_H 1

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bcommon.h>
#include <tol/tol_bdat.h>

////////////////////////////////////////////////////////////////////////////////
//Forward types declaring are defined in tol_bvmat_impl.h
////////////////////////////////////////////////////////////////////////////////
//Forward declaring of tol types
class BText;
class BSet;
class BStandardOperator;
class BProbDist;
class BStream;
class BOisCreator;
class BOisLoader;
template<class Any> class BArray;
template<class Any> class BPolyn;
template<class Any> class BRational;
template<class Any> class BMatrix;
//Forward declaring of cholmod types
struct cholmod_common_struct;
struct cholmod_dense_struct;
struct cholmod_sparse_struct;
struct cholmod_factor_struct;
struct cholmod_triplet_struct;

////////////////////////////////////////////////////////////////////////////////
//Standarized type names
////////////////////////////////////////////////////////////////////////////////
#define cholmod_R_dense   cholmod_dense_struct
#define cholmod_R_sparse  cholmod_sparse_struct
#define cholmod_R_factor  cholmod_factor_struct
#define cholmod_R_triplet cholmod_triplet_struct

#define cholmod_C_dense   cholmod_dense_struct
#define cholmod_C_sparse  cholmod_sparse_struct
#define cholmod_C_factor  cholmod_factor_struct
#define cholmod_C_triplet cholmod_triplet_struct

#define blas_R_dense      cholmod_dense_struct
//still undefined
#define blas_R_triang     void
#define blas_R_banded     void

#define blas_C_dense      cholmod_dense
//still undefined
#define blas_C_triang     void
#define blas_C_banded     void

////////////////////////////////////////////////////////////////////////////////
class TOL_API BVMat

/*! Virtual matrix handler for polymorphic virtual matrix storement and handling
 */
////////////////////////////////////////////////////////////////////////////////
{
public:
/*------------------------------------------------------------------------------
  Memeber types
------------------------------------------------------------------------------*/
//Identifier of engine type, the internal system used to handle with matrices
  enum EEngineType  
  {
    EET_Blas    = 0, 
    EET_Cholmod = 1 
  };          
//Identifier of cell data type
  enum ECellType  
  {
    ECT_Real    = 0, 
    ECT_Complex = 1
  };      
//Identifier of each posible storement strategy
  enum EStoreType
  {
    EST_Dense   = 0,
    EST_Sparse  = 1,
    EST_Factor  = 2,
    EST_Triplet = 3,
    EST_Banded  = 4,
    EST_Triang  = 5
  };
//Identifier of each virtual matrix sub-type defined by crossing available 
//engine, cell and store types
  enum ECode
  {
    ESC_undefined    = 0,
    ESC_blasRdense   = 1,
    ESC_chlmRsparse  = 2,
    ESC_chlmRfactor  = 3,
    ESC_chlmRtriplet = 4
  };
//Allows references to all available sub-types in just a pointer
  union UStore
  {
    void              *undefined_;
    blas_R_dense      *blasRdense_;
    cholmod_R_sparse  *chlmRsparse_;
    cholmod_R_factor  *chlmRfactor_;
    cholmod_R_triplet *chlmRtriplet_;
  };
//Function pointer types  
  typedef int    (*BConvert) (BVMat& left, const BVMat& right);
  typedef int    (*BBinFun)  (const BVMat& A, const BVMat& B, BVMat& C);
  typedef double (*BFunR2R)  (double x, double y);
  typedef double (*BFunRR)   (double x);
  typedef BDat   (*BFunRR_)  (const BDat& x);
  typedef void   (*BChfFun)  (const BVMat& X, BVMat& L,
                              bool& isOk, bool& isNotPosDef);
//Attach each available triplet of engine, cell, store with a sub-type code
  struct TOL_API StrDefCode
  {
    int         id;
    ECode       code;
    EEngineType engine;
    ECellType   cell;
    EStoreType  store;
    StrDefCode()  {};
    StrDefCode(int i, ECode c, EEngineType e, ECellType l, EStoreType s)
    : id(i), code(c), cell(l), store(s) {}
  };
//Attach each conversor function to a pair of subtypes  
  struct TOL_API StrConvert
  {
    int      id;
    ECode    left; 
    ECode    right;
    BConvert fun;
    StrConvert()  {};
    StrConvert(int i, ECode l, ECode r, BConvert f)
    : id(i), left(l), right(r), fun(f) {}
  };
//Attach each prodcut function to a pair of subtypes  
  struct TOL_API StrProduct
  {
    int      id;
    ECode    left; 
    ECode    right;
    BBinFun  fun;
    StrProduct()  {};
    StrProduct(int i, ECode l, ECode r, BBinFun f)
    : id(i), left(l), right(r), fun(f) {}
  };
//Identifer of all factorizations that it's posible to make
  enum ECholFacOri
  {
    ECFO_none = 0, // Invalid or undefined factorization
    ECFO_X    = 1, // L L' = X
    ECFO_XtX  = 2, // L L' = X' X
    ECFO_XXt  = 3, // L L' = X  X'
  };
//Attach each Choleski decomposition function to a subtype and a origin 
//definition
  struct TOL_API StrCholFac
  {
    int         id;
    ECode       code;
    ECholFacOri ori;
    BChfFun     fun;
    StrCholFac()  {};
    StrCholFac(int i, ECode c, ECholFacOri o, BChfFun f)
    : id(i), code(c), ori(o), fun(f) {}
  };
//Identifer of all systems that it's posible to solve with a Choleski 
//decomposition
  enum ECholSolSys
  {
    ECSS_none   = 0, // Invalid or undefined system
    ECSS_PtLLtP = 1, // P'*L*L'*P*x = b
    ECSS_LLt    = 2, //    L*L'  *x = b
    ECSS_LtP    = 3, //     L'*P *x = b
    ECSS_PtL    = 4, // P'*L     *x = b
    ECSS_Lt     = 5, //      L'  *x = b
    ECSS_L      = 6, //    L     *x = b
    ECSS_Pt     = 7, // P'       *x = b
    ECSS_P      = 8  //        P *x = b
  };
//Attach each Choleski solver function to a pair of subtypes and system 
  struct TOL_API StrCholSol
  {
    int         id;
    ECode       left; 
    ECode       right;
    ECholSolSys sys;
    BBinFun     fun;
    StrCholSol()  {};
    StrCholSol(int i, ECode l, ECode r, ECholSolSys s, BBinFun f)
    : id(i), left(l), right(r), sys(s), fun(f) {}
  };

/*------------------------------------------------------------------------------
  Data members  
------------------------------------------------------------------------------*/
private:
  static const int engineTypeName_numOpt = 2;
  static const int cellTypeName_numOpt   = 2;
  static const int storeTypeName_numOpt  = 4;
  static const int codeName_numOpt       = 5;
  static const int cSysName_numOpt       = 9;
  static const int cFacName_numOpt       = 4;
  static const int defCode_numOpt        = 5;
  static const int convert_numOpt        = 8;
  static const int product_numOpt        = 5;
  static const int cholSol_numOpt        = 25;
  static const int cholFac_numOpt        = 7;

  static BText      *engineTypeName_ [engineTypeName_numOpt];
  static BText      *cellTypeName_   [cellTypeName_numOpt];
  static BText      *storeTypeName_  [storeTypeName_numOpt];
  static BText      *codeName_       [codeName_numOpt];
  static BText      *cSysName_       [cSysName_numOpt];
  static BText      *cFacName_       [cFacName_numOpt];
  static StrDefCode  defCode_        [defCode_numOpt];
  static StrConvert  convert_        [convert_numOpt];
  static StrProduct  product_        [product_numOpt];
  static StrCholSol  cholSol_        [cholSol_numOpt];
  static StrCholFac  cholFac_        [cholFac_numOpt];

  static cholmod_common_struct  *common_;
  static FILE* cholmod_print_file_;
  static BDat autoConvert_;
public:
  UStore s_;
  ECode  code_;

/*------------------------------------------------------------------------------
  vmat.cpp: Generic methods  
------------------------------------------------------------------------------*/
public:
  static int InitializeClass();
//Constructors
  BVMat();
  BVMat(const BVMat& v);
  BVMat(const BVMat& v, ECode c);
//Destructor
 ~BVMat();
//Internal destructor  
  void Delete();
//Copy operator
  const BVMat& operator = (const BVMat& v);  
  int  Check       () const;
  void Pack        ();
  void Copy        (const BVMat& v);
  void BlasRDense  (int nrow, int ncol);
  void ChlmRSparse (int nrow, int ncol, int nzmax);
  void ChlmRTriplet(int nrow, int ncol, int nzmax);
//Dimensions
  bool CheckDefined(const char* fName) const;
  int  Rows        () const;
  int  Columns     () const;
  int  NonNullCells(double chop=0.0) const;
  bool IsDiagonal  () const;
  bool IsSymmetric () const;
  void CompactSymmetric(bool check);
  
//TOL BGenContens<Any> required methods  
  BText Info() const;
  BText Dump() const;
  static BVMat& Unknown();
  static BDat Compare(const BVMat* A, const BVMat* B);
//Partial access
  double GetCell(int i, int j) const;
  int    PutCell(int i, int j, const double& x);
  int    PutBlock(int i0, int j0, const BVMat& x);

  int  StoredCells() const;
  bool StoredData (const double *&x, int &nzmax) const;
  bool StoredData (      double *&x, int &nzmax);

  int Enlarge(int nrow, int ncol, int row0, int col0);
  int MergeRows(int nrow, BSet& items);

  BVMat ApplyFunRR (BFunRR f,  bool f00, const char* fName) const;
  BVMat ApplyFunR2R(BFunR2R f, bool f00, const char* fName,
                    const BVMat& B) const;
  BVMat ApplyFunR2R(BFunR2R f, bool f00, const char* fName,
                   double b) const;
  static int ApplyFunRR (BFunRR f, bool f00, const char* fName,
                         const BVMat& A,  BVMat& B);
  static int ApplyFunR2R(BFunR2R f, bool f00, const char* fName,
                         const BVMat& A, const BVMat& B, BVMat& C);
  static int ApplyFunR2R(BFunR2R f, bool f00, const char* fName,
                         const BVMat& A, double b, BVMat& C);
  static void BinGroup(BBinFun f, BVMat** arr, int num, BVMat& C);
  static BDat& AutoConvert();
private:
  static bool force_natural_order(bool force);
  static void restore_cholmod_common();
  double* bRd_checkCell(int i, int j, int& accessCode) const;
  double* cRs_checkCell(int i, int j, int& accessCode) const;
  double* checkCell(int i, int j,
                    int& accessCode, bool readOnly,
                    const char* fName, bool showMsg) const;
  int bRd_bRd_PutBlock(int i0, int j0, const BVMat& x, int& accessCode);
  int cRs_bRd_PutBlock(int i0, int j0, const BVMat& x, int& accessCode);


/*------------------------------------------------------------------------------
  vmat_io.cpp: input-output methods
------------------------------------------------------------------------------*/  
public:
  void   Print(FILE* file, const char* name, int level) const;
  void   Scan (FILE* file);
  void   WriteMatrixMarket(FILE* file);
  size_t Bytes() const;
  bool   Write(BOisCreator& ois, BStream* name) const;
  bool   Read (BOisLoader& ois, BStream* stream);
  
  static void err_cannot_apply(const char* fName, const char* cond, 
                               const BVMat& a);
private:
  size_t bytes_blasRdense  () const;
  size_t bytes_chlmRsparse () const;
  size_t bytes_chlmRfactor () const;
  size_t bytes_chlmRtriplet() const;
  bool write_blasRdense  (BOisCreator& ois, BStream* stream) const;
  bool write_chlmRsparse (BOisCreator& ois, BStream* stream) const;
  bool write_chlmRfactor (BOisCreator& ois, BStream* stream) const;
  bool write_chlmRtriplet(BOisCreator& ois, BStream* stream) const;
  bool read_blasRdense   (BOisLoader&  ois, BStream* stream);
  bool read_chlmRsparse  (BOisLoader&  ois, BStream* stream);
  bool read_chlmRfactor  (BOisLoader&  ois, BStream* stream);
  bool read_chlmRtriplet (BOisLoader&  ois, BStream* stream);
  
  static void warn_convert2bRd      (const char* fName, const BVMat& a);
  static void warn_convert2cRt      (const char* fName, const BVMat& a);
  static void warn_convert2cRs      (const char* fName, const BVMat& a);
  static void warn_cannot_apply     (const char* fName, const char* cond, 
                                     const BVMat& a);
  static void err_wrong_defined     (const char* fName, const BVMat& a);
  static void err_cannot_create     (const char* fName, ECode code);
  static void err_cannot_create     (const char* fName, const char* cond);
  static void err_invalid_subtype   (const char* fName,
                                     const BVMat& a);
  static void err_invalid_subtypes  (const char* fName, 
                                     const BVMat& a, const BVMat& b);
  static void err_invalid_dimensions(const char* fName, int nrow, int ncol);
  static void err_invalid_dimensions(const char* fName, 
                                     const BVMat& a, const BVMat& b);
  static void err_invalid_dimensions(const char* fName, 
                                     const BVMat& a, const BVMat& b, const BVMat& c);
  static void cholmod_error_handler (int status, char *file,int line,char *msg);
  static int  cholmod_print_function(const char *fmt, ...);
  

/*------------------------------------------------------------------------------
  vmat_subtypes.cpp: Identification of subtypes and related methods
------------------------------------------------------------------------------*/
public:
  static const BText& EngineTypeNames();
  static const BText& CellTypeNames  ();
  static const BText& StoreTypeNames ();
  static const BText& CodeNames      ();
  static const BText& CSysNames      ();
  static const BText& CFacNames      ();

  static const BText& EngineTypeName(EEngineType et);
  static const BText& CellTypeName  (ECellType   ct);
  static const BText& StoreTypeName (EStoreType  st);
  static const BText& CodeName      (ECode       sc);
  static const BText& CSysName      (ECholSolSys cs);
  static const BText& CFacName      (ECholFacOri cf);
  static       ECode       FindCodeName(const BText& codeName);
  static       ECholSolSys FindCSysName(const BText& csysName);
  static       ECholFacOri FindCFacName(const BText& cfacName);
  static const StrDefCode* FindDefCode (EEngineType eng, ECellType cl,
                                        EStoreType st);
  static const StrConvert* FindConvert (ECode left, ECode right);
  static const StrProduct* FindProduct (ECode left, ECode right);
  static const StrCholSol* FindCholSol (ECode left, ECode right, 
                                        ECholSolSys sys);
  static const StrCholFac* FindCholFac (ECode code, ECholFacOri ori);
  static const BText& AvailConvert();
  static const BText& AvailProduct();
private:
  static int initializeIdentifiers();
  static int defCodeId  (EEngineType engine, ECellType cell, EStoreType store);
  static int codePairId (ECode left, ECode right);
  static int cholSolId  (ECode left, ECode right, ECholSolSys sys);
  static int cholFacId  (ECode code, ECholFacOri ori);
  static int defCodeAdd (StrDefCode& s);
  static int convertAdd (StrConvert& s);
  static int productAdd (StrProduct& s);
  static int cholSolAdd (StrCholSol& s);
  static int cholFacAdd (StrCholFac& s);
  
/*------------------------------------------------------------------------------
  vmat_convert.cpp: Convering methods 
------------------------------------------------------------------------------*/
public:
//Importing methods
  void Convert     (const BVMat& v, ECode c);
  void Convert     (const BVMat& v, const BText& codeName);

  void DMat2dense  (const BMatrix<double>& d);
  void DMat2denseT (const BMatrix<double>& d);
  void DMat2sparse (const BMatrix<double>& d);
  void DMat2sparseT(const BMatrix<double>& d);
  void DMat2VMat   (const BMatrix<double>& d, 
                    bool traspose = false, 
                    double minSparse = 0.5,
                    double drop = 0.0);
  void DMat2triplet(const BMatrix<double>& d, int nrow, int columns);
  void DMat2triplet(const BMatrix<double>& d, int nrow, int columns,
                    const BArray<int>& rowIdx, const BArray<int>& colIdx);
  void Set2triplet (const BSet& ijx, int nrow, int columns);
  void Set2triplet (const BSet& ijx, int nrow, int columns,
                    const BArray<int>& rowIdx, const BArray<int>& colIdx);
  void BSet2sparse (const BSet& sparseDef);
  void BPol2sparse (const BPolyn<BDat>& pol_, int nrow, int ncol);
  void BSetSparseLinReg2sparse(const BSet& sparseDef);
//Exporting methods
  void GetTriplet(BMatrix<double>& ijx) const;
  void GetDMatT  (BMatrix<double>& d)   const;
  void GetDMat   (BMatrix<double>& d)   const;
//Matrix instances
  void Eye  (int nrow, int ncol, int diag, const BVMat& v);
  void Eye  (int nrow, int ncol, int diag, double x=1.0);
  void Eye  (int nrow, int ncol, ECode c=ESC_chlmRsparse);
  void Zeros(int nrow, int ncol, ECode c=ESC_chlmRsparse);
  void Eye  (int n, ECode c=ESC_chlmRsparse);
  void Zeros(int n, ECode c=ESC_chlmRsparse);
//Assembly
  static int CatCol(const BVMat& A, const BVMat& B, BVMat& C);
  static int CatRow(const BVMat& A, const BVMat& B, BVMat& C);
  BVMat operator| (const BVMat& B)   const;
  BVMat operator<<(const BVMat& B)   const;
//Partial access
  int Sub     (int i, int j, int r, int c, BVMat& aux) const;
  int SubCells(const BArray<int>& rows, const BArray<int>& col,BVMat& aux)const;
  int SubRows (const BArray<int>& rows, BVMat& aux) const;
  int SubCols (const BArray<int>& rows, BVMat& aux) const;
  int SubBand (int from, int until, BVMat& aux) const;
  int SubDiag(int numDiag, BVMat& aux) const;

  BVMat  Sub     (int i, int j, int r, int c  ) const;
  BVMat  SubCells(const BArray<int>& rows, const BArray<int>& cols) const;
  BVMat  SubRows (const BArray<int>& rows ) const;
  BVMat  SubCols (const BArray<int>& cols ) const;
  BVMat  SubBand (int from, int until) const;
  BVMat  SubDiag (int numDiag) const;
  
  static void cRs_ensure_packed(cholmod_sparse_struct* sp);

private:
  static cholmod_sparse_struct* cRf2cRs(cholmod_factor_struct* factor);
  static int bRd2cRs(BVMat& left, const BVMat& right);
  static int bRd2cRt(BVMat& left, const BVMat& right);
  static int cRf2cRs(BVMat& left, const BVMat& right);
  static int cRt2cRs(BVMat& left, const BVMat& right);
  static int cRs2cRt(BVMat& left, const BVMat& right);
  static int cRs2bRd(BVMat& left, const BVMat& right);
  static int cRt2bRd(BVMat& left, const BVMat& right);
  static int convertIfNeeded2cRs(const BVMat&  A,  const BVMat&  B, 
                                       BVMat*& A_,       BVMat*& B_,
                                 const char* fName, bool forze=false);
  static int convertIfNeeded2bRd(const BVMat&  A,  const BVMat&  B, 
                                       BVMat*& A_,       BVMat*& B_,
                                 const char* fName, bool forze=false);
  static int convertIfNeeded2bRd(const BVMat&  A,  BVMat*& A_, 
                                 const char* fName);
  static int convertIfNeeded_cRt2cRs(const BVMat&  A,  const BVMat&  B, 
                                           BVMat*& A_,       BVMat*& B_,
                                     const char* fName, bool forze=false);
/*------------------------------------------------------------------------------
  vmat_stats.cpp: Statistics methods
------------------------------------------------------------------------------*/
public:
  double Trace() const;
  double LogTrace() const;
  double Sum() const;
  double Avr() const;
  double Moment(int order) const;
  double CenterMoment(int order) const;
  double Var() const;
  double StDs() const;
  double SquaresdSum() const;
  double EuclideanNorm() const;
  double FrobeniusNorm() const;
  double Asymmetry() const;
  double Kurtosis() const;
  double Max() const;
  double Min() const;
  void Rand(int nrow, int ncol,
            double min, double max, 
            ECode c=ESC_blasRdense, int nzmax=-1);
  void Gaussian(int nrow, int ncol, double nu, double sigma);
  static int GetBoundsInPolytope(const BVMat&  D, 
                                 const BVMat&  d, 
                                       BVMat&  z,
                                       int     j,
                                       double& lower,
                                       double& upper);
  void TruncStdGaussian(const BVMat& D, const BVMat& d_, 
                        const BVMat& z0_, int ncol, int burnin);
private:
  static cholmod_dense_struct*   bRd_rand(int nrow, int ncol,
                                          BProbDist& xu);
  static cholmod_triplet_struct* cRt_rand(int nrow, int ncol, int nzmax,
                                          BProbDist& xu);
  static int trunc_std_gaussian(const BVMat& D, 
                                const BVMat& d, 
                                      BVMat& b, 
                                      BVMat& z);
  
/*------------------------------------------------------------------------------
  vmat_arith.cpp: Arithmetic methods 
------------------------------------------------------------------------------*/
public:
//Matrix algebra operators
  int Drop    (double drop);
  BVMat Abs   () const;
  BVMat Round () const;
  BVMat Floor () const;
  BVMat Ceil  () const;
  BVMat Sqrt  () const;
  BVMat Exp   () const;
  BVMat Log   () const;
  BVMat Log10 () const;
  BVMat Sin   () const;
  BVMat Cos   () const;
  BVMat Tan   () const;
  BVMat ASin  () const;
  BVMat ACos  () const;
  BVMat ATan  () const;
  BVMat Sign  () const;
  BVMat SinH  () const;
  BVMat CosH  () const;
  BVMat TanH  () const;
  BVMat ASinH () const;
  BVMat ACosH () const;
  BVMat ATanH () const;
  BVMat T     () const;
  BVMat MtMSqr() const;
  BVMat MMtSqr() const;
  BVMat Sum  (double b) const;
  BVMat Rest (double b) const;
  BVMat Prod (double b) const;
  BVMat Quot (double b) const;
  BVMat Pow  (double b) const;
  BVMat operator+ (double b) const;
  BVMat operator- (double b) const;
  BVMat operator* (double b) const;
  BVMat operator/ (double b) const;
  BVMat operator^ (double b) const;
  BVMat operator+ (const BVMat&   B) const;
  BVMat operator- (const BVMat&   B) const;
  BVMat operator* (const BVMat&   B) const;
  BVMat WeightProd(const BVMat&   B) const;
  BVMat WeightQuot(const BVMat&   B) const;

  static BVMat Drop  (const BVMat& A, double drop);
  static int Abs   (const BVMat& A, BVMat& B);
  static int Round (const BVMat& A, BVMat& B);
  static int Floor (const BVMat& A, BVMat& B);
  static int Ceil  (const BVMat& A, BVMat& B);
  static int Sqrt  (const BVMat& A, BVMat& B);
  static int Exp   (const BVMat& A, BVMat& B);
  static int Log   (const BVMat& A, BVMat& B);
  static int Log10 (const BVMat& A, BVMat& B);
  static int Sin   (const BVMat& A, BVMat& B);
  static int Cos   (const BVMat& A, BVMat& B);
  static int Tan   (const BVMat& A, BVMat& B);
  static int ASin  (const BVMat& A, BVMat& B);
  static int ACos  (const BVMat& A, BVMat& B);
  static int ATan  (const BVMat& A, BVMat& B);
  static int Sign  (const BVMat& A, BVMat& B);
  static int SinH  (const BVMat& A, BVMat& B);
  static int CosH  (const BVMat& A, BVMat& B);
  static int TanH  (const BVMat& A, BVMat& B);
  static int ASinH (const BVMat& A, BVMat& B);
  static int ACosH (const BVMat& A, BVMat& B);
  static int ATanH (const BVMat& A, BVMat& B);
  static int T     (const BVMat& A, BVMat& B);
  static int MtMSqr(const BVMat& A, BVMat& B);
  static int MMtSqr(const BVMat& A, BVMat& B);
  static int Sum (const BVMat& A, double b, BVMat& C);
  static int Rest(const BVMat& A, double b, BVMat& C);
  static int Prod(const BVMat& A, double b, BVMat& C);
  static int Quot(const BVMat& A, double b, BVMat& C);
  static int Pow (const BVMat& A, double b, BVMat& C);
  static int Sum       (const BVMat& A, const BVMat& B, BVMat& C);
  static int Rest      (const BVMat& A, const BVMat& B, BVMat& C);
  static int Prod      (const BVMat& A, const BVMat& B, BVMat& C);
  static int WeightProd(const BVMat& A, const BVMat& B, BVMat& C);
  static int WeightQuot(const BVMat& A, const BVMat& B, BVMat& C);

  static double GetBackwardValue(const BPolyn  <BDat>& P,
                                 const double* X0, int n0, 
                                 const double* X,  int n, 
                                 int i, int k);
  static double GetForwardValue (const BPolyn  <BDat>& P,
                                 const double* X0, int n0, 
                                 const double* X,  int n, 
                                 int i, int k);
  static void DifEq(const BRational <BDat>& R,
                    const BVMat& X0,
                    const BVMat& X,
                    const BVMat& Y0,
                          BVMat& Y_,
                          double minSparse,
                          double chop);
  static void BackDifEq(const BRational <BDat>& R,
                        const BVMat& X0,
                        const BVMat& X,
                        const BVMat& Y0,
                              BVMat& Y_,
                              double minSparse,
                              double chop);
  static void DifEq(const BRational <BDat>& R,
                    const BVMat& X,
                          BVMat& Y_,
                          double minSparse,
                          double chop);
  static void BackDifEq(const BRational <BDat>& R,
                    const BVMat& X,
                          BVMat& Y_,
                          double minSparse,
                          double chop);

private:
  static int sum(const BVMat& A, double b, const BVMat& B, BVMat& C,
                 const char* fName);
  static int bRd_bRd_prod(const BVMat& A, const BVMat&  B, BVMat& C);
  static int cRs_bRd_prod(const BVMat& A, const BVMat&  B, BVMat& C);
  static int bRd_cRs_prod(const BVMat& A, const BVMat&  B, BVMat& C);
  static int cRs_cRs_prod(const BVMat& A, const BVMat&  B, BVMat& C);  
  static int pow_nz      (const BVMat& A, double b, BVMat& C);

  
/*------------------------------------------------------------------------------
  vmat_logic.cpp: Arithmetic methods 
------------------------------------------------------------------------------*/
public:
  BVMat Not() const;
  BVMat Yes() const;
  BVMat IsUnknown() const;
  BVMat IsFinite() const;
  BVMat And(const BVMat& B) const;
  BVMat Or (const BVMat& B) const;
  BVMat EQ (const BVMat& B) const;
  BVMat NE (const BVMat& B) const;
  BVMat LE (const BVMat& B) const;
  BVMat LT (const BVMat& B) const;
  BVMat GE (const BVMat& B) const;
  BVMat GT (const BVMat& B) const;
  BVMat Min(const BVMat& B) const;
  BVMat Max(const BVMat& B) const;

  static int Not(const BVMat& A, BVMat& B);
  static int Yes(const BVMat& A, BVMat& B);
  static int IsUnknown(const BVMat& A, BVMat& B);
  static int IsFinite(const BVMat& A, BVMat& B);
  static int And(const BVMat& A, const BVMat& B_, BVMat& C);
  static int Or (const BVMat& A, const BVMat& B, BVMat& C);
  static int EQ (const BVMat& A, const BVMat& B, BVMat& C);
  static int NE (const BVMat& A, const BVMat& B, BVMat& C);
  static int LE (const BVMat& A, const BVMat& B, BVMat& C);
  static int LT (const BVMat& A, const BVMat& B, BVMat& C);
  static int GE (const BVMat& A, const BVMat& B, BVMat& C);
  static int GT (const BVMat& A, const BVMat& B, BVMat& C);
  static int Min(const BVMat& A, const BVMat& B, BVMat& C);
  static int Max(const BVMat& A, const BVMat& B, BVMat& C);
  
/*------------------------------------------------------------------------------
  vmat_chol.cpp: Choleski methods 
------------------------------------------------------------------------------*/
public:
//Linear systems of equations  
  BVMat CholeskiFactor(ECholFacOri ori, bool checkSymmetric,
                            bool forceNaturalOrder);
  BVMat CholeskiFactor(const BText& ori, bool checkSymmetric,
                            bool forceNaturalOrder);
  BVMat CholeskiSolve(const BVMat& B, ECholSolSys sys)const;
  BVMat CholeskiSolve(const BVMat& B, const BText& sys)const;
  BVMat CholeskiInv  ()const;

  static int CholeskiFactor(const BVMat& X, BVMat& L, 
                            ECholFacOri ori, bool checkSym,
                            bool forceNaturalOrder);
  static int CholeskiFactor(const BVMat& X, BVMat& L, 
                            const BText& ori, bool checkSym,
                            bool forceNaturalOrder);
  static int CholeskiSolve (const BVMat& L,const BVMat& B, 
                            BVMat& X, ECholSolSys sys);
  static int CholeskiSolve (const BVMat& L,const BVMat& B,
                            BVMat& X, const BText&sys);
  static int CholeskiInv   (const BVMat& X, BVMat& Xi);
  
private:
  static void bRd_choFac_X  (const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef);
  static void bRd_choFac_XtX(const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef);
  static void bRd_choFac_XXt(const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef);
  static void cRs_choFac_X  (const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef);
  static void cRs_choFac_XtX(const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef);
  static void cRs_choFac_XXt(const BVMat& X, BVMat& L,
                             bool& isOk, bool& isNotPosDef);

  static int bRd_bRd_cholSolLLt   (const BVMat& L, const BVMat& b, BVMat& x);
  static int bRd_bRd_cholSolLt    (const BVMat& L, const BVMat& b, BVMat& x);
  static int bRd_bRd_cholSolL     (const BVMat& L, const BVMat& b, BVMat& x);
  static int bRd_bRd_cholSolId    (const BVMat& L, const BVMat& b, BVMat& x);

  static int cRs_bRd_cholSolPtLLtP(const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_bRd_cholSolLLt   (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_bRd_cholSolLtP   (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_bRd_cholSolPtL   (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_bRd_cholSolLt    (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_bRd_cholSolL     (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_bRd_cholSolPt    (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_bRd_cholSolP     (const BVMat& L, const BVMat& b, BVMat& x);

  static int cRs_cRs_cholSolPtLLtP(const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_cRs_cholSolLLt   (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_cRs_cholSolLtP   (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_cRs_cholSolPtL   (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_cRs_cholSolLt    (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_cRs_cholSolL     (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_cRs_cholSolPt    (const BVMat& L, const BVMat& b, BVMat& x);
  static int cRs_cRs_cholSolP     (const BVMat& L, const BVMat& b, BVMat& x);

/*------------------------------------------------------------------------------
  vmat_iterative.cpp: Iterative methods for solving linear problems
------------------------------------------------------------------------------*/
public:
  //Linear systems of equations  
  static BVMat MinimumResidualsSolve(const BVMat& A,
                                     const BVMat& b0,
                                     const BVMat& x0,
                                     double chop,
                                     int maxIter);
  static BVMat MinimumResidualsSolve(const BVMat& A,
                                     const BVMat& b0,
                                     double chop,
                                     int maxIter);
  static BVMat MinimumResidualsSolve(BStandardOperator* A,
                                     const BVMat& b0,
                                     const BVMat& x0,
                                     double chop,
                                     int maxIter);
  static BVMat MinimumResidualsSolve(BStandardOperator* A,
                                     const BVMat& b0,
                                     double chop,
                                     int maxIter);

};

////////////////////////////////////////////////////////////////////////////////
class BVMatColMajIter
////////////////////////////////////////////////////////////////////////////////
{
 public:
  //Calling method to show in messages
  const char* fName_;
  //Virtual matrix to be iterated
  const BVMat&      M_;
  //Number of rows
  int r_;
  //Number of columns
  int c_;
  //Current cell pointer
  double* cell_;
  //Current row
  int i_;
  //Current column
  int j_;

  //Cholmod sparse specific iterators
  static double zero_; //To be referenced in null cells
  int k0_; //Index of first not null cell of current column
  int k1_; //Index of first not null cell of next column
  int k_;  //Index of current or next not null cell in current column
  int i_nz_; //Row of last null cell

  BVMatColMajIter(const BVMat& M, const char* fName);
  double GetCell();
  bool   NextRow();
  bool   NextCol();
  bool   GoToCol(int j);
  bool   GetCol(int j, BArray<double>& x);
  bool   cRs_SetCell();

};


#endif // TOL_BVMAT_H
