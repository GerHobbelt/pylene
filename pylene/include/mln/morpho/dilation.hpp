#pragma once

#include <mln/accu/accumulators/h_infsup.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/core/canvas/parallel_local.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/extension/padding.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/ops.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/value/value_traits.hpp>

#include <mln/morpho/private/localmax.hpp>

#include <fmt/core.h>

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
      using has_incremental_sup             = std::false_type;
      static inline constexpr auto accu_sup = mln::accu::accumulators::sup<V>{};
    };

    template <class V>
    struct dilation_value_set<V, std::enable_if_t<std::is_integral_v<V> && (value_traits<V>::quant <= 16)>>
      : dilation_value_set_base<V>
    {
      using has_incremental_sup                         = std::true_type;
      static inline constexpr auto accu_sup             = mln::accu::accumulators::sup<V>{};
      static inline constexpr auto accu_incremental_sup = mln::accu::accumulators::h_sup<V>{};
    };
  } // namespace details

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void dilation(InputImage&& image, const mln::details::StructuringElement<SE>& se, BorderManager bm, OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;

    mln_entering("mln::morpho::dilation");

    // To enable when we can concept check that domain are comparable
    // assert(image.domain() == out.domain());

    if (!(bm.method() == mln::extension::BorderManagementMethod::Fill ||
          bm.method() == mln::extension::BorderManagementMethod::User))
      throw std::runtime_error("Invalid border management method (should be FILL or USER)");

    details::localmax(image, out, static_cast<const SE&>(se), bm, details::dilation_value_set<image_value_t<I>>());
  }


  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&&                                image,
                                                                 const mln::details::StructuringElement<SE>& se)
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
/*
    template <class InputImage, class SE>
    class TileLoader_DilationTranspose : TileLoaderBase
    {
    public:
      TileLoader_DilationTranspose(InputImage input, SE se, size_t tile_width, size_t tile_height)
        : _in{input}
        , _se{se}
      {
      }

      void load_tile(mln::box2d roi) final {}

      mln::ndbuffer_image get_tile() final {}

    private:
      InputImage _in;
      SE         _se;
    };
*/

    template <class InputImage>
    class TileLoader_Dilation : public TileLoaderBase
    {
    public:

      /// \param input_roi The extended roi required to compute the tile region
      TileLoader_Dilation(InputImage input, mln::box2d input_roi)
        : _in{input}
        , m_tile{input_roi}
      {
      }

      TileLoader_Dilation(const TileLoader_Dilation& other)
        : _in{other._in}
        , m_tile{other.m_tile.domain()}
      {
      }

      TileLoader_Dilation& operator=(const TileLoader_Dilation&) = delete;

      /// \param roi The tile region
      /// \param input_roi The extended roi required to compute the tile region
      void load_tile(mln::box2d roi, mln::box2d input_roi) const final
      {
        mln::box2d image_roi = _in.domain();

        mln::box2d copy_roi = image_roi;
        copy_roi.clip(input_roi);

        mln::box2d dest_roi = copy_roi;
        dest_roi.tl() -= roi.tl();

        mln::copy(_in.clip(copy_roi), m_tile.clip(dest_roi)); // TODO mln::details::copy_block

        if (copy_roi != input_roi)
        {
          int borders[2][2];

          borders[0][0] = copy_roi.tl().x() - input_roi.tl().x();
          borders[0][1] = input_roi.br().x() - copy_roi.br().x();
          borders[1][0] = copy_roi.tl().y() - input_roi.tl().y();
          borders[1][1] = input_roi.br().y() - copy_roi.br().y();

          image_value_t<InputImage> padding_value  = 0;
          // FIXME mln::value_traits<image_value_t<I>>::inf()
          auto padding_method = mln::PAD_ZERO;
          pad(m_tile, padding_method, borders, padding_value);
        }
      }

      mln::ndbuffer_image get_tile() const final
      {
        return m_tile;
      }

    private:
      InputImage _in;
      mutable mln::image2d<image_value_t<InputImage>> m_tile;
    };

    template <class V, class SE>
    class TileExecutor_Dilation : public TileExecutorBase
    {
    public:
      TileExecutor_Dilation(const SE& se)
        : _se{se}
      {
      }

      void execute(mln::ndbuffer_image in, mln::ndbuffer_image out) const final
      {
        // Perform operation on tile according to algorithm.
        auto       in_image2d  = *(in.cast_to<V, 2>());
        auto       out_image2d = *(out.cast_to<V, 2>());
        auto       vs          = mln::morpho::details::dilation_value_set<V>();
        mln::box2d inroi       = in_image2d.domain();
        mln::box2d roi         = out_image2d.domain();

        fmt::print("In: x={} y={} w={} h={}\n", inroi.x(), inroi.y(), inroi.width(), inroi.height());
        fmt::print("Out: x={} y={} w={} h={}\n", roi.x(), roi.y(), roi.width(), roi.height());
        mln::morpho::details::impl::localmax(in_image2d, out_image2d, vs, _se, roi);
      }

    private:
      const SE&      _se;
    };

    template <class OutputImage>
    class TileWriter_Dilation : public TileWriterBase
    {
    public:
      TileWriter_Dilation(OutputImage out)
        : _out{out}
      {
      }

      void write_tile(mln::box2d roi) const final { (void)roi; }

      mln::ndbuffer_image get_tile(mln::box2d roi) const final { return _out.clip(roi); }

    private:
      OutputImage _out;
    };

/*
    template <class OutputImage>
    class TileWriter_DilationTranspose : TileWriterBase
    {
    public:
      template <class OutputImage>
      TileWriter_DilationTranspose(OutputImage out)
        : _out{out}
      {
        _tile = imconcretize(_out);
      }

      void write_tile(mln::box2d roi) final { mln::details::transpose_block2d(/ TODO /); }

      mln::ndbuffer_image get_tile(mln::box2d roi) final { return out.clip(roi); }

    private:
      OutputImage _out;
    };
*/

    template <class InputImage, class SE, class OutputImage>
    struct DilationParallel : ParallelLocalCanvas2D
    {
    public:
      DilationParallel(InputImage& in, OutputImage& out, SE& se)
        : m_in{in}
        , m_se{se},
          m_tile_l{in, m_se.compute_input_region({TILE_WIDTH, TILE_HEIGHT})},
          m_tile_w{out},
          m_tile_e{se}
      {
      }

      mln::box2d GetDomain() const noexcept final { return m_in.domain(); }
      mln::box2d ComputeInputRegion(mln::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }

      const TileLoaderBase*   GetTileLoader() const noexcept   final { return &m_tile_l; };
      const TileWriterBase*   GetTileWriter() const noexcept   final { return &m_tile_w; };
      const TileExecutorBase* GetTileExecutor() const noexcept final { return &m_tile_e; };

    private:
      using tile_loader_t   = TileLoader_Dilation<InputImage>;
      using tile_executor_t = TileExecutor_Dilation<image_value_t<InputImage>, SE>;
      using tile_writer_t   = TileWriter_Dilation<OutputImage>;

      InputImage       m_in;
      SE               m_se;
      tile_loader_t    m_tile_l;
      tile_writer_t    m_tile_w;
      tile_executor_t  m_tile_e;
    };

    template <class InputImage, class SE, class OutputImage>
    void dilation(InputImage&& image, const SE& se, OutputImage&& out)
    {
      DilationParallel caller(image, out, se);
      parallel_execute_local2D(caller);
    }

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> dilation(InputImage&& image, const SE& se)
    {
      using I = std::remove_reference_t<InputImage>;

      image_concrete_t<I> out = imconcretize(image);
      dilation(image, se, out);
      return out;
    }
  } // namespace parallel

} // namespace mln::morpho
