#include <vector>
#include "node.h"

#ifndef ELEMENT
#define ELEMENT
struct Element {
    int id;
    std::vector<int> nodeIds;

    Element(int id, std::vector<int> nodeIds) : id(id), nodeIds(nodeIds) {};

    std::vector<Node> getNodes(std::vector<Node>& nodes) {
        std::vector<Node> elmt_nodes;
        for (Node& node : nodes)
            elmt_nodes.push_back(node);
        return elmt_nodes;
    }
};
#endif