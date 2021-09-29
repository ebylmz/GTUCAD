#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cad2d.h"
#define FAIL -1
#define TRY -2

// ! label için point olur birde textfield olur belkide hashcode olur



/* In case of matching returns index of the hierarchy, o.w. returns FAIL(-1) */
int u_check_hierarchy (Hierarchy * root, Hierarchy * h) {
    int i, r, hcode;

    /* First hierarchy is unique since list initiliazed yet */
    if (root != NULL || root->size == 0)
        r = h->hash_code;
    else {
        hcode = h->hash_code;
        printf("%d\n", hcode);

        /* use hash-code as index of the unique hierarchy */
        for (i = 0, r = TRY; i < root->size && r == TRY; ++i) {
            if (hcode >= root->size)
                hcode %= root->size;

            /* same hash-code is a sign for same keys */
            if (root->child[hcode] != NULL) {
                if (strcmp(root->child[hcode]->name, h->name) == 0)
                    r = FAIL;
                else
                    ++hcode;
            }
            else 
                r = hcode;
        }
        //! ALSO CHECK THE CHILD OF THE CHILD HIERARCHY
        /* If there isn't match yet, continue with children */
        for (i = 0; i < root->size && r != FAIL; ++i)
            if (root->child[i] != NULL)
                r = u_find_hierarchy(h->cad ,h); //! delete h->cad
    }
    printf("u_find_hierarchy() returns %d\n", r);
    return r;
}





/*
1 0 0 		 setrgbcolor	red
0 1 0 		 setrgbcolor	green
0 0 1 		 setrgbcolor	dark blue
0 1 1 		 setrgbcolor	light blue
1 0 1 		 setrgbcolor	magenta
1 1 0 		 setrgbcolor	yellow
1 1 1 		 setrgbcolor	white
0 0 0 		 setrgbcolor	black
1.0 0.7 0.0  setrgbcolor	orange
0.7 0.3 1.0  setrgbcolor	purple
0.7 0.3 0.0  setrgbcolor	brown
0.0 0.5 0.0  setrgbcolor	dark green
*/

/*
typedef enum {
    white, silver, gray, black, red, maroon, yellow, 
    olive, lime, green, aqua, teal, blue, navy,
    fuchsia, purple
} ColorPalette;
*/

/*
    White 	#FFFFFF 	rgb(255, 255, 255)
	Silver 	#C0C0C0 	rgb(192, 192, 192)
	Gray 	#808080 	rgb(128, 128, 128)
	Black 	#000000 	rgb(0, 0, 0)
	Red 	#FF0000 	rgb(255, 0, 0)
	Maroon 	#800000 	rgb(128, 0, 0)
	Yellow 	#FFFF00 	rgb(255, 255, 0)
	Olive 	#808000 	rgb(128, 128, 0)
	Lime 	#00FF00 	rgb(0, 255, 0)
	Green 	#008000 	rgb(0, 128, 0)
	Aqua 	#00FFFF 	rgb(0, 255, 255)
	Teal 	#008080 	rgb(0, 128, 128)
	Blue 	#0000FF 	rgb(0, 0, 255)
	Navy 	#000080 	rgb(0, 0, 128)
	Fuchsia #FF00FF 	rgb(255, 0, 255)
	Purple 	#800080 	rgb(128, 0, 128)
*/
       
	    /*  u_check_unique_hierarchy() returns the next empty place
            based on it's hash-func regarding the size of the array.
            So be sure we are not exceeding the size                */

int u_link_hierarchy (Hierarchy * child, Hierarchy * parent) {
    Hierarchy ** tmp;
    int i = 0, n, r = u_check_unique_hierarchy(child);

    if (r == FAIL)  //!!!!! Is it okay?
        printf("'%s' named Hierarchy already exist\n", child->name);
    else {
        if (r >= parent->size) {
            n = parent->size;

            if (parent->size == 0)
                parent->size = INIT_HASH;
            else
                parent->size *= 2;

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
        }
    }

    return r;
}

//! NOT TESTED YET
Hierarchy * u_find_hierarchy (CAD2D * cad, const Hierarchy * h) {
    int i, hcode = u_create_hash_code(h->name, h->size);
    Hierarchy * r = NULL;
    Hierarchy * trav;

    if (cad->hierarchy != NULL) {
        trav = cad->hierarchy;

        /* Go to the root Hierarchy and check up to down */
        /*
        while (trav->parent != NULL)
            trav = trav->parent;
        */
        
        for (i = 0; i < cad->hierarchy->size && r == NULL; ++i)
            
            /* look the child's of root hierarchy */
            for (i = 0; i < cad->hierarchy->size && r == NULL; ++i) {
                if (hcode >= cad->hierarchy->size)
                    hcode %= cad->hierarchy->size;

                if (cad->hierarchy->child[(hcode + i) % cad->hierarchy->size] == h)
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