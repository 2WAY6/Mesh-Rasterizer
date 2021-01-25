#include <iostream>
#include "mesh.h"
#include "raster.h"
#include "gdal_io.h"

// TODO:
// - Make file paths Paths (c++14 or 17?)
// - Add an export path
// - Add argument parsing
// - Add tests (do the input files exist? Are nodes inside? Is it renumbered?)
// - Use templates for vector<vector<T>> stuff
// - Create autoconf and makefile

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