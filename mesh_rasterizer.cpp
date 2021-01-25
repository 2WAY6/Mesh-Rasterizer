#include <iostream>
#include "include/mesh.h"
#include "include/raster.h"
#include "include/gdal_io.h"

int main () {
     const int cellSize = 1;

     std::string path_mesh = "/home/titan/Programming/CPP/Mesh-Rasterizer/data/small/hydro_as-2d.2dm";
     Mesh mesh = Mesh();
     mesh.import_2dm_mesh(path_mesh);

     Raster raster = Raster();
     raster.fromMesh(mesh, cellSize);

     export_to_geotiff(raster); 

     std::cout << "\nProgram finished." << std::endl;
}