cmake_minimum_required(VERSION 3.14)
project(STTAnalytics VERSION 1.0 LANGUAGES CXX)

# Generate compile_commands.json for VS Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard (target-level alternative is also fine; this is simple and clear)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable folders in IDEs like Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Default build type for single-config generators (e.g., Ninja/Unix Makefiles)
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# Global default output dirs (used by single-config generators and as base)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Per-configuration output dirs (Debug, Release, RelWithDebInfo, MinSizeRel)
foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER "${OUTPUTCONFIG}" OUTPUTCONFIG_UPPER)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
endforeach()

# Source files (explicit is fine; avoids surprising globs)
set(SOURCES
  main.cpp
  src/comparefilename.cpp
  src/ParametersFileReader.cpp
  src/PhotonProcessor.cpp
  src/SurfaceMap.cpp
  src/tonatiuhreader.cpp
)

# Create executable
add_executable(STTAnalytics ${SOURCES})

# Target-scoped include directories (avoid global header leakage)
target_include_directories(STTAnalytics
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Warnings per compiler
if(MSVC)
  target_compile_options(STTAnalytics PRIVATE /permissive- /W4 /Zc:__cplusplus)
else()
  target_compile_options(STTAnalytics PRIVATE -Wall -Wextra -Wpedantic)
  # Uncomment if you want stricter checks:
  # target_compile_options(STTAnalytics PRIVATE -Wconversion -Wsign-conversion)
endif()

# Debug-only sanitizers on GCC/Clang (very helpful during development on Ubuntu)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(STTAnalytics PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
  )
  target_link_options(STTAnalytics PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
  )
endif()

# Optional: enable link-time optimization for Release if supported
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_ok OUTPUT ipo_msg)
if(ipo_ok)
  set_property(TARGET STTAnalytics PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()