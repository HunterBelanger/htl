cmake_minimum_required(VERSION 3.2)
project(htl
  DESCRIPTION "Template library of containers not provided by the STL"
  LANGUAGES CXX
)

option(HTL_BUILD_TESTS "Compiler tests for HTL" OFF)

# Create htl library target
add_library(htl INTERFACE)

# Add public interface include directory
target_include_directories(htl INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Require C++17 standard
target_compile_features(htl INTERFACE cxx_std_17)

# Build tests if desired
if(HTL_BUILD_TESTS)
  find_package(GTest REQUIRED)
  if(GTest_FOUND)
    add_subdirectory(tests)
  endif()
endif()

# Install
install(TARGETS htl EXPORT htl)
install(EXPORT htl FILE htl-config.cmake DESTINATION share/cmake/htl)
install(DIRECTORY include/htl DESTINATION include)
