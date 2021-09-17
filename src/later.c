// * permit the user to create new types. rectangle + circle = cirec use as postscript func

/*  
    * cad2d data yapısı farklı turden elemanları tutabiliyor olması gerek
    * muhtemelen void pointerlı bir linked list olacak list yada agac olacak
*/

// ! label için point olur birde textfield olur belkide hashcode olur
//! Be sure label's are unique to provide this use hash table

//* Hierarchy * c2d_create_hierarchy?(CAD2D * cad)
//? Convert given CAD to hierarchy
//* Hierarchy * c2d_create_hierarchy?(CAD2D * cad, Hierarchy * parent):
//? convert given cad to hierarchy and link the child of given parent Hierarchy  

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

/*
Node ** create_hash_table () {
    return (Node **) calloc(HSIZE, sizeof(Node *));
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
*/

