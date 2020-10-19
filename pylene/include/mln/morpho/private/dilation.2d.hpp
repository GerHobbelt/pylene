#pragma once

#include <mln/core/canvas/parallel_local.hpp>
#include <mln/core/algorithm/paste.hpp>

#include <mln/core/canvas/private/loader2d.hpp>
#include <mln/core/canvas/private/writer2d.hpp>

#include <mln/morpho/private/block_algorithm.hpp>
#include <mln/morpho/private/block_running_max.hpp>
#include <mln/morpho/private/localmax.hpp>

/// \file Provides specialization for 2d dilation

namespace mln::morpho::details
{

  // Generic 2D dilation for any se (will not perform a decomposition test)
  // Will fallback to the local accumulation canvas
  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& output, const SE& se, ValueSet& vs);


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
        mln::trace::warn("[Performance] Running the specialization with perodic lines.");
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
        auto roi_o = mln::box2d(roi.width(), roi.height());
        auto roi_t = mln::box2d(roi.height(), roi.width());

        { // 1. Transpose
          block_transpose(&in.at(roi.tl()), m_tmp_in.buffer(), roi_t, in.byte_stride(), m_tmp_in.byte_stride());
        }
        { // 2. Run vertically inplace
          block_running_max(m_tmp_in.buffer(), roi_t.width(), roi_t.height(), m_tmp_in.byte_stride(), m_se.repetition(),
                            m_vs->sup, m_vs->zero);
        }
        { // 3. Transpose and write
          block_transpose(m_tmp_in.buffer(), out.buffer(), roi_t, m_tmp_in.byte_stride(), out.byte_stride());
        }
      }
    }

    mln::box2d ComputeInputRegion(mln::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }
    mln::box2d ComputeOutputRegion(mln::box2d roi) const noexcept final { return m_se.compute_output_region(roi); }


    std::unique_ptr<SimpleDilation2D> Clone() const final
    {
      return std::make_unique<SimpleDilation2D>(m_se, m_vs, m_tmp_in.width(), m_tmp_in.height());
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
    mutable mln::ndbuffer_image                  m_tile_in;
    mutable mln::ndbuffer_image                  m_tile_out;

    std::function<void(mln::ndbuffer_image&&)>           load_tile;
    std::function<void(mln::ndbuffer_image&&)>           write_tile;
    std::function<void(mln::ndbuffer_image&, int, int)>  create_tile;

  private:
    FilterChain() = default;


  public:
    template <class T>
    static FilterChain MakeChain(int tile_width, int tile_height)
    {
      FilterChain c;
      c.m_tile_in   = mln::image2d<T>(tile_width, tile_height);
      c.m_tile_out  = mln::image2d<T>(tile_width, tile_height);
      c.create_tile = [](mln::ndbuffer_image& in, int width, int height) { in = mln::image2d<T>(width, height); };
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



/*




    DilationTileExecutor(const SE& se, ValueSet& vs, int tile_width, int tile_height)
      : m_se{&se}
      , m_vs{&vs}

    {
      // If se is _decomposable allocate extra buffer for processing
      if (m_se->is_decomposable())
      {
        m_aux_1.resize(tile_width, tile_height);
        if (auto ses = m_se->decompose(); ses.size() > 2)
          m_aux_2.resize(tile_width, tile_height);
      }
    }

    DilationTileExecutor(const DilationTileExecutor& other)
      : m_se{other.m_se}
      , m_vs{other.m_vs}
    {
      int w = other.m_aux_1.width();
      int h = other.m_aux_1.height();
      m_aux_1.resize(w, h);
      m_aux_2.resize(w, h);
    }

    // For periodic lines
    void execute(mln::image2d<V>& in, mln::image2d<V>& out, const se::periodic_line2d* se) const
    {
      if (se->is_vertical())
      {
        mln::trace::warn("[Performance] Running the specialization with vertical lines.");
        auto roi = out.domain();
        auto tmp = in.clip(roi);
        block_running_max(tmp.buffer(), roi.width(), roi.height(), tmp.byte_stride(), se->repetition(), m_vs->sup,
                          m_vs->zero);
        mln::paste(in, roi, out);
      }
      else if (se->is_horizontal())
      {
      }
      else
      {
        mln::trace::warn("[Performance] Running the specialization with perodic lines.");
        dilation_by_periodic_line(in, out, *se, m_vs->sup, out.domain());
      }
    }

    // Generic
    template <class SE2>
    void execute(mln::image2d<V>& in, mln::image2d<V>& out, const mln::details::StructuringElement<SE2>* se) const
    {
      mln::morpho::details::impl::localmax(in, out, *m_vs, *static_cast<const SE2*>(se), out.domain());
    }

    void execute(mln::ndbuffer_image in_, mln::ndbuffer_image out_) const final
    {
      auto in  = *(in_.cast_to<V, 2>());
      auto out = *(out_.cast_to<V, 2>());


      mln::box2d roi = out.domain();
      assert(in.domain().includes(roi));


      if (m_se->is_decomposable())
      {
        auto input_roi = m_se->compute_input_region(roi);
        auto ses       = m_se->decompose();

        // First dilation
        {
          auto&& se         = ses[0];
          auto   output_roi = se.compute_output_region(input_roi);

          m_aux_1.set_domain_topleft(output_roi.tl());
          m_aux_1.clip(output_roi);

          this->execute(in, m_aux_1, &se);
          input_roi = output_roi;
        }

        // Intermediate dilation
        for (std::size_t i = 1; i < ses.size() - 1; ++i)
        {
          auto&& se         = ses[i];
          auto   output_roi = se.compute_output_region(input_roi);

          m_aux_2.set_domain_topleft(output_roi.tl());
          m_aux_2.clip(output_roi);

          this->execute(m_aux_1, m_aux_2, &se);
          input_roi = output_roi;
          std::swap(m_aux_1, m_aux_2);
        }

        // Last dilation
        {
          auto&& se = ses.back();
          this->execute(m_aux_1, out, &se);
        }
      }
      else
      {
        this->execute(in, out, m_se);
      }
    }

  private:
    const SE*          m_se;
    ValueSet*          m_vs;
    mutable image2d<V> m_aux_1;
    mutable image2d<V> m_aux_2;
  };


    template <class InputImage, class SE, class OutputImage, class ValueSet>
    struct DilationParallel : ParallelLocalCanvas2D
    {
      static_assert(std::is_same_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    private:
      DilationParallel(InputImage& in, OutputImage& out, const SE& se, ValueSet& vs, mln::box2d roi,
                       mln::box2d tile_dims)
        : m_se{se}
        , m_vs{vs}
        , m_output_roi{roi}
        , m_tile_l{in, tile_dims.width(), tile_dims.height(), mln::PAD_CONSTANT, vs.zero}
        , m_tile_w{out}
        , m_tile_e{se, vs, tile_dims.width(), tile_dims.height()}
      {
      }

    public:
      DilationParallel(InputImage& in, OutputImage& out, const SE& se, ValueSet& vs, mln::box2d roi)
        : DilationParallel(in, out, se, vs, roi, se.compute_input_region({TILE_WIDTH, TILE_HEIGHT}))
      {
      }

      DilationParallel(InputImage& in, OutputImage& out, const SE& se, ValueSet& vs, mln::box2d roi, int tile_width,
                       int tile_height)
        : DilationParallel(in, out, se, vs, roi, se.compute_input_region({tile_width, tile_height}))
      {
        ParallelLocalCanvas2D::TILE_WIDTH  = tile_width;
        ParallelLocalCanvas2D::TILE_HEIGHT = tile_height;
      }

      std::unique_ptr<ParallelLocalCanvas2D> clone() const final { return std::make_unique<DilationParallel>(*this); }
      mln::box2d                             GetOutputRegion() const noexcept final { return m_output_roi; }
      mln::box2d ComputeInputRegion(mln::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }

      const TileLoaderBase*   GetTileLoader() const noexcept final { return &m_tile_l; };
      const TileWriterBase*   GetTileWriter() const noexcept final { return &m_tile_w; };
      const TileExecutorBase* GetTileExecutor() const noexcept final { return &m_tile_e; };

    private:
      using tile_loader_t   = mln::details::TileLoader2D<InputImage>;
      using tile_executor_t = DilationTileExecutor<image_value_t<InputImage>, SE, ValueSet>;
      using tile_writer_t   = mln::details::DirectTileWriter2D<OutputImage>;

      SE              m_se;
      ValueSet        m_vs;
      mln::box2d      m_output_roi;
      tile_loader_t   m_tile_l;
      tile_writer_t   m_tile_w;
      tile_executor_t m_tile_e;
    };
*/

    template <class I, class J, class SE, class ValueSet>
    void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height)
    {
      using V = image_value_t<I>;

      //DilationParallel alg{input, out, se, vs, out.domain(), tile_width, tile_height};


      mln::details::DirectTileLoader2D<I, V> loader = {&input, mln::PAD_CONSTANT, vs.zero};
      mln::details::DirectTileWriter2D<J, V> writer = {&out};

      mln::box2d tile_roi(tile_width, tile_height);
      tile_roi = se.compute_input_region(tile_roi);

      FilterChain chain = FilterChain::MakeChain<V>(tile_roi.width(), tile_roi.height());

      chain.SetLoadFunction(std::move(loader));
      chain.SetWriteFunction(std::move(writer));

      if (se.is_decomposable())
      {
        auto roi = se.compute_input_region(tile_roi);
        auto ses       = se.decompose();

        for (auto se : ses)
        {
          chain.addFilter(
            std::make_unique<SimpleDilation2D<V, const decltype(se), ValueSet>>(se, &vs, roi.width(), roi.height()));
          roi = se.compute_output_region(roi);
        }
      }
      else
      {
        chain.addFilter(
          std::make_unique<SimpleDilation2D<V, const SE, ValueSet>>(se, &vs, tile_roi.width(), tile_roi.height()));
      }

      chain.execute(out.domain(), tile_width, tile_height);
    }
  }
