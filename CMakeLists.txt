cmake_minimum_required(VERSION 3.18)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

if(NOT DEFINED VCPKG_TARGET_TRIPLET AND DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
    set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}")
endif()

project(cnvToPdf)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32 AND MSVC)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -utf-8 -DUNICODE -U_WINDOWS")
    add_compile_options(-utf-8 -DUNICODE -U_WINDOWS)
endif()

find_package(unofficial-libharu CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
	src/cnvToPdf.cpp
	src/jpgfileToPdf.cpp
	src/ExArgv.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    unofficial::libharu::hpdf
)
