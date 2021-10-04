#include <stdio.h>
#include <stdlib.h>
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
        c2d_delete(cad);
        printf("<<< Test0 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

/* Draws a life is good page */
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
        /* Add a polyline */
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

        /* Add text */
        c2d_set_point(&start, -400, -400);
        l_text = c2d_add_text(cad, start, text);
        
        if (l_text != NULL) {
            c2d_set_rgb(&c, 0.3, 0, 1);
            c2d_set_text_style(cad, l_text, Coronet, c, fs_medium);
        }

        /* Delete specific entity */
            c2d_remove_entity(cad, &l_circle);
        /*
            c2d_remove_entity(cad, &l_text);
            c2d_remove_entity(cad, &l_pline);
        */

        c2d_export(cad, "test1.eps", "eps");
        c2d_delete(cad);
        printf("<<< Test1 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

/* Draws a Hello World page :) */
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
        c2d_delete(cad);
        printf("<<< Test2 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

/* Draws a sweet home */
void test3 () {
    CAD2D * cad_root, * cad_body, * cad_roof;
    Point2D p1, p2, p3;
    double  margin;
    double  body_x          = 400,  body_y      = 300;
    double  roof_w          = 1000, roof_h      = 250,
            chimney_w       = 70,   chimney_h   = 150,
            win_circle_r    = 100, 
            win_rect_w      = 150,  win_rect_h  = 350, 
            door_w          = 100,  door_h      = 250; 
    
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
            margin =  (body_x - (win_rect_w * 2)) * 3/4;

            c2d_set_point(&p1, -margin -win_rect_w, win_rect_h * 4/5);
            c2d_set_point(&p2, -margin, -win_rect_h * 1/4);
            c2d_add_rectangle(cad_body, p1, p2);

            c2d_set_point(&p2, -margin - 2 * win_rect_w, -win_rect_h * 2/5);
            c2d_add_rectangle(cad_body, p1, p2);
            
            /* Set door as two rectangle */
            c2d_set_point(&p1, body_x - margin, -body_y);
            c2d_set_point(&p2, body_x - margin - door_w, -body_y + door_h);
            c2d_add_rectangle(cad_body, p1, p2);

            c2d_set_point(&p1, body_x - margin - door_w, -body_y);
            c2d_set_point(&p2, body_x - margin - 2 * door_w, -body_y + door_h);
            c2d_add_rectangle(cad_body, p1, p2);
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

        /* To export all the hierarchy give ROOT CAD to export function */
        c2d_export(cad_root, "test3.eps", "eps");
        c2d_delete(cad_root);
        printf("<<< Test3 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

void test4 () {
    Point2D p1, p2, p3;
    CAD2D * root = c2d_start_wh(1000, 1000);
    CAD2D * cad1 = c2d_start_wh_hier(1000, 1000, root->hierarchy);
    CAD2D * cad2 = c2d_start_wh_hier(1000, 1000, root->hierarchy);
    CAD2D * cad3 = c2d_start_wh_hier(1000, 1000, root->hierarchy);
    CAD2D * cad4 = c2d_start_wh_hier(1000, 1000, root->hierarchy);
    int i, n;

    c2d_set_point(&p1, 100, 100);
    c2d_set_point(&p2, 200, 100);
    c2d_set_point(&p3, 100, 200);
    c2d_add_triangle(root, p1, p2, p3);

    for (i = 0; i < 1000; i += 100) {
        c2d_set_point(&p1, -i, -i);
        c2d_set_point(&p2, i, i);
        c2d_add_rectangle(cad1, p1, p2);
    }
/*
    c2d_delete_hierarchy(cad2->hierarchy);
    c2d_delete_hierarchy(cad3->hierarchy);
    c2d_delete_hierarchy(cad4->hierarchy);
    c2d_delete_hierarchy(cad1->hierarchy);
*/
    
    c2d_export(root, "test4.eps", "eps");
    c2d_delete(root);
    printf("<<< Test4 DONE >>>\n\n");
}

/* Draws a more sweet home */
void test5 () {
    //! NOT IMPLEMENTED YET
    CAD2D * cad;

    cad = c2d_start_wh(1500, 1500);
    if (cad != NULL) {
        
        c2d_export(cad, "test5.eps", "eps");
        c2d_delete(cad);    
        printf("<<< Test5 DONE >>>\n\n");
    } 
}

/* Draw an engine */
void test6 () {
    CAD2D * cad;
    double  canvas_size = 1600.0,
            plane_size = canvas_size / 2,
            k = plane_size / 20;
    Point2D pc, p1, p2, p3;

    cad = c2d_start_wh(canvas_size, canvas_size);
    if (cad != NULL) {
        /* Left side */
        /* Set center point for circles */
        c2d_set_point(&pc, -plane_size * 1/3, 0);    
        p3.x = pc.x;
        p3.y = 7 * k;

        /* Add half circle whose radius is 7k */
        c2d_add_arc(cad, pc, 7 * k, 90, 270);
        
        /* Add two circle whose radius are 4k and 3k */
        c2d_add_circle(cad, pc, 4 * k);
        c2d_add_circle(cad, pc, 3 * k);

        pc.y = 5.5 * k; 
        c2d_add_circle(cad, pc, 0.5 * k);

        pc.y = -5.5 * k; 
        c2d_add_circle(cad, pc, 0.5 * k);

        pc.x = -12.5 * k, pc.y = 0;
        c2d_add_circle(cad, pc, 0.5 * k);

        /* Right side */
        c2d_set_point(&pc, 4 * k, 0);
        c2d_add_arc(cad, pc, 2 * k, 90, 270);
        
        /* To add line later, keep center point inside p1 with changing it's y value */
        p1.x = pc.x;
        p1.y = 2 * k;

        /* Add two arc whose radius are 2k and 4k */
        pc.x += 7 * k;
        c2d_add_arc(cad, pc, 2 * k, 270, 90);
        c2d_add_arc(cad, pc, 4 * k, 270, 90);

        p2.x = pc.x;
        p2.y = 2 * k;
        c2d_add_line(cad, p1, p2);

        p1.y = p2.y = -2 * k;
        c2d_add_line(cad, p1, p2);
       
        p2.y = 4 * k;
        c2d_add_line(cad, p3, p2);
        
        p3.y *= -1;
        p2.y *= -1;
        c2d_add_line(cad, p3, p2);

        c2d_export(cad, "test6.eps", "eps");
        c2d_delete(cad);
        printf("<<< Test6 DONE >>>\n\n");
    } 
}

int main (void) {
    test0();
    //! https://web.stanford.edu/class/archive/cs/cs107/cs107.1218/resources/valgrind.html
    //! Handle errors casued invalid write ... 
    test0();
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    /**
    **/
}