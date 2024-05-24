cmake_minimum_required(VERSION 3.11)
project(htl
  DESCRIPTION "Template library of containers not provided by the STL"
  LANGUAGES CXX
)

# Create htl library target
add_library(htl INTERFACE)

# Add public interface include directory
target_include_directories(htl INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Require C++20 standard
target_compile_features(htl INTERFACE cxx_std_20)
