#pragma once

#include "quadtree.h"

using namespace std;

class converter
{
public:
	static quadtree build_quadtree_from_coo(const vector<pair<int, int>>& pairs, int k);
	static vector<pair<int, int>> get_nnz_from_quadtree(const quadtree& qt);
};