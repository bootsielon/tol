/* tol_ieee.h: Handling with IEEE representation
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

#include <gsl/gsl_math.h>
#include <float.h>
#include <limits>

//--------------------------------------------------------------------
   struct Bit32
// Structure to mask objects of 32 bits
//--------------------------------------------------------------------
{
  bool b00_:1;bool b01_:1;bool b02_:1;bool b03_:1;
  bool b04_:1;bool b05_:1;bool b06_:1;bool b07_:1;
  bool b08_:1;bool b09_:1;bool b10_:1;bool b11_:1;
  bool b12_:1;bool b13_:1;bool b14_:1;bool b15_:1;
  bool b16_:1;bool b17_:1;bool b18_:1;bool b19_:1;
  bool b20_:1;bool b21_:1;bool b22_:1;bool b23_:1;
  bool b24_:1;bool b25_:1;bool b26_:1;bool b27_:1;
  bool b28_:1;bool b29_:1;bool b30_:1;bool b31_:1;
};

//--------------------------------------------------------------------
  struct Bit64
//Structure to mask objects of 64 bits
//--------------------------------------------------------------------
{
  Bit32 a0_;
  Bit32 a1_;
};

//--------------------------------------------------------------------
  struct Byte8
//Structure to mask objects of 8 bytes
//--------------------------------------------------------------------
{
  unsigned char B0_;
  unsigned char B1_;
  unsigned char B2_;
  unsigned char B3_;
  unsigned char B4_;
  unsigned char B5_;
  unsigned char B6_;
  unsigned char B7_;
};


//--------------------------------------------------------------------
  BText GetBitsString(const double& x)
//--------------------------------------------------------------------
{
  const Bit64& b = *((const Bit64*)&x);
  BText txt;
  txt << b.a0_.b00_<< b.a0_.b01_<< b.a0_.b02_<< b.a0_.b03_
      << b.a0_.b04_<< b.a0_.b05_<< b.a0_.b06_<< b.a0_.b07_<<" "
      << b.a0_.b08_<< b.a0_.b09_<< b.a0_.b10_<< b.a0_.b11_
      << b.a0_.b12_<< b.a0_.b13_<< b.a0_.b14_<< b.a0_.b15_<<" "
      << b.a0_.b16_<< b.a0_.b17_<< b.a0_.b18_<< b.a0_.b19_
      << b.a0_.b20_<< b.a0_.b21_<< b.a0_.b22_<< b.a0_.b23_<<" "
      << b.a0_.b24_<< b.a0_.b25_<< b.a0_.b26_<< b.a0_.b27_
      << b.a0_.b28_<< b.a0_.b29_<< b.a0_.b30_<< b.a0_.b31_<<" "
      << b.a1_.b00_<< b.a1_.b01_<< b.a1_.b02_<< b.a1_.b03_
      << b.a1_.b04_<< b.a1_.b05_<< b.a1_.b06_<< b.a1_.b07_<<" "
      << b.a1_.b08_<< b.a1_.b09_<< b.a1_.b10_<< b.a1_.b11_
      << b.a1_.b12_<< b.a1_.b13_<< b.a1_.b14_<< b.a1_.b15_<<" "
      << b.a1_.b16_<< b.a1_.b17_<< b.a1_.b18_<< b.a1_.b19_
      << b.a1_.b20_<< b.a1_.b21_<< b.a1_.b22_<< b.a1_.b23_<<" "
      << b.a1_.b24_<< b.a1_.b25_<< b.a1_.b26_<< b.a1_.b27_
      << b.a1_.b28_<< b.a1_.b29_<< b.a1_.b30_<< b.a1_.b31_;
  return(txt);
}

//--------------------------------------------------------------------
bool PrintBitsAllSpecialDouble()
//--------------------------------------------------------------------
{
  double zero    =  0.0;
  double epsilon =  std::numeric_limits<double>::epsilon();
  double denrmin =  std::numeric_limits<double>::denorm_min();
  double posmin  =  std::numeric_limits<double>::min();
  double negmin  = -std::numeric_limits<double>::min();
  double posmax  =  std::numeric_limits<double>::max();
  double negmax  = -std::numeric_limits<double>::max();
  double posinf  =  std::numeric_limits<double>::infinity();
  double neginf  = -std::numeric_limits<double>::infinity();
  double nan     =  GSL_NAN;
  BText txt;
  Std(BText("ZERO    = ")+GetBitsString(zero   )+"\t"+txt.Copy(zero,   "%.16lg")+"\n");
  Std(BText("DENRMIN = ")+GetBitsString(denrmin)+"\t"+txt.Copy(denrmin,"%.16lg")+"\n");
  Std(BText("EPSILON = ")+GetBitsString(epsilon)+"\t"+txt.Copy(epsilon,"%.16lg")+"\n");
  Std(BText("POSMIN  = ")+GetBitsString(posmin )+"\t"+txt.Copy(posmin, "%.16lg")+"\n");
  Std(BText("NEGMIN  = ")+GetBitsString(negmin )+"\t"+txt.Copy(negmin, "%.16lg")+"\n");
  Std(BText("POSMAX  = ")+GetBitsString(posmax )+"\t"+txt.Copy(posmax, "%.16lg")+"\n");
  Std(BText("NEGMAX  = ")+GetBitsString(negmax )+"\t"+txt.Copy(negmax, "%.16lg")+"\n");
  Std(BText("POSINF  = ")+GetBitsString(posinf )+"\t"+txt.Copy(posinf, "%.16lg")+"\n");
  Std(BText("NEGINF  = ")+GetBitsString(neginf )+"\t"+txt.Copy(neginf, "%.16lg")+"\n");
  Std(BText("NAN     = ")+GetBitsString(nan    )+"\t"+txt.Copy(nan,    "%.16lg")+"\n");
  return(true);
}

/*
PrintBitsAllSpecialDouble: double IEEE for WIN 32 with Visual C++
     a_   00000000 00000000 00000000 00000000 11111111 11111111 11111111 11111111
     b_   00000000 00111111 11112222 22222233 00000000 00111111 11112222 22222233
          01234567 89012345 67890123 45678901 01234567 89012345 67890123 45678901

ZERO    = 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
DENRMIN = 10000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
EPSILON = 00000000 00000000 00000000 00000000 00000000 00000000 00001101 00111100
POSMIN  = 00000000 00000000 00000000 00000000 00000000 00000000 00001000 00000000
NEGMIN  = 00000000 00000000 00000000 00000000 00000000 00000000 00001000 00000001
POSMAX  = 11111111 11111111 11111111 11111111 11111111 11111111 11110111 11111110
NEGMAX  = 11111111 11111111 11111111 11111111 11111111 11111111 11110111 11111111
POSINF  = 00000000 00000000 00000000 00000000 00000000 00000000 00001111 11111110
NEGINF  = 00000000 00000000 00000000 00000000 00000000 00000000 00001111 11111111
NAN     = 00000000 00000000 00000000 00000000 00000000 00000000 00011111 11111111
*/
