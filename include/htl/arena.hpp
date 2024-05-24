#ifndef HTL_ARENA_H
#define HTL_ARENA_H

#include "details/base_arena.hpp"

namespace htl {

class arena : public details::base_arena {
 public:
  arena(std::size_t capacity = 0) {
    if (capacity > 0) {
      data_ = new std::byte[capacity];
      offset_ = data_;
      prev_offset_ = offset_;
      end_ = data_ + capacity;
    }
  }

  arena(arena&& other) {
    this->data_ = other.data_;
    this->offset_ = other.offset_;
    this->prev_offset_ = other.prev_offset_;
    this->end_ = other.end_;

    other.data_ = nullptr;
    other.offset_ = nullptr;
    other.prev_offset_ = nullptr;
    other.end_ = nullptr;
  }

  arena& operator=(arena&& other) {
    this->deallocate();

    this->data_ = other.data_;
    this->offset_ = other.offset_;
    this->prev_offset_ = other.prev_offset_;
    this->end_ = other.end_;

    other.data_ = nullptr;
    other.offset_ = nullptr;
    other.prev_offset_ = nullptr;
    other.end_ = nullptr;

    return *this;
  }

  ~arena() { this->deallocate(); }

  // No copying Arenas ! This can lead to a use after free if one of them goes
  // out of scope, and then deallocates the data array.
  arena(const arena& other) = delete;
  arena& operator=(const arena& other) = delete;

 private:
  void deallocate() {
    if (data_) {
      delete[] data_;
      data_ = nullptr;
      offset_ = nullptr;
      prev_offset_ = nullptr;
      end_ = nullptr;
    }
  }
};

}  // namespace htl

#endif
