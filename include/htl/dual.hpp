#ifndef HTL_DUAL_H
#define HTL_DUAL_H

#include <cmath>
#include <concepts>

namespace htl {

template <std::floating_point T>
class dual {
 public:
  using value_type = T;
  using const_reference = const value_type&;

  constexpr dual(const_reference v = value_type(),
                 const_reference e = value_type())
      : value_(v), epsilon_(e) {}

  [[nodiscard]] constexpr value_type value() const { return value_; }
  constexpr void value(value_type v) { value_ = v; }

  [[nodiscard]] constexpr value_type epsilon() const { return epsilon_; }
  constexpr void epsilon(value_type e) { epsilon_ = e; }

  [[nodiscard]] dual operator+() const { return *this; }

  [[nodiscard]] dual operator-() const { return Dual(-value_, -epsilon_); }

  dual& operator+=(const dual& other) {
    this->value() += other.value();
    this->epsilon() += other.epsilon();
    return *this;
  }

  dual& operator+=(const_reference v) {
    this->value() += v;
    return *this;
  }

  dual& operator-=(const dual& other) {
    this->value() -= other.value();
    this->epsilon() -= other.epsilon();
    return *this;
  }

  dual& operator-=(const_reference v) {
    this->value() -= v;
    return *this;
  }

  dual& operator*=(const dual& other) {
    this->value() *= other.value();
    this->epsilon() =
        this->epsilon() * other.value() + this->value() * other.epsilon();
    return *this;
  }

  dual& operator*=(const_reference v) {
    this->value() *= v;
    this->epsilon() *= v;
    return *this;
  }

  dual& operator/=(const dual& other) {
    this->value() /= other.value();
    this->epsilon() =
        (this->epsilon() * other.value() - this->value() * other.epsilon()) /
        (other.value() * other.value());
    return *this;
  }

  dual& operator/=(const_reference v) {
    this->value() /= v;
    this->epsilon() /= v;
    return *this;
  }

  [[nodiscard]] dual operator+(const dual& other) const {
    dual out = *this;
    out += other;
    return out;
  }

  [[nodiscard]] dual operator+(const_reference v) const {
    dual out = *this;
    out += v;
    return out;
  }

  [[nodiscard]] dual operator-(const dual& other) const {
    dual out = *this;
    out -= other;
    return out;
  }

  [[nodiscard]] dual operator-(const_reference v) const {
    dual out = *this;
    out -= v;
    return out;
  }

  [[nodiscard]] dual operator*(const dual& other) const {
    dual out = *this;
    out *= other;
    return out;
  }

  [[nodiscard]] dual operator*(const_reference v) const {
    dual out = *this;
    out *= v;
    return out;
  }

  [[nodiscard]] dual operator/(const dual& other) const {
    dual out = *this;
    out /= other;
    return out;
  }

  [[nodiscard]] dual operator/(const_reference v) const {
    dual out = *this;
    out /= v;
    return out;
  }

 private:
  value_type value_, epsilon_;
};

//==========================================================
// abs
template <typename T>
[[nodiscard]] auto abs(T arg) {
  return std::abs(arg);
}

template <typename T>
[[nodiscard]] dual<T> abs(dual<T> arg) {
  return dual<T>(std::abs(arg.value()),
                 arg.value() >= 0. ? arg.epsilon() : -arg.epsilon());
}

//==========================================================
// pow
template <typename T1, typename T2>
[[nodiscard]] auto pow(T1 base, T2 exp) {
  return std::pow(base, exp);
}

template <typename T1, typename T2>
[[nodiscard]] dual<T1> pow(dual<T1> base, T2 exp) {
  return dual<T1>(std::pow(base.value(), exp),
                  base.epsilon() * exp * std::pow(base.value(), exp - 1.));
}

template <typename T1, typename T2>
[[nodiscard]] dual<T2> pow(T1 base, dual<T2> exp) {
  const T2 out_val = std::pow(base, exp.value());
  return dual<T2>(out_val, exp.epsilon() * out_val * std::log(base));
}

//==========================================================
// sqrt
template <typename T>
[[nodiscard]] auto sqrt(T arg) {
  return std::sqrt(arg);
}

template <typename T>
[[nodiscard]] dual<T> sqrt(dual<T> arg) {
  const T sqrt_value = std::sqrt(arg.value());
  return dual<T>(sqrt_value, 0.5 * arg.epsilon() / sqrt_value);
}

//==========================================================
// cbrt
template <typename T>
[[nodiscard]] auto cbrt(T arg) {
  return std::cbrt(arg);
}

template <typename T>
[[nodiscard]] dual<T> cbrt(dual<T> arg) {
  constexpr T one_third = 1. / 3.;
  return dual<T>(
      std::cbrt(arg.value()),
      one_third * arg.epsilon() / (std::pow(arg.value(), 2. * one_third)));
}

//==========================================================
// exp
template <typename T>
[[nodiscard]] auto exp(T arg) {
  return std::exp(arg);
}

template <typename T>
[[nodiscard]] dual<T> exp(dual<T> arg) {
  const T exp_value = std::exp(arg.value());
  return dual<T>(exp_value, arg.epsilon() * exp_value);
}

//==========================================================
// exp2
template <typename T>
[[nodiscard]] auto exp2(T arg) {
  return std::exp2(arg);
}

template <typename T>
[[nodiscard]] dual<T> exp2(dual<T> arg) {
  const T exp2_value = std::exp2(arg.value());
  return dual<T>(exp2_value, arg.epsilon() * exp2_value * std::log(2.));
}

//==========================================================
// expm1
template <typename T>
[[nodiscard]] auto expm1(T arg) {
  return std::expm1(arg);
}

template <typename T>
[[nodiscard]] dual<T> expm1(dual<T> arg) {
  dual<T> out = exp(arg);
  out.value() -= 1.;
  return out;
}

//==========================================================
// log
template <typename T>
[[nodiscard]] auto log(T arg) {
  return std::log(arg);
}

template <typename T>
[[nodiscard]] dual<T> log(dual<T> arg) {
  return dual<T>(std::log(arg.value()), arg.epsilon() / arg.value());
}

//==========================================================
// log2
template <typename T>
[[nodiscard]] auto log2(T arg) {
  return std::log2(arg);
}

template <typename T>
[[nodiscard]] dual<T> log2(dual<T> arg) {
  return dual<T>(std::log2(arg.value()),
                 arg.epsilon() / (arg.value() * std::log(2.)));
}

//==========================================================
// log10
template <typename T>
[[nodiscard]] auto log10(T arg) {
  return std::log10(arg);
}

template <typename T>
[[nodiscard]] dual<T> log10(dual<T> arg) {
  return dual<T>(std::log10(arg.value()),
                 arg.epsilon() / (arg.value() * std::log(10.)));
}

//==========================================================
// log1p
template <typename T>
[[nodiscard]] auto log1p(T arg) {
  return std::log1p(arg);
}

template <typename T>
[[nodiscard]] dual<T> log1p(dual<T> arg) {
  return dual<T>(std::log1p(arg.value()), arg.epsilon() / (1. + arg.value()));
}

//==========================================================
// sin
template <typename T>
[[nodiscard]] auto sin(T arg) {
  return std::sin(arg);
}

template <typename T>
[[nodiscard]] dual<T> sin(dual<T> arg) {
  return dual<T>(std::sin(arg.value()), arg.epsilon() * std::cos(arg.value()));
}

//==========================================================
// cos
template <typename T>
[[nodiscard]] auto cos(T arg) {
  return std::cos(arg);
}

template <typename T>
[[nodiscard]] dual<T> cos(dual<T> arg) {
  return dual<T>(std::cos(arg.value()), -arg.epsilon() * std::sin(arg.value()));
}

//==========================================================
// tan
template <typename T>
[[nodiscard]] auto tan(T arg) {
  return std::tan(arg);
}

template <typename T>
[[nodiscard]] dual<T> tan(dual<T> arg) {
  const T cos_val = std::cos(arg.value());
  return dual<T>(std::tan(arg.value()), arg.epsilon() / (cos_val * cos_val));
}

//==========================================================
// asin
template <typename T>
[[nodiscard]] auto asin(T arg) {
  return std::asin(arg);
}

template <typename T>
[[nodiscard]] dual<T> asin(dual<T> arg) {
  const T val_sqrd = arg.value() * arg.value();
  return dual<T>(std::asin(arg.value()),
                 arg.epsilon() / std::sqrt(1. - val_sqrd));
}

//==========================================================
// acos
template <typename T>
[[nodiscard]] auto acos(T arg) {
  return std::acos(arg);
}

template <typename T>
[[nodiscard]] dual<T> acos(dual<T> arg) {
  const T val_sqrd = arg.value() * arg.value();
  return dual<T>(std::acos(arg.value()),
                 -arg.epsilon() / std::sqrt(1. - val_sqrd));
}

//==========================================================
// atan
template <typename T>
[[nodiscard]] auto atan(T arg) {
  return std::atan(arg);
}

template <typename T>
[[nodiscard]] dual<T> atan(dual<T> arg) {
  const T val_sqrd = arg.value() * arg.value();
  return dual<T>(std::atan(arg.value()), arg.epsilon() / (1. + val_sqrd));
}

//==========================================================
// atan2
template <typename T1, typename T2>
[[nodiscard]] auto atan2(T2 y, T2 x) {
  return std::atan2(y, x);
}

template <typename T1, typename T2>
[[nodiscard]] dual<T1> atan2(dual<T1> y, T2 x) {
  const T1 deriv = -y.value() / (x * x + y.value() * y.value());
  return dual<T1>(std::atan2(y.value(), x), y.epsilon() * deriv);
}

template <typename T1, typename T2>
[[nodiscard]] dual<T2> atan2(T1 y, dual<T2> x) {
  const T2 deriv = x.value() / (x.value() * x.value() + y * y);
  return dual<T2>(std::atan2(y, x.value()), x.epsilon() * deriv);
}

//==========================================================
// sinh
template <typename T>
[[nodiscard]] auto sinh(T arg) {
  return std::sinh(arg);
}

template <typename T>
[[nodiscard]] dual<T> sinh(dual<T> arg) {
  return dual<T>(std::sinh(arg.value()),
                 arg.epsilon() * std::cosh(arg.value()));
}

//==========================================================
// cosh
template <typename T>
[[nodiscard]] auto cosh(T arg) {
  return std::cosh(arg);
}

template <typename T>
[[nodiscard]] dual<T> cosh(dual<T> arg) {
  return dual<T>(std::cosh(arg.value()),
                 arg.epsilon() * std::sinh(arg.value()));
}

//==========================================================
// tanh
template <typename T>
[[nodiscard]] auto tanh(T arg) {
  return std::tan(arg);
}

template <typename T>
dual<T> tanh(dual<T> arg) {
  const T cosh_val = std::cosh(arg.value());
  return dual<T>(std::tanh(arg.value()), arg.epsilon() / (cosh_val * cosh_val));
}

//==========================================================
// asinh
template <typename T>
[[nodiscard]] auto asinh(T arg) {
  return std::asinh(arg);
}

template <typename T>
[[nodiscard]] dual<T> asinh(dual<T> arg) {
  const T val_sqrd = arg.value() * arg.value();
  return dual<T>(std::asinh(arg.value()),
                 arg.epsilon() / std::sqrt(1. + val_sqrd));
}

//==========================================================
// acosh
template <typename T>
[[nodiscard]] auto acosh(T arg) {
  return std::acosh(arg);
}

template <typename T>
[[nodiscard]] dual<T> acosh(dual<T> arg) {
  const T val_sqrd = arg.value() * arg.value();
  return dual<T>(std::acosh(arg.value()),
                 arg.epsilon() / std::sqrt(val_sqrd - 1.));
}

//==========================================================
// atanh
template <typename T>
[[nodiscard]] auto atanh(T arg) {
  return std::atanh(arg);
}

template <typename T>
[[nodiscard]] dual<T> atanh(dual<T> arg) {
  const T val_sqrd = arg.value() * arg.value();
  return dual<T>(std::atanh(arg.value()), arg.epsilon() / (1. - val_sqrd));
}

}  // namespace htl

#endif
