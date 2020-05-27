#pragma once

#include <mln/core/canvas/parallel_local.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/extension/padding.hpp>
#include <mln/core/ops.hpp>
#include <mln/core/value/value_traits.hpp>
#include <mln/core/trace.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/accu/accumulators/h_infsup.hpp>

#include <mln/morpho/private/localmax.hpp>

namespace mln::morpho
{

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se);

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


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    template <class V>
    struct dilation_value_set_base
    {
      static inline constexpr auto _sup = [](V a, V b) { return sup(a, b); };
      static inline constexpr auto sup  = _sup;
      static inline constexpr auto zero = mln::value_traits<V>::inf();
    };


    template <class V, class = void>
    struct dilation_value_set : dilation_value_set_base<V>
    {
      using has_incremental_sup = std::false_type;
      static inline constexpr auto accu_sup = mln::accu::accumulators::sup<V>{};
    };

    template <class V>
    struct dilation_value_set<V, std::enable_if_t<std::is_integral_v<V> && (value_traits<V>::quant <= 16)>>
      : dilation_value_set_base<V>
    {
      using has_incremental_sup = std::true_type;
      static inline constexpr auto accu_sup = mln::accu::accumulators::sup<V>{};
      static inline constexpr auto accu_incremental_sup = mln::accu::accumulators::h_sup<V>{};
    };
  }

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm,
                OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;

    mln_entering("mln::morpho::dilation");

    // To enable when we can concept check that domain are comparable
    // assert(image.domain() == out.domain());

    if (! (bm.method() == mln::extension::BorderManagementMethod::Fill || bm.method() == mln::extension::BorderManagementMethod::User))
      throw std::runtime_error("Invalid border management method (should be FILL or USER)");

    details::localmax(image, out, static_cast<const SE&>(se), bm, details::dilation_value_set<image_value_t<I>>());
  }


  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(image);
    dilation(image, se, mln::extension::bm::fill(mln::value_traits<image_value_t<I>>::inf()), out);
    return out;
  }

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm,
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
    dilation(image, se, mln::extension::bm::fill(mln::value_traits<image_value_t<I>>::inf()),  out);
  }

  namespace parallel
  {
    template <class InputImage, class SE, typename DataType>
    class TileLoader_Dilation : TileLoaderBase
    {
    public:
      TileLoader_Dilation(InputImage input, SE se, DataType type, size_t tile_width, size_t tile_height)
        : _in{input}
        , _se{se}
      {
        mln::box2d roi(tile_width, tile_height);

        mln::box2d input_roi = _se.compute_input_region(roi);
        _tile                = mln::image2d<image_value_t<InputImage>>(input_roi);
      }

      void load_tile(mln::box2d roi) final
      {
        mln::box2d input_roi = _se.compute_input_region(roi);
        mln::box2d image_roi = _in.domain();

        mln::box2d copy_roi =  image_roi;
        copy_roi.clip(input_roi);

        mln::box2d dest_roi = copy_roi;
        dest_roi.tl() -= roi.tl();

        mln::copy(_in.clip(copy_roi), _tile.clip(dest_roi)); // TODO mln::details::copy_block

        if (copy_roi != input_roi)
        {
          int borders[2][2];

          borders[0][0] = copy_roi.tl().x() - input_roi.tl().x();
          borders[0][1] = input_roi.br().x() - copy_roi.br().x();
          borders[1][0] = copy_roi.tl().y() - input_roi.tl().y();
          borders[1][1] = input_roi.br().y() - copy_roi.br().y();

          DataType padding_value = 0;
          auto padding_method = mln::PAD_ZERO;
          pad(_tile, padding_method, borders, padding_value);
        }
      }

      mln::ndbuffer_image get_tile() final
      {
        image_concrete_t<InputImage> out = imconcretize(_tile);
        return out;
      }

    private:
      InputImage _in;
      SE _se;
    };

    template <class SE, class BorderManager, class Image>
    class TileExecutor_Dilation : TileExecutorBase
    {
    public:
      template <class SE, class BorderManager, class Image>
      TileExecutor_Dilation(const mln::experimental::StructuringElement<SE>& se, BorderManager bm, Image& in)
        : _se{se}
        , _bm{bm}
        , _in{in}
      {}

      void execute(mln::ndbuffer_image in, mln::ndbufferimage out) final
      {
        // Perform operation on tile according to algorithm.
        auto in_image2d = in.cast_to<image_value_t<Image>, 2>();
        auto out_image2d = out.cast_to<image_value_t<Image>, 2>();
        auto vs = mln::morpho::details::dilation_value_set<image_value_t<Image>>();
        mln::box2d roi = in_image2d.domain();
        auto [imas, ses] = bm.manage(in_image2d, _se);
        std::visit([&](auto&& ima, auto&& se) { mln::morpho::details::impl::localmax(ima, output, vs, se, roi); }, imas, ses);
      }

    private:
      SE            _se;
      BorderManager _bm;
      Image         _in;
    };

    template <class OutputImage>
    class TileWriter_Dilation : TileWriterBase
    {
    public:
      template <class OutputImage>
      TileWriter_Dilation(OutputImage out)
        : _out{out}
      {}

      void write_tile(mln::box2d roi) final
      {
      }

      mln::ndbuffer_image get_tile(mln::box2d roi) final
      {
        return out.clip(roi);
      }

    private:
      OutputImage _out;
    };

    template <class OutputImage>
    class TileWriter_DilationTranspose : TileWriterBase
    {
    public:
      template <class OutputImage>
      TileWriter_DilationTranspose(OutputImage out)
        : _out{out}
      {
        _tile = imconcretize(_out)
      }

      void write_tile(mln::box2d roi) final
      {
        mln::details::transpose_block2d(/* TODO */);
      }

      mln::ndbuffer_image get_tile(mln::box2d roi) final
      {
        return out.clip(roi);
      }

    private:
      OutputImage _out;
    };

  } // namespace parallel

} // namespace mln::morpho::
