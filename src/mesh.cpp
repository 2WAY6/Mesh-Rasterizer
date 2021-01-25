#include "mesh.h"
#include "mesh_utils.h"

Mesh::Mesh() {
    nodes = std::vector<Node>();
    elements = std::vector<Element>();
}

Mesh::Mesh(std::vector<Node> nodes, std::vector<Element> elements) : nodes(nodes), elements(elements) {};

std::vector<Element>* Mesh::getElements() { return &elements; }

std::vector<Node>* Mesh::getNodes() { return &nodes; }

void Mesh::import_2dm_mesh(std::string path_mesh) {
    std::cout << "\nImporting mesh from " << path_mesh << " ...\n";

    std::ifstream infile(path_mesh);
    std::string line;
    std::vector<std::vector<int>> quad_elements;

    std::cout << "- Reading nodes and elements from file..." << std::endl;
    while (std::getline(infile, line)) {
        if (line.rfind("ND", 0) == 0) {
            std::vector<std::string> parts = splitStringAtWhitespace(line);
            float x = std::stod(parts[2]);
            float y = std::stod(parts[3]);
            float z = std::stod(parts[4]);
            Node node = Node(maxNodeId++, x, y, z);
            nodes.push_back(node);

            if (node.x < xMin)
                xMin = node.x;
            else if (node.x > xMax)
                xMax = node.x;
            if (node.y < yMin)
                yMin = node.y;
            else if (node.y > yMax)
                yMax = node.y;
            if (node.z < zMin)
                zMin = node.z;
            else if (node.z > zMax)
                zMax = node.z;
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
            float x = 0.0;
            float y = 0.0;
            float z = 0.0;
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
        std::cout << "  -> " << nodes.size() << " nodes and " << elements.size() 
                  << " elements after splitting." << std::endl;
    }
}