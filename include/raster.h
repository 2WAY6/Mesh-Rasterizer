#include <vector>
#include <limits>
#include <math.h>
#include "mesh.h"


bool isCCW(Node& n1, Node& n2, Node& n3) {
    return (n3.y - n1.y) * (n2.x - n1.x) >= (n2.y - n1.y) * (n3.x - n1.x);
}


double interpolateElevationOnTriangle(double x, double y, std::vector<Node>& nodes) {
    Node& v1 = nodes[0];
    Node& v2 = nodes[1];
    Node& v3 = nodes[2];

    double w1 = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
    double w2 = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
    double w3 = 1 - w1 - w2;
    double z = w1 * v1.z + w2 * v2.z + w3 * v3.z;
    return z;
}


#ifndef BOUNDINGBOX
#define BOUNDINGBOX
struct BoundingBox {
    double xMin;
    double xMax;
    double yMin;
    double yMax;

    BoundingBox() {
        xMin = std::numeric_limits<double>::infinity();
        xMax = 0;
        yMin = std::numeric_limits<double>::infinity();
        yMax = 0;
    }

    BoundingBox(std::vector<Node>& nodes) {
        for (Node node : nodes) {
            if (node.x < xMin)
                xMin = node.x;
            else if (node.x > xMax)
                xMax = node.x;
            if (node.y < yMin)
                yMin = node.y;
            else if (node.y > yMax)
                yMax = node.y;
        }
    }

    void toInteger() {
        xMax = ceil(xMax);
        xMin = floor(xMax);
        yMin = floor(yMin);
        yMax = ceil(yMax);
    }
};
#endif


#ifndef RASTER
#define RASTER
struct Raster {
    int nRows;
    int nCols;
    std::vector<std::vector<double>> matrix;
    
    Raster() : nRows(1), nCols(1) {};

    Raster(const int nRows, const int nCols) : nRows(nRows), nCols(nCols) {
        matrix = std::vector<std::vector<double>>(nCols, std::vector<double>(nRows));
    }

    void fromMesh(Mesh& mesh, int cellSize) {
        nCols = ceil(mesh.xMax) - floor(mesh.xMin);
        nRows = ceil(mesh.yMax) - floor(mesh.yMax);
        matrix = std::vector<std::vector<double>>(nCols, std::vector<double>(nRows));
        interpolateValuesFromMesh(mesh, cellSize);
    }

    void interpolateValuesFromMesh(Mesh& mesh, int cellSize) {
        for (Element& element : mesh.elements) {
            std::vector<Node> elmt_nodes = element.getNodes(mesh.nodes);
            BoundingBox extent = BoundingBox(elmt_nodes);
            extent.toInteger();

            for (int x=extent.xMin; x<extent.xMax; x+=cellSize) {
                for (int y=extent.yMin; y<extent.yMax; y+=cellSize) {
                    Node n = Node(0, x, y, 0);
                    bool bool1 = isCCW(n, elmt_nodes[0], elmt_nodes[1]);
                    bool bool2 = isCCW(n, elmt_nodes[1], elmt_nodes[2]);
                    bool bool3 = isCCW(n, elmt_nodes[2], elmt_nodes[0]);

                    if ((bool1 && bool2 && bool3) || (!bool1 && !bool2 && !bool3)) {
                        int rCol = int(x - extent.xMin); // round?
                        int rRow = int(extent.yMax - y); // round??
                        matrix[rRow][rCol] = interpolateElevationOnTriangle(x, y, elmt_nodes);
                    }
                }
            }
        }
    }
};
#endif
