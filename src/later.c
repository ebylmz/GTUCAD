#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cad2d.h"

/*
    case point_t:
        break;
    case xy_plane:
        break;
    case line_t:
        break;
    case polyline_t:
        break;
    case irregular_polygon_t:
        break;
    case regular_polygon_t:
        break;
    case triangle_t:
        break;
    case rectangle_t:
        break;
    case circle_t:
        break;
    case arc_t:
        break;
    case ellipse_t:
        break;
    case text_t:
        break;
    default:
        printf("NOT IMPLEMENTED YET\n");
*/

/* Returns the closest distance between point and polyline */
/*
double u_measure_point_polyline (Point2D * point, PointList * pline) {
    double m;
    double d;

    m.start = *point;
    m.distance = u_get_euclidean_dist(point, &pline->point);

    pline = pline->next;
    //! THIS ALGO IS WRONG
    //! Use perpendicular line
    while (pline != NULL) {
        d = u_get_euclidean_dist(point, &pline->point);
        if (d < m.distance) {
            m.distance = d;
            m.end = pline->point;
        }
        pline = pline->next;
    }

    return m;
}
*/

/* Measures the distance between center of given two entity */
/*
double c2d_measure_center2D (CAD2D * cad, Label * ls, Label * lt) {
    EntityInfo * s_info = c2d_find_entity(cad, ls), * t_info = c2d_find_entity(cad, lt);
    Point2D c1, c2;
    double m;

    if (s_info != NULL && t_info != NULL) {
        c1 = c2d_get_center2D(s_info->entity->data);
        c2 = c2d_get_center2D(t_info->entity->data);

        m.start = c1;
        m.end = c2;
        m.distance = u_get_euclidean_dist(&c1, &c2);
    }    
    else 
        m.distance = -1;
    //! Check if distance is valid
    
    free(s_info);
    free(t_info);

    return m;
}

*/

/*
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ...) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h) {}
Label * c2d_add_<BASIC>?(CAD2D * cad, ..., const Hierarchy * h, const Label * l) {}
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