#include "application.h"

#include "file_random_access.h"
#include "huffman_algorithm.h"

Application::Application(
    CompressAlgorithm<uint8_t> &algorithm,
    std::ostream &out,
    std::ostream &err,
    std::string_view target
) noexcept
    :   algorithm(algorithm),
        target(target),
        out(out),
        err(err)
{

}

Application::~Application() noexcept
{}

int Application::Run()
{
    out << "file name is " << target;

    std::string result_file_name = std::string(target) + std::string(".arch");

    fileutils::File new_f(target);
    fileutils::File result_f(result_file_name);

    result_f.CreateBinary(0, new_f.GetSize(), true);

    FileRandomAccessible target_random_accessible(target);
    FileRandomAccessible result_random_accessible(result_file_name);

    std::size_t new_size = algorithm.Compress(target_random_accessible, result_random_accessible);

    result_f.Truncate(new_size);

    return 0;
}