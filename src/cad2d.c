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
        cad->list = NULL;
        cad->list_size.max = cad->list_size.cur = 0;
        cad->canvas = NULL;
        cad->hierarchy = c2d_create_hierarchy(cad);
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
    int i;
    
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
void insert_entity_list (CAD2D * cad, Entity * e) {
    Entity ** list = cad->list;
    Entity ** tmp;
    int i;

    if (cad->list_size.cur == cad->list_size.max) {
        cad->list_size.max = cad->list_size.max == 0 ? INIT_SIZE_HASH : cad->list_size.max * 2;        
        
        tmp = (Entity **) calloc(cad->list_size.max, sizeof(Entity *));

        if (tmp != NULL) {
            for (i = 0; i < cad->list_size.cur; ++i)
                tmp[i] = list[i];
            
            free(list);
            cad->list = tmp;
        }
    }

    cad->list[(cad->list_size.cur)++] = e;
}

Label * c2d_create_label (EntityType type, char * name) {
    Label * l = (Label *) malloc(sizeof(Label));

    if (l != NULL) {
        l->name = name;
        l->type = type;
        l->hash_code = -1;  //! !!!!!!!!!!!!!
    }

    return l;
}

Entity * c2d_create_entity_filled (Label * l, void * data) {
    Entity * e = (Entity *) malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = data;
        e->label = l;
    }

    return e;
}

Entity * c2d_create_entity_empty (EntityType type) {
    Entity * e = (Entity *) malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = NULL;

        switch (type) {
            case point:
                e->data = (Point2D *) malloc (sizeof(Point2D));
                break;
            case line:
                e->data = (Line *) malloc (sizeof(Line));
                break;
            case spline:
                e->data = (Spline *) malloc (sizeof(Spline));
                break;
            case polyline:
                e->data = (Polyline *) malloc (sizeof(Polyline));
                break;
            case polygon:
                e->data = (Polygon *) malloc (sizeof(Polygon));
                break;
            case rectangle:
                e->data = (Rectangle *) malloc (sizeof(Rectangle));
                break;
            case circle:
                e->data = (Circle *) malloc (sizeof(Circle));
                break;
            case arc:
                e->data = (Arc *) malloc (sizeof(Arc));
                break;
            case ellipse:
                e->data = (Ellipse *) malloc (sizeof(Ellipse));
                break;
            case text:
                e->data = (Text *) malloc (sizeof(Text));
                break;
            case image:
                e->data = (Image *) malloc (sizeof(Image));
                break;
        }
           
        if (e->data == NULL) {
            free(e);
            e = NULL;
        }
        else {
            e->label = (Label *) malloc(sizeof(Label));
            if (e->label == NULL) {
                free(e->data);
                free(e);
                e = NULL;
            }
        }
    }
    return e;
}

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
    Entity * e = c2d_create_entity_empty(line);
    Line * data;

    if (e != NULL) {
        data = (Line *) e->data;
        data->start = start;
        data->end = end;
        //! NOT IMPLEMENTED YET: data->style
        //! NOT IMPLEMENTED YET: Create a unique label
        
        // insert list function requried 
        /*
        if (cad->list_size.cur < cad->list_size.max) {
            cad->list[(cad->list_size.cur)++] = e;
        }
        else {
            //! NOT IMPLEMENTED YET: Reallocate new memory for entity list
        }
        */
    }

    return e != NULL ? e->label : NULL;
}

Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle) {
    Entity * e = c2d_create_entity_empty(arc);
    Arc * data;

    if (e != NULL) {
        data = (Arc *) e->data;

        data->center = center;
        data->radius = radius;
        data->start_angle = start_angle;
        data->end_angle = end_angle;

        //! NOT IMPLEMENTED YET: Create a unique label
        insert_entity_list(cad, e);
    }

    return e != NULL ? e->label : NULL;
}

Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius) {
    Entity * e = c2d_create_entity_empty(circle);
    Circle * data;

    if (e != NULL) {
        data = (Circle *) e->data;
        data->center = center;
        data->radius = radius;
        data->start_angle = 0;
        data->end_angle = 360;

        //! NOT IMPLEMENTED YET: Create a unique label
        insert_entity_list(cad, e);
    }
    return e != NULL ? e->label : NULL;
}

Label * c2d_add_rectangle (CAD2D * cad, Point2D corne) {
    //! NOT IMPLEMENTED YET
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

double c2d_measure (CAD2D * cad, const Label * ls, const Label * lt) {
    //! NOT IMPLEMENTED YET
}

/*********************************************************************************
 * Snapping Labels
*********************************************************************************/

void c2d_snap (CAD2D * cad, const Label * ls, const Label * lt) {
    //! NOT IMPLEMENTED YET
}

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

void c2d_export (CAD2D * cad, char * file_name, char * options) {
    FILE * fid = fopen(file_name, "wt");

    if (fid != NULL) {
        if (strcmp(options, "eps") == 0)
            export_eps(cad, fid);
        else if (strcmp(options, "gtucad") == 0)
            export_gtucad(cad, fid);
        //! NOT IMPLEMENTED YET: else 

        fclose(fid);
    }
}

void export_eps (CAD2D * cad, FILE * fid) {
    Entity ** e = cad->list;
    int i;

    fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
    
    /* Check if canvas is decleared or not */
    if (cad->canvas != NULL)
        fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);

    for (i = 0; i < cad->list_size.cur; ++i) {
        switch (e[i]->label->type) {
            case point:
                //! NOT IMPLEMENTED YET
                break;
            case line:
                printf("Draw line: ");
                draw_line(fid, (Line *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case spline:
                //! NOT IMPLEMENTED YET
                break;
            case polyline:
                //! NOT IMPLEMENTED YET
                break;
            case polygon:
                //! NOT IMPLEMENTED YET
                break;
            case rectangle:
                printf("Draw rectangle: ");
                draw_rectangle(fid, (Rectangle *) e[i]->data);
                printf("Done\n");
                break;
            case circle:
            case arc:
                printf("Draw Arc: ");
                draw_arc(fid, (Arc *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
            case ellipse:
                //! NOT IMPLEMENTED YET
                break;
            case text:
                //! NOT IMPLEMENTED YET
                break;
            case image:
                //! NOT IMPLEMENTED YET
                break;
            default:
                //!  NOT IMPLEMENTED YET
        }
    }
    fprintf(fid, "\nshowpage\n");
}

void export_gtucad(CAD2D * cad, FILE * fid) {
    //! NOT IMPLEMENTED YET
}

/* import option for now consist of only "GTUCAD", in the future it can be improved */
CAD2D * c2d_import (char * file_name, char * options) {
    //! NOT IMPLEMENTED YET
}

/*********************************************************************************
 * Memory Deletion
*********************************************************************************/

void c2d_delete (CAD2D * cad) {
    //! NOT IMPLEMENTED YET
} 
