#ifndef HTL_NDARRAY_H
#define HTL_NDARRAY_H

#include <algorithm>
#include <array>
#include <complex>
#include <stdexcept>
#include <string>
#include <vector>

#include "details/npy.hpp"

namespace htl {

template <class T>
class ndarray {
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

  ndarray() : data_(), shape_(), c_continuous_(true), dimensions_(0) {}

  ndarray(std::vector<size_type> init_shape, bool c_continuous = true)
      : data_(), shape_(), c_continuous_(true) {
    if (init_shape.size() > 0) {
      shape_ = init_shape;
      dimensions_ = shape_.size();

      size_type ne = init_shape[0];
      for (size_type i = 1; i < dimensions_; i++) {
        ne *= init_shape[i];
      }

      data_.resize(ne);

      c_continuous_ = c_continuous;
    } else {
      throw std::runtime_error(
          "htl::ndarray shape vector must have at least one element");
    }
  }

  ndarray(std::vector<value_type> data, std::vector<size_type> init_shape,
          bool c_continuous = true)
      : data_(), shape_(), c_continuous_(true) {
    if (init_shape.size() > 0) {
      shape_ = init_shape;
      dimensions_ = shape_.size();

      size_type ne = init_shape[0];
      for (size_type i = 1; i < dimensions_; i++) {
        ne *= init_shape[i];
      }

      if (ne != data.size()) {
        throw std::runtime_error(
            "htl::ndarray: shape is incompatible with number of elements");
      }

      data_ = data;

      c_continuous_ = c_continuous;
    } else {
      throw std::runtime_error(
          "htl::ndarray: shape vector must have at least one element");
    }
  }

  [[nodiscard]] reference operator()(const std::vector<size_type>& indices) {
    size_type indx = c_continuous_ ? c_continuous_index(indices)
                                   : fortran_continuous_index(indices);
    return data_[indx];
  }

  [[nodiscard]] const_reference operator()(const std::vector<size_type>& indices) const {
    size_type indx = c_continuous_ ? c_continuous_index(indices)
                                   : fortran_continuous_index(indices);
    return data_[indx];
  }

  template <typename... INDS>
  [[nodiscard]] reference operator()(INDS... inds) {
    std::array<size_type, sizeof...(inds)> indices{
        static_cast<size_type>(inds)...};

    size_type indx = c_continuous_ ? c_continuous_index(indices)
                                   : fortran_continuous_index(indices);
    return data_[indx];
  }

  template <typename... INDS>
  [[nodiscard]] const_reference operator()(INDS... inds) const {
    std::array<size_type, sizeof...(inds)> indices{
        static_cast<size_type>(inds)...};

    size_type indx = c_continuous_ ? c_continuous_index(indices)
                                   : fortran_continuous_index(indices);
    return data_[indx];
  }

  [[nodiscard]] reference at(const std::vector<size_type>& indices) {
    size_type indx = c_continuous_ ? at_c_continuous_index(indices)
                                   : at_fortran_continuous_index(indices);
    return data_[indx];
  }

  [[nodiscard]] const_reference at(const std::vector<size_type>& indices) const {
    size_type indx = c_continuous_ ? at_c_continuous_index(indices)
                                   : at_fortran_continuous_index(indices);
    return data_[indx];
  }

  template <typename... INDS>
  [[nodiscard]] reference at(INDS... inds) {
    std::array<size_type, sizeof...(inds)> indices{
        static_cast<size_type>(inds)...};

    size_type indx = c_continuous_ ? at_c_continuous_index(indices)
                                   : at_fortran_continuous_index(indices);
    return data_[indx];
  }

  template <typename... INDS>
  [[nodiscard]] const_reference at(INDS... inds) const {
    std::array<size_type, sizeof...(inds)> indices{
        static_cast<size_type>(inds)...};

    size_type indx = c_continuous_ ? at_c_continuous_index(indices)
                                   : at_fortran_continuous_index(indices);
    return data_[indx];
  }

  [[nodiscard]] reference operator[](size_type i) { return data_[i]; }

  [[nodiscard]] const_reference operator[](size_type i) const { return data_[i]; }

  [[nodiscard]] std::vector<size_type> shape() const { return shape_; }

  [[nodiscard]] size_type size() const { return data_.size(); }

  [[nodiscard]] size_type linear_index(const std::vector<size_type>& indices) const {
    return c_continuous_ ? at_c_continuous_index(indices)
                         : at_fortran_continuous_index(indices);
  }

  template <typename... INDS>
  [[nodiscard]] size_type linear_index(INDS... inds) const {
    std::array<size_type, sizeof...(inds)> indices{
        static_cast<size_type>(inds)...};
    return c_continuous_ ? at_c_continuous_index(indices)
                         : at_fortran_continuous_index(indices);
  }

  [[nodiscard]] bool c_continuous() const { return c_continuous_; }

  [[nodiscard]] static ndarray load(std::string fname) {
    using namespace details;

    // Get expected DType according to T
    DType expected_dtype;
    const char* T_type_name = typeid(value_type).name();

    if (T_type_name == typeid(char).name())
      expected_dtype = DType::CHAR;
    else if (T_type_name == typeid(unsigned char).name())
      expected_dtype = DType::UCHAR;
    else if (T_type_name == typeid(uint16_t).name())
      expected_dtype = DType::UINT16;
    else if (T_type_name == typeid(uint32_t).name())
      expected_dtype = DType::UINT32;
    else if (T_type_name == typeid(uint64_t).name())
      expected_dtype = DType::UINT64;
    else if (T_type_name == typeid(int16_t).name())
      expected_dtype = DType::INT16;
    else if (T_type_name == typeid(int32_t).name())
      expected_dtype = DType::INT32;
    else if (T_type_name == typeid(int64_t).name())
      expected_dtype = DType::INT64;
    else if (T_type_name == typeid(float).name())
      expected_dtype = DType::FLOAT32;
    else if (T_type_name == typeid(double).name())
      expected_dtype = DType::DOUBLE64;
    else if (T_type_name == typeid(std::complex<float>).name())
      expected_dtype = DType::COMPLEX64;
    else if (T_type_name == typeid(std::complex<double>).name())
      expected_dtype = DType::COMPLEX128;
    else {
      throw std::runtime_error(
          "htl::ndarray: the datatype is not supported by the npy format");
    }

    // Variables to send to npy function
    char* data_ptr;
    std::vector<value_type> data_vector;
    std::vector<size_type> data_shape;
    DType data_dtype;
    bool data_c_continuous;

    // Load data into variables
    load_npy(fname, data_ptr, data_shape, data_dtype, data_c_continuous);

    // Ensure DType variables match
    if (expected_dtype != data_dtype) {
      throw std::runtime_error(
          "htl::ndarray: template datatype does not match specified datatype "
          "in npy file");
    }

    if (data_shape.size() < 1) {
      throw std::runtime_error(
          "htl::ndarray: shape vector must have at least one element");
    }

    // Number of elements
    size_type ne = data_shape[0];
    for (size_type i = 1; i < data_shape.size(); i++) {
      ne *= data_shape[i];
    }

    data_vector = {reinterpret_cast<pointer>(data_ptr),
                   reinterpret_cast<pointer>(data_ptr) + ne};

    // Create ndarray object
    ndarray<value_type> return_object(data_vector, data_shape);
    return_object.c_continuous_ = data_c_continuous;

    // Free data_ptr
    delete[] data_ptr;

    // Return object
    return return_object;
  }

  void save(const std::string& fname) const {
    using namespace details;

    // Get expected DType according to T
    DType dtype;
    const char* T_type_name = typeid(value_type).name();

    if (T_type_name == typeid(char).name())
      dtype = DType::CHAR;
    else if (T_type_name == typeid(unsigned char).name())
      dtype = DType::UCHAR;
    else if (T_type_name == typeid(uint16_t).name())
      dtype = DType::UINT16;
    else if (T_type_name == typeid(uint32_t).name())
      dtype = DType::UINT32;
    else if (T_type_name == typeid(uint64_t).name())
      dtype = DType::UINT64;
    else if (T_type_name == typeid(int16_t).name())
      dtype = DType::INT16;
    else if (T_type_name == typeid(int32_t).name())
      dtype = DType::INT32;
    else if (T_type_name == typeid(int64_t).name())
      dtype = DType::INT64;
    else if (T_type_name == typeid(float).name())
      dtype = DType::FLOAT32;
    else if (T_type_name == typeid(double).name())
      dtype = DType::DOUBLE64;
    else if (T_type_name == typeid(std::complex<float>).name())
      dtype = DType::COMPLEX64;
    else if (T_type_name == typeid(std::complex<double>).name())
      dtype = DType::COMPLEX128;
    else {
      throw std::runtime_error(
          "htl::ndarray: shape vector must have at least one element");
    }

    // Write data to file
    write_npy(fname, reinterpret_cast<const char*>(data_.data()), shape_, dtype,
              c_continuous_);
  }

  void fill(const_reference val) { std::fill(data_.begin(), data_.end(), val); }

  void reshape(std::vector<size_type> new_shape) {
    // Ensure new shape has proper dimensions
    if (new_shape.size() < 1) {
      throw std::runtime_error(
          "htl::ndarray: shape vector must have at least one element to "
          "reshpae");
    } else {
      size_type ne = new_shape[0];

      for (size_type i = 1; i < new_shape.size(); i++) {
        ne *= new_shape[i];
      }

      if (ne == data_.size()) {
        shape_ = new_shape;
        dimensions_ = shape_.size();
      } else {
        throw std::runtime_error(
            "htl::ndarray: new shape is incompatible with number of elements");
      }
    }
  }

  void reallocate(std::vector<size_type> new_shape) {
    // Ensure new shape has proper dimensions
    if (new_shape.size() < 1) {
      throw std::runtime_error(
          "htl::ndarray: shape vector must have at least one element to "
          "reallocate");
    } else {
      size_type ne = new_shape[0];

      for (size_type i = 1; i < new_shape.size(); i++) {
        ne *= new_shape[i];
      }

      shape_ = new_shape;
      dimensions_ = shape_.size();
      data_.resize(ne);
    }
  }

  [[nodiscard]] iterator begin() noexcept { return reinterpret_cast<iterator>(&data_[0]); }

  [[nodiscard]] const_iterator begin() const noexcept {
    return reinterpret_cast<const_iterator>(&data_[0]);
  }

  [[nodiscard]] const_iterator cbegin() const noexcept {
    return reinterpret_cast<const_iterator>(&data_[0]);
  }

  [[nodiscard]] iterator end() noexcept {
    return reinterpret_cast<iterator>(&data_[data_.size()]);
  }

  [[nodiscard]] const_iterator end() const noexcept {
    return reinterpret_cast<const_iterator>(&data_[data_.size()]);
  }

  [[nodiscard]] const_iterator cend() const noexcept {
    return reinterpret_cast<const_iterator>(&data_[data_.size()]);
  }

  [[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return reverse_iterator(end());
  }

  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return reverse_iterator(end());
  }

  [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return reverse_iterator(begin());
  }

  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return reverse_iterator(begin());
  }

 private:
  std::vector<value_type> data_;
  std::vector<size_type> shape_;
  bool c_continuous_;
  size_type dimensions_;

  template <class V>
  [[nodiscard]] size_type at_c_continuous_index(const V& indices) const {
    // Make sure proper number of indices
    if (indices.size() != dimensions_) {
      throw std::runtime_error(
          "htl::ndarray: improper number of indicies provided");
    }

    size_type indx = indices[dimensions_ - 1];
    if (indx >= shape_[dimensions_ - 1]) {
      throw std::out_of_range("htl::ndarray: provided index out of range");
    }

    size_type coeff = 1;

    for (size_type i = dimensions_ - 1; i > 0; i--) {
      if (indices[i] >= shape_[i]) {
        throw std::out_of_range("htl::ndarray: provided index out of range");
      }

      coeff *= shape_[i];
      indx += coeff * indices[i - 1];
    }

    return indx;
  }

  template <class V>
  [[nodiscard]] size_type c_continuous_index(const V& indices) const {
    size_type indx = indices[dimensions_ - 1];
    size_type coeff = 1;

    for (size_type i = dimensions_ - 1; i > 0; i--) {
      coeff *= shape_[i];
      indx += coeff * indices[i - 1];
    }

    return indx;
  }

  template <class V>
  [[nodiscard]] size_type at_fortran_continuous_index(const V& indices) const {
    // Make sure proper number of indices
    if (indices.size() != dimensions_) {
      throw std::runtime_error(
          "htl::ndarray: improper number of indicies provided");
    }

    size_t indx = indices[0];
    if (indx >= shape_[0]) {
      throw std::out_of_range("htl::ndarray: provided index out of range");
    }
    size_t coeff = 1;

    for (size_t i = 0; i < dimensions_ - 1; i++) {
      if (indices[i] >= shape_[i]) {
        throw std::out_of_range("htl::ndarray: provided index out of range");
      }

      coeff *= shape_[i];
      indx += coeff * indices[i + 1];
    }

    return indx;
  }

  template <class V>
  [[nodiscard]] size_type fortran_continuous_index(const V& indices) const {
    size_t indx = indices[0];
    size_t coeff = 1;

    for (size_t i = 0; i < dimensions_ - 1; i++) {
      coeff *= shape_[i];
      indx += coeff * indices[i + 1];
    }

    return indx;
  }
};

}  // namespace htl

#endif
