// * TODO:

// * permit the user to create new types. rectangle + circle = cirec use as postscript func


#ifndef cad2d
#define cad2d

#define HSIZE 20 /* Size of hash table */

/*********************************************************************************
 * Fundamental Structures
*********************************************************************************/

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

// ! point olur birde textfield olur belkide hashcode olur
/* the user editable name of this object, it is an arbitrary UTF8 string. */
/* A label for a given CAD entity (line, arch, circle...) */
typedef struct Label {
    EntityType type;    /* identifies the type of the cad entity */
    char * name;        /* name of the object */
    int index;          /* index of the this entity in entity_list */
} Label;

//! Be sure label's are unique
/* Entity can be basic entity or more complex like CAD2D */
typedef struct Entity {
    void * data;        /* Start Point, radius, angle etc */
    Label * label;      /* unique label to identify cad entities */
} Entity;

/* EntityList holds all the CAD entities as linked list */
//!     double c2d_measure(CAD2D * cad, const Label * ls, const Label * lt)
//!     entityList can be hash table we can reach any data constant time in this way
/*
typedef struct EntityList {
    Entity * entity;
    struct EntityList * next;
} EntityList;
*/

typedef struct Size {
    int cur, max;
} Size;

//* Label * c2d_add_<BASIC>? (CAD2D *, ..., const Label *, const Hierarchy *) 
//* Purpose of function: creates CAD entity and returns it's unique label
//*    
//! So we need a data structure to keep cad entites in CAD not Hierarchy or Label

//* Hierarchy * c2d_create_hierarchy?(CAD2D * cad)
//? Convert given CAD to hierarchy
//* Hierarchy * c2d_create_hierarchy?(CAD2D * cad, Hierarchy * parent):
//? ... and link the child of given parent Hierarchy  

/* Holds the hierarchy information for CAD entities. */
typedef struct Hierarchy {
    CAD2D * cad;
    struct Hierarchy * parent;
    struct Hierarchy ** children;   /* All the children hierarchies */
    Size size;
} Hierarchy;

/* A data structure to hold coordinates of 2D points */
typedef struct Point2D {
    double x, y;
    struct Point2D * next;
} Point2D;

typedef struct Canvas {
    Point2D start;      /* (-width/2, -height/2) */
    Point2D end;        /* ( width/2,  height/2) */
} Canvas;

// ! cad2d data yapısı farklı turden elemanları tutabiliyor olması gerek
// ! muhtemelen void pointerlı bir linked list olacak list yada agac olacak
// ! every cad entity belongs to a hierarchy

/* The data structure to hold the current CAD content */
typedef struct CAD2D {
    Canvas * canvas;
    void ** entity_list;
    Size entit_size;
    Hierarchy * hierarchy;  /* to reach all the CAD entities */    
} CAD2D;

/*********************************************************************************
 * Basic CAD Entities:
 * point, line, spline, polyline, polygon, rectangle, circle, arc, ellipse, text, image
*********************************************************************************/

typedef struct Line {
    Point2D start, end;
    Style style;
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

// ! We actualy do not need a new data type for polyline, we can add * next inside of the Point2D
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

/*********************************************************************************
 * Function Definitions
*********************************************************************************/

CAD2D * c2d_start_wh (double width, double height);
CAD2D * c2d_start();
Label * c2d_add_line(CAD2D * cad, Point2D start, Point2D end);
Label * c2d_add_point_xy (CAD2D * cad, double x, double y);
void c2d_export (CAD2D * cad, char * file_name, char * options);
void c2d_export_eps (CAD2D * cad, char * file_name);
Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle);

#endif

