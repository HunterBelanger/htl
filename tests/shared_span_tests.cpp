#include <htl/shared_span.hpp>
#include <gtest/gtest.h>

namespace {

  using namespace htl;

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

  TEST(shared_span, subspan_copy_constructor) {
    shared_span<double> span({1,2,3,4,5});
    shared_span<double> span2(span, 2, 3);

    EXPECT_EQ(span2.size(), 3);
    
    for(size_t i = 0; i < span2.size(); i++) {
      EXPECT_DOUBLE_EQ(span2[i], span[i+2]); 
    }
  }

  TEST(shared_span, first) {
    shared_span<size_t> span({0,1,2,3,4,5,6,7,8}); 

    shared_span<size_t> first_4 = span.first(4);

    EXPECT_EQ(first_4.size(), 4);

    for(size_t i = 0; i < first_4.size(); i++) {
      EXPECT_EQ(i, first_4[i]); 
    }
  } 

  TEST(shared_span, last) {
    shared_span<size_t> span({0,1,2,3,4,5,6,7}); 

    shared_span<size_t> last_4 = span.last(4);

    EXPECT_EQ(last_4.size(), 4);

    for(size_t i = 0; i < last_4.size(); i++) {
      EXPECT_EQ(i+4, last_4[i]); 
    } 
  }

  TEST(shared_span, subspan) {
    shared_span<size_t> span({0,1,2,3,4,5,6,7}); 

    shared_span<size_t> sub_4 = span.subspan(2, 4);

    EXPECT_EQ(sub_4.size(), 4);

    for(size_t i = 0; i < sub_4.size(); i++) {
      EXPECT_EQ(i+2, sub_4[i]); 
    }
  }

  TEST(shared_span, size) {
    shared_span<size_t> span_not_empty({0,1,2,3,4,5,6,7}); 
    shared_span<size_t> span_empty({});

    EXPECT_EQ(span_empty.size(), 0);
    EXPECT_EQ(span_not_empty.size(), 8);
  }

  TEST(shared_span, size_bytes) {
    shared_span<size_t> span_not_empty({0,1,2,3,4,5,6,7}); 
    shared_span<size_t> span_empty({});

    EXPECT_EQ(span_empty.size_bytes(), 0);
    EXPECT_EQ(span_not_empty.size_bytes(), 8*sizeof(size_t));
  }

  TEST(shared_span, empty) {
    shared_span<size_t> span_not_empty({0,1,2,3,4,5,6,7}); 
    shared_span<size_t> span_empty({});

    EXPECT_TRUE(span_empty.empty());
    EXPECT_FALSE(span_not_empty.empty());
  }

  TEST(shared_span, indexing) {
    shared_span<size_t> span({0,1,2,3,4,5,6,7});

    for(size_t i = 0; i < span.size(); i++) {
      EXPECT_EQ(i, span[i]); 
    }
  }

  TEST(shared_span, front) {
    shared_span<double> span({3,4,5,6,7}); 

    EXPECT_DOUBLE_EQ(span.front(), 3.);
  }

  TEST(shared_span, back) {
    shared_span<double> span({3,4,5,6,7}); 

    EXPECT_DOUBLE_EQ(span.back(), 7.);
  }

  TEST(shared_span, data) {
    shared_span<double> span({3,4,5,6,7}); 

    double* data_ptr = span.data();

    for(size_t i = 0; i < span.size(); i++) {
      EXPECT_DOUBLE_EQ(span[i], *(data_ptr+i));
    }
  }

  TEST(shared_span, begin) {
     shared_span<double> span({3,4,5,6,7});

     EXPECT_DOUBLE_EQ(*span.begin(), span.front());
  }

  TEST(shared_span, end) {
     shared_span<double> span({3,4,5,6,7});

     auto begin = span.begin();
     auto end = span.end();

     size_t size = end - begin;

     EXPECT_EQ(size, span.size());
     EXPECT_DOUBLE_EQ(*(--end), span.back());
  }

  TEST(shared_span, rbegin) {
     shared_span<size_t> span({0,1,2,3,4,5,6,7});

     EXPECT_EQ(*span.rbegin(), span.back());
      
     size_t i = 7;
     for(auto it = span.rbegin(); it != span.rend(); it++) {
        EXPECT_EQ(*it, i);
        if(i != 0) i--;
     }
  }

  TEST(shared_span, rend) {
     shared_span<size_t> span({0,1,2,3,4,5,6,7});

     auto rend = span.rend();

     EXPECT_EQ(*(--rend), span.front());
  }

};
