#include "node.h"

bool isCCW(Node& n1, Node& n2, Node& n3) {
    return (n3.y - n1.y) * (n2.x - n1.x) >= (n2.y - n1.y) * (n3.x - n1.x);
}

float interpolateElevationOnTriangle(float x, float y, Node& v1, Node& v2, Node& v3) {
    float w1 = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
    float w2 = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
    float w3 = 1 - w1 - w2;
    float z = w1 * v1.z + w2 * v2.z + w3 * v3.z;
    return z;
}

