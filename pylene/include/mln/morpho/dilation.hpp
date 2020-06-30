#pragma once


#include <mln/accu/accumulators/h_infsup.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/core/algorithm/paste.hpp>
#include <mln/core/algorithm/fill.hpp>
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


    template <class InputImage>
    class TileLoader_Dilation : public TileLoaderBase
    {
    public:

      /// \param width the extended width of the tile
      /// \param width the extended height of the tile
      TileLoader_Dilation(InputImage input, int width, int height)
        : _in{input}
        , m_tile{width, height}
      {
      }

      TileLoader_Dilation(const TileLoader_Dilation& other)
        : _in{other._in}
        , m_tile{other.m_tile.width(), other.m_tile.height()}
      {
      }

      TileLoader_Dilation& operator=(const TileLoader_Dilation&) = delete;

      /// \param roi The tile region
      /// \param input_roi The extended roi required to compute the tile region
      void load_tile(mln::box2d, mln::box2d input_roi) const final
      {
        assert(m_tile.width() >= input_roi.width() && "Tile width mismatches");
        assert(m_tile.height() >= input_roi.height() && "Tile height mismatches");

        m_tile.set_domain_topleft(input_roi.tl());
        assert(m_tile.domain().includes(input_roi));
        assert(m_tile.domain().tl() == input_roi.tl());

        // Clip roi so that it does not go outside image boundaries
        mln::box2d clipped_roi = _in.domain();
        clipped_roi.clip(input_roi);


        // The output tile is in the padding region (this work here but buggy if periodic is used).
        image_value_t<InputImage> padding_value  = 0;
        if (clipped_roi.empty())
        {
          mln::fill(m_tile, padding_value);
          return;
        }

        {
          auto dst = m_tile.clip(clipped_roi);
          mln::details::copy_block(_in, clipped_roi, dst.buffer(), dst.stride());
        }
        if (clipped_roi == input_roi)
          return;

        // Padding is required
        {
          int borders[2][2];

          borders[0][0] = clipped_roi.tl().x() - input_roi.tl().x();
          borders[0][1] = input_roi.br().x() - clipped_roi.br().x();
          borders[1][0] = clipped_roi.tl().y() - input_roi.tl().y();
          borders[1][1] = input_roi.br().y() - clipped_roi.br().y();

          // FIXME mln::value_traits<image_value_t<I>>::inf()
          auto padding_method = mln::PAD_ZERO;
          auto dst = m_tile.clip(input_roi);
          pad(dst, padding_method, borders, padding_value);
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
        assert(in.domain().includes(out.domain()));

        auto in_image2d  = *(in.cast_to<V, 2>());
        auto out_image2d = *(out.cast_to<V, 2>());
        auto vs          = mln::morpho::details::dilation_value_set<V>();
        auto roi         = out_image2d.domain();


        if (std::is_same_v<SE, mln::se::periodic_line2d>)
        {
          mln::morpho::details::impl::localmax(in_image2d, out_image2d, vs, _se, roi);
        }
        else
        {
          // Note: the roi is not considered by the generic version of accumulate, we need to fix that later
          auto tmp = in_image2d.clip(roi);
          mln::morpho::details::impl::localmax(tmp, out_image2d, vs, _se, roi);
        }
      }

    private:
      const SE&         _se;
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
      static_assert(std::is_same_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    private:
      DilationParallel(InputImage& in, OutputImage& out, SE& se, mln::box2d roi, mln::box2d tile_dims)
        : m_in{in}
        , m_se{se}
        , m_output_roi{roi}
        , m_tile_l{in, tile_dims.width(), tile_dims.height()}
        , m_tile_w{out}
        , m_tile_e{se}
      {}

    public:
      DilationParallel(InputImage& in, OutputImage& out, SE& se, mln::box2d roi)
        : DilationParallel(in, out, se, roi, se.compute_input_region({TILE_WIDTH, TILE_HEIGHT}))
      {
      }

      std::unique_ptr<ParallelLocalCanvas2D> clone()  const final { return std::make_unique<DilationParallel>(*this); }
      mln::box2d GetOutputRegion() const noexcept final { return m_output_roi; }
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
      mln::box2d       m_output_roi;
      tile_loader_t    m_tile_l;
      tile_writer_t    m_tile_w;
      tile_executor_t  m_tile_e;
    };

    template <class InputImage, class SE, class OutputImage>
    void dilation(InputImage&& image, const SE& se, OutputImage&& out)
    {
      auto output_roi = out.domain();
      if (se.is_decomposable())
      {
        auto input_roi    = se.compute_input_region(output_roi);
        image_concrete_t<std::remove_reference_t<InputImage>> tmp_1;
        image_concrete_t<std::remove_reference_t<InputImage>> tmp_2;

        auto ses = se.decompose();
        assert(ses.size() > 1);

        tmp_1.resize(input_roi);

        if (ses.size() > 2)
          tmp_2.resize(input_roi);

        // First dilation
        {
          auto&&           se         = ses[0];
          auto             output_roi = se.compute_output_region(input_roi);
          DilationParallel caller(image, tmp_1, se, output_roi);
          parallel_execute_local2D(caller);
          input_roi = output_roi;
        }
        // Intermediate dilation
        for (std::size_t i = 1; i < ses.size() - 1; ++i)
        {
          auto&&           se         = ses[i];
          auto             output_roi = se.compute_output_region(input_roi);
          DilationParallel caller(tmp_1, tmp_2, se, output_roi);
          parallel_execute_local2D(caller);
          input_roi = output_roi;
          std::swap(tmp_1, tmp_2);
        }
        // Last dilation
        {
          auto&&           se         = ses.back();
          auto             output_roi = se.compute_output_region(input_roi);
          DilationParallel caller(tmp_1, out, se, output_roi);
          parallel_execute_local2D(caller);
        }
      }
      else
      {
        DilationParallel caller(image, out, se, output_roi);
        parallel_execute_local2D(caller);
      }
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
