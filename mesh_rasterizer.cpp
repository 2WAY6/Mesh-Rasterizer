#include <iostream>
#include "include/mesh.h"
#include "include/gdal_io.h"

int main () {
     std::string path_mesh = "/home/titan/Programming/CPP/Mesh-Rasterizer/data/big/hydro_as-2d.2dm";
     Mesh mesh = Mesh();
     mesh.import_2dm_mesh(path_mesh);
     export_to_geotiff(); 
}