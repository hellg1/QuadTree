#pragma warning (disable:4996)
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "opencl_preliminaries.h"

double get_event_exec_time(cl_event event)
{
    cl_ulong start, end;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    double total = (end - start) * 1e-6;
    return total;
}

cl_program load_program(cl_context context, cl_device_id device, const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    cl_program program = 0;
    cl_int status = 0;
    char* data;
    char* build_log;
    size_t ret_val_size;
    size_t length;

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    data = new char[length + 1];
    fread(data, sizeof(char), length, fp);
    data[length] = '\0';

    program = clCreateProgramWithSource(context, 1, (const char**)&data, 0, 0);
    if (program == 0) return 0;

    status = clBuildProgram(program, 0, 0, 0, 0, 0);
    if (status != CL_SUCCESS)
    {
        printf("Error occured building program from file %s\n", filename);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
        build_log = new char[ret_val_size + 1];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
        build_log[ret_val_size] = '\0';
        printf("Building log %s\n", build_log);
        return 0;
    }
    return program;
}

cl_platform_id* getAvailablePlatforms(cl_platform_id* platforms, cl_uint num)
{
    clGetPlatformIDs(0, 0, &num);
    platforms = new cl_platform_id[num];
    clGetPlatformIDs(num, platforms, NULL);
    printf("Found %d Platforms: \n", (int)num);
    for (int i = 0; i < num; i++)
    {
        char str[256];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 256, str, NULL);
        printf("\t%d: %s", i, str);
        char str1[256];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 256, str1, NULL);
        printf("  Version  - %s\n", str1);
    }
    return platforms;
}

cl_device_id* getAvailableDevices(cl_context &context, cl_device_id* devices, size_t cb)
{
    char devname[16][256] = { {0} };
    char info_size[16][256] = { {0} };
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    devices = new cl_device_id[cb];
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, devices, 0);
    uint32_t num_total_device = cb / sizeof(cl_device_id);
    printf("Found %d devices:\n", num_total_device);
    for (unsigned i = 0; i < num_total_device; i++)
    {
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 256, devname[i], 0);
        printf("\t%d: %s", i, devname[i]);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(int), &cb, 0);
        printf("  - %d ", (int)cb);
        clGetDeviceInfo(devices[i], CL_DEVICE_OPENCL_C_VERSION, 256, info_size[i], 0); // if 2.0 -- OK
        printf("  - %s\n", info_size[i]);                                              // Version 1.0 doesn't support dynamic parallelism
    }
    return devices;
}

cl_mem transfer_qt(cl_context context, const quadtree& qt)
{
    cl_mem quadtree_structure = clCreateBuffer(context, 
                                        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                        qt.tree_structure_data.size() * 4,
                                        (void*)qt.tree_structure_data.data(), 
                                        NULL);
    return quadtree_structure;
}