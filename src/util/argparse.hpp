#pragma once
#include <iostream>
#include <boost/program_options.hpp>

class ArgumentParser
{
public:
    ArgumentParser() = delete;

    ArgumentParser(int argc, char **argv) noexcept
        : argc(argc), argv(argv)
    {
    }

    ~ArgumentParser() noexcept
    {
    }

    template <typename... Args>
    ArgumentParser &operator()(
        Args &&...args)
    {
        options.add_options()(std::forward<Args>(args)...);
        return *this;
    }

    boost::program_options::variables_map &Parse()
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), variables);
        boost::program_options::notify(variables);

        return variables;
    }

    std::string GetDescription()
    {
        std::ostringstream oss;
        oss << options << "\n";
        return oss.str();
    }

private:
    int argc;
    char **argv;
    boost::program_options::options_description options{"Allowed options"};
    boost::program_options::variables_map       variables;
};
