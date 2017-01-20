/* arexp.c -- arithmetic expression calculator */
/* reads options, intermediate operators, and expression strings
 * note: 'e' is translated to ^ because of cmd */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "errchk.h"
#include "eval.h"

// option flags
#define HELP		'h'
#define ECHO		'o'
#define F_PREC		'p'
#define VER			'v'
#define EXAMPLE		'x'

// value indicating no argument was read from the string
#define NO_ARG		-1

// decimal precision values
#define MIN_PREC	0
#define MAX_PREC	10
#define PREC_ERR	-2

// the comment character; everything else after it is ignored
#define COMMENT		'#'

// print macros
#define PROMPT 		printf("\r?> ")
#define PRINT_RSLT	printf("result: %.*f\n", f_prec, curr_result)
#define PRINT_VER	printf("%s %s\n", prog_name, prog_ver)

// value indicating there's no intermediate operator
#define NO_OP 		0

// zero out the current result
#define ZERO_OUT	'c'

// this gets translated to '^'
#define EXPON_OP	'e'

// quits interactive mode
#define QUIT		'q'

// the expression buffer size
#define BUFF_SIZE 	1023

// text buffer containing the expression
static char expr_buff[BUFF_SIZE + 1];

// program info
static char * prog_name = "arexp";
static char * prog_ver = "v1.0";

// default values
int f_prec = 2; // see eval.h
static bool echo = false;

static int handle_arg(const char * arg);
static int get_string(void);
static void print_help(void);
static void print_example(void);

/* --------------- MAIN CODE --------------- */
int main(int argc, char * argv[])
{
	/* read arguments, intermediate operators, 
	 * expression, check for errors, send for evaluation */
	
	// parse args in non-interactive mode
	for (++argv; *argv != NULL; ++argv, --argc)
	{
		switch (handle_arg(*argv))
		{
			case HELP:
			case EXAMPLE:
			case VER:
				return 0;
				break;
			case PREC_ERR:
				return -1;
				break;
			case ECHO:
			case F_PREC:
				break;
			default:
				goto out;
				break;
		}
	}
	
	out:
	if (argc > 1)
	{
		// get arguments in the expression buffer
		double curr_result = 0.0;
		int i, ch, j;
		
		j = 0;
		while (*(argv) != NULL)
		{
			for (i = 0; (ch = (*argv)[i]) != '\0' && j < BUFF_SIZE; ++i)
			{
				// translate exponent operator 
				if (EXPON_OP == ch)
					ch = '^';
				// disregard spaces
				if (!isspace(ch))
					expr_buff[j++] = ch;
			}	
			++argv;
		}
		// terminate string
		expr_buff[j] = '\0';	
		
		// print
		puts(expr_buff);
		
		// check for errors
		if (errchk(expr_buff) != 0)
			return 1;
		
		// evaluate
		curr_result = calculate(expr_buff);
		PRINT_RSLT;
	}
	else
	{
		// interactive use; get input from stdin
		int str_ret, op;
		char * expr_start;
		double curr_result, prev_result;
		
		// op is intermediate operator
		op = NO_OP; 
		prev_result = curr_result = 0.0;
		puts("Enter expression (q to quit):");
		PRINT_RSLT;
		while (true)
		{
			str_ret = get_string();
			
			// check for empty string
			if ('\0' == *expr_buff)
				continue;
			
			// check if a switch was entered
			if (handle_arg(expr_buff) != NO_ARG)
				continue;
			
			// break on quit command or EOF
			if (str_ret < 0)
				break;
			// exit on buffer overflow
			else if (str_ret > 0)
				return -1;
			
			expr_start = expr_buff;
			// check for intermediate operator
			if (strchr("ce*^/+-", *expr_start) != NULL)
			{
				op = *expr_start;
				if (ZERO_OUT == op)
				{
					prev_result = curr_result = 0.0;
					PRINT_RSLT;
					op = NO_OP;
					continue;
				}
				// move the start of the expression past the operator
				++expr_start;
				// don't go further if only an op was entered
				if ('\0' == *expr_start)
					continue;
			}
			
			// error check past the operator if any
			if (errchk(expr_start) != 0)
				continue;
			
			// evaluate
			curr_result = calculate(expr_start);
			// the current result is the result of the expression
			
			if (op != NO_OP)
			{
				// place '<previous result> op <current result>' in the buffer
				sprintf(expr_buff, "%.*f%c%.*f", 
				f_prec, prev_result, op, f_prec, curr_result);
				
				if (errchk(expr_buff) != 0)
					continue;
				
				// evaluate
				curr_result = calculate(expr_buff);
				// the current result is the result of '<previous result> op <current result>'
				
				op = NO_OP;
			}
			prev_result = curr_result;
			PRINT_RSLT;
		}
		puts("Goodbye!");
	}	
	return 0;
}

static int handle_arg(const char * arg)
{
	/* argument handling */
	int ret;
	
	// check for dash
	if (*arg != '-')
		return NO_ARG;
	
	// read next character
	ret = *++arg;
	switch (*arg)
	{
		case ECHO:
			if (echo)
			{
				echo = false;
				printf("Echo is now off\n");
			}
			else
			{
				echo = true;
				printf("Echo is now on\n");
			}
			break;
		case F_PREC:
			if (!isdigit((*(arg+1))) || sscanf((arg+1), "%d", &f_prec) != 1 ||
				(f_prec < MIN_PREC || f_prec > MAX_PREC))
			{
				fprintf(stderr, "Err: invalid precision value\n");
				ret = PREC_ERR;
			}
			else
				printf("Precision is set to %d\n", f_prec);
			break;
		case HELP:
			print_help();
			break;
		case EXAMPLE:
			print_example();
			break;
		case VER:
			PRINT_VER;
			break;
		default:
			ret = NO_ARG;
			break;
	}
	
	return ret;
}

static int get_string(void)
{
	/* read input into the buffer */
	int ch, ret, j;
	
	ret = j = 0;
	PROMPT;
	
	while (true)
	{
		ch = getchar();
		// translate exponent operator
		if (EXPON_OP == ch)
			ch = '^';
		
		if (QUIT == ch)
		{
			expr_buff[j++] = ch;
			ret = -1;
			break;
		}
		else if (EOF == ch)
		{
			// mark end of file to the user
			expr_buff[j++] = 'e';
			expr_buff[j++] = 'o';
			expr_buff[j++] = 'f';
			ret = -1;
			break;
		}
		else if (j >= BUFF_SIZE)
		{
			// expression buffer overflow
			fprintf(stderr, "Err: the expression is too long\n");
			fprintf(stderr, "It should be no more than %d characters\n", BUFF_SIZE);
			ret = 1;
			break;
		}
		else if (COMMENT == ch)
		{
			// eat the line
			while (ch != '\n')
				ch = getchar();
			break;
		}
		else if ('\n' == ch)
			break;
		else if (!isspace(ch))
			expr_buff[j++] = ch;
	}
	// terminate string
	expr_buff[j] = '\0';
	
	// echo to stdout or not
	if (echo)
		puts(expr_buff);
	
	return ret;
}

static void print_help(void)
{
	/* print help info */
	printf("%s -- infix arithmetic expression calculator\n", prog_name);
	
	printf("\nSupported operators:\n");
	printf("^ or %c\t- exponentiation\n", EXPON_OP);
	printf("*\t- multiplication\n");
	printf("/\t- division\n");
	printf("+\t- addition\n");
	printf("-\t- subtraction\n");
	printf("%c\t- zero out the current result\n", ZERO_OUT);
	printf("%c\t- quit\n", QUIT);
	printf("%c\t- comment; everything after it is ignored\n", COMMENT);
	
	printf("\nSupported options:\n");
	printf("-%c<number>\t- sets the number of digits displayed after the decimal point\n", F_PREC);
	printf("\t\t <number> must be between %d and %d including.\n", MIN_PREC, MAX_PREC);
	printf("-%c\t- toggles echo; when it's on everything entered is echoed\n", ECHO);
	printf("\t to the screen. It's needed when the input is redirected.\n");
	printf("-%c\t- this screen\n", HELP);
	printf("-%c\t- print an example input file\n", EXAMPLE);
	printf("-%c\t- print version info\n", VER);
	
	printf("\n%s can be called directly from the command line or used interactively\n", prog_name);
	printf("Command line use: %s <option> <infix expression>\n", prog_name);
	printf("\nExample:\n");
	printf("%s 1.2 + (3 + 1) e 2\n", prog_name);
	printf("1.2+(3+1)^2\n");
	printf("3.00 + 1.00 = 4.00\n");
	printf("4.00 ^ 2.00 = 16.00\n");
	printf("1.20 + 16.00 = 17.20\n");
	printf("result: 17.20\n");
	
	printf("\nIn interactive use the current result can be used in an operation\n");
	printf("with the result of the next evaluated expression.\n");
	printf("Interactive use command: %s \n", prog_name);
	
	printf("\nExample:\n");
	printf("Enter expression (q to quit):\n");
	printf("result: 0.00\n");
	printf("?> 1 + 2 * 3\n");
	printf("2.00 * 3.00 = 6.00\n");
	printf("1.00 + 6.00 = 7.00\n");
	printf("result: 7.00\n");
	printf("?> +\n");
	printf("?> 2\n");
	printf("7.00 + 2.00 = 9.00\n");
	printf("result: 9.00\n");
	printf("?> * 3 - 1\n");
	printf("3.00 - 1.00 = 2.00\n");
	printf("9.00 * 2.00 = 18.00\n");
	printf("result: 18.00\n");
	printf("?> q\n");
	printf("Goodbye!\n");
	
	printf("\nVersion: %s\n", prog_ver);
	return;
}

static void print_example(void)
{
	/* print an example input file */
	puts("# calculates the volume of a sphere with radius 5");
	puts("# formula: 4/3 * pi * r^3");
	puts("-p6");
	puts("-o");
	puts("4/3 * 3.141592 * 5^3");
	puts("q");
	return;
}