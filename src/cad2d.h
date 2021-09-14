#ifndef cad2d
#define cad2d

// ! any basic 2D shape can have color, thickness and line style 

typedef enum EntityType {
    point, line, spline, polyline, polygon,
    rectangle, circle, arc, ellipse, text, image
} EntityType;

/* A data structure to hold coordinates of 2D points */
typedef struct Point2D {
    double x, y;
    // Label label;
} Point2D;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
/* Holds the hierarchy information for CAD entities. */
typedef struct Hierarchy {
    struct CAD2D * child, *next;
} Hierarchy;


// ! tipini identy etmem de gerek
// ! point olur birde textfield olır belkide hashcode olur

/* A label for a given CAD entity (line, arch, circle...) */
typedef struct Label {
    EntityType type; /* identifies the type of the cad entity */
    void * entity;
    char * text; 
} Label;

typedef struct Canvas{
    Point2D start;      /* (-width/2, -height/2) */
    Point2D end;        /* ( width/2,  height/2) */
} Canvas;

/* ListLabel holds all the cad entities (each of them are unique label) as linked list */
typedef struct ListLabel {
    Label * label;
    struct ListLabel * next;
} ListLabel;

/***************************************************************************************************
// ? CAD 2D
***************************************************************************************************/

// ! cad2d data yapısı farklı turden elemanları tutabiliyor olması gerek
// ! muhtemelen void pointerlı bir linked list olacak list yada agac olacak
// ! every cad entity belongs to a hierarchy
/* The data structure to hold the current CAD content */
typedef struct CAD2D {
    Canvas * canvas;
    Hierarchy * h;    
    ListLabel * label_list;
} CAD2D;

/* point, line, spline, polyline, polygon, rectangle, circle, arc, ellipse, text, image */
typedef struct Line {
    Point2D start, end;
    // Label label;
} Line;

typedef struct Arc {
    Point2D center;
    double radius;
    double startAngle, endAngle;
} Arc; 

typedef struct Circle {
    Point2D center;
    double radius;
} Circle;

typedef struct Polyline {
    Point2D * point;
    struct Polyline * next;
} Polyline;

typedef struct Text {
    Point2D * point;
    char * text_field;
} Text;

typedef struct Rectangle {
    Point2D corner1, corner2;
} Rectangle;


CAD2D * c2d_start_wh (double width, double height);
CAD2D * c2d_start();
Label * c2d_add_line(CAD2D * cad, Point2D * start, Point2D * end);
Label * c2d_add_point_xy (CAD2D * cad, double x, double y);
void c2d_export (CAD2D * cad, char * file_name, char * options);
void c2d_export_eps (CAD2D * cad, char * file_name);

#endif

