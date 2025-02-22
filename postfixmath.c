#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "stack.h"
#include "queue.h"
#include "postfixmath.h"


// constants
#define STACK_SIZE 20
#define GRID_SIZE 60.0

/* ____________________________________________________________________________

    double evaluate_function(char *func, double x)

    Evaluates a mathematical function for a given X value.

    Parameters:
        func - The mathematical function as a string.
        x - The X value to evaluate.

    Returns:
        The result of the function evaluation or NAN if the function is invalid.
   ____________________________________________________________________________
*/
double evaluate_function( char *func, double x) {

    // sanity check
    if(!func) return NAN;


    // Compare and evaluate functions
    if(strstr(func, "sin") == func) {
        return sin(x);
    }else if(strstr(func, "cos") == func) {
        return cos(x);
    }else if(strstr(func, "tan") == func) {
        return tan(x);
    }else if(strstr(func, "asin") == func) {
        return asin(x);
    }else if(strstr(func, "acos") == func) {
        return acos(x);
    }else if(strstr(func, "atan") == func) {
        return atan(x);
    }else if(strstr(func, "sinh") == func) {
        return sinh(x);
    }else if(strstr(func, "cosh") == func) {
        return cosh(x);
    }else if(strstr(func, "tanh") == func) {
        return tanh(x);
    }else if(strstr(func, "log") == func) {
        return log10(x);
    }else if(strstr(func, "ln") == func) {
        return log(x);
    }else if(strstr(func, "sqrt") == func) {
        return sqrt(x);
    }else if(strstr(func, "abs") == func) {
        return fabs(x);
    }


    return x;
}

/* ____________________________________________________________________________

    double evaluate_postfix_expression(queue *expression, double x_value)

    Evaluates a mathematical expression in Reverse Polish Notation (RPN)

    Parameters:
        expression - A queue containing the postfix expression
        x_value - The value of the variable X for evaluation

    Returns:
        The result of the evaluation or NAN if the evaluation fails
   ____________________________________________________________________________
*/
double evaluate_postfix_expression(queue *expression, double x_value) {

    // sanity check
    if(!expression) return NAN;

    // create a stack for evaluation
    stack *s = stack_create(STACK_SIZE, sizeof(double));
    if(!s) return NAN;

    // copy the queue for safety iterating
    queue *expression_copy = queue_copy(expression);
    if(!expression_copy) {
        stack_free(&s);
        return NAN;
    }

    // helping values
    double a, b, result = 0.0;
    char token;

    // buffers for reading numbers and functions
    char number_buffer[32] = {0};
    int number_index = 0;
    char function_name[16] = {0};
    int function_index = 0;

    // indicator for unary minus
    int is_unary = 0;

    while(queue_dequeue(expression_copy, &token)) {

        // if the token is a digit or a decimal point
        if(isdigit(token) || token == '.') {
            number_buffer[number_index++] = token;
        }

        // if the token is scientific notation E
        else if(token == 'E') {
            number_buffer[number_index++] = 'E';
        }

        // if the token is a unary minus ~
        else if(token == '~') {
            is_unary = 1;
        }
        // push the value of x_value for x
        if(token == 'x') {
            if(is_unary) {
                // apply unary minus
                x_value = -x_value;
                is_unary = 0;
            }
            stack_push(s, &x_value);
        }

        // if the number is complete or followed by another token
        else if(number_index > 0 && (strchr("+-*/^~$", token) || isalpha(token) || token == 'x')) {
            number_buffer[number_index] = '\0'; // terminate the number string
            double num = atof(number_buffer);  // convert to double
            if(is_unary) {
                // aplly unary minus
                num = -num;
                is_unary = 0;
            }
            stack_push(s, &num);
            memset(number_buffer, 0, sizeof(number_buffer)); // Vymazání bufferu
            number_index = 0;
        }

        // if the token is part of a function name
        else if(isalpha(token) && token != '$') {
            function_name[function_index++] = token;
        }

        // if the token marks the end of a function $
        else if(token == '$') {
            function_name[function_index] = '\0'; // terminate the function name
            if(!stack_pop(s, &a)) {
                queue_free(&expression_copy);
                stack_free(&s);
                return NAN;
            }
            if(is_unary) {
                // apply unary minus to the function result
                a = -a;
                is_unary = 0;
            }
            result = evaluate_function(function_name, a);
            stack_push(s, &result);

            // clear the function buffer
            memset(function_name, 0, sizeof(function_name));
            function_index = 0;
        }

        // if the token is an operator
        else if(strchr("+-*/^", token)) {
            if(!stack_pop(s, &b) || !stack_pop(s, &a)) {
                queue_free(&expression_copy);
                stack_free(&s);
                return NAN;
            }
            switch(token) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': result = a / b; break;
                case '^': result = pow(a, b); break;
                default:
                    queue_free(&expression_copy);
                    stack_free(&s);
                    return NAN;
            }
            stack_push(s, &result);
        }
    }

     // process a number in the buffer
    if(number_index > 0) {
        number_buffer[number_index] = '\0';
        double num = atof(number_buffer);
        if (is_unary) {
            num = -num;
        }
        stack_push(s, &num);
    }

    // Final result on top of the stack
    if(!stack_pop(s, &result)) {
        queue_free(&expression_copy);
        stack_free(&s);
        return NAN;
    }

    // free memory
    queue_free(&expression_copy);
    stack_free(&s);

    return result;
}
