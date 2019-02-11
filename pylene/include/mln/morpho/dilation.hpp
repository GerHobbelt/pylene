#pragma once


#include <mln/accu/accumulators/h_infsup.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/ops.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/value/value_traits.hpp>

#include <mln/morpho/private/localmax.hpp>
#include <mln/morpho/private/dilation.2d.hpp>

namespace mln::morpho
{

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&&                                image,
                                                                 const mln::details::StructuringElement<SE>& se);

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm,
           std::enable_if_t<!mln::is_a<BorderManager, mln::details::Image>::value>* = nullptr);

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value>
  dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, OutputImage&& out);

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm,
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

      template <std::size_t N>
      constexpr auto operator()(xsimd::batch<V, N> a, xsimd::batch<V, N> b) const
      {
        return xsimd::max(a, b);
      }
    };

    template <>
    struct dilation_sup_t<bool>
    {
      constexpr auto operator()(bool a, bool b) const { return a || b; }

      template <std::size_t N>
      constexpr auto operator()(xsimd::batch<uint8_t, N> a, xsimd::batch<uint8_t, N> b) const
      {
        return xsimd::max(a, b);
      }
    };



    template <class V>
    struct dilation_value_set_base
    {
      static inline constexpr dilation_sup_t<V> sup  = {};
      static inline constexpr auto  zero = mln::value_traits<V>::inf();
    };


    template <class V, class = void>
    struct dilation_value_set : dilation_value_set_base<V>
    {
      using has_incremental_sup             = std::false_type;
      static inline constexpr auto accu_sup = mln::accu::accumulators::sup<V>{};
    };

    template <class V>
    struct dilation_value_set<V, std::enable_if_t<std::is_integral_v<V> && (value_traits<V>::quant <= 16) && std::is_unsigned_v<V>>>
      : dilation_value_set_base<V>
    {
      using has_incremental_sup                         = std::true_type;
      static inline constexpr auto accu_sup             = mln::accu::accumulators::sup<V>{};
      static inline constexpr auto accu_incremental_sup = mln::accu::accumulators::h_sup<V>{};
    };




    // Generic implementation
    template <class InputImage, class SE, class ValueSet, class BorderManager, class OutputImage>
    void dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm, ValueSet& vs, OutputImage&& out)
    {
      mln_entering("mln::morpho::dilation");

      // To enable when we can concept check that domain are comparable
      // assert(image.domain() == out.domain());

      if (!(bm.method() == mln::extension::BorderManagementMethod::Fill ||
            bm.method() == mln::extension::BorderManagementMethod::User))
        throw std::runtime_error("Invalid border management method (should be FILL or USER)");

      mln::morpho::details::localmax(image, out, static_cast<const SE&>(se), bm, vs);
    }

    // Specialisation for 2D images
    template <class InputImage, class SE, class ValueSet, class BorderManager, class OutputImage>
    requires(::concepts::same_as<image_domain_t<std::remove_reference_t<InputImage>>, mln::box2d>&& //
             ::concepts::same_as<image_domain_t<std::remove_reference_t<OutputImage>>, mln::box2d>) //

    void dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm,
                  ValueSet& vs, OutputImage&& out)
    {
      using V = image_value_t<std::remove_reference_t<InputImage>>;
      mln_entering("mln::morpho::dilation (2d spe)");

      if (!(bm.method() == mln::extension::BorderManagementMethod::Fill ||
            bm.method() == mln::extension::BorderManagementMethod::User))
        throw std::runtime_error("Invalid border management method (should be FILL or USER)");



      int  tile_width  = 128;
      int  tile_height = 128;
      bool parallel    = false;

      if (bm.method() == mln::extension::BorderManagementMethod::User)
        mln::morpho::details::dilation2d(image, out, static_cast<const SE&>(se), vs, tile_width, tile_height, parallel);
      else if (bm.method() == mln::extension::BorderManagementMethod::Fill)
      {
        V padding_value = std::any_cast<V>(bm.get_value());
        mln::morpho::details::dilation2d(image, out, static_cast<const SE&>(se), vs, tile_width, tile_height, parallel,
                                         mln::PAD_CONSTANT, padding_value);
      }
    }

  } // namespace details

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm, OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;
    details::dilation_value_set<image_value_t<I>> vs;
    details::dilation(std::forward<InputImage>(image), se, bm, vs, std::forward<OutputImage>(out));
  }


  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> //
  dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    dilation(image, se, mln::extension::bm::fill(mln::value_traits<image_value_t<I>>::inf()), out);
    return out;
  }

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm,
           std::enable_if_t<!mln::is_a<BorderManager, mln::details::Image>::value>*)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    dilation(image, se, bm, out);
    return out;
  }

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value>
  dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;
    dilation(image, se, mln::extension::bm::fill(mln::value_traits<image_value_t<I>>::inf()), out);
  }

  namespace parallel
  {

    template <class InputImage, class SE, class OutputImage>
    void dilation(InputImage&& image, const SE& se, OutputImage&& out, int tile_width, int tile_height)
    {
      using I = std::remove_reference_t<InputImage>;
      using VS = morpho::details::dilation_value_set<image_value_t<I>>;

      static_assert(std::is_same_v<image_domain_t<std::remove_reference_t<InputImage>>, mln::box2d>);
      static_assert(std::is_same_v<image_domain_t<std::remove_reference_t<OutputImage>>, mln::box2d>);

      VS vs;
      bool parallel = true;
      morpho::details::dilation2d(std::forward<InputImage>(image), std::forward<OutputImage>(out), se, vs, tile_width,
                                  tile_height, parallel);
    }

    template <class InputImage, class SE, class OutputImage>
    void dilation(InputImage&& image, const SE& se, OutputImage&& out)
    {
      constexpr int kDefaultTileWidth = 128;
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
      constexpr int kDefaultTileWidth = 128;
      constexpr int kDefaultTileHeight = 128;
      return dilation(image, se, kDefaultTileWidth, kDefaultTileHeight);
    }
  } // namespace parallel

} // namespace mln::morpho
