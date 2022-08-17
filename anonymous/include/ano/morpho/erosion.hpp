#pragma once

#include <ano/accu/accumulators/h_infsup.hpp>
#include <ano/accu/accumulators/infsup.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/core/concepts/structuring_element.hpp>
#include <ano/core/extension/border_management.hpp>
#include <ano/core/ops.hpp>
#include <ano/core/trace.hpp>
#include <ano/core/value/value_traits.hpp>

#include <ano/morpho/dilation.hpp>

namespace ano::morpho
{

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> erosion(InputImage&&                                image,
                                                                const ano::details::StructuringElement<SE>& se);

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  erosion(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm,
          std::enable_if_t<!ano::is_a<BorderManager, ano::details::Image>::value>* = nullptr);

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<ano::is_a<std::remove_reference_t<OutputImage>, ano::details::Image>::value>
  erosion(InputImage&& image, const ano::details::StructuringElement<SE>& se, OutputImage&& out);

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void erosion(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm, OutputImage&& out);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {

    template <class V>
    struct erosion_sup_t
    {
      constexpr auto operator()(V a, V b) const { return inf(a, b); }

      template <std::size_t N>
      constexpr auto operator()(xsimd::batch<V, N> a, xsimd::batch<V, N> b) const
      {
        return xsimd::min(a, b);
      }
    };

    template <>
    struct erosion_sup_t<bool>
    {
      constexpr auto operator()(bool a, bool b) const { return a && b; }

      template <std::size_t N>
      constexpr auto operator()(xsimd::batch<uint8_t, N> a, xsimd::batch<uint8_t, N> b) const
      {
        return xsimd::min(a, b);
      }
    };

    template <class V>
    struct erosion_value_set_base
    {
      static inline constexpr erosion_sup_t<V> sup  = {};
      static inline constexpr auto             zero = ano::value_traits<V>::sup();
    };


    template <class V, class = void>
    struct erosion_value_set : erosion_value_set_base<V>
    {
      using has_incremental_sup             = std::false_type;
      static inline constexpr auto accu_sup = ano::accu::accumulators::inf<V>{};
    };

    template <class V>
    struct erosion_value_set<
        V, std::enable_if_t<std::is_integral_v<V> && (value_traits<V>::quant <= 16) && std::is_unsigned_v<V>>>
      : erosion_value_set_base<V>
    {
      using has_incremental_sup                         = std::true_type;
      static inline constexpr auto accu_sup             = ano::accu::accumulators::inf<V>{};
      static inline constexpr auto accu_incremental_sup = ano::accu::accumulators::h_inf<V>{};
    };


  } // namespace details

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void erosion(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm, OutputImage&& out)
  {
    ano_entering("ano::morpho::erosion");

    using I = std::remove_reference_t<InputImage>;
    details::erosion_value_set<image_value_t<I>> vs;
    ano::morpho::details::dilation(std::forward<InputImage>(image), se, bm, vs, std::forward<OutputImage>(out));
  }


  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> erosion(InputImage&&                                image,
                                                                const ano::details::StructuringElement<SE>& se)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    erosion(image, se, ano::extension::bm::fill(ano::value_traits<image_value_t<I>>::sup()), out);
    return out;
  }

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  erosion(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm,
          std::enable_if_t<!ano::is_a<BorderManager, ano::details::Image>::value>*)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    erosion(image, se, bm, out);
    return out;
  }

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<ano::is_a<std::remove_reference_t<OutputImage>, ano::details::Image>::value>
  erosion(InputImage&& image, const ano::details::StructuringElement<SE>& se, OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;
    erosion(image, se, ano::extension::bm::fill(ano::value_traits<image_value_t<I>>::sup()), out);
  }


  namespace parallel
  {

    template <class InputImage, class SE, class OutputImage>
    void erosion(InputImage&& image, const SE& se, OutputImage&& out, int tile_width, int tile_height)
    {
      using I  = std::remove_reference_t<InputImage>;
      using VS = morpho::details::erosion_value_set<image_value_t<I>>;

      static_assert(std::is_same_v<image_domain_t<std::remove_reference_t<InputImage>>, ano::box2d>);
      static_assert(std::is_same_v<image_domain_t<std::remove_reference_t<OutputImage>>, ano::box2d>);

      VS   vs;
      bool parallel = true;
      morpho::details::dilation2d(std::forward<InputImage>(image), std::forward<OutputImage>(out), se, vs, tile_width,
                                  tile_height, parallel);
    }

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> //
    erosion(InputImage&& image, const SE& se, int tile_width, int tile_height)
    {
      using I = std::remove_reference_t<InputImage>;

      image_concrete_t<I> out = imconcretize(image);
      erosion(image, se, out, tile_width, tile_height);
      return out;
    }

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> erosion(InputImage&& image, const SE& se)
    {
      constexpr int kDefaultTileWidth  = 128;
      constexpr int kDefaultTileHeight = 128;
      return erosion(image, se, kDefaultTileWidth, kDefaultTileHeight);
    }
  } // namespace parallel

} // namespace ano::morpho
