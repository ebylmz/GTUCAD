# 📐 GTUCAD

A CAD Library for drawing and manipulating 2D shapes.

![alt text](https://github.com/ebylmz/GTUCAD/blob/master/intro.png?raw=true)

🛠️ COMPILING
---------------------
To compile the library and test files, use the provided makefile.

🧱 IMPLEMENTATION
---------------------
The repository features a variety of data structures, including:

- **🏛️ CAD2D**: This data structure holds CAD content and handles export/import operations.

- **🌳 Hierarchy**: It establishes hierarchical relationships among CAD entities, ensuring uniqueness through Hash-Table with linear probing.

- **✏️ Entity**: Entities like polylines, rectangles, and text are stored here, each with a unique label for customization.

- **🏷️ Label**: Data structures to specify entities, ensuring uniqueness through Hash-Table.

CAD content can be exported in EPS or GTUCAD formats, while GTUCAD format is used for importing.

## 🧪 Testing & Examples

All code has been rigorously tested with the Evince application. View test results in the "examples" folder.

After compilation, execute the "test" executable to generate outputs with *.eps and *.gtucad extensions. To view the result of the first test, use the bash command `evince test0.eps`. To view all examples at once, use `evince test*eps`.

## 🚀 Optimization

While basic snapping functions are provided, advanced purposes may require additional optimization in the future. Stay tuned for better-snapping functions.
