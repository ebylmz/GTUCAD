#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cad2d.h"

/*********************************************************************************
 * CAD Start
*********************************************************************************/
CAD2D * c2d_start () {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));
    
    if (cad != NULL) {
        cad->list = (Entity **) calloc(INIT_SIZE_HASH, sizeof(Entity *));

        if (cad->list != NULL) {
            cad->canvas = NULL;
            cad->list_size.cur = 0;
            cad->list_size.max = INIT_SIZE_HASH;  /* initially 10 entity can created in this cad object */
            cad->hierarchy = c2d_create_hierarchy(cad);
        }
        else {
            free(cad);
            cad = NULL;
        }
    }
    return cad;
}

CAD2D * c2d_start_wh (double width, double height) {
    CAD2D * cad = c2d_start();
    
    if (cad != NULL) {
        cad->canvas = (Canvas *) malloc(sizeof(Canvas));
        
        if (cad->canvas != NULL) {
            cad->canvas->start.x = -width / 2;
            cad->canvas->start.y = -height / 2;
            cad->canvas->end.x = width / 2; 
            cad->canvas->end.y = height / 2; 
        }
        else {
            free(cad);
            cad = NULL;
        }
    }
    
    return cad;
}
/* Initialize the new CAD at children of given hierarchy */
CAD2D * c2d_start_wh_hier (double width, double height, Hierarchy * h) {
    CAD2D * cad = c2d_start_wh(width, height);

    if (cad != NULL)
        h->parent = h;

    return cad;
}

/*********************************************************************************
 * Label
*********************************************************************************/

/*********************************************************************************
 * Hierarchy
*********************************************************************************/
Hierarchy * c2d_create_hierarchy (CAD2D * cad) {
    Hierarchy * h = (Hierarchy *) malloc(sizeof(Hierarchy));

    if (h != NULL) {
        h->children = (Hierarchy **) calloc(INIT_SIZE_HIER, sizeof(Hierarchy *));
        if (h->children != NULL) {
            h->cad = cad;
            h->parent = NULL;
            h->size.max = INIT_SIZE_HIER;
            h->size.cur = 0;

            cad->hierarchy = h;
        }
        else {
            free(h);
            h = NULL;
        }
    }

    return h;
}

//* Hierarchy * c2d_create_hierarchy?(CAD2D * cad, …) {}

Hierarchy * c2d_create_hierarchy_parent (CAD2D * cad, Hierarchy * parent) {
    Hierarchy * h = c2d_create_hierarchy(cad);

    if (h != NULL) {
        h->parent = parent;

        /* add new hierarchy as child of given parent */
        if (parent->size.cur < parent->size.max) 
            parent->children[parent->size.cur] = h;
        else {
            //! NOT IMPLEMENTED YET: Reallocating memory for hierarchy array (newsize by doubling current max size)
        }
    }

    return h;
}

/*********************************************************************************
 * Adding Basic CAD Entities
*********************************************************************************/

// ! any basic 2D shape can have color, thickness and line style 

/* point, line, spline, polyline, polygon, rectangle, circle, arc, ellipse, text, image */
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

Label * c2d_add_line (CAD2D * cad, Point2D start, Point2D end) {
    Line * data = (Line *) malloc(sizeof(Line));
    Entity * e;

    if (data != NULL) {
        data->start = start;
        data->end = end;
        // ! NOT IMPLEMENTED YET: data->style maybe a function required for this
        // c2d_add_entity_list(&cad->entity_list, e);
    }

    return e != NULL ? e->label : NULL;
}

Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle) {
    Entity * e = (Entity *) malloc(sizeof(Entity));
    Label * e_label = NULL;
    Arc * e_data;

    if (e != NULL) {
        e_data = (Arc *) malloc(sizeof(Arc));

        if (e_data != NULL) {
            e_data->center = center;
            e_data->radius = radius;
            e_data->start_angle = start_angle;
            e_data->end_angle = end_angle;

            e_label = e->label = c2d_create_label(cad, "", arc);
            e->entity_data = e_data;
            c2d_add_entity_list(&cad->entity_list, e);
        }
    }
    return e_label;
}

Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius) {
    Entity * e = (Entity *) malloc(sizeof(Entity));
    Label * e_label = NULL;
    Circle * e_data;

    if (e != NULL) {
        e_data = (Circle *) malloc(sizeof(Circle));

        if (e_data != NULL) {
            e_data->center = center;
            e_data->radius = radius;
            e_data->start_angle = 0;
            e_data->end_angle = 360;

            e->entity_data = e_data;
            e_label = e->label = c2d_create_label(cad, "", circle);
            c2d_add_entity_list(&cad->entity_list, e);
        }
    }
    return e_label;
}

Label * c2d_add_rectangle (CAD2D * cad, Point2D corne) {
    Entity * e = malloc(sizeof(entity));

}

/* 
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
 * Import & Export & Memory Deletion
*********************************************************************************/
void draw_line (FILE * fid, Line * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->start.x , e->start.y);
    fprintf(fid, "%.2f %.2f lineto\n", e->end.x, e->end.y);
    // fprintf(fid, "stroke\n");
}

void draw_arc (FILE * fid, Arc * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n", e->center.x, e->center.y, e->radius, e->start_angle, e->end_angle);
    // fprintf(fid, "stroke\n");
}

void draw_rectangle (FILE * fid, Rectangle * e) {
    double  height = (e->corner_top_right.y - e->corner_down_left.y),
            width = (e->corner_down_left.x - e->corner_top_right.x);

    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto", e->corner_down_left.x, e->corner_down_left.y);
    fprintf(fid, "%.2f %.2f rlineto", 0, height);
    fprintf(fid, "%.2f %.2f rlineto", width, 0);
    fprintf(fid, "%.2f %.2f rlineto", 0, -height);
    fprintf(fid, "closepath");
} 

// in eps mode we do not deal with labels but in gtu mode we do

void c2d_export (CAD2D * cad, char * file_name, char * options) {}

void c2d_export_eps (CAD2D * cad, char * file_name) {
    FILE * fid = fopen(file_name, "wt");
    EntityList * l = cad->entity_list;

    if (fid != NULL) {
        fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
        
        /* Check if canvas is decleared or not */
        if (cad->canvas != NULL)
            fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);

        while (l != NULL) {
            switch (l->entity->label->type) {
                case point:
                    // ! NOT IMPLEMENTED YET
                    break;
                case line:
                    printf("Draw line: ");
                    draw_line(fid, (Line *) l->entity->entity_data);
                    fprintf(fid, "stroke\n");
                    printf("Done\n");
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
                    printf("Draw rectangle: ");
                    draw_rectangle(fid, (Rectangle *) l->entity->entity_data);
                    printf("Done\n");
                    break;
                case circle:
                case arc:
                    printf("Draw Arc: ");
                    draw_arc(fid, (Arc *) l->entity->entity_data);
                    fprintf(fid, "stroke\n");
                    printf("Done\n");
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
