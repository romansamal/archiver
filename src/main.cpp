#include <iostream>
#include "runner.h"


int main(int argc, char *argv[])
{
    Runner runner(std::cout, std::cerr, argc, argv);

    return runner.Run();
}