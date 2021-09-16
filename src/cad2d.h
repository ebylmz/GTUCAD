#ifndef cad2d
#define cad2d

/*********************************************************************************
 * Fundamental Structures
*********************************************************************************/
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
typedef enum {
    point, line, spline, polyline, polygon,
    rectangle, circle, arc, ellipse, text, image
} EntityType;

typedef enum {
    white, silver, gray, black, red, maroon, yellow, 
    olive, lime, green, aqua, teal, blue, navy,
    fuchsia, purple
} ColorPalette;

typedef enum  {stroke, fill} DrawStyle;

/* RGB color scheme */
typedef struct RGBColor {
    double red, green, blue;
} RGBColor;

typedef struct LineStyle {
    // ! NOT IMPLEMENTED YET:
} LineStyle;

typedef struct Style {
    // ! NOT IMPLEMENTED YET: Color choice
    LineStyle line_style;
    double thickness;
    int filled;
} Style;

// ! tipini identy etmem de gerek
// ! point olur birde textfield olur belkide hashcode olur

// * permit the user to create new types for exp. rectangle + circle = cirec use as postscript function
/* the user editable name of this object, it is an arbitrary UTF8 string. */
/* A label for a given CAD entity (line, arch, circle...) */
typedef struct Label {
    EntityType type; /* identifies the type of the cad entity */
    char * text; 
} Label;

typedef struct Entity {
    void * entity_data; /* Start Point, radius, angle etc */
    Label * label;      /* unique label to identify cad entities */
} Entity;

/* EntityList holds all the CAD entities as linked list */
typedef struct EntityList {
    Entity * entity;
    struct EntityList * next;
} EntityList;

/* A data structure to hold coordinates of 2D points */
typedef struct Point2D {
    double x, y;
    // Label label;
} Point2D;

typedef struct Canvas {
    Point2D start;      /* (-width/2, -height/2) */
    Point2D end;        /* ( width/2,  height/2) */
} Canvas;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
/* Holds the hierarchy information for CAD entities. */
typedef struct Hierarchy {
    struct CAD2D * child,
                 * next;
} Hierarchy;

// ! cad2d data yapısı farklı turden elemanları tutabiliyor olması gerek
// ! muhtemelen void pointerlı bir linked list olacak list yada agac olacak
// ! every cad entity belongs to a hierarchy

/* The data structure to hold the current CAD content */
typedef struct CAD2D {
    Canvas * canvas;
    Hierarchy * h;    
    EntityList * entity_list;
} CAD2D;

/*********************************************************************************
 * Basic CAD Entities:
 * point, line, spline, polyline, polygon, rectangle, circle, arc, ellipse, text, image
*********************************************************************************/

typedef struct Line {
    Point2D start, end;
    Style style;
    // Label label;
} Line;

typedef struct Arc {
    Point2D center;
    double radius;
    double start_angle, end_angle;
    Style style;
} Arc; 

typedef struct Circle {
    Point2D center;
    double radius;
    double start_angle, end_angle;
    Style style;
} Circle;

typedef struct Polyline {
    Point2D point;
    struct Polyline * next;
    Style style;
} Polyline;

typedef struct Text {
    Point2D point;
    char * text_field;
    Style style;
} Text;

typedef struct Rectangle {
    Point2D corner_down_left, corner_top_right;
    Style style;
} Rectangle;

typedef struct Spline {
    // ! NOT IMPLMENTED YET
} Spline;

typedef struct Polygon {
    // ! NOT IMPLMENTED YET
} Polygon;

typedef struct Ellipse{
    // ! NOT IMPLMENTED YET
} Ellipse;

typedef struct Image {
    // ! NOT IMPLMENTED YET
} Image;

CAD2D * c2d_start_wh (double width, double height);
CAD2D * c2d_start();
Label * c2d_add_line(CAD2D * cad, Point2D start, Point2D end);
Label * c2d_add_point_xy (CAD2D * cad, double x, double y);
void c2d_export (CAD2D * cad, char * file_name, char * options);
void c2d_export_eps (CAD2D * cad, char * file_name);
Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle);

#endif

