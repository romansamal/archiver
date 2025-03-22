#pragma once
#include <memory>

class ArgumentParser;
class Application;

class Runner
{
public:
    Runner(
        std::ostream &out,
        std::ostream &err,
        int argc,
        char **argv
    ) noexcept;

    ~Runner();

    [[nodiscard]] int Run();

private:
    bool ProcessArgs();

    std::ostream                    &out;
    std::ostream                    &err;
    std::unique_ptr<Application>    app;
    std::unique_ptr<ArgumentParser> arg_parser;
};