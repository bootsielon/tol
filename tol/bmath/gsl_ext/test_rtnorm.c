/* @(#)test_rtnorm.c
 *
 * gcc  -Wall test_rtnorm.c rtnorm.c -I/usr/local/gsl/include/ -L/usr/local/gsl/lib -lgsl -lgslcblas
 */

#include <stdio.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "gsl_ext.h"

int main(int argc, char * argv[])
{
 const gsl_rng_type * T;
 gsl_rng * r;
 int i, n = 100;
 double mu = 0.0;
 double sigma = 1.0;
 double lim_inf = -1;
 double lim_sup = 1;
 enum {below, above, both, unbounded} type_fun;
     
 /* create a generator chosen by the environment variable GSL_RNG_TYPE */

 gsl_rng_env_setup();
 T = gsl_rng_default;
 r = gsl_rng_alloc (T);

 /* print n random variates chosen from the normal truncated distribution with
  * mean parameter mu */
 
 if (argc > 1) {
   n = atoi(argv[1]);
   if (n<=0)
     n = 100;
 }
 type_fun = both;
 if (argc > 2) {
   if (!strcasecmp("-inf", argv[2])) {
     type_fun = above;
   } else {
     lim_inf = atof(argv[2]);
     type_fun = below;
   }
   if (argc > 3) {
     if (!strcasecmp("+inf", argv[3])) {
       if (type_fun == above)
         type_fun = unbounded;
     } else {
       lim_sup = atof(argv[3]);
       if (type_fun == below)
         type_fun = both;
     }
   }
 }
 
 printf("mu = %lf, sigma = %lf, below = %lf, above = %lf\n",
        mu, sigma, lim_inf, lim_sup);
 double x;
 
 for (i = 0;i < n; i++) {
   switch (type_fun) {
       case below:
         x = gsl_rtbnorm_combo(r, mu, sigma, lim_inf, 10);
         /* x = gsl_rtbnorm(r, mu, sigma, lim_inf);*/
         break;
       case above:
         x = gsl_rtanorm_combo(r, mu, sigma, lim_sup, 10);
         break;
       case both:
         x = gsl_rtnorm_combo(r, mu, sigma, lim_inf, lim_sup);
         break;
       default:
         x = mu + gsl_ran_gaussian(r,sigma);
   }
   printf ("%lf\n", x);
 }
 return 0; 
}



