#ifndef MLN_COLORS_LAB_HPP
# define MLN_COLORS_LAB_HPP

# include <mln/core/vec_base.hpp>
# include <mln/core/colors.hpp>
# include <mln/colors/xyz.hpp>
# include <cmath>
// FIXME: optimize this out (slow because of floats and saturations)

namespace mln
{

  struct lab_tag {};

  template <typename T>
  using lab = internal::vec_base<T, 3, lab_tag>;

  typedef lab<uint8> lab8;

  namespace internal
  {
    template <>
    struct vec_base_traits<lab_tag>
    {
      static const bool is_additive = true;
      static const bool is_additive_ext = true;
      static const bool is_multiplicative = false;
      static const bool is_multiplicative_ext = true;
      static const bool is_less_than_comparable = true;
      static const bool is_equality_comparable = true;
    };

  }

  template <typename T>
  lab<float> rgb2lab(const rgb<T>& v);

  // template <typename T>
  // rgb<T> lab2rgb(const lab<T>& v);


  /*********************/
  /*** Implementation **/
  /*********************/

  template <typename T>
  inline
  lab<float>
  rgb2lab(const rgb<T>& v)
  {
    auto xyz = rgb2xyz(v);

    constexpr float xn = (0.4125 + 0.3576 + 0.1804) * value_traits<T>::max();
    constexpr float yn = (0.2127 + 0.7152 + 0.0722) * value_traits<T>::max();
    constexpr float zn = (0.0193 + 0.1192 + 0.9502) * value_traits<T>::max();

    auto f = [] (float t) {
      return (t > (216.0f / 24389.0f)) ? std::cbrt(t) : (t * 841.0f / 208.0f + (4.0f / 29.0f));
    };

    float xr = f(xyz[0] / xn);
    float yr = f(xyz[1] / yn);
    float zr = f(xyz[2] / zn);

    float l = 116 * yr - 16;
    float a = 500 * (xr - yr);
    float b = 200 * (yr - zr);

    return {l, a, b};
  }

}

#endif // ! MLN_COLORS_LAB_HPP
