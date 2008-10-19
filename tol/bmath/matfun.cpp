/* matfun.cpp: Generic arithmetical functions
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif
#include <tol/tol_btext.h>
#include <tol/tol_bmatfun.h>
#include <tol/tol_bout.h>
#include <tol/tol_butil.h>
#include <tol/tol_brealfun.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_hyperg.h>

#define LIMITFactorial	100

//--------------------------------------------------------------------
BDat LogFactorial (BInt n)
//--------------------------------------------------------------------
{
    if(n<=LIMITFactorial)
    {
	return(Log(Factorial(n)));
    }
    else
    {
	return(LogGamma(n,0.1));
    }
}


//--------------------------------------------------------------------
BDat Factorial (BInt n)
//--------------------------------------------------------------------
{
  if(n<0) { return(BDat::Unknown()); }
  else if(n<=LIMITFactorial)
  {
    static BDat F[LIMITFactorial+1] =
    {
/*    0 */					   1.0,
/*    1 */					   1.0,
/*    2 */					   2.0,
/*    3 */					   6.0,
/*    4 */					  24.0,
/*    5 */					 120.0,
/*    6 */					 720.0,
/*    7 */					5040.0,
/*    8 */				       40320.0,
/*    9 */				      362880.0,
/*   10 */				     3628800.0,
/*   11 */				    39916800.0,
/*   12 */				   479001600.0,
/*   13 */				  6227020800.0,
/*   14 */				 87178291200.0,
/*   15 */			       1307674368000.0,
/*   16 */			      20922789888000.0,
/*   17 */			     355687428096000.0,
/*   18 */			    6402373705728000.0,
/*   19 */			  121645100408832000.0,
/*   20 */			 2432902008176640000.0,
/*   21 */			51090942171709440000.0,
/*   22 */		       1.12400072777760768e+21,
/*   23 */		      2.585201673888497821e+22,
/*   24 */			6.2044840173323941e+23,
/*   25 */		      1.551121004333098606e+25,
/*   26 */		      4.032914611266056503e+26,
/*   27 */		      1.088886945041835194e+28,
/*   28 */		      3.048883446117138367e+29,
/*   29 */		      8.841761993739700772e+30,
/*   30 */		      2.652528598121910322e+32,
/*   31 */		      8.222838654177893607e+33,
/*   32 */		      2.631308369336875779e+35,
/*   33 */		      8.683317618811870591e+36,
/*   34 */		      2.952327990395923703e+38,
/*   35 */		      1.033314796638614906e+40,
/*   36 */		       3.71993326789906284e+41,
/*   37 */		      1.376375309122635548e+43,
/*   38 */			5.2302261746660698e+44,
/*   39 */		      2.039788208119747201e+46,
/*   40 */		      8.159152832479056345e+47,
/*   41 */		      3.345252661316392738e+49,
/*   42 */		       1.40500611775287713e+51,
/*   43 */		       6.04152630633749719e+52,
/*   44 */		      2.658271574788457036e+54,
/*   45 */		      1.196222208654837384e+56,
/*   46 */			5.5026221598119888e+57,
/*   47 */		      2.586232415111792834e+59,
/*   48 */		      1.241391559253631516e+61,
/*   49 */		      6.082818640342559218e+62,
/*   50 */		      3.041409320171388418e+64,
/*   51 */		      1.551118753287408754e+66,
/*   52 */		      8.065817517094494241e+67,
/*   53 */		      4.274883284060000965e+69,
/*   54 */		      2.308436973392454005e+71,
/*   55 */		       1.26964033536590093e+73,
/*   56 */		      7.109985878048745287e+74,
/*   57 */		      4.052691950487695056e+76,
/*   58 */		      2.350561331282902355e+78,
/*   59 */		      1.386831185456881799e+80,
/*   60 */			8.3209871127416814e+81,
/*   61 */		      5.075802138772366982e+83,
/*   62 */		      3.146997326038658665e+85,
/*   63 */		      1.982608315404363448e+87,
/*   64 */		      1.268869321858806742e+89,
/*   65 */		      8.247650592082087955e+90,
/*   66 */		      5.443449390774301008e+92,
/*   67 */		      3.647111091818842385e+94,
/*   68 */		      2.480035542436828651e+96,
/*   69 */		      1.711224524281468522e+98,
/*   70 */		     1.197857166996925307e+100,
/*   71 */		      8.50478588567834326e+101,
/*   72 */		      6.12344583768858698e+103,
/*   73 */		      4.47011546151277096e+105,
/*   74 */		     3.307885441519178301e+107,
/*   75 */		     2.480914081139462378e+109,
/*   76 */		     1.885494701666068537e+111,
/*   77 */		     1.451830920282878201e+113,
/*   78 */		     1.132428117820668175e+115,
/*   79 */		     8.946182130781855842e+116,
/*   80 */		     7.156945704626677114e+118,
/*   81 */		     5.797126020747553615e+120,
/*   82 */		     4.753643337013109166e+122,
/*   83 */		     3.945523969720426282e+124,
/*   84 */		     3.314240134565287059e+126,
/*   85 */		     2.817104114380629782e+128,
/*   86 */		     2.422709538367189684e+130,
/*   87 */		     2.107757298379661191e+132,
/*   88 */		     1.854826422573944416e+134,
/*   89 */		     1.650795516090730856e+136,
/*   90 */		      1.48571596448172119e+138,
/*   91 */		     1.352001527678368224e+140,
/*   92 */		     1.243841405464182806e+142,
/*   93 */		     1.156772507081641219e+144,
/*   94 */		     1.087366156656718454e+146,
/*   95 */		    1.0329978488238504238e+148,
/*   96 */		     9.916779348709596418e+149,
/*   97 */		     9.619275968248377176e+151,
/*   98 */		     9.426890448882526029e+153,
/*   99 */		     9.332621544394403892e+155,
/*  100 */		     9.332621544394224865e+157,
    };
    return(F[n]);
  }
  else
  {
    return(Exp(LogFactorial(n)));
  }
}


//--------------------------------------------------------------------
BDat LogBiFactorial (BInt n)
//--------------------------------------------------------------------
{
    static BDat log2 = Log(2.0);
    if(n%2==0)
    {
	BInt m = n/2;
	return(BDat(m)*log2+LogFactorial(m));
    }
    else
    {
	return(LogFactorial(n)-LogBiFactorial(n-1));
    }
}


//--------------------------------------------------------------------
BDat BiFactorial (BInt n)
//--------------------------------------------------------------------
{
    return(Exp(LogBiFactorial(n)));
}


//--------------------------------------------------------------------
BDat LogCombinat  (BInt m, BInt n)
//--------------------------------------------------------------------
{
    if(m<n) { return(BDat::Unknown()); }
    else    { return(LogFactorial(m)-LogFactorial(n)-LogFactorial(m-n)); }
}


//--------------------------------------------------------------------
BDat Combinat	 (BInt m, BInt n)
//--------------------------------------------------------------------
{
    if(m<n) { return(BDat::Unknown()); }
    else    { return(Exp(LogCombinat(m,n))); }
}


//--------------------------------------------------------------------
BDat RiemmanZeta_1(BDat s, BDat tol)

/*! Returns the Riemman Zeta function minus one.
 *  Abramowitz, page 807, 23.2.1
 */
//--------------------------------------------------------------------
{
    if(s<=1) { return(BDat::Unknown()); }
    BDat add, sum = 0;
    BInt k=2;
    do
    {
	add = k^(-s);
	sum += add;
	k++;
    }
    while(add>tol);
//Std(BText("\n")+s.Name()+"\t"+sum.Name());
    return(sum);
}


//--------------------------------------------------------------------
BDat RiemmanZeta(BDat s, BDat tol)

/*! Returns the Riemman Zeta function.
 *  Abramowitz, page 807, 23.2.1
 */
//--------------------------------------------------------------------
{
    return(1+RiemmanZeta_1(s,tol));
}



//--------------------------------------------------------------------
BDat ReciprocalPowerSum(BInt m, BDat tol)

/*! Returns the sum of the reciprocal m-power of all natural numbers.
 *  Abramowitz, page 807, 23.2.1
 */
//--------------------------------------------------------------------
{
    return(1+ReciprocalPowerSum_1(m,tol));
}


#define LIMITReciprocalPowerSum	 100

//--------------------------------------------------------------------
BDat ReciprocalPowerSum_1(BInt m, BDat tol)

/*! Returns the sum of the reciprocal m-power of all natural numbers.
 *  Abramowitz, page 807, 23.2.1
 */
//--------------------------------------------------------------------
{
  if(m<=1) { return(BDat::Unknown()); }
  static BReal Z[LIMITReciprocalPowerSum+1] =
  {
/*    0 */					     0,
/*    1 */					     0,
/*    2 */			  0.644934066848226406,
/*    3 */			 0.2020569031595942922,
/*    4 */			0.08232323371113818566,
/*    5 */			0.03692775514336992665,
/*    6 */			0.01734306198444914016,
/*    7 */		       0.008349277381922827132,
/*    8 */		       0.004077356197944339601,
/*    9 */		       0.002008392826082214255,
/*   10 */		      0.0009945751278180852556,
/*   11 */		      0.0004941886041194645286,
/*   12 */		      0.0002460865533080483199,
/*   13 */		      0.0001227133475784891454,
/*   14 */		      6.124813505870480056e-05,
/*   15 */		      3.058823630702049327e-05,
/*   16 */		      1.528225940865186419e-05,
/*   17 */		       7.63719763789975918e-06,
/*   18 */		      3.817293264999840218e-06,
/*   19 */		      1.908212716553938548e-06,
/*   20 */		      9.539620338727960002e-07,
/*   21 */		      4.769329867878064468e-07,
/*   22 */		      2.384505027277329514e-07,
/*   23 */		      1.192199259653110637e-07,
/*   24 */		       5.96081890512594801e-08,
/*   25 */		      2.980350351465227928e-08,
/*   26 */		      1.490155482836503937e-08,
/*   27 */		      7.450711789835430061e-09,
/*   28 */		      3.725334024788457283e-09,
/*   29 */		      1.862659723513049556e-09,
/*   30 */		      9.313274324196681656e-10,
/*   31 */		      4.656629065033783658e-10,
/*   32 */		      2.328311833676505336e-10,
/*   33 */		      1.164155017270051931e-10,
/*   34 */		      5.820772087902700158e-11,
/*   35 */		      2.910385044497099359e-11,
/*   36 */		      1.455192189104198166e-11,
/*   37 */		      7.275959835057481796e-12,
/*   38 */		      3.637979547378650863e-12,
/*   39 */		      1.818989650307065667e-12,
/*   40 */		      9.094947840263890426e-13,
/*   41 */		      4.547473783042154218e-13,
/*   42 */		       2.27373684582465244e-13,
/*   43 */		      1.136868407680227914e-13,
/*   44 */		      5.684341987627585417e-14,
/*   45 */		      2.842170976889302004e-14,
/*   46 */		      1.421085482803160832e-14,
/*   47 */		      7.105427395210852706e-15,
/*   48 */		      3.552713691337113934e-15,
/*   49 */		      1.776356843579120414e-15,
/*   50 */		      8.881784210930816193e-16,
/*   51 */		      4.440892103143813132e-16,
/*   52 */		      2.220446050798041907e-16,
/*   53 */		      1.110223025141066149e-16,
/*   54 */		      5.551115124845480986e-17,
/*   55 */		      2.775557562136123907e-17,
/*   56 */		      1.387778780972523194e-17,
/*   57 */		       6.93889390454415344e-18,
/*   58 */		      3.469446952165922543e-18,
/*   59 */		      1.734723476047576546e-18,
/*   60 */		      8.673617380119933001e-19,
/*   61 */		      4.336808690020650567e-19,
/*   62 */		      2.168404344997219812e-19,
/*   63 */		      1.084202172494241415e-19,
/*   64 */		      5.421010862456645842e-20,
/*   65 */		      2.710505431223468976e-20,
/*   66 */		      1.355252715610116406e-20,
/*   67 */		      6.776263578045189425e-21,
/*   68 */		      3.388131789020796676e-21,
/*   69 */		      1.694065894509799118e-21,
/*   70 */		       8.47032947254699819e-22,
/*   71 */		      4.235164736272833295e-22,
/*   72 */		      2.117582368136194714e-22,
/*   73 */		      1.058791184068023301e-22,
/*   74 */		      5.293955920339870826e-23,
/*   75 */		      2.646977960169853129e-23,
/*   76 */		       1.32348898008489894e-23,
/*   77 */			6.6174449004244036e-24,
/*   78 */		      3.308722450212171678e-24,
/*   79 */		      1.654361225106075554e-24,
/*   80 */		      8.271806125530344708e-25,
/*   81 */		      4.135903062765161333e-25,
/*   82 */		      2.067951531382576534e-25,
/*   83 */		      1.033975765691287119e-25,
/*   84 */		      5.169878828456431004e-26,
/*   85 */		      2.584939414228214354e-26,
/*   86 */		      1.292469707114106603e-26,
/*   87 */		       6.46234853557053158e-27,
/*   88 */		      3.231174267785265072e-27,
/*   89 */		      1.615587133892632536e-27,
/*   90 */		      8.077935669463162681e-28,
/*   91 */		      4.038967834731580444e-28,
/*   92 */		      2.019483917365790222e-28,
/*   93 */		      1.009741958682895111e-28,
/*   94 */		      5.048709793414475554e-29,
/*   95 */		      2.524354896707237777e-29,
/*   96 */		      1.262177448353618889e-29,
/*   97 */		      6.310887241768094444e-30,
/*   98 */		      3.155443620884047222e-30,
/*   99 */		      1.577721810442023611e-30,
/*  100 */		      7.888609052210118054e-31
  };
  if(m<=LIMITReciprocalPowerSum)
  {
    return(Z[m]);
  }
  else
  {
    return(RiemmanZeta_1(m,tol));
  }
}


//--------------------------------------------------------------------
BDat LogAbsBernouilli0(BInt m, BDat tol)

/*! Returns the Logarithm of absolute value of the m-th Bernouilli number.
 *  Abramowitz, page 807, 23.2.1
 */
//--------------------------------------------------------------------
{
    BDat lb;
    if((m>=0) && !(m%2))
    {
	/*
	  static BDat l2   = Log(2);
	  static BDat lpi2 = Log(2*PI);
	*/
	static BDat l2   = M_LN2;
	static BDat lpi2 = M_LN2 + M_LNPI;
	lb = l2+LogFactorial(m)-m*lpi2+Log(ReciprocalPowerSum(m,tol));
    }
    return(lb);
}


//--------------------------------------------------------------------
BDat Bernouilli0(BInt m, BDat tol)

/*! Returns the m-th Bernouilli number.
 */
//--------------------------------------------------------------------
{
    BDat b = 0;
    if((m>=0) && !(m%2))
    {
	BInt s = (m%4)?1:-1;
	b = Exp(LogAbsBernouilli0(m,tol))*s;
    }
    return(b);
}

/*
//--------------------------------------------------------------------
   BDat RecGamma(BDat z, BDat tol)

// PURPOSE: Returns the Euler's Gamma function.
//	    (Handbook of mathematical functions, Milton Abramowitz) page 257
//
//--------------------------------------------------------------------
{
  if(z>0)	   { return(Exp(-LogGamma(z,tol))); }
  if(z==Floor(z))  { return(0); }
  z-=1;
  static BDat c_[27] =
  {
     0.0,
     1.0000000000000000,
     0.5772156649015329,
    -0.6558780715202538,
    -0.0420026350340952,
     0.1665386113822915,
    -0.0421977345555443,
    -0.0096219715278770,
     0.0072189432466630,
    -0.0011651675918591,
    -0.0002152416741149,
     0.0001280502823882,
    -0.0000201348547807,
    -0.0000012504934821,
     0.0000011330272320,
    -0.0000002056338417,
     0.0000000061160950,
     0.0000000050020075,
    -0.0000000011812746,
     0.0000000001043427,
     0.0000000000077823,
    -0.0000000000036968,
     0.0000000000005100,
    -0.0000000000000206,
    -0.0000000000000054,
     0.0000000000000014,
     0.0000000000000001
  };
  BDat g = 0;
  BDat zz = 1;
  for(BInt i=1; i<=26; i++)
  {
    g += c_[i]*zz;
//  Std(BText("\n")+i+ "\t" + zz.Name() + "\t" + g.Name() );
    if(i<26) { zz *= z; }
  }
  return(g);
}
*/


//--------------------------------------------------------------------
static BDat LogGammaAbsLT2(BDat z, BDat tol)

/*! Returns the neperian logarithm of Euler's Gamma function
 *  when  0 < z < 2
 *  Abramowitz, page 256, 6.1.33
 */
//--------------------------------------------------------------------
{
    z-=1;
    if(Abs(z)>=2) { return(BDat::Unknown()); }
    BDat sum = -Log(1+z)+
	z*(1.0-EULER);
    BDat add = tol;
    BInt s = 1;
    for(BInt n=2; (Abs(add)>=tol); n++)
    {
	add = s*((ReciprocalPowerSum(n,tol)-1)*(z^n))/n;
	sum += add;
	s*=-1;
    }
    return(sum);
}


//--------------------------------------------------------------------
static BDat LogGammaGE10(BDat z, BDat tol)

/*! Returns the neperian logarithm of Euler's Gamma function
 *  when z is great
 *
 *  Abramowitz, page 257, 6.1.39
 */
//--------------------------------------------------------------------
{
    //  static BDat L2PI = 0.5*Log(2.0*PI);
    static BDat L2PI = 0.5 * (M_LN2 + M_LNPI);
    BDat sum = (z-0.5)*Log(z)+L2PI-z;
    tol*=0.1;
    BDat add = tol;
    for(BInt m=2; (m<=14)||(Abs(add)>=tol); m+=2)
    {
	add =   Bernouilli0(m);
	add /= (m-1)*m*(z^(m-1));
	sum += add;
//  Std(BText("\n m = ")+m+"\t add = "+add.Name()+"\t loggamma = "+sum.Name());
    }
    return(sum);
    
/*
  static BDat L2PI = 0.5*Log(2*PI);
  static BDat a_[7] =
  {
	1.0000000 /    12.0000000,
	1.0000000 /    30.0000000,
       53.0000000 /   210.0000000,
      195.0000000 /   371.0000000,
    22999.0000000 / 22737.0000000,
    29944.5230000 / 19733.1420000,
    10953.5241009 / 48264.2754620
  };
  BDat Stirling = (z-0.5)*Log(z)+L2PI-z;
  BDat lng = 0;
  for(BInt i=6; i>=0; i--) { lng = a_[i]/(z+lng); }
  return(Stirling+lng);
*/
}

#define LIMITGamma  31

//--------------------------------------------------------------------
BDat LogGamma(BDat z, BDat tol)

/*! Returns the neperian logarithm of Euler's Gamma function.
 *
 *  Abramowitz, page 256
 */
//--------------------------------------------------------------------
{
    if(z<=0) 	       { return(BDat::Unknown()); }
    if((z==1)||(z==2)) { return(0); }
    if(z<= 2)	       { return(LogGammaAbsLT2(z,tol)); }
    if(z<=LIMITGamma ) { return(Log(Gamma(z,tol))); }
    return(LogGammaGE10(z,tol));
    
}


//--------------------------------------------------------------------
BDat LogAbsRecGamma(BDat z, BDat tol)

/*! Returns the reciprocal of the Euler's Gamma function.
 *
 *  Abramowitz, page 256
 */
//--------------------------------------------------------------------
{
    if(z==Floor(z))
    {
	if(z<=0)      { return(BDat::Unknown()); }
	else if(z<=2) { return(0); }
    }
    if (z>0)  { return(-LogGamma(z,tol)); }
    { return( LogGamma(1-z,tol)+Log(Abs(Sin(M_PI*z))/M_PI)); }
}


//--------------------------------------------------------------------
BDat SignRecGamma(BDat z, BDat tol)
       
/*! Returns the reciprocal of the Euler's Gamma function.
 *
 *  Abramowitz, page 256
 */
//--------------------------------------------------------------------
{
    if(z>0)  { return(1); }
    BInt fz = (BInt)Floor(z).Value();
    BDat rz = z-Floor(z);
    if(rz==0){ return( 0); }
    if(fz%2) { return(-1); }
    else     { return( 1); }
}


//--------------------------------------------------------------------
BDat RecGamma(BDat z, BDat tol)

/*! Returns the reciprocal of the Euler's Gamma function.
 *
 *  Abramowitz, page 256
 */
//--------------------------------------------------------------------
{
    if(z==Floor(z))
    {
	if(z<=0)      { return(0); }
	else if(z<=2) { return(1); }
    }
    if (z>0)   { return(1/Gamma(z,tol)); }
    { return(Gamma(1-z,tol)*Sin(M_PI*z)/M_PI); }
}


//--------------------------------------------------------------------
BDat Gamma(BDat z, BDat tol)

/*! Returns the Euler's Gamma function.
 *
 *  Abramowitz, page 256
 */
//--------------------------------------------------------------------
{
    if(z==Floor(z))
    {
	if(z<=0)         { return(BDat::Unknown()); }
	else if(z<=2)    { return(1); }
    }
    if (z<0)	       { return(1/RecGamma(z,tol)); }
    if (z<=2)	       { return(Exp(LogGamma(z,tol))); }
    if (z<=LIMITGamma) { return((z-1)*Gamma(z-1,tol/2)); }
    { return(Exp(LogGamma(z,tol))); }
}


//--------------------------------------------------------------------
BDat LogBeta(BDat m, BDat n, BDat tol)

/*! Returns the neperian logarithm of Euler's Beta fucntion.
 */
//--------------------------------------------------------------------
{
    return(LogGamma(m,tol)+LogGamma(n,tol)-LogGamma(m+n,tol));
}


//--------------------------------------------------------------------
BDat Beta(BDat m, BDat n, BDat tol)

/*! Returns the Euler's Beta function.
 *  B(a, b) = Ganma(a)*Ganma(b)/Ganma(a+b) for a > 0 & b > 0 
 */
//--------------------------------------------------------------------
{
    return(Exp(LogBeta(m,n)));
}


//--------------------------------------------------------------------
BDat FHyp(BDat a, BDat b, BDat c, BDat z, BDat tol)

/*! Returns the Hypergeometric F function
 *
 *  Abramowitz, page 556
 */
//--------------------------------------------------------------------
{
    return gsl_sf_hyperg_2F1(a.Value(),b.Value(),c.Value(),z.Value());

  //  return(hyp2f1(a.Value(),b.Value(),c.Value(),z.Value()));

/*
  if(z<0) { return(((1-z)^(-a))*FHyp(a,c-b,c,z/(z-1))); }
  BInt n;
  BDat ad=1, add=tol, sum=1;
  BDat ga,gb,gc,lz;
  BBool gamma = BFALSE;
  for(n=1; sum.IsKnown() && (add>=tol); n++)
  {
    BDat oldSum = sum;
    BDat an = a+n-1;
    BDat bn = b+n-1;
    BDat cn = c+n-1;
    if(Minimum(Minimum(an,bn),cn)<LIMITGamma)
    { ad *= (an*bn*z)/(cn*n); }
    else
    {
      if(!gamma)
      {
	gamma = BTRUE;
	ga = LogGamma(a,tol);
	gb = LogGamma(b,tol);
	gc = LogGamma(c,tol);
	lz = Log(z);
      }
      ad = Exp((LogGamma(an,tol)+LogGamma(bn,tol)+gc+n*lz)-
	       (LogGamma(cn,tol)+ga+gb+LogFactorial(n)));
    }
    sum += ad;
    add = Abs(sum-oldSum);
  }
  return(sum);
*/
}


//--------------------------------------------------------------------
static BDat MCHypSmall(BDat a, BDat b, BDat z, BDat tol)

/*! Returns the Confluent Hypergeometric M function for |z|<=10
 *
 *  Abramowitz, page 504
 */
//--------------------------------------------------------------------
{
    BInt n;
    tol = Minimum(tol,BDat::Tolerance());
    BDat ad=1, add=tol, sum=1;
//Std(BText("\na=")+a.Name()+"\tb="+b.Name()+"\tz="+z.Name());
    BDat N = Maximum(Abs(a),Abs(b));
    for(n=1; (n<N.Value())||(sum.IsKnown() && (add>=tol)); n++)
    {
	ad *= ((a+n-1)*z)/((b+n-1)*n);
	BDat oldSum = sum;
	sum += ad;
	add = Abs(sum-oldSum);
//  Std(BText("\n=")+n+"\tad="+ad.Name()+"\tMsum="+sum.Name());
    }
    //Std("\n");
    return(sum);
}


//--------------------------------------------------------------------
static BDat MCHypLarge(BDat a, BDat b, BDat z, BDat tol)

/*! Returns the Confluent Hypergeometric M function for |z|>10
 *
 *  Abramowitz, page 504
 */
//--------------------------------------------------------------------
{
    BInt n;
    BDat c   = b-a;
    BDat rb  = RecGamma  (b,tol);
    BDat ra  = RecGamma  (a,tol);
    BDat rc  = RecGamma  (c,tol);
    BDat R0  = Cos(M_PI*a)*Sign(z)*Exp(-a*Log(Abs(z)))*rc/rb;
    BDat S0  = Exp(z)   *Sign(z)*Exp(-c*Log(Abs(z)))*ra/rb;
    BDat R   = R0;
    BDat S   = S0;
    BDat add=tol, sum=R0+S0;
//Std(BText("\na=")+a.Name()+"\tb="+b.Name()+"\tz="+z.Name());
    BDat N = Maximum(Abs(a),Abs(b));
    for(n=1; (n<N.Value())||(sum.IsKnown() && (add>=tol)); n++)
    {
	BDat oldSum = sum;
	if(R!=0) { R *= (a+n-1)*(n-c)/(-z*n); sum += R; }
	if(S!=0) { S *= (c+n-1)*(n-a)/( z*n); sum += S; }
	add = Abs(sum-oldSum);
//  Std(BText("\n=")+n+"\tR="+R.Name()+"\tS="+S.Name()+"\tMsum="+sum.Name());
    }
//Std("\n");
    return(sum);
}

/*
//--------------------------------------------------------------------
   static BDat MCHyp_ab(BDat a, BDat b, BDat z, BDat tol)

// PURPOSE: Returns the Confluent Hypergeometric M function for a,b>0
//
//	    Abramowitz, page 505
//
//--------------------------------------------------------------------
{
  class derFunction : public BRRFunction
  {
  private :
    BDat a_, b_, z_;
  public :
     derFunction(BDat a, BDat b, BDat z)
    : BRRFunction(), a_(a), b_(b), z_(z) { }
    void Evaluate(BDat& y, const BDat& u)
    {
      y = Exp(z_/u)*(u^(-b_))*((u-1)^(b_-a_-1));
    }
  };
  derFunction f(a,b,z);
  return(f.AdaptiveSimpson(1,z/Log(tol*0.01),tol*0.1));
}
  if((a >0)&&(b >0)) { return(MCHyp_ab(a,b,z,tol)); }
  if((a <0)&&(b >0)) { return(Exp(z)*MCHyp_ab(b-a,b,-z,tol)); }
*/


//--------------------------------------------------------------------
BDat MCHyp(BDat a, BDat b, BDat z, BDat tol)

/*! Returns the Confluent Hypergeometric M function.
 *
 *  Abramowitz, page 504
 */
//--------------------------------------------------------------------
{
    return gsl_sf_hyperg_1F1(a.Value(),b.Value(),z.Value());
  //  return(hyperg(a.Value(),b.Value(),z.Value()));
/*
  if((b<=0)&&(b==Floor(b))) { return(BDat::Unknown()); }
//  if((z==0)||(a==0)) { return(1); }
//  M = Exp(z)*MCHyp(b-a,b,-z,tol);
  BDat M;

//if((a<30)&&(z> 20))  {  M = Exp(z)*MCHyp(b-a,b,-z,tol); }
//else
  if(Abs(z)>=100)  { M = MCHypLarge(a,b,z,tol); }
  else		   { M = MCHypSmall(a,b,z,tol); }

//Std(BText("\na=")+a.Name()+"\tb="+b.Name()+"\tz="+z.Name()+"\tM="+M.Name());
  return(M);
*/
}



//--------------------------------------------------------------------
static BDat UCHypSmall(BDat a, BDat b, BDat z, BDat tol)

/*! Returns the Confluent Hypergeometric U function for |z|<=10
 *
 *  Abramowitz, page 504
 */
//--------------------------------------------------------------------
{
    BDat M1 = MCHyp(a,b,z,tol*0.1)/
	      (Gamma(1.0+a-b,tol)*Gamma(b,tol));
    BDat M2 = MCHyp(1.0+a-b,2.0-b,z,tol*0.1)/
              (Gamma(2.0-b,tol)*Gamma(a,tol));
    return(M_PI*(M1-(z^(1.0-b))*M2)/Sin(M_PI*b));
}



//--------------------------------------------------------------------
static BDat UCHypLarge(BDat a, BDat b, BDat z, BDat tol)

/*! Returns the Confluent Hypergeometric U function for |z|>10
 *
 *  Abramowitz, page 504
 */
//--------------------------------------------------------------------
{
    BInt n;
    BDat ad0 = z^(-a);
    BDat ad=ad0, add=tol, sum=ad;
    for(n=1; sum.IsKnown() && (add>=tol); n++)
    {
	ad *= (a+n-1.0)*(a-b+n)/(-z*n);
	BDat oldSum = sum;
	sum += ad;
	add = Abs(sum-oldSum);
//    Std(BText("\n=")+n+"\tad="+ad.Name()+"\tUsum="+sum.Name());
    }
    return(sum);
}



//--------------------------------------------------------------------
BDat UCHyp(BDat a, BDat b, BDat z, BDat tol)

/*! Returns the Confluent Hypergeometric U function.
 *
 *    Abramowitz, page 504
 */
//--------------------------------------------------------------------
{
    BDat U;
    if((a>50)&&(z>0))
    {
	BDat za = z^(-a);
	if(Abs(za)<tol*0.1/z) { return(za); }
	BDat c = 2*b-4*a;
	BDat fi = z/c;
	if(c>z)
	{
	    BDat cf = Cos(fi);
	    BDat sf = Sin(2*fi);
	    BDat U1 = Exp(0.5*c*cf*cf)*((0.5*c*cf)^(1-b))/Sqrt((0.5*b-a)*sf);
	    if(Abs(U1)<tol*0.1/(b-a))
	    {
		return(U1*Sin((0.5*b-a)*(2*fi-sf)+M_PI_4));
	    }
	}
    }
    if(Abs(z)<=10) { U = UCHypSmall(a,b,z,tol); }
    else	   { U = UCHypLarge(a,b,z,tol); }
//  Std(BText("\na=")+a.Name()+"\tb="+b.Name()+"\tz="+z.Name()+"\tU="+U.Name());
    return(U);
}



//--------------------------------------------------------------------
BDat UParCyl(BDat a, BDat z, BDat tol)

/*! Returns the Parabolic Cylinder U function.
 *
 *	    Abramowitz, page 504
 */
//--------------------------------------------------------------------
{
    BDat v = 0.5*a+0.25;
    BDat z2 = z*z;
    return(Exp(-0.25*z2-v*Log(2))*UCHyp(v,0.5,0.5*z2,tol));
}



//--------------------------------------------------------------------
BDat DParCyl(BDat a, BDat z, BDat tol)

/*! Returns the Parabolic Cylinder D function.
 *
 *	    Abramowitz, page 504
 */
//--------------------------------------------------------------------
{
    return(UParCyl(-(0.5+a),z,tol));
}



//--------------------------------------------------------------------
BDat Hh(BInt n, BDat z, BDat tol)

/*! Returns n-th repeated integral of the error function
 *
 *  Abramowitz, page 299
 */
//--------------------------------------------------------------------
{
    if(n==-1) { return(Exp(-0.5*z*z)); }
    else      { return(Exp(-0.25*z*z)*UParCyl(n+0.5,z,tol)); }
}


//--------------------------------------------------------------------
BDat Erfc(BInt n, BDat z, BDat tol)

/*! Returns n-th repeated integral of the error function
 *
 *  Abramowitz, page 299
 */
//--------------------------------------------------------------------
{
    return(Exp(-0.5*z*z)*DParCyl(-n-1,z*Sqrt(2),tol)/
	   Sqrt(M_PI*(BDat(2)^(n-1))));
}


//--------------------------------------------------------------------
BDat LogStirling(const BDat& x)
//--------------------------------------------------------------------
{
  // static BDat log2pi = Log(2.0*PI);
    static BDat log2pi = M_LN2 + M_LNPI;
    if(x<=1) { return(0); }
    BDat logx = Log(x);
    return( (log2pi+logx)/2.0 + x*(logx-1.0));
};

//--------------------------------------------------------------------
BDat InvFactorial(const BDat& x)
      
/*! Returns the great number n that n! <= x
 */
//--------------------------------------------------------------------
{
    if(x< 2) { return(1); }
    if(x< 6) { return(2); }
    if(x<24) { return(3); }
    BRRFunction logStirling;
    logStirling.PutEval(LogStirling);
    return((BInt)logStirling.NewtonSolve(Log(x),Log(x)).Value());
};


