#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cad2d.h"

/*********************************************************************************
 * Hierarchy
*********************************************************************************/
Hierarchy * u_check_child_hierarchy (const Hierarchy * h) {
    //! NOT IMPLEMENTED YET
}

Hierarchy * u_check_parent_hierarchy (const Hierarchy * h) {
    //! NOT IMPLEMENTED YET
}

/* Cheks's if given hierarchy exist */
Hierarchy * u_find_hierarchy (CAD2D * cad, const Hierarchy * h) {
    int i;
    Hierarchy * r = NULL;
    //! NOT IMPLEMENTED YET

    if (cad->hierarchy != h) {

        /* look the child's of root hierarchy */
        for (i = 0; i < cad->hierarchy->size.cur && r == NULL; ++i)
            if (cad->hierarchy->child[i] == h)
                r = h;
        /* if isn't founded yet, look the children's children */
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

int u_get_tree_depth (Hierarchy * h) {
    int r;
    
    if (h->parent == NULL)
        r = 0;
    else
        r = 1 + u_get_tree_depth(h->parent);
    return r;
}

char * u_deci_to_hexa (int deci) {
    int r, i = 0, j;
    char tmp[20], * hexa;

    if (deci == 0)
        tmp[i++] = '0';
    else {
        do {
            r = deci % 16;
            
            if (r < 10)
                tmp[i++] =  r + '0';
            else
                tmp[i++] = r - 10 + 'A';
            
            deci /= 16;
        } while (deci > 0);
    }

    hexa = (char *) calloc(i + 1, sizeof(char));

    if (hexa != NULL) {
        j = 0;

        while (i >= 0)
            hexa[j++] = tmp[--i];
        hexa[j] = '\0';
    }

    return hexa;
}

char * u_create_hierarchy_name (Hierarchy * h) {
    char * tmp = u_deci_to_hexa((u_get_tree_depth(h)));;
    char * r = (char *) calloc(strlen(tmp) + 2, sizeof(char));

    r[0] = 'h';
    strcpy(r + 1, tmp);

    free(tmp);
    return r;    
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
        h->name = u_create_hierarchy_name(h);
        cad->hierarchy = h; //???????????????
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
        cad->elist = NULL;
        cad->list_size.max = 0;
        cad->list_size.cur = 0;
        cad->canvas = NULL;
        cad->llist = NULL;
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
/* Produce a hash-code according to given string */
int u_hash_function (char * s, int q, int p) {
    int i;
    int code = 0;

    //! Be sure p is the biggest prime number which is smaller than elist max size
    for (i = 0; s[i] != '\0'; ++i)
        code = (code * q + s[i]) % p;
    return code;
}

/* Returns an index of hash value in case of unique label, o.w. returns -1 */
int u_get_hash_code (CAD2D * cad, char * key) {
    int i, r = -2, h = u_hash_function(key, 10, 7);
    //! NOT IMPLEMENTED YET: Hash code calling parameters should be variable inside hash

    printf("Max-List-Size: %d\nhashInit(%s): %d\n", cad->list_size.max, key, h);
    
    /* First label is unique since elist initiliazed yet */
    if (cad->list_size.max == 0) 
        r = h;
    else {
        /* hashing by linear-probing */
        for (i = 0; i < cad->list_size.max && r == -2; ++i) {
            if (h >= cad->list_size.max)
                h %= cad->list_size.max;
            
            /* same hash-code is a sign for same key */
            if (cad->elist[h] != NULL) {
                printf("...\n");
                if (strcmp(cad->elist[h]->label->name, key) == 0)
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

char * u_create_label_name (CAD2D * cad, Label * l) {
    char name[10], * tmp;
    char * r = NULL;
    int n = 0, i = 0;

    /* LabelName: EntityType + Hierarchy level/depth + instance */
    switch (l->type) {
        case point_t:
            name[n++] = 'P';    break;
        case line_t:
            name[n++] = 'L';    break;
        case spline_t:
            name[n++] = 'S';
            name[n++] = 'l';    break;
        case polyline_t:
            name[n++] = 'P';
            name[n++] = 'l';    break;
        case polygon_t:
            name[n++] = 'P';
            name[n++] = 'g';    break;
        case rectangle_t:
            name[n++] = 'R';    break;
        case circle_t:
            name[n++] = 'C';    break;
        case arc_t:
            name[n++] = 'A';    break;
        case ellipse_t:
            name[n++] = 'E';    break;
        case text_t:
            name[n++] = 'T';    break;
        case image_t:
            name[n++] = 'I';    break;
    }

    tmp = u_deci_to_hexa(u_get_tree_depth(cad->hierarchy)); 

    for (i = 1; tmp[i] != '\0'; ++i)
        name[n++] = tmp[i];
    name[n + 1] = '\0';
    
    free(tmp);

    /* Be sure produced label is unique, to do that use hash value of new label */
    do {
        name[n] = '0' + i;
        ++i;
        l->hash_code = u_get_hash_code(cad, name);
    } while (i < 10 && l->hash_code == -1);

    r = calloc(n + 2, sizeof(char));

    if (r != NULL) 
        strcpy(r, name);

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
            printf("Given label name is already exist\n");
            free(l);
            l = NULL;
        }
    }

    return l;
}

/*********************************************************************************
 * Style
*********************************************************************************/
Entity * u_find_entity (CAD2D * cad, Label * l) {
    Entity * e = NULL, * tmp;    
    int i, h = l->hash_code;

    for (i = 0; i < cad->list_size.max && e == NULL; ++i) {
        if (h >= cad->list_size.max)
            h %= cad->list_size.max;
        //! is hash code should point it's place
        printf("...\n");
        tmp = cad->elist[h + i];
        if (tmp != NULL && strcmp(l->name, tmp->label->name) == 0)
            e = tmp;
    } 
    //! NOT IMPLEMENTED YET: Also check other hierarchies 
    return e;
}

/* set's the style of entity by given it's label */
EntityStyle * c2d_set_entity_style (CAD2D * cad, Label * label, LineStyle l, RGBColor c, DrawStyle d, double w) {
    EntityStyle * style = NULL;
    Entity * e = u_find_entity(cad, label); 
    
    if (e != NULL) {
        style = (EntityStyle *) malloc(sizeof(EntityStyle));

        if (style != NULL) {
            style->color = c;
            style->draw = d;
            style->line = l;
            style->line_width = w;

            e->style = style;
        }
    }

    return style;
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

void u_insert_label_list (LabeList ** llist, Label * l) {
    while (*llist != NULL) llist = &(*llist)->next;

    *llist = (LabeList *) malloc(sizeof(LabeList));

    if (*llist != NULL) {
        (*llist)->label = l;
        (*llist)->next = NULL;
    }
}

void u_insert_entity_list (CAD2D * cad, Entity * e) {
    Entity ** tmp;
    int i;

    /* Be sure hash-table has a place for new entity */
    if (cad->list_size.cur >= cad->list_size.max - 1) {
        cad->list_size.max = cad->list_size.max == 0 ? INIT_HASH : cad->list_size.max * 2;        
        
        tmp = (Entity **) calloc(cad->list_size.max, sizeof(Entity *));

        if (tmp != NULL) {
            for (i = 0; i < cad->list_size.cur; ++i)
                tmp[i] = cad->elist[i];
            
            free(cad->elist);
            cad->elist = tmp;
        }
    }
    ++(cad->list_size.cur);
    cad->elist[e->label->hash_code] = e;
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

/* common for point and lines */
void u_free_point (Point2D * p) {
    Point2D * tmp;

    while (p != NULL) {
        tmp = p;
        p = p->next;
        free(tmp);
    }
}

void u_free_text (Text * t) {
    free(t->text);
    free(t->style);
}

void u_free_entity (Entity * e) {
    if (e != NULL) {
        if (e->label != NULL) {
            if (e->data != NULL) {
                switch (e->label->type) {
                    case point_t:
                    case line_t:
                    case polyline_t:
                    case polygon_t:
                        u_free_point((Point2D *) e->data);
                        break;
                    case spline_t:
                        //! NOT IMPLEMENTED YET
                        break;
                    case rectangle_t:
                        free(e->data);
                        break;
                    case circle_t:
                    case arc_t:
                        free(e->data);
                        break;
                    case ellipse_t:
                        //! NOT IMPLEMENTED YET
                        break;
                    case text_t:
                        u_free_text(e->data);       
                        break;
                    case image_t:
                        //! NOT IMPLEMENTED YET
                        break;
                }
            }
            free(e->label->name);
            free(e->label);
        } 
        else //! NOT SURE
            free(e->data);
        free(e->style);
        free(e);
    }
}

void c2d_remove_entity (CAD2D * cad, Label ** l) {
    Entity * e = u_find_entity(cad, *l);
    u_free_entity(e);
    *l = NULL;
}

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
            case arc_t:
                e->data = (Circle *) malloc (sizeof(Circle));
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

int u_is_in_canvas (Canvas * c, Point2D * p) {
    int r = 1;

    if (c != NULL ) {
        while (p != NULL && r == 1) {
            if (p->x > c->end.x || p->y > c->end.y || p->x < c->start.x || p->y < c->start.y)
                r = 0;
            else
                p = p->next;
        }
    }

    return r;
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
        //! check if we are in canvas area
        e->label = c2d_create_label_default(cad, point_t);
        u_insert_entity_list(cad, e); 
        u_insert_label_list(&cad->llist, e->label); 
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
        u_insert_entity_list(cad, e); 
        u_insert_label_list(&cad->llist, e->label); 
    }

    return e != NULL ? e->label : NULL;
}

//! NOT TESTED YET:
Label * c2d_add_point_ph (CAD2D * cad, Point2D p, const Hierarchy * h) {
    Hierarchy * r = u_find_hierarchy(cad, h);
    /* be sure given hierarchy exist */
    /* add point at desired hiearchy as if it's root */
    
    
    // u_insert_entity_list(cad, e); 
    // u_insert_label_list(&cad->llist, e->label); 

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

        if (u_is_in_canvas(cad->canvas, data)) {
            e->label = c2d_create_label_default(cad, line_t);
            u_insert_entity_list(cad, e); 
            u_insert_label_list(&cad->llist, e->label); 
        }
        else {
            u_free_entity(e);
            e = NULL;
        }
    }
    
    return e != NULL ? e->label : NULL;
}

//! NOT TESTED YET
Label * c2d_add_polyline (CAD2D * cad, Point2D * p, int size) {
    Entity * e = u_create_entity(polyline_t);
    Point2D * data;
    Point2D ** trav;
    int i;

    if (e != NULL && u_is_in_canvas(cad->canvas, p)) {
        trav = (Point2D **) &e->data;

        for (i = 0; i < size; ++i) {
            *trav = c2d_create_point(p[i].x, p[i].y);
            trav = &(*trav)->next;
        }
        e->label = c2d_create_label_default(cad, polyline_t);
        u_insert_entity_list(cad, e);
        u_insert_label_list(&cad->llist, e->label); 
    }
    else
        u_free_entity(e);
    
    return e != NULL ? e->label : NULL;
}

Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius) {
    Entity * e = u_create_entity(circle_t);
    Circle * data;

    if (e != NULL) {
        //! Check if it's in canvas
        data = (Circle *) e->data;
        data->center = center;
        data->radius = radius;
        data->start_angle = 0.0;
        data->end_angle = 360.0;
        
        e->label = c2d_create_label_default(cad, circle_t);
        u_insert_entity_list(cad, e);
        u_insert_label_list(&cad->llist, e->label); 
    }
    return e != NULL ? e->label : NULL;
}

Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle) {
    Entity * e = u_create_entity(arc_t);
    Circle * data;

    if (e != NULL) {
        data = (Circle *) e->data;

        data->center = center;
        data->radius = radius;
        data->start_angle = start_angle;
        data->end_angle = end_angle;

        e->label = c2d_create_label_default(cad, arc_t);
        u_insert_entity_list(cad, e);
        u_insert_label_list(&cad->llist, e->label); 
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
        //! Check if it's in canvas

        e->label = c2d_create_label_default(cad, rectangle_t);
        u_insert_entity_list(cad, e);
        u_insert_label_list(&cad->llist, e->label); 
    }
    return e != NULL ? e->label : NULL;
}

Label * c2d_add_text (CAD2D * cad, Point2D * point, char * text, TextStyle * style) {
    Entity * e = u_create_entity(text_t);
    Text * data;

    //! Check if it's still in canvas
    if (e != NULL && u_is_in_canvas(cad->canvas, point)) {
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
        u_insert_label_list(&cad->llist, e->label); 
    }

    return e != NULL ? e->label : NULL;
}

/* 
Label * c2d_add_ellipse(CAD2D * cad, ...) {}
Label * c2d_add_splines(CAD2D * cad, ...) {}
Label * c2d_add_polygons(CAD2D * cad, ...) {}
Label * c2d_add_image(CAD2D * cad, ...) {}
*/

/* general function for adding basic entities */
Label * u_add_entity () {
    //! NOT IMPLEMENTED YET
}

/*********************************************************************************
 * Measurement between two Labels
*********************************************************************************/

/* measures the distance between two entity given by their label */
// double c2d_measure (CAD2D * cad, const Label * ls, const Label * lt) {
double c2d_measure (CAD2D * cad, Label * ls, Label * lt) {
    Entity * s = u_find_entity(cad, ls), * t = u_find_entity(cad, lt);
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

void u_print_entity_style (FILE * fid, Label * l, EntityStyle * s) {
    fprintf(fid, "%.2f %.2f %.2f setrgbcolor\n", s->color.red, s->color.green, s->color.blue);
    fprintf(fid, "%.2f setlinewidth\n", s->line_width);
    fprintf(fid, s->line == dashed ? "[3 3] 0 setdash\n" : "");
    fprintf(fid, s->draw == stroke && l->type != polygon_t ? "stroke\n" : "fill\n");
}

void u_print_xy_plane (FILE * fid, Canvas * canvas, EntityStyle * s) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", canvas->start.x, 0.0);
    fprintf(fid, "%.2f %.2f lineto\n", canvas->end.x, 0.0);
    fprintf(fid, "%.2f %.2f moveto\n", 0.0, canvas->start.y);
    fprintf(fid, "%.2f %.2f lineto\n", 0.0, canvas->end.y);
}

void u_print_circle (FILE * fid, Circle * e) {
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

/* common function for line, polyline and polygone types */
void u_print_line (FILE * fid, Point2D * e) {
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
    LabeList * l = cad->llist;
    Entity * e;

    fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
    
    /* Check if canvas is decleared or not */
    if (cad->canvas != NULL)
        fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);

        printf("Entity Number: %d\n", cad->list_size.cur);
        printf("EXPORT:\n");

    /* export current hierarchy */
    printf("Hierarchy: %s\n", cad->hierarchy->name);
    
    while (l != NULL) {
            e = u_find_entity(cad, l->label);
            
            //! is if needed
            if (e != NULL) {
                printf("\tc: %s\thash-code: %d\n", e->label->name, e->label->hash_code);
                
                switch (e->label->type) {
                    case point_t:
                        //! NOT IMPLEMENTED YET
                        break;
                    case spline_t:
                        //! NOT IMPLEMENTED YET
                        break;
                    case line_t:
                    case polyline_t:
                    case polygon_t:
                        u_print_line(fid, (Point2D *) e->data);
                        break;
                    case rectangle_t:
                        u_print_rectangle(fid, (Rectangle *) e->data);
                        break;
                    case circle_t:
                    case arc_t:
                        u_print_circle(fid, (Circle *) e->data);
                        break;
                    case ellipse_t:
                        //! NOT IMPLEMENTED YET
                        break;
                    case text_t:
                        u_print_text(fid, (Text *) e->data);
                        break;
                    case image_t:
                        //! NOT IMPLEMENTED YET
                        break;
                    default:
                        continue;
                }

                if (e->style != NULL) 
                    u_print_entity_style(fid, e->label, e->style);
                else if (e->label->type != text_t)
                    fprintf(fid, "stroke\n");

                l = l->next;
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
        else printf("%s is not a valid file format\nPlease select 'gtucad' or 'eps'\n", options);

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