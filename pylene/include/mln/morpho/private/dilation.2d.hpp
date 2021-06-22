#pragma once

#include <mln/core/canvas/parallel_local.hpp>
#include <mln/core/algorithm/paste.hpp>

#include <mln/core/extension/padding.hpp>
#include <mln/morpho/private/block_algorithm.hpp>
#include <mln/morpho/private/block_running_max.hpp>
#include <mln/morpho/private/localmax.hpp>

#include <mln/bp/transpose.hpp>
#include <mln/bp/copy.hpp>
#include <mln/bp/alloc.hpp>

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


  // Generic implementation
  template <class V, class SE, class ValueSet>
  class TileExecutor_Dilation2D final : public TileExecutorBase
  {
  public:
    TileExecutor_Dilation2D(SE se, ValueSet* vs)
      : m_se{se}
      , m_vs{vs}
    {
    }


    void Execute(mln::bp::Tile2DView<void> in_, mln::point2d i_anchor, mln::bp::Tile2DView<void> out_, mln::point2d o_anchor) final
    {
      auto in = mln::bp::downcast<V>(in_);
      auto out = mln::bp::downcast<V>(out_);
      auto in_as_image  = mln::image2d<V>::from_tile(in, i_anchor);
      auto out_as_image = mln::image2d<V>::from_tile(out, o_anchor);

      mln::box2d roi = out_as_image.domain();

      auto tmp = in_as_image.clip(roi);
      mln::morpho::details::impl::localmax(tmp, out_as_image, *m_vs, m_se, roi);
    }

    mln::box2d ComputeInputRegion(mln::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }
    mln::box2d ComputeOutputRegion(mln::box2d roi) const noexcept final { return m_se.compute_output_region(roi); }
    int        GetElementSize() const noexcept final { return sizeof(V); }
    bool       NeedTranspose() const noexcept final { return false; }

  public:
    SE                       m_se;
    ValueSet*                m_vs;
  };


  // Specialization for periodic lines
  template <class V, class ValueSet>
  class TileExecutor_Dilation2D<V, mln::se::periodic_line2d, ValueSet> final : public TileExecutorBase
  {
  private:
    enum orientation_t
    {
      VERTICAL,
      HORIZONTAL,
      OTHER
    };

    mln::se::periodic_line2d m_se;
    ValueSet*                m_vs;
    orientation_t            m_orient;

  public:
    TileExecutor_Dilation2D(mln::se::periodic_line2d se, ValueSet* vs)
      : m_se{se}
      , m_vs{vs}
    {
      if (se.is_vertical())
        m_orient = VERTICAL;
      else if (se.is_horizontal())
        m_orient = HORIZONTAL;
      else
        m_orient = OTHER;
    }

    int  GetElementSize() const noexcept final { return sizeof(V); }
    bool NeedTranspose() const noexcept final { return m_orient == HORIZONTAL; }


    void Execute(mln::bp::Tile2DView<void> in_, mln::point2d i_anchor, mln::bp::Tile2DView<void> out_, mln::point2d o_anchor) final
    {
      auto in           = mln::bp::downcast<V>(in_);
      auto out          = mln::bp::downcast<V>(out_);

      if (m_orient == OTHER)
      {
        auto in_as_image  = mln::image2d<V>::from_tile(in, i_anchor);
        auto out_as_image = mln::image2d<V>::from_tile(out, o_anchor);
        auto roi          = out_as_image.domain();

        auto tmp = in_as_image.clip(roi);
        dilation_by_periodic_line(tmp, out_as_image, m_se, m_vs->sup, roi);
      }
      else if (m_orient == VERTICAL || m_orient == HORIZONTAL)
      {
        auto diff       = o_anchor - i_anchor;
        auto in_clipped = in.clip(diff.x(), diff.y(), out.width(), out.height());

        block_running_max(in_clipped.data(), in_clipped.width(), in_clipped.height(), in.stride(), m_se.repetition(), m_vs->sup, m_vs->zero);
        mln::bp::copy(in_clipped, out);
      }
    }

    mln::box2d ComputeInputRegion(mln::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }
    mln::box2d ComputeOutputRegion(mln::box2d roi) const noexcept final { return m_se.compute_output_region(roi); }
  };


  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel, e_padding_mode padding_mode, image_value_t<I> padding_value)
  {
    using V = image_value_t<I>;

    // DilationParallel alg{input, out, se, vs, out.domain(), tile_width, tile_height};

    TileExecutor_Dilation2D<V, SE, ValueSet> exec(se, &vs);
    LocalFilter2D<V> filter;
    filter.set_input(input, padding_mode, padding_value);
    filter.set_output(out);
    filter.set_executor(&exec);

    filter.execute(out.domain(), tile_width, tile_height, parallel);

    /*
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
    */
  }


  template <class I, class J, class SE, class ValueSet>
  void dilation2d(I& input, J& out, const SE& se, ValueSet& vs, int tile_width, int tile_height, bool parallel)
  {
    dilation2d(input, out, se, vs, tile_width, tile_height, parallel, mln::PAD_CONSTANT, vs.zero);
  }

} // namespace mln::morpho::details
