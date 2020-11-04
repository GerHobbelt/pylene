#pragma once

#include <mln/core/canvas/parallel_local.hpp>
#include <mln/core/algorithm/paste.hpp>

#include <mln/core/extension/padding.hpp>
#include <mln/morpho/private/block_algorithm.hpp>
#include <mln/morpho/private/block_running_max.hpp>
#include <mln/morpho/private/localmax.hpp>

/// \file Provides specialization for 2d dilation

namespace mln::morpho::details
{

  // Generic 2D dilation for any se (will not perform a decomposition test)
  // Will fallback to the local accumulation canvas
  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel);

  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel, e_padding_mode padding_mode, image_value_t<I> padding_value);




  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  class SimpleFilter2D
  {
  public:
    virtual ~SimpleFilter2D()                                                                         = default;
    virtual void                            Execute(mln::ndbuffer_image& in, mln::ndbuffer_image out) = 0;
    virtual mln::box2d                      ComputeInputRegion(mln::box2d roi) const noexcept         = 0;
    virtual mln::box2d                      ComputeOutputRegion(mln::box2d roi) const noexcept        = 0;
    virtual std::unique_ptr<SimpleFilter2D> Clone() const                                             = 0;
  };


  template <class V, class SE, class ValueSet>
  class SimpleDilation2D final : public SimpleFilter2D
  {
  public:
    SimpleDilation2D(SE se, ValueSet* vs, int /*tile_width*/, int /*tile_height*/)
      : m_se{std::move(se)}
      , m_vs{vs}
    {
    }


    void Execute(mln::ndbuffer_image& in_, mln::ndbuffer_image out_) final
    {
      auto& in  = in_.__cast<V, 2>();
      auto& out = out_.__cast<V, 2>();
      mln::box2d roi = out.domain();
      auto tmp = in.clip(roi);
      mln::morpho::details::impl::localmax(tmp, out, *m_vs, m_se, roi);
    }

    std::unique_ptr<SimpleFilter2D> Clone() const final { return std::make_unique<SimpleDilation2D>(*this); }

    mln::box2d ComputeInputRegion(mln::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }
    mln::box2d ComputeOutputRegion(mln::box2d roi) const noexcept final { return m_se.compute_output_region(roi); }

  private:
    SE                       m_se;
    ValueSet*                m_vs;
  };


  template <class V, class ValueSet>
  class SimpleDilation2D<V, mln::se::periodic_line2d, ValueSet> : public SimpleFilter2D
  {
  public:
    SimpleDilation2D(mln::se::periodic_line2d se, ValueSet* vs, int input_tile_width, int input_tile_height)
      : m_se{se}
      , m_vs{vs}
    {
      if (se.is_vertical())
        m_orient = VERTICAL;
      else if (se.is_horizontal())
        m_orient = HORIZONTAL;
      else
        m_orient = OTHER;


      if (m_orient == HORIZONTAL)
      {
        // Allocate space for the transposition
        m_tmp_in = mln::image2d<V>(input_tile_height, input_tile_width);
      }
    }


    void Execute(mln::ndbuffer_image& in_, mln::ndbuffer_image out_) final
    {
      auto& in  = in_.__cast<V, 2>();
      auto& out = out_.__cast<V, 2>();
      auto roi = out.domain();

      if (m_orient == OTHER)
      {
        mln::trace::warn("[Performance] Running the specialization with perodic lines (1).");
        dilation_by_periodic_line(in, out, m_se, m_vs->sup, roi);
      }
      else if (m_orient == VERTICAL)
      {
        mln::trace::warn("[Performance] Running the specialization with vertical lines.");
        block_running_max(&in.at(roi.tl()), roi.width(), roi.height(), in.byte_stride(), m_se.repetition(), m_vs->sup,
                          m_vs->zero);
        mln::paste(in, roi, out);

      }
      else if (m_orient == HORIZONTAL)
      {
        mln::trace::warn("[Performance] Running the specialization with horizontal lines.");
        auto input_roi  = this->ComputeInputRegion(roi);

        { // 1. Transpose
          block_transpose(&in.at(input_roi.tl()), m_tmp_in.buffer(), input_roi.height(), input_roi.width(),
                          in.byte_stride(), m_tmp_in.byte_stride());
        }

        V* block_ptr = mln::ptr_offset(m_tmp_in.buffer(), m_se.repetition() * m_tmp_in.byte_stride());
        { // 2. Run vertically inplace
          block_running_max(block_ptr, roi.height(), roi.width(), m_tmp_in.byte_stride(), m_se.repetition(), m_vs->sup,
                            m_vs->zero);
        }
        { // 3. Transpose and write
          block_transpose(block_ptr, out.buffer(), roi.width(), roi.height(), m_tmp_in.byte_stride(), out.byte_stride());
        }
      }
    }

    mln::box2d ComputeInputRegion(mln::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }
    mln::box2d ComputeOutputRegion(mln::box2d roi) const noexcept final { return m_se.compute_output_region(roi); }


    std::unique_ptr<SimpleFilter2D> Clone() const final
    {
      return std::make_unique<SimpleDilation2D>(m_se, m_vs, m_tmp_in.height(), m_tmp_in.width());
    }


  private:
    enum orientation_t
    {
      HORIZONTAL,
      VERTICAL,
      OTHER
    };

    mln::se::periodic_line2d  m_se;
    ValueSet*                 m_vs;
    orientation_t             m_orient;
    mln::image2d<V>           m_tmp_in;  // Temporary image used for transposition
  };



  class FilterChain : public ParallelLocalCanvas2DBase
  {
    std::vector<std::unique_ptr<SimpleFilter2D>> m_filters; // List of filters to apply
    mln::ndbuffer_image                  m_tile_in;
    mln::ndbuffer_image                  m_tile_out;

    std::function<void(mln::ndbuffer_image&&)>             load_tile;
    std::function<void(mln::ndbuffer_image&&)>             write_tile;
    std::function<mln::ndbuffer_image(int, int)>           create_tile;

  private:
    FilterChain() = default;


  public:
    template <class T>
    static FilterChain MakeChain(int tile_width, int tile_height)
    {
      FilterChain c;
      c.m_tile_in   = mln::image2d<T>(tile_width, tile_height);
      c.m_tile_out  = mln::image2d<T>(tile_width, tile_height);
      c.create_tile = [](int width, int height) -> mln::ndbuffer_image { return mln::image2d<T>(width, height); };
      return c;
    }


    FilterChain(const FilterChain&) = delete;
    FilterChain(FilterChain&&) = default;
    FilterChain& operator=(const FilterChain&) = delete;


    void SetLoadFunction(std::function<void(mln::ndbuffer_image&&)> fn);
    void SetWriteFunction(std::function<void(mln::ndbuffer_image&&)> fn);
    void addFilter(std::unique_ptr<SimpleFilter2D> f);



    mln::box2d ComputeInputRegion(mln::box2d roi) const;

    void                                             ExecuteTile(mln::box2d roi) const final;
    std::unique_ptr<ParallelLocalCanvas2DBase>       clone() const final;
  };

  template <class InputImage, typename V>
  struct DirectTileLoader2D
  {

    void operator()(mln::ndbuffer_image&& out_) const
    {
      auto& out = out_.__cast<V, 2>();
      copy_pad(m_in, out, m_padding_mode, m_padding_value);
    }


    InputImage     m_in;
    e_padding_mode m_padding_mode;
    V              m_padding_value;
  };

  template <class OutputImage, typename V>
  struct DirectTileWriter2D
  {

    void operator()(mln::ndbuffer_image&& in_) const
    {
      auto& in  = in_.__cast<V, 2>();
      mln::paste(in, in.domain(), m_out);
    }

    OutputImage m_out;
  };


  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel, e_padding_mode padding_mode, image_value_t<I> padding_value)
  {
    using V = image_value_t<I>;

    // DilationParallel alg{input, out, se, vs, out.domain(), tile_width, tile_height};


    DirectTileLoader2D<I, V> loader = {input, padding_mode, padding_value};
    DirectTileWriter2D<J, V> writer = {out};

    mln::box2d tile_roi(tile_width, tile_height);
    tile_roi = se.compute_input_region(tile_roi);

    FilterChain chain = FilterChain::MakeChain<V>(tile_roi.width(), tile_roi.height());


    chain.SetLoadFunction(std::cref(loader));
    chain.SetWriteFunction(std::cref(writer));

    bool decompose = false;
    if constexpr (SE::decomposable::value)
    {
      decompose = se.is_decomposable();
      if (decompose)
      {
        auto roi = se.compute_input_region(tile_roi);
        auto ses = se.decompose();

        for (auto se : ses)
        {
          chain.addFilter(
              std::make_unique<SimpleDilation2D<V, decltype(se), ValueSet>>(se, &vs, roi.width(), roi.height()));
          roi = se.compute_output_region(roi);
        }
      }
    }
    if (!decompose)
    {
      chain.addFilter(
          std::make_unique<SimpleDilation2D<V, SE, ValueSet>>(se, &vs, tile_roi.width(), tile_roi.height()));
    }

    chain.execute(out.domain(), tile_width, tile_height, parallel);
  }


  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel)
  {
    dilation2d(input, out, se, vs, tile_width, tile_height, parallel, mln::PAD_CONSTANT, vs.zero);
  }

} // namespace mln::morpho::details
