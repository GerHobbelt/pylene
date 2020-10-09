#pragma once

#include <mln/core/algorithm/paste.hpp>
#include <mln/core/box.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/value/value_traits.hpp>

namespace mln
{
  namespace details
  {
    template <class I, class T>
    [[gnu::noinline]] void copy_block(I& in, mln::box2d roi, T* __restrict out, std::ptrdiff_t out_stride)
    {
      const int x0 = roi.x();
      const int y0 = roi.y();

      for (int y = 0; y < roi.height(); ++y)
      {
        T* lineptr = out + y * out_stride;
        for (int x = 0; x < roi.width(); ++x)
          lineptr[x] = in.at({x0 + x, y0 + y});
      }
    }

    template <class I, class T>
    [[gnu::noinline]] void copy_block(T* __restrict in, std::ptrdiff_t istride, mln::box2d roi, I& out)
    {
      const int x0 = roi.x();
      const int y0 = roi.y();

      for (int y = 0; y < roi.height(); ++y)
      {
        const T* lineptr = in + y * istride;
        for (int x = 0; x < roi.width(); ++x)
          out.at({x0 + x, y0 + y}) = lineptr[x];
      }
    }
/*
    template <class I, class T>
    [[gnu::noinline]] void transpose_block2D(I& in, mln::box2d input_roi, T* __restrict out, std::ptrdiff_t out_stride)
    {
      const int x0 = input_roi.x();
      const int y0 = input_roi.y();


      for (int y = 0; y < input_roi.height(); ++y)
        for (int x = 0; x < input_roi.width(); ++x)
          *(out + x * out_stride + y) = in.at({x0 + x, y0 + y});
    }

    template <class I, class T>
    [[gnu::noinline]] void transpose_block2D(T* __restrict in, std::ptrdiff_t istride, mln::box2d output_roi, I& out)
    {
      const int x0 = output_roi.x();
      const int y0 = output_roi.y();

      for (int y = 0; y < output_roi.height(); ++y)
        for (int x = 0; x < output_roi.width(); ++x)
          out.at({x0 + x, y0 + y}) = *(in + x * istride + y);
    }
*/
  } // namespace details

  /*TODO
  Dilation : DONE
  Erosion : everything
  Opening (Erosion -> Dilation) : everything
  Closing (Dilation -> Erosion) : everything
  Hit Or Miss : everything
  Rank Filter : everything
  Median filter : everything
  Mean (Box) filter : everything
  Morphological gradients : everything
  Top hat : everything
  */

  class TileExecutorBase
  {
  public:
    virtual void execute(mln::ndbuffer_image input, mln::ndbuffer_image output) const = 0;
  };

  class TileLoaderBase
  {
  public:
    virtual void                load_tile(mln::box2d roi, mln::box2d input_roi) const = 0;
    virtual mln::ndbuffer_image get_tile() const                                      = 0;
  };

  class TileWriterBase
  {
  public:
    virtual void                write_tile(mln::box2d roi) const = 0;
    virtual mln::ndbuffer_image get_tile(mln::box2d roi) const   = 0;
  };


  class ParallelLocalCanvas2D
  {
  public:
    int TILE_WIDTH  = 8;
    int TILE_HEIGHT = 8;

    virtual ~ParallelLocalCanvas2D() = default;
    virtual std::unique_ptr<ParallelLocalCanvas2D> clone() const = 0;

    // Compute the input region required to compute the given output region
    virtual mln::box2d              ComputeInputRegion(mln::box2d out_roi) const noexcept = 0;
    virtual const TileLoaderBase*   GetTileLoader() const noexcept                = 0;
    virtual const TileWriterBase*   GetTileWriter() const noexcept                = 0;
    virtual const TileExecutorBase* GetTileExecutor() const noexcept              = 0;

    // The whole output roi we want to compute
    virtual mln::box2d GetOutputRegion() const = 0;

    // Execute to compute this output roi
    virtual void       ExecuteTile(mln::box2d out_roi) const final;
  };

  void parallel_execute_local2D(ParallelLocalCanvas2D& canvas);
  void sequential_execute_local2D(ParallelLocalCanvas2D& canvas);
} // namespace mln
