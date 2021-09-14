#include <stdio.h>
#include "cad2d.h"

/* Draw a line */
void test0 () {
    CAD2D * cad = c2d_start_wh(500, 500);    
    Point2D start, end;
    
    if (cad != NULL) {
        start.y = start.x = 5;
        end.y = end.x = 100;

        c2d_add_line(cad, &start, &end);

        c2d_export_eps(cad, "test0.eps");

        printf("Test0 - Draw Line: DONE\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

/*
void test1 () {
    CAD2D * cad;
    Point2D p = {10.0, 10.0};
    Point2D pl[3] = {{10.0, 10.0}, {10.0, 20.0}, {20.0, 20.0}}; 

    cad = c2d_start(1000, 1000); 

    c2d_add_point_xy(cad, 0.0, 0.0);
    c2d_add_point_p(cad, p);

    c2d_add_polyline_p2(cad, pl, 3);

    c2d_export(cad, "test1.gtucad", "gtucad");
    c2d_export(cad, "test1.eps", "eps");

    c2d_delete(cad);
}

void test2 () {
    CAD2D * cad;

    cad = c2d_import("test1.gtucad");

    c2d_add_point_xy(cad, 10.0, 0.0);

    c2d_export(cad, "test1.eps", "eps");

    c2d_delete(cad);   
}

*/


int main (void) {
    test0(0);
    // test1();
    // test2();
}