/* errchk.c -- infix arithmetic expression error checking */
/* works by looking at the next character and determines 
 * if it's expected or not
 * also, translates unary operators to internal representation */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "errchk.h"

#define ERR_RETURN {err_code = 1; return err_code;}

static int err_code;

// see if the next character in the expression is correct
static int expect(const char * buff, const char * curr, const char * list);

/* --------------- MAIN CODE --------------- */
int errchk(char * expr)
{
	/* parse the expression in expr */
	char * crr_lx = expr;
	
	// must be zero in the end
	int par_count = 0;
	
	err_code = 0;
	while (*crr_lx != '\0')
	{
		switch (*crr_lx)
		{
			case '(':
				// ( expects digit | ( | + | -
				expect(expr, crr_lx, "d(+-");
				++par_count;
				break;
			case ')': 
				// ) expects not first | ) | operator
				expect(expr, crr_lx, "~)^*/+-");
				--par_count;
				break;
			// unary + expect digit
			// unary - expect digit | (
			// non-unary +- expect digit | ( | + | -
			case '+':
				// check if unary and replace
				if (crr_lx == expr || strchr("(^*/+-", *(crr_lx - 1)) )
				{
					expect(expr, crr_lx, "d");
					*crr_lx = UNARY_PLUS;
				}
				else
					expect(expr, crr_lx, "d(+-");
				break;
			case '-':
				// check if unary and replace
				if (crr_lx == expr || strchr("(^*/+-", *(crr_lx - 1)) )
				{
					expect(expr, crr_lx, "d(");
					*crr_lx = UNARY_MINUS;
				}	
				else
					expect(expr, crr_lx, "d(+-");
				break;
				// ^*/ expect not first | digit | ( | + | -
			case '*':
				expect(expr, crr_lx, "~d(+-");
				break;
			case '/':
				expect(expr, crr_lx, "~d(+-");
				break;
			case '^':
				expect(expr, crr_lx, "~d(+-");
				break;
			default:
				; 	/* prevents error: a label can only be part of a statement 
					/ and a declaration is not a statement */

				// get numbers
				if ( isdigit(*crr_lx) )
				{
					while (isdigit(*crr_lx) || '.' == *crr_lx)
					{
						if (isdigit(*crr_lx))
						{
							// digit expects digit| . | ) | op
							expect(expr, crr_lx, "d.)^*/+-");
						}
						else
						{
							// . expects not first | digit
							expect(expr, crr_lx, "~d");
							// expect not last
							expect(expr, crr_lx, "<");
						}
						++crr_lx;
					}
					// see end of loop
					--crr_lx;
				}
				else
				{
					fprintf(stderr, "Err: invalid character < %c >\n", *crr_lx);
					ERR_RETURN;
				}
				break;
		}
		// check if crr_lx is a valid last character
		expect(expr, crr_lx, "<");
		
		// on error go home
		if (err_code)
			return err_code;
			
		++crr_lx;
	}
	
	if (par_count != 0)
	{
		puts("Err: umatched parentheses");
		ERR_RETURN;
	}
	
	return err_code;
}

static int expect(const char * buff, const char * curr, const char * list)
{
	/* expect the next char to be containted in list
	 * if it's not, an error is reported
	 * 'd' means 'decimal digit', '~' means 'not first'
	 * '~' and 'd' must appear first in the list and '~' must be before 'd' 
	 * < means 'can't be last' and should appear in the list by itself */
	const char * list_start = list;
	
	if ('<' == *list_start)
	{
		// check for valid last character
		if ( ('\0' == *(curr + 1)) && (!isdigit(*curr)) && (*curr != ')') )
		{
			fprintf(stderr, "Err: unfinished expression; < %c > can't be last\n", *curr);
			ERR_RETURN;
		}
		return 0;
	}
	
	while ('~' == *list_start || 'd' == *list_start)
	{
		switch (*list_start)
		{
			case '~':
				// check for 'not first'
				if (curr == buff)
				{
					fprintf(stderr, "Err: < %c > can't begin an expression\n", *curr);
					ERR_RETURN;
				}
				break;
			case 'd':
				// check for digit
				if (!isdigit(*(curr + 1)) )
				{
					// if current character is not a digit, 
					// but it's still in the list it's fine
					if (strchr(list_start + 1, *(curr + 1)) != NULL)
						return 0;
					
					fprintf(stderr, "Err: a digit or one of '%s' expected instead of < %c >\n",
					list_start + 1,*(curr + 1));
					ERR_RETURN;
				}
				else
					return 0;
				break;
			default:
				break;
		}
		++list_start;
	}
	
	// check next character from the expression
	if (strchr(list_start, *(curr + 1)) == NULL)
	{
		fprintf(stderr, "Err: < %c > should be followed by one of '%s'\n", *curr, list_start);
		fprintf(stderr, "but it is instead followed by < %c >\n", *(curr + 1));
		ERR_RETURN;
	}
	return 0;
}