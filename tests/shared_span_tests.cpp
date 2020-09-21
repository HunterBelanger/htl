#include <htl/shared_span.hpp>
#include <gtest/gtest.h>

namespace {

  using namespace htl;

  TEST(shared_span, vector_constructor) {
    std::vector<double> vec {1,2,3,4,5}; 
    shared_span<double> span(vec);

    EXPECT_EQ(vec.size(), span.size());

    for(size_t i = 0; i < vec.size(); i++) {
      EXPECT_DOUBLE_EQ(vec[i], span[i]); 
    }

    std::vector<double> vec2 {};
    shared_span<double> span2(vec2);
    EXPECT_EQ(vec2.size(), span2.size());
  }

  TEST(shared_span, initializer_list_constructor) {
    shared_span<double> span({1,2,3,4,5});

    EXPECT_EQ(span.size(), 5);

    std::vector<double> vec {1,2,3,4,5};
    for(size_t i = 0; i < vec.size(); i++) {
      EXPECT_DOUBLE_EQ(vec[i], span[i]); 
    }

    shared_span<double> span2({});
    EXPECT_EQ(span2.size(), 0);
  }

  TEST(shared_span, iterator_constructor) {
    std::vector<double> vec {1,2,3,4,5}; 
    shared_span<double> span(vec.begin(), vec.end());

    EXPECT_EQ(vec.size(), span.size());

    for(size_t i = 0; i < vec.size(); i++) {
      EXPECT_DOUBLE_EQ(vec[i], span[i]); 
    }

    std::vector<double> vec2 {};
    shared_span<double> span2(vec2.begin(), vec2.end());
    EXPECT_EQ(vec2.size(), span2.size());
  }

  TEST(shared_span, copy_constructor) {
    shared_span<double> span({1,2,3,4,5});
    shared_span<double> span2(span);

    EXPECT_EQ(span.size(), span2.size());
    
    for(size_t i = 0; i < span.size(); i++) {
      EXPECT_DOUBLE_EQ(span2[i], span[i]); 
    }
  }

};
