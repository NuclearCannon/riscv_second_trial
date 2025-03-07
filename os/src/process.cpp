#include "process.hpp"
#include "kernel_print.hpp"

Process::Process(int pid):pid(pid)
{
    kprint::printf("process %d inited\n", pid);
}

Process::~Process()
{
    kprint::printf("process %d deconstructed\n", pid);
    pid = 0;
}