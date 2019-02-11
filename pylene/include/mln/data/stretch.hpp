#pragma once

#include <mln/accu/accumulators/minmax.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/image.hpp>


namespace mln::data
{
  /// \brief Transform image values to fit the complete dynamic of \p V
  ///
  /// If V is integral, values are streched to \f$[V_{min}, V_{max}]\f$, otherwise,
  /// if V is floating point, values are streched to \f$[0,1]\f. The method only support
  /// V and not vectorial type.
  template <class V, class I>
  image_ch_value_t<std::remove_reference_t<I>, V> stretch(I&& f);

  template <class I, class J>
  void stretch_to(I f, J out);

  /************************/
  /*** Implementation   ***/
  /************************/


  template <class I, class O>
  void stretch_to(I input, O out)
  {
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<O, mln::details::Image>());

    using V = image_value_t<O>;
    static_assert(std::is_convertible<image_value_t<I>, V>::value, "The image value type must be convertible to V");
    static_assert(std::is_arithmetic<V>::value, "V must be arithmetic");

    mln_entering("mln::data:stretch");

    double m, M;
    std::tie(m, M) = accumulate(input, accu::features::minmax<>());

    double x = (not std::is_floating_point<V>::value) ? (double)value_traits<V>::min() : 0.0;
    double y = (not std::is_floating_point<V>::value) ? (double)value_traits<V>::max() : 1.0;
    double r = (y - x) / (M - m);
    auto   scale_fn = [m, x, r](double v) -> V { return static_cast<V>(x + (v - m) * r); };
    mln::transform(input, out, scale_fn);
  }


  template <class V, class InputImage>
  image_ch_value_t<std::remove_reference_t<InputImage>, V> stretch(InputImage&& f)
  {
    using I = std::remove_reference_t<InputImage>;

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(std::is_convertible<image_value_t<I>, V>::value, "The image value type must be convertible to V");
    static_assert(std::is_arithmetic<V>::value, "V must be arithmetic");


    image_ch_value_t<I, V> out = imchvalue<V>(f);
    stretch_to(f, out);
    return out;
  }

} // namespace mln::data::
