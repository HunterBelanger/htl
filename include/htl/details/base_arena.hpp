#ifndef HTL_DETAILS_BASE_ARENA_H
#define HTL_DETAILS_BASE_ARENA_H

#include <cstddef>

namespace htl {
namespace details {

class base_arena {
 public:
  void* malloc(std::size_t size) {
    if (size > remaining() || size == 0) {
      return nullptr;
    }

    // Ensure that the returned address will be suitably aligned for any type.
    constexpr std::size_t MAX_ALIGN = alignof(std::max_align_t);
    const std::size_t modulo =
        reinterpret_cast<std::size_t>(offset_) % MAX_ALIGN;
    if (modulo > 0) {
      if ((size + MAX_ALIGN - modulo) > remaining()) {
        return nullptr;
      }

      offset_ += MAX_ALIGN - modulo;
    }

    void* out = reinterpret_cast<void*>(offset_);

    prev_offset_ = offset_;
    offset_ += size;

    return out;
  }

  template <typename T, typename... Types>
  T* make(Types... args) {
    T* out = reinterpret_cast<T*>(malloc(sizeof(T)));

    if (out == nullptr) {
      return nullptr;
    }

    new (out) T(args...);

    return out;
  }

  template <typename T>
  auto dtor() {
    return [](T* ptr) { ptr->~T(); };
  }

  std::size_t capacity() const {
    return static_cast<std::size_t>(end_ - data_);
  }

  std::size_t remaining() const {
    return static_cast<std::size_t>(end_ - offset_);
  }

  void clear() {
    unsigned char* data = reinterpret_cast<unsigned char*>(data_);

    for (std::size_t i = 0; i < capacity(); i++) data[i] = 0;

    offset_ = data_;
    prev_offset_ = data_;
  }

 protected:
  std::byte* data_;         // Beginning of allocation
  std::byte* offset_;       // Beginning of unused memory
  std::byte* prev_offset_;  // Previous offset
  std::byte* end_;          // End of the allocation

  base_arena()
      : data_(nullptr),
        offset_(nullptr),
        prev_offset_(nullptr),
        end_(nullptr) {}
};

}  // namespace details
}  // namespace htl

#endif
