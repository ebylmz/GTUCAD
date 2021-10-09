#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cad2d.h"

int u_is_prime (int n);
int u_find_close_prime (int n);
char * u_read_bin_str (FILE * fid);

int u_create_hash_code (char * key, int size);
int u_is_empty (void * p);
int u_find_hierarchy (Hierarchy * root, Hierarchy * h);
int u_link_hierarchy (Hierarchy * child, Hierarchy * parent);
int u_get_tree_depth (Hierarchy * h);

double u_min (double a, double b);
char * u_deci_to_hexa (int deci);
char * u_create_hierarchy_name (Hierarchy * h); 

void u_insert_label (LabeList ** llist, Label * l);
void u_remove_label (LabeList ** llist, Label ** l);
int u_find_label (CAD2D * cad, Label * l);
char * u_create_label_name (CAD2D * cad, Label * l);

int u_insert_entity_list (CAD2D * cad, Entity * e);
void u_free_point_list (PointList * p);
void u_free_text (Text * t);
Entity * u_create_entity (CAD2D * cad, Label * l, void * d, EntityStyle * s);

double u_find_hypotenuse (double x, double y);
double  u_get_euclidean_dist (Point2D * p1, Point2D * p2);
Point2D u_get_center_rectangle (Rectangle * e);
Point2D u_get_center_rectangle (Rectangle * e); 
Point2D u_get_center_triangle(Triangle * e);
Point2D u_get_center_line (PointList * e);

Canvas * u_max_canvas (Canvas * c1, Canvas * c2);
Canvas * u_min_canvas (Canvas * c1, Canvas * c2);
int u_check_canvas_p (CAD2D * cad, Point2D * p);
int u_check_canvas_xy (CAD2D * cad, double x, double y);

double u_measure_point_polyline (Point2D * point, PointList * pline);

void u_snap_point_point (Point2D * s, Point2D * t);
void u_snap_point_line (Point2D * s, PointList * t);
void u_snap_arc_point (Circle * s, Point2D * t);

void u_export_eps (FILE * fid, CAD2D * cad);
void u_export_eps_text_style (FILE * fid, TextStyle * s);
void u_export_eps_entity_style (FILE * fid, Label * l, EntityStyle * s);
void u_export_eps_entity_style_reset (FILE * fid);
void u_export_eps_xy_plane (FILE * fid, Canvas * canvas, EntityStyle * s);
void u_export_eps_point(FILE * fid, Point2D * e);
void u_export_eps_circle (FILE * fid, Circle * e);
void u_export_eps_ellipse (FILE * fid, Ellipse * e);
void u_export_eps_triangle (FILE * fid, Triangle * e);
void u_export_eps_rectangle (FILE * fid, Rectangle * e);
void u_export_eps_line (FILE * fid, PointList * e);
void u_export_eps_regular_polygon (FILE * fid, RegularPolygon * e);
void u_export_eps_text (FILE * fid, Text * e);
void u_export_gtucad (FILE * fid, CAD2D * cad);
void u_export_gtucad_elist (FILE * fid, Entity ** elist, int elist_size);
void u_export_gtucad_llist (FILE * fid, LabeList * llist);
void u_export_gtucad_hierarchy (FILE * fid, Hierarchy * h);

CAD2D * u_import_gtucad (FILE * fid);
Entity ** u_import_gtucad_elist (FILE * fid, Entity ** elist, int elist_size);
LabeList * u_import_gtucad_llist (FILE * fid, LabeList * llist, Entity ** elist, int elist_size);
Hierarchy * u_import_gtucad_hierarchy (FILE * fid, CAD2D * cad);

Label * u_find_label_elist (Entity ** elist, int elist_size, Label * target_l);

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
        cad->hierarchy = h; 
        h->name = u_create_hierarchy_name(h);
    }

    return h;
}

/* creates a hierarchy from given cad and add it as a child of given parent hierarchy */
Hierarchy * c2d_create_hierarchy_parent (CAD2D * cad, Hierarchy * parent) {
    Hierarchy * h = (Hierarchy *) malloc(sizeof(Hierarchy));
    CAD2D * cad_root;

    if (h != NULL) {
        h->parent = parent;
        h->cad = cad;
        h->child = NULL;
        h->size = 0;
        cad->hierarchy = h; 
        h->name = u_create_hierarchy_name(h);
        u_link_hierarchy(h, parent);
        /*  If auto-canvas is on, update the root size as max-canvas
            o.w. change the canvas of child-cad as min-canvas        */
        cad_root = c2d_get_root_cad(cad);
        if (cad_root != NULL) {
            if (1 == cad_root->auto_canvas)
                cad_root->canvas = *u_max_canvas(&cad_root->canvas, &cad->canvas);
            else
                cad->canvas = *u_min_canvas(&cad_root->canvas, &cad->canvas);
        }
    }

    return h;
}

void c2d_delete_hierarchy (Hierarchy * h) {
    Hierarchy * parent, * child;

    int i, n, r;

    /* Remove it from it's parent */
    if (h->parent != NULL) {
        parent = h->parent;
        for (i = 0, n = h->hash_code, r = TRY; i < parent->size && r == TRY; ++i) {
            if (n >= parent->size)
                n %= parent->size;
            
            //! THERE IS A PROBLEM I CANNOT SOLVE YET
            //! DOES NOT DELETE JUST RUNS SIZE TIMES
            /* Nth child definitly different from NULL */
            if (parent->child[n] == NULL)
                r = FAIL;
            if (parent->child[n] != DELETED && strcmp(parent->child[n]->name, h->name) == 0) {
                child = parent->child[n];
                if (child->child != NULL)
                    for (i = 0; i < parent->child[n]->size; ++i)
                        if (parent->child[n] != NULL && parent->child[n] != DELETED)
                            c2d_delete_hierarchy(child->child[i]);
                free(child->name);
                free(child);
                //! free cad content
                parent->child[n] = DELETED;
            }
            /* Deleted or not empty, check next index */
            else {
                printf("++\n");
                ++n;
            }
        }
    }
    /* Remove all the content inside of the hierarchy */
    printf("DELETED Hierarchy %s, %d\n", h->name, i);
    c2d_delete(h->cad);
}

Hierarchy * c2d_get_root_hierarchy (Hierarchy * h) {
    if (h == NULL || h->parent == NULL)
        return h;
    else
        return c2d_get_root_hierarchy(h->parent); 
}

CAD2D * c2d_get_root_cad (CAD2D * cad) {
    Hierarchy * h;

    if (cad != NULL) {
        /* Since every CAD has hierarchy inf. h must be different from NULL */
        h = c2d_get_root_hierarchy(cad->hierarchy);
        return h->cad;
    }
    else   
        return NULL;
}

char * u_create_hierarchy_name (Hierarchy * h) {
    Hierarchy * root = c2d_get_root_hierarchy(h);
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
        //! What happens if name is not created
    }

    printf("New Hierarchy: %s\n", r);
    return r;    
}


/* In case of matching returns nonnegative index value of the hierarchy, o.w. returns FAIL(-1) */
int u_find_hierarchy (Hierarchy * root, Hierarchy * h) {
    int i, r, n;

    /* First hierarchy is unique since list uninitiliazed yet */
    if (root == NULL || root->size == 0)
        r = FAIL;
    else {
        if (h == root)  /* No need to check, switch the child hierarchies */
            r = FAIL;
        else {
            /* use hash-code as index of the unique hierarchy */
            n = h->hash_code;
            for (i = 0, r = TRY; i < root->size && r == TRY; ++i) {
                if (n >= root->size)
                    n %= root->size;
                
                /* Not in this hierarchy */
                if (root->child[n] == NULL)
                    r = FAIL;
                /* same hash-code is a sign for same keys */
                else if (root->child[n] != DELETED && strcmp(root->child[n]->name, h->name) == 0)             
                    r = n;
                /* Deleted or not empty */
                else 
                    ++n;
            }
        } 
        /* If there isn't match yet, continue with children */
        if (r == TRY || r == FAIL) 
            for (i = 0, r = FAIL; i < root->size && r == FAIL; ++i)
                if (root->child[i] != NULL)
                    r = u_find_hierarchy(root->child[i], h);
    }
    if (root != NULL)
        printf("for %s - %d: u_find_hierarchy() returns %d\n", h->name, h->hash_code, r);
    else
        printf("Root NULL\n");
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
            if (parent->child[n] != NULL && parent->child[n] != DELETED) {
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
        /*  We cannot find place inside of the current hash-table, 
            so the next empty place is outside of the table        */
        if (parent->size > 0 && r == TRY)
            r = i; 
        
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
            printf("%s inserted as index %d and child of %s\n\n\n", child->name, r, child->parent->name);
        }
    }
    else    
        printf("%s named hierarchy already exist!\n", child->name);

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
    CAD2D * cad_root = c2d_get_root_cad(cad);
    char c = '0', * r = calloc(10, sizeof(char)), * h;
    int n = 0;

    /* LabelName: EntityType + Hierarchy level/depth + instance */
    switch (l->type) {
        case point_t:
            r[0] = 'P';    break;
        case xy_plane:
            r[0] = 'X';
            r[1] = 'Y';    
            r[2] = 'P';    break;
        case line_t:
            r[0] = 'L';    break;
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
        default:
            printf("Undefined entity type during label name creation\n");
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
            
            /* Not in this elist */
            if (cad->elist[n] == NULL)
                r = FAIL;
            /* same hash-code is a sign for same keys */
            else if (cad->elist[n] != DELETED && strcmp(cad->elist[n]->label->name, l->name) == 0)
                r = n;
            else
                ++n;
        }
    }

    /* If label haven't find yet, continue with child hierarchies */
    if (r == FAIL || r == TRY) {
        r = FAIL;
        if (cad->hierarchy != NULL) 
            for (i = 0; i < cad->hierarchy->size && r == FAIL; ++i)
                if (cad->hierarchy->child[i] != NULL && cad->hierarchy->child[i] != DELETED) {
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

/* Removes the source label from given LabeList */
void u_remove_label (LabeList ** llist, Label ** l) {
    LabeList * tmp;
    while (*llist != NULL && *l != NULL) {
        if ((*llist)->label == *l) {
            tmp = *llist;
            *llist = (*llist)->next;
            
            free((*l)->name);
            free(*l);
            free(tmp);
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

        if (u_insert_entity_list(cad, e) == FAIL) {
            c2d_remove_entity(cad, &l);
            e = NULL;
        }
        else
            u_insert_label(&cad->llist, e->label); 
    }
    
    return e;
}

/*  In case of matching returns the entity, it's index
    at entitylist and it's contianer cad                 */
EntityInfo * c2d_find_entity (CAD2D * root, Label * l) {
    int i, r, n;
    EntityInfo * e_info = NULL;

    if (root != NULL && l != NULL) {
        n = l->hash_code;
        for (i = 0, r = TRY; i < root->elist_size && r == TRY; ++i) {
            if (n >= root->elist_size)
                n %= root->elist_size;
            
            /* Not in this list */
            if (root->elist[n] == NULL) 
                r = FAIL;     
            /* Finded, Return e_info */
            else if (root->elist[n] != DELETED && strcmp(l->name, root->elist[n]->label->name) == 0) {
                r = n;
                e_info = (EntityInfo *) malloc(sizeof(EntityInfo));
                if (e_info != NULL) {
                    e_info->entity = root->elist[n];
                    e_info->cad = root;
                    e_info->index = n; 
                }
            }
            /* Still can be there, continue searching */
            else
                ++n;
        }

        /* Continue searching with child hierarchies */
        if (r = TRY || r == FAIL) {
            if (root->hierarchy != NULL)
                for (i = 0; i < root->hierarchy->size && e_info == NULL; ++i)
                    if (root->hierarchy->child[i] != NULL && root->hierarchy->child[i] != DELETED)
                        e_info = c2d_find_entity(root->hierarchy->child[i]->cad, l);
        } 
    }
    return e_info;
}

/* Removes the entity given by it's label */
void c2d_remove_entity (CAD2D * cad, Label ** l) {
    EntityInfo * e_info = c2d_find_entity(c2d_get_root_cad(cad), *l);
    Entity * e;

    /* free allocated memory, and delete it's record from lists */
    if (e_info != NULL) {
        /* Be sure given cad is container of given label */
        cad = e_info->cad;
        e = cad->elist[e_info->index];
        
        printf("Delete %s: ", (*l)->name);
        
        /* Free entity data */
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
        }
        
        /* Free style and label of entity, then free itself */
        u_remove_label(&cad->llist, l);
        free(e->style);
        free(e);
        
        /* Set Deleted flag in hash-table to maintain linear-probing */
        cad->elist[e_info->index] = DELETED;
        printf("Done\n");
        free(e_info);
    }
}

int u_insert_entity_list (CAD2D * cad, Entity * e) {
    Entity ** tmp;
    int i, n, r;

    /* Find proper where for given entity */
    for (i = 0, r = TRY, n = e->label->hash_code; i < cad->elist_size && r == TRY; ++i) {
        if (n >= cad->elist_size)
            n %= cad->elist_size;
        
        /* We find an empty place */
        if (cad->elist[n] == NULL || cad->elist[n] == DELETED)        
            r = n; 
        else if (strcmp(cad->elist[n]->label->name, e->label->name) == 0)
            r = FAIL;
        /* check next empty place */
        else                
            ++n;
    }

    if (r != FAIL) {
        /* In case of UNFAIL, if an empty place finded r would be n o.w. r would be list_size */
        
        /*  We cannot find place inside of the current hash-table, 
        so the next empty place is outside of the table        */
        if (r == TRY && cad->elist_size > 0)
            r = i;  /* r = cad->elist_size */
        else
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
    free(t);
}

/*********************************************************************************
 * CAD Start
*********************************************************************************/

/* Initialize a new cad without any constrain */
CAD2D * c2d_start () {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));
    
    if (cad != NULL) {
        /* Initialize canvas as 1000x1000 also enable automaticly expand canvas mode */
        cad->auto_canvas = 1;   
        cad->canvas.start.x = cad->canvas.start.y = -500.0;
        cad->canvas.end.x = cad->canvas.end.y = 500.0;

        cad->elist = NULL;
        cad->elist_size = 0;
        cad->llist = NULL;
        
        cad->hierarchy = c2d_create_hierarchy(cad);
    }
    return cad;
}

CAD2D * c2d_start_wh (double width, double height) {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));
    
    if (cad != NULL) {
        /* Set canvas as fixed size */
        cad->auto_canvas = 0;   
        cad->canvas.start.x = -width / 2;
        cad->canvas.start.y = -height / 2;
        cad->canvas.end.x = width / 2; 
        cad->canvas.end.y = height / 2; 

        cad->elist = NULL;
        cad->elist_size = 0;
        cad->llist = NULL;
        
        cad->hierarchy = c2d_create_hierarchy(cad);
    }
    return cad;
}

/* Initialize a new CAD at child of given hierarchy */
CAD2D * c2d_start_wh_hier (double width, double height, Hierarchy * h) {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));
    CAD2D * root;

    if (cad != NULL) {
        cad->auto_canvas = 0;   
        cad->canvas.start.x = -width / 2;
        cad->canvas.start.y = -height / 2;
        cad->canvas.end.x = width / 2; 
        cad->canvas.end.y = height / 2; 

        cad->elist = NULL;
        cad->elist_size = 0;
        cad->llist = NULL;

        cad->hierarchy = c2d_create_hierarchy_parent(cad, h);
        if (cad->hierarchy != NULL) {
            /*  If auto-canvas is on, update the root size as max-canvas
                o.w. change the canvas of child-cad as min-canvas        */
            root = c2d_get_root_cad(cad);
            if (NULL != root) {
                if (1 == root->auto_canvas)
                    root->canvas = *u_max_canvas(&root->canvas, &cad->canvas);
                else
                    cad->canvas = *u_min_canvas(&root->canvas, &cad->canvas);
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

Label * c2d_add_point_xy (CAD2D * cad, double x, double y) {
    Point2D * d;
    Label * l = NULL;
    if (u_check_canvas_xy(cad, x, y)) {
    
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

Label * c2d_add_point_p (CAD2D * cad, Point2D p) {
    Point2D * d;
    Label * l = NULL;

    if (u_check_canvas_p(cad, &p)) {
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

    if (u_check_canvas_p(cad, &start) && u_check_canvas_p(cad, &end)) {
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

/* takes an point array */
Label * c2d_add_polyline (CAD2D * cad, Point2D * p, int size) {
    PointList * head = NULL, ** trav = &head;
    Label * l = NULL;
    int i;

    for (i = 0; i < size; ++i) {
        if (u_check_canvas_p(cad, p + i)) {
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

Label * c2d_add_regular_polygon (CAD2D * cad, int n, Point2D center, double out_radius) {
    RegularPolygon * d; 
    Label * l = NULL;

    if (u_check_canvas_xy(cad, center.x + out_radius, center.y + out_radius) &&
        u_check_canvas_xy(cad, center.x + out_radius, center.y + out_radius)) {
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

Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius) {
    Circle * d;
    Label * l = NULL;

    if (u_check_canvas_xy(cad, center.x + radius, center.y + radius) &&
        u_check_canvas_xy(cad, center.x - radius, center.y - radius)) {
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
    if (u_check_canvas_xy(cad, center.x + radius, center.y + radius) &&
        u_check_canvas_xy(cad, center.x - radius, center.y - radius)) {

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

    if (u_check_canvas_xy(cad, center.x + radius_x, center.y + radius_y) &&
        u_check_canvas_xy(cad, center.x - radius_x, center.y - radius_y)) {
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

Label * c2d_add_triangle (CAD2D * cad, Point2D corner1, Point2D corner2, Point2D corner3) {
    Triangle * d;
    Label * l = NULL;

    if (u_check_canvas_p(cad, &corner1) && 
        u_check_canvas_p(cad, &corner2) &&
        u_check_canvas_p(cad, &corner3)) {
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
        
    if (u_check_canvas_p(cad, &corner1) && u_check_canvas_p(cad, &corner1)) {
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

Label * c2d_add_xy_plane (CAD2D * cad) {
    Label * l = c2d_create_label_default(cad, xy_plane);
    if (l != NULL)
        u_create_entity(cad, l, NULL, NULL);

    return l;
}

/* Focuses center of the given entity */
Label * c2d_add_focus_point (CAD2D * cad, Label * ls) {
    EntityInfo * e_info = c2d_find_entity(c2d_get_root_cad(cad), ls);
    Point2D * p;
    Label * l = NULL;

    if (e_info != NULL) {
        p = (Point2D *) malloc(sizeof(Point2D));
        if (p != NULL) {
            p->x = ((Point2D *) e_info->entity->data)->x;
            p->y = ((Point2D *) e_info->entity->data)->y;
            //! NOT IMPLEMENTED YET
            // l = c2d_create_label_default(cad, focus_point_t);
                            
            if (l != NULL)
                u_create_entity(cad, l, p, NULL);
            else
                free(p);
        }
    } 

    return l;
}

Label * c2d_add_measurement (CAD2D * cad, Label * ls1, Label * ls2) {
    double * d;
    Label * l = NULL;
    double distance;

    /* Get the distance */
    // distance = c2d_measure(cad, ls1, ls2);
        //! We need 2 point
    /* Define the Points */
    /* Be sure we are not exceeding canvas */
    d = (double *) malloc(sizeof(double));
        /*
    if (d != NULL) {
        d->start; 
        d->end;
        d->distance = distance;
    }
        */

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
    if (u_check_canvas_xy(cad, p.x, p.y)) {
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

Canvas * u_max_canvas (Canvas * c1, Canvas * c2) {
    double w1, h1, w2, h2;

    /* Calculate width and height of canvases */
    w1 = c1->end.x - c1->start.x;
    h1 = c1->end.y - c1->start.y; 
    
    w2 = c2->end.x - c2->start.x;
    h2 = c2->end.y - c2->start.y; 
    
    return w1 * h1 > w2 * h2 ? c1 : c2;
}

Canvas * u_min_canvas (Canvas * c1, Canvas * c2) {
    double w1, h1, w2, h2;

    /* Calculate width and height of canvases */
    w1 = c1->end.x - c1->start.x;
    h1 = c1->end.y - c1->start.y; 
    
    w2 = c2->end.x - c2->start.x;
    h2 = c2->end.y - c2->start.y; 
    
    return w1 * h1 < w2 * h2 ? c1 : c2;
}

int u_check_canvas_p (CAD2D * cad, Point2D * p) {
    CAD2D * root;
    int r;
    
    do {
        r = p->x >= cad->canvas.start.x  && 
            p->y >= cad->canvas.start.y  && 
            p->x <= cad->canvas.end.x    && 
            p->y <= cad->canvas.end.y;

        /* if auto_canvas is on, check if canvas size is enough o.w. return the result */
        if (1 == cad->auto_canvas) {
            if (0 == r) {
                printf("Canvas size doubled\n");
                getchar();
                /* Double up the canvas size */
                cad->canvas.start.x *= 2;
                cad->canvas.start.y *= 2;
                cad->canvas.end.x *= 2;
                cad->canvas.end.y *= 2;

                root = c2d_get_root_cad(cad);                
                if (root != NULL) {
                    root->canvas.start.x = cad->canvas.start.x;
                    root->canvas.start.y = cad->canvas.start.y;
                    root->canvas.end.x = cad->canvas.end.x;
                    root->canvas.end.y = cad->canvas.end.y;
                }
                /* to check the new size is enough, TRY flag is up */
                r = TRY;    
            }
        }
    } while (TRY == r);

    return r;
}

int u_check_canvas_xy (CAD2D * cad, double x, double y) {
    CAD2D * root;
    int r;

    do {
        r = x >= cad->canvas.start.x &&
            y >= cad->canvas.start.y &&
            x <= cad->canvas.end.x   &&
            y <= cad->canvas.end.y;

        /* if auto_canvas is on, check if canvas size is enough o.w. return the result */
        if (1 == cad->auto_canvas) {
            if (0 == r) {
                /* Double up the canvas size */
                printf("Canvas size doubled\n");
                getchar();
                cad->canvas.start.x *= 2;
                cad->canvas.start.y *= 2;
                cad->canvas.end.x *= 2;
                cad->canvas.end.y *= 2;

                root = c2d_get_root_cad(cad);                
                if (root != NULL) {
                    root->canvas.start.x = cad->canvas.start.x;
                    root->canvas.start.y = cad->canvas.start.y;
                    root->canvas.end.x = cad->canvas.end.x;
                    root->canvas.end.y = cad->canvas.end.y;
                }
                /* to check the new size is enough, TRY flag is up */
                r = TRY;    
            }
        }
    } while (TRY == r);

    return r;
}

/*********************************************************************************
 * Measurement Between Entity
*********************************************************************************/

/* Measures the distance between two entity given by their labels */
double c2d_measure (CAD2D * cad, Label * ls, Label * lt) {
    double m; //! Is returnining measurement suitable

    /* Define the types of given two entity to implement specific algorithm */
        /* Get the measurement and return it */
    /* o.w. return m.distance = -1 */
    return m;
}

/* Returns the closest distance between point and polyline */
double u_measure_point_polyline (Point2D * point, PointList * pline) {

}

double u_measure_point_arc (Point2D * p, Circle * c) {
    //! NOT IMPLEMENTED YET
}

/* Measures the distance between center of given two entity */
double c2d_measure_center2D (CAD2D * cad, Label * ls, Label * lt) {

}

/* creates an point in center of given entity //! LABEL ? */
Point2D c2d_get_center2D (Entity * e) {
    Point2D c;

    if (e != NULL) {
        switch (e->label->type) {
            case point_t:
                break;
            case line_t:
            case polyline_t:
            case irregular_polygon_t:
            case regular_polygon_t:
                c = u_get_center_line(e->data);
                break;
            case triangle_t:
                c = u_get_center_triangle(e->data);
                break;
            case rectangle_t:
                c = u_get_center_rectangle(e->data);
                break;
            case circle_t:
            case arc_t:
                c = ((Circle *) e->data)->center;
                break;
            case ellipse_t:
                c = ((Ellipse *) e->data)->center;
                break;
            case text_t:
                //! NOT IMPLEMENTED YET
                break;
            default:
                c.x = c.y = -1;
                printf("Given entity is not a 2D shape\n");
        }
    }

    return c;
}

/* Common function for line, polyline and polygone */
Point2D u_get_center_line (PointList * e) {
    Point2D c;
    int i = 0;
    c.y = c.x = 0;

    while (e != NULL) {
        c.x += e->point.x;
        c.y += e->point.y;

        e = e->next;
        ++i;
    }

    if (i > 1) {
        c.x /= i;
        c.y /= i;
    }
    return c;
}

Point2D u_get_center_triangle (Triangle * e) {
    Point2D c;
    c.x = (e->corner1.x + e->corner2.x + e->corner3.x) / 3;
    c.x = (e->corner1.y + e->corner2.y + e->corner3.y) / 3;
    return c;
}

Point2D u_get_center_rectangle (Rectangle * e) {
    Point2D c;
    c.x = (e->corner1.x + e->corner2.x) / 2;
    c.y = (e->corner1.y + e->corner2.y) / 2;
    return c;
}

/*********************************************************************************
 * Snapping Labels
*********************************************************************************/  

/*
Polyline    Point       Define a strategy!
Polyline    Polyline    Define a strategy!
Polyline    Polygon     Define a strategy!
Polygon     Point       stle polgon as it's center is given target point
Polygon     Polyline    Define a strategy!
// Arc         Point       set arc as it's center is given target point
Arc         Polyline    Define a strategy
*/

void c2d_snap (CAD2D * cad, const Label * ls, const Label * lt) {
    EntityInfo * se_info = c2d_find_entity(cad, ls), * te_info = c2d_find_entity(cad, lt);
    Entity * s, * t;
    Point2D * c1, * c2;

    if (se_info != NULL && te_info != NULL) {
        s = se_info->entity;
        t = te_info->entity;
        switch (ls->type) {
            case point_t:
                switch (lt->type) {
                    case point_t:
                        u_snap_point_point(s->data, t->data);
                        break;                       
                    case line_t:
                    case polyline_t:
                        u_snap_point_line(s->data, t->data);
                        //! NOT IMPLEMENTED YET
                        break;
                    case irregular_polygon_t:
                    case regular_polygon_t:
                        // (s->data) = u_get_center_line(t->data);
                        break;
                    case rectangle_t:
                        // (s->data) = u_get_center_rectangle(t->data);
                        break;
                    case triangle_t:
                        //! NOT IMPLEMENTED YET
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
    free(se_info);
    free(te_info);
}

/* Snaps the source point to the target point */
void u_snap_point_point (Point2D * s, Point2D * t) {
    s->x = t->x;
    s->y = t->y;
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
    EntityInfo * e_info = c2d_find_entity(c2d_get_root_cad(cad), label); 
    EntityStyle * style = NULL;
    
    if (e_info != NULL) {
        style = (EntityStyle *) malloc(sizeof(EntityStyle));
        if (style != NULL) {
            style->color = c;
            style->draw = d;
            style->line = l;
            style->line_width = w;
            e_info->entity->style = style;
        }
        free(e_info);
    }

    return style;
}

TextStyle * c2d_set_text_style (CAD2D * cad, Label * label, FontStyle f, RGBColor c, double s) {
    TextStyle * style;
    EntityInfo * e_info = c2d_find_entity(cad, label); 
    Text * d;
    
    if (e_info != NULL) {
        style = (TextStyle *) malloc(sizeof(TextStyle));
        if (style != NULL) {
            style->color = c;
            style->font = f;
            style->scale = s;
            
            d = (Text *) e_info->entity->data;
            d->style = style;
        }
        free(e_info);
    }

    return style;
}

void c2d_set_color_pallette (RGBColor * c, ColorPalette color) {
    switch (color) {
        case red:
            c->red = 1.0, c->green = 0.0, c->blue = 0.0; break; 
        case green:
            c->red = 0.0, c->green = 1.0, c->blue = 0.0; break; 
        case green_dark:
            c->red = 0.0, c->green = 0.5, c->blue = 0.0; break; 
        case blue:
            c->red = 0.0, c->green = 0.0, c->blue = 1.0; break; 
        case blue_light:
            c->red = 0.0, c->green = 1.0, c->blue = 1.0; break; 
        case magenta:
            c->red = 1.0, c->green = 0.0, c->blue = 1.0; break; 
        case yellow:
            c->red = 1.0, c->green = 1.0, c->blue = 0.0; break; 
        case white:
            c->red = 1.0, c->green = 1.0, c->blue = 1.0; break; 
        case black:
            c->red = 0.0, c->green = 0.0, c->blue = 0.0; break; 
        case orange:
            c->red = 1.0, c->green = 0.7, c->blue = 0.0; break; 
        case purple:
            c->red = 0.7, c->green = 0.3, c->blue = 1.0; break; 
        case brown:
            c->red = 0.7, c->green = 0.3, c->blue = 0.0; break; 
        default:
            printf("Given color cannot find in color pallette\n");      
    }
}

void c2d_set_color_rgb (RGBColor * c, double red, double green, double blue) {
    c->red = red;
    c->green = green;
    c->blue = blue;
}

/*********************************************************************************
 * Import & Export
*********************************************************************************/

/* Import's a CAD object from given file, for now only gtucad format available */
CAD2D * c2d_import (char * file_name, ExImOption option) {
    CAD2D * cad = NULL;
    FILE * fid;

    switch (option) {
        /* Import in gtucad format */
        case gtucad:        
            fid = fopen(file_name, "rb");
            if (fid != NULL) {
                printf("IMPORT GTUCAD:\n");
                cad = u_import_gtucad(fid);
                fclose(fid);
            }
            break;
        default:
            printf("Given import format is invalid\n");        
            printf("Please select file format \"gtucad\"\n");        
    }

    return cad;
}

CAD2D * u_import_gtucad (FILE * fid) {
    CAD2D * cad = (CAD2D *) malloc(sizeof(CAD2D));
    
    if (cad != NULL) {
        fread(cad, sizeof(CAD2D), 1, fid);
        
        /*  If cad element points different than NULL, it means
            we need to allocate memory for that data and read from the file */

        cad->elist = u_import_gtucad_elist(fid, cad->elist, cad->elist_size);
        cad->llist = u_import_gtucad_llist(fid, cad->llist, cad->elist, cad->elist_size);
        cad->hierarchy = u_import_gtucad_hierarchy(fid, cad);
    }
    return cad;
}

Entity ** u_import_gtucad_elist (FILE * fid, Entity ** elist, int elist_size) {
    Entity * e;
    PointList ** l;
    int i;
    
    if (elist != NULL) {
        elist = (Entity **) calloc(elist_size, sizeof(Entity *));
        if (elist != NULL) {
            /* Read the entities array, then read each filled space by seperatly */
            fread(elist, sizeof(Entity *), elist_size, fid);

            /*  If given entity data based on linked-list
                structure, than read them one by one        */
            for (i = 0; i < elist_size; ++i) {
                if (elist[i] != NULL && elist[i] != DELETED) {
                    e = elist[i] = (Entity *) malloc(sizeof(Entity));
                    if (elist[i] != NULL) {
                        fread(e, sizeof(Entity), 1, fid);

                        /* Read label of entity */
                        e->label = (Label *) malloc(sizeof(Label));
                        if (e->label != NULL) {
                            fread(e->label, sizeof(Label), 1, fid);
                            e->label->name = u_read_bin_str(fid);
                        }
                        /*
                            for entities consist of PointList(linked-list) 
                            should be exported seperatly node by node like  */
                        if (e->label != NULL) {
                            /* Read entity data */
                            switch (e->label->type) {
                                case line_t:
                                case polyline_t:
                                case irregular_polygon_t:
                                    for (l = (PointList **) &e->data; *l != NULL; l = &(*l)->next) {
                                        *l = (PointList *) malloc(sizeof(PointList));
                                        if (*l != NULL)
                                            fread(*l, sizeof(PointList), 1, fid);
                                        else
                                            break;  /* Prevents segFault comes during increment */
                                    }
                                    break;
                                case point_t:
                                    e->data = (Point2D *) malloc(sizeof(Point2D));
                                    if (e->data != NULL)
                                        fread(e->data, sizeof(Point2D), 1, fid); 
                                    break;
                                case regular_polygon_t:
                                    e->data = (RegularPolygon *) malloc(sizeof(RegularPolygon));
                                    if (e->data != NULL)
                                        fread(e->data, sizeof(RegularPolygon), 1, fid); 
                                    break;
                                case triangle_t:
                                    e->data = (Triangle *) malloc(sizeof(Triangle));
                                    if (e->data != NULL)
                                        fread(e->data, sizeof(Triangle), 1, fid); 
                                    break;
                                case rectangle_t:
                                    e->data = (Rectangle *) malloc(sizeof(Rectangle ));
                                    if (e->data != NULL)    
                                        fread(e->data, sizeof(Rectangle), 1, fid); 
                                    break;
                                case circle_t:
                                case arc_t:
                                    e->data = (Circle *) malloc(sizeof(Circle ));
                                    if (e->data != NULL)
                                        fread(e->data, sizeof(Circle), 1, fid); 
                                    break;
                                case ellipse_t:
                                    e->data = (Ellipse *) malloc(sizeof(Ellipse ));
                                    if (e->data != NULL)
                                        fread(e->data, sizeof(Ellipse), 1, fid); 
                                    break;
                                case text_t:
                                    /* Import Text object, it's text(str) and it's style */
                                    e->data = (Text *) malloc(sizeof(Text));
                                    if (e->data != NULL) {
                                        fread(e->data, sizeof(Text), 1, fid); 
                                        ((Text *) e->data)->text = u_read_bin_str(fid);
                                        if (((Text *) e->data)->style != NULL)
                                            fread(((Text *) e->data)->style, sizeof(TextStyle), 1, fid);
                                    }
                                    break;
                            }
                        }
                        /* Read entity style */
                        if (e->style != NULL) {
                            e->style = (EntityStyle *) malloc(sizeof(EntityStyle));
                            if (e->style != NULL)
                                fread(e->style, sizeof(EntityStyle), 1, fid);
                        }
                    }
                } 
            }
            printf("Import elist is DONE\n");
        } 
    }
    return elist;
}

LabeList * u_import_gtucad_llist (FILE * fid, LabeList * llist, Entity ** elist, int elist_size) {
    LabeList ** l = &llist;
    Label tmp_l;

    while (*l != NULL) {
        *l = (LabeList *) malloc(sizeof(LabeList));
        if (*l != NULL) {
            fread(*l, sizeof(LabeList), 1, fid);
            
            /*  Since label is already allocated with import_elist,
                just find address of that label by name of label and
                assing it without allocating new memory for same values     */
            fread(&tmp_l, sizeof(Label), 1, fid);
            tmp_l.name = u_read_bin_str(fid); 
            (*l)->label = u_find_label_elist(elist, elist_size, &tmp_l);
            
            free(tmp_l.name);
            l = &(*l)->next;
        }
    } 

    printf ("Import llist is DONE\n");
    
    return llist;
}

Hierarchy * u_import_gtucad_hierarchy (FILE * fid, CAD2D * cad) {
    Hierarchy * h = NULL;
    CAD2D * child_cad;
    int i;
    

    if (cad->hierarchy != NULL) {
        h = (Hierarchy *) malloc(sizeof(Hierarchy));
        if (h != NULL) {
            fread(h, sizeof(Hierarchy), 1, fid);
            h->cad = cad;

            /* Read the hierarchy name */
            if (h->name != NULL) {
                h->name = u_read_bin_str(fid);
                printf("export: %s\n", h->name);
            } 

            /* Import the child hierarchies */
            if (h->child != NULL) {
                h->child = (Hierarchy **) calloc(h->size, sizeof(Hierarchy *));
                if (h->child != NULL) {
                    fread(h->child, sizeof(Hierarchy *), h->size, fid);
                    for (i = 0; i < h->size; ++i) {
                        if (h->child[i] != NULL && h->child[i] != DELETED) {
                            child_cad = u_import_gtucad(fid);    
                            /* Link child with parent hierarchy */
                            if (child_cad != NULL) {
                                h->child[i] = child_cad->hierarchy;
                                h->child[i]->parent = h;
                            }
                        }
                    }
                }
            }
        }
    }

    return h;
}

/* Utility function for import_llist serve purpose of do not allocating new memory */
Label * u_find_label_elist (Entity ** elist, int elist_size, Label * l) {
    Label * src_l = NULL;
    int i, n, r;

    if (elist != NULL && l != NULL) {
        for (i = 0, n = l->hash_code, r = TRY; i < elist_size && r == TRY; ++i) {
            if (n >= elist_size)
                n %= elist_size;
            
            /* Not in this elist */
            if (elist[n] == NULL)
                r = FAIL;
            else if (elist[n] != DELETED && strcmp(elist[n]->label->name, l->name) == 0) {
                r = n; 
                src_l = elist[n]->label;
            }
            else
                ++n;
        }
    } 

    return src_l;
}

char * u_read_bin_str (FILE * fid) {
    char buff[1000], * r;
    int i = 0;

    do fread(buff + i, sizeof(char), 1, fid);
    while (buff[i++] != '\0');

    r = (char *) calloc(i + 1, sizeof(char));
    if (r != NULL)
        strcpy(r, buff);

    printf("Readed from file: %s\n", r);
    return r;
}

/* takes ROOT CAD cad, and exports all the content inside it */
void c2d_export (CAD2D * cad, char * file_name, ExImOption option) {
    FILE * fid;

    if (cad != NULL) {
        switch (option) {
            /* Export in eps format */
            case eps:        
                fid = fopen(file_name, "wt");
                if (fid != NULL) {
                    printf("EXPORT EPS:\n");

                    /* Write the eps file specification and settings of CAD */
                    fprintf(fid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
                    fprintf(fid, "%%%%BoundingBox: %.2f %.2f %.2f %.2f\n", cad->canvas.start.x, cad->canvas.start.y, cad->canvas.end.x, cad->canvas.end.y);

                    u_export_eps(fid, cad);
                    fprintf(fid, "\nshowpage\n");
                    fclose(fid);
                }  
                break;
            /* Export in gtucad format */
            case gtucad:     
                fid = fopen(file_name, "wb");
                if (fid != NULL) {
                    printf("EXPORT GTUCAD:\n");
                    u_export_gtucad(fid, cad);
                    fclose(fid);
                }            
                break;
            default:
                printf("Given export format is invalid\n");        
                printf("Please select one of the formats \"gtucad\" or \"eps\"\n");        
        }
    }
}

/* Exports all the CAD content by recursivly with it's utility functions it's hierarchy data by recursivly */
void u_export_gtucad (FILE * fid, CAD2D * cad) {
    /* Export CAD object */
    printf("export: %s\n", cad->hierarchy->name);


    fwrite(cad, sizeof(CAD2D), 1, fid);
    u_export_gtucad_elist(fid, cad->elist, cad->elist_size);
    u_export_gtucad_llist(fid, cad->llist);
    u_export_gtucad_hierarchy(fid, cad->hierarchy);
}

void u_export_gtucad_elist (FILE * fid, Entity ** elist, int elist_size) {
    int i;
    Entity * e;
    PointList * l;

    if (elist != NULL) {
        fwrite(elist, sizeof(Entity *), elist_size, fid);
        /* Export the data of filled blocks with entity data */
        for (i = 0; i < elist_size; ++i) {
            if (elist[i] != NULL && elist[i] != DELETED) {
                e = elist[i];
                fwrite(e, sizeof(Entity), 1, fid);
                
                /* Export entity label and it's data */
                fwrite(e->label, sizeof(Label), 1, fid);
                fwrite(e->label->name, sizeof(char), strlen(e->label->name) + 1, fid);

                switch (e->label->type) {
                    case line_t:
                    case polyline_t:
                    case irregular_polygon_t:
                        for (l = (PointList *) e->data; l != NULL; l = l->next)
                            fwrite(l, sizeof(PointList), 1, fid);
                        break;
                    case point_t:
                    case regular_polygon_t:
                        fwrite(e->data, sizeof(RegularPolygon), 1, fid); 
                        break;
                    case triangle_t:
                        fwrite(e->data, sizeof(Triangle), 1, fid); 
                        break;
                    case rectangle_t:
                        fwrite(e->data, sizeof(Rectangle), 1, fid); 
                        break;
                    case circle_t:
                    case arc_t:
                        fwrite(e->data, sizeof(Circle), 1, fid); 
                        break;
                    case ellipse_t:
                        fwrite(e->data, sizeof(Ellipse), 1, fid); 
                        break;
                    case text_t:
                        /* Export Text object, it's text(str) and it's style */
                        fwrite(e->data, sizeof(Text), 1, fid); 
                        fwrite(((Text *) e->data)->text, sizeof(char), strlen(((Text *) e->data)->text) + 1, fid);
                        if (((Text *) e->data)->style != NULL)
                            fwrite(((Text *) e->data)->style, sizeof(TextStyle), 1, fid);
                        break;
                }

                /* Export entity style */
                if (e->style != NULL) {
                    fwrite(e->style, sizeof(EntityStyle), 1, fid);
                }
            }
        }
    }
    printf("Export elist is DONE\n");
}

void u_export_gtucad_llist (FILE * fid, LabeList * llist) {
    while (llist != NULL) {
        fwrite(llist, sizeof(LabeList), 1, fid);
        fwrite(llist->label, sizeof(Label), 1, fid);
        fwrite(llist->label->name, sizeof(char), strlen(llist->label->name) + 1, fid);
        llist = llist->next;
    }
    printf ("Export llist is DONE\n");
}

/*  Actualy u_export_gtucad is recursive function 
    because this function called by u_export_gtucad  */
void u_export_gtucad_hierarchy (FILE * fid, Hierarchy * h) {
    int i;

    if (h != NULL) {
        fwrite(h, sizeof(Hierarchy), 1, fid);
        fwrite(h->name, sizeof(char), strlen(h->name) + 1, fid);
        
        /* Export the childs */
        if (h->child != NULL) {
            fwrite(h->child, sizeof(Hierarchy *), h->size, fid);
            
            /* Here recursive comes the scene */
            for (i = 0; i < h->size; ++i) 
                if (h->child[i] != NULL && h->child[i] != DELETED)
                    u_export_gtucad(fid, h->child[i]->cad);
        } 
    }
}

void u_export_eps (FILE * fid, CAD2D * cad) {
    EntityInfo * e_info;
    Hierarchy * h;
    LabeList * l; 
    Entity * e;
    int i;

    if (cad != NULL) {
        h = cad->hierarchy;
        l = cad->llist;
        
        printf("\nHierarchy: %s\n", cad->hierarchy->name);
        
        /* export current cad */
        while (l != NULL) {
            e_info = c2d_find_entity(cad, l->label);
            
            if (e_info != NULL) {
                e = e_info->entity;
                
                printf("\t%s\t%d\n", e->label->name, e->label->hash_code);
                switch (e->label->type) {
                    case point_t:
                        u_export_eps_point(fid, e->data);
                        break;
                    case xy_plane:
                        u_export_eps_xy_plane(fid, &cad->canvas, e->style);
                        break;
                    case line_t:
                    case polyline_t:
                    case irregular_polygon_t:
                        u_export_eps_line(fid, e->data);
                        break;
                    case regular_polygon_t:
                        u_export_eps_regular_polygon(fid, e->data);
                        break;
                    case rectangle_t:
                        u_export_eps_rectangle(fid, e->data);
                        break;
                    case triangle_t:
                        u_export_eps_triangle (fid, e->data);
                        break;
                    case circle_t:
                    case arc_t:
                        u_export_eps_circle(fid, e->data);
                        break;
                    case ellipse_t:
                        u_export_eps_ellipse(fid, e->data);
                        break;
                    case text_t:
                        u_export_eps_text(fid, e->data);
                        break;
                    default:
                        continue;
                }

                /*  One specific design choice is reflected all the page. 
                    To reflect just one entity, reset the style as default  */
                if (e->style != NULL) {
                    u_export_eps_entity_style(fid, e->label, e->style);
                    u_export_eps_entity_style_reset(fid);
                }
                else if (e->label->type != text_t) {
                    fprintf(fid, "stroke\n");
                    if (e->label->type == xy_plane)
                        u_export_eps_entity_style_reset(fid);
                }

                free(e_info);
            }
            else
                printf("%s not find in %s at exporting process\n", l->label->name, cad->hierarchy->name);
            l = l->next;
        }
        /* Export child of the root hierarchy */
        printf("Child hiearchy number: %d\n", h->size);
        for (i = 0; i < h->size; ++i)
            if (h->child[i] != NULL && h->child[i] != DELETED)
                u_export_eps(fid, h->child[i]->cad);
    }
}

/* set font, color and scale of the text */
void u_export_eps_text_style (FILE * fid, TextStyle * s) {
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

void u_export_eps_entity_style (FILE * fid, Label * l, EntityStyle * s) {
    fprintf(fid, "\n%% Set Entity style\n");
    fprintf(fid, "%.2f %.2f %.2f setrgbcolor\n", s->color.red, s->color.green, s->color.blue);
    if (s->line_width != DEFAULT) 
        fprintf(fid, "%.2f setlinewidth\n", s->line_width);
    
    if (s->line != DEFAULT) 
        fprintf(fid, s->line == dashed ? "[3 3] 0 setdash\n" : "");
    
    if (s->draw != DEFAULT)
        fprintf(fid, s->draw == stroke && l->type != irregular_polygon_t ? "stroke\n" : "fill\n");
    else
        fprintf(fid, "stroke\n");
}

/* Sets default Entity style */
void u_export_eps_entity_style_reset (FILE * fid) {
    fprintf(fid, "\n%% Come back default Entity style\n");
    fprintf(fid, "%.2f %.2f %.2f setrgbcolor\n", 0.0, 0.0, 0.0);
    fprintf(fid, "%.2f setlinewidth\n", 1.1);
    fprintf(fid, "[1 0] 0 setdash\n");
}

void u_export_eps_xy_plane (FILE * fid, Canvas * canvas, EntityStyle * s) {
    fprintf(fid, "\n%% X-Y Plane\n");
    fprintf(fid, "newpath\n");

    if (canvas != NULL) {
        fprintf(fid, "%.2f %.2f moveto\n", canvas->start.x, 0.0);
        fprintf(fid, "%.2f %.2f lineto\n", canvas->end.x, 0.0);
        fprintf(fid, "%.2f %.2f moveto\n", 0.0, canvas->start.y);
        fprintf(fid, "%.2f %.2f lineto\n", 0.0, canvas->end.y);
    }
    else {
        fprintf(fid, "%.2f %.2f moveto\n", -1000.0, 0.0);
        fprintf(fid, "%.2f %.2f lineto\n", 1000.0, 0.0);
        fprintf(fid, "%.2f %.2f moveto\n", 0.0, -1000.0);
        fprintf(fid, "%.2f %.2f lineto\n", 0.0, 1000.0);
    }

    if (s == NULL) {
        fprintf(fid, "\n%% Set Entity style\n");
        fprintf(fid, "%.2f %.2f %.2f setrgbcolor\n", 1.0, 0.0, 0.0);
        fprintf(fid, "%.2f setlinewidth\n", 0.5);
        fprintf(fid, "[3 3] 0 setdash\n");
    }
}

//! NOT IMPLEMENTED YET
void u_export_eps_fiducial (FILE * fid, Canvas * canvas, Point2D * center) {
    double r1, r2, d;  

    /* Calculate the size of fidicual according to canvas size */
    if (canvas != NULL) {
        r1 = u_min(canvas->end.x, canvas->end.y) / 100.0; 
        r2 = 2 * r1; 
    }
    else {
        
    }

    fprintf(fid, "\n%% Fiducial\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "1.00 0.00 0.00 setrgbcolor\n");
    fprintf(fid, "0.70 setlinewidth\n");

    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n stroke\n", center->x, center->y, r1, 0.0, 360.0);
    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n stroke\n", center->x, center->y, r2, 0.0, 360.0);
    
    fprintf(fid, "%.2f %.2f moveto\n", center->x - d, center->y);
    fprintf(fid, "%.2f %.2f rlineto\n", 2 * d, 0.0);
    fprintf(fid, "%.2f %.2f moveto\n", center->x, center->y - d);
    fprintf(fid, "%.2f %.2f rlineto\n", 0.0, 2 * d);
    fprintf(fid, "stroke");
}

void u_export_eps_point (FILE * fid, Point2D * e) {
    fprintf(fid, "\n%% Point\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f 1.00 0.00 360.00 arc\n", e->x, e->y);
}

void u_export_eps_focus_point (FILE * fid, Point2D * e) {
    //! NOT IMPLEMENTED YET
}

void u_export_eps_circle (FILE * fid, Circle * e) {
    fprintf(fid, "\n%% Circle\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n", e->center.x, e->center.y, e->radius, e->start_angle, e->end_angle);
}

void u_export_eps_ellipse (FILE * fid, Ellipse * e) {
    double  scale = e->radius_y / e->radius_x,
            radius = u_min(e->radius_x, e->radius_y);   

    fprintf(fid, "\n%% Ellipse\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "1 %.2f scale\n", scale);
    fprintf(fid, "%.2f %.2f %.2f %.2f %.2f arc\n", e->center.x, e->center.y, radius, 0.0, 360.0);
}

void u_export_eps_triangle (FILE * fid, Triangle * e) {
    fprintf(fid, "\n%% Triangle\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->corner1.x, e->corner1.y);
    fprintf(fid, "%.2f %.2f lineto\n", e->corner2.x, e->corner2.y);
    fprintf(fid, "%.2f %.2f lineto\n", e->corner3.x, e->corner3.y);
    fprintf(fid, "closepath\n");
}

void u_export_eps_rectangle (FILE * fid, Rectangle * e) {
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
void u_export_eps_line (FILE * fid, PointList * e) {
    fprintf(fid, "\nnewpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->point.x , e->point.y);
    e = e->next;
    
    while (e != NULL) {
        fprintf(fid, "%.2f %.2f lineto\n", e->point.x, e->point.y);
        e = e->next;
    }
}

void u_export_eps_regular_polygon (FILE * fid, RegularPolygon * e) {
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

void u_export_eps_text (FILE * fid, Text * e) {
    fprintf(fid, "\n%% Text\n");
    fprintf(fid, "newpath\n");
    fprintf(fid, "%.2f %.2f moveto\n", e->point.x , e->point.y);
    u_export_eps_text_style(fid, e->style);
    fprintf(fid, "(%s) show\n", e->text);
}

/*********************************************************************************
 * Memory Deletion
*********************************************************************************/

/* Deletes the memory allocated for entity list */
void u_delete_elist (Entity ** elist, int size) {
    int i;
    if (elist != NULL) {
        for (i = 0; i < size; ++i) {
            if (elist[i] != NULL && elist[i] != DELETED) {
                /* Free entity data */
                switch (elist[i]->label->type) {
                    case line_t:
                    case polyline_t:
                    case irregular_polygon_t:
                        u_free_point_list(elist[i]->data);
                        break;
                    case point_t:
                    case regular_polygon_t:
                    case triangle_t:
                    case rectangle_t:
                    case circle_t:
                    case arc_t:
                    case ellipse_t:
                        free(elist[i]->data);
                        break;
                    case text_t:
                        u_free_text(elist[i]->data);       
                        break;
                }
                /* Free style and label of en,tity, then delete itself */
                free(elist[i]->style);
                free(elist[i]->label->name);
                free(elist[i]->label);
                free(elist[i]);
            }
        }
        free(elist);
    }
}

/* Deletes the memory allocated for label list, since label's are already freed just free the list */
void u_delete_llist (LabeList * llist) {
    if (llist != NULL) {
        u_delete_llist(llist->next);
        free(llist);
    }
}

/* Takes the root CAD and deletes all the content inside of it */
void c2d_delete (CAD2D * cad) {
    int i;
    
    if (cad != NULL) {
        /* First delete it's child hierarchies */
        if (cad->hierarchy != NULL) {
            for (i = 0; i < cad->hierarchy->size; ++i)
                if (cad->hierarchy->child[i] != NULL && cad->hierarchy->child[i] != DELETED)
                    c2d_delete(cad->hierarchy->child[i]->cad);
            
            printf("Delete %s\n", cad->hierarchy->name);
            free(cad->hierarchy->name);
            free(cad->hierarchy->child);
            free(cad->hierarchy);
        }
        
        /* Lastly free given cad and it's entities */
        printf("Delete elist: ");
        u_delete_elist(cad->elist, cad->elist_size);
        printf("DONE\n");

        printf("Delete llist: ");
        u_delete_llist(cad->llist);
        printf("DONE\n");

        free(cad);
    }
}

/*********************************************************************************
 * Other
*********************************************************************************/

/* Produce a hash-code according to given key value */
int u_create_hash_code (char * key, int size) {
    int i;
    int code = 0;
    int p, q = 97;

    /*  set the p as largest prime smaller than size.
        in case of uninitialized hash table use it's INIT size */
    if (size == 0)
        size = INIT_HASH;
    p = u_find_close_prime(size);
    
    for (i = 0; key[i] != '\0'; ++i)
        code = (code * q + key[i]) % p;
    return code;
}

/* Returns if given block at hash table is empty or not */
int u_is_empty (void * p) {
    return p == NULL || p == DELETED;
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

char * u_deci_to_hexa (int deci) {
    char tmp[20], * hexa;
    int r, n = 0, i;

    do {
        r = deci % 16;
        tmp[n++] = r < 10 ? r + '0' : r - 10 + 'A';
        deci /= 16;
    } while (deci > 0);

    hexa = (char *) calloc(n + 1, sizeof(char));
    if (hexa != NULL) {
        for (i = 0, n -= 1; n >= 0; ++i, --n)
            hexa[i] = tmp[n];
        hexa[i] = '\0';
    }

    return hexa;
}

double u_find_hypotenuse (double x, double y) {
    return sqrt(x*x + y*y);
} 

double  u_get_euclidean_dist (Point2D * p1, Point2D * p2) {
    return u_find_hypotenuse(p1->x - p2->x, p1->y - p2->y);
}