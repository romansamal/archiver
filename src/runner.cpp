#include "runner.h"

#include "application.h"

#include "argparse.hpp"

#include "huffman_algorithm.h"

Runner::Runner(
    std::ostream &out,
    std::ostream &err,
    int argc,
    char **argv
) noexcept
    :   huffman_algorithm(std::make_unique<HuffmanAlgorithm>()),
        out(out),
        err(err),
        app(nullptr),
        arg_parser(std::make_unique<ArgumentParser>(argc, argv))
{

}

Runner::~Runner() = default;

int Runner::Run()
{
    const bool is_args_processed = ProcessArgs();

    return is_args_processed ? app->Run() : 0;
}


bool Runner::ProcessArgs()
{
    bool result = false;

    (*arg_parser)
        ("help", "Print Help Message About Using This Tool")
        ("file", boost::program_options::value<std::string>(), "Specify File To Be Processed")
    ;

    auto &args = arg_parser->Parse();

    if (args.contains("help"))
    {
        out << arg_parser->GetDescription();
    }

    if (args.contains("file"))
    {
        auto &target = args["file"].as<std::string>();

        app = std::make_unique<Application>(*huffman_algorithm, out, err, target);

        result = true;
    }

    return result;
}