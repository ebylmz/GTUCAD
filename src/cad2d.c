#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cad2d.h"

#define TRY -2
#define FAIL -1

int u_is_prime (int n);
int u_find_close_prime (int n);

int u_create_hash_code (char * key, int size);
int u_find_hierarchy (Hierarchy * root, Hierarchy * h);
int u_link_hierarchy (Hierarchy * child, Hierarchy * parent);
int u_get_tree_depth (Hierarchy * h);

double u_min (double a, double b);
char * u_deci_to_hexa (int deci);
char * u_create_hierarchy_name (Hierarchy * h); 
Hierarchy * u_get_root_hierarchy (Hierarchy * h);
CAD2D * u_get_root_cad (CAD2D * cad);

void u_insert_label (LabeList ** llist, Label * l);
void u_remove_label (LabeList ** llist, Label ** l);
int u_find_label (CAD2D * cad, Label * l);
char * u_create_label_name (CAD2D * cad, Label * l);

int u_insert_entity_list (CAD2D * cad, Entity * e);
void u_free_point_list (PointList * p);
void u_free_text (Text * t);
void u_free_entity (Entity * e);
Entity * u_create_entity (CAD2D * cad, Label * l, void * d, EntityStyle * s);
int u_is_in_canvas_xy (Canvas * c, double x, double y);
int u_is_in_canvas_p (Canvas * c, Point2D * p);

double u_find_hypotenuse (double x, double y);
double  u_get_euclidean_dist (Point2D * p1, Point2D * p2);
void u_get_center_line (PointList * e, Point2D * c);
void u_get_center_triangle(Triangle * e, Point2D * c);
void u_get_center_rectangle (Rectangle * e, Point2D * c);

double u_measure_point_polyline (Point2D * point, PointList * pline);

void u_snap_point_point (Point2D * s, Point2D * t);
void u_snap_point_line (Point2D * s, PointList * t);
void u_snap_arc_point (Circle * s, Point2D * t);
RGBColor u_convert_rgb (Color c);

void u_print_text_style (FILE * fid, TextStyle * s);
void u_print_entity_style (FILE * fid, Label * l, EntityStyle * s);
void u_print_xy_plane (FILE * fid, Canvas * canvas, EntityStyle * s);
void u_print_circle (FILE * fid, Circle * e);
void u_print_ellipse (FILE * fid, Ellipse * e);
void u_print_triangle (FILE * fid, Triangle * e);
void u_print_rectangle (FILE * fid, Rectangle * e);
void u_print_line (FILE * fid, PointList * e);
void u_print_spline (FILE * fid, PointList * e);
void u_print_regular_polygon (FILE * fid, RegularPolygon * e);
void u_print_text (FILE * fid, Text * e);
void u_export_hierarchy (FILE * fid, Hierarchy * h);
void u_export_eps (CAD2D * cad, FILE * fid);
void u_export_gtucad(CAD2D * cad, FILE * fid);

/*********************************************************************************
 * Hierarchy
*********************************************************************************/

/* creates a hierarchy from given cad */
Hierarchy * c2d_create_hierarchy (CAD2D * cad) {
    Hierarchy * h = (Hierarchy *) malloc(sizeof(Hierarchy));

    if (h != NULL) {
        h->parent = NULL;
        h->cad = cad;
        h->child = NULL;
        h->size = 0;
        cad->hierarchy = h; //???????????????
        h->name = u_create_hierarchy_name(h);
    }

    return h;
}

//! Hierarchy * c2d_create_hierarchy?(CAD2D * cad, …) {}

/* creates a hierarchy from given cad and add it as a child of given parent hierarchy */
Hierarchy * c2d_create_hierarchy_parent (CAD2D * cad, Hierarchy * parent) {
    Hierarchy * h = (Hierarchy *) malloc(sizeof(Hierarchy));
    
    if (h != NULL) {
        h->parent = parent;
        h->cad = cad;
        h->child = NULL;
        h->size = 0;
        cad->hierarchy = h; //???????????????
        h->name = u_create_hierarchy_name(h);
        u_link_hierarchy(h, parent);
    }


    return h;
}

/* Assumes given h is not NULL */
Hierarchy * u_get_root_hierarchy (Hierarchy * h) {
    if (h->parent == NULL)
        return h;
    else
        return u_get_root_hierarchy(h->parent); 
}

CAD2D * u_get_root_cad (CAD2D * cad) {
    Hierarchy * h = u_get_root_hierarchy(cad->hierarchy);
    return h->cad;
}

char * u_create_hierarchy_name (Hierarchy * h) {
    Hierarchy * root = u_get_root_hierarchy(h);
    char c, * r = (char *) calloc(10, sizeof(char));
    int n;

    if (r != NULL) {
        n = u_get_tree_depth(h);

        /* assume tree depth is maxmimum 16 */
        c = n < 9 ? '0' + n : 'A' + n; 
        r[0] = 'h'; r[1] = c;
        h->name = r;
        
        c = '0';
        do {
            if (c == '9' + 1)
                c = 'A';
            r[2] = c++;
       
            if (h->parent != NULL)
                h->hash_code = u_create_hash_code(h->name, h->parent->size);
            else
                h->hash_code = u_create_hash_code(h->name, INIT_HASH);
        } while (c <= 'Z' && u_find_hierarchy(root, h) != FAIL);
    }

    printf("New Hierarchy: %s\n", r);
    return r;    
}


/* In case of matching returns nonnegative index value of the hierarchy, o.w. returns FAIL(-1) */
int u_find_hierarchy (Hierarchy * root, Hierarchy * h) {
    int i, r, n;

    /* First hierarchy is unique since list uninitiliazed yet */
    if (root == NULL || root->size == 0 || h == root) //!! || h == root is needed
        r = FAIL;
    else {
        /* use hash-code as index of the unique hierarchy */
        n = h->hash_code;
        for (i = 0, r = TRY; i < root->size && r == TRY; ++i) {
            if (n >= root->size)
                n %= root->size;

            /* same hash-code is a sign for same keys */
            if (root->child[n] != NULL) {
                if (strcmp(root->child[n]->name, h->name) == 0)
                    r = n;
                else
                    ++n;
            }
            //! USE FLAG FOR DELETED ITEMS: first implement delete hierarchy
            /* Not in this hiearchy */
            else 
                break;
        }
        /* If there isn't match yet, continue with children */
        if (r == TRY) 
            for (i = 0, r = FAIL; i < root->size && r == FAIL; ++i)
                if (root->child[i] != NULL)
                    r = u_find_hierarchy(root->child[i], h);
    }
    printf("for %s - %d: u_find_hierarchy() returns %d\n", h->name, h->hash_code, r);
    return r;
}

/* links given two hiearchy as child-parent relationship */
int u_link_hierarchy (Hierarchy * child, Hierarchy * parent) {
    Hierarchy ** tmp;
    int i = 0, n, r;

    /* Find a proper where in the hash-table */
    if (parent->child != NULL) {
        for (i = 0, r = TRY, n = child->hash_code; i < parent->size && r == TRY; ++i) {
            if (n >= parent->size)
                n %= parent->size;
            
            /* In case of matching, check if they are same */
            if (parent->child[n] != NULL) {
                if (strcmp(parent->child[n]->name, child->name) == 0)
                    r = FAIL;
                else
                    ++n;
            }
            else
                r = n;
        }            
    }
    else
        r = child->hash_code;
    
    /* Locate the child hierarchy to the place indexed by r */
    if (r != FAIL) {
        /* We cannot find place inside of the current hash-table */
        if (r == TRY) 
            r = n;
        
        /* Current size exceeding, we need more memory */
        if (r >= parent->size) {
            n = parent->size;

            parent->size = parent->size == 0 ? INIT_HASH : parent->size * 2;

            tmp = (Hierarchy **) calloc(parent->size, sizeof(Hierarchy *));

            if (tmp != NULL) {
                for (i = 0; i < n; ++i) 
                    tmp[i] = parent->child[i];

                free(parent->child);
                parent->child = tmp;
            }
            else
                r = FAIL;
        }
        
        if (r != FAIL) {
            parent->child[r] = child; 
            child->parent = parent;
            printf("%s inserted as child of %s\n\n\n", child->name, child->parent->name);
        }
    }

    return r;
}

/*********************************************************************************
 * Label
*********************************************************************************/

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

        if (u_find_label(cad, l) != FAIL) {
            printf("\"%s\" named Label is already exist\n", name);
            free(l);
            l = NULL;
        }
    }

    return l;
}

char * u_create_label_name (CAD2D * cad, Label * l) {
    CAD2D * cad_root = u_get_root_cad(cad);
    char c = '0', * r = calloc(10, sizeof(char)), * h;
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
        case irregular_polygon_t:
            r[0] = 'I';
            r[1] = 'P';
            r[2] = 'g';    break;
        case regular_polygon_t:
            r[0] = 'R';
            r[1] = 'P';
            r[2] = 'g';    break;
        case triangle_t:
            r[0] = 'T';
            r[1] = 'r';    break;
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

    /* define the instance number from 1 to 9 and A to Z */
    /* check if given */
    do {
        if (c == '9' + 1)
            c = 'A';
        r[n] = c++;
        /* calculate the hash-code of new created name */
        l->hash_code = u_create_hash_code(l->name, cad->elist_size); 
    } while (c <= 'Z' && u_find_label(cad_root, l) != FAIL);

    return r;
}

/* In case of matching returns nonnegative index value of the label, o.w. returns FAIL(-1) */
int u_find_label (CAD2D * cad, Label * l) {
    int i, r, n;

    if (cad->elist_size == 0)  
        r = FAIL;
    else {
        /* Search in current hierarchy */
        n = l->hash_code;
        for (i = 0, r = TRY; i < cad->elist_size && r == TRY; ++i) {
            if (n >= cad->elist_size)
                n %= cad->elist_size;
            
            /* same hash-code is a sign for same keys */
            if (cad->elist[n] != NULL) {
                if (strcmp(cad->elist[n]->label->name, l->name) == 0)
                    r = n;
                else
                    ++n;
            }
            //! USE FLAG FOR DELETED ITEMS: first implement delete hierarchy
            /* Not in this elist */
            else
                r = FAIL;
        }
    }

    /* If label haven't find yet, continue with child hierarchies */
    if (r == FAIL) {
        for (i = 0; i < cad->hierarchy->size && r == FAIL; ++i)
            if (cad->hierarchy->child[i] != NULL) {
printf("Search %s in hierarchy %s\n", l->name, cad->hierarchy->name);
                r = u_find_label(cad->hierarchy->child[i]->cad, l);
            }
    }

    if (r == FAIL && cad->hierarchy != NULL)
        printf("NOT FIND (%s, %2d) in %s\n", l->name, l->hash_code, cad->hierarchy->name);
    else
        printf("FIND (%s, %2d) in %s\n", l->name, l->hash_code, cad->hierarchy->name);
    return r;
}

void u_insert_label (LabeList ** llist, Label * l) {
    while (*llist != NULL) llist = &(*llist)->next;

    *llist = (LabeList *) malloc(sizeof(LabeList));

    if (*llist != NULL) {
        (*llist)->label = l;
        (*llist)->next = NULL;
    }
}

//! NOT TESTED YET
void u_remove_label (LabeList ** llist, Label ** l) {
    while (*llist != NULL && *l != NULL) {
        if ((*llist)->label == *l) {
            *llist = (*llist)->next;
            free((*l)->name);
            free(*l);
            *l = NULL;
        }
        else
            llist = &(*llist)->next;
    }
}

/*********************************************************************************
 * Entity
*********************************************************************************/

/* Creates an entity with giveb parameters ans save that entity to the lists of CAD */
Entity * u_create_entity (CAD2D * cad, Label * l, void * d, EntityStyle * s) {
    Entity * e = (Entity *) malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = d;
        e->label = l;
        e->style = s;

        u_insert_entity_list(cad, e);
        u_insert_label(&cad->llist, e->label); 
    }
    
    return e;
}

/* Returns the entity by given it's label */
Entity * c2d_find_entity (CAD2D * cad, Label * l) {
    Entity * e = NULL, * tmp;    
    int i, h = l->hash_code;

    for (i = 0; i < cad->elist_size && e == NULL; ++i) {
        if (h >= cad->elist_size)
            h %= cad->elist_size;
        tmp = cad->elist[h + i];
        if (tmp != NULL && strcmp(l->name, tmp->label->name) == 0)
            e = tmp;
    } 
    //! NOT IMPLEMENTED YET: Also check other hierarchies 
    return e;
}

//! NOT TESTED YET
void c2d_remove_entity (CAD2D * cad, Label ** l) {
    int r = u_find_label(cad, *l);

    //! assume we are still in same hierarchy
    //! r r is always -1 becasue it's there
    if (r != FAIL) {
        /* free allocated memory, and delete it's record from lists */
        u_free_entity(cad->elist[r]);
        cad->elist[r] = NULL;
        u_remove_label(&cad->llist, l);
    }    
}

int u_insert_entity_list (CAD2D * cad, Entity * e) {
    Entity ** tmp;
    int i, n, r;

    /* Find proper where for given entity */
    for (i = 0, r = TRY, n = e->label->hash_code; i < cad->elist_size && r == TRY; ++i) {
        if (n >= cad->elist_size)
            n %= cad->elist_size;
        
        if (cad->elist[n] != NULL) {
            if (strcmp(cad->elist[n]->label->name, e->label->name) == 0)
                r = FAIL;
            else
                ++n;
        }
        else        /* We find an empty place */
            r = n; 
    }

    if (r != FAIL) {
        /* We cannot find place inside of the current hash-table */
        if (r == TRY)
            r = n;
        
        /* Be sure we are not exceeding current size of hash-table */
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

        /* Assign it */
        if (r != FAIL) {
            cad->elist[r] = e;
            printf("(->) INSERT (%s %-2d) to %s\n\n", e->label->name, e->label->hash_code, cad->hierarchy->name);
        }
    }

    if (r == FAIL)
        printf("(-) INSERT FAIL (%s %-2d) to %s\n\n", e->label->name, e->label->hash_code, cad->hierarchy->name);


    return r;
}

/* common for point and lines */
void u_free_point_list (PointList * p) {
    PointList * tmp;

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
                    case line_t:
                    case polyline_t:
                    case irregular_polygon_t:
                        u_free_point_list(e->data);
                        break;
                    case point_t:
                    case regular_polygon_t:
                    case triangle_t:
                    case rectangle_t:
                    case circle_t:
                    case arc_t:
                    case ellipse_t:
                        free(e->data);
                        break;
                    case text_t:
                        u_free_text(e->data);       
                        break;
                    case spline_t:
                        //! NOT IMPLEMENTED YET
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
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));

    if (cad != NULL) {
        cad->elist = NULL;
        cad->elist_size = 0;
        cad->llist = NULL;
        cad->canvas = (Canvas *) malloc(sizeof(Canvas));
        
        if (cad->canvas != NULL) {
            cad->canvas->start.x = -width / 2;
            cad->canvas->start.y = -height / 2;
            cad->canvas->end.x = width / 2; 
            cad->canvas->end.y = height / 2; 
            cad->hierarchy = c2d_create_hierarchy_parent(cad, h);

            if (cad->hierarchy == NULL) {
                free(cad->canvas);
                free(cad);
                cad = NULL;
            }
        }
        else {
            free(cad);
            cad = NULL;
        }
    }
    return cad;
}


/*********************************************************************************
 * Adding Basic CAD Entities
*********************************************************************************/
/*
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h, const Label * l) {}
*/

//! NOT TESTED YET:
Label * c2d_add_point_xy (CAD2D * cad, double x, double y) {
    Point2D * d;
    Label * l = NULL;

    if (u_is_in_canvas_xy(cad->canvas, x, y)) {
        d = c2d_create_point(x, y);
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
    Point2D * d;
    Label * l = NULL;

    if (u_is_in_canvas_p(cad->canvas, &p)) {
        d = (Point2D *) malloc(sizeof(Point2D));
        if (d != NULL) {
            *d = p; 

            l = c2d_create_label_default(cad, point_t);
            if (l != NULL)
                u_create_entity(cad, l, d, NULL);
            else
                free(d); 
        }
    }
    
    return l;
}

//! NOT TESTED YET:
Label * c2d_add_point_ph (CAD2D * cad, Point2D p, const Hierarchy * h) {
    int r = u_find_hierarchy(u_get_root_hierarchy(h), h);

    /* add point at desired hiearchy as if it's root */
    return r != FAIL ? c2d_add_point_p(h->cad, p) : NULL;
}

Label * c2d_add_point_phl (CAD2D * cad, Point2D p, const Hierarchy * h, const Label * l) {
    Point2D * d;
    //! assumes given label is unique
    if (l != NULL && u_is_in_canvas_p(cad->canvas, &p)) {
        d = (Point2D *) malloc(sizeof(Point2D));
        if (d != NULL) {
            *d = p; 
            
            u_create_entity(cad, l, d, NULL);
        }
    }
    
    return l;
}

void c2d_set_point (Point2D * p, double x, double y) {
    p->x = x;
    p->y = y;
}

PointList * c2d_create_point_list_p (Point2D p) {
    PointList * l = (PointList *) malloc(sizeof(PointList));
    if (l != NULL) {
        l->point = p;
        l->next = NULL;
    }

    return l;
}

Point2D * c2d_create_point (double x, double y) {
    Point2D * p = (Point2D *) malloc(sizeof(Point2D));
    if (p != NULL) {
        p->x = x;
        p->y = y;
    }

    return p;
}

Label * c2d_add_line (CAD2D * cad, Point2D start, Point2D end) {
    PointList * head;
    Label * l = NULL;

    if (u_is_in_canvas_p(cad->canvas, &start) && u_is_in_canvas_p(cad->canvas, &end)) {
        head = c2d_create_point_list_p(start);
        head->next = c2d_create_point_list_p(end);
        
        l = c2d_create_label_default(cad, line_t);
        
        if (l != NULL)
            u_create_entity(cad, l, head, NULL);
        else
            u_free_point_list(head);
    }

    return l;
}

Label * c2d_add_line_h (CAD2D * cad, Point2D start, Point2D end, Hierarchy * h) {
    //! NOT IMPLEMENTED YET
}

Label * c2d_add_line_hl (CAD2D * cad, Point2D start, Point2D end, Hierarchy * h, Label * l) {
    //! NOT IMPLEMENTED YET
}

/* takes an point array */
Label * c2d_add_polyline (CAD2D * cad, Point2D * p, int size) {
    PointList * head, ** trav;
    Label * l = NULL;
    int i;

    trav = &head;

    for (i = 0; i < size; ++i) {
        if (u_is_in_canvas_p(cad->canvas, p + i)) {
            *trav = c2d_create_point_list_p(p[i]);
            if (*trav != NULL)
                trav = &(*trav)->next;
            else {
                u_free_point_list(head);
                head = NULL;
            }
        }
    }
    
    if (head != NULL) {
        l = c2d_create_label_default(cad, polyline_t);
        
        if (l != NULL)
            u_create_entity(cad, l, head, NULL);
        else
            u_free_point_list(head);
    }
    
    return l;
}

Label * c2d_add_polyline_h (CAD2D * cad, Point2D * p, int size, Hierarchy * h) {
    //! NOT IMPLEMENTED YET
}

Label * c2d_add_polyline_hl (CAD2D * cad, Point2D * p, int size, Hierarchy * h, Label * l) {
    //! NOT IMPLEMENTED YET
}

Label * c2d_add_irregular_polygon (CAD2D * cad, Point2D * p, int size) {
    return c2d_add_polyline(cad, p, size);
}

Label * c2d_add_irregular_polygon_h (CAD2D * cad, Point2D * p, int size, Hierarchy * h) {
    //! NOT IMPLEMENTED YET
}

Label * c2d_add_irregular_polygon_hl (CAD2D * cad, Point2D * p, int size, Hierarchy * h, Label * l) {
    //! NOT IMPLEMENTED YET
}

Label * c2d_add_regular_polygon (CAD2D * cad, int n, Point2D center, double out_radius) {
    RegularPolygon * d; 
    Label * l = NULL;

    if (u_is_in_canvas_xy(cad->canvas, center.x + out_radius, center.y + out_radius) &&
        u_is_in_canvas_xy(cad->canvas, center.x + out_radius, center.y + out_radius)) {
        d = (RegularPolygon *) malloc(sizeof(RegularPolygon));
        if (d != NULL) {
            d->center = center;
            d->n = n;
            d->out_radius = out_radius;

            l = c2d_create_label_default(cad, regular_polygon_t);
                
            if (l != NULL)
                u_create_entity(cad, l, d, NULL);
            else
                free(d);
        }
    }

    return l;
}


Label * c2d_add_splines(CAD2D * cad, Point2D * p, int size) {
    //! we need 3 point
    // use (P1 P2 P3 curveto) 
}

Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius) {
    Circle * d;
    Label * l = NULL;

    if (u_is_in_canvas_xy(cad->canvas, center.x + radius, center.y + radius) &&
        u_is_in_canvas_xy(cad->canvas, center.x - radius, center.y - radius)) {
        d = (Circle *) malloc(sizeof(Circle));
        if (d != NULL) {
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
    }

    return l;
}

Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle) {
    Circle * d;
    Label * l = NULL;

    //! CHECK THE CANVAS 
    if (u_is_in_canvas_xy(cad->canvas, center.x + radius, center.y + radius) &&
        u_is_in_canvas_xy(cad->canvas, center.x - radius, center.y - radius)) {

        d = (Circle *) malloc(sizeof(Circle));
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
    }
    return l;
}

Label * c2d_add_ellipse(CAD2D * cad, Point2D center, double radius_x, double radius_y) {
    Ellipse * d;
    Label * l;

    if (u_is_in_canvas_xy(cad->canvas, center.x + radius_x, center.y + radius_y) &&
        u_is_in_canvas_xy(cad->canvas, center.x - radius_x, center.y - radius_y)) {
        d = (Ellipse *) malloc(sizeof(Ellipse));
        if (d != NULL) {
            d->center = center;
            d->radius_x = radius_x;
            d->radius_y = radius_y;

            l = c2d_create_label_default(cad, ellipse_t);
            
            if (l != NULL)
                u_create_entity(cad, l, d, NULL);
            else
                free(d);
        }
    }

    return l;
}

//! ADD THIN TRIANGLE TYPE TO SWITCH CASES
Label * c2d_add_triangle (CAD2D * cad, Point2D corner1, Point2D corner2, Point2D corner3) {
    Triangle * d;
    Label * l = NULL;

    if (u_is_in_canvas_p(cad->canvas, &corner1) && 
        u_is_in_canvas_p(cad->canvas, &corner2) &&
        u_is_in_canvas_p(cad->canvas, &corner3)) {
        d = (Triangle *) malloc(sizeof(Triangle));
        if (d != NULL) {
            d->corner1 = corner1;
            d->corner2 = corner2;
            d->corner3 = corner3;

            l = c2d_create_label_default(cad, triangle_t);
            if (l != NULL) 
                u_create_entity(cad, l, d, NULL);
            else
                free(d);
        }
    }

    return l;
}

Label * c2d_add_rectangle (CAD2D * cad, Point2D corner1 , Point2D corner2) {
    Rectangle * d;
    Label * l = NULL;
        
    if (u_is_in_canvas_p(cad->canvas, &corner1) && u_is_in_canvas_p(cad->canvas, &corner1)) {
        d = (Rectangle *) malloc(sizeof(Rectangle));
        if (d != NULL) {
            d->corner1 = corner1;
            d->corner2 = corner2;
            l = c2d_create_label_default(cad, rectangle_t);

            if (l != NULL)
                u_create_entity(cad, l, d, NULL);
            else
                free(d);
        }
    }

    return l;
}

Label * c2d_add_text (CAD2D * cad, Point2D p, char * text) {
    Text * d;    
    Label * l = NULL;
/*
    multiply these number with strlen(text) then add to start point to check ...
    fs_xsmall   0.16
    fs_small    0.08
    fs_medium   0.04
    fs_large    0.02
*/
    //! CHECK THE CANVAS
    if (u_is_in_canvas_xy(cad->canvas, p.x, p.y)) {
        d = (Text *) malloc(sizeof(Text));
        if (d != NULL) {
            d->point.x = p.x;
            d->point.y = p.y;
            d->text = (char *) calloc(strlen(text), sizeof(char));

            if (d->text != NULL) {
                strcpy(d->text, text);
                l = c2d_create_label_default(cad, text_t);

                if (l != NULL)
                    u_create_entity(cad, l, d, NULL);
                else
                    u_free_text(d);
           }       
        }
    }

    return l;
}

//! Label * c2d_add_image(CAD2D * cad, ...) {}

int u_is_in_canvas_xy (Canvas * c, double x, double y) {
    return c == NULL ? 1 :
        (x >= c->start.x && y >= c->start.y && x <= c->end.x && y <= c->end.y);
}

int u_is_in_canvas_p (Canvas * c, Point2D * p) {
    return c == NULL ? 1 :
        (p->x >= c->start.x && p->y >= c->start.y && p->x <= c->end.x && p->y <= c->end.y);
}

/*********************************************************************************
 * Measurement Between Entity
*********************************************************************************/

/* measures the distance between two entity given by their labels */
double c2d_measure (CAD2D * cad, Label * ls, Label * lt) {
    Entity * e1 = c2d_find_entity(cad, ls), * e2 = c2d_find_entity(cad, lt);
    double m;

    /* check if given label's are exist */
    if (e1 != NULL && e2 != NULL) {
        //! NOT IMPLEMENTED YET KEEP THE DECISION STRUCTURE
    }    

    return m;
}

/* Returns the closest distance between point and polyline */
double u_measure_point_polyline (Point2D * point, PointList * pline) {
    double d = -1, tmp;

    /* Take min distance by travelling each point on pline and calculating their ED  */
    while (pline != NULL) {
        tmp = u_get_euclidean_dist(point, &pline->point);

        if (tmp < d || d == -1)
            d = tmp;
        pline = pline->next;
    }

    return d;
}

double u_measure_point_arc (Point2D * p, Circle * c) {
    //! NOT IMPLEMENTED YET
}

/* Measures the distance between center of given two entity */
double c2d_measure_center2D (CAD2D * cad, Label * ls, Label * lt) {
    Entity * s = c2d_find_entity(cad, ls), * t = c2d_find_entity(cad, lt);
    Point2D * c1, * c2;
    double m;

    /* check if given label's are exist */
    if (s != NULL && t != NULL) {
        /* take center points of these two entity and calculate the distance */
        c1 = c2d_get_center2D(s->data);
        c2 = c2d_get_center2D(t->data);

        m = u_get_euclidean_dist(c1, c2);
        free(c1);
        free(c2);
    }    
    else
        m = -1;

    return m;
}

/* creates an point in center of given entity //! LABEL ? */
Point2D * c2d_get_center2D (Entity * e) {
    Point2D * trav, * c = NULL;

    if (e != NULL) {
        c = (Point2D *) malloc(sizeof(Point2D));
        if (c != NULL) {
            switch (e->label->type) {
                case point_t:
                    *c = *((Point2D *) e->data);
                    break;
                case line_t:
                case polyline_t:
                case irregular_polygon_t:
                case regular_polygon_t:
                    u_get_center_line(e->data, c);
                    break;
                case spline_t:
                    //! NOT IMPLEMENTED YET
                    break;
                case triangle_t:
                    u_get_center_triangle(e->data, c);
                    break;
                case rectangle_t:
                    u_get_center_rectangle(e->data, c);
                    break;
                case circle_t:
                case arc_t:
                    *c = ((Circle *) e->data)->center;
                    break;
                case ellipse_t:
                    *c = ((Ellipse *) e->data)->center;
                    break;
                case image_t:
                    //! NOT IMPLEMENTED YET
                    break;
                case text_t:
                    //! NOT IMPLEMENTED YET
                    break;
                default:
                    printf("Given entity is not a 2D shape\n");
                    free(c);
            }
        }
    }

    return c;
}

/* c: center e: entity */
void u_get_center_line (PointList * e, Point2D * c) {
    int i = 0;
    c->y = c->x = 0;

    while (e != NULL) {
        c->x += e->point.x;
        c->y += e->point.y;

        e = e->next;
        ++i;
    }

    if (i > 1) {
        c->x /= i;
        c->y /= i;
    }
}

void u_get_center_triangle(Triangle * e, Point2D * c) {
    c->x = (e->corner1.x + e->corner2.x + e->corner3.x) / 3;
    c->x = (e->corner1.y + e->corner2.y + e->corner3.y) / 3;
}

void u_get_center_rectangle (Rectangle * e, Point2D * c) {
    c->x = (e->corner1.x + e->corner2.x) / 2;
    c->y = (e->corner1.y + e->corner2.y) / 2;
}

/*********************************************************************************
 * Snapping Labels
*********************************************************************************/  

/*
Polyline    Point       Define a strategy!
Polyline    Polyline    Define a strategy!
Polyline    Polygon     Define a strategy!
Polygon     Point       set polgon as it's center is given target point
Polygon     Polyline    Define a strategy!
// Arc         Point       set arc as it's center is given target point
Arc         Polyline    Define a strategy
*/

void c2d_snap (CAD2D * cad, const Label * ls, const Label * lt) {
    Entity * s = c2d_find_entity(cad, ls), * t = c2d_find_entity(cad, lt);
    Point2D * c1, * c2;

    if (s != NULL && t != NULL) {
        switch (ls->type) {
            case point_t:
                switch (lt->type) {
                    case point_t:
                        u_snap_point_point(s->data, t->data);
                        break;
                    case line_t:
                    case spline_t:
                    case polyline_t:
                        u_snap_point_line(s->data, t->data);
                        //! NOT IMPLEMENTED YET
                        break;
                    case irregular_polygon_t:
                    case regular_polygon_t:
                        u_get_center_line(t->data, s->data);
                        break;
                    case rectangle_t:
                        u_get_center_rectangle(t->data, s->data);
                        break;
                    case circle_t:
                    case arc_t:
                        *((Point2D *) s->data) = ((Circle *) t->data)->center;
                        break;
                    case ellipse_t:
                        *((Point2D *) s->data) = ((Ellipse *) t->data)->center;
                        break;
                    case text_t:
                        //! NOT IMPLEMENTED YET
                        break;
                }
                break;
            case line_t:
                //! Define a strategy!
                break;
            case spline_t:
                //! Define a strategy!
                break;
            case polyline_t:
                //! Define a strategy!
                break;
            case irregular_polygon_t:
                //! Define a strategy!
                break;
            case rectangle_t:
                //! Define a strategy!
                break;
            case circle_t:
                //! Define a strategy!
                break;
            case arc_t:
                //! Define a strategy!
                break;
            case ellipse_t:
                //! Define a strategy!
                break;
            case text_t:
                //! Define a strategy!
                break;
        }
    } 
}

/* Snaps the source point to the target point */
void u_snap_point_point (Point2D * s, Point2D * t) {
    s->x = t->x;
    s->y = t->y;
    //! s->next; 
}   

/* Snaps the source point to the closest end of the line */
void u_snap_point_line (Point2D * s, PointList * t) {
    //! NOT IMPLEMENTED YET
}

/* Snaps the arc to the given point as center of arc */
void u_snap_arc_point (Circle * s, Point2D * t) {
    s->center = *t;
}

/*********************************************************************************
 * Style
*********************************************************************************/

/* set's the style of entity by given it's label */
EntityStyle * c2d_set_entity_style (CAD2D * cad, Label * label, LineStyle l, RGBColor c, DrawStyle d, double w) {
    EntityStyle * style = NULL;
    Entity * e = c2d_find_entity(cad, label); 
    
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

TextStyle * c2d_set_text_style (CAD2D * cad, Label * label, FontStyle f, RGBColor c, double s) {
    TextStyle * style;
    Entity * e = c2d_find_entity(cad, label);
    Text * d;
    
    if (e != NULL) {
        style = (TextStyle *) malloc(sizeof(TextStyle));

        if (style != NULL) {
            style->color = c;
            style->font = f;
            style->scale = s;
            
            d = (Text *) e->data;
            d->style = style;
        }
    }

    return style;
}

void c2d_set_rgb (RGBColor * c, double red, double green, double blue) {
    c->red = red;
    c->green = green;
    c->blue = blue;
}

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

/*********************************************************************************
 * Import & Export
*********************************************************************************/

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

/* Exports all the CAD data by travelling recursively CAD hierarchies */
void u_export_eps (CAD2D * cad, FILE * fid) {
    fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
    
    printf("EXPORT:\n");
    /* Check if canvas is decleared or not */
    if (cad->canvas != NULL)
        fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas->start.x, cad->canvas->start.y, cad->canvas->end.x, cad->canvas->end.y);
    u_export_hierarchy(fid, cad->hierarchy);
    fprintf(fid, "\nshowpage\n");
}

void u_export_gtucad(CAD2D * cad, FILE * fid) {
    //! NOT IMPLEMENTED YET
}

//! NOT TESTED YET
void u_export_hierarchy (FILE * fid, Hierarchy * h) {
    int i;
    CAD2D * cad; 
    Entity * e;
    LabeList * l; 

    if (h != NULL) {
        cad = h->cad;
        l = cad->llist;
        
        printf("Hierarchy: %s\n", cad->hierarchy->name);
        
        /* export current cad */
        while (l != NULL) {
                e = c2d_find_entity(cad, l->label);
                
                if (e != NULL) {
                    printf("%s\t%d\n", e->label->name, e->label->hash_code);
                    
                    switch (e->label->type) {
                        case point_t:
                            //! NOT IMPLEMENTED YET
                            break;
                        case line_t:
                        case polyline_t:
                        case irregular_polygon_t:
                            u_print_line(fid, e->data);
                            break;
                        case regular_polygon_t:
                            u_print_regular_polygon(fid, e->data);
                            break;
                        case rectangle_t:
                            u_print_rectangle(fid, e->data);
                            break;
                        case triangle_t:
                            u_print_triangle (fid, e->data);
                            break;
                        case circle_t:
                        case arc_t:
                            u_print_circle(fid, e->data);
                            break;
                        case ellipse_t:
                            u_print_ellipse(fid, e->data);
                            break;
                        case text_t:
                            u_print_text(fid, e->data);
                            break;
                        case spline_t:
                            u_print_spline (fid, e->data);
                            //! NOT IMPLEMENTED YET
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
        /* export child of the root hierarchy */
        printf("hsize: %d\n", h->size);
        for (i = 0; i < h->size; ++i)
            if (h->child[i] != NULL)
                u_export_hierarchy(fid, h->child[i]);
    }
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
    fprintf(fid, s->draw == stroke && l->type != irregular_polygon_t ? "stroke\n" : "fill\n");
}

void c2d_show_xy_plane () {
    //! NOT IMPLEMENTED YET
}

void u_print_xy_plane (FILE * fid, Canvas * canvas, EntityStyle * s) {
    fprintf(fid, "\n%% X-Y Plane\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", canvas->start.x, 0.0);
    fprintf(fid, "%.2f %.2f lineto\n", canvas->end.x, 0.0);
    fprintf(fid, "%.2f %.2f moveto\n", 0.0, canvas->start.y);
    fprintf(fid, "%.2f %.2f lineto\n", 0.0, canvas->end.y);
}

void u_print_circle (FILE * fid, Circle * e) {
    fprintf(fid, "\n%% Circle\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n", e->center.x, e->center.y, e->radius, e->start_angle, e->end_angle);
}

void u_print_ellipse (FILE * fid, Ellipse * e) {
    double  scale = e->radius_y / e->radius_x,
            radius = u_min(e->radius_x, e->radius_y);   

    fprintf(fid, "\n%% Ellipse\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "1 %.2f scale\n", scale);
    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n", e->center.x, e->center.y, radius, 0.0, 360.0);
}

void u_print_triangle (FILE * fid, Triangle * e) {
    fprintf(fid, "\n%% Triangle\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->corner1.x, e->corner1.y);
    fprintf(fid, "%.2f %.2f lineto\n", e->corner2.x, e->corner2.y);
    fprintf(fid, "%.2f %.2f lineto\n", e->corner3.x, e->corner3.y);
    fprintf(fid, "closepath\n");
}

void u_print_rectangle (FILE * fid, Rectangle * e) {
    double  height = (e->corner2.y - e->corner1.y),
            width = (e->corner2.x - e->corner1.x);

    fprintf(fid, "\n%% Rectangle\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->corner1.x, e->corner1.y);
    fprintf(fid, "%.2f %.2f rlineto\n", 0.0, height);
    fprintf(fid, "%.2f %.2f rlineto\n", width, 0.0);
    fprintf(fid, "%.2f %.2f rlineto\n", 0.0, -height);
    fprintf(fid, "closepath\n");
} 

/* common function for line, polyline and polygon types */
void u_print_line (FILE * fid, PointList * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->point.x , e->point.y);
    e = e->next;
    
    while (e != NULL) {
        fprintf(fid, "%.2f %.2f lineto\n", e->point.x, e->point.y);
        e = e->next;
    }
}

void u_print_spline (FILE * fid, PointList * e) {
    int i;
    fprintf(fid, "\n%% Spline\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->point.x , e->point.y);
    //! NOT IMPLEMENTED YET
    //! At least 3 or 4 point needed 

    for (i = 0; i < 3; ++i) {
        fprintf(fid, "%.2f %.2f ", e->point.x, e->point.y);
        e = e->next;
    }    
    fprintf(fid, "curveto\n");
}

void u_print_regular_polygon (FILE * fid, RegularPolygon * e) {
    double angle, d = 360 / e->n;

    /*  (cos(angle) * radius) defines the x-distance from center then 
        add to it center point to establish the x-component of target point */

    fprintf(fid, "\n%% Regular Polygon\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f cos %.2f mul %.2f add %.2f sin %.2f mul %.2f add moveto\n", angle, e->out_radius, e->center.x, angle, e->out_radius, e->center.y);
    
    for (angle = d; angle < 360; angle += d)
    fprintf(fid, "%.2f cos %.2f mul %.2f add %.2f sin %.2f mul %.2f add lineto\n", angle, e->out_radius, e->center.x, angle, e->out_radius, e->center.y);
    fprintf(fid, "closepath\n");
}

void u_print_text (FILE * fid, Text * e) {
    fprintf(fid, "\n%% Text\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->point.x , e->point.y);
    u_print_text_style(fid, e->style);
    fprintf(fid, "(%s) show\n", e->text);
}

/*********************************************************************************
 * Memory Deletion
*********************************************************************************/

void c2d_delete (CAD2D * cad) {
    //! NOT IMPLEMENTED YET
}

/*********************************************************************************
 * Other
*********************************************************************************/

/* Produce a hash-code according to given key value */
int u_create_hash_code (char * key, int size) {
    int i;
    int code = 0;
    int p, q = 97;  //! q

    /*  set the p as largest prime smaller than size.
        in case of uninitialized hash table use it's INIT size */
    if (size == 0)
        size = INIT_HASH;
    p = u_find_close_prime(size);
    
    for (i = 0; key[i] != '\0'; ++i)
        code = (code * q + key[i]) % p;
    return code;
}

int u_is_prime (int n) {
    int i, r, b = sqrt(n);

    if (n < 2)
    	r = 0;
    else 
	    for (i = 2, r = 1; i <= b && r == 1; ++i)
	        if (n % i == 0)
	            r = 0;
    return r;
}

/* Returs largest prime number which is smaller than n */
int u_find_close_prime (int n) {
    while (--n > 1)
        if (u_is_prime(n))
            break;
    return n;
}

double u_min (double a, double b) {
    return a < b ? a : b;
}

int u_get_tree_depth (Hierarchy * h) {
    int r;
    
    if (h->parent == NULL)
        r = 0;
    else
        r = 1 + u_get_tree_depth(h->parent);
    return r;
}

//! DELETE 
char u_get_hierarchy_level (Hierarchy * h) {
    int d = u_get_tree_depth(h);
    char l;
    if (d < 16)
        l = d <= 9 ? '0' + d : 'A' + d;
    else {

        printf("Max hierarchy level reached\n");
    }
    
    return l;
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

double u_find_hypotenuse (double x, double y) {
    return sqrt(x*x + y*y);
} 

double  u_get_euclidean_dist (Point2D * p1, Point2D * p2) {
    return u_find_hypotenuse(p1->x - p2->x, p1->y - p2->y);
}