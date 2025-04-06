#pragma once

#include "kernel_print.hpp"


#define assert(x) ({if(!(x))kprint::printf("%s %s Assert Fail!\n", __FILE__, __LINE__);})