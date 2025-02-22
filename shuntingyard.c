#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"
#include "queue.h"
#include "shuntingyard.h"

/* ____________________________________________________________________________

    char *strdup(const char *s)

    Creates a duplicate of a given string

    Parameters:
        s - The input string to duplicate

    Returns:
        A pointer to the newly allocated string containing a copy of `s`
        or NULL if memory allocation fails.
   ____________________________________________________________________________
*/
char *strdup(const char *s) {

    // sanity check
    if (!s) return NULL;

    // length of the string
    size_t len = strlen(s) + 1;

    // allocate memory
    char *dup = malloc(len);
    if (!dup) return NULL;

    // copy the content
    memcpy(dup, s, len);

    return dup;
}

/* ____________________________________________________________________________

    int precedence(char token)

    Determines the precedence of a mathematical operator

    Parameters:
        token - The operator character

    Returns:
        An integer representing the priority of the operator

   ____________________________________________________________________________
*/
int precedence(char token) {

    // sanity check
    if(!token) return 0;

    // operator and their priorities
    if(token == '+' || token == '-') return 1;
    if(token == '*' || token == '/') return 2;
    if(token == '^') return 3;
    return 0;
}

/* ____________________________________________________________________________

    int is_left_associative(char token)

    Determines if a mathematical operator is left-associative

    Parameters:
        token - The operator character

    Returns:
        1 if the operator is left-associative, 0 otherwise
   ____________________________________________________________________________
*/
int is_left_associative(char token) {

    // sanity check
    if(!token) return 0;

    return token != '^';
}


/* ____________________________________________________________________________

    int is_operator(char token)

    Checks if a given character is a mathematical operator

    Parameters:
        token - The character to check

    Returns:
        1 if the character is a recognized operator, 0 otherwise
   ____________________________________________________________________________
*/
int is_operator(char token) {

    // sanity check
    if(!token) return 0;

    return token == '+' || token == '-' || token == '*' || token == '/' || token == '^';
}

/* ____________________________________________________________________________

    int is_function(const char *token)

    Checks if a given string represents a valid mathematical function

    Parameters:
        token - The string to check

    Returns:
        1 if the string matches a recognized function, 0 otherwise
   ____________________________________________________________________________
*/
int is_function(const char *token) {

    // sanity check
    if(!token) return 0;

    return strcmp(token, "sin") == 0 || strcmp(token, "cos") == 0 ||
       strcmp(token, "tan") == 0 || strcmp(token, "asin") == 0 ||
       strcmp(token, "acos") == 0 || strcmp(token, "atan") == 0 ||
       strcmp(token, "sinh") == 0 || strcmp(token, "cosh") == 0 ||
       strcmp(token, "tanh") == 0 || strcmp(token, "ln") == 0 ||
       strcmp(token, "log") == 0 || strcmp(token, "sqrt") == 0 ||
       strcmp(token, "abs") == 0 || strcmp(token, "exp") == 0;
}


/* ____________________________________________________________________________

    void shunting_yard(const char *expression, queue *output)

    Converts a mathematical expression into Reverse Polish Notation (RPN)
    using the Shunting Yard algorithm

    Parameters:
        expression - The input mathematical expression as a string
        output - A pointer to a queue where the RPN tokens will be stored

    Returns:
        Nothing. The result is stored in the provided output queue
   ____________________________________________________________________________
*/
void shunting_yard(const char *expression, queue *output) {

    // sanity check
    if(!expression || !output) return;

    // create a stack for operators and functions
    stack *holding_stack = stack_create(100, sizeof(char));
    if(!holding_stack) return;

    // copy the input expression for safe processing
    char *expr_copy = strdup(expression);
    if(!expr_copy) {
        stack_free(&holding_stack);
        return;
    }

    // Tokenize the expression by spaces
    char *token = strtok(expr_copy, " ");

    // Process each token
    while(token != NULL) {

        // Handling numbers
        if(isdigit(token[0]) ||
            (token[0] == '-' && isdigit(token[1])) ||
            (strchr(token, 'E'))) {
            for(size_t i = 0; i < strlen(token); i++) {
                char num_char = token[i];
                if(!queue_enqueue(output, &num_char)) {
                    free(expr_copy);
                    stack_free(&holding_stack);
                    return;
                }
            }
        }
        // handling unary minus (~)
        else if(token[0] == '~') {
            char unary_minus = '~';
            if(!queue_enqueue(output, &unary_minus)) {
                free(expr_copy);
                stack_free(&holding_stack);
                return;
            }
        }

        // handling functions
        else if(is_function(token)) {
            char end_marker = '$';
            stack_push(holding_stack, &end_marker);
            for(size_t i = strlen(token); i > 0; i--) {
                char func_char = token[i - 1];
                stack_push(holding_stack, &func_char);
            }
        }

        // handling variable x
        else if(strcmp(token, "x") == 0) {
            char var = 'x';
            if(!queue_enqueue(output, &var)) {
                free(expr_copy);
                stack_free(&holding_stack);
                return;
            }
        }

        // handling operators
        else if(is_operator(token[0])) {
            char op1 = token[0];
            while(holding_stack->sp >= 0) {
                char top_op;
                if(!stack_peek(holding_stack, &top_op)) break;
                if(is_operator(top_op) &&
                    ((precedence(top_op) > precedence(op1)) ||
                     (precedence(top_op) == precedence(op1) && is_left_associative(op1)))) {
                    stack_pop(holding_stack, &top_op);
                    if(!queue_enqueue(output, &top_op)) {
                        free(expr_copy);
                        stack_free(&holding_stack);
                        return;
                    }
                } else {
                    break;
                }
            }
            stack_push(holding_stack, &op1);
        }

        // handling open bracket
        else if(strcmp(token, "(") == 0) {
            char paren = '(';
            stack_push(holding_stack, &paren);
        }

        // handling close bracket
        else if(strcmp(token, ")") == 0) {
            char top_op;
            while(stack_pop(holding_stack, &top_op) && top_op != '(') {
                if(!queue_enqueue(output, &top_op)) {
                    free(expr_copy);
                    stack_free(&holding_stack);
                    return;
                }
            }
            while(holding_stack->sp >= 0) {
                stack_peek(holding_stack, &top_op);
                if(isalpha(top_op) || top_op == '$') {
                    stack_pop(holding_stack, &top_op);
                    if(!queue_enqueue(output, &top_op)) {
                        free(expr_copy);
                        stack_free(&holding_stack);
                        return;
                    }
                } else {
                    break;
                }
            }
        }

        // move to the next token
        token = strtok(NULL, " ");
    }

    // moving remaining operators to the output queue
    char top_op;
    while(stack_pop(holding_stack, &top_op)) {
        if(top_op == '(' || top_op == ')') {
            break;
        }
        if(!queue_enqueue(output, &top_op)) {
            free(expr_copy);
            stack_free(&holding_stack);
            return;
        }
    }

    // free memory
    free(expr_copy);
    expr_copy = NULL;
    stack_free(&holding_stack);
    holding_stack = NULL;

}