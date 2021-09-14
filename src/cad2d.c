#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cad2d.h"

/*********************************************************************************
 * CAD Start
*********************************************************************************/

/* creates an root cad with unlimited canvas */
CAD2D * c2d_start () {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));

    if (cad != NULL)
        cad->canvas = NULL;
        cad->label_list = NULL; 
        cad->h = NULL;
    
    return cad;
}

CAD2D * c2d_start_wh (double width, double height) {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));

    if (cad != NULL) {
        cad->canvas = (Canvas *) malloc(sizeof(Canvas));

        if (cad->canvas != NULL) {
            cad->canvas->start.x = -width / 2;
            cad->canvas->start.y = -height / 2;
            cad->canvas->end.x = width / 2;
            cad->canvas->end.y =  height / 2;

            cad->label_list = NULL;
            cad->h = NULL;
        } else {
            free(cad);
            cad = NULL;
        }
    }
    
    return cad;
}

CAD2D * c2d_start_wh_hier (double width, double height, Hierarchy * h) {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));

    if (cad != NULL) {
        cad->canvas = (Canvas *) malloc(sizeof(Canvas));

        if (cad->canvas != NULL) {
            cad->canvas->start.x = -width / 2;
            cad->canvas->start.y = -height / 2;
            cad->canvas->end.x = width / 2;
            cad->canvas->end.y =  height / 2;

            cad->label_list = NULL;
            cad->h = NULL;

            // ! NOT IMPLEMENTED YET: defining Hierarcy 
        } else {
            free(cad);
            cad = NULL;
        }
    }
    
    return cad;
}

/*********************************************************************************
 * Adding Basic CAD Entities
*********************************************************************************/
void c2d_add_label_list (ListLabel ** list, Label * label) {
    while (*list != NULL) list = &(*list)->next;

    *list = (ListLabel *) malloc(sizeof(ListLabel));

    if (*list != NULL) {
        (*list)->label = label;    
        (*list)->next = NULL;         
    }
}

// ! any basic 2D shape can have color, thickness and line style 

/*
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h, const Label * l) {}
*/

Label * c2d_add_point_xy (CAD2D * cad, double x, double y) {}
Label * c2d_add_point_p (CAD2D * cad, Point2D p) {}
Label * c2d_add_point_ph (CAD2D * cad, Point2D p, const Hierarchy * h) {}
Label * c2d_add_point_phl (CAD2D * cad, Point2D p, const Hierarchy * h, const Label * l) {}

Label * c2d_add_line(CAD2D * cad, Point2D * start, Point2D * end) {
    Label * label = (Label *) malloc(sizeof(Label));    
    Line * e;

    if (label != NULL) {
        e = (Line *) malloc(sizeof(Line));
        if (e != NULL) {
            e->start = *start;
            e->end = *end;

            label->entity = (void *) e;
            label->type = line;
            
            /* insert this new cad entity to cad entity list */
            c2d_add_label_list(&cad->label_list, label);
        }
    }

    return label;
}

/* point, line, spline, polyline, polygon, rectangle, circle, arc, ellipse, text, image */

/* 
Label * c2d_add_arc(CAD2D * cad, ...) {}
Label * c2d_add_circle(CAD2D * cad, ...) {}
Label * c2d_add_ellipse(CAD2D * cad, ...) {}
Label * c2d_add_splines(CAD2D * cad, ...) {}
Label * c2d_add_polyline(CAD2D * cad, ...) {}
Label * c2d_add_text(CAD2D * cad, char * text, Color, Font, Thickness ...) {}
Label * c2d_add_polygons(CAD2D * cad, ...) {}
Label * c2d_add_image(CAD2D * cad, ...) {}
*/

/*********************************************************************************
 * Measurement between two Labels
*********************************************************************************/

double c2d_measure (CAD2D * cad, const Label * ls, const Label * lt) {}


/*********************************************************************************
 * Snapping Labels
*********************************************************************************/

void c2d_snap (CAD2D * cad, const Label * ls, const Label * lt) {}


/*********************************************************************************
 * Hierarchy
*********************************************************************************/

/*
Hierarchy * c2d_create_hierarchy?(CAD2D * cad) {}
Hierarchy * c2d_create_hierarchy?(CAD2D * cad, …) {}
Hierarchy * c2d_create_hierarchy?(CAD2D * cad, Hierarchy * parent) {}
*/

/*********************************************************************************
 * Import & Export & Memory Deletion
*********************************************************************************/
void draw_line (FILE * fid, Line * l) {
    fprintf(fid, "\n%.2f %.2f moveto\n", l->start.x , l->start.y);
    fprintf(fid, "%.2f %.2f lineto\n", l->end.x, l->end.y);
    fprintf(fid, "stroke\n");
}


void c2d_export (CAD2D * cad, char * file_name, char * options) {}

void c2d_export_eps (CAD2D * cad, char * file_name) {
    FILE * fid = fopen(file_name, "wt");
    ListLabel * l = cad->label_list;


    if (fid != NULL) {
        fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
        
        /* Check if canvas is decleared or not */
        if (cad->canvas != NULL)
            fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);

        while (l != NULL) {
            switch (l->label->type) {
                case point:
                    // ! NOT IMPLEMENTED YET
                    break;
                case line:
                    draw_line(fid, (Line *) l->label->entity);
                    break;
                case spline:
                    // ! NOT IMPLEMENTED YET
                    break;
                case polyline:
                    // ! NOT IMPLEMENTED YET
                    break;
                case polygon:
                    // ! NOT IMPLEMENTED YET
                    break;
                case rectangle:
                    // ! NOT IMPLEMENTED YET
                    break;
                case circle:
                    // ! NOT IMPLEMENTED YET
                    break;
                case arc:
                    // ! NOT IMPLEMENTED YET
                    break;
                case ellipse:
                    // ! NOT IMPLEMENTED YET
                    break;
                case text:
                    // ! NOT IMPLEMENTED YET
                    break;
                case image:
                    // ! NOT IMPLEMENTED YET
                    break;
                default:
                    // ! NOT IMPLEMENTED YET
                    printf("ERROR\n");
            }

            l = l->next;
        }
        fprintf(fid, "\nshowpage\n");

        fclose(fid);
    } 
    else 
        printf("export file cannot open");
}

/* import option for now consist of only "GTUCAD", in the future it can be improved */
CAD2D * c2d_import (char * file_name, char * options) {}

/*********************************************************************************
 * Memory Deletion
*********************************************************************************/

void c2d_delete (CAD2D * cad) {} 
