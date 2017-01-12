
/* Microsoft stdext::hash_map */
  #define __HASH_MAP_MSVC__    1
/* google::dense_hash_map and google::sparse_hash_map */
  #define __HASH_MAP_GOOGLE__  2
/* The used hashing handler */
  #define __USE_HASH_MAP__   __HASH_MAP_GOOGLE__ 

#undef BIGGEST_FIELD_ALIGNMENT 
#define BIGGEST_FIELD_ALIGNMENT 4

#include <string.h>
#include <tol/tol_init.h>
#include <tol/tol_bfsmem.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdate.h>
#include <tol/tol_bdtegra.h>

#ifdef UNIX
  #define DLLEXPORT(f) f
#else
  #define DLLEXPORT(f) __declspec(dllexport) f
#endif

#include <R.h>

extern "C" DLLEXPORT(void) RInitTolKernel(void);
  
//////////////////////////////////////////////////////////////////////////////

extern BList R_stack;
void R_concat(BList* head, BList* tail);

BList R_stack;

//----------------------------------------------------------------------------
void R_concat(BList* head, BList* tail)
//----------------------------------------------------------------------------
{
  if (head->Cdr()) {
    R_concat(head->Cdr(), tail);
  }
  else {
    head->PutCdr(tail);
  }
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolCompile(char **name, int *ok)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_lvalue = (char*)*name;
  BSyntaxObject * so = GraAnything()->EvaluateExpr(str_lvalue);
  if (so) {
    BList* result = NIL;
    BList* aux = NIL;
    LstFastAppend(result, aux, so);
    R_concat(&R_stack, result);
    *ok = 1;
  }
  else {
    *ok = 0;
  }
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolDecompile(char **name)
//----------------------------------------------------------------------------
{
  char* str_name = (char*)*name;
  BList *P = R_stack.Cdr(), *prev = &R_stack;
  short found = 0;
  
  while (!found && P) {
    BSyntaxObject * so = (BSyntaxObject*)(P->Car());
    const char* object_name = so->Name().String();
    if (!strcmp(object_name, str_name)) {
	  prev->PutCdr(P->Cdr());
      delete P;
      DESTROY(so);
      found = 1;
	  break;
    }
	prev = P;
    P = P->Cdr();
  }
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTOLVersion(char **version)
//----------------------------------------------------------------------------
{
  *version = (char*)TOLVersion();
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTOLHasBeenInitialized(int *initialized)
//----------------------------------------------------------------------------
{
  *initialized = TOLHasBeenInitialized();
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RInitTolKernel(void)
//----------------------------------------------------------------------------
{
  InitTolKernel(NULL, 0, "vA");
  LoadInitLibrary(true, true);
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolGetReal(char **name, double *value)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_lvalue = (char*)*name;
  BSyntaxObject * so = GraReal()->EvaluateExpr(str_lvalue);
  if (so) {
    BDat & d = Dat(so);
    *value = d.Value();
    DESTROY(so);
  }
  else {
    error("Real expression '%s' is not valid", str_lvalue);
  }
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolGetText(char **name, char **value)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_lvalue = (char*)*name;
  BSyntaxObject * so = GraText()->EvaluateExpr(str_lvalue);
  if (so) {
    BText & d = Text(so);
    *value = (char *)(d.String());
    DESTROY(so);
  }
  else {
    error("Text expression '%s' is not valid", str_lvalue);
  }
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolGetDate(char **name, double *value)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_lvalue = (char*)*name;
  BSyntaxObject * so = GraDate()->EvaluateExpr(str_lvalue);
  if (so) {
    BDate & d = Date(so);
    *value = (double)(d.Index());
    DESTROY(so);
  }
  else {
    error("Date expression '%s' is not valid", str_lvalue);
  }
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolGetMatrix(char **name, double *value)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_lvalue = (char*)*name;
  BSyntaxObject * so = GraMatrix()->EvaluateExpr(str_lvalue);
  if (so) {
    BMat & m = Mat(so);
    int cols = m.Columns();
    int rows = m.Rows();	
    for (int c=0; c<cols; ++c) {
      for (int r=0; r<rows; ++r) {
        BDat & d = m(r, c);
        value[c*rows+r] = d.Value();
      }
    }
    DESTROY(so);
  }
  else {
    error("Matrix expression '%s' is not valid", str_lvalue);
  }
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolSetReal(char **name, int *type, double *value, int *ok)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_name = (char*)*name;
  BDat d;
  if(*type == 0) {
    d.PutValue(*value);
  } else if(*type == 2) {
    d.PutValue(BDat::PosInf());
  } else if(*type == 3) {
    d.PutValue(BDat::NegInf());
  } else { // type 1
    d.PutKnown(BUNKNOWN);
  }
  BSyntaxObject * so = new BContensDat(str_name, d, "");
  if (so) {
    BList* result = NIL;
    BList* aux = NIL;
    LstFastAppend(result, aux, so);
    R_concat(&R_stack, result);    
    *ok = 1;
  }
  else {
    *ok = 0;
  }
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolSetText(char **name, char **value, int *ok)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_name = (char*)*name;
  char* str_value = (char*)*value;
  BSyntaxObject * so = new BContensText(str_name, str_value, "");
  if (so) {
    BList* result = NIL;
    BList* aux = NIL;
    LstFastAppend(result, aux, so);
    R_concat(&R_stack, result);    
    *ok = 1;
  }
  else {
    *ok = 0;
  }
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolSetDate(char **name, int *type, double *value, int *ok)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_name = (char*)*name;
  BDate d;
  if(*type == 0) {
    d.PutIndex(*value);
  } else if(*type == 2) {
    d.PutIndex(219147); // End
  } else if(*type == 3) {
    d.PutIndex(-115782); // Begin
  } else { // type 1
    d.PutIndex(-300000); // Unknown
  }
  BSyntaxObject * so = new BContensDate(str_name, d, "");
  if (so) {
    BList* result = NIL;
    BList* aux = NIL;
    LstFastAppend(result, aux, so);
    R_concat(&R_stack, result);    
    *ok = 1;
  }
  else {
    *ok = 0;
  }
}

//----------------------------------------------------------------------------
extern "C" DLLEXPORT(void) RTolSetMatrix(char **name, int *rows, int *cols, 
  bool *mask, int *type, double *value, int *ok)
//----------------------------------------------------------------------------
{
  if(TOLHasBeenInitialized()==0) RInitTolKernel();
  char* str_name = (char*)*name;
  BMat d(*rows, *cols);
  for (int c=0; c< (*cols); ++c) {   
    for (int r=0; r< (*rows); ++r) {
      int p = c * (*rows) + r;
      if(*mask) {
        if(type[p] == 0) {
          d(r, c).PutValue(value[p]);
        } else if(type[p] == 2) {
          d(r, c).PutValue(BDat::PosInf());
        } else if(type[p] == 3) {
          d(r, c).PutValue(BDat::NegInf());
        } else { // type 1
          d(r, c).PutKnown(BUNKNOWN);
        }
      } else {
        d(r, c).PutValue(value[p]);
      }
    }
  }
  BSyntaxObject * so = new BContensMat(str_name, d, "");
  if (so) {
    BList* result = NIL;
    BList* aux = NIL;
    LstFastAppend(result, aux, so);
    R_concat(&R_stack, result);    
    *ok = 1;
  }
  else {
    *ok = 0;
  }
}

//////////////////////////////////////////////////////////////////////////////

