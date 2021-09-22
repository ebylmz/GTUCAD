#include <stdio.h>
#include <stdlib.h>
#include "cad2d.h"

void test0 () {
    CAD2D * cad;    
    Point2D start, end, center;
    double radius = 50, canvas_size = 500;

    cad = c2d_start_wh(canvas_size, canvas_size);
    
    if (cad != NULL) {
        /* draw arcs at the center of canvas */
        c2d_set_point(&center, 0, 0, NULL);
        c2d_add_arc(cad, center, radius, 30, 150);
        c2d_add_arc(cad, center, radius, 210, 330);
        
        /* draw x = y line */
        c2d_set_point(&start, -canvas_size / 2, -canvas_size / 2, NULL);
        c2d_set_point(&end, canvas_size / 2, canvas_size / 2, NULL);
        c2d_add_line(cad, start, end);

        /* draw an rectangle */
        c2d_set_point(&start, -20, -20, NULL);
        c2d_set_point(&end, 20, 20, NULL);
        c2d_add_rectangle(cad, start, end);

        /* export the data as eps mode */
        c2d_export(cad, "test0.eps", "eps");
        printf("<<< Test0 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

void test1 () {
    CAD2D * cad;
    Point2D start;
    Label * l_circle, * l_pline;
    TextStyle * style;
    char * text = "Life is good";
    int x = 400, n = 5;
    RGBColor c1 = {0, 0, 0}, c2 = {1, 0.5, 0};
    Point2D p[9] = {{-x, 0}, {-x/n, x/n}, {0, x}, {x/n, x/n}, {x, 0},
                    {x/n, -x/n}, {0, -x}, {-x/n, -x/n}, {-x, 0}};
    
    cad = c2d_start_wh(1000, 1000);
    
    if (cad != NULL) {        
        /* add a filled polyline */
        l_pline = c2d_add_polyline(cad, p, 9);
        c2d_set_entity_style(cad, l_pline, dashed, c1, fill, 2);

        c2d_set_point(&start, 0, 0, NULL);
        l_circle = c2d_add_circle(cad, start, x / n);
        c2d_set_entity_style(cad, l_circle, solid, c2, stroke, 2);

        //! It would be better to gettig style without dynamic memory allocation 
        c2d_set_point(&start, -400, -400, NULL);
        style = c2d_create_text_style(Coronet, c1, 40);
        c2d_add_text(cad, start, text, style);

        c2d_export(cad, "test1.eps", "eps");
        free(style);
        printf("<<< Test1 DONE >>>\n\n");
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
    /**
    **/
    test0();
    test1();
    /**
    test2();
    **/
}