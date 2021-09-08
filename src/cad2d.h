#ifndef cad2d
#define cad2d

/* A data structure to hold coordinates of 2D points */
typedef struct Point2D {
    double x, y;
    struct Point2D * next; 
} Point2D;


/* Holds the hierarchy information for CAD entities. */
typedef struct Hierarchy {
    char * h_name;
    struct CAD2D * parent,
                 * child,
                 * next_child;
} Hierarchy;


/* https://youtu.be/3iVj_rH58ys?list=PLuid2q5iknOBtKeJ-SwtVPEpTjUYOXSp2&t=816 */
/* A label for a given CAD entity (line, arch, circle...) */
typedef struct Label {
    Point2D point;  /* every shape consist of points that will connected with lines */
    char * text; 
    //  int hashcode;   /* label's should be unique */
} Label;


typedef struct Canvas{
    Point2D start;      /* (-width/2, -height/2) */
    Point2D end;        /* ( width/2,  height/2) */
} Canvas;


/* CadEntity holds all the cad entities (each of them are unique label) as linked list */
typedef struct CadEntity {
    Label current;
    struct CadEntity * next;
} CadEntity;


/* The data structure to hold the current CAD content */
typedef struct CAD2D {
    Canvas * canvas;
    Hierarchy h;   /* every cad entity belongs to a hierarchy */
    CadEntity * entity;
} CAD2D;


CAD2D * c2d_start_wh (double width, double height);
CAD2D * c2d_start();
Label * c2d_add_line(CAD2D * cad, Point2D * start, Point2D * end);
Label * c2d_add_point_xy (CAD2D * cad, double x, double y);
void c2d_export (CAD2D * cad, char * file_name, char * options);
void c2d_export_eps (CAD2D * cad, char * file_name);



/*
typedef struct Color {
    double red, green, blue;
} Color;
*/ 
/*
    typedef struct Design {
    Color color;
    double Thickness;
    LineStyle line_style;
} Design;
*/
/*
typedef struct Geometry {
    Label *
} Geometry;

*/
#endif

