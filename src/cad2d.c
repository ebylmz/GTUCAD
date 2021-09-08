#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cad2d.h"

/*********************************************************************************
 * CAD Start
*********************************************************************************/

/* creates an root cad with undefined canvas */
CAD2D * c2d_start() {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));

    if (cad != NULL) {
        cad->h.child = cad->h.parent = NULL;   
        cad->entity = NULL;
        cad->canvas = NULL;
    }

    return cad;
}

CAD2D * c2d_start_wh (double width, double height) {
    CAD2D * cad = c2d_start();

    if (cad != NULL) {
        cad->canvas = (Canvas *) malloc(sizeof(Canvas));
        
        /* define the canvas */
        if (cad->canvas != NULL) {
            cad->canvas->start.x = -width /2;
            cad->canvas->start.y = -height /2;
            cad->canvas->end.x = width / 2;
            cad->canvas->end.y = height / 2;
        }
        else {
            return 0;
            /* return ERROR VALUE */
        }
    }

    return cad;
}

CAD2D * c2d_start_wh_hier (double width, double height, Hierarchy * h) {}

/*********************************************************************************
 * Adding Basic CAD Entities
*********************************************************************************/

/* CAD <BASIC>
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h, const Label * l) {}
*/
//! any basic 2D shape can have color, thickness and line stle 

Label * c2d_add_point_xy (CAD2D * cad, double x, double y) {}
Label * c2d_add_point_p (CAD2D * cad, Point2D p) {}
Label * c2d_add_point_ph (CAD2D * cad, Point2D p, const Hierarchy * h) {}
Label * c2d_add_point_phl (CAD2D * cad, Point2D p, const Hierarchy * h, const Label * l) {}

Label * c2d_add_line(CAD2D * cad, Point2D * start, Point2D * end) {
    Label * r;
    CadEntity ** new = &cad->entity;

    while (*new != NULL) new = &(*new)->next;


    *new = (CadEntity *) malloc(sizeof(CadEntity));

    if (*new != NULL) {
        (*new)->current.point.x = start->x; 
        (*new)->current.point.y = start->y;

        new = &(*new)->next;
        *new = (CadEntity *) malloc(sizeof(CadEntity));

        if (*new != NULL) {
            (*new)->current.point.x = end->x;            
            (*new)->current.point.y = end->y;            
        } 
    }
}

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

void c2d_export (CAD2D * cad, char * file_name, char * options) {}

void c2d_export_eps (CAD2D * cad, char * file_name) {
    FILE * fid = fopen(file_name, "wt");

    if (fid != NULL) {
        fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
        /* Check if canvas is decleared or not */
        if (cad->canvas != NULL)
            fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);
    
        fprintf(fid, "\n%.2f %.2f moveto\n", cad->entity->current.point.x, cad->entity->current.point.y);
        fprintf(fid, "%.2f %.2f lineto\n", cad->entity->next->current.point.x, cad->entity->next->current.point.y);
        fprintf(fid, "stroke\n");
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
