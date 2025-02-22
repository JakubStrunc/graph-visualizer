#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#include "postscript.h"

// constants for e and pi
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// constants for return
#define SUCCESS 0
#define ERR_INVALID_ARGUMENTS 1
#define ERR_INVALID_FUNCTION 2
#define ERR_FILE_ERROR 3
#define ERR_INVALID_LIMITS 4

/* ____________________________________________________________________________

    char *add_spaces(const char *expression)

    Processes a mathematical expression string:
    - Adds spaces around operators and parentheses
    - Replaces constants e or pi with their numerical values
    - Converts exp(x) to e ^ x

    Parameters:
        expression - mathematical expression string

    Returns:
        a new string with the processed expression
   ____________________________________________________________________________
*/
char *add_spaces(const char *expression) {

    // sanity check
    if(expression == NULL) return NULL;

    // allocate memory for the processed string
    size_t length = strlen(expression);
    char *result = (char *)malloc(4 * length + 1);
    if(!result) exit(EXIT_FAILURE);


    size_t j = 0;
    for(size_t i = 0; i < length; i++) {
        char current = expression[i];

        // handling exp
        if(current == 'e' && i + 3 < length &&
            expression[i + 1] == 'x' &&
            expression[i + 2] == 'p' &&
            expression[i + 3] == '(') {
            // adding constant e
            char e_str[32];
            sprintf(e_str, "%.15g", M_E); // converting to string
            for(size_t k = 0; k < strlen(e_str); k++) {
                result[j++] = e_str[k];
            }
            if(j > 0 && result[j - 1] != ' ') result[j++] = ' ';

            // adding ^
            result[j++] = '^';
            result[j++] = ' ';

            // skipping exp(
            i += 3;

            // adding content in brackets
            size_t paren_depth = 1;
            while(paren_depth > 0 && ++i < length) {
                current = expression[i];
                if(current == '(') paren_depth++;
                if(current == ')') paren_depth--;
                if(paren_depth > 0) {
                    result[j++] = current;
                }
            }
			if(current == ')') result[j++] = ' ';
            continue;
        }

        // replace negative signs with ~ for unary negation
        if(current == '-' &&
    		(i == 0 || (!isdigit(expression[i - 1]) && expression[i - 1] != ')')) && // check previous character
    		(i + 1 < length &&
     			(expression[i + 1] == '(' ||
      			isalpha(expression[i + 1]) ||
      			isdigit(expression[i + 1])))) { // check next character
    		if(j > 0 && result[j - 1] != ' ') result[j++] = ' ';
            result[j++] = '~';
            result[j++] = ' ';

        }

		// replace e with Euler's number
        else if(current == 'e' &&
                 (i == 0 || !isalpha(expression[i - 1])) && // checks if the letter is not part any function
                 (i + 1 == length || !isalpha(expression[i + 1]))) {
            char e_str[32];
            sprintf(e_str, "%.15g", M_E); // convert to string
            for(size_t k = 0; k < strlen(e_str); k++) {
                result[j++] = e_str[k];
            }
        }

        // replace pi with Pi's value
        else if(current == 'p' && i + 1 < length && expression[i + 1] == 'i' &&
                 (i == 0 || !isalpha(expression[i - 1])) && // checks if the letter is not part any function
                 (i + 2 == length || !isalpha(expression[i + 2]))) {
            char pi_str[32];
            sprintf(pi_str, "%.15g", M_PI); // convert to string
            for(size_t k = 0; k < strlen(pi_str); k++) {
                result[j++] = pi_str[k];
            }
            i++; // skipping next char
        }

        // add spaces around operators
        else if(current == '+' || current == '-' || current == '*' || current == '/' || current == '^') {
            if(j > 0 && result[j - 1] != ' ') result[j++] = ' ';
            result[j++] = current;
            if(expression[i + 1] != ' ') result[j++] = ' ';
        }

        // add space after function names before open bracket
        else if(isalpha(current)) {
            result[j++] = current;
            if(i + 1 < length && expression[i + 1] == '(') {
                result[j++] = ' ';
            }
        }

        // handling open bracket
        else if(current == '(') {
            result[j++] = current;
            if(expression[i + 1] != ' ') result[j++] = ' ';
        }

        // handling close bracket
        else if(current == ')') {
            if(j > 0 && result[j - 1] != ' ') result[j++] = ' ';
            result[j++] = current;
        }

        // handle leading decimal points
        else if(current == '.' && (i == 0 || !isdigit(expression[i - 1]))) {
            result[j++] = '0'; // add 0 before alone decimal point
            result[j++] = current;
        }
		// copy other characters unchanged
        else {
            result[j++] = current;
        }
    }
    // terminate the string
    result[j] = '\0';

    return result;
}

/* ____________________________________________________________________________

    int is_valid_function(const char *func)

    Validates a mathematical function string

    Parameters:
        func - The mathematical function string to validate

    Returns:
        1 if the function is valid, 0 otherwise
   ____________________________________________________________________________
*/
int is_valid_function(const char *func) {

    // sanity check
    if(func == NULL) return 0;

    // allowed chars
    const char *allowed_chars = "0123456789+-*/^~().xE";

    // allowed functions
    const char *allowed_functions[] = {"sin", "cos", "tan",
                                       "asin", "acos", "atan",
                                       "sinh", "cosh", "tanh",
                                       "log", "ln", "sqrt", "abs", NULL};

    // bracket counter
    int bracket_count = 0;

    // iterate through string
    size_t i = 0;
    while(func[i] != '\0') {
        char c = func[i];

        // ignore spaces
        if(isspace(c)) {
            i++;
            continue;
        }

        // check individual characters
        if(!strchr(allowed_chars, c)) {
            // check if the character is a valid function name
            if(isalpha(c)) {
                int valid_function = 0;
                for(int j = 0; allowed_functions[j] != NULL; j++) {
                    size_t len = strlen(allowed_functions[j]);
                    // check if the function matches and is followed by '(' or space
                    if(strncmp(&func[i], allowed_functions[j], len) == 0 &&
                        (func[i + len] == '(' || isspace(func[i + len]) || func[i + len] == '\0')) {
                        valid_function = 1;
                        i += len - 1;
                        break;
                    }
                }

                if(!valid_function) {
                    return 0; // invalid function
                }
            } else {
                return 0; // invalid character
            }
        }

        // count open brackets
        if(c == '(') {
            bracket_count++;
        }

        // count close brackets
        if(c == ')') {
            bracket_count--;
            // check if closing bracket appears before an opening bracket
            if(bracket_count < 0) {
                return 0;
            }
        }

        i++;
    }

    // check if all open brackets are closed
    if(bracket_count != 0) {
        return 0;
    }

    return 1;
}


/* ____________________________________________________________________________

    int main(int argc, char *argv[])

    Main function to process input, validate mathematical expressions,
    and generate a PostScript file for graph rendering

    Parameters:
        argc - The number of command-line arguments
        argv - Array of command-line arguments:
               argv[1] - Mathematical function as a string
               argv[2] - Output file name for the PostScript file
               argv[3] (optional) - Limits for the graph in the format x_min:x_max:y_min:y_max

    Returns:
        SUCCESS (0) if the graph is generated successfully
        Error codes for invalid arguments, functions, limits, or file errors
   ____________________________________________________________________________
*/
int main(int argc, char *argv[]) {

    // check the number of arguments
    if(argc < 3) {
        printf("Error: Missing arguments\nCode needs all these arguments: graph.exe <func> <out-file> [<limits>]\n");
        return ERR_INVALID_ARGUMENTS;
    }

    // assign arguments
    char *func = add_spaces(argv[1]);
    char *outfile = argv[2];
    char *limits = argc > 3 ? argv[3] : NULL;


    printf("Function %s\n", func);
    printf("Outfile %s\n", outfile);
    printf("Limits %s\n", limits);

    // check if the function contains the variable x
    if(strstr(func, "x") == NULL) {
        printf("Error: The function must contain the variable x.\n");
        return ERR_INVALID_FUNCTION;
    }

    // check if the function contains only allowed characters and functions
    if(!is_valid_function(func)) {
        printf("Error: The function contains invalid characters or unsupported functions.\n");
        free(func);
        return ERR_INVALID_FUNCTION;
    }



    // parse limits from input
    double x_min = -10, x_max = 10, y_min = -10, y_max = 10;
    if(limits) {
        if (sscanf(limits, "%lf:%lf:%lf:%lf", &x_min, &x_max, &y_min, &y_max) != 4) {
            fprintf(stderr, "Error: Invalid format for limits.\n");
            return ERR_INVALID_LIMITS;
        }

        // ensure min is less than max
        if(x_min >= x_max) {
            fprintf(stderr, "Error: x_min must be less than x_max.\n");
            return ERR_INVALID_LIMITS;
        }
        if(y_min >= y_max) {
            fprintf(stderr, "Error: y_min must be less than y_max.\n");
            return ERR_INVALID_LIMITS;
        }
    }

	// create a PostScript file
    postscript *ps = create_postscript(outfile, func, x_min, x_max, y_min, y_max);
    if(!ps) {
        free(func);
        fprintf(stderr, "Error: Failed to create PostScript file.\n");
        return ERR_FILE_ERROR;
    }

    // render axes, grid, and graph
  	draw_square_axis(ps);
    draw_ticks_and_labels(ps);
    draw_graph(ps);

    // close the PostScript file
    close_postscript(ps);


    // free allocated memory
    free(func);


    printf("Graph successfully generated in file: %s\n", outfile);
    return SUCCESS;
}


