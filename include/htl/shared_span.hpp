/*  BSD 3-Clause License
 *
 *  Copyright (c) 2020, Hunter Belanger (hunter.belanger@gmail.com)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  3. Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * */
#ifndef HTL_SHARED_SPAN
#define HTL_SHARED_SPAN

#include <iterator>
#include <memory>
#include <vector>

namespace htl {

/*
 *  Class: htl::shared_span<T>
 *
 *  Description: This class provides an object similar in spirit to a standard
 *  std::span (C++20 standard), except that the span owns the data, via a
 *  std::shared_ptr. In this manner, multiple shared_span objects can point to
 *  the same data set, without requiring that the true data be stored in
 *  another object, which could eventually be destroyed, or invalidated (via a
 *  push_back to a std::vector for example). The data must be initialized when
 *  the shared_span is created, and data cannot be added after. Other spans over
 *  the same data can be made, which may reference any subset of the full data
 *  set. This class has more or less the same interface as std::span, with the
 *  addition of the use_count method, which functions the same way as it does
 *  with std::shared_ptr, giving the number of instances which use the
 *  underlying data set. The underlying data is only deleted when the last
 *  instance of shared_span using that data is destroyed. This also ensures
 *  that all shared_spans remain valid for the duration of the existance. The
 *  class uses indicies begin_ and end_ to describe the valid range of the
 *  span, instead of using a pointer and size. This is done to facilitate
 *  serialization of shared_span instances, as many serialization libraries
 *  (such as cereal) cannot serialize raw pointers, only smart pointers. This
 *  allows the class to be easily modified to work with such libraries.
 *
 * */
template <class T>
class shared_span {
 public:
  // constants and types
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = element_type*;
  using const_pointer = const element_type*;
  using reference = element_type&;
  using const_reference = const element_type&;
  using iterator = pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;

  // constructors, copy, move, and assignment
  template <class InputIt>
  shared_span(InputIt first, InputIt last);
  shared_span(std::initializer_list<element_type> init);
  shared_span(const shared_span& other, std::size_t Offset, std::size_t Count);

  shared_span(const shared_span& other) = default;
  shared_span(shared_span&& other) = default;
  constexpr shared_span& operator=(const shared_span& other) noexcept = default;
  ~shared_span() noexcept = default;

  // subviews
  constexpr shared_span first(std::size_t Count) const;
  constexpr shared_span last(std::size_t Count) const;
  constexpr shared_span subspan(std::size_t Offset, std::size_t Count) const;

  // observers
  constexpr size_type size() const noexcept;
  constexpr size_type size_bytes() const noexcept;
  constexpr bool empty() const noexcept;
  long use_count() const noexcept;

  // element access
  constexpr reference operator[](size_type idx) const;
  constexpr reference front() const;
  constexpr reference back() const;
  constexpr pointer data() const noexcept;

  // iterator support
  constexpr iterator begin() const noexcept;
  constexpr iterator end() const noexcept;
  constexpr reverse_iterator rbegin() const noexcept;
  constexpr reverse_iterator rend() const noexcept;

 private:
  std::shared_ptr<std::vector<element_type>> data_;
  size_type begin_;
  size_type end_;
};

template <class T>
inline shared_span<T>::shared_span(std::initializer_list<element_type> init)
    : data_{nullptr}, begin_{0}, end_{0} {
  data_ = std::make_shared<std::vector<element_type>>(init);
  begin_ = 0;
  end_ = data_->size();
}

template <class T>
template <class InputIt>
inline shared_span<T>::shared_span(InputIt first, InputIt last)
    : data_{nullptr}, begin_{0}, end_{0} {
  data_ = std::make_shared<std::vector<element_type>>(first, last);
  begin_ = 0;
  end_ = data_->size();
}

template <class T>
inline shared_span<T>::shared_span(const shared_span& other, std::size_t Offset,
                                   std::size_t Count)
    : data_{other.data_}, begin_{other.begin_}, end_{other.end_} {
  begin_ += Offset;
  if (begin_ + Count <= end_) end_ = begin_ + Count;
}

template <class T>
inline constexpr shared_span<T> shared_span<T>::first(std::size_t Count) const {
  return shared_span(*this, 0, Count);
}

template <class T>
inline constexpr shared_span<T> shared_span<T>::last(std::size_t Count) const {
  if (Count > (end_ - begin_)) return shared_span(*this);
  return shared_span(*this, end_ - Count, end_);
}

template <class T>
inline constexpr shared_span<T> shared_span<T>::subspan(
    std::size_t Offset, std::size_t Count) const {
  return {*this, Offset, Count};
}

template <class T>
inline constexpr typename shared_span<T>::size_type shared_span<T>::size() const
    noexcept {
  return end_ - begin_;
}

template <class T>
inline constexpr typename shared_span<T>::size_type shared_span<T>::size_bytes()
    const noexcept {
  return (end_ - begin_) * sizeof(element_type);
}

template <class T>
inline constexpr bool shared_span<T>::empty() const noexcept {
  return (end_ - begin_) == 0;
}

template <class T>
inline long shared_span<T>::use_count() const noexcept {
  return data_.use_count();
}

template <class T>
inline constexpr typename shared_span<T>::reference shared_span<T>::operator[](
    size_type idx) const {
  return (*data_)[begin_ + idx];
}

template <class T>
inline constexpr typename shared_span<T>::reference shared_span<T>::front()
    const {
  return (*data_)[begin_];
}

template <class T>
inline constexpr typename shared_span<T>::reference shared_span<T>::back()
    const {
  return (*data_)[end_ - 1];
}

template <class T>
inline constexpr typename shared_span<T>::pointer shared_span<T>::data() const
    noexcept {
  return data_->data() + begin_;
}

template <class T>
inline constexpr typename shared_span<T>::iterator shared_span<T>::begin() const
    noexcept {
  return data_->data() + begin_;
}

template <class T>
inline constexpr typename shared_span<T>::iterator shared_span<T>::end() const
    noexcept {
  return data_->data() + end_;
}

template <class T>
inline constexpr typename shared_span<T>::reverse_iterator
shared_span<T>::rbegin() const noexcept {
  return reverse_iterator(data_->data() + end_);
}

template <class T>
inline constexpr typename shared_span<T>::reverse_iterator
shared_span<T>::rend() const noexcept {
  return reverse_iterator(data_->data() + begin_);
}

};  // namespace htl

#endif  // HTL_SHARED_SPAN
