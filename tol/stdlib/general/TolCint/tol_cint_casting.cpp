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

  Matrix(int rows, int columns, double* buffer)
  : rows_(rows),
    columns_(columns),
    buffer_(buffer)
  {}

 ~Matrix() {}

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

  Text(int length, char* buffer)
  : length_(length),
    buffer_(buffer)
  {}

 ~Text() {}

  double& operator() (int i) const
  {
    return(buffer_[i]);
  }
};
