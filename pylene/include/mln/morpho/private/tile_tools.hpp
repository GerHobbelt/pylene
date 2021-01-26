#pragma once

#include <mln/core/box.hpp>

namespace mln::morpho::details
{

  class TileLoaderBase
  {
    public:
    // Load tile from memory (roi is in the vertical layout coordinates system)
    virtual void load_tile(std::byte* out, std::ptrdiff_t byte_stride, mln::box2d roi) = 0;

    virtual void copy_block(std::byte* out, std::ptrdiff_t byte_stride, mln::box2d roi) = 0;
    virtual void transpose_block(std::byte* out, std::ptrdiff_t byte_stride, mln::box2d output_roi) = 0;
  };

  class TileWriterBase
  {
    public:
    // Copy a line to output (coordinates and size are in the vertical layout coordinates system)
    virtual void write_tile(const std::byte* in, std::ptrdiff_t byte_stride, mln::box2d roi) = 0;

    virtual void copy_block(std::byte* out, std::ptrdiff_t byte_stride, mln::box2d roi) = 0;
    virtual void transpose_block(std::byte* out, std::ptrdiff_t byte_stride, mln::box2d input_roi) = 0;
  };


  template <class I, class T>
  [[gnu::noinline]] void copy_block(I&& in, mln::box2d roi, T* __restrict out, std::ptrdiff_t out_stride)
  {
    const int x0 = roi.x();
    const int y0 = roi.y();

    for (int y = 0; y < roi.height(); ++y)
    {
      T* lineptr = out + y * out_stride;
      for (int x = 0; x < roi.width(); ++x)
        lineptr[x] = in.at(mln::point2d{x0 + x, y0 + y});
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
        out.at(mln::point2d{x0 + x, y0 + y}) = lineptr[x];
    }
  }

  template <class I, class T>
  [[gnu::noinline]] void transpose_block2d(I&& in, mln::box2d input_roi, T* __restrict out,
                                           std::ptrdiff_t out_stride)
  {
    const int x0 = input_roi.x();
    const int y0 = input_roi.y();


    for (int y = 0; y < input_roi.height(); ++y)
      for (int x = 0; x < input_roi.width(); ++x)
        *(out + x * out_stride + y) = in.at(mln::point2d{x0 + x, y0 + y});
  }

  template <class I, class T>
  [[gnu::noinline]] void transpose_block2d(const T* __restrict in, std::ptrdiff_t istride, mln::box2d output_roi,
                                           I& out)
  {
    const int x0 = output_roi.x();
    const int y0 = output_roi.y();

    for (int y = 0; y < output_roi.height(); ++y)
      for (int x = 0; x < output_roi.width(); ++x)
        out.at(mln::point2d{x0 + x, y0 + y}) = *(in + x * istride + y);
  }

  template <class I, class T>
  class TileLoader : public TileLoaderBase
  {
  public:
    // Load tile from memory (roi is in the vertical layout coordinates system)
    void load_tile(std::byte* out, std::ptrdiff_t byte_stride, mln::box2d roi) override final
    {
      if (m_vertical)
        mln::morpho::details::copy_block(*m_input, roi, (T*)out, byte_stride / sizeof(T));
      else
      {
        mln::box2d region(roi.y(), roi.x(), roi.height(), roi.width());
        transpose_block2d(*m_input, region, (T*)out, byte_stride / sizeof(T));
      }
    }


    void copy_block(std::byte* out, std::ptrdiff_t stride, mln::box2d roi) final
    {
      mln::morpho::details::copy_block(*m_input, roi, (T*)out, stride);
    }

    void transpose_block(std::byte* out, std::ptrdiff_t stride, mln::box2d roi) final
    {
      mln::box2d region(roi.y(), roi.x(), roi.height(), roi.width());
      mln::morpho::details::transpose_block2d(*m_input, region, (T*)out, stride);
    }


    TileLoader(I& input, bool vertical)
      : m_input(&input)
      , m_vertical{vertical}
    {
    }

  private:
    I*       m_input;
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
        mln::morpho::details::copy_block((const T*)in, byte_stride / sizeof(T), roi, *m_output);
      else
      {
        mln::box2d region(roi.y(), roi.x(), roi.height(), roi.width());
        transpose_block2d((const T*)in, byte_stride / sizeof(T), region, *m_output);
      }
    }

    void copy_block(std::byte* in, std::ptrdiff_t stride, mln::box2d roi) final
    {
      mln::morpho::details::copy_block((const T*)in, stride, roi, *m_output);
    }

    void transpose_block(std::byte* in, std::ptrdiff_t stride, mln::box2d roi) final
    {
      mln::box2d region(roi.y(), roi.x(), roi.height(), roi.width());
      mln::morpho::details::transpose_block2d((const T*)in, stride, region, *m_output);
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
