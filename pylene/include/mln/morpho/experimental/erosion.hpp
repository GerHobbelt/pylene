#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/ops.hpp>
#include <mln/core/value/value_traits.hpp>
#include <mln/core/trace.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/accu/accumulators/h_infsup.hpp>

#include <mln/morpho/experimental/private/localmax.hpp>

namespace mln::morpho::experimental
{

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> erosion(InputImage&& image, const mln::experimental::StructuringElement<SE>& se);

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  erosion(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, BorderManager bm,
           std::enable_if_t<!mln::is_a<BorderManager, mln::experimental::Image>::value>* = nullptr);

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<mln::is_a<std::remove_reference_t<OutputImage>, mln::experimental::Image>::value>
  erosion(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, OutputImage&& out);

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void erosion(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, BorderManager bm,
                OutputImage&& out);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    struct inf_t
    {
      template <class T>
      T operator() (T x, T y) const { return inf(x,y); }

      template <class T>
      std::experimental::simd<T> operator() (std::experimental::simd<T> x, std::experimental::simd<T> y) const { return std::experimental::min(x,y); }
    };


    template <class V>
    struct erosion_value_set_base
    {
      static inline constexpr inf_t sup = {};
      static inline constexpr auto zero = mln::value_traits<V>::sup();
    };


    template <class V, class = void>
    struct erosion_value_set : erosion_value_set_base<V>
    {
      using has_incremental_sup = std::false_type;
      static inline constexpr auto accu_sup = mln::accu::accumulators::inf<V>{};
    };

    template <class V>
    struct erosion_value_set<V, std::enable_if_t<std::is_integral_v<V> && (value_traits<V>::quant <= 16)>>
      : erosion_value_set_base<V>
    {
      using has_incremental_sup = std::true_type;
      static inline constexpr auto accu_sup = mln::accu::accumulators::inf<V>{};
      static inline constexpr auto accu_incremental_sup = mln::accu::accumulators::h_inf<V>{};
    };
  }

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void erosion(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, BorderManager bm,
                OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;

    mln_entering("mln::morpho::experimental::erosion");

    // To enable when we can concept check that domain are comparable
    // assert(image.domain() == out.domain());

    if (! (bm.method() == mln::extension::BorderManagementMethod::Fill || bm.method() == mln::extension::BorderManagementMethod::User))
      throw std::runtime_error("Invalid borde management method (should be FILL or USER)");

    details::localmax(image, out, static_cast<const SE&>(se), bm, details::erosion_value_set<image_value_t<I>>());
  }


  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> erosion(InputImage&& image, const mln::experimental::StructuringElement<SE>& se)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    erosion(image, se, mln::extension::bm::fill(mln::value_traits<image_value_t<I>>::sup()), out);
    return out;
  }

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>> erosion(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, BorderManager bm,
                                                                 std::enable_if_t<!mln::is_a<BorderManager, mln::experimental::Image>::value>*)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    erosion(image, se, bm, out);
    return out;
  }

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<mln::is_a<std::remove_reference_t<OutputImage>, mln::experimental::Image>::value>
  erosion(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;
    erosion(image, se, mln::extension::bm::fill(mln::value_traits<image_value_t<I>>::sup()),  out);
  }


} // namespace mln::morpho::experimental
