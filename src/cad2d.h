#ifndef cad2d
#define cad2d

#define INIT_HASH   10              /* Size of hash table                   */
#define PRIME       17              /* Prime number for hash function       */
#define DELETED     (void *) -3     /* Hash-table value for deleted places  */
#define TRY         -2
#define FAIL        -1
#define DEFAULT     -1              /* Default design chocie                */

/*********************************************************************************
 * Fundamental Structures
*********************************************************************************/

typedef enum {
    point_t, line_t, polyline_t, regular_polygon_t, irregular_polygon_t,
    triangle_t, rectangle_t, circle_t, arc_t, ellipse_t, text_t
} EntityType;

typedef struct {
    double red, green, blue;
} RGBColor;

//! NOT USED 
typedef enum {
    red, green, green_dark, blue, blue_light, magenta,
    yellow, white, black, orange, purple, brown 
} Color;

typedef enum {
    Helvetica, Courier, Times, Coronet, Symbol, Albertus, Arial, Bodoni
} FontStyle;

typedef enum {
    dashed, solid
} LineStyle;

typedef enum  {
    stroke, fill
} DrawStyle;

typedef enum {
    lw_small = 1, lw_medium = 2, lw_large = 3
} LineWidth;

typedef enum {
    fs_xsmall = 10, fs_small = 20, fs_medium = 40, fs_large = 80
} FontScale;

//! use -1 for default style choices
typedef struct {
    LineStyle line;
    RGBColor color;
    DrawStyle draw;
    double line_width;
} EntityStyle;

typedef struct {
    FontStyle font;
    double scale; 
    RGBColor color;
} TextStyle;

typedef struct Label {
    EntityType type;    /* identifies the type of the cad entity */
    char * name;        /* name of the object */
    int hash_code;      /* labels are should be unique to do that we use hashing */
} Label;

typedef struct Entity {
    Label * label;      /* unique label to identify cad entities */
    void * data;        /* drawing data like radius, width... */
    EntityStyle * style;   
} Entity;

typedef struct Hierarchy {
    struct CAD2D * cad;
    char * name;                    /* name of the hieararchy */
    int hash_code;      
    struct Hierarchy * parent;
    struct Hierarchy ** child;      /* array of child hierarchies */
    int size;
} Hierarchy;

/* Point2D can also treated as Line, Polyline, Polygone and Spline */
typedef struct Point2D {
    double x, y;
} Point2D;

typedef struct Canvas {
    Point2D start;      /* (-width/2, -height/2) */
    Point2D end;        /* ( width/2,  height/2) */
} Canvas;

typedef struct LabeList {
    Label * label;
    struct LabeList * next;     
} LabeList;

typedef struct CAD2D {
    Canvas * canvas;
    LabeList * llist;
    int elist_size;         
    Entity ** elist;            /* Entity list keeps entities by hashing */
    Hierarchy * hierarchy;      /* hierarchy data of root CAD (this CAD) */   
} CAD2D;

typedef struct EntityInfo {
    Entity * entity;            /* Entity itself                    */
    CAD2D * cad;                /* Container cad of entity          */
    int index;                  /* index inside of the entity list  */
} EntityInfo;

/*********************************************************************************
 * Basic CAD Entities:
 * point, line, polyline, polygon, rectangle, circle, arc, ellipse, text
*********************************************************************************/

/*  Line, IrregularPolygon and Polyline are represented as PointList.
    Implementation are same but interpretation are changes from type to type */
typedef struct PointList {
    Point2D point;
    struct PointList * next;
} PointList;

/* To measure the distance between entities */
typedef struct Measurement {
    //! NOT IMPLEMENTED YET
    Point2D start, end;     /* Two points for line */
    double distance;
    EntityStyle * style;    /* Style for line */
} Measurement;

typedef struct Circle {
    Point2D center;
    double radius;
    double start_angle, end_angle;  
} Circle;

typedef struct Text {
    Point2D point;  /* start point of text */
    char * text;
    TextStyle * style;
} Text;

typedef struct Triangle {
    Point2D corner1, corner2, corner3;
} Triangle;

typedef struct Rectangle {
    Point2D corner1, corner2;   /* Takes two opposite corners as start and end point */
} Rectangle;

typedef struct RegularPolygon {
    int n;                  /* the number of sides      */
    double out_radius;      /* radius of outer circle   */
    Point2D center;
} RegularPolygon;

typedef struct Ellipse {
    Point2D center;
    double radius_x, radius_y;
} Ellipse;

/*********************************************************************************
 * Function Definitions
*********************************************************************************/
CAD2D * c2d_start ();
CAD2D * c2d_start_wh (double width, double height);
CAD2D * c2d_start_wh_hier (double width, double height, Hierarchy * h);

Hierarchy * c2d_create_hierarchy (CAD2D * cad);
Hierarchy * c2d_create_hierarchy_parent (CAD2D * cad, Hierarchy * parent);
void c2d_delete_hierarchy (Hierarchy * h);

Point2D * c2d_create_point (double x, double y);
void c2d_set_point (Point2D * p, double x, double y);

Label * c2d_create_label (CAD2D * cad, EntityType type, char * name);

Hierarchy * c2d_get_root_hierarchy (Hierarchy * h);
CAD2D * c2d_get_root_cad (CAD2D * cad);
EntityInfo * c2d_find_entity (CAD2D * root, Label * l); 
EntityStyle * c2d_set_entity_style (CAD2D * cad, Label * label, LineStyle l, RGBColor c, DrawStyle d, double w);
TextStyle * c2d_set_text_style (CAD2D * cad, Label * label, FontStyle f, RGBColor c, double s);
void c2d_set_rgb (RGBColor * c, double red, double green, double blue);
void c2d_remove_entity (CAD2D * cad, Label ** l);
Point2D c2d_get_center2D (Entity * e);
PointList * c2d_create_point_list_p (Point2D p);

Label * c2d_add_point_xy (CAD2D * cad, double x, double y);
Label * c2d_add_point_p (CAD2D * cad, Point2D p);
Label * c2d_add_line(CAD2D * cad, Point2D start, Point2D end);
Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle);
Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius);
Label * c2d_add_ellipse(CAD2D * cad, Point2D center, double radius_x, double radius_y);
Label * c2d_add_triangle (CAD2D * cad, Point2D corner1, Point2D corner2, Point2D corner3);
Label * c2d_add_rectangle (CAD2D * cad, Point2D corner1 , Point2D corner2);
// Label * c2d_add_polyline (CAD2D * cad, Point2D * p);
Label * c2d_add_polyline (CAD2D * cad, Point2D * p, int size);
Label * c2d_add_irregular_polygon (CAD2D * cad, Point2D * p, int size);
Label * c2d_add_regular_polygon (CAD2D * cad, int n, Point2D center, double out_radius);
Label * c2d_add_text (CAD2D * cad, Point2D p, char * text);

void c2d_export (CAD2D * cad, char * file_name, char * options);
CAD2D * c2d_import (char * file_name, char * options);
void c2d_delete (CAD2D * cad);

#endif

