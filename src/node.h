#ifndef NODE
#define NODE
struct Node {
    int id;
    float x;
    float y;
    float z;

    Node(int id, float x, float y, float z) : id(id), x(x), y(y), z(z) {};
};    
#endif