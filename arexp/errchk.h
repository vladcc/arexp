/* errchk.h -- interface for errchk.c */

#ifndef ERRCHK_H_
#define ERRCHK_H_

#define UNARY_PLUS	' '
#define UNARY_MINUS	'u'

int errchk(char * expr);
/*
returns: 1 on error in the expression, 0 otherwise

description: Goes through expr and determines if it contains a valid 
infix expression. If the expression is not valid an error message is displayed.
*/
#endif
