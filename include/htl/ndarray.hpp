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
#ifndef HTL_NDARRAY
#define HTL_NDARRAY

#include <iterator>
#include <memory>
#include <vector>

namespace htl {

/*
 *  Class: htl::ndarray<T>
 *
 *  Description:
 *
 * */
template <class T>
class ndarray {
 public:
  // constants and types
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = pointer;
  using const_iterator = const iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = const reverse_iterator;

  // constructors, copy, move, and assignment
  template <typename... SHAPE>
  ndarray(SHAPE... shape, const_reference value = T(), bool row_major = true);
  ndarray(std::vector<size_type> shape, const_reference value = T(),
          bool row_major = true);

  ndarray(const ndarray& other) = default;
  ndarray(ndarray&& other) = default;
  constexpr ndarray operator=(const ndarray& other) = default;
  ~ndarray() = default;

  // observers
  constexpr size_type size() const noexcept;
  constexpr std::vector<size_type> shape() const noexcept;
  constexpr size_type dimensions() const noexcept;
  constexpr bool row_major() const noexcept;

  // element access
  constexpr reference operator[](size_type idx);
  constexpr const_reference operator[](size_type idx) const;

  template <typename... INDS>
  constexpr reference operator()(INDS... inds);
  template <typename... INDS>
  constexpr const_reference operator()(INDS... inds) const;
  constexpr reference operator()(std::vector<size_type> idx);
  constexpr const_reference operator()(std::vector<size_type> idx) const;

  template <typename... INDS>
  constexpr reference at(INDS... inds);
  template <typename... INDS>
  constexpr const_reference at(INDS... inds) const;
  constexpr reference at(std::vector<size_type> idx) const;
  constexpr const_reference at(std::vector<size_type> idx);

  constexpr pointer data() const noexcept;

  // other
  constexpr void reshape(std::vector<size_type> new_shape);
  constexpr void fill(const_reference value) noexcept;

  // iterator support
  constexpr iterator begin() noexcept;
  constexpr const_iterator begin() const noexcept;
  constexpr const_iterator cbegin() const noexcept;

  constexpr iterator end() noexcept;
  constexpr const_iterator end() const noexcept;
  constexpr const_iterator cend() const noexcept;

  constexpr reverse_iterator rbegin() noexcept;
  constexpr const_reverse_iterator rbegin() const noexcept;
  constexpr const_reverse_iterator crbegin() const noexcept;

  constexpr reverse_iterator rend() noexcept;
  constexpr const_reverse_iterator rend() const noexcept;
  constexpr const_reverse_iterator crend() const noexcept;

 private:
  std::vector<value_type> data_;
  std::vector<size_type> shape_;
  bool row_major_;
};

};  // namespace htl

#endif  // HTL_NDARRAY
