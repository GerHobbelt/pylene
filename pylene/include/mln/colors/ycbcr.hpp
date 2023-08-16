#pragma once

#include <mln/core/colors.hpp>
#include <mln/core/vec_base.hpp>

#include <concepts>

namespace mln
{

  struct ycbcr_tag
  {
  };

  template <typename T>
  using ycbcr = internal::vec_base<T, 3, ycbcr_tag>;

  using ycbcr8 = ycbcr<std::uint8_t>;

  namespace internal
  {
    template <>
    struct vec_base_traits<ycbcr_tag>
    {
      static const bool is_additive             = true;
      static const bool is_additive_ext         = true;
      static const bool is_multiplicative       = false;
      static const bool is_multiplicative_ext   = true;
      static const bool is_less_than_comparable = true;
      static const bool is_equality_comparable  = true;
    };
  } // namespace internal

  /// \brief Perform the conversion from RGB to YCbCr colorspace
  ///
  /// If input value type is:
  /// * an integral: values are supposed to be in the range [min,max]
  /// * a float: values are supposed to be in the range [0,1]
  /// If output value type is:
  /// * an integral: values are stretched in the range [min,max]
  /// * a float: values are stretched in the range [0,1]
  template <typename T>
  ycbcr<T> rgb2ycbcr(const rgb<T>& v);

  template <typename T>
  rgb<T> ycbcr2rgb(const ycbcr<T>& v);

  /*********************/
  /*** Implementation **/
  /*********************/

  template <typename T>
  inline ycbcr<T> rgb2ycbcr(const rgb<T>& v)
  {
    static constexpr double bias =
        std::integral<T> ? (((double)value_traits<T>::min() + (double)value_traits<T>::max()) / 2) : 0;

    T y  = 0.299 * v[0] + 0.587 * v[1] + 0.114 * v[2];
    T cb = -0.1687 * v[0] - 0.3313 * v[1] + 0.5 * v[2] + bias;
    T cr = 0.5 * v[0] - 0.4187 * v[1] - 0.0813 * v[2] + bias;

    return {y, cb, cr};
  }

  template <typename T>
  inline rgb<T> ycbcr2rgb(const ycbcr<T>& v)
  {
    static constexpr double bias =
        std::integral<T> ? (((double)value_traits<T>::min() + (double)value_traits<T>::max()) / 2) : 0;

    T r = v[0] + 1.402 * (v[2] - bias);
    T g = v[0] - 0.34454 * (v[1] - bias) - 0.71414 * (v[2] - bias);
    T b = v[0] + 1.772 * (v[1] - bias);

    return {r, g, b};
  }
} // namespace mln
