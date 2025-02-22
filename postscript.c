#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "postscript.h"
#include "stack.h"
#include "shuntingyard.h"
#include "postfixmath.h"

// constants
#define POST_SCRIPT_WIDTH 560
#define POST_SCRIPT_HEIGHT 560


/* ____________________________________________________________________________

    postscript *create_postscript(const char *filename, const char *func,
                                  double x_min, double x_max,
                                  double y_min, double y_max)

    Creates a structure for generating a PostScript file

    Parameters:
        filename - Name of the PostScript file to create
        func - Mathematical function as a string in infix notation
        x_min - Minimum X value for the graph
        x_max - Maximum X value for the graph
        y_min - Minimum Y value for the graph
        y_max - Maximum Y value for the graph

    Returns:
        A pointer to the postscript structure, or NULL
   ____________________________________________________________________________
*/
postscript *create_postscript(const char *filename, const char *func, double x_min, double x_max, double y_min, double y_max){

    postscript *ps;

    // sanity check
    if(!filename || !func || !x_min || !x_max || !y_min || !y_max) return NULL;

    // allocate memory for the PostScript structure
    ps = (postscript *)malloc(sizeof(postscript));
    if(!ps) return NULL;

    // open file for writing
    ps->file = fopen(filename, "w");
    if(!ps->file) {
        printf("H");
        free(ps);
        return NULL;
    }

    // initialize parameters
    ps->x_min = x_min;
    ps->x_max = x_max;
    ps->y_min = y_min;
    ps->y_max = y_max;

    // create a queue for the postfix expression
    ps->func = queue_create(100, sizeof(char));
    if(!ps->func) {
        printf("N");
        fclose(ps->file);
        free(ps);
        return NULL;
    }

    // convert the function to postfix notation
    shunting_yard(func, ps->func);
    char x;
    while (queue_dequeue(ps->func, &x)) {
      printf("%c", x);
    }

    // calculate scaling factors
    ps->scale_x = POST_SCRIPT_WIDTH / (x_max - x_min);
    ps->scale_y = POST_SCRIPT_HEIGHT / (y_max - y_min);

    // center the graph on the screen
    double x_center_offset = (x_min + x_max) / 2.0 * ps->scale_x;
    double y_center_offset = (y_min + y_max) / 2.0 * ps->scale_y;

    // write PostScript file header
    fprintf(ps->file, "%%!PS-Adobe-3.0\n");
    fprintf(ps->file, "%%%%Creator: C Function Graph Generator\n");
    fprintf(ps->file, "%%%%Title: Function Graph\n");
    fprintf(ps->file, "%%%%Pages: 1\n");
    fprintf(ps->file, "%%%%EndComments\n");
    fprintf(ps->file, "300 400 translate\n");
    fprintf(ps->file, "%.2f %.2f translate\n", -x_center_offset, -y_center_offset);

    return ps;
}

/* ____________________________________________________________________________

    void draw_square_axis(postscript *ps)

    Draws square axes with X and Y bounds on the PostScript canvas

    Parameters:
        ps - A pointer to the PostScript structure

    Returns:
        Nothing. The axes are written to the PostScript file
   ____________________________________________________________________________
*/
void draw_square_axis(postscript *ps) {

    // sanity check
    if(!ps || !ps->file) return;

    // set line style
    fprintf(ps->file, "0.7 setlinewidth\n");
    fprintf(ps->file, "0 setgray\n");

    // draw the bottom axis
    fprintf(ps->file, "newpath\n");
    fprintf(ps->file, "%.2f %.2f moveto\n", ps->x_min * ps->scale_x, ps->y_min * ps->scale_y);
    fprintf(ps->file, "%.2f %.2f lineto\n", ps->x_max * ps->scale_x, ps->y_min * ps->scale_y);
    fprintf(ps->file, "stroke\n");

    // draw the top boundary line
    fprintf(ps->file, "newpath\n");
    fprintf(ps->file, "%.2f %.2f moveto\n", ps->x_min * ps->scale_x, ps->y_max * ps->scale_y);
    fprintf(ps->file, "%.2f %.2f lineto\n", ps->x_max * ps->scale_x, ps->y_max * ps->scale_y);
    fprintf(ps->file, "stroke\n");

    // draw the left boundary line
    fprintf(ps->file, "newpath\n");
    fprintf(ps->file, "%.2f %.2f moveto\n", ps->x_min * ps->scale_x, ps->y_min * ps->scale_y);
    fprintf(ps->file, "%.2f %.2f lineto\n", ps->x_min * ps->scale_x, ps->y_max * ps->scale_y);
    fprintf(ps->file, "stroke\n");

    // draw the right boundary line
    fprintf(ps->file, "newpath\n");
    fprintf(ps->file, "%.2f %.2f moveto\n", ps->x_max * ps->scale_x, ps->y_min * ps->scale_y);
    fprintf(ps->file, "%.2f %.2f lineto\n", ps->x_max * ps->scale_x, ps->y_max * ps->scale_y);
    fprintf(ps->file, "stroke\n");
}

/* ____________________________________________________________________________

    void draw_ticks_and_labels(postscript *ps)

    Draws tick marks and labels on the X and Y axes of the PostScript canvas

    Parameters:
        ps - A pointer to the PostScript structure

    Returns:
        Nothing. Tick marks and labels are written directly to the PostScript file
   ____________________________________________________________________________
*/
void draw_ticks_and_labels(postscript *ps) {

    // sanity check
    if(!ps || !ps->file) return;

    // set styles for tick marks and labels
    fprintf(ps->file, "0.5 setlinewidth\n");
    fprintf(ps->file, "0 setgray\n");
    fprintf(ps->file, "/Times-Roman findfont 12 scalefont setfont\n");

    // calculate grid intervals
    double y_grid_size = (ps->y_max - ps->y_min) / 8;
    double x_grid_size = (ps->x_max - ps->x_min) / 8;

    // draw tick marks and labels on the Y axis
    for(double y = ps->y_min; y <= ps->y_max; y += y_grid_size) {

        // convert Y coordinate to screen position
        double y_pos = y * ps->scale_y;

        // draw tick mark
        fprintf(ps->file, "newpath\n");
        fprintf(ps->file, "%.2f %.2f moveto\n", ps->x_min * ps->scale_x, y_pos);
        fprintf(ps->file, "%.2f %.2f lineto\n", ps->x_max * ps->scale_x, y_pos);
        fprintf(ps->file, "stroke\n");

        // draw label
        fprintf(ps->file, "%.2f %.2f moveto\n", ps->x_min * ps->scale_x - 20, y_pos - 3);
        fprintf(ps->file, "(%.1f) show\n", y);
    }

    // draw tick marks and labels on the X axis
    for(double x = ps->x_min; x <= ps->x_max; x += x_grid_size) {

       // convert X coordinate to screen position
       double x_pos = x * ps->scale_x;

        // draw tick mark
        fprintf(ps->file, "newpath\n");
        fprintf(ps->file, "%.2f %.2f moveto\n", x_pos, ps->y_min * ps->scale_y);
        fprintf(ps->file, "%.2f %.2f lineto\n", x_pos, ps->y_max * ps->scale_y);
        fprintf(ps->file, "stroke\n");

        // draw label
        fprintf(ps->file, "%.2f %.2f moveto\n", x_pos - 10, ps->y_min * ps->scale_y - 20);
        fprintf(ps->file, "(%.1f) show\n", x);
    }
}



/* ____________________________________________________________________________

    void draw_graph(postscript *ps)

    Draws the graph of a mathematical function on the PostScript canvas.

    Parameters:
        ps - A pointer to the PostScript structure.

    Returns:
        Nothing. The graph is written directly to the PostScript file.
   ____________________________________________________________________________
*/
void draw_graph(postscript *ps) {

    // sanity check
    if(!ps) return;

    // set the line style for the graph
    fprintf(ps->file, "1 setlinewidth\n");
    fprintf(ps->file, "0 0 1 setrgbcolor\n");
    fprintf(ps->file, "newpath\n");

    int pen_down = 1;

    // iterate through x values in the range
    for(double x = ps->x_min; x <= ps->x_max; x += 0.001) {

        // evaluate the function for the current x
        double y = evaluate_postfix_expression(ps->func, x);
		printf("%.2f %.2f lineto\n", x, y);

        // check if y is within the allowed range
        if(y < ps->y_min || y > ps->y_max || isnan(y)) {
            pen_down = 0;
            continue;
        }

        // convert coordinates to screen space
        double x_screen = x * ps->scale_x;
        double y_screen = y * ps->scale_y;

        // if the pen is not down move to the current point
        if(!pen_down) {
            fprintf(ps->file, "%.2lf %.2lf moveto\n", x_screen, y_screen);
            pen_down = 1;

        // if the pen is down continue drawing the line
        } else {
            if(x == ps->x_min)
                fprintf(ps->file, "%.2lf %.2lf moveto\n", x_screen, y_screen);
            else
                fprintf(ps->file, "%.2lf %.2lf lineto\n", x_screen, y_screen);
        }





    }
    // finish drawing the graph
    fprintf(ps->file, "stroke\n");
}

/* ____________________________________________________________________________

    void close_postscript(postscript *ps)

    Closes the PostScript file and releases resources.

    Parameters:
        ps - A pointer to the PostScript structure.

    Returns:
        Nothing. Frees the memory and closes the file.
   ____________________________________________________________________________
*/
void close_postscript(postscript *ps) {

    // sanity check
    if(!ps) return;

    // closing PostScript file
    if(ps->file) {
        fprintf(ps->file, "showpage\n");
        fclose(ps->file);
    }

    // free memory
    queue_free(&ps->func);
    free(ps);
}

