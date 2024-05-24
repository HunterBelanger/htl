#ifndef HTL_STATIC_ARENA_H
#define HTL_STATIC_ARENA_H

#include "details/base_arena.hpp"

namespace htl {

template <std::size_t CAPACITY>
class static_arena : public details::base_arena {
 public:
  static_arena() {
    if (CAPACITY > 0) {
      data_ = &data_array_[0];
      offset_ = data_;
      prev_offset_ = offset_;
      end_ = data_ + CAPACITY;
    }
  }

  // Because the memory is stored within the class, we cannot safely
  // copy or move instances of a static arena.
  static_arena(const static_arena& other) = delete;
  static_arena& operator=(const static_arena& other) = delete;
  static_arena(static_arena&& other) = delete;
  static_arena& operator=(static_arena&& other) = delete;

 private:
  std::byte data_array_[CAPACITY];
};

}  // namespace htl

#endif
