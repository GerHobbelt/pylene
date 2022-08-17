#pragma once


#include <ano/accu/accumulators/h_infsup.hpp>
#include <ano/accu/accumulators/infsup.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/core/concepts/structuring_element.hpp>
#include <ano/core/extension/border_management.hpp>
#include <ano/core/ops.hpp>
#include <ano/core/trace.hpp>
#include <ano/core/value/value_traits.hpp>

#include <ano/morpho/private/localmax.hpp>

namespace ano::morpho
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


  namespace parallel
  {


    template <class InputImage, class SE, class OutputImage>
    void dilation(InputImage&& image, const SE& se, OutputImage&& out, int tile_width, int tile_height);

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> //
    dilation(InputImage&& image, const SE& se, int tile_width, int tile_height);

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&& image, const SE& se);

  } // namespace parallel


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {

    template <class V>
    struct dilation_sup_t
    {
      constexpr auto operator()(V a, V b) const { return sup(a, b); }
    };

    template <>
    struct dilation_sup_t<bool>
    {
      constexpr auto operator()(bool a, bool b) const { return a || b; }
    };


    template <class V>
    struct dilation_value_set_base
    {
      static inline constexpr dilation_sup_t<V> sup  = {};
      static inline constexpr auto              zero = ano::value_traits<V>::inf();
    };


    template <class V, class = void>
    struct dilation_value_set : dilation_value_set_base<V>
    {
      using has_incremental_sup             = std::false_type;
      static inline constexpr auto accu_sup = ano::accu::accumulators::sup<V>{};
    };

    template <class V>
    struct dilation_value_set<
        V, std::enable_if_t<std::is_integral_v<V> && (value_traits<V>::quant <= 16) && std::is_unsigned_v<V>>>
      : dilation_value_set_base<V>
    {
      using has_incremental_sup                         = std::true_type;
      static inline constexpr auto accu_sup             = ano::accu::accumulators::sup<V>{};
      static inline constexpr auto accu_incremental_sup = ano::accu::accumulators::h_sup<V>{};
    };


    // Generic implementation
    template <class InputImage, class SE, class ValueSet, class BorderManager, class OutputImage>
    void dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm, ValueSet& vs,
                  OutputImage&& out)
    {
      ano_entering("ano::morpho::dilation");

      // To enable when we can concept check that domain are comparable
      // assert(image.domain() == out.domain());

      if (!(bm.method() == ano::extension::BorderManagementMethod::Fill ||
            bm.method() == ano::extension::BorderManagementMethod::User))
        throw std::runtime_error("Invalid border management method (should be FILL or USER)");

      ano::morpho::details::localmax(image, out, static_cast<const SE&>(se), bm, vs);
    }

  } // namespace details

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm, OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;
    details::dilation_value_set<image_value_t<I>> vs;
    details::dilation(std::forward<InputImage>(image), se, bm, vs, std::forward<OutputImage>(out));
  }


  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> //
  dilation(InputImage&& image, const ano::details::StructuringElement<SE>& se)
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

  namespace parallel
  {

    template <class InputImage, class SE, class OutputImage>
    void dilation(InputImage&& image, const SE& se, OutputImage&& out)
    {
      constexpr int kDefaultTileWidth  = 128;
      constexpr int kDefaultTileHeight = 128;
      return dilation(image, se, out, kDefaultTileWidth, kDefaultTileHeight);
    }


    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> //
    dilation(InputImage&& image, const SE& se, int tile_width, int tile_height)
    {
      using I = std::remove_reference_t<InputImage>;

      image_concrete_t<I> out = imconcretize(image);
      dilation(image, se, out, tile_width, tile_height);
      return out;
    }

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&& image, const SE& se)
    {
      constexpr int kDefaultTileWidth  = 128;
      constexpr int kDefaultTileHeight = 128;
      return dilation(image, se, kDefaultTileWidth, kDefaultTileHeight);
    }
  } // namespace parallel

} // namespace ano::morpho
