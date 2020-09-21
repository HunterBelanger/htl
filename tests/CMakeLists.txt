cmake_minimum_required(VERSION 3.2)
project(htl_test
  DESCRIPTION "Test for the HTL template library"
  LANGUAGES CXX
)

add_executable(htl-tests tests_main.cpp shared_span_tests.cpp)

target_compile_features(htl-tests PRIVATE cxx_std_17)

target_link_libraries(htl-tests PRIVATE htl gtest)
