#include "raster.h"
#include "raster_utils.h"

Raster::Raster() : nRows(1), nCols(1) {};

Raster::Raster(const int nRows, const int nCols) : nRows(nRows), nCols(nCols) {
    matrix = std::vector<std::vector<float>>(nCols, std::vector<float>(nRows));
}

// bool Raster::isCCW(Node& n1, Node& n2, Node& n3) {
//     return (n3.y - n1.y) * (n2.x - n1.x) >= (n2.y - n1.y) * (n3.x - n1.x);
// }


// float Raster::interpolateElevationOnTriangle(float x, float y, Node& v1, Node& v2, Node& v3) {
//     float w1 = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
//     float w2 = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
//     float w3 = 1 - w1 - w2;
//     float z = w1 * v1.z + w2 * v2.z + w3 * v3.z;
//     return z;
// }


void Raster::fromMesh(Mesh& mesh, int cellSize) {
    std::cout << "\nCreating Raster from Mesh..." << std::endl;
    cellSize = cellSize;
    xMin = floor(mesh.xMin);
    yMin = floor(mesh.yMin);
    xMax = ceil(mesh.xMax);
    yMax = ceil(mesh.yMax);

    nCols = xMax - xMin;
    nRows = yMax - yMin;
    matrix = std::vector<std::vector<float>>(nRows, std::vector<float>(nCols));

    for (int rowi = 0; rowi < nRows; rowi++) {
        for (int coli = 0; coli < nCols; coli++) {
            matrix[rowi][coli] = 0.0;
        }
    }

    interpolateValuesFromMesh(mesh, cellSize);
};

void Raster::interpolateValuesFromMesh(Mesh& mesh, int cellSize) {
    std::cout << " - Interpolating raster values from mesh..." << std::endl;
    std::time_t t0 = std::time(nullptr);
    int interpolationCount = 0;
    for (Element& element : mesh.elements) {
        // std::vector<Node> elmt_nodes = element.getNodes(mesh.nodes);
        Node& node1 = mesh.nodes[element.nodeIds[0]];
        Node& node2 = mesh.nodes[element.nodeIds[1]];
        Node& node3 = mesh.nodes[element.nodeIds[2]];
        int elmtXMin = floor(std::min({node1.x, node2.x, node3.x}));
        int elmtYMin = floor(std::min({node1.y, node2.y, node3.y}));
        int elmtXMax = ceil(std::max({node1.x, node2.x, node3.x}));
        int elmtYMax = ceil(std::max({node1.y, node2.y, node3.y}));

        for (int x=elmtXMin; x<elmtXMax; x+=cellSize) {
            for (int y=elmtYMin; y<elmtYMax; y+=cellSize) {
                Node n = Node(0, x, y, 0);
                bool bool1 = isCCW(n, node1, node2);
                bool bool2 = isCCW(n, node2, node3);
                bool bool3 = isCCW(n, node3, node1);

                if ((bool1 && bool2 && bool3) || (!bool1 && !bool2 && !bool3)) {
                    int rCol = int(x - xMin); // round?
                    int rRow = int(yMax - y); // round??

                    float z = interpolateElevationOnTriangle(x, y, node1, node2, node3);
                    matrix[rRow][rCol] = z;
                    interpolationCount++;

                    if (z > zMax)
                        zMax = z;
                    else if (z < zMin)
                        zMin = z;
                }
            }
        }
    }
    std::time_t t1 = std::time(nullptr);
    std::time_t dt = t1 - t0;
    std::cout << "  -> " << interpolationCount << " points interpolated." << std::endl;
    std::cout << "- Finished after " << dt << " seconds" << std::endl;
}