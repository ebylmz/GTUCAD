/**
 * @file    cad2d.h
 * @author  Emirkan Burak Yilmaz (ebylmz17@gmail.com)
 * @brief   CAD2D Library Header
 * @version 0.1
 * @date    2021-10-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef cad2d
#define cad2d

#define INIT_HASH   10              /* Size of hash table                   */
#define PRIME       17              /* Prime number for hash function       */
#define DELETED     (void *) -3     /* Hash-table value for deleted places  */
#define TRY         -2
#define FAIL        -1
#define DEFAULT     -1              /* Default design chocie                */
#define PI          3.141592654 

/* Available export/import options */
typedef enum {
    gtucad, eps  
} ExImOption; 

typedef enum {
    point_t, line_t, polyline_t, regular_polygon_t, irregular_polygon_t,
    triangle_t, rectangle_t, circle_t, arc_t, ellipse_t, text_t, xy_plane
} EntityType;

typedef enum {
    red, green, green_dark, blue, blue_light, magenta,
    yellow, white, black, orange, purple, brown 
} ColorPalette;

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
    fs_xsmall, fs_small, fs_medium, fs_large
} FontScale;

typedef struct {
    double red, green, blue;
} RGBColor;

typedef struct {
    LineStyle line;
    RGBColor color;
    DrawStyle draw;
    double line_width;
} EntityStyle;

typedef struct {
    EntityType type;    /* identifies the type of the cad entity */
    char * name;        /* name of the Entity */
    int hash_code;      /* labels are should be unique to do that we use hashing */
} Label;

typedef struct {
    Label * label;      /* Unique label to identify cad entities    */
    void * data;        /* Entity data like radius, width...        */
    EntityStyle * style;   
} Entity;

typedef struct Hierarchy {
    struct CAD2D * cad;
    char * name;                    
    int hash_code;      
    struct Hierarchy * parent;
    struct Hierarchy ** child;      /* Array of child hierarchies   */
    int size;                       /* Size of the child array      */
} Hierarchy;

typedef struct {
    double x, y;
} Point2D;

typedef struct {
    Point2D start; /* (-width/2, -height/2) */
    Point2D end;   /* ( width/2,  height/2) */
} Canvas;

typedef struct LabeList {
    Label * label;
    struct LabeList * next;     
} LabeList;

typedef struct CAD2D {
    Canvas canvas;
    int auto_canvas;        /* Enables to increase canvas size when exceeding entity occurs */
    LabeList * llist;
    int elist_size;         
    Entity ** elist;        /* Entity list keeps entities by hashing */
    Hierarchy * hierarchy;  /* hierarchy data of root CAD (this CAD) */   
} CAD2D;

typedef struct {
    Entity * entity;        /* Entity itself                    */
    CAD2D * cad;            /* Container cad of entity          */
    int index;              /* index inside of the entity list  */
} EntityInfo;

/*  Line, IrregularPolygon and Polyline are represented as PointList.
    Implementation are same but interpretation are changes from type to type    */
typedef struct PointList {
    Point2D point;
    struct PointList * next;
} PointList;

typedef struct {
    Point2D center;
    double radius;
    double start_angle, end_angle;  
} Circle;

typedef struct {
    Point2D point;  /* start point of text */
    char * text;    /* text field */
    FontStyle font;
    RGBColor color;
    double scale; 
} Text;

typedef struct {
    Point2D corner1, corner2, corner3;
} Triangle;

typedef struct {
    Point2D corner1, corner2;   /* Takes two opposite corners as start and end point */
} Rectangle;

typedef struct {
    int n;                /* the number of sides      */
    double out_radius;    /* radius of outer circle   */
    Point2D center;
} RegularPolygon;

typedef struct {
    Point2D center;
    double radius_x, radius_y;
} Ellipse;

CAD2D * c2d_start ();
CAD2D * c2d_start_hier (Hierarchy * h);
CAD2D * c2d_start_wh (double width, double height);
CAD2D * c2d_start_wh_hier (double width, double height, Hierarchy * h);
CAD2D * c2d_get_root_cad (CAD2D * cad);
Hierarchy * c2d_create_hierarchy (CAD2D * cad);
Hierarchy * c2d_create_hierarchy_parent (CAD2D * cad, Hierarchy * parent);
Hierarchy * c2d_get_root_hierarchy (Hierarchy * h);
void c2d_delete_hierarchy (Hierarchy * h);
Label * c2d_create_label (CAD2D * cad, EntityType type, char * name);
EntityInfo * c2d_find_entity (CAD2D * root, Label * l); 
void c2d_remove_entity (CAD2D * cad, Label ** l);
void c2d_remove_entity_elist (CAD2D * cad, Label ** l);
EntityStyle * c2d_set_entity_style (CAD2D * cad, Label * label, LineStyle l, RGBColor c, DrawStyle d, double w);
Point2D * c2d_create_point (double x, double y);
PointList * c2d_create_point_list_p (Point2D p);
void c2d_set_point (Point2D * p, double x, double y);
void c2d_set_color_rgb (RGBColor * c, double red, double green, double blue);
void c2d_set_color_pallette (RGBColor * c, ColorPalette color);
Label * c2d_add_point_xy (CAD2D * cad, double x, double y);
Label * c2d_add_point_xy_l (CAD2D * cad, char * lname, double x, double y);
Label * c2d_add_point_p (CAD2D * cad, Point2D p);
Label * c2d_add_point_p_l (CAD2D * cad, char * lname, Point2D p);
Label * c2d_add_line(CAD2D * cad, Point2D start, Point2D end);
Label * c2d_add_line_l(CAD2D * cad, char * lname, Point2D start, Point2D end);
Label * c2d_add_arc (CAD2D * cad, Point2D center, double radius, double start_angle, double end_angle);
Label * c2d_add_arc_l (CAD2D * cad, char * lname, Point2D center, double radius, double start_angle, double end_angle);
Label * c2d_add_circle (CAD2D * cad, Point2D center, double radius);
Label * c2d_add_circle_l (CAD2D * cad, char * lname, Point2D center, double radius);
Label * c2d_add_ellipse(CAD2D * cad, Point2D center, double radius_x, double radius_y);
Label * c2d_add_ellipse_l(CAD2D * cad, char * lname, Point2D center, double radius_x, double radius_y);
Label * c2d_add_triangle (CAD2D * cad, Point2D corner1, Point2D corner2, Point2D corner3);
Label * c2d_add_triangle_l (CAD2D * cad, char * lname, Point2D corner1, Point2D corner2, Point2D corner3);
Label * c2d_add_rectangle (CAD2D * cad, Point2D corner1 , Point2D corner2);
Label * c2d_add_rectangle_l (CAD2D * cad, char * lname, Point2D corner1 , Point2D corner2);
Label * c2d_add_polyline (CAD2D * cad, Point2D * p, int size);
Label * c2d_add_polyline_l (CAD2D * cad, char * lname, Point2D * p, int size);
Label * c2d_add_irregular_polygon (CAD2D * cad, Point2D * p, int size);
Label * c2d_add_irregular_polygon_l (CAD2D * cad, char * lname, Point2D * p, int size);
Label * c2d_add_regular_polygon (CAD2D * cad, int n, Point2D center, double out_radius);
Label * c2d_add_regular_polygon_l (CAD2D * cad, char * lname, int n, Point2D center, double out_radius);
Label * c2d_add_text (CAD2D * cad, Point2D p, char * text, FontStyle f, RGBColor c, FontScale fs);
Label * c2d_add_text_l (CAD2D * cad, char * lname, Point2D p, char * text, FontStyle f, RGBColor c, FontScale fs);
Label * c2d_add_xy_plane (CAD2D * cad);
double c2d_measure (CAD2D * cad, Label * ls, Label * lt);
Point2D c2d_get_center_point (Entity * e);
Point2D * c2d_get_center_address (Entity * e);
void c2d_snap (CAD2D * cad, Label * ls, Label * lt);
void c2d_export (CAD2D * cad, char * file_name, ExImOption option);
CAD2D * c2d_import (char * file_name, ExImOption option);
void c2d_delete (CAD2D * cad);

#endif

