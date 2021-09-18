#include <stdio.h>
#include "cad2d.h"

void test0 () {
    CAD2D * cad;    
    Point2D start, end, center;
    double radius = 50, canvas_size = 500;

    cad = c2d_start_wh(canvas_size, canvas_size);
    
    if (cad != NULL) {
        /* draw arcs at the center of canvas */
        center.x = center.y = 0;
        c2d_add_arc (cad, center, radius, 30, 150);
        c2d_add_arc (cad, center, radius, 210, 330);
        
        /* draw x = y line */
        start.y = start.x = -canvas_size / 2;
        end.x = end.y = canvas_size / 2;

        c2d_add_line(cad, &start, &end);
        
        start.x = start.y = -20.0;
        end.x = end.y = 20.0;
        c2d_add_rectangle(cad, start, end);

        c2d_export(cad, "test0.eps", "eps");
        printf("Test0: DONE\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

void test1 () {
    CAD2D * cad;
    Point2D * p;

    cad = c2d_start_wh(1000, 1000);

    if (cad != NULL) {
        p = c2d_create_point(0, 0);
        p->next = c2d_create_point(5, 5);

        c2d_add_polyline(cad, p);

        c2d_export(cad, "test1.eps", "eps");
        printf("Test1: DONE\n");
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
    test1();
    /*
    test0();
    test2();
    */
}