#include <vector>
#include <limits>
#include <math.h>
#include <ctime>
#include <initializer_list>
#include <algorithm>
#include "mesh.h"


bool isCCW(Node& n1, Node& n2, Node& n3) {
    return (n3.y - n1.y) * (n2.x - n1.x) >= (n2.y - n1.y) * (n3.x - n1.x);
}


double interpolateElevationOnTriangle(double x, double y, Node& v1, Node& v2, Node& v3) {
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
        std::cout << "Creating Raster from Mesh..." << std::endl;
        nCols = ceil(mesh.xMax) - floor(mesh.xMin);
        nRows = ceil(mesh.yMax) - floor(mesh.yMin);
        matrix = std::vector<std::vector<double>>(nCols, std::vector<double>(nRows));
        interpolateValuesFromMesh(mesh, cellSize);
    }

    void interpolateValuesFromMesh(Mesh& mesh, int cellSize) {
        std::cout << " - Interpolating raster values from mesh..." << std::endl;
        std::time_t t0 = std::time(nullptr);
        for (Element& element : mesh.elements) {
            // std::vector<Node> elmt_nodes = element.getNodes(mesh.nodes);
            Node& node1 = mesh.nodes[element.nodeIds[0]];
            Node& node2 = mesh.nodes[element.nodeIds[1]];
            Node& node3 = mesh.nodes[element.nodeIds[2]];
            int xMin = floor(std::min({node1.x, node2.x, node3.x}));
            int yMin = floor(std::min({node1.y, node2.y, node3.y}));
            int xMax = ceil(std::max({node1.x, node2.x, node3.x}));
            int yMax = ceil(std::max({node1.y, node2.y, node3.y}));

            for (int x=xMin; x<xMax; x+=cellSize) {
                for (int y=yMin; y<yMax; y+=cellSize) {
                    Node n = Node(0, x, y, 0);
                    bool bool1 = isCCW(n, node1, node2);
                    bool bool2 = isCCW(n, node2, node3);
                    bool bool3 = isCCW(n, node3, node1);

                    if ((bool1 && bool2 && bool3) || (!bool1 && !bool2 && !bool3)) {
                        int rCol = int(x - xMin); // round?
                        int rRow = int(yMax - y); // round??

                        matrix[rRow][rCol] = interpolateElevationOnTriangle(x, y, node1, node2, node3);
                    }
                }
            }
        }
        std::time_t t1 = std::time(nullptr);
        std::time_t dt = t1 - t0;
        std::cout << "  -> Raster erzeugt." << std::endl;
        std::cout << dt << " seconds" << std::endl;
    }
};
#endif
