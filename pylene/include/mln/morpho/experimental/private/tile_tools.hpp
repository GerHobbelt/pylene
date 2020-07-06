#pragma once

#include <mln/core/box.hpp>

namespace mln::morpho::experimental::details
{

  class TileLoaderBase
  {
    // Load tile from memory (roi is in the vertical layout coordinates system)
    virtual void load_tile(std::byte* out, std::ptrdiff_t byte_stride, mln::box2d roi) = 0;
  };

  class TileWriterBase
  {
    // Copy a line to output (coordinates and size are in the vertical layout coordinates system)
    virtual void write_tile(const std::byte* in, std::ptrdiff_t byte_stride, mln::box2d roi) = 0;
  };


  template <class I, class T>
  [[gnu::noinline]] void copy_block(const I& in, mln::box2d roi, T* __restrict out, std::ptrdiff_t out_stride)
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
  [[gnu::noinline]] void copy_block(const T* __restrict in, std::ptrdiff_t istride, mln::box2d roi, I& out)
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

  template <class I, class T>
  [[gnu::noinline]] void transpose_block2d(const I& in, mln::box2d input_roi, T* __restrict out,
                                           std::ptrdiff_t out_stride)
  {
    const int x0 = input_roi.x();
    const int y0 = input_roi.y();


    for (int y = 0; y < input_roi.height(); ++y)
      for (int x = 0; x < input_roi.width(); ++x)
        *(out + x * out_stride + y) = in.at({x0 + x, y0 + y});
  }

  template <class I, class T>
  [[gnu::noinline]] void transpose_block2d(const T* __restrict in, std::ptrdiff_t istride, mln::box2d output_roi,
                                           I& out)
  {
    const int x0 = output_roi.x();
    const int y0 = output_roi.y();

    for (int y = 0; y < output_roi.height(); ++y)
      for (int x = 0; x < output_roi.width(); ++x)
        out.at({x0 + x, y0 + y}) = *(in + x * istride + y);
  }

  template <class I, class T>
  class TileLoader : public TileLoaderBase
  {
  public:
    // Load tile from memory (roi is in the vertical layout coordinates system)
    void load_tile(std::byte* out, std::ptrdiff_t byte_stride, mln::box2d roi) override final
    {
      if (m_vertical)
        copy_block(*m_input, roi, (T*)out, byte_stride / sizeof(T));
      else
      {
        mln::box2d region(roi.y(), roi.x(), roi.height(), roi.width());
        transpose_block2d(*m_input, region, (T*)out, byte_stride / sizeof(T));
      }
    }

    TileLoader(const I& input, bool vertical)
      : m_input(&input)
      , m_vertical{vertical}
    {
    }

  private:
    const I* m_input;
    bool     m_vertical;
  };

  template <class I, class T>
  class TileWriter : public TileWriterBase
  {
  public:
    // Copy a line to output (coordinates and size are in the vertical layout coordinates system)
    void write_tile(const std::byte* in, std::ptrdiff_t byte_stride, mln::box2d roi) override final
    {
      if (m_vertical)
        copy_block((const T*)in, byte_stride / sizeof(T), roi, *m_output);
      else
      {
        mln::box2d region(roi.y(), roi.x(), roi.height(), roi.width());
        transpose_block2d((const T*)in, byte_stride / sizeof(T), region, *m_output);
      }
    }

    TileWriter(I& output, bool vertical)
      : m_output(&output)
      , m_vertical{vertical}
    {
    }

  private:
    I*   m_output;
    bool m_vertical;
  };

} // namespace mln::morpho::experimental::details
