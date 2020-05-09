#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>

typedef uint16_t tile;

class quadtree {
public:
    static const uint32_t TILE_SZ_LOG = 2;
    static const uint32_t LEAF_MASK = (1u << 31u);
    // k = log_2(n)
    int k;
    std::vector<std::array<uint32_t, 4>> tree_structure_data;
    quadtree(int _k, std::vector<std::array<uint32_t, 4>> _data);
};
