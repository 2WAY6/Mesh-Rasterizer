#include "mesh.h"

bool isCCW(Node& n1, Node& n2, Node& n3) {
    return (n2.y - n1.y) * (n2.x - n1.x) >= (n2.y - n1.y) * (n3.x - n1.x);
}


double interpolateElevationOnTriangle(double x, double y, Node& v1, Node& v2, Node& v3) {
    double w1 = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
    double w2 = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
    double w3 = 1 - w1 - w2;
    double z = w1 * v1.z + w2 * v2.z + w3 * v3.z;
    return z;
}
