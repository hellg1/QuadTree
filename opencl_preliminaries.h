#include <CL/cl.h>
#include "quadtree.h"

typedef uint32_t u32;

double get_event_exec_time(cl_event event);
cl_program load_program(cl_context context, cl_device_id device, const char* filename);
cl_platform_id* getAvailablePlatforms(cl_platform_id* platforms, cl_uint num);
cl_device_id* getAvailableDevices(cl_context &context, cl_device_id* devices, size_t cb);
cl_mem transfer_qt(cl_context context, const quadtree& qt);