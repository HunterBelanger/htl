#ifndef HTL_STATIC_VECTOR_H
#define HTL_STATIC_VECTOR_H

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace htl {

template <class T, std::size_t CAPACITY>
class static_vector {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  static_vector() : data_(), size_(0) {}

  static_vector(std::size_t count, const_reference value) : data_(), size_(0) {
    check_capacity(count);

    try {
      for (size_type i = 0; i < count; i++) {
        this->push_back(value);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  explicit static_vector(std::size_t count) : data_(), size_(0) {
    check_capacity(count);

    try {
      for (size_type i = 0; i < count; i++) {
        new (static_cast<void*>(&data_[size_])) value_type();
        size_++;
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  template <class InputIt>
  static_vector(InputIt first, InputIt last) : data_(), size_(0) {
    check_capacity(std::distance(first, last));

    try {
      for (auto it = first; it != last; it++) {
        push_back(*it);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  static_vector(const static_vector& other) : data_(), size_(0) {
    try {
      for (const auto& val : other) {
        push_back(val);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  static_vector(static_vector&& other) : data_(), size_(0) {
    try {
      for (std::size_t i = 0; i < other.size(); i++) {
        push_back(std::move(other[i]));
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  static_vector(std::initializer_list<value_type> init) : data_(), size_(0) {
    check_capacity(init.size());

    try {
      for (auto it = init.begin(); it != init.end(); it++) {
        push_back(*it);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  static_vector& operator=(const static_vector& other) {
    clear();

    try {
      for (const auto& item : other) {
        push_back(item);
      }
    } catch (...) {
      clear();
      throw;
    }

    return *this;
  }

  static_vector& operator=(static_vector&& other) noexcept {
    clear();

    try {
      for (size_type i = 0; i < other.size(); i++) {
        push_back(std::move(other[i]));
      }
    } catch (...) {
      clear();
      throw;
    }

    return *this;
  }

  static_vector& operator=(std::initializer_list<value_type> init) {
    check_capacity(init.size());

    try {
      for (auto it = init.begin(); it != init.end(); it++) {
        push_back(*it);
      }
    } catch (...) {
      clear();
      throw;
    }

    return *this;
  }

  ~static_vector() {
    // Destroy all objects we contain first
    clear();
  }

  reference at(std::size_t i) {
    check_index(i);

    return index(i);
  }

  const_reference at(std::size_t i) const {
    check_index(i);

    return index(i);
  }

  reference operator[](std::size_t i) { return index(i); }

  const_reference operator[](std::size_t i) const { return index(i); }

  reference front() { return index(0); }

  const_reference front() const { return index(0); }

  reference back() { return index(size_ - 1); }

  const_reference back() const { return index(size_ - 1); }

  pointer data() { return reinterpret_cast<pointer>(&data_[0]); }

  const_pointer data() const {
    return reinterpret_cast<const_pointer>(&data_[0]);
  }

  bool empty() const noexcept { return size_ == 0; }

  size_type size() const noexcept { return size_; }

  constexpr size_type max_size() const noexcept { return capacity(); }

  constexpr size_type capacity() const noexcept { return CAPACITY; }

  void clear() noexcept {
    if ((std::is_trivially_destructible<value_type>::value == false) &&
        (size_ > 0)) {
      // Call all destructors
      for (size_type i = 0; i < size_; i++) {
        index(i).~value_type();
      }
    }

    size_ = 0;
  }

  iterator insert(const_iterator pos, const_reference value) {
    check_capacity(1);
    const size_type indx = pos - begin();

    // First, move all objects forward one spot
    std::move_backward(begin() + indx, end(), end() + 1);

    // Now insert new element
    new (begin() + indx) value_type(value);
    size_++;

    return begin() + indx;
  }

  iterator insert(const_iterator pos, value_type&& value) {
    check_capacity(1);
    const size_type indx = pos - begin();

    // First, move all objects forward one spot
    std::move_backward(begin() + indx, end(), end() + 1);

    // Now insert new element
    new (begin() + indx) value_type(value);
    size_++;

    return begin() + indx;
  }

  iterator insert(const_iterator pos, size_type count, const_reference value) {
    check_capacity(count);
    const size_type indx = pos - begin();

    // First, move all objects forward count spots
    std::move_backward(begin()+indx, end(), end() + count);

    // Now insert new element
    for (size_type i = 0; i < count; i++) {
      new (begin() + indx + i) value_type(value);
      size_++;
    }

    return begin() + indx;
  }

  template <class InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    const size_type count = std::distance(first, last);

    check_capacity(count);
    const size_type indx = pos - begin();

    // First, move all objects forward count spots
    std::move_backward(begin() + indx, end(), end() + count);

    // Now insert new element
    for (size_type i = 0; i < count; i++) {
      new (begin() + indx + i) value_type(first++);
      size_++;
    }

    return begin() + indx;
  }

  iterator insert(const_iterator pos, std::initializer_list<value_type> init) {
    check_capacity(init.size());
    const size_type indx = pos - begin();

    // First, move all objects forward one spot
    std::move_backward(begin() + indx, end(), end() + init.size());

    // Now insert new element
    size_type i = 0;
    for (auto it = init.begin(); it != init.end(); it++) {
      new (begin() + indx + i) value_type(*it);
      size_++;
      i++;
    }

    return begin() + indx;
  }

  template <class... Args>
  iterator emplace(const_iterator pos, Args... args) {
    check_capacity(1);
    const size_type indx = pos - begin();

    // First, move all objects forward one spot
    std::move_backward(begin() + indx, end(), end() + 1);

    // Now insert new element
    new (begin() + indx) value_type(args...);
    size_++;

    return begin() + indx;
  }

  iterator erase(const_iterator pos) {
    const size_type indx = pos - begin();

    // First, destroy object at pos
    pos->~value_type();

    // Move all objects backwards one spot
    std::move(begin() + indx + 1, end(), begin() + indx);

    // Reduce size
    size_--;

    return begin() + indx;
  }

  iterator erase(const_iterator first, const_iterator last) {
    if (first == last) return last;
    const size_type indx_first = first - begin();
    const size_type indx_last = last - begin();

    // First, destroy object at pos
    for (iterator it = first; it != last; it++) {
      it->~value_type();
    }

    // Move all objects backwards one spot
    std::move(begin() + indx_last, end(), begin() + indx_first);

    // Reduce size
    size_ -= std::distance(first, last);

    return begin() + indx_first;
  }

  void push_back(const_reference value) {
    check_capacity();

    new (static_cast<void*>(&data_[size_])) value_type(value);
    size_++;
  }

  void push_back(value_type&& value) {
    check_capacity();

    new (static_cast<void*>(&data_[size_])) value_type(value);
    size_++;
  }

  template <class... Args>
  void emplace_back(Args&&... args) {
    check_capacity();

    new (static_cast<void*>(&data_[size_])) value_type(args...);
    size_++;
  }

  void pop_back() {
    if (size_ > 0) {
      index(size_ - 1).~value_type();
      size_--;
    }
  }

  void resize(size_type count) {
    const size_type diff = count < size_ ? size_ - count : count - size_;

    if (count < size_) {
      for (size_type i = 0; i < diff; i++) pop_back();
    } else if (count > size_) {
      check_capacity(diff);

      for (size_type i = 0; i < diff; i++) {
        new (static_cast<void*>(&data_[size_])) value_type();
        size_++;
      }
    }
  }

  void resize(size_type count, const_reference value) {
    const size_type diff = count < size_ ? size_ - count : count - size_;

    if (count < size_) {
      for (size_type i = 0; i < diff; i++) pop_back();
    } else if (count > size_) {
      check_capacity(diff);

      for (size_type i = 0; i < diff; i++) {
        new (static_cast<void*>(&data_[size_])) value_type(value);
        size_++;
      }
    }
  }

  iterator begin() noexcept { return reinterpret_cast<iterator>(&data_[0]); }

  const_iterator begin() const noexcept {
    return reinterpret_cast<const_iterator>(&data_[0]);
  }

  const_iterator cbegin() const noexcept {
    return reinterpret_cast<const_iterator>(&data_[0]);
  }

  iterator end() noexcept { return reinterpret_cast<iterator>(&data_[size_]); }

  const_iterator end() const noexcept {
    return reinterpret_cast<const_iterator>(&data_[size_]);
  }

  const_iterator cend() const noexcept {
    return reinterpret_cast<const_iterator>(&data_[size_]);
  }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const noexcept {
    return reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const noexcept {
    return reverse_iterator(end());
  }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const noexcept {
    return reverse_iterator(begin());
  }

  const_reverse_iterator crend() const noexcept {
    return reverse_iterator(begin());
  }

 private:
  struct storage_type {
    alignas(value_type) unsigned char data[sizeof(value_type)];
  };

  storage_type data_[CAPACITY];
  size_type size_;

  void check_index(const std::size_t& i) {
    if (i >= size_)
      throw std::out_of_range(
          "htl::static_vector::check_index: i >= this->size()");
  }

  void check_capacity() {
    if (size_ == capacity()) {
      throw std::length_error(
          "htl::static_vector::check_capacity: Cannot extend capacity");
    }
  }

  void check_capacity(const size_type& count_to_add) {
    if (size_ + count_to_add > capacity()) {
      throw std::length_error(
          "htl::static_vector::check_capacity: Cannot extend capacity");
    }
  }

  reference index(const std::size_t& i) {
    return *reinterpret_cast<pointer>(&data_[i]);
  }

  const_reference index(const std::size_t& i) const {
    return *reinterpret_cast<const_pointer>(&data_[i]);
  }
};
}  // namespace htl

#endif
