/* aval.c -- evaluates infix arithmetic expressions */
/* works by going through the expression, saving the numbers
 * in an array, the unary minus operations in a queue, the exponentiation
 * operations on a stack (since exponentiation is right associative),
 * multiplcation and division in a queue, addition and subtraction in a queue;
 * after that, the operations are performed in order, substituting their left
 * operands with the result (in case of unary minus the number is just negated);
 * parentheses are handled by evaluating the expression inside them recursively */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "stack.h"
#include "queue.h"
#include "errchk.h"
#include "eval.h"

// operator record
// holds the type of operator and it's right operand
typedef struct op_record_ {
	int op;
	int pos_right_num;
} op_rec;

// number record
// holds a numbed and a flag
// the flag shows if the number has been consumed by another operator
typedef struct num_record_ {
	bool empty;
	double num;
} num_rec;

#define NUM_BUFF_SIZE 	256
// the number buffer
static num_rec num_buff[NUM_BUFF_SIZE];

// the number buffer counter
static int nb_count;

// verbose prints out operations as they are performed
static bool verbose = true;

// see eval.h
extern int f_prec;

// creates an operator record
static op_rec * make_op_rec(void);

// pushes an operation on a stack
static void push_op(Stack * s, op_rec * orc, int op);

// enqueues an operation in a queue
static void enq_op(Queue * q, op_rec * orc, int op);

// gets the left operand for an operation
static double * get_left_num(int curr_pos);

// queue constants
enum {	
		UNR_Q,	// unary queue
		MD_Q, 	// multiplcation/division queue
		AS_Q, 	// addition/subtraction queue
		NUM_QS	// the number of queues
};

// performs the actual calculations
static void eval(Queue * qs[], Stack * s);

// parses the string and calls eval()
static double parse(const char * expr);

// points to the expression string
static const char * buff_ptr = NULL;

/* --------------- MAIN CODE --------------- */
double calculate(char * expr)
{
	/* prepare and send to parse() */
	
	// set pointer
	buff_ptr = expr;
	
	// initiate the num_buff counter
	nb_count = -1;
	
	return parse(expr);
}

static double parse(const char * expr)
{
	/* parse the expression */
	
	// stack for right associative
	Stack pow_stk_;
	Stack * pow_stk = &pow_stk_;
	
	// queues for left associative and unary
	Queue unr_q, md_q, as_q;
	Queue * q_list[] = {&unr_q, &md_q, &as_q};
	op_rec * pop_r = NULL;
	int i;
	
	// initiate
	stack_init(pow_stk, NULL);
	for (i = 0; i < NUM_QS; ++i)
		queue_init(q_list[i], NULL);

	// go through the string
	while (*buff_ptr != '\0')
	{
		switch (*buff_ptr)
		{
			case '(':
				// recursive call for expression in parentheses
				parse(++buff_ptr);
				break;
			case ')': 
				// evaluate
				eval(q_list, pow_stk);
				// return from recursive call
				return 0;
				break;
			case UNARY_PLUS:
				// do nothing
				break;
			case '+':
				enq_op(q_list[AS_Q], pop_r, *buff_ptr);
				break;
			case UNARY_MINUS:
				enq_op(q_list[UNR_Q], pop_r, *buff_ptr);
				break;
			case '-':
				enq_op(q_list[AS_Q], pop_r, *buff_ptr);
				break;
			case '*':
				enq_op(q_list[MD_Q], pop_r, *buff_ptr);
				break;
			case '/':
				enq_op(q_list[MD_Q], pop_r, *buff_ptr);
				break;
			case '^':
				push_op(pow_stk, pop_r, *buff_ptr);
				break;
			default:
				; 	/* prevents error: a label can only be part of a statement 
					/ and a declaration is not a statement */
				const char * num_start = buff_ptr;
				
				// eat numbers
				while (isdigit(*buff_ptr) || '.' == *buff_ptr)
					++buff_ptr;
				
				// check num_buff size
				++nb_count;
				if (nb_count >= NUM_BUFF_SIZE)
				{
					fprintf(stderr, "Err: too many numbers\n");
					fprintf(stderr, "No more than %d numbers are supported in a single expression\n", 
					NUM_BUFF_SIZE);
					exit(EXIT_FAILURE);
				}
				
				// read number
				num_buff[nb_count].empty = false;
				sscanf(num_start, "%lf", &num_buff[nb_count].num);
				
				// see four lines down
				--buff_ptr;
				break;
		}
		++buff_ptr;
	}
	
	// evaluate
	eval(q_list, pow_stk);
	
	i = 0;
	// at this point only the result is left, get it
	while (true == num_buff[i].empty)
		++i;
	
	// marking the result as empty marks the whole num_buff[] as empty
	num_buff[i].empty = true;

	return num_buff[i].num;
}

static void eval(Queue * qs[], Stack * s)
{
	/* evaluate in order:
	 * negation
	 * exponentiation 
	 * multiplication/division
	 * addition/subtraction */
	 
	op_rec * opr;
	num_rec * nr;
	void * data;
	double * left_num, * right_num, reslt;
	
	if (nb_count < 0)
	{
		fprintf(stderr, "Err: no numbers\n");
		exit(EXIT_FAILURE);
	}
		
	while (qs[UNR_Q]->size != 0)
	{
		// get operation from the unary queue
		queue_deq(qs[UNR_Q], &data);
		opr = (op_rec *)data;
		// read right operand
		nr = &num_buff[opr->pos_right_num];
		right_num = &nr->num;
		
		// negate number
		*right_num = -(*right_num);
		free(data);
	}

	while (s->size != 0)
	{
		// get exponentiation operand from the stack
		stack_pop(s, &data);
		opr = (op_rec *)data;
		nr = &num_buff[opr->pos_right_num];
		// read right operand
		right_num = &nr->num;
		// find left operand
		left_num = get_left_num(opr->pos_right_num);
		// perform exponentiation
		reslt = pow(*left_num, *right_num);
		
		// print or not
		if (verbose)
			printf("%.*f %c %.*f = %.*f\n", 
			f_prec, *left_num, opr->op, f_prec, *right_num, f_prec, reslt);
		
		// save result in the left operand
		*left_num = reslt;
		// mark the right operand as empty
		nr->empty = true;
		free(data);
	}

	while (qs[MD_Q]->size != 0)
	{
		// logic smilira as above
		queue_deq(qs[MD_Q], &data);
		opr = (op_rec *)data;
		nr = &num_buff[opr->pos_right_num];
		right_num = &nr->num;
		left_num = get_left_num(opr->pos_right_num);
		
		if ('*' == opr->op)
			reslt = *left_num * *right_num;
		else
			reslt = *left_num / *right_num;
		
		if (verbose)
			printf("%.*f %c %.*f = %.*f\n", 
			f_prec, *left_num, opr->op, f_prec, *right_num, f_prec, reslt);
		
		*left_num = reslt;
		nr->empty = true;
		free(data);
	}

	while (qs[AS_Q]->size != 0)
	{
		// logic similar as above
		queue_deq(qs[AS_Q], &data);
		opr = (op_rec *)data;
		nr = &num_buff[opr->pos_right_num];
		right_num = &nr->num;
		left_num = get_left_num(opr->pos_right_num);
		
		if ('+' == opr->op)
			reslt = *left_num + *right_num;
		else
			reslt = *left_num - *right_num;
	
		if (verbose)
			printf("%.*f %c %.*f = %.*f\n", 
			f_prec, *left_num, opr->op, f_prec, *right_num, f_prec, reslt);
			
		*left_num = reslt;
		nr->empty = true;
		free(data);
	}
		
	return;
}

static double * get_left_num(int curr_pos)
{
	/* scan number array left for a non-empty entry */
	
	// decrement since curr_pos is pointing to the right operand
	--curr_pos;
	while (true == num_buff[curr_pos].empty)
		--curr_pos;
	
	return &num_buff[curr_pos].num;
}

static void push_op(Stack * s, op_rec * orc, int op)
{
	/* push operation and it's right operand position on the stack
     * here used only for exponentiation */
	orc = make_op_rec();
	orc->op = op;
	orc->pos_right_num = nb_count + 1;
	stack_push(s, (void *)orc);
	return;
}

static void enq_op(Queue * q, op_rec * orc, int op)
{
	/* enqueue operation and save it's right operand position
	 * used for left associative operators */
	orc = make_op_rec();
	orc->op = op;
	orc->pos_right_num = nb_count + 1;
	queue_enq(q, (void *)orc);
	return;
}

static op_rec * make_op_rec(void)
{
	/* allocate memory for an operator record */
	op_rec * ret;
	if ( (ret = malloc(sizeof(*ret))) == NULL)
	{
		fprintf(stderr, "Err: memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	return ret;
}
