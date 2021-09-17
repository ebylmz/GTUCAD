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


Node ** create_hash_table () {
    return (Node **) calloc(HSIZE, sizeof(Node *));
}


void insert_entity_list (EntityList ** l, Entity * e) {

    while (*l != NULL) l = &(*l)->next;

    *l = (EntityList *) malloc(sizeof(EntityList));

    if (*l != NULL) {
        (*l)->entity = e;
        (*l)->next = NULL;
    }
}


Label * create_label (EntityType type, char * name) {
    Label * l = (Label *) malloc(sizeof(Label));

    if (l != NULL) {
        l->name = name;
        l->type = type;
    }
    // ! NOT IMPLEMENTED YET: Be sure given parameters can produce unique label to do that we need root CAD
    // ! if user do not give a name for label set something sentinal value  
    return l;
}

Entity * c2d_create_entity (EntityType type, void * data, Label * label) {
    Entity * e = malloc(sizeof(Entity));

    if (e != NULL) {
        e->data = data;
        e->label = label;
    }

    return e;
}

/*
Entity * c2d_create_entity (EntityType type) {
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
            // ! Is this needed
            case object:
                e->data = (CAD2D *) malloc(sizeof(CAD2D));
        }
           
        if (e->data != NULL) {
            e->label = create_label(type, "");
            
            if (e->label == NULL) {
                free(e->data);
                free(e);
                e = NULL;
            }
        }
        else {
            free(e);
            e = NULL;
        }
    }
    return e;
}
*/
