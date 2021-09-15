#ifndef cad2d
#define cad2d

/*********************************************************************************
 * Fundamental Structures
*********************************************************************************/

typedef enum EntityType {
    point, line, spline, polyline, polygon,
    rectangle, circle, arc, ellipse, text, image
} EntityType;

// ! any basic 2D shape can have color, thickness and line style 
// ! tipini identy etmem de gerek
// ! point olur birde textfield olur belkide hashcode olur

/* A label for a given CAD entity (line, arch, circle...) */
typedef struct Label {
    EntityType type; /* identifies the type of the cad entity */
    char * text; 
} Label;

typedef struct Entity {
    void * entity_data;
    Label * label;
} Entity;

/* EntityList holds all the CAD entities (each of them has unique label) as linked list */
typedef struct EntityList {
    Entity * entity;
    struct EntityList * next;
} EntityList;

/* A data structure to hold coordinates of 2D points */
typedef struct Point2D {
    double x, y;
    // Label label;
} Point2D;

typedef struct Canvas{
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
 * Basic CAD Entities
*********************************************************************************/

/* point, line, spline, polyline, polygon, rectangle, circle, arc, ellipse, text, image */
typedef struct Line {
    Point2D start, end;
    // Label label;
} Line;

typedef struct Arc {
    Point2D center;
    double radius;
    double start_angle, end_angle;
} Arc; 

typedef struct Circle {
    Point2D center;
    double radius;
    double start_angle, end_angle;
} Circle;

typedef struct Polyline {
    Point2D point;
    struct Polyline * next;
} Polyline;

typedef struct Text {
    Point2D point;
    char * text_field;
} Text;

typedef struct Rectangle {
    Point2D corner1, corner2;
} Rectangle;

CAD2D * c2d_start_wh (double width, double height);
CAD2D * c2d_start();
Label * c2d_add_line(CAD2D * cad, Point2D start, Point2D end);
Label * c2d_add_point_xy (CAD2D * cad, double x, double y);
void c2d_export (CAD2D * cad, char * file_name, char * options);
void c2d_export_eps (CAD2D * cad, char * file_name);
Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle);

#endif

