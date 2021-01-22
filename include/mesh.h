#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "utils.h"



struct Node {
    int id;
    double x;
    double y;
    double z;

    Node(int id, double x, double y, double z) : id(id), x(x), y(y), z(z) {};
};    

class Element {
private:
    int id;
    std::vector<int> nodeIds;

public:
    Element(int id, std::vector<int> nodeIds) : id(id), nodeIds(nodeIds) {};
};


class Mesh {
private:
    std::vector<Node> nodes;
    std::vector<Element> elements;
    int maxNodeId = 0;
    int maxElementId = 0;

public:
    Mesh() {
        std::cout << "Creating Mesh..." << std::endl;
        nodes = std::vector<Node>();
        elements = std::vector<Element>();
    }

    Mesh(std::vector<Node> nodes, std::vector<Element> elements) : nodes(nodes), 
        elements(elements) {};

    std::vector<Element>* getElements() { return &elements; }

    std::vector<Node>* getNodes() { return &nodes; }

    void import_2dm_mesh(std::string path_mesh) {
        std::cout << "\nImporting mesh from " << path_mesh << " ...\n";

        std::ifstream infile(path_mesh);
        std::string line;
        std::vector<std::vector<int>> quad_elements;

        std::cout << "- Reading nodes and elements from file..." << std::endl;
        while (std::getline(infile, line)) {
            if (line.rfind("ND", 0) == 0) {
                std::vector<std::string> parts = splitStringAtWhitespace(line);
                double x = std::stod(parts[2]);
                double y = std::stod(parts[3]);
                double z = std::stod(parts[4]);
                Node node = Node(maxNodeId++, x, y, z);
                nodes.push_back(node);
            }
            else if (line.rfind("E3T", 0) == 0) {
                std::vector<std::string> parts = splitStringAtWhitespace(line);
                int nid1 = std::stoi(parts[2]) - 1;
                int nid2 = std::stoi(parts[3]) - 1;
                int nid3 = std::stoi(parts[4]) - 1;
                std::vector<int> nodeIds {nid1, nid2, nid3};
                Element element = Element(maxElementId++, nodeIds);
                elements.push_back(element);
            }
            else if (line.rfind("E4Q", 0) == 0) {
                std::vector<std::string> parts = splitStringAtWhitespace(line);
                int nid1 = std::stoi(parts[2]) - 1;
                int nid2 = std::stoi(parts[3]) - 1;
                int nid3 = std::stoi(parts[4]) - 1;
                int nid4 = std::stoi(parts[5]) - 1;
                std::vector<int> nodeIds {nid1, nid2, nid3, nid4};
                quad_elements.push_back(nodeIds);
            }
        }
        infile.close();
        std::cout << "  -> Imported " << nodes.size() << " nodes and " << elements.size() << " elements." << std::endl;

        if (quad_elements.size() > 0) {
            std::cout << "- Splitting quads into three triangles..." << std::endl;

            for (std::vector<int> quad : quad_elements) {
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;
                for (int nid : quad) {
                    Node& pNode = nodes[nid];
                    x += pNode.x;
                    y += pNode.y;
                    z += pNode.z;
                }
                x /= 4;
                y /= 4;
                z /= 4;
                Node node = Node(maxNodeId++, x, y, z);
                nodes.push_back(node);
                
                elements.push_back(Element(maxElementId++, 
                    std::vector<int> {quad[0], quad[1], maxNodeId}));
                elements.push_back(Element(maxElementId++, 
                    std::vector<int> {quad[1], quad[2], maxNodeId}));
                elements.push_back(Element(maxElementId++, 
                    std::vector<int> {quad[2], quad[3], maxNodeId}));
                elements.push_back(Element(maxElementId++, 
                    std::vector<int> {quad[3], quad[0], maxNodeId}));
            }
            std::cout << "  -> " << nodes.size() << " nodes and " << elements.size() << " elements after splitting." << std::endl;
        }
    }
};

