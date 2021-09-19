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

/* Initialize a new CAD at child of given hierarchy */
CAD2D * c2d_start_wh_hier (double width, double height, Hierarchy * h) {
    CAD2D * cad = c2d_start_wh(width, height);

    if (cad != NULL) {
        u_link_hierarchy(cad->hierarchy, h);
        h->parent = h;
    }

    return cad;
}

/*********************************************************************************
 * Hierarchy
*********************************************************************************/
//! NOT TESTED YET
Hierarchy * u_find_hierarchy (CAD2D * cad, const Hierarchy * h) {
    int i;
    Hierarchy * r = NULL;

    if (cad->hierarchy != h) {
        /* look the child's of root hierarchy */
        for (i = 0; i < cad->hierarchy->size.cur && r == NULL; ++i)
            if (cad->hierarchy->child[i] == h)
                r = h;
        /* if it isn't finded yet, look the children's children */
        if (r == NULL) {
            for (i = 0; i < cad->hierarchy->size.cur && r == NULL; ++i)
                r = u_find_hierarchy(cad->hierarchy->child[i]->cad, h);
        }
    }
    return r;
}

/* links given two hiearchy as child-parent relationship */
void u_link_hierarchy (Hierarchy * child, Hierarchy * parent) {
    Hierarchy ** tmp;
    int i;

    if (parent->size.cur == parent->size.max) {
        parent->size.max = parent->size.max == 0 ? INIT_HIER : parent->size.cur * 2; 

        tmp = (Hierarchy **) calloc(parent->size.max, sizeof(Hierarchy *));

        if (tmp != NULL) {
            for (i = 0; i < parent->size.cur; ++i)
                tmp[i] = parent->child[i];

            free(parent->child);
            parent->child = tmp;
        }
    }

    child->parent = parent;
    parent->child[(parent->size.cur)++] = child;
}

/* creates a hierarchy from given cad */
Hierarchy * c2d_create_hierarchy (CAD2D * cad) {
    Hierarchy * h = (Hierarchy *) malloc(sizeof(Hierarchy));

    if (h != NULL) {
        h->parent = NULL;
        h->cad = cad;
        h->child = NULL;
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
        h->parent = parent;

        /* add new hierarchy as child of given parent */
        u_link_hierarchy(h, parent);
    }

    return h;
}
/*********************************************************************************
 * Label
*********************************************************************************/
int u_get_hash (char * s, int q, int p) {
    int i;
    int code;

    for (i = 0; s[i] != '\0'; ++i)
        code = (code * q + s[i]) % p;
    //! Be sure p is the biggest prime number which is smaller than list max size
    return code;
}

/* Returns an index of hash value in case of unique label, o.w. returns -1 */
int u_get_hash_index (CAD2D * cad, Label * l) {
    int i, r = -2, h = l->hash_code;

    /* find proper where by linear-probing */
    for (i = 0; cad->list_size.max && r == -2; ++i) {
        if (h >= cad->list_size.max)
            h %= cad->list_size.max;
        
        /* same hash-code is a sign for same label */
        if (cad->list[h] != NULL) {
            if (strcmp(cad->list[h]->label->name, l->name) == 0)
                r = -1;
            else
                ++h;
        }
        else
            r = i; 
    }
    return r;
}

int u_get_tree_depth (Hierarchy * h) {
    int r;
    if (h == NULL)
        r = 0;
    else
        r = 1 + u_get_tree_depth(h->parent);
    return r;
}

char * u_create_label_name (CAD2D * cad, EntityType type) {
    char name[10];
    char * r;
    int n = 0, i = 0;
    /* 
        EntityType + Hierarchy level + instance
        R00 : First rectangle, hiearchy h0, 0. instance 
        Pl23: Third polyline in hierarchy h2
    */

    switch (type) {
        case point_t:
            name[n++] = 'P';
            break;
        case line_t:
            name[n++] = 'L';
            break;
        case spline_t:
            name[n++] = 'S';
            name[n++] = 'l';
            break;
        case polyline_t:
            name[n++] = 'P';
            name[n++] = 'l';
            break;
        case polygon_t:
            name[n++] = 'P';
            name[n++] = 'g';
            break;
        case rectangle_t:
            name[n++] = 'R';
            break;
        case circle_t:
            name[n++] = 'C';
            break;
        case arc_t:
            name[n++] = 'A';
            break;
        case ellipse_t:
            name[n++] = 'E';
            break;
        case text_t:
            name[n++] = 'T';
            break;
        case image_t:
            name[n++] = 'I';
            break;
    }

    //! what if depth larger than 9
    name[n++] = u_get_tree_depth(cad->hierarchy) + '0'; 

    /* Be sure produced label is unique, to do that use hash value of new label */
    do {
        name[n] = '0' + i;
        ++i;
        //! NOT IMPLEMENTED YET: Check if new label name is unique
    } while ();

    name[++n] = '\0';

    r = calloc(n + 1, sizeof(char));

    if (r != NULL) 
        strcpy(r, name);
    //! NOT IMPLEMENTED YET: Else

    return r;
}

Label * c2d_creat_label_default (CAD2D * cad, EntityType type) {
    Label * l = (Label *) malloc(sizeof(Label));
 
    if (l != NULL) {
        l->type = type;
        l->name = u_create_label_name(cad, type);
        //! l->hash_code =
    } 

    return l;
}

Label * c2d_create_label (CAD2D * cad, EntityType type, char * name) {
    Label * l = (Label *) malloc(sizeof(Label));
    if (l != NULL) {
        l->type = type;

        //! NOT IMPLEMENTED YET: Hash code calling parameters
        // l->hash_code = u_get_hash(l, 10 , PRIME);

        //! NOT IMPLEMETED YET: Be sure produced or given name is unique by hashing
    }

    return l;
}

/*********************************************************************************
 * Adding Basic CAD Entities
*********************************************************************************/
/* point, line, spline, polyline, polygon, rectangle, circle, arc, ellipse, text, image */
/*
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h, const Label * l) {}
*/

void u_insert_entity_list (CAD2D * cad, Entity * e) {
    Entity ** tmp;
    int i, h;

    /* Be sure hash-table has a place for new entity */
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
    
    i = u_get_hash_index(cad, e->label);

    //! given label is not unique
    if (i == -1) 
        printf("NOT UNIQUE\n");
    else 
        cad->list[i] = e;
}

/*
Entity * u_create_entity_filled (Label * l, void * data) {
    Entity * e = (Entity *) malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = data;
        e->label = l;
    }

    return e;
}
*/
Entity * u_create_entity (EntityType type) {
    Entity * e = (Entity *) malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = NULL;

        switch (type) {
            case point_t:
            case line_t:
            case polyline_t:
            case polygon_t:
                e->data = (Point2D *) malloc (sizeof(Point2D));
                break;
            case spline_t:
                e->data = (Spline *) malloc (sizeof(Spline));
                break;
            case rectangle_t:
                e->data = (Rectangle *) malloc (sizeof(Rectangle));
                break;
            case circle_t:
                e->data = (Circle *) malloc (sizeof(Circle));
                break;
            case arc_t:
                e->data = (Arc *) malloc (sizeof(Arc));
                break;
            case ellipse_t:
                e->data = (Ellipse *) malloc (sizeof(Ellipse));
                break;
            case text_t:
                e->data = (Text *) malloc (sizeof(Text));
                break;
            case image_t:
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

Point2D * c2d_create_point (double x, double y) {
    Point2D * p = (Point2D *) malloc(sizeof(Point2D));

    if (p != NULL) {
        p->x = x;
        p->y = y;
        p->next = NULL;
    }

    return p;
}

//! NOT TESTED YET:
Label * c2d_add_point_xy (CAD2D * cad, double x, double y) {
    Entity * e = u_create_entity(point_t);
    Point2D * data;

    if (e != NULL) {
        data = (Point2D *) e->data;

        data->x = x;
        data->y = y;
        data->next = NULL;
    }

    return e != NULL ? e->label : NULL;
}

//! NOT TESTED YET:
Label * c2d_add_point_p (CAD2D * cad, Point2D p) {
    Entity * e = u_create_entity(point_t);
    Point2D * data;

    if (e != NULL) {
        data = (Point2D *) e->data;

        data->x = p.x;
        data->y = p.y;
        data->next = NULL;
    }

    return e != NULL ? e->label : NULL;
}

//! NOT TESTED YET:
Label * c2d_add_point_ph (CAD2D * cad, Point2D p, const Hierarchy * h) {
    Hierarchy * r = u_find_hierarchy(cad, h);

    /* be sure given hierarchy exist */
    /* add point at desired hiearchy as if it's root */
    return r != NULL ? c2d_add_point_p(r->cad, p) : NULL;
}

Label * c2d_add_point_phl (CAD2D * cad, Point2D p, const Hierarchy * h, const Label * l) {
    //! NOT IMPLEMENTED YET
}

Label * c2d_add_line (CAD2D * cad, Point2D * start, Point2D * end) {
    Entity * e = u_create_entity(line_t);
    Point2D * data;

    if (e != NULL) {
        data = (Point2D *) e->data;
        data->x = start->x;
        data->y = start->y;
    
        data->next = c2d_create_point(end->x, end->y);
        //! NOT IMPLEMENTED YET: if (data->next == NULL)

        e->label = c2d_create_label(cad, line_t, "");
        u_insert_entity_list(cad, e); 
    }

    return e != NULL ? e->label : NULL;
}

//! NOT TESTED YET
Label * c2d_add_polyline (CAD2D * cad, Point2D * p) {
    Entity * e = u_create_entity(polyline_t);
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

        e->label = c2d_create_label(cad, polyline_t, "");
        u_insert_entity_list(cad, e);
    }
    
    return e != NULL ? e->label : NULL;
}


Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle) {
    Entity * e = u_create_entity(arc_t);
    Arc * data;

    if (e != NULL) {
        data = (Arc *) e->data;

        data->center = center;
        data->radius = radius;
        data->start_angle = start_angle;
        data->end_angle = end_angle;

        //! NOT IMPLEMENTED YET: Create a unique label
        e->label = c2d_create_label(cad, arc_t, "");
        u_insert_entity_list(cad, e);
    }

    return e != NULL ? e->label : NULL;
}

Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius) {
    Entity * e = u_create_entity(circle_t);
    Circle * data;

    if (e != NULL) {
        data = (Circle *) e->data;
        data->center = center;
        data->radius = radius;
        data->start_angle = 0.0;
        data->end_angle = 360.0;

        e->label = c2d_create_label(cad, circle_t, "");
        u_insert_entity_list(cad, e);
    }
    return e != NULL ? e->label : NULL;
}

//* Label * c2d_add_rectangle (CAD2D * cad, Point2D center , double width, double hight) {

Label * c2d_add_rectangle (CAD2D * cad, Point2D cornerA , Point2D cornerC) {
    Entity * e = u_create_entity(rectangle_t);
    Rectangle * data;
    
    if (e != NULL) {
        data = (Rectangle *) e->data;
        data->cornerA = cornerA;
        data->cornerC = cornerC;
        //! NOT IMPLEMENTED YET: data->style

        e->label = c2d_create_label(cad, rectangle_t, "");
        u_insert_entity_list(cad, e);
    }
    return e != NULL ? e->label : NULL;
}

Label * c2d_add_text (CAD2D * cad, Point2D * point, char * text) {
    Entity * e = u_create_entity(text_t);
    Text * data;

    if (e != NULL) {
        data = (Text *) e->data;
        data->point.x = point->x;
        data->point.y = point->y;
        //! NOT IMPLEMENTED YET: data->style
        data->text = (char *) calloc(strlen(text), sizeof(char));

        if (data->text != NULL)
            strcpy(data->text, text);
        //! NOT IMPLEMENTED YET: else
        e->label = c2d_create_label(cad, text_t, "");
        u_insert_entity_list(cad, e);
    }

    return e != NULL ? e->label : NULL;
}

/* 
Label * c2d_add_ellipse(CAD2D * cad, ...) {}
Label * c2d_add_splines(CAD2D * cad, ...) {}
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
void u_set_font_style (FILE * fid, FontStyle s, double thickness) {
     switch (s) {
        case Helvetica:
            fprintf(fid, "/Helvetica");
            break;
        case Courier:
            fprintf(fid, "/Courier");
            break;
        case Times:
            fprintf(fid, "/Times");
            break;
        case Coronet:
            fprintf(fid, "/Coronet");
            break;
        case Symbol:
            fprintf(fid, "/Symbol");
            break;
        case Albertus:
            fprintf(fid, "/Albertus");
            break;
        case Arial:
            fprintf(fid, "/Arial");
            break;
        case Bodoni:
            fprintf(fid, "/Bodoni");
            break;
        default:
            /* don't do anyting */
            return;
    }

    fprintf(fid, "findfont %.2f scalefont setfont\n", thickness);
}

void u_set_color (FILE * fid, RGBColor s) {
    fprintf(fid, "%.2f %.2f %.2f setrgbcolor\n", s.red, s.green, s.blue);
}

void u_set_line_Style (FILE * fid, LineStyle s) {
    if (s == dashed)
        fprintf(fid, "[3 3] 0 setdash\n");
    
}
void u_set_draw_style (FILE * fid, DrawStyle s) {
    if (s == fill)
        fprintf(fid, "fill\n");
    else
        fprintf(fid, "stroke\n");
}

void u_set_style (FILE * fid, Style * s) {
    u_set_color(fid, s->color);
    u_set_font_style(fid, s->font_type, s->thickness);
    u_set_line_Style(fid, s->line_type);
    u_set_draw_style(fid, s->draw);
}

void u_draw_line (FILE * fid, Point2D * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->x , e->y);
    fprintf(fid, "%.2f %.2f lineto\n", e->next->x, e->next->y);
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

void u_draw_text (FILE * fid, Text * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->point.x , e->point.y);
    //! NOT IMPLEMENTED YET: 
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
            case point_t:
                //! NOT IMPLEMENTED YET
                break;
            case line_t:
                printf("Draw line: ");
                u_draw_line(fid, (Point2D *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case spline_t:
                //! NOT IMPLEMENTED YET
                break;
            case polyline_t:
                printf("Draw polyline: ");
                u_draw_polyline(fid, (Point2D *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case polygon_t:
                //! NOT IMPLEMENTED YET
                break;
            case rectangle_t:
                printf("Draw rectangle: ");
                u_draw_rectangle(fid, (Rectangle *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case circle_t:
            case arc_t:
                printf("Draw Arc: ");
                u_draw_arc(fid, (Arc *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case ellipse_t:
                //! NOT IMPLEMENTED YET
                break;
            case text_t:
                printf("Draw Text: ");
                u_draw_text(fid, (Text *) e[i]->data);
                fprintf(fid, "stroke\n");
                printf("Done\n");
                break;
            case image_t:
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
