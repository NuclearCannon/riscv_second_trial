#pragma once

#include "kernel_print.hpp"
#include "sbi.hpp"

#define panic(x) ({kprint::printf("%s %s %d:%s\n", __FILE__, __FUNCTION__, __LINE__, x);sbi::sbi_shutdown();})