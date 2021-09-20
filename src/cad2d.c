#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cad2d.h"

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
 * Label
*********************************************************************************/
/* Produce a hash code according to given string */
int u_hash_function (char * s, int q, int p) {
    int i;
    int code;

    //! Be sure p is the biggest prime number which is smaller than list max size
    for (i = 0; s[i] != '\0'; ++i)
        code = (code * q + s[i]) % p;
    return code;
}

/* Returns an index of hash value in case of unique label, o.w. returns -1 */
int u_get_hash_code (CAD2D * cad, char * key) {
    int i, r = -2, h = u_hash_function(key, 10, 7);
    //! NOT IMPLEMENTED YET: Hash code calling parameters should be variable inside hash

    printf("Max-List-Size: %d\nhashInit(%s): %d\n", cad->list_size.max, key, h);
    
    /* First label is unique since list initiliazed yet */
    if (cad->list_size.max == 0) 
        r = h;
    else {
        /* hashing by linear-probing */
        for (i = 0; i < cad->list_size.max && r == -2; ++i) {
            if (h >= cad->list_size.max)
                h %= cad->list_size.max;
            
            /* same hash-code is a sign for same key */
            if (cad->list[h] != NULL) {
                printf("...\n");
                if (strcmp(cad->list[h]->label->name, key) == 0)
                    r = -1;
                else
                    ++h;
            }
            else
                r = h; 
        }
    }

    printf("hashEnd (%s): %d\n\n", key, r);
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

char * u_create_label_name (CAD2D * cad, Label * l) {
    char name[10];
    char * r;
    int n = 0, i = 0;
    /* 
        EntityType + Hierarchy level + instance
        R00 : First rectangle, hiearchy h0, 0. instance 
        Pl23: Third polyline in hierarchy h2
    */

    switch (l->type) {
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
    name[n + 1] = '\0';
    
    /* rc00, rc01, rc02, rc03... */
    do {
        name[n] = '0' + i;
        ++i;
        l->hash_code = u_get_hash_code(cad, name);
    } while (i < 10 && l->hash_code == -1);

    r = calloc(n + 2, sizeof(char));

    if (r != NULL) 
        strcpy(r, name);
    //! NOT IMPLEMENTED YET: Else

    return r;
}
/* Creates an default unique label  */
Label * c2d_create_label_default (CAD2D * cad, EntityType type) {
    Label * l = (Label *) malloc(sizeof(Label));
 
    if (l != NULL) {
        //* create_label function produce unique label
        l->type = type;
        l->name = u_create_label_name(cad, l);
        // l->hash_code = u_get_hash_code(cad, l->name);
    } 

    return l;
}

Label * c2d_create_label (CAD2D * cad, EntityType type, char * name) {
    Label * l = (Label *) malloc(sizeof(Label));
    if (l != NULL) {
        l->type = type;
        l->name = name;
        l->hash_code = u_get_hash_code(cad, l->name);

        if (l->hash_code == -1) {
            printf("Given label name already exist\n");
            free(l);
            l = NULL;
        }
    }

    return l;
}

/*********************************************************************************
 * Style
*********************************************************************************/
void c2d_set_entity_style (Label * label, LineStyle l, RGBColor c, DrawStyle d, double w) {
    //! NOT IMPLEMENTED YET
}

EntityStyle * c2d_create_entity_style (LineStyle l, RGBColor c, DrawStyle d, double w) {
    EntityStyle * style = (EntityStyle *) malloc(sizeof(EntityStyle));
    
    if (style != NULL) {
        style->line = l;
        style->color = c;
        style->draw = d;
        style->line_width = w;
    }

    return style;
}

TextStyle * c2d_create_text_style (FontStyle f, RGBColor c, double s) {
    TextStyle * style = (TextStyle *) malloc(sizeof(EntityStyle));

    if (style != NULL) {
        style->color = c;
        style->font = f;
        style->scale = s;
    }
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
    if (cad->list_size.cur >= cad->list_size.max - 1) {
        cad->list_size.max = cad->list_size.max == 0 ? INIT_HASH : cad->list_size.max * 2;        
        
        tmp = (Entity **) calloc(cad->list_size.max, sizeof(Entity *));

        if (tmp != NULL) {
            for (i = 0; i < cad->list_size.cur; ++i)
                tmp[i] = cad->list[i];
            
            free(cad->list);
            cad->list = tmp;
        }
    }
    ++(cad->list_size.cur);
    cad->list[e->label->hash_code] = e;
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
        e->label = c2d_create_label_default(cad, point_t);
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
        e->label = c2d_create_label_default(cad, point_t);
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

        e->label = c2d_create_label_default(cad, line_t);
        u_insert_entity_list(cad, e); 
    }

    return e != NULL ? e->label : NULL;
}

//! NOT TESTED YET
Label * c2d_add_polyline (CAD2D * cad, Point2D * p, int size) {
    Entity * e = u_create_entity(polyline_t);
    Point2D * data;
    Point2D ** trav;
    int i;

    if (e != NULL) {
        trav = (Point2D **) &e->data;

        for (i = 0; i < size; ++i) {
            *trav = c2d_create_point(p[i].x, p[i].y);
            trav = &(*trav)->next;
        }

        e->label = c2d_create_label_default(cad, polyline_t);
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

        e->label = c2d_create_label_default(cad, arc_t);
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

        
        e->label = c2d_create_label_default(cad, circle_t);
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

        e->label = c2d_create_label_default(cad, rectangle_t);
        u_insert_entity_list(cad, e);
    }
    return e != NULL ? e->label : NULL;
}

Label * c2d_add_text (CAD2D * cad, Point2D * point, char * text, TextStyle * style) {
    Entity * e = u_create_entity(text_t);
    Text * data;

    if (e != NULL) {
        data = (Text *) e->data;
        data->point.x = point->x;
        data->point.y = point->y;
        data->style = style;
        data->text = (char *) calloc(strlen(text), sizeof(char));

        if (data->text != NULL)
            strcpy(data->text, text);
        //! NOT IMPLEMENTED YET: else
        e->label = c2d_create_label_default(cad, text_t);
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
RGBColor u_convert_rgb (Color c) {
    RGBColor rgb;

    switch (c) {
        case red:
            rgb.red = 1, rgb.green = 0, rgb.blue = 0; break;
        case green:
            rgb.red = 0, rgb.green = 1, rgb.blue = 0; break;
        case green_dark:
            rgb.red = 0, rgb.green = 0.5, rgb.blue = 0; break;
        case blue:
            rgb.red = 0, rgb.green = 0, rgb.blue = 1; break;
        case blue_light:
            rgb.red = 0, rgb.green = 1, rgb.blue = 1; break;
        case magenta:
            rgb.red = 1, rgb.green = 0, rgb.blue = 1; break;
        case yellow:
            rgb.red = 1, rgb.green = 1, rgb.blue = 0; break;
        case orange:
            rgb.red = 1, rgb.green = 0.7, rgb.blue = 0; break;
        case purple:
            rgb.red = 0.7, rgb.green = 0.3, rgb.blue = 1; break;
        case brown:
            rgb.red = 0.7, rgb.green = 0.3, rgb.blue = 0; break;
        case white:
            rgb.red = 1, rgb.green = 1, rgb.blue = 1; break;
        case black:
        default:
            rgb.red = 0, rgb.green = 0, rgb.blue = 0;
    }

    return rgb;
}

/* set font, color and scale of the text */
void u_print_text_style (FILE * fid, TextStyle * s) {
    fprintf(fid, "%.2f %.2f %.2f setrgbcolor\n", s->color.red, s->color.green, s->color.blue);

    switch (s->font) {
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

    fprintf(fid, " findfont %.2f scalefont setfont\n", s->scale);
}

void u_print_entity_style (FILE * fid, EntityStyle * s) {
    fprintf(fid, "%.2f %.2f %.2f setrgbcolor\n", s->color.red, s->color.green, s->color.blue);
    fprintf(fid, "%.2f setlinewidth\n", s->line_width);
    fprintf(fid, s->line == dashed ? "[3 3] 0 setdash\n" : "");
    fprintf(fid, s->draw == fill ? "fill" : "stroke");
}

void u_print_xy_plane (FILE * fid, Canvas * canvas, EntityStyle * s) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", canvas->start.x, 0.0);
    fprintf(fid, "%.2f %.2f lineto\n", canvas->end.x, 0.0);
    fprintf(fid, "%.2f %.2f moveto\n", 0.0, canvas->start.y);
    fprintf(fid, "%.2f %.2f lineto\n", 0.0, canvas->end.y);
    u_print_entity_style(fid, s);
}

void u_print_line (FILE * fid, Point2D * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->x , e->y);
    fprintf(fid, "%.2f %.2f lineto\n", e->next->x, e->next->y);
}

void u_print_arc (FILE * fid, Arc * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n", e->center.x, e->center.y, e->radius, e->start_angle, e->end_angle);
}

void u_print_rectangle (FILE * fid, Rectangle * e) {
    double  height = (e->cornerC.y - e->cornerA.y),
            width = (e->cornerC.x - e->cornerA.x);

    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->cornerA.x, e->cornerA.y);
    fprintf(fid, "%.2f %.2f rlineto\n", 0.0, height);
    fprintf(fid, "%.2f %.2f rlineto\n", width, 0.0);
    fprintf(fid, "%.2f %.2f rlineto\n", 0.0, -height);
    fprintf(fid, "closepath\n");
} 

void u_print_polyline (FILE * fid, Point2D * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->x , e->y);
    e = e->next;
    
    while (e != NULL) {
        fprintf(fid, "%.2f %.2f lineto\n", e->x, e->y);
        e = e->next;
    }
}

void u_print_text (FILE * fid, Text * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->point.x , e->point.y);
    u_print_text_style(fid, e->style);
    fprintf(fid, "(%s) show\n", e->text);
}

void u_export_eps (CAD2D * cad, FILE * fid) {
    Entity ** e = cad->list;
    int i;

    fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
    
    /* Check if canvas is decleared or not */
    if (cad->canvas != NULL)
        fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);

        printf("Entity Number: %d\n", cad->list_size.cur);
        printf("EXPORT:\n");

    for (i = 0; i < cad->list_size.max; ++i) {
        if (e[i] != NULL) {
            printf("\tc: %s\th: %d\n", e[i]->label->name, e[i]->label->hash_code);
            switch (e[i]->label->type) {
                case point_t:
                    //! NOT IMPLEMENTED YET
                    break;
                case line_t:
                    u_print_line(fid, (Point2D *) e[i]->data);
                    fprintf(fid, "stroke\n");
                    break;
                case spline_t:
                    //! NOT IMPLEMENTED YET
                    break;
                case polyline_t:
                    u_print_polyline(fid, (Point2D *) e[i]->data);
                    fprintf(fid, "stroke\n");
                    break;
                case polygon_t:
                    //! NOT IMPLEMENTED YET
                    break;
                case rectangle_t:
                    u_print_rectangle(fid, (Rectangle *) e[i]->data);
                    fprintf(fid, "stroke\n");
                    break;
                case circle_t:
                case arc_t:
                    u_print_arc(fid, (Arc *) e[i]->data);
                    fprintf(fid, "stroke\n");
                    break;
                case ellipse_t:
                    //! NOT IMPLEMENTED YET
                    break;
                case text_t:
                    u_print_text(fid, (Text *) e[i]->data);
                    break;
                case image_t:
                    //! NOT IMPLEMENTED YET
                    break;
                    //!  NOT IMPLEMENTED YET: default:
            }
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
