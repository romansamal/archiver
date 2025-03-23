#pragma once
#include <string>
#include "interfaces.h"

class Application
{
public:
    Application(
        CompressAlgorithm<uint8_t> &algorithm,
        std::ostream &out,
        std::ostream &err,
        std::string_view target
    ) noexcept;

    ~Application() noexcept;

    /**
     * Main compress application runner
     */
    int Run();

private:
    CompressAlgorithm<uint8_t> &algorithm;
    std::string_view target;
    std::ostream    &out;
    std::ostream    &err;
};