cmake_minimum_required(VERSION 3.28)

project(acrhiver)

enable_language(C CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.87 REQUIRED filesystem iostreams program_options)
find_package(absl REQUIRED)

set(SRC src/main.cpp
    src/runner.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SRC})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
    ${Boost_INCLUDE_DIR}
    src/util
    src/include)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC absl::strings
    Boost::filesystem Boost::iostreams Boost::program_options)

target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC
    -O0
    -g
)