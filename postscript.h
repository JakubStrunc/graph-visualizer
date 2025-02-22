#ifndef POSTSCRIPT_H
#define POSTSCRIPT_H

#include <stdio.h>
#include "queue.h"


/* ____________________________________________________________________________

    Function Prototypes
   ____________________________________________________________________________
*/

typedef struct {
    FILE *file;
    queue *func;
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    double scale_x;
    double scale_y;
} postscript;

postscript *create_postscript(const char *filename, const char *func, double x_min, double x_max, double y_min, double y_max);

void draw_square_axis(postscript *ps);

void draw_ticks_and_labels(postscript *ps);

void draw_graph(postscript *ps);

void close_postscript(postscript *ps);

#endif // POSTSCRIPT_H
