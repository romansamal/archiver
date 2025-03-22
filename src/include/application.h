#pragma once
#include <string>

class Application
{
public:
    Application(
        std::ostream &out,
        std::ostream &err,
        std::string_view target
    ) noexcept
        : target(target),
            out(out),
            err(err)
    {

    }

    ~Application() noexcept
    {}

    int Run()
    {
        out << "file name is " << target;

        return 0;
    }

private:
    std::string_view target;
    std::ostream    &out;
    std::ostream    &err;
};