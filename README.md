# cse102-summer-project

INTRODUCTION
---------------------
CAD Library for drawing and manipulating 2D shapes. All the code
is written in C language. To make library functions safe, reliable
and efficient every function is written carefully. The usage of
memory is checked and provided to the user without any memory leak. 


COMPILING
---------------------
To compile the library and test file, use provided makefile.


IMPLEMENTATION
---------------------
There are plenty of created data structures to provide easy to use
functions. One of the most important ones are given below.

1. CAD2D:
    Data structure to keep all the CAD content. Export/import 
    is done with this data type. Every CAD content 
    start and end with CAD2D type.   

2. Hierarchy: 
    Data structure to provide a hierarchical relationship with other
    CAD contents. Every CAD has a hierarchy and every CAD entity belongs
    to a hierarchy. The library guarantees that each hierarchies are unique.
    To maintain their uniqueness, implementation done with Hash-Table which uses Linear-Probing.

3. Entity:
    Data structure to hold a CAD entity like polyline, rectangle, text...
    Every entity has a unique label whether created by the user or generated
    by the library as the default option. Each entity has a style choice to
    modify colour, line shape or draw choice.

3. Label:
    Data structure to specified entities. These labels are produced whether 
    by the user or generated by the library as the default option. To
    maintain their uniqueness, again Hash-Table is used.

CAD content can be exported in one of the formats EPS or GTUCAD. For 
importing GTUCAD format used.


TESTING & EXAMPLES
---------------------
All the codes are tested with an application called Evince, so 
strongly suggested to use Evince for the result of tests. Also the 
results are given in "examples" folder.

After compilation, the library, executes the produced executable file
called test. After execution there will be produced outputs as file
extensions *.eps and *.gtucad. Then tester can use the bash command
`evince test0.eps` to see the result of first test. As a shortcut to see
all the examples at once use the command `evince test*eps`. 


OPTIMIZATION
---------------------
Snapping functions are enough for basic purposes, for more advance
purposes it might have some optimization. In the future better snapping
functions are provided.


GITHUB Repository
---------------------
https://github.com/ebylmz17/cse102-summer-project.git
