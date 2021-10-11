#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cad2d.h"

void test0 () {
    CAD2D * cad;    
    Point2D start, end, center;
    double radius = 50.0, canvas_size = 800.0;
    RGBColor c;
    Label * l;
    cad = c2d_start_wh(canvas_size, canvas_size);
    
    if (cad != NULL) {
        c2d_set_color_pallette(&c, red);
        
        /* Add points by using two different function */
        l = c2d_add_point_xy(cad, -125.0, 125.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, DEFAULT);
        
        c2d_set_color_pallette(&c, blue);
        c2d_set_point(&start, 125.0, -125.0);
        
        l = c2d_add_point_p(cad, start);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, DEFAULT);

        /* Add a hexagon */
        c2d_set_point(&center, 0.0, 0.0);
        c2d_add_regular_polygon(cad, 5, center, 100.0);        
        
        /* Add arcs at the center of canvas */
        c2d_add_arc(cad, center, radius, 45.0 + 30.0, 225.0 - 30.0);
        c2d_add_arc(cad, center, radius, 225.0 + 30.0, 15.0);
        
        /* Add x = y line */
        c2d_set_point(&start, -canvas_size / 2, -canvas_size / 2);
        c2d_set_point(&end, canvas_size / 2, canvas_size / 2);
        c2d_add_line(cad, start, end);

        /* Add a rectangle */
        c2d_set_point(&start, 20.0, -20.0);
        c2d_set_point(&end, -20.0, 20.0);
        c2d_add_rectangle(cad, start, end);

        c2d_export(cad, "test0.gtucad", gtucad);
        c2d_export(cad, "test0.eps", eps);
        c2d_delete(cad);
        printf("<<< Test0 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

void test00 () {
    CAD2D * cad = c2d_import("test0.gtucad", gtucad);
    c2d_export(cad, "test00.eps", eps);
    c2d_delete(cad);
    printf("<<< Test00 DONE >>>\n\n");
}

/* Draws a life is good page */
void test1 () {
    CAD2D * cad;
    Point2D start;
    RGBColor c;
    double k = 40.0;
    Label * l_sun, * l_mount;
    int x = 400, n = 6;
    Point2D p[6];
    
    cad = c2d_start_wh(1000.0, 1000.0);
    
    if (cad != NULL) {        
        /* Draw a mountain with polyline */
        p[0].x = -12 * k, p[0].y = 0.0;
        p[1].x =  -3 * k, p[1].y = 7 * k;
        p[2].x =       k, p[2].y = 5 * k;
        p[3].x =   7 * k, p[3].y = 9 * k;
        p[4].x =  12 * k, p[4].y = 0.0;
        p[5].x =  -5 * k, p[5].y = 0.0;
        l_mount = c2d_add_polyline(cad, p, 6);
        c2d_set_color_pallette(&c, green_dark);
        c2d_set_entity_style(cad, l_mount, DEFAULT, c, fill, DEFAULT);

        /* Draw sun as a circle */
        c2d_set_point(&start, 0.0, 9 * k);  
        l_sun = c2d_add_circle(cad, start, x / (n - 1));
        c2d_set_color_pallette(&c, orange);
        c2d_set_entity_style(cad, l_sun, DEFAULT, c, fill, DEFAULT);

        /* Add text */
        c2d_set_point(&start, -2 * k, -2 * k);
        c2d_set_color_rgb(&c, 0.2, 0.5, 1.0);
        c2d_add_text(cad, start, "Life is good", Coronet, c, fs_medium);

        /* Delete specific entity */
        /*
            c2d_remove_entity(cad, &l_sun);
            c2d_remove_entity(cad, &l_mount);
            c2d_remove_entity(cad, &l_text);
        */

        c2d_export(cad, "test1.eps", eps);
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
    Label * l;
    RGBColor c;

    double radius = 120.0, canvas_size = 500.0;

    cad = c2d_start_wh(canvas_size, canvas_size);

    if (cad != NULL) {
        c2d_set_point(&center, 0.0, 0.0);

        /* draw first ellipse as 2/1 ratio */
        l = c2d_add_ellipse(cad, center, radius * 2, radius);
        c2d_set_color_rgb(&c, 0.0, 0.5, 0.0);
        c2d_set_entity_style(cad, l, solid, c, stroke, lw_medium);
        
        /* draw first ellipse as 1/2 ratio */
        l = c2d_add_ellipse(cad, center, radius, radius * 2);
        c2d_set_color_rgb(&c, 0.0, 0.5, 1.0);
        c2d_set_entity_style(cad, l, solid, c, stroke, lw_medium);

        /* Add text */
        c2d_set_point(&center, -80.0, -200.0);
        c2d_set_color_rgb(&c, 1.0, 0.0, 0.5);
        c2d_add_text(cad, center, "Hello, World!", Courier, c, fs_small);

        c2d_export(cad, "test2.eps", eps);
        c2d_delete(cad);
        printf("<<< Test2 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

/* Draws a sweet home */
void test3 () {
    CAD2D   * cad_root, * cad_body, * cad_roof;
    Point2D p1, p2, p3;
    RGBColor c_gray, c_black, c_white;
    Label   * l_win_circ, * l_win_rect1, * l_win_rect2, * l_body_rect, 
            * l_roof, * l_chimney, * l_door1, * l_door2;
    double  margin, space = 2,
            body_x          = 400.0,  body_y      = 300.0,
            roof_w          = 1000.0, roof_h      = 250.0,
            chimney_w       = 70.0,   chimney_h   = 150.0,
            win_circ_r      = 120.0, 
            win_rect_w      = 150.0,  win_rect_h  = 350.0, 
            door_w          = 100.0,  door_h      = 250.0; 

    cad_root = c2d_start();
    if (cad_root != NULL) {
        cad_body = c2d_start_wh_hier(1500.0, 1500.0, cad_root->hierarchy);
        
        /* Set colors */
        c2d_set_color_rgb(&c_gray, 0.5, 0.5, 0.5);
        c2d_set_color_rgb(&c_black, 0.2, 0.2, 0.2);
        c2d_set_color_rgb(&c_white, 0.8, 0.8, 0.8);

        if (cad_body != NULL) {
            /* Set body part of the building */
            c2d_set_point(&p1, -body_x, -body_y);
            c2d_set_point(&p2, body_x, body_y);
            l_body_rect = c2d_add_rectangle_l(cad_body, "body-rect", p1, p2);
            c2d_set_entity_style(cad_body, l_body_rect, DEFAULT, c_gray, fill, DEFAULT);

            /* Set a circle window */
            c2d_set_point(&p1, body_x / 2.0, body_y / 2.0);
            l_win_circ = c2d_add_circle_l(cad_body, "wind-circ", p1, win_circ_r);
            c2d_set_entity_style(cad_body, l_win_circ, DEFAULT, c_white, fill, DEFAULT);

            /* Set two rectangle windows */
            margin =  (body_x - (win_rect_w * 2.0)) * 3.0/4.0;

            c2d_set_point(&p1, -margin -win_rect_w, win_rect_h * 4.0/5.0);
            c2d_set_point(&p2, -margin, -win_rect_h * 1.0/4.0);
            l_win_rect1 = c2d_add_rectangle_l(cad_body, "wind-rec1", p1, p2);
            c2d_set_entity_style(cad_body, l_win_rect1, DEFAULT, c_white, fill, DEFAULT);

            p1.x -= space;
            c2d_set_point(&p2, -margin - 2 * win_rect_w, -win_rect_h * 2.0/5.0);
            l_win_rect2 = c2d_add_rectangle_l(cad_body, "wind-rec2", p1, p2);
            c2d_set_entity_style(cad_body, l_win_rect2, DEFAULT, c_white, fill, DEFAULT);

            /* Set door as two rectangle */
            c2d_set_point(&p1, body_x - margin, -body_y);
            c2d_set_point(&p2, body_x - margin - door_w, -body_y + door_h);
            l_door1 = c2d_add_rectangle_l(cad_body, "door-rec1", p1, p2);
            c2d_set_entity_style(cad_body, l_door1, DEFAULT, c_black, fill, DEFAULT);
                
            c2d_set_point(&p1, body_x - margin - door_w - space, -body_y);
            c2d_set_point(&p2, body_x - margin - 2 * door_w, -body_y + door_h);
            l_door2 = c2d_add_rectangle_l(cad_body, "door-rec2", p1, p2);
            c2d_set_entity_style(cad_body, l_door2, DEFAULT, c_black, fill, DEFAULT);
        }

        cad_roof = c2d_start_wh_hier(1500.0, 1500.0, cad_root->hierarchy);
        if (cad_roof != NULL) {
            /* Set chimney */
            c2d_set_point(&p1, body_x / 2.0 - chimney_w / 2.0, body_y + roof_h * 1.0/3.0);
            c2d_set_point(&p2, body_x / 2.0 + chimney_w / 2.0, body_y + roof_h * 1.0/3.0 + chimney_h);
            l_chimney = c2d_add_rectangle_l(cad_roof, "chimney-rec", p1, p2);
            c2d_set_entity_style(cad_roof, l_chimney, DEFAULT, c_gray, fill, DEFAULT);

            /* Set roof */
            c2d_set_point(&p1, -roof_w / 2.0, body_y);
            c2d_set_point(&p2, 0.0, body_y + roof_h);
            c2d_set_point(&p3, roof_w / 2.0, body_y);
            l_roof = c2d_add_triangle_l(cad_roof, "roof-tri", p1, p2, p3);
            c2d_set_entity_style(cad_roof, l_roof, DEFAULT, c_black, fill, DEFAULT);
        }

        /* Export in gtucad mode to continue later */
        c2d_export(cad_root, "test3.gtucad", gtucad);
        c2d_delete(cad_root);
        printf("<<< Test3 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

void test4 () {
    CAD2D * cad = c2d_import("test3.gtucad", gtucad);
    if (cad != NULL) {
        c2d_export(cad, "test4.eps", eps);
        c2d_delete(cad);
        printf("<<< Test4 DONE >>>\n\n");
    }
    else 
        printf("CAD cannot started properly\n");
}

/* Draw rectangles by using new hierarchies */
void test5 () {
    CAD2D * root, * cad1, * cad2, * cad3, * cad4;
    Point2D p1, p2, p3;
    RGBColor c;
    Label * l;
    double d;
    int i;

    root = c2d_start();
    if (NULL != root) {
        for (i = red, d = 1000; d > 0; ++i, d -= 100) {
            if (i >= brown + 1)
                i %= brown;
            c2d_set_point(&p1, -d, -d);
            c2d_set_point(&p2, d, d);
            c2d_set_color_pallette(&c, i);
            l = c2d_add_rectangle(root, p1, p2);
            c2d_set_entity_style(root, l, DEFAULT, c, fill, DEFAULT);
        }

        cad1 = c2d_start_wh_hier(1000.0, 1000.0, root->hierarchy),
        cad2 = c2d_start_wh_hier(1000.0, 1000.0, root->hierarchy),
        cad3 = c2d_start_wh_hier(1000.0, 1000.0, root->hierarchy),
        cad4 = c2d_start_wh_hier(1000.0, 1000.0, root->hierarchy);
        if (NULL != cad1 && NULL != cad2 && NULL != cad3 && NULL != cad4) {
            c2d_set_point(&p1, 200.0, 200.0);
            c2d_set_point(&p2, 300.0, 200.0);
            c2d_set_point(&p3, 200.0, 300.0);
            c2d_add_triangle(cad1, p1, p2, p3);

            c2d_set_point(&p1, -200.0, 200.0);
            c2d_set_point(&p2, -300.0, 200.0);
            c2d_set_point(&p3, -200.0, 300.0);
            c2d_add_triangle(cad2, p1, p2, p3);

            c2d_set_point(&p1, 200.0, -200.0);
            c2d_set_point(&p2, 300.0, -200.0);
            c2d_set_point(&p3, 200.0, -300.0);
            c2d_add_triangle(cad3, p1, p2, p3);

            c2d_set_point(&p1, -200.0, -200.0);
            c2d_set_point(&p2, -300.0, -200.0);
            c2d_set_point(&p3, -200.0, -300.0);
            c2d_add_triangle(cad4, p1, p2, p3);

            /* Deletes child hierarchies and their entities(triangle) */
        /*
            c2d_delete_hierarchy(cad1->hierarchy);
            c2d_delete_hierarchy(cad2->hierarchy);
            c2d_delete_hierarchy(cad3->hierarchy);
            c2d_delete_hierarchy(cad4->hierarchy);
        */
        }
        c2d_export(root, "test5.gtucad", gtucad);
        c2d_export(root, "test5.eps", eps);
        c2d_delete(root);
        printf("<<< Test5 DONE >>>\n\n");
    }
}


void test55 () {
    CAD2D * cad = c2d_import("test5.gtucad", gtucad);
    c2d_export(cad, "test55.eps", eps);
    c2d_delete(cad);
    printf("<<< Test55 DONE >>>\n\n");
}

/* Draw an engine */
void test6 () {
    CAD2D * cad;
    double  plane_size = 1600.0 / 2.0,
            k = plane_size / 20.0;
    Point2D pc, p1, p2, p3;
    RGBColor c;
    Label * l;

    cad = c2d_start();
    if (cad != NULL) {
        /* Left side */
        /* Set center point for circles */
        c2d_set_point(&pc, -plane_size * 1.0/3.0, 0.0);    
        p3.x = pc.x;
        p3.y = 7 * k;

        /* Add half circle whose radius is 7k */
        c2d_add_arc(cad, pc, 7 * k, 90.0, 270.0);

        /* Add two circle whose radius are 4k and 3k */
        
        c2d_add_circle(cad, pc, 4 * k);

        c2d_set_color_pallette(&c, black);
        l = c2d_add_circle(cad, pc, 3 * k);
        c2d_set_entity_style(cad, l, DEFAULT, c, fill, lw_large);
        
        pc.y = 5.5 * k; 
        c2d_set_color_pallette(&c, red);
        l = c2d_add_circle(cad, pc, 0.5 * k);
        c2d_set_entity_style(cad, l, DEFAULT, c, stroke, lw_large);

        pc.y = -5.5 * k; 
        l = c2d_add_circle(cad, pc, 0.5 * k);
        c2d_set_entity_style(cad, l, DEFAULT, c, stroke, lw_large);

        pc.x = -12.5 * k, pc.y = 0.0;
        l = c2d_add_circle(cad, pc, 0.5 * k);
        c2d_set_entity_style(cad, l, DEFAULT, c, stroke, lw_large);

        /* Right side */
        c2d_set_point(&pc, 4 * k, 0.0);
        l = c2d_add_arc(cad, pc, 2 * k, 90.0, 270.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, stroke, lw_large);
        
        /* To add line later, keep center point inside p1 with changing it's y value */
        p1.x = pc.x;
        p1.y = 2.0 * k;

        /* Add two arc whose radius are 2k and 4k */
        pc.x += 7 * k;
        l = c2d_add_arc(cad, pc, 2 * k, 270.0, 90.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, stroke, lw_large);
        
        c2d_add_arc(cad, pc, 4 * k, 270.0, 90.0);

        p2.x = pc.x;
        p2.y = 2.0 * k;
        l = c2d_add_line(cad, p1, p2);
        c2d_set_entity_style(cad, l, DEFAULT, c, stroke, lw_large);

        p1.y = p2.y = -2 * k;
        l =  c2d_add_line(cad, p1, p2);
        c2d_set_entity_style(cad, l, DEFAULT, c, stroke, lw_large);
       
        p2.y = 4.0 * k;
        c2d_add_line(cad, p3, p2);
        
        p3.y *= -1;
        p2.y *= -1;
        c2d_add_line(cad, p3, p2);

        c2d_export(cad, "test6.eps", eps);
        c2d_export(cad, "test6.gtucad", gtucad);

        c2d_delete(cad);
        printf("<<< Test6 DONE >>>\n\n");
    } 
}

/* Draws 2D shapes on xy plane */
void test7 () {
    CAD2D * cad;
    Point2D p[6];
    int psize = 6;
    Point2D center, c1, c2, c3;
    RGBColor c;
    Label * l_rec, * l_reg_poly, * l_tri, * l_irr_poly; 

    cad = c2d_start();
    if (NULL != cad) {
        /* Show xy plane */
        c2d_add_xy_plane(cad);
        
        /* Add an irregular polygon */
        p[0].x = 100.0, p[0].y = 200.0;
        p[1].x = 150.0, p[1].y = 300.0;
        p[2].x = 220.0, p[2].y = 120.0;
        p[3].x = 400.0, p[3].y = 240.0;
        p[4].x = 220.0, p[4].y =  60.0;
        p[5].x = 100.0, p[5].y = 200.0;

        c2d_set_color_pallette(&c, magenta);
        l_irr_poly = c2d_add_irregular_polygon(cad, p, psize);
        c2d_set_entity_style(cad, l_irr_poly, DEFAULT, c, fill, DEFAULT);

        /* Add a rectangle polygon */
        c2d_set_color_pallette(&c, blue_light);
        c2d_set_point(&c1, -140.0, 140.0);
        c2d_set_point(&c2, -340.0, 440.0);
        l_rec = c2d_add_rectangle(cad, c1, c2);
        c2d_set_entity_style(cad, l_rec, DEFAULT, c, fill, DEFAULT);

        /* Add a triangle */
        c2d_set_color_pallette(&c, green_dark);
        c2d_set_point(&c1, -240.0, -240.0);
        c2d_set_point(&c2, -100.0, -340.0);
        c2d_set_point(&c3, -340.0, -440.0);
        l_tri = c2d_add_triangle(cad, c1, c2, c3);
        c2d_set_entity_style(cad, l_tri, DEFAULT, c, fill, DEFAULT);


        /* Add a octagon(regular polygon) */
        c2d_set_color_pallette(&c, red);
        c2d_set_point(&center, 240.0, -240.0);
        l_reg_poly = c2d_add_regular_polygon(cad, 9, center, 100.0);
        c2d_set_entity_style(cad, l_reg_poly, DEFAULT, c, fill, DEFAULT);

        c2d_export(cad, "test7.eps", eps);
        c2d_delete(cad);
        printf("<<< Test7 DONE >>>\n\n");
    }
}

/* Writes GTUCAD */
void test8 () {
    CAD2D * cad;
    Point2D p1, p2, p3;
    double k = 40.0;
    RGBColor c;
    Label * l;
    Point2D p_arr[10];

    cad = c2d_start();
    if (cad != NULL) {
        /* c2d_add_xy_plane(cad); */
        c2d_set_color_pallette(&c, black);

        /* Draw letter G with arc and polyline */
        c2d_set_point(&p1, -15.5 * k, 4.5 * k);
        l = c2d_add_arc(cad, p1, 2.5 * k, 45.0, 0.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        l = c2d_add_arc(cad, p1, 1.5 * k, 45.0, 320.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        p_arr[0].x = -13   * k, p_arr[0].y = 4.5 * k;
        p_arr[1].x = -15.5 * k, p_arr[1].y = 4.5 * k; 
        p_arr[2].x = -15.5 * k, p_arr[2].y = 3.5 * k; 
        p_arr[3].x = -14.4 * k, p_arr[3].y = 3.5 * k; 
        l = c2d_add_polyline(cad, p_arr, 4);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        /* Initialize p1 and p2 at center then add the trigonometric values */
        c2d_set_point(&p1, -15.5 * k, 4.5 * k);
        c2d_set_point(&p2, -15.5 * k, 4.5 * k);
        p1.x += 2.5 * k * cos(PI / 4);
        p1.y += 2.5 * k * sin(PI / 4);
        p2.x += 1.5 * k * cos(PI / 4);
        p2.y += 1.5 * k * sin(PI / 4);
        l = c2d_add_line(cad, p1, p2);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        /* Draw letter T with polyline or polygone */
        p_arr[0].x = -12 * k, p_arr[0].y = 6 * k; 
        p_arr[1].x = -12 * k, p_arr[1].y = 7 * k; 
        p_arr[2].x =  -7 * k, p_arr[2].y = 7 * k; 
        p_arr[3].x =  -7 * k, p_arr[3].y = 6 * k; 
        p_arr[4].x =  -9 * k, p_arr[4].y = 6 * k; 
        p_arr[5].x =  -9 * k, p_arr[5].y = 2 * k; 
        p_arr[6].x = -10 * k, p_arr[6].y = 2 * k; 
        p_arr[7].x = -10 * k, p_arr[7].y = 6 * k; 
        p_arr[8].x = -12 * k, p_arr[8].y = 6 * k; 
        l = c2d_add_polyline(cad, p_arr, 9);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        /* Draw letter U with arc and polyline */
        c2d_set_point(&p1, -4 * k, 4 * k);

        l = c2d_add_arc(cad, p1, 2 * k, 180.0, 360.0);    
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);
        l = c2d_add_arc(cad, p1,     k, 180.0, 360.0);    
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);
        
        p_arr[0].x = -6 * k, p_arr[0].y = 4 * k; 
        p_arr[1].x = -6 * k, p_arr[1].y = 7 * k; 
        p_arr[2].x = -5 * k, p_arr[2].y = 7 * k; 
        p_arr[3].x = -5 * k, p_arr[3].y = 4 * k; 
        l = c2d_add_polyline(cad, p_arr, 4);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        p_arr[0].x = -3 * k, p_arr[0].y = 4 * k; 
        p_arr[1].x = -3 * k, p_arr[1].y = 7 * k; 
        p_arr[2].x = -2 * k, p_arr[2].y = 7 * k; 
        p_arr[3].x = -2 * k, p_arr[3].y = 4 * k; 
        l = c2d_add_polyline(cad, p_arr, 4);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        /* Draw letter C with arc and polyline */
        c2d_set_point(&p1, 1.5 * k, 4.5 * k);
        l = c2d_add_arc(cad, p1, 2.5 * k, 30.0, 330.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        l = c2d_add_arc(cad, p1, 1.5 * k, 30.0, 330.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        /* Initialize p1 and p2 at center then add the trigonometric values */
        p2 = p1;
        p1.x += 2.5 * k * cos(PI / 6);
        p1.y += 2.5 * k * sin(PI / 6);
        p2.x += 1.5 * k * cos(PI / 6);
        p2.y += 1.5 * k * sin(PI / 6);
        l = c2d_add_line(cad, p1, p2);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        c2d_set_point(&p1, 1.5 * k, 4.5 * k);
        c2d_set_point(&p2, 1.5 * k, 4.5 * k);
        p1.x += 2.5 * k * cos(PI / 6);
        p1.y -= 2.5 * k * sin(PI / 6);
        p2.x += 1.5 * k * cos(PI / 6);
        p2.y -= 1.5 * k * sin(PI / 6);
        l = c2d_add_line(cad, p1, p2);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        /* Draw letter A with triangle and polyline */
        p_arr[0].x =  5   * k, p_arr[0].y = 2   * k; 
        p_arr[1].x =  7   * k, p_arr[1].y = 7   * k; 
        p_arr[2].x =  8   * k, p_arr[2].y = 7   * k; 
        p_arr[3].x = 10   * k, p_arr[3].y = 2   * k; 
        p_arr[4].x =  9   * k, p_arr[4].y = 2   * k; 
        p_arr[5].x =  9   * k, p_arr[5].y = 2   * k; 
        p_arr[6].x =  8.4 * k, p_arr[6].y = 3.5 * k; 
        p_arr[7].x =  6.6 * k, p_arr[7].y = 3.5 * k;
        p_arr[8].x =  6   * k, p_arr[8].y = 2   * k; 
        p_arr[9].x =  5   * k, p_arr[9].y = 2   * k; 
        l = c2d_add_polyline(cad, p_arr, 10);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        c2d_set_point(&p1, 7   * k, 4.5 * k);
        c2d_set_point(&p2, 7.5 * k, 6 * k);
        c2d_set_point(&p3, 8   * k, 4.5 * k);
        l = c2d_add_triangle(cad, p1, p2, p3);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        /* Draw letter D with triangle and polyline */
        c2d_set_point(&p1, 12 * k, 4.5 * k);
        l = c2d_add_arc(cad, p1, 2.5 * k, 270.0, 90.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        l = c2d_add_arc(cad, p1, 1.5 * k, 270.0, 90.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        p_arr[0].x = 12 * k, p_arr[0].y = 2 * k; 
        p_arr[1].x = 11 * k, p_arr[1].y = 2 * k; 
        p_arr[2].x = 11 * k, p_arr[2].y = 7 * k; 
        p_arr[3].x = 12 * k, p_arr[3].y = 7 * k; 
        l = c2d_add_polyline(cad, p_arr, 4);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        c2d_set_point(&p1, 12 * k, 6 * k);
        c2d_set_point(&p2, 12 * k, 3 * k);
        l = c2d_add_line(cad, p1, p2);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        /* Add brand message */
        c2d_set_point(&p1, 12 * k,  0.0);
        c2d_set_color_rgb(&c, 0.0, 0.5, 1.0);
        c2d_add_text(cad, p1, "The World Is Yours...", Times, c, fs_medium);

        c2d_export(cad, "test8.gtucad", gtucad);
        c2d_delete(cad);
        printf("<<< Test8 DONE >>>\n\n");
    }
}

void test9 () {
    CAD2D * cad = c2d_import("test8.gtucad", gtucad);
    c2d_export(cad, "test9.eps", eps);
    c2d_delete(cad);
    printf("<<< Test9 DONE >>>\n\n");
}

void test10 () {
    CAD2D * cad;
    Point2D p1;
    Label * l;
    RGBColor c;

    cad = c2d_start_wh(500, 500);
    if (NULL != cad) {
        c2d_set_point(&p1, 0.0, 0.0);
        c2d_set_color_pallette(&c, blue_light);

        l = c2d_add_arc_l(cad, "center-arc", p1,  30.0, 0.0, 360.0);
        c2d_set_entity_style(cad, l, DEFAULT, c, DEFAULT, lw_large);

        c2d_set_point(&p1, 150.0, 0.0);
        c2d_add_text(cad, p1, "The World Is Yours AB", Coronet, c, fs_xsmall);

        c2d_add_xy_plane(cad);
        c2d_export(cad, "test10.eps", eps);
        c2d_delete(cad);
        printf("<<< Test10 DONE >>>\n\n");
    }
}

/*
    https://web.stanford.edu/class/archive/cs/cs107/cs107.1218/resources/valgrind.html
*/
int main (void) {
    /**
    test3();
    test4();
     **/
    test10();
    
    /*
    test0();
    test00();
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test55();
    */
}