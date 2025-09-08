#ifndef __CALCULATOR_H__
#define __CALCULATOR_H__


typedef enum status
{
    SUCCESS = 0,
    SYNTAX_ERROR,
    MATH_ERROR,
    ALLOCATION_FAIL
} calc_status_t;


/*
Description: This function extracts the mathematical expression from the input string and returns the calculated result.
Args :       expression - string that contains the mathematical expression (can contain operands, operators, spaces).
             Operand: (positive/negative) 0-9
             Operators: +, -, *, /, ^, (, )
result:      a pointer to a double to store the calculated result. 
Return value: 
            SUCCESS - If expression is valid.
            SYNTAX_ERROR - If expression is invalid due to order of operands and operators.
            MATH_ERROR - If expression contains mathematical error.
            ALLOCATION_FAIL - If expression failed to allocate space. 
*/
calc_status_t InfinityCalculate(const char* expression, double* result);


#endif /*__CALCULATOR_H__*/