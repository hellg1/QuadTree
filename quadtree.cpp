#include "quadtree.h"

using namespace std;

quadtree::quadtree(int _k, vector<array<uint32_t, 4>> _data) : k(_k), tree_structure_data(_data) {}