cmake_minimum_required(VERSION 3.16)
project(LucideIconCpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration options
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(BUILD_EXAMPLES "Build example programs" OFF)

# Find required packages
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Define directories
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(ICONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lucide/icons)
set(NANOSVG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nanosvg)

# Create generated directory
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Generate icons during CMake configuration
message(STATUS "Generating icon code from SVG files...")
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/script/buildbinary.py --output-dir ${GENERATED_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE GENERATION_RESULT
    OUTPUT_VARIABLE GENERATION_OUTPUT
    ERROR_VARIABLE GENERATION_ERROR
)

if(NOT GENERATION_RESULT EQUAL 0)
    message(FATAL_ERROR "Icon generation failed: ${GENERATION_ERROR}")
else()
    message(STATUS "Icon generation completed successfully")
endif()

# Add nanosvg as interface library (fix interface directories issue)
add_library(nanosvg INTERFACE)
target_include_directories(nanosvg INTERFACE 
    $<BUILD_INTERFACE:${NANOSVG_DIR}/src>
    $<INSTALL_INTERFACE:include/lucide/nanosvg>
)
target_compile_definitions(nanosvg INTERFACE NANOSVG_IMPLEMENTATION)

# Define source files
set(LUCIDE_SOURCES
    lucideicon.cpp
    src/iconwrapper.cpp
    ${GENERATED_DIR}/icon_registry.cpp
    ${GENERATED_DIR}/icon_functions.cpp
)

# Define header files
set(LUCIDE_HEADERS
    lucideicon.hpp
    src/iconwrapper.hpp
    ${GENERATED_DIR}/icon_declarations.hpp
)

# Create main library (static or shared based on BUILD_SHARED_LIBS)
add_library(lucide-icon ${LUCIDE_SOURCES})

# Set library properties
if(BUILD_SHARED_LIBS)
    message(STATUS "Building LucideIconCpp as SHARED library")
    set_target_properties(lucide-icon PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    # Add export definitions for Windows DLL
    if(WIN32)
        target_compile_definitions(lucide-icon PRIVATE LUCIDE_EXPORTS)
        target_compile_definitions(lucide-icon INTERFACE LUCIDE_IMPORTS)
    endif()
else()
    message(STATUS "Building LucideIconCpp as STATIC library")
endif()

# Link nanosvg to the library
target_link_libraries(lucide-icon PUBLIC nanosvg)

# Set include directories
target_include_directories(lucide-icon 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${GENERATED_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include/lucide>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set compiler features
target_compile_features(lucide-icon PUBLIC cxx_std_17)

# Add compiler flags
if(MSVC)
    target_compile_options(lucide-icon PRIVATE /W4)
else()
    target_compile_options(lucide-icon PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add example subdirectory
if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

# Install targets
install(TARGETS lucide-icon nanosvg
    EXPORT LucideIconCppTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/lucide
)

install(FILES ${LUCIDE_HEADERS}
    DESTINATION include/lucide
)

# Install nanosvg headers
install(FILES 
    ${NANOSVG_DIR}/src/nanosvg.h
    ${NANOSVG_DIR}/src/nanosvgrast.h
    DESTINATION include/lucide/nanosvg
)

# Export targets
install(EXPORT LucideIconCppTargets
    FILE LucideIconCppTargets.cmake
    NAMESPACE LucideIconCpp::
    DESTINATION lib/cmake/LucideIconCpp
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/LucideIconCppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LucideIconCppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LucideIconCppConfig.cmake"
    INSTALL_DESTINATION lib/cmake/LucideIconCpp
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/LucideIconCppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/LucideIconCppConfigVersion.cmake"
    DESTINATION lib/cmake/LucideIconCpp
)