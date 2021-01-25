#include <vector>
#include <limits>
#include <math.h>
#include <ctime>
#include <initializer_list>
#include <algorithm>
#include "mesh.h"

#ifndef RASTER
#define RASTER
struct Raster {
    int nRows;
    int nCols;
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    float zMax = 0;
    float zMin = 100000000;

    int cellSize;
    std::vector<std::vector<float>> matrix;
    
    Raster();

    Raster(const int nRows, const int nCols);

    // bool isCCW(Node& n1, Node& n2, Node& n3);

    void interpolateValuesFromMesh(Mesh& mesh, int cellSize);

    // float interpolateElevationOnTriangle(float x, float y, Node& v1, Node& v2, Node& v3);

    void fromMesh(Mesh& mesh, int cellSize);
};
#endif
