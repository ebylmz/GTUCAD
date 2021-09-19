// ! label için point olur birde textfield olur belkide hashcode olur

/*
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
    u_insert_hash_table(cad->list, e);
}

*/

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

typedef enum {
    white, silver, gray, black, red, maroon, yellow, 
    olive, lime, green, aqua, teal, blue, navy,
    fuchsia, purple
} ColorPalette;

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