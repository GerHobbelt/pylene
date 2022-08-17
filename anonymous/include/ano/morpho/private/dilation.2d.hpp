#pragma once

#include <ano/core/algorithm/paste.hpp>
#include <ano/core/canvas/parallel_local.hpp>

#include <ano/core/extension/padding.hpp>
#include <ano/morpho/private/block_algorithm.hpp>
#include <ano/morpho/private/block_running_max.hpp>
#include <ano/morpho/private/localmax.hpp>

#include <ano/bp/alloc.hpp>
#include <ano/bp/transpose.hpp>

/// \file Provides specialization for 2d dilation

namespace ano::morpho::details
{

  // Generic 2D dilation for any se (will not perform a decomposition test)
  // Will fallback to the local accumulation canvas
  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel);

  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel,
                  e_padding_mode padding_mode, image_value_t<I> padding_value);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  class SimpleFilter2D
  {
  public:
    SimpleFilter2D() = default;

    virtual ~SimpleFilter2D()                                                                         = default;
    virtual void                            Execute(ano::ndbuffer_image& in, ano::ndbuffer_image out) = 0;
    virtual ano::box2d                      ComputeInputRegion(ano::box2d roi) const noexcept         = 0;
    virtual ano::box2d                      ComputeOutputRegion(ano::box2d roi) const noexcept        = 0;
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


    void Execute(ano::ndbuffer_image& in_, ano::ndbuffer_image out_) final
    {
      auto&      in  = in_.__cast<V, 2>();
      auto&      out = out_.__cast<V, 2>();
      ano::box2d roi = out.domain();
      auto       tmp = in.clip(roi);
      ano::morpho::details::impl::localmax(tmp, out, *m_vs, m_se, roi);
    }

    std::unique_ptr<SimpleFilter2D> Clone() const final { return std::make_unique<SimpleDilation2D>(*this); }

    ano::box2d ComputeInputRegion(ano::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }
    ano::box2d ComputeOutputRegion(ano::box2d roi) const noexcept final { return m_se.compute_output_region(roi); }

  private:
    SE        m_se;
    ValueSet* m_vs;
  };


  template <class V, class ValueSet>
  class SimpleDilation2D<V, ano::se::periodic_line2d, ValueSet> final : public SimpleFilter2D
  {
  public:
    SimpleDilation2D(ano::se::periodic_line2d se, ValueSet* vs, int input_tile_width, int input_tile_height)
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
        // m_tmp_in = ano::image2d<V>(input_tile_height, input_tile_width);
        std::ptrdiff_t pitch;
        m_tmp_in        = ano::bp::aligned_alloc_2d<V>(input_tile_height, input_tile_width, pitch);
        m_tmp_in_width  = input_tile_height;
        m_tmp_in_height = input_tile_width;
        m_tmp_in_stride = pitch;
      }
    }

    ~SimpleDilation2D() final
    {
      if (m_tmp_in)
        ano::bp::aligned_free_2d(m_tmp_in, m_tmp_in_width, m_tmp_in_height, m_tmp_in_stride);
    }

    SimpleDilation2D(const SimpleDilation2D&)            = delete;
    SimpleDilation2D(SimpleDilation2D&&)                 = default;
    SimpleDilation2D& operator=(const SimpleDilation2D&) = delete;
    SimpleDilation2D& operator=(SimpleDilation2D&&)      = default;


    void Execute(ano::ndbuffer_image& in_, ano::ndbuffer_image out_) final
    {
      auto& in  = in_.__cast<V, 2>();
      auto& out = out_.__cast<V, 2>();
      auto  roi = out.domain();

      if (m_orient == OTHER)
      {
        ano::trace::warn("[Performance] Running the specialization with perodic lines (1).");
        dilation_by_periodic_line(in, out, m_se, m_vs->sup, roi);
      }
      else if (m_orient == VERTICAL)
      {
        ano::trace::warn("[Performance] Running the specialization with vertical lines.");
        block_running_max(&in.at(roi.tl()), roi.width(), roi.height(), in.byte_stride(), m_se.repetition(), m_vs->sup,
                          m_vs->zero);
        ano::paste(in, roi, out);
      }
      else if (m_orient == HORIZONTAL)
      {
        ano::trace::warn("[Performance] Running the specialization with horizontal lines.");
        auto input_roi = this->ComputeInputRegion(roi);

        { // 1. Transpose
          ano::bp::transpose(&in.at(input_roi.tl()), m_tmp_in, input_roi.height(), input_roi.width(), in.byte_stride(),
                             m_tmp_in_stride);
        }

        V* block_ptr = ano::ptr_offset(m_tmp_in, m_se.repetition() * m_tmp_in_stride);
        { // 2. Run vertically inplace
          block_running_max(block_ptr, roi.height(), roi.width(), m_tmp_in_stride, m_se.repetition(), m_vs->sup,
                            m_vs->zero);
        }
        { // 3. Transpose and write
          ano::bp::transpose(block_ptr, out.buffer(), roi.width(), roi.height(), m_tmp_in_stride, out.byte_stride());
        }
      }
    }

    ano::box2d ComputeInputRegion(ano::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }
    ano::box2d ComputeOutputRegion(ano::box2d roi) const noexcept final { return m_se.compute_output_region(roi); }


    std::unique_ptr<SimpleFilter2D> Clone() const final
    {
      return std::make_unique<SimpleDilation2D>(m_se, m_vs, m_tmp_in_height, m_tmp_in_width);
    }


  private:
    enum orientation_t
    {
      HORIZONTAL,
      VERTICAL,
      OTHER
    };

    ano::se::periodic_line2d m_se;
    ValueSet*                m_vs;
    orientation_t            m_orient;
    V*                       m_tmp_in = nullptr; // Temporary image used for transposition
    int                      m_tmp_in_width;
    int                      m_tmp_in_height;
    std::ptrdiff_t           m_tmp_in_stride;
  };


  class FilterChain : public ParallelLocalCanvas2DBase
  {
    std::vector<std::unique_ptr<SimpleFilter2D>> m_filters; // List of filters to apply
    ano::ndbuffer_image                          m_tile_in;
    ano::ndbuffer_image                          m_tile_out;

    std::function<void(ano::ndbuffer_image&&)>   load_tile;
    std::function<void(ano::ndbuffer_image&&)>   write_tile;
    std::function<ano::ndbuffer_image(int, int)> create_tile;

  private:
    FilterChain() = default;


  public:
    template <class T>
    static FilterChain MakeChain(int tile_width, int tile_height)
    {
      FilterChain c;
      c.m_tile_in   = ano::image2d<T>(tile_width, tile_height);
      c.m_tile_out  = ano::image2d<T>(tile_width, tile_height);
      c.create_tile = [](int width, int height) -> ano::ndbuffer_image { return ano::image2d<T>(width, height); };
      return c;
    }


    FilterChain(const FilterChain&)            = delete;
    FilterChain(FilterChain&&)                 = default;
    FilterChain& operator=(const FilterChain&) = delete;


    void SetLoadFunction(std::function<void(ano::ndbuffer_image&&)> fn);
    void SetWriteFunction(std::function<void(ano::ndbuffer_image&&)> fn);
    void addFilter(std::unique_ptr<SimpleFilter2D> f);


    ano::box2d ComputeInputRegion(ano::box2d roi) const;

    void                                       ExecuteTile(ano::box2d roi) const final;
    std::unique_ptr<ParallelLocalCanvas2DBase> clone() const final;
  };

  template <class InputImage, typename V>
  struct DirectTileLoader2D
  {

    void operator()(ano::ndbuffer_image&& out_) const
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

    void operator()(ano::ndbuffer_image&& in_) const
    {
      auto& in = in_.__cast<V, 2>();
      ano::paste(in, in.domain(), m_out);
    }

    OutputImage m_out;
  };


  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel,
                  e_padding_mode padding_mode, image_value_t<I> padding_value)
  {
    using V = image_value_t<I>;

    // DilationParallel alg{input, out, se, vs, out.domain(), tile_width, tile_height};


    DirectTileLoader2D<I, V> loader = {input, padding_mode, padding_value};
    DirectTileWriter2D<J, V> writer = {out};

    ano::box2d tile_roi(tile_width, tile_height);
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
    dilation2d(input, out, se, vs, tile_width, tile_height, parallel, ano::PAD_CONSTANT, vs.zero);
  }

} // namespace ano::morpho::details
