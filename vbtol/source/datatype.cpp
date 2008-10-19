#include <windows.h>
#include <string.h>

extern BSTR ChartoBSTR(char* str);

char module[101];
short initVar = 10;

// Simple Sub Call
void __declspec(dllexport) CALLBACK TestSub()
{
	GetModuleFileName(NULL,(LPTSTR)module,100);
	::MessageBox(NULL,module,"Location of the client",0);
}

// Receives a value and returns one.
short __declspec(dllexport) CALLBACK SimpleFunc(short value)
{
	return value * initVar;
}

//// Receives a double value and returns one.
double __declspec(dllexport) CALLBACK DoubleExample(double value)
{
	return value * initVar;
}

// Receives a value and returns one through an argument.
void __declspec(dllexport) CALLBACK FuncByRef(short value,short *returnVar)
{
	*returnVar = value * initVar;
}

// Receives a string argument and returns one.
BSTR __declspec(dllexport) CALLBACK StringExample(BSTR stringVar)
{
	LPSTR buffer;
	buffer = (LPSTR)stringVar; // The address is coerced into an LPSTR.
	::MessageBox(NULL,buffer,"in C++",0);
	buffer = _strrev(buffer);
	return(SysAllocString(stringVar)); // Since buffer has the address, the argument
									   // can still be returned.
}

//// Receives a string argument and returns a transformed one.
BSTR __declspec(dllexport) CALLBACK MyStringExample(BSTR stringVar)
{
	LPSTR buffer;

	buffer = (LPSTR)stringVar; // The address is coerced into an LPSTR.
	::MessageBox(NULL,buffer,"in C++",0);

	int size = strlen(buffer)*2+4;
    char * st = new char[size];
	strcpy_s(st, size, buffer);
	strcat_s(st, size, " ó ");
	strcat_s(st, size, buffer);

	BSTR str = SysAllocStringByteLen(st, strlen(st));
	delete[] st;
	return (str);
}

// Receives an array and returns one of its elements.
BSTR __declspec(dllexport) CALLBACK ArrayExample(LPSAFEARRAY FAR *data,short index)
{
	BSTR *tempArray;
//	index--;			// Arrays begin at zero in C/C++ and at one in Visual Basic.
	tempArray = (BSTR*)(*data)->pvData;
	return(SysAllocString(tempArray[index]));
}

// Receives an array of integer and returns one of its elements.
short __declspec(dllexport) CALLBACK MyArrayExample(LPSAFEARRAY FAR *data,short index)
{
	short *tempArray;
	tempArray = (short*)(*data)->pvData;
	return tempArray[index];
}

// Receives an array of double and returns one of its elements.
double __declspec(dllexport) CALLBACK My2ArrayExample(LPSAFEARRAY FAR *data,short index)
{
	double *tempArray;
	tempArray = (double*)(*data)->pvData;
	return tempArray[index];
}

// Receives an array of double and modifies it.
void __declspec(dllexport) CALLBACK MyModArrayExample(LPSAFEARRAY FAR *data)
{
	double *tempArray;
	tempArray = (double*)(*data)->pvData;
    for(int i=0; i<=5; ++i)
		tempArray[i] *= 10;
}

// Receives an array of double and return another.
short __declspec(dllexport) CALLBACK MyNewArrayExample(LPSAFEARRAY FAR *data)
{
	// Create a new 1-D array of Integers.
  SAFEARRAY * psaiNew; 
  SAFEARRAYBOUND aDim[1]; 
  aDim[0].lLbound = 1; 
  aDim[0].cElements = 8; 
  // Equivalent to: Dim aiNew(1 To 8) As Integer.
  psaiNew = SafeArrayCreate(/*VT_I2 - short*/ /*double - */VT_R8, 1, aDim);
  //  if (psaiNew == NULL) throw ERROR_NOT_ENOUGH_MEMORY;
  if (psaiNew == NULL)
	return 0;

  //  if (hres = SafeArrayDestroy(psaiNew)) throw hres;
  if (SafeArrayDestroy(*data))
	return 0;

  *data = psaiNew;

  // Use array.
  double * tempArray;
  tempArray = (double*)(*data)->pvData;
  for(int i=0; i<8; ++i)
	tempArray[i] = i+10.1;

  return 1;
}

// Receives a matrix of double and modifies it.
void __declspec(dllexport) CALLBACK MyMatrixExample(LPSAFEARRAY FAR *data)
{
  SAFEARRAYBOUND * aDims = (*data)->rgsabound;
  // Modify 2-D array with SafeArrayGetElement and SafeArrayGetElement.
  long ai[2];
  double iVal;
  int yMin = aDims[0].lLbound;
  int yMax = yMin + (int)aDims[0].cElements - 1;
  int xMin = aDims[1].lLbound; 
  int xMax = xMin + (int)aDims[1].cElements - 1;
  for (int x = xMin; x <= xMax; x++) {
    ai[0] = x;
    for (int y = yMin; y <= yMax; y++) {
      ai[1] = y;
      if (SafeArrayGetElement(*data, ai, &iVal))
		return;
      // Equivalent to: aiInOut(x, y) = aiInOut(x, y) + 1.
      iVal *= 10;
      iVal += 1000;
      if (SafeArrayPutElement(*data, ai, &iVal))
		return;
    }
  }
}

// Structure for the struc argument example.
struct testStruct
{
	short shortVar;
	unsigned char ucharVar;
	float floatVar;
	BSTR stringVar;
};

// Receives a structure and alters it.
void __declspec(dllexport) CALLBACK StructExample(testStruct *data)
{
	data->shortVar += 1234;
	data->ucharVar += 12;
	data->floatVar = (float)-3.4;
}

// Receives a structure, alters it and returns one of its elements.
BSTR __declspec(dllexport) CALLBACK MyStructExample(testStruct *data)
{
	BSTR tempArray;
	data->shortVar += 1234;
	data->ucharVar += 12;
	data->floatVar = (float)-3.4;
	tempArray = data->stringVar;
//	return SysAllocString(tempArray);
	int size = 8;
    char * st = new char[size];
	strcpy_s(st, size, "1234567");
	BSTR str = SysAllocStringByteLen(st, strlen(st));
	delete[] st;
	return (str);
}

// Receives a structure, incorporates it in a list and
// returns a concatenation of one of its fields.

typedef struct smytestStruct
{
	short shortVar;
	unsigned char ucharVar;
	float floatVar;
	BSTR stringVar;
    smytestStruct * next;
} mytestStruct;

typedef mytestStruct * pstruct;

pstruct head = NULL;

BSTR __declspec(dllexport) CALLBACK MyListExample(testStruct *data)
{
	pstruct p = new mytestStruct;

	LPSTR buffer;
	buffer = (LPSTR)data->stringVar;
	::MessageBox(NULL,buffer,"in C++",0);
	int size = strlen(buffer)+1;
    char * st = new char[size];
	strcpy_s(st, size, buffer);
	BSTR str = SysAllocStringByteLen(st, size);
	delete[] st;
	p->next = NULL;
    p->stringVar = str;
	p->shortVar = data->shortVar;
	p->ucharVar = data->ucharVar;
	p->floatVar = data->floatVar;
    if (head)
	  p->next = head;
	head = p;
 
    st = new char[100];
	st[0] = '\0';
	for (p=head; p; p=p->next) {
	  buffer = (LPSTR)p->stringVar;
	  ::MessageBox(NULL,buffer,"in C++",0);
	  strcat_s(st, 100, buffer);
  	  strcat_s(st, 100, " + ");
	}
	str = SysAllocStringByteLen(st, strlen(st));
	delete[] st;
	return (str);
}

// VARIANTS.
VARIANT __declspec(dllexport) CALLBACK VariantExample(VARIANT data)
{
	short i = data.iVal;
	VARIANT v;
	VariantInit(&v);
	v.vt = VT_R8;
	v.dblVal = i * 0.1;
	return v;
}
/*
// Receives the address of a Visual Basic function and calls it.
void __declspec(dllexport) CALLBACK CallbackExample(long Addr1)
{
	typedef void (__stdcall *FNPTR)(BSTR stringVar);
	FNPTR FunctionCall;
	FunctionCall = (FNPTR)Addr1;
//	BSTR temp = ChartoBSTR("hello");
	BSTR temp0 = BSTR("hello");
//	BSTR temp = SysAllocStringByteLen("hello", 6);
	BSTR temp = SysAllocString(temp0);

//    char * st = new char[10];
//	strcpy_s(st, 10, "hello");
//    ::MessageBox(NULL,st,"in C++",0);
//	BSTR temp = SysAllocStringByteLen(st, strlen(st));
//	delete[] st;
 
//	FunctionCall(SysAllocString(temp));
	FunctionCall(temp);
//	FunctionCall(SysAllocStringByteLen(st, strlen(st)));
//	FunctionCall(SysAllocString("hello"));
}
*/
/*
// Receives the address of a Visual Basic function and calls it.
void __declspec(dllexport) CALLBACK CallbackExample(long Addr1)
{
	typedef void (__stdcall *FNPTR)(double dVar);
	FNPTR FunctionCall;
	FunctionCall = (FNPTR)Addr1;
    double temp = 3.1415;
	FunctionCall(temp);
}
*/

// Receives the address of a Visual Basic function and calls it.

typedef void (__stdcall *FNPTR)(double dVar);
FNPTR FunctionCall;

void __declspec(dllexport) CALLBACK SetCallbackExample(long Addr1)
{
	FunctionCall = (FNPTR)Addr1;
}

void __declspec(dllexport) CALLBACK CallbackExample()
{
    static double temp = 3.1415;
	temp += 1;
	FunctionCall(temp);
}

typedef void (__stdcall *FN2PTR)(BSTR dVar);

FN2PTR Function2Call = NULL;

void __declspec(dllexport) CALLBACK Set2CallbackExample(long Addr1)
{
  Function2Call = (FN2PTR)Addr1;
}

void __declspec(dllexport) CALLBACK Call2backExample()
{
//	BSTR temp = SysAllocStringByteLen("hello", 6);
//    Function2Call(temp);

	char buffer[] = "Helló BSTR";
	::MessageBox(NULL,buffer,"ín C++",0);
	int size = strlen(buffer)*2+4;
    char * st = new char[size];
	strcpy_s(st, size, buffer);
	strcat_s(st, size, " + ");
	strcat_s(st, size, buffer);

//	BSTR temp = SysAllocStringByteLen(st, strlen(st));
//	p->stringVar = SysAllocStringByteLen(st, strlen(st));

    BSTR temp = ChartoBSTR(st);
	BSTR ret = SysAllocString(temp);
	Function2Call(ret);
	delete[] st;
	delete[] temp;
}

// Receives a currency argument and returns one.
__int64 __declspec(dllexport) CALLBACK CurrencyExample(__int64 c1,__int64 c2)
{
	return(c1 + c2);
}

#include <config.h>
#include <tol_init.h>
#include <tol_bgrammar.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bmatgra.h>

//---------------------------------------------------------------------------
short __declspec(dllexport) CALLBACK TolGetReal(BSTR lvalue, double * retvalue)
//---------------------------------------------------------------------------
{
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;

  BSyntaxObject * so = GraReal()->EvaluateExpr(str_lvalue);
  if (so) {
    BDat & d = Dat(so);
	*retvalue = d.Value();
    DESTROY(so);
	ret = 1;
  }
  return ret;
}

//---------------------------------------------------------------------------
short __declspec(dllexport) CALLBACK TolCreateReal(BSTR name, double value)
//---------------------------------------------------------------------------
{
  LPSTR str_name = (LPSTR)name;
  short ret = 0;

  new BContensDat(str_name, value, str_name);

/*  if (so) {
    BDat & d = Dat(so);
	*retvalue = d.Value();
    DESTROY(so);
	ret = 1;
  }*/
  return ret;
}

//---------------------------------------------------------------------------
short __declspec(dllexport) CALLBACK TolGetText(BSTR lvalue, BSTR * retvalue)
//---------------------------------------------------------------------------
{
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;

  BSyntaxObject * so = GraText()->EvaluateExpr(str_lvalue);
  if (so) {
    BText & d = Text(so);
    char * st = (char *)(d.String());

	SysFreeString(*retvalue);
	*retvalue = SysAllocStringByteLen(st, strlen(st));

	DESTROY(so);
	ret = 1;
  }
  return ret;
}

//---------------------------------------------------------------------------
short __declspec(dllexport) CALLBACK TolGetMatrix(BSTR lvalue, LPSAFEARRAY FAR * retvalue)
//---------------------------------------------------------------------------
{
  LPSTR str_lvalue = (LPSTR)lvalue;
  short ret = 0;

  if (SafeArrayDestroy(*retvalue))    // Free previous array
    return -1;
  BSyntaxObject * so = GraMatrix()->EvaluateExpr(str_lvalue);
  if (so) {
    BMat & d = Mat(so);
    BInt cols = d.Columns();
    BInt rows = d.Rows();

	// Create a new 2-D array of Doubles
    // Equivalent to: Dim aiNew(1 To rows, 1 To cols) As Double
    SAFEARRAY * psaiNew;
    SAFEARRAYBOUND aDims[2];
    aDims[0].lLbound = 1;
    aDims[0].cElements = cols;
    aDims[1].lLbound = 1;
    aDims[1].cElements = rows;
    psaiNew = SafeArrayCreate(VT_R8, 2, aDims);
	if (psaiNew==NULL) {                  // ERROR_NOT_ENOUGH_MEMORY
      DESTROY(so);
	  return -1;
	}

	// Use 2-D array with SafeArrayGetElement and SafeArrayGetElement
/*    long ai[2];
    double iVal;
    for (int x = 1; x <= cols; x++) {
      ai[0] = x;
      for (int y = 1; y <= rows; y++) {
        ai[1] = y;
        iVal = d(x-1,y-1).Value();
        assert (!SafeArrayPutElement(psaiNew, ai, &iVal));
      }
    }
*/

	// Use 2-D array with SafeArrayPtrOfIndex, SafeArrayLock and SafeArrayUnlock
    assert (!SafeArrayLock(psaiNew));
	long ai[2];
    double iVal;
	double * piInOut;
    for (int x = 1; x <= cols; x++) {
      ai[0] = x;
      for (int y = 1; y <= rows; y++) {
        ai[1] = y;
        iVal = d(x-1,y-1).Value();
        assert (!SafeArrayPtrOfIndex(psaiNew, ai, (void **)&piInOut));
		*piInOut = iVal;
      }
    }
	assert (!SafeArrayUnlock(psaiNew));
	
	*retvalue = psaiNew;                // Assing new array
    DESTROY(so);
	ret = 1;
  }
  return ret;
}
