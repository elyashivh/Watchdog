/*
Things to handle after removing all if:
1. pow  - precidence
2. bracks (simple state, or ( = oprator. ) = oprand )
3. unary - 
4. divided by 0 - combine with the calculate?
*/

/*
Author:     Elyashiv Horden  
Date:		11-1-2024	    
CR: 	    Yechiel Shirion
*/

#include <assert.h>     /*assert*/
#include <stddef.h>     /*size_t*/
#include <ctype.h>      /*isdigit*/
#include <stdlib.h>     /*strtod*/
#include <string.h>     /*memcpy*/

#include "calculator.h"
#include "stack.h"

#define CAPACITY (1000)
#define ASCII (128)

typedef enum states
{
    WAITING_FOR_OPERAND = 0,
    WAITING_FOR_OPERATOR = 1,
    DEAD_END = 2
}calc_state_t;

typedef double(*action_func_t)(double a, double b);
typedef char *(*handler_t)(char *str);
typedef char *(*validiation_handler_t)(char *str, calc_state_t* fsm_state);


typedef enum transitions
{
    OPERAND = 0,
    OPERATOR = 1
}oper_type_t;

static stack_t* g_stack_arr[3] = {NULL};

typedef struct calc
{
    action_func_t operator_func;
    handler_t handler;
    int precedence;
    int associative;
} calculator_t;

typedef struct calc_valid
{
    validiation_handler_t handler;
} calculator_validition_t;

static calculator_t LUT[ASCII];
static calculator_validition_t validiation_LUT[ASCII];


/*                       FORWARD DECLARATION                          */
/******************************************************************** */
calc_status_t ValiditionTest(char* expression);
static void InitLUTS();
static void InitOperatorsFuncs();
static void InitPrecedence();
static void InitAssociative();
static void Inithandlers();
static void InitValidiation();
static void CalculateHelper(char operator);
static char* HandlerCloserBrack(char* str);
static char* HandlerOpenBrack(char *str);
static char* HandlerOperator(char *str);
static char* HandlerSpace(char *str);
static char* HandlerOperands(char *str);
static char* ValiditionHandlerOperator(char *str, calc_state_t* state);
static char* ValiditionHandlerOperands(char *str, calc_state_t* state);
static char* ValiditionHandlerOpenBrack(char *str, calc_state_t* state);
static char* ValiditionHandlerCloserBrack(char *str, calc_state_t* fsm_state);
static char* JumpSpaces(char* expression);
/* static char* ValiditionHandlerSpace(char *str , calc_state_t* fsm_state);*/
static double PlusOperator(double a, double b);
static double MinusOperator(double a, double b);
static double MultOperator(double a, double b);
static double DivOperator(double a, double b);
static double PowOperator(double a, double b);
/********************************************************************* */



/*                           API FUNCTION                               */
/********************************************************************** */
calc_status_t InfinityCalculate(const char* expression, double* result)
{
    calc_status_t is_valid = SUCCESS;
    char* temp = NULL;
    assert(expression);
    assert(result);

    g_stack_arr[OPERAND] = StackCreate(CAPACITY, sizeof(double));
    if (NULL == g_stack_arr[OPERAND])
    {
        return ALLOCATION_FAIL;
    }
    g_stack_arr[OPERATOR] = StackCreate(CAPACITY, sizeof(char));
    if (NULL == g_stack_arr[OPERATOR])
    {
        StackDestroy(g_stack_arr[OPERAND]);
        return ALLOCATION_FAIL;
    }
    g_stack_arr[2] = StackCreate(CAPACITY, sizeof(char));
    if(NULL == g_stack_arr[2])
    {
        StackDestroy(g_stack_arr[OPERAND]);
        StackDestroy(g_stack_arr[OPERATOR]);
        return ALLOCATION_FAIL;
    }

    InitLUTS();
    temp = (char*)expression;
    is_valid = ValiditionTest((char*)temp);

    if (SUCCESS != is_valid)
    {
        StackDestroy(g_stack_arr[OPERAND]);
        StackDestroy(g_stack_arr[OPERATOR]);
        return is_valid;
    }

    StackPush(g_stack_arr[OPERATOR], "(");

    while ('\0' != *expression)
    {
        expression = LUT[(int)*expression].handler((char*)expression);
    }

    HandlerCloserBrack((char*)expression);
    
    *result = *(double*)StackPeek(g_stack_arr[OPERAND]);

    
    StackDestroy(g_stack_arr[OPERAND]);
    StackDestroy(g_stack_arr[OPERATOR]);
    StackDestroy(g_stack_arr[2]);

    return SUCCESS;
}


/*                           STATIC FUNCTION                            */
/********************************************************************** */
static void InitLUTS()
{
    InitValidiation();
    InitOperatorsFuncs();
    InitPrecedence();
    InitAssociative();
    Inithandlers();
}    

static void InitOperatorsFuncs()
{
    LUT['+'].operator_func = PlusOperator;
    LUT['-'].operator_func = MinusOperator;
    LUT['*'].operator_func = MultOperator;
    LUT['/'].operator_func = DivOperator;
    LUT['^'].operator_func = PowOperator;
}

static void InitPrecedence()
{
    LUT['+'].precedence = 1;
    LUT['-'].precedence = 1;
    LUT['*'].precedence = 2;
    LUT['/'].precedence = 2;
    LUT['('].precedence = 0;
    LUT['^'].precedence = 2 ;
}

static void InitAssociative()
{
    LUT['+'].associative = 0;
    LUT['-'].associative = 0;
    LUT['*'].associative = 0;
    LUT['/'].associative = 0;
    LUT['('].associative = 0;
    LUT['^'].associative = 1;
}

static void InitValidiation()
{
    int i = 0;

    for(i = '0'; i <= '9'; ++i)
    {
        validiation_LUT[i].handler = ValiditionHandlerOperands;
    }
    
    validiation_LUT['+'].handler = ValiditionHandlerOperator;
    validiation_LUT['-'].handler = ValiditionHandlerOperator;
    validiation_LUT['*'].handler = ValiditionHandlerOperator;
    validiation_LUT['/'].handler = ValiditionHandlerOperator;
    validiation_LUT['^'].handler = ValiditionHandlerOperator;
    validiation_LUT['('].handler = ValiditionHandlerOpenBrack;
    validiation_LUT[')'].handler = ValiditionHandlerCloserBrack;
    /* validiation_LUT[' '].handler = ValiditionHandlerSpace; */
}


static void Inithandlers()
{
    
    int i = 0;

    for(i = '0'; i <= '9'; ++i)
    {
        LUT[i].handler = HandlerOperands;
    }
    
    LUT['+'].handler = HandlerOperator;
    LUT['-'].handler = HandlerOperator;
    LUT['*'].handler = HandlerOperator;
    LUT['/'].handler = HandlerOperator;
    LUT['^'].handler = HandlerOperator;
    LUT['('].handler = HandlerOpenBrack;
    LUT[')'].handler = HandlerCloserBrack;
    LUT[' '].handler = HandlerSpace;
}

static char* HandlerCloserBrack(char* str)
{
    char temp_operand = '\0';
    temp_operand = *(char*)StackPeek(g_stack_arr[OPERATOR]);

    while ('(' != temp_operand)
    {
        CalculateHelper(temp_operand);
        StackPop(g_stack_arr[OPERATOR]);
        temp_operand = *(char*)StackPeek(g_stack_arr[OPERATOR]);
    }

    StackPop(g_stack_arr[OPERATOR]);
    return ++str;
}

static void CalculateHelper(char operator)
{
    double top_opearnd = 0;
    double sec_operand = 0;
    double result = 0;

    top_opearnd = *(double*)StackPeek(g_stack_arr[OPERAND]);
    StackPop(g_stack_arr[OPERAND]);
    sec_operand = *(double*)StackPeek(g_stack_arr[OPERAND]);
    StackPop(g_stack_arr[OPERAND]);

    result = LUT[(int)operator].operator_func(top_opearnd, sec_operand);
    StackPush(g_stack_arr[OPERAND], &result);
}

static char* HandlerOperator(char *str)
{
    char top_operator = 0;
    int associative = LUT[(int)*str].associative;
    
    top_operator = *(char*)StackPeek(g_stack_arr[OPERATOR]);

    while (LUT[(int)top_operator].precedence >= LUT[(int)*str].precedence + associative)
    {
        CalculateHelper(top_operator);
        StackPop(g_stack_arr[OPERATOR]);
        top_operator = *(char*)StackPeek(g_stack_arr[OPERATOR]);
    }

    StackPush(g_stack_arr[OPERATOR], str);

    return ++str;
}

static char* HandlerOpenBrack(char *str)
{
    StackPush(g_stack_arr[OPERATOR], str);

    return ++str;
}

static char* HandlerSpace(char *str)
{
    return ++str;
}

static char* HandlerOperands(char *str)
{
    char *return_ptr = NULL;
    double number = 0;

   number = strtod(str, &return_ptr);

   StackPush(g_stack_arr[OPERAND], &number);

   return return_ptr;
}

static double PlusOperator(double a, double b)
{
    return b + a;
}

static double MinusOperator(double a, double b)
{
    return b - a;
}

static double MultOperator(double a, double b)
{
    return b * a;
}

static double DivOperator(double a, double b)
{
    return b / a;
}

static double PowOperator(double a, double b)
{
    double result = 1;

    if (b < 0)
    {
        return -1;
    }

    while (b > 0) 
    {
        if ((int)b % 2 == 1) {
            result *= a;
        }
        a *= a;
        b /= 2;
    }

    return result;
}


calc_status_t ValiditionTest(char* expression)
{
    calc_state_t fsm_state = WAITING_FOR_OPERAND;
    calc_state_t prev_state = WAITING_FOR_OPERATOR;
    calc_status_t calc_status = SUCCESS;

    while ('\0' != *expression && calc_status != SYNTAX_ERROR)
    {
        if ((prev_state == WAITING_FOR_OPERAND && fsm_state == WAITING_FOR_OPERAND) ||
            (prev_state == WAITING_FOR_OPERATOR && fsm_state == WAITING_FOR_OPERATOR))
        {
            calc_status = SYNTAX_ERROR;
            break;
        }
        prev_state = fsm_state;
        expression = JumpSpaces(expression);
        expression = validiation_LUT[(int)*expression].handler((char*)expression, &fsm_state);
    }
    
    if (!StackIsEmpty(g_stack_arr[2]))
    {
        calc_status = SYNTAX_ERROR;
    }
    
    return calc_status;
}

static char* ValiditionHandlerOperator(char *str, calc_state_t* fsm_state)
{
    *fsm_state = WAITING_FOR_OPERAND;

    return ++str;
}

static char* ValiditionHandlerOperands(char *str, calc_state_t* fsm_state)
{
    char *return_ptr = NULL;
    *fsm_state = WAITING_FOR_OPERATOR;
    strtod(str, &return_ptr);

    return return_ptr;
}

static char* ValiditionHandlerOpenBrack(char *str, calc_state_t* fsm_state)
{
    StackPush(g_stack_arr[2], str);
    *fsm_state = WAITING_FOR_OPERAND;

    return ++str;
}

static char* ValiditionHandlerCloserBrack(char *str, calc_state_t* fsm_state)
{
    if (StackIsEmpty(g_stack_arr[2]))
    {
        *fsm_state = DEAD_END;
        return ++str;  
    }
    StackPop(g_stack_arr[2]);
    *fsm_state = WAITING_FOR_OPERATOR;

    return ++str;
}

/* static char* ValiditionHandlerSpace(char *str , calc_state_t* fsm_state)
{
    *fsm_state = IS_SPACE;
    return ++str;
} */

static char* JumpSpaces(char* expression)
{
    while('\0' != *expression && isspace(*expression))
    {
        ++expression;
    }
    return expression;
}


