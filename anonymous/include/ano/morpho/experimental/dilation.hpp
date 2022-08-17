#pragma once

#include <ano/accu/accumulators/h_infsup.hpp>
#include <ano/accu/accumulators/infsup.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/core/concepts/structuring_element.hpp>
#include <ano/core/extension/border_management.hpp>
#include <ano/core/ops.hpp>
#include <ano/core/trace.hpp>
#include <ano/core/value/value_traits.hpp>

#include <ano/morpho/experimental/private/localmax.hpp>

#include <experimental/simd>

namespace ano::morpho::experimental
{

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&&                                image,
                                                                 const ano::details::StructuringElement<SE>& se);

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm,
           std::enable_if_t<!ano::is_a<BorderManager, ano::details::Image>::value>* = nullptr);

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<ano::is_a<std::remove_reference_t<OutputImage>, ano::details::Image>::value>
  dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se, OutputImage&& out);

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm,
                OutputImage&& out);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    struct sup_t
    {
      template <class T>
      T operator()(T x, T y) const
      {
        return ano::sup(x, y);
      }

      template <class T>
      std::experimental::simd<T> operator()(std::experimental::simd<T> x, std::experimental::simd<T> y) const
      {
        return std::experimental::max(x, y);
      }
    };

    template <class V>
    struct dilation_value_set_base
    {
      static inline constexpr sup_t sup  = {};
      static inline constexpr auto  zero = ano::value_traits<V>::inf();
    };


    template <class V, class = void>
    struct dilation_value_set : dilation_value_set_base<V>
    {
      using has_incremental_sup             = std::false_type;
      static inline constexpr auto accu_sup = ano::accu::accumulators::sup<V>{};
    };


    template <class V>
    struct dilation_value_set<V, std::enable_if_t<std::is_integral_v<V> && (value_traits<V>::quant <= 16)>>
      : dilation_value_set_base<V>
    {
      using has_incremental_sup                         = std::true_type;
      static inline constexpr auto accu_sup             = ano::accu::accumulators::sup<V>{};
      static inline constexpr auto accu_incremental_sup = ano::accu::accumulators::h_sup<V>{};
    };
  } // namespace details

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm, OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;

    ano_entering("ano::morpho::experimental::dilation");

    // To enable when we can concept check that domain are comparable
    // assert(image.domain() == out.domain());

    if (!(bm.method() == ano::extension::BorderManagementMethod::Fill ||
          bm.method() == ano::extension::BorderManagementMethod::User))
      throw std::runtime_error("Invalid border management method (should be FILL or USER)");

    details::localmax(image, out, static_cast<const SE&>(se), bm, details::dilation_value_set<image_value_t<I>>());
  }


  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&&                                image,
                                                                 const ano::details::StructuringElement<SE>& se)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    dilation(image, se, ano::extension::bm::fill(ano::value_traits<image_value_t<I>>::inf()), out);
    return out;
  }

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm,
           std::enable_if_t<!ano::is_a<BorderManager, ano::details::Image>::value>*)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    dilation(image, se, bm, out);
    return out;
  }

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<ano::is_a<std::remove_reference_t<OutputImage>, ano::details::Image>::value>
  dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se, OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;
    dilation(image, se, ano::extension::bm::fill(ano::value_traits<image_value_t<I>>::inf()), out);
  }


} // namespace ano::morpho::experimental
