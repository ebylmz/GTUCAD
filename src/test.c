#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include "cad2d.h"

void test0 () {
    CAD2D * cad;    
    Point2D start, end, center;
    double radius = 50, canvas_size = 500;

    cad = c2d_start_wh(canvas_size, canvas_size);
    
    if (cad != NULL) {
        /* draw a hexagon */
        c2d_set_point(&center, 0, 0, NULL);
        c2d_add_regular_polygon(cad, 6, center, 100);        
        
        /* draw arcs at the center of canvas */
        c2d_add_arc(cad, center, radius, 45 + 30, 225 - 30);
        c2d_add_arc(cad, center, radius, 225 + 30, 15);
        
        /* draw x = y line */
        c2d_set_point(&start, -canvas_size / 2, -canvas_size / 2, NULL);
        c2d_set_point(&end, canvas_size / 2, canvas_size / 2, NULL);
        c2d_add_line(cad, start, end);

        /* draw an rectangle */
        c2d_set_point(&start, 20, -20, NULL);
        c2d_set_point(&end, -20, 20, NULL);
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
    RGBColor c;
    Label * l_circle, * l_pline, * l_text;
    char * text = "Life is good";
    int x = 400, n = 6;
    Point2D p[9] = {{-x, 0}, {-x/n, x/n}, {0, x}, {x/n, x/n}, {x, 0},
                    {x/n, -x/n}, {0, -x}, {-x/n, -x/n}, {-x, 0}};
    
    cad = c2d_start_wh(1000, 1000);
    
    if (cad != NULL) {        
        /* Add a filled polyline or polygone */
        l_pline = c2d_add_polyline(cad, p, 9);

        if (l_pline != NULL) {
            c2d_set_rgb(&c, 0, 0, 0);
            c2d_set_entity_style(cad, l_pline, dashed, c, fill, lw_medium);
        }

        /* Add a dashed circle */
        c2d_set_point(&start, 0, 0, NULL);  
        l_circle = c2d_add_circle(cad, start, x / (n - 1));

        if (l_circle != NULL) {
            c2d_set_rgb(&c, 1, 0, 1);
            c2d_set_entity_style(cad, l_circle, solid, c, stroke, lw_medium);
        }

        /* Add text */
        c2d_set_point(&start, -400, -400, NULL);
        l_text = c2d_add_text(cad, start, text);
        
        if (l_text != NULL) {
            c2d_set_rgb(&c, 0.3, 0, 1);
            c2d_set_text_style(cad, l_text, Coronet, c, fs_medium);
        }

        c2d_remove_entity(cad, &l_text);

        c2d_export(cad, "test1.eps", "eps");
        printf("<<< Test1 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

void test2 () {
    CAD2D * cad;    
    Point2D center;
    Label * l_ellipse1, * l_ellipse2, * l_text;
    RGBColor c;
    char * text = "Hello, World!";

    double radius = 120, canvas_size = 500;

    cad = c2d_start_wh(canvas_size, canvas_size);

    if (cad != NULL) {
        c2d_set_point(&center, 0, 0, NULL);

        /* draw first ellipse as 2/1 ratio */
        l_ellipse1 = c2d_add_ellipse(cad, center, radius * 2, radius);

        if (l_ellipse1 != NULL) {
            c2d_set_rgb(&c, 0, 0.5, 0);
            c2d_set_entity_style(cad, l_ellipse1, solid, c, stroke, lw_medium);
        }
        
        /* draw first ellipse as 1/2 ratio */
        l_ellipse2 = c2d_add_ellipse(cad, center, radius, radius * 2);

        if (l_ellipse2 != NULL) {
            c2d_set_rgb(&c, 0, 0.5, 1);
            c2d_set_entity_style(cad, l_ellipse2, solid, c, stroke, lw_medium);
        }        

        /* Add text */
        c2d_set_point(&center, -80, -200, NULL);
        l_text = c2d_add_text(cad, center, text);
        
        if (l_text != NULL) {
            c2d_set_rgb(&c, 1, 0, 0.5);
            c2d_set_text_style(cad, l_text, Courier, c, fs_small);
        }

        c2d_export(cad, "test2.eps", "eps");
        printf("<<< Test2 DONE >>>\n\n");
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
    test2();
    /**
    **/
}