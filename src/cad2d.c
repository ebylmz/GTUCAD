#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cad2d.h"

#define TRY -2
#define FAIL -1

/*********************************************************************************
 * Hierarchy
*********************************************************************************/
Hierarchy * u_check_child_hierarchy (const Hierarchy * h) {
    //! NOT IMPLEMENTED YET
}

Hierarchy * u_check_parent_hierarchy (const Hierarchy * h) {
    //! NOT IMPLEMENTED YET
}

int u_is_prime (int n) {
    int i, r = 1, b = sqrt(n);

    if (n < 2)
    	r = 0;
    else 
	    for (i = 2; i <= b && r == 1; ++i)
	        if (n % i == 0)
	            r = 0;
    return r;
}

/* Returs largest prime number which is smaller than n */
int u_get_close_prime (int n) {
    while (--n > 1)
        if (u_is_prime(n))
            break;
    return n;
}

/* Produce a hash-code according to given key value */
int u_create_hash_code (char * key, int size) {
    int i;
    int code = 0;
    int p, q = 97;  //! q

    /* set the p as largest prime smaller than size */
    if (size == 0)
        p = INIT_HASH;
    else
        p = u_get_close_prime(size);
    
    printf("p: %d\n", p);

    for (i = 0; key[i] != '\0'; ++i)
        code = (code * q + key[i]) % p;
    return code;
}

//! NOT TESTED YET
Hierarchy * u_find_hierarchy (CAD2D * cad, const Hierarchy * h) {
    int i, hcode = u_create_hash_code(h->name, h->size);
    Hierarchy * r = NULL;

    if (cad->hierarchy != h) {
        /* look the child's of root hierarchy */
        
        for (i = 0; i < cad->hierarchy->size && r == NULL; ++i) {
            if (hcode >= cad->hierarchy->size) 
                hcode %= cad->hierarchy->size;

            if (cad->hierarchy->child[hcode + i] == h)
                r = h;
        }
        /* if isn't founded yet, look the children's children */
        if (r == NULL) {
            for (i = 0; i < cad->hierarchy->size && r == NULL; ++i)
                r = u_find_hierarchy(cad->hierarchy->child[i]->cad, h);
        }
    }

    return r;
}

int u_check_unique_hierarchy (Hierarchy * child) {
    Hierarchy ** tmp, * parent = child->parent;
    int i, r, hcode;

    //! ALSO CHECK THE CHILD OF THE CHILD HIERARCHY

    if (parent == NULL)
        r = 1;
    else {
        hcode = u_create_hash_code(child->name, parent->size);
        /* First hierarchy is unique since list initiliazed yet */
        if (parent->size == 0)
            r = hcode;
        else {
            /* use hash-code as index of the unique hierarchy */
            r = TRY;
            for (i = 0; i < parent->size && r == TRY; ++i) {
                if (hcode >= parent->size)
                    hcode %= parent->size;

                /* same hash-code is a sign for same key values */
                if (parent->child[hcode] != NULL) {
                    if (strcmp(parent->child[hcode]->name, child->name) == 0)
                        r = FAIL;
                    else
                        ++hcode;
                }
                else 
                    r = hcode;
            }
        }
    }

    return r;
}

/* links given two hiearchy as child-parent relationship */
int u_link_hierarchy (Hierarchy * child, Hierarchy * parent) {
    Hierarchy ** tmp;
    int i = 0, n, r = u_check_unique_hierarchy(child);

    if (r == FAIL)  //!!!!! Is it okay?
        printf("'%s' named Hierarchy already exist\n", child->name);
    else {
        /*  u_check_unique_hierarchy() returns the next empty place
            based on it's hash-func regarding the size of the array.
            So be sure we are not exceeding the size                */
        if (r >= parent->size) {
            n = parent->size;

            if (parent->size == 0)
                parent->size = INIT_HASH;
            else
                parent->size *= 2;

            tmp = (Hierarchy **) calloc(parent->size, sizeof(Hierarchy *));

            if (tmp != NULL) {
                for (i = 0; i < n; ++i) 
                    tmp[n] = parent->child[n];

                free(parent->child);
                parent->child = tmp;
            }
            else
                r = FAIL;
        }
        
        if (r != FAIL)
            parent->child[r] = child; 
    }

    return r;
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
    char c = '0', * tmp, * r;
    int n;
    
    tmp = u_deci_to_hexa((u_get_tree_depth(h)));;
    n = strlen(tmp);
    r = (char *) calloc(strlen(tmp) + 3, sizeof(char));
    
    if (r != NULL) {
        r[0] = 'h';
        strcpy(r + 1, tmp);
        h->name = r;
        
        do {
            if (c == '9' + 1)
                c = 'A';

            r[n + 1] = c++;

            printf(">>>>>>>>>>>><%s\n\n", h->name);
        } while (c <= 'Z' && u_check_unique_hierarchy(h) == FAIL);
        r[n + 2] = '\0';
    }

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
        h->size = 0;
        h->name = u_create_hierarchy_name(h);
        cad->hierarchy = h; //???????????????
    }

    return h;
}

//* Hierarchy * c2d_create_hierarchy?(CAD2D * cad, …) {}

/* creates a hierarchy from given cad and add it as a child of given parent hierarchy */
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
        cad->elist_size = 0;
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
/* returns the index of it's index if it's unique, o.w. returns -1 */
int u_check_unique_label (CAD2D * cad, Label * l) {
    int i, r, hcode = l->hash_code;

    printf("Max-List-Size: %d\nhashInit(%s): %d\n", cad->elist_size, l->name, hcode);
    
    /* First label is unique since elist initiliazed yet */
    if (cad->elist_size == 0) 
        r = hcode;
    else {
        /* hashing by linear-probing */
        r = TRY;
        for (i = 0; i < cad->elist_size && r == TRY; ++i) {
            if (hcode >= cad->elist_size)
                hcode %= cad->elist_size;
            
            /* same hash-code is a sign for same key values */
            if (cad->elist[hcode] != NULL) {
                if (strcmp(cad->elist[hcode]->label->name, l->name) == 0)
                    r = FAIL;
                else
                    ++hcode;
            }
            else
                r = hcode; 
        }
    }

    printf("hashEnd (%s): %d\n\n", l->name, r);
    return r;
}

char * u_create_label_name (CAD2D * cad, Label * l) {
    char c = '0', * r = calloc(10, sizeof(char)),
         * h;
    int n = 0;

    /* LabelName: EntityType + Hierarchy level/depth + instance */
    switch (l->type) {
        case point_t:
            r[0] = 'P';    break;
        case line_t:
            r[0] = 'L';    break;
        case spline_t:
            r[0] = 'S';
            r[1] = 'l';    break;
        case polyline_t:
            r[0] = 'P';
            r[1] = 'l';    break;
        case polygon_t:
            r[0] = 'P';
            r[1] = 'g';    break;
        case rectangle_t:
            r[0] = 'R';    break;
        case circle_t:
            r[0] = 'C';    break;
        case arc_t:
            r[0] = 'A';    break;
        case ellipse_t:
            r[0] = 'E';    break;
        case text_t:
            r[0] = 'T';    break;
        case image_t:
            r[0] = 'I';    break;
    }
    
    /* get hierarchy information */
    h = u_deci_to_hexa(u_get_tree_depth(cad->hierarchy)); 

    strcat(r, h);
    free(h);
    
    /* Be sure produced label is unique, to do that use hash value of new label */
    n = strlen(r);
    l->name = r;

    do {
        if (c > '9' == '9' + 1)
            c = 'A';

        r[n] = c++;

        printf("%s\n\n", r);
        /* calculate the hash-code of new created name */
        l->hash_code = u_create_hash_code(l->name, cad->elist_size); 
    } while (c <= 'Z' && u_check_unique_label(cad, l) == FAIL);

    return r;
}

/* Creates an default unique label  */
Label * c2d_create_label_default (CAD2D * cad, EntityType type) {
    Label * l = (Label *) malloc(sizeof(Label));
 
    if (l != NULL) {
        //* create_label function produce unique label
        l->type = type;
        l->name = u_create_label_name(cad, l);
    } 

    return l;
}

Label * c2d_create_label (CAD2D * cad, EntityType type, char * name) {
    Label * l = (Label *) malloc(sizeof(Label));
    if (l != NULL) {
        l->type = type;
        l->name = name;
        l->hash_code = u_create_hash_code(name, cad->elist_size); 

        if (u_check_unique_label(cad, l) == -1) {
            printf("\"%s\" named Label is already exist\n", name);
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

    for (i = 0; i < cad->elist_size && e == NULL; ++i) {
        if (h >= cad->elist_size)
            h %= cad->elist_size;
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

int u_insert_entity_list (CAD2D * cad, Entity * e) {
    Entity ** tmp;
    int i, n, r = u_check_unique_label(cad, e->label);

    if (r == FAIL) //!!!!! Is it okay?
        printf("\"%s\" named Label is already exist\n", e->label->name);
    else {
        /*  u_check_unique_label() returns the next empty place
            based on it's hash-func regarding the size of the array.
            So be sure we are not exceeding the size                */
        if (r >= cad->elist_size) {
            n = cad->elist_size;

            if (cad->elist_size == 0)
                cad->elist_size = INIT_HASH;
            else
                cad->elist_size *= 2;

            tmp = (Entity **) calloc(cad->elist_size, sizeof(Entity *));

            if (tmp != NULL) {
                for (i = 0; i < n; ++i)
                    tmp[i] = cad->elist[i];
                
                free(cad->elist);
                cad->elist = tmp;
            }
            else
                r = FAIL;
        }

        if (r != FAIL)
            cad->elist[r] = e;
    }
    return r;
}


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

/* Creates an entity with giveb parameters ans save that entity to the lists of CAD */
Entity * u_create_entity (CAD2D * cad, Label * l, void * d, EntityStyle * s) {
    Entity * e = (Entity *) malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = d;
        e->label = l;
        e->style = s;

        u_insert_entity_list(cad, e);
        u_insert_label_list(&cad->llist, e->label); 
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
    Point2D * d = c2d_create_point(x, y);
    Label * l = NULL;

    if (d != NULL && u_is_in_canvas(cad->canvas, d)) {
        if (d != NULL) {
            l = c2d_create_label_default(cad, point_t);
            
            if (l != NULL)
                u_create_entity(cad, l, d, NULL);
            else
                free(d);
        }
    }
    else
        free(d);
    
    return l;
}

//! NOT TESTED YET:
Label * c2d_add_point_p (CAD2D * cad, Point2D p) {
    Point2D * d = (Point2D *) malloc(sizeof(Point2D));
    Label * l = NULL;

    /* Be sure there is only one point */
    p.next = NULL;

    if (u_is_in_canvas(cad->canvas, &p)) {
        if (d != NULL) {
            d->x = p.x;
            d->y = p.y;
            d->next = NULL;

            l = c2d_create_label_default(cad, point_t);
            
            if (l != NULL)
                u_create_entity(cad, l, d, NULL);
            else
                free(d); 
        }
    }
    else
        free(d);
    
    return l;
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
    Point2D * head;
    Label * l = NULL;

    if (u_is_in_canvas(cad->canvas, start) && u_is_in_canvas(cad->canvas, end)) {
        head = c2d_create_point(start->x, start->y);
        head->next = c2d_create_point(end->x, end->y);
        
        l = c2d_create_label_default(cad, line_t);
        
        if (l != NULL)
            u_create_entity(cad, l, head, NULL);
        else
            u_free_point(head);
    }

    return l;
}

//! NOT TESTED YET
Label * c2d_add_polyline (CAD2D * cad, Point2D * p, int size) {
    Point2D * head, * trav;
    Label * l = NULL;
    int i;

    if (u_is_in_canvas(cad->canvas, p)) {
        trav = head = c2d_create_point(p[0].x, p[0].y);
        
        for (i = 1; i < size; ++i) {
            trav->next = c2d_create_point(p[i].x, p[i].y);
            trav = trav->next;
        }
        
        l = c2d_create_label_default(cad, polyline_t);
        
        if (l != NULL)
            u_create_entity(cad, l, head, NULL);
        else
            u_free_point(head);
    }
    
    return l;
}

Label * c2d_add_polygone (CAD2D * cad, Point2D * p, int size) {
    return c2d_add_polyline(cad, p, size);
}

Label * c2d_add_splines(CAD2D * cad, Point2D * p, int size) {
    //! we need 3 point
    // use (P1 P2 P3 curveto) 
}



Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius) {
    Circle * d = (Circle *) malloc(sizeof(Circle));
    Label * l = NULL;
    
    if (d != NULL) {
        //! Check if it's in canvas, we need an another function for checkinh x-y values
        d->center = center;
        d->radius = radius;
        d->start_angle = 0.0;
        d->end_angle = 360.0;

        l = c2d_create_label_default(cad, circle_t);
        
        if (l != NULL)
            u_create_entity(cad, l, d, NULL);
        else
            free(d);
    }
    return l;
}

Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle) {
    Circle * d = (Circle *) malloc(sizeof(Circle));
    Label * l = NULL;

    if (d != NULL) {
        d->center = center;
        d->radius = radius;
        d->start_angle = start_angle;
        d->end_angle = end_angle;
        
        l = c2d_create_label_default(cad, arc_t);
        
        if (l != NULL)
            u_create_entity(cad, l, d, NULL);
        else
            free(d);
    }

    return l;
}

//* Label * c2d_add_rectangle (CAD2D * cad, Point2D center , double width, double hight) {

Label * c2d_add_rectangle (CAD2D * cad, Point2D cornerA , Point2D cornerC) {
    Rectangle * d = (Rectangle *) malloc(sizeof(Rectangle));
    Label * l = NULL;
        
    if (d != NULL) {
        d->cornerA = cornerA;
        d->cornerC = cornerC;
        //! Check if it's in canvas

        l = c2d_create_label_default(cad, rectangle_t);

        if (l != NULL)
            u_create_entity(cad, l, d, NULL);
        else
            free(d);
    }

    return l;
}

Label * c2d_add_text (CAD2D * cad, Point2D * point, char * text, TextStyle * style) {
    Text * d = (Text *) malloc(sizeof(Text));    
    Label * l = NULL;

    //! Check if it's still in canvas
    if (d != NULL && u_is_in_canvas(cad->canvas, point)) {
        d->point.x = point->x;
        d->point.y = point->y;
        d->style = style;
        d->text = (char *) calloc(strlen(text), sizeof(char));

        if (d->text != NULL)
            strcpy(d->text, text);
        //! NOT IMPLEMENTED YET: else
        
        l = c2d_create_label_default(cad, text_t);

        if (l != NULL)
            u_create_entity(cad, l, d, NULL);
        else {
            free(d->text);
            free(d);
        }
    }

    return l;
}

/* 
Label * c2d_add_ellipse(CAD2D * cad, ...) {}
Label * c2d_add_image(CAD2D * cad, ...) {}
*/
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

void u_print_spline (FILE * fid, Point2D * e) {
    int i;
    fprintf(fid, "\nnewpath\n");
    //! At least 3 or 4 point needed 
    fprintf(fid, "%.2f %.2f moveto\n", e->x , e->y);

    for (i = 0; i < 3; ++i) {
        fprintf(fid, "%.2f %.2f ", e->x, e->y);
        e = e->next;
    }    
    fprintf(fid, "curveto\n");
}

void u_print_text (FILE * fid, Text * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->point.x , e->point.y);
    u_print_text_style(fid, e->style);
    fprintf(fid, "(%s) show\n", e->text);
}

 
//! /* Exports all the data by recursively travelling CAD hierarchy */
void u_export_eps (CAD2D * cad, FILE * fid) {
    LabeList * l = cad->llist;
    Entity * e;

    fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
    /* Check if canvas is decleared or not */
    if (cad->canvas != NULL)
        fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);

    // printf("Entity Number: %d\n", cad->llsist_size);
    printf("EXPORT:\n");

    /* export current hierarchy */
    printf("Hierarchy: %s\n", cad->hierarchy->name);
    
    while (l != NULL) {
            e = u_find_entity(cad, l->label);
            
            if (e != NULL) {
                printf("\tc: %s\thash-code: %d\n", e->label->name, e->label->hash_code);
                
                switch (e->label->type) {
                    case point_t:
                        //! NOT IMPLEMENTED YET
                        break;
                    case line_t:
                    case polyline_t:
                    case polygon_t:
                        u_print_line(fid, e->data);
                        break;
                    case spline_t:
                        u_print_spline (fid, e->data);
                        break;
                    case rectangle_t:
                        u_print_rectangle(fid, e->data);
                        break;
                    case circle_t:
                    case arc_t:
                        u_print_circle(fid, e->data);
                        break;
                    case ellipse_t:
                        //! NOT IMPLEMENTED YET
                        break;
                    case text_t:
                        u_print_text(fid, e->data);
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
/* takes ROOT CAD cad, and exports all the content inside it */
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