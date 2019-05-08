cmake_minimum_required(VERSION 3.0.0)

project(binaryen-const-counter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(
    binaryen
    URL https://github.com/WebAssembly/binaryen/archive/version_83.tar.gz
    PREFIX binaryen
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    CMAKE_ARGS
        -DBUILD_STATIC_LIB=ON
        -DCMAKE_BUILD_TYPE=Release
)

ExternalProject_Get_Property(binaryen source_dir)
ExternalProject_Get_Property(binaryen binary_dir)

add_library(binaryen::binaryen STATIC IMPORTED)
add_dependencies(binaryen::binaryen binaryen)

set(binaryen_LIBS
    ${binary_dir}/lib/libwasm.a
    ${binary_dir}/lib/libasmjs.a
    ${binary_dir}/lib/libpasses.a
    ${binary_dir}/lib/libcfg.a
    ${binary_dir}/lib/libir.a
    ${binary_dir}/lib/libemscripten-optimizer.a
    ${binary_dir}/lib/libsupport.a
    ${binary_dir}/lib/libwasm.a
    Threads::Threads
)

file(MAKE_DIRECTORY ${source_dir}/src)

set_target_properties(binaryen::binaryen
    PROPERTIES
    IMPORTED_LOCATION ${binary_dir}/lib/libbinaryen.a
    INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/src
    INTERFACE_LINK_LIBRARIES "${binaryen_LIBS}"
)

add_executable(count-const
    main.cpp
)

target_link_libraries(count-const
    binaryen::binaryen
)
