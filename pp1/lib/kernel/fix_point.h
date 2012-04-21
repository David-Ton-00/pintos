#ifndef FIX_POINT_H
#define FIX_POINT_H

#include "lib/stdint.h"
#define FORMAT 14                               /* 17.14 format */

int n2fix(int n);                             /* convert a numeric to its fixd point format */
int fix2n_0(int fix);                         /* convert a numeric from its fixed point format to real value, rounded to 0 */
int fix2n_near(int fix);                      /* convert a numeric from its fixed point format to real value, rounded to nearest integer */
int add2fix(int x,int y);                     /* add two numerics in their fixed point format */
int subtract2fix(int x,int y);                /* subtract two numerics in their fixed point format: x-y */
int add_fix_n(int fix,int n);                 /* add a regular numeric with a fixed point format numeric; return fixed point format numeric */
int subtract_fix_n(int fix,int n);            /* subtract a regular numeric with a fixed point format numeric; return fixed point format numeric */ 
int multiply2fix(int x,int y);                /* multiply two numerics in their fixed point format */ 
int multiply_fix_n(int fix,int n);            /* multiply a regular numeric with a fixed point format numeric; return fixed point format numeric */
int divide2fix(int x,int y);                  /* a fixed point format numeric divided by another:x/y */
int divide_fix_by_n(int fix,int n);           /* a fixed point format numeric divided by a regular one */
int my_pow(int n,int expo);                   /* n**expo; n is a regular numeric */

#endif
