/* -*- mode: C++ -*- */
//////////////////////////////////////////////////////////////////////////////
// FILE   : tol_cint_casting.cpp
// PURPOSE: Defines C/C++ casting for allowed TOL types 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
typedef double Real;
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
class Matrix
//////////////////////////////////////////////////////////////////////////////
{
public:
  int rows_;
  int columns_;
  double* buffer_;
  int deleteBuffer_;

  Matrix(int rows, int columns, double* buffer, int deleteBuffer=0)
  : rows_(rows),
    columns_(columns),
    buffer_(buffer),
    deleteBuffer_(deleteBuffer)
  {}

  Matrix(int rows, int columns)
  : rows_(0),
    columns_(0),
    buffer_(NULL),
    deleteBuffer_(false)
  {
    Alloc(rows,columns);
  }

 ~Matrix()
  {
    Free();
  }

  void Free()
  {
    if(deleteBuffer_ && buffer_) delete  [] buffer_;
    buffer_ = NULL;
    rows_ = 0;
    columns_ = 0;
    deleteBuffer_ = false;
  }

  void Alloc(int rows, int columns)
  {
    Free();
    rows_ = rows;
    columns_ = columns;
    buffer_ = new double[rows*columns)
    deleteBuffer_ = true;
  }

//Zero based vector access
  double& operator[] (int k) const
  {
    return(buffer_[k]);
  }
//Zero based matrix access
  double& operator[] (int i, int j) const
  {
    return(buffer_[i*columns_+j]);
  }
//One based vector access
  double& operator() (int k) const
  {
    return(buffer_[k]);
  }
//One based matrix access
  double& operator() (int i, int j) const
  {
    return(buffer_[(i-1)*columns_+j-1]);
  }
};

//////////////////////////////////////////////////////////////////////////////
class Text
//////////////////////////////////////////////////////////////////////////////
{
public:
  int length_;
  char* buffer_;
  int deleteBuffer_;

  Text(int length, char* buffer, int deleteBuffer=0)
  : length_(length),
    buffer_(buffer),
    deleteBuffer_(deleteBuffer)
  {}

  Text(char* buffer)
  : length_(strlen(buffer)),
    buffer_(buffer),
    deleteBuffer_(0)
  {}

 ~Text() 
  {
    Free();
  }

  void Free()
  {
    if(deleteBuffer_ && buffer_) delete  [] buffer_;
    length_ = 0;
    buffer_ = NULL;
    deleteBuffer_ = false;
  }

  void Alloc(int length)
  {
    Free();
    length_ = length;
    buffer_ = new char[length_+1)
    deleteBuffer_ = true;
  }

//Zero based vector access
  double& operator[] (int i) const
  {
    return(buffer_[i]);
  }
//One based vector access
  double& operator() (int i) const
  {
    return(buffer_[i-1]);
  }
//concat operations
  Text operator + (const Text& right)
  {
    int len = length_ + right.length_;
    char* buf = new char[len+1];
    sprintf(buf,"%s%s",buffer_,right.buffer);
    Text txt(len, buf, 1);
    return(txt);
  }
  Text operator + (const char* right)
  {
    int len = length_ + strlen(right);
    char* buf = new char[len+1];
    sprintf(buf,"%s%s",buffer_,right);
    Text txt(len, buf, 1);
    return(txt);
  }
  Text operator + (int num)
  {
    int len = length_ + 64;
    char* buf = new char[len+1];
    sprintf(buf,"%s%ld",buffer_,num);
    Text txt(len, buf, 1);
    return(txt);
  }
  Text operator + (double num)
  {
    int len = length_ + 64;
    char* buf = new char[len+1];
    sprintf(buf,"%s%.16lg",buffer_,n);
    Text txt(len, buf, 1);
    return(txt);
  }
};


