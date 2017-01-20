/* eval.h -- interface for eval.c */

#ifndef EVAL_H_
#define EVAL_H_

// the decimal precision printed to the screen
extern int f_prec;

double calculate(char * expr);
/* 
returns: the result of expr if expr contains a valid infix expression

description: evaluates an infix expression
*/
#endif
