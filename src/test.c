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
        c2d_set_point(&center, 0, 0);
        c2d_add_regular_polygon(cad, 5, center, 100);        
        
        /* draw arcs at the center of canvas */
        c2d_add_arc(cad, center, radius, 45 + 30, 225 - 30);
        c2d_add_arc(cad, center, radius, 225 + 30, 15);
        
        /* draw x = y line */
        c2d_set_point(&start, -canvas_size / 2, -canvas_size / 2);
        c2d_set_point(&end, canvas_size / 2, canvas_size / 2);
        c2d_add_line(cad, start, end);

        /* draw an rectangle */
        c2d_set_point(&start, 20, -20);
        c2d_set_point(&end, -20, 20);
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
        /* Add a filled polyline(polygon) */
        l_pline = c2d_add_polyline(cad, p, 9);

        if (l_pline != NULL) {
            c2d_set_rgb(&c, 0, 0, 0);
            c2d_set_entity_style(cad, l_pline, dashed, c, fill, lw_medium);
        }

        /* Add a dashed circle */
        c2d_set_point(&start, 0, 0);  
        l_circle = c2d_add_circle(cad, start, x / (n - 1));

        if (l_circle != NULL) {
            c2d_set_rgb(&c, 1, 0, 1);
            c2d_set_entity_style(cad, l_circle, solid, c, stroke, lw_medium);
        }

        c2d_remove_entity(cad, &l_circle);

        /* Add text */
        c2d_set_point(&start, -400, -400);
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
        c2d_set_point(&center, 0, 0);

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
        c2d_set_point(&center, -80, -200);
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

void test3 () {
    CAD2D * cad_root, * cad_body, * cad_roof;
    Point2D p1, p2, p3;
    double body_x = 400, body_y = 300;
    double margin;
    double  roof_w = 1000, roof_h = 250,
            chimney_w = 70, chimney_h = 150,
            win_circle_r = 150, 
            win_rect_w = 150, win_rect_h = 350; 

    cad_root = c2d_start_wh(1500, 1500);
    if (cad_root != NULL) {

        cad_body = c2d_start_wh_hier(1500, 1500, cad_root->hierarchy);
        if (cad_body != NULL) {
            /* Set body part of the building */
            c2d_set_point(&p1, -body_x, -body_y);
            c2d_set_point(&p2, body_x, body_y);
            c2d_add_rectangle(cad_body, p1, p2);

            /* Set a circle window */
            c2d_set_point(&p1, body_x / 2, body_y / 2);
            c2d_add_circle(cad_body, p1, win_circle_r);

            /* Set two rectangle windows */
            margin =  (body_x - (win_rect_w * 2)) / 2;

            c2d_set_point(&p1, -margin -win_rect_w, win_rect_h * 4/5);
            c2d_set_point(&p2, -margin, -win_rect_h * 1/4);
            c2d_add_rectangle(cad_body, p1, p2);

            c2d_set_point(&p2, -margin - 2 * win_rect_w, -win_rect_h * 2/5);
            c2d_add_rectangle(cad_body, p1, p2);
        
            /* Set door as two rectangle */
        }

        cad_roof = c2d_start_wh_hier(1500, 1500, cad_root->hierarchy);
        if (cad_roof != NULL) {
            /* Set roof */
            c2d_set_point(&p1, -roof_w / 2, body_y);
            c2d_set_point(&p2, 0, body_y + roof_h);
            c2d_set_point(&p3, roof_w / 2, body_y);
            c2d_add_triangle(cad_roof, p1, p2, p3);

            /* Set chimney */
            c2d_set_point(&p1, body_x / 2 - chimney_w / 2, body_y + roof_h * 1/3);
            c2d_set_point(&p2, body_x / 2 + chimney_w / 2, body_y + roof_h * 1/3 + chimney_h);
            c2d_add_rectangle(cad_roof, p1, p2);
        }

        /* export the root cad */
        c2d_export(cad_root, "test3.eps", "eps");
        printf("<<< Test3 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

void test4 () {
    Point2D p1, p2, p3;
    CAD2D * cad_root = c2d_start_wh(1000, 1000);
    CAD2D * data = c2d_start_wh_hier(1000, 1000, cad_root->hierarchy);
    CAD2D * cse = c2d_start_wh_hier(1000, 1000, cad_root->hierarchy);
    CAD2D * security = c2d_start_wh_hier(1000, 1000, cad_root->hierarchy);
    CAD2D * home = c2d_start_wh_hier(1000, 1000, cad_root->hierarchy);
    
    c2d_set_point(&p1, 100, 100);
    c2d_set_point(&p2, 200, 100);
    c2d_set_point(&p3, 100, 200);
    c2d_add_triangle(cad_root, p1, p2, p3);

    c2d_export(cad_root, "test4.eps", "eps");
    printf("<<< Test4 DONE >>>\n\n");
}

int main (void) {
    /**
    test0();
    test1();
    test2();
    **/
    test3();
    /**
    test4();
    **/
}