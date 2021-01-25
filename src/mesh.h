#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <math.h>
#include "node.h"
#include "element.h"


#ifndef MESH
#define MESH
struct Mesh {
    std::vector<Node> nodes;
    std::vector<Element> elements;
    int maxNodeId = 0;
    int maxElementId = 0;
    float xMin = std::numeric_limits<float>::infinity();
    float xMax = 0.0;
    float yMin = std::numeric_limits<float>::infinity();
    float yMax = 0.0;
    float zMin = std::numeric_limits<float>::infinity();
    float zMax = 0.0;

    Mesh();

    Mesh(std::vector<Node> nodes, std::vector<Element> elements);

    std::vector<Element>* getElements();

    std::vector<Node>* getNodes();

    void import_2dm_mesh(std::string path_mesh);
        
};
#endif