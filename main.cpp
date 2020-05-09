#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <iostream>
#include <algorithm>
#include <array>
#include <bitset>
#include <string>
#include <cassert>
#include <chrono>
#include <random>

#include "opencl_preliminaries.h"
#include "coo_converter.h"

typedef uint32_t u32;
using namespace std;

template <class Callback>
void measure(Callback f) {
    auto __start = std::chrono::system_clock::now();
    f();
    auto __end = std::chrono::system_clock::now();
    int ems = std::chrono::duration_cast
        <std::chrono::milliseconds>(__end - __start).count();
    std::cerr << "Building quadtree took " << ems << "ms." << std::endl;
}

vector<pair<int, cl_mem>> gpu_quadtrees;

size_t build_and_register(cl_context context, int k, std::vector<std::pair<int, int>>& coo) {
    gpu_quadtrees.
        emplace_back(k,
            transfer_qt(context,
                converter::build_quadtree_from_coo(coo, k)));
    return gpu_quadtrees.size() - 1;
}

int main() 
{
    cl_program program = 0;
    cl_int err = 0;
    cl_uint num = 0;
    cl_platform_id* platforms = {0};
    platforms = getAvailablePlatforms(platforms, num);
    cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[1], 0 };
    cl_context context = 0;
    context = clCreateContextFromType(props, CL_DEVICE_TYPE_GPU, NULL, NULL, NULL);
    cl_device_id* devices = {0};
    size_t cb = 0;
    devices = getAvailableDevices(context, devices, cb);
    cl_command_queue queue;
    cl_kernel adder;
    cl_event event;

    int klog = 10;
    int k = 1 << klog;
    mt19937 gen;
    std::vector<std::pair<int, int>> pairs;
    for (int i = 0; i < k; i++)
        pairs.push_back({ i, i });
   /* for (int i = 0; i < k; i++)
        pairs.push_back({ i, k - 1 - i });
    for (int i = 0; i < k; i++)
        pairs.push_back({ k / 2, i });
    for (int i = 0; i < k; i++)
        pairs.push_back({ i, k / 2 });

    for (int i = 0; i < k; i++)
        for (int s = 0; s < 6; s++)
            pairs.push_back({ i, gen() % k });*/
    std::sort(pairs.begin(), pairs.end());
    pairs.erase(std::unique(pairs.begin(), pairs.end()), pairs.end());


    quadtree qt =
    converter::build_quadtree_from_coo(pairs, klog);
    for (int i = 0; i < qt.tree_structure_data.size(); i++) {
        std::cout << i << ": [";
        for (int j = 0; j < 4; j++) {
            if (j) std::cout << ", ";
            if (qt.tree_structure_data[i][j] & quadtree::LEAF_MASK)
                std::cout << std::bitset<16>(qt.tree_structure_data[i][j] & ~quadtree::LEAF_MASK);
            else
                std::cout << qt.tree_structure_data[i][j];
        }
        std::cout << "]\n";
    }

    measure( [&]() {
        build_and_register(context, klog, pairs);
        });

    clReleaseMemObject(transfer_qt(context, converter::build_quadtree_from_coo(pairs, klog)));
}