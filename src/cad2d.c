#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cad2d.h"

/*********************************************************************************
 * CAD Start
*********************************************************************************/
/* Initialize a new cad without any constrain */
CAD2D * c2d_start () {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));
    
    if (cad != NULL) {
        cad->list = NULL;
        cad->list_size.max = 0;
        cad->list_size.cur = 0;
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

/* Initialize a new CAD at children of given hierarchy */
CAD2D * c2d_start_wh_hier (double width, double height, Hierarchy * h) {
    CAD2D * cad = c2d_start_wh(width, height);

    if (cad != NULL) {
        u_insert_hierarchy(cad->hierarchy, h);
        h->parent = h;
    }

    return cad;
}

/*********************************************************************************
 * Hierarchy
*********************************************************************************/
/* links given two hiearchy as child-parent */
void u_insert_hierarchy (Hierarchy * child, Hierarchy * parent) {
    Hierarchy ** tmp;
    int i;

    if (parent->size.cur == parent->size.max) {
        parent->size.max = parent->size.max == 0 ? INIT_HIER : parent->size.cur * 2; 

        tmp = (Hierarchy **) calloc(parent->size.max, sizeof(Hierarchy *));

        if (tmp != NULL) {
            for (i = 0; i < parent->size.cur; ++i)
                tmp[i] = parent->children[i];

            free(parent->children);
            parent->children = tmp;
        }
    }

    parent->children[(parent->size.cur)++] = child;
}

/* creates a hierarchy from given cad */
Hierarchy * c2d_create_hierarchy (CAD2D * cad) {
    Hierarchy * h = (Hierarchy *) malloc(sizeof(Hierarchy));

    if (h != NULL) {
        h->deep = 0;    /* Root hierarchy */
        h->parent = NULL;
        h->cad = cad;
        h->children = NULL;
        h->size.max = 0;
        h->size.cur = 0;

        cad->hierarchy = h;
    }

    return h;
}

//* Hierarchy * c2d_create_hierarchy?(CAD2D * cad, …) {}

/* creates a hierarchy from given cad and add it as child of given parent hierarchy */
Hierarchy * c2d_create_hierarchy_parent (CAD2D * cad, Hierarchy * parent) {
    Hierarchy * h = c2d_create_hierarchy(cad);
    int i;
    
    if (h != NULL) {
        h->deep = 1 + parent->deep;
        h->parent = parent;

        /* add new hierarchy as child of given parent */
        u_insert_hierarchy(h, parent);
    }

    return h;
}

/*********************************************************************************
 * Label
*********************************************************************************/
char * u_produce_label_name (CAD2D * cad, EntityType type) {
    char name[10];
    char * r;
    int n, i = 0;

    /* since every hierarchy name's unique thhis process produce unique label */

    /* 
        Type + Hierarchy + order
        First rectangle in hiearchy named h0 will produce 
            R00
        Second
            R01
        Third polyline in hierarchy named h2 will produce
            Pl23
    */

    /* add type factor */
    switch (type) {
        case point:
            name[i++] = 'P';
            break;
        case line:
            name[i++] = 'L';
            break;
        case spline:
            name[i++] = 'S';
            name[i++] = 'l';
            break;
        case polyline:
            name[i++] = 'P';
            name[i++] = 'l';
            break;
        case polygon:
            name[i++] = 'P';
            name[i++] = 'g';
            break;
        case rectangle:
            name[i++] = 'R';
            break;
        case circle:
            name[i++] = 'C';
            break;
        case arc:
            name[i++] = 'A';
            break;
        case ellipse:
            name[i++] = 'E';
            break;
        case text:
            name[i++] = 'T';
            break;
        case image:
            name[i++] = 'I';
            break;
    }

    //! NOT IMPLEMENTED YET: Better solution needed
    /* add deep factor */
    /* add order */
    name[i++] = cad->hierarchy->deep + '0';
    name[i++] = '\0';

    if (r != NULL) 
        strcpy(r, name);
    //! NOT IMPLEMENTED YET: Else

    return r;
}

int u_get_hash (Label * l, int q, int p) {
    int i;
    int code;

    //! is mod(p) suitable
    for (i = 0; l->name[i] != '\0'; ++i)
        code = (code * q + l->name[i]) % p;

    return code;
}

/*  Creates an unique label inside the given CAD.
    For automic naming use "" (empty string) as name    */
Label * c2d_create_label (CAD2D * cad, EntityType type, char * name) {
    Label * l = (Label *) malloc(sizeof(Label));

    if (l != NULL) {
        l->type = type;

        /* If user not provided a label name, produce a unique one */
        //l->name = name[0] == '\0' ? u_produce_label_name(cad, type) : name;
        //! NOT IMPLEMENTED YET: Hash code calling parameters
        // l->hash_code = u_get_hash(l, 10 , PRIME);

        //! NOT IMPLEMETED YET: Be sure produced or given name is unique by hashing
    }

    return l;
}

/*********************************************************************************
 * Adding Basic CAD Entities
*********************************************************************************/

void u_insert_entity_list (CAD2D * cad, Entity * e) {
    Entity ** tmp;
    int i;

    if (cad->list_size.cur == cad->list_size.max) {
        cad->list_size.max = cad->list_size.max == 0 ? INIT_HASH : cad->list_size.max * 2;        
        
        tmp = (Entity **) calloc(cad->list_size.max, sizeof(Entity *));

        if (tmp != NULL) {
            for (i = 0; i < cad->list_size.cur; ++i)
                tmp[i] = cad->list[i];
            
            free(cad->list);
            cad->list = tmp;
        }
    }

    cad->list[(cad->list_size.cur)++] = e;
}

//* /////////////////////////////////////////////////////////
//! /////////////////////////////////////////////////////////
//? /////////////////////////////////////////////////////////

Entity * u_create_entity_filled (Label * l, void * data) {
    Entity * e = (Entity *) malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = data;
        e->label = l;
    }

    return e;
}

Entity * u_create_entity_empty (EntityType type) {
    Entity * e = (Entity *) malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = NULL;

        switch (type) {
            case point:
            case line:
            case polyline:
            case polygon:
                e->data = (Point2D *) malloc (sizeof(Point2D));
                break;
            case spline:
                e->data = (Spline *) malloc (sizeof(Spline));
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

Label * c2d_add_line (CAD2D * cad, Point2D * start, Point2D * end) {
    Entity * e = u_create_entity_empty(line);
    Point2D * data;

    if (e != NULL) {
        data = (Point2D *) e->data;
        data->x = start->x;
        data->y = start->y;
    
        data->next = c2d_create_point(end->x, end->y);
        //! NOT IMPLEMENTED YET: if (data->next == NULL)
        //! NOT IMPLEMENTED YET: data->style

        e->label = c2d_create_label(cad, line, "");
        u_insert_entity_list(cad, e); 
    }

    return e != NULL ? e->label : NULL;
}

Point2D * c2d_create_point (double x, double y) {
    Point2D * p = (Point2D *) malloc(sizeof(Point2D));

    if (p != NULL) {
        p->x = x;
        p->y = y;
        p->next = NULL;
    }

    return p;
}

Label * c2d_add_polyline (CAD2D * cad, Point2D * p) {
    Entity * e = u_create_entity_empty(polyline);
    Point2D * data;
    Point2D ** trav;

    if (e != NULL) {
        data = (Point2D *) e->data;
        trav = &data;

        while (p != NULL) {
            *trav = c2d_create_point(p->x, p->y);
            
            if (*trav != NULL) {
                trav = &(*trav)->next;
                p = p->next;
            }
            else    
                break;
            //! NOT IMPLEMENTED YET: else
        }
        /* lastly trav will point the last Point2D pointer */
        *trav = NULL;

        e->label = c2d_create_label(cad, polyline, "");
        u_insert_entity_list(cad, e);
    }
    
    return e != NULL ? e->label : NULL;
}


Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle) {
    Entity * e = u_create_entity_empty(arc);
    Arc * data;

    if (e != NULL) {
        data = (Arc *) e->data;

        data->center = center;
        data->radius = radius;
        data->start_angle = start_angle;
        data->end_angle = end_angle;

        //! NOT IMPLEMENTED YET: Create a unique label
        e->label = c2d_create_label(cad, arc, "");
        u_insert_entity_list(cad, e);
    }

    return e != NULL ? e->label : NULL;
}

Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius) {
    Entity * e = u_create_entity_empty(circle);
    Circle * data;

    if (e != NULL) {
        data = (Circle *) e->data;
        data->center = center;
        data->radius = radius;
        data->start_angle = 0.0;
        data->end_angle = 360.0;

        e->label = c2d_create_label(cad, circle, "");
        u_insert_entity_list(cad, e);
    }
    return e != NULL ? e->label : NULL;
}
//* Label * c2d_add_rectangle (CAD2D * cad, Point2D center , double width, double hight) {

Label * c2d_add_rectangle (CAD2D * cad, Point2D cornerA , Point2D cornerC) {
    Entity * e = u_create_entity_empty(rectangle);
    Rectangle * data;
    
    if (e != NULL) {
        data = (Rectangle *) e->data;
        data->cornerA = cornerA;
        data->cornerC = cornerC;
        //! NOT IMPLEMENTED YET: data->style

        e->label = c2d_create_label(cad, rectangle, "");
        u_insert_entity_list(cad, e);
    }
    return e != NULL ? e->label : NULL;
}

/* 
Label * c2d_add_ellipse(CAD2D * cad, ...) {}
Label * c2d_add_splines(CAD2D * cad, ...) {}
Label * c2d_add_polygons(CAD2D * cad, ...) {}
Label * c2d_add_text(CAD2D * cad, char * text, Color, Font, Thickness ...) {}
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
void u_draw_line (FILE * fid, Point2D * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->x , e->y);
    e = e->next;
    fprintf(fid, "%.2f %.2f lineto\n", e->x, e->y);
}

void u_draw_arc (FILE * fid, Arc * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n", e->center.x, e->center.y, e->radius, e->start_angle, e->end_angle);
}

void u_draw_rectangle (FILE * fid, Rectangle * e) {
    double  height = (e->cornerC.y - e->cornerA.y),
            width = (e->cornerC.x - e->cornerA.x);

    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->cornerA.x, e->cornerA.y);
    fprintf(fid, "%.2f %.2f rlineto\n", 0.0, height);
    fprintf(fid, "%.2f %.2f rlineto\n", width, 0.0);
    fprintf(fid, "%.2f %.2f rlineto\n", 0.0, -height);
    fprintf(fid, "closepath\n");
} 

void u_draw_polyline (FILE * fid, Point2D * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->x , e->y);
    e = e->next;
    
    while (e != NULL) {
        fprintf(fid, "%.2f %.2f lineto\n", e->x, e->y);
        e = e->next;
    }
}

// in eps mode we do not deal with labels but in gtu mode we do

void c2d_export (CAD2D * cad, char * file_name, char * options) {
    FILE * fid = fopen(file_name, "wt");

    if (fid != NULL) {
        if (strcmp(options, "eps") == 0)
            u_export_eps(cad, fid);
        else if (strcmp(options, "gtucad") == 0)
            u_export_gtucad(cad, fid);
        //! NOT IMPLEMENTED YET: else 

        fclose(fid);
    }
}

void u_export_eps (CAD2D * cad, FILE * fid) {
    Entity ** e = cad->list;
    int i;

    fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
    
    /* Check if canvas is decleared or not */
    if (cad->canvas != NULL)
        fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);

//! DELETE DELETE DELETE DELETE DELETE DELETE DELETE DELETE DELETE 
    printf("list size: %d\n", cad->list_size.cur);

    for (i = 0; i < cad->list_size.cur; ++i) {
//! DELETE DELETE DELETE DELETE DELETE DELETE DELETE DELETE DELETE 
        printf("label->type: %d\n", e[i]->label->type);
        switch (e[i]->label->type) {
            case point:
                //! NOT IMPLEMENTED YET
                break;
            case line:
                printf("Draw line: ");
                u_draw_line(fid, (Point2D *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case spline:
                //! NOT IMPLEMENTED YET
                break;
            case polyline:
                printf("Draw polyline: ");
                u_draw_polyline(fid, (Point2D *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case polygon:
                //! NOT IMPLEMENTED YET
                break;
            case rectangle:
                printf("Draw rectangle: ");
                u_draw_rectangle(fid, (Rectangle *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case circle:
            case arc:
                printf("Draw Arc: ");
                u_draw_arc(fid, (Arc *) e[i]->data);
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
                //!  NOT IMPLEMENTED YET: default:
        }
    }
    fprintf(fid, "\nshowpage\n");
}

void u_export_gtucad(CAD2D * cad, FILE * fid) {
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
