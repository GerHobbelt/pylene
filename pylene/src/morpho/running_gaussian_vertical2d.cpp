#include <mln/morpho/experimental/gaussian_directional_2d.hpp>

#include <memory>

namespace mln::morpho::experimental::details
{


  void running_gaussian2d_algo_base_t::running_gaussian_block2d(std::byte* __restrict f, std::byte* __restrict g,
                                                                std::byte* __restrict h, std::ptrdiff_t f_byte_stride,
                                                                std::ptrdiff_t g_byte_stride,
                                                                std::ptrdiff_t h_byte_stride, mln::box2d roi,
                                                                const recursivefilter_coef_& coef, bool use_extension)
  {
    int x0     = roi.x();
    int y0     = roi.y();
    int width  = roi.width();
    int height = roi.height();

    assert(width <= this->get_block_width());

    const int alpha = 2 * k + 1;

    {
      int chunk_start = use_extension ? -k : 0;
      int rem         = use_extension ? (height + 2 * k) : height;

      for (; rem > 0; chunk_start += alpha, rem -= alpha)
      {
        int chunk_size = std::min(rem, alpha);

        // Copy the into the tile$
        if (m_tile_loader)
        {
          mln::box2d region(x0, y0 + chunk_start, width, chunk_size);
          m_tile_loader->load_tile(f + chunk_start * f_byte_stride, f_byte_stride, region);
        }

        // Forward pass
        // Compute g[x] = Max f(y), y ∈ [α * ⌊x / α⌋ : x]
        this->partial_sum_block2d(f + chunk_start * f_byte_stride, //
                                  g + chunk_start * g_byte_stride, //
                                  width, chunk_size, f_byte_stride, g_byte_stride);

        // Backward pass
        // Compute h[x] = Max f(y) y ∈ [x : α * (⌊x/α⌋+1))
        this->partial_sum_block2d(f + (chunk_start + chunk_size - 1) * f_byte_stride, //
                                  h + (chunk_start + chunk_size - 1) * h_byte_stride, //
                                  width, chunk_size, -f_byte_stride, -h_byte_stride);
      }
    }


    // Compute local maximum out[x] = Max f(y) y ∈ [x-k:x+k]
    // out[x] = Max   (Max f[x-k:b),  Max f[b:x+k]) with b = α.⌈(x-k)/α⌉ = α.⌊(x+k)/α⌋
    //        = Max( h[x-k], g[x+k] )
    {
      const int kBlockHeight = 16;
      for (int y = 0; y < height; y += kBlockHeight)
      {
        int hroi = std::min(kBlockHeight, height - y);
        for (int i = 0; i < hroi; ++i)
        {
          this->apply_sup(h + (y + i - k) * h_byte_stride, //
                          g + (y + i + k) * g_byte_stride, //
                          f + (y + i) * f_byte_stride, width);
        }

        // Write the tile
        if (m_tile_writer)
        {
          mln::box2d region(x0, y0 + y, width, hroi);
          m_tile_writer->write_tile(f + y * f_byte_stride, f_byte_stride, region);
        }
      }
    }
  }


  void running_gaussian2d_algo_base_t::execute(mln::box2d roi, const recursivefilter_coef_& coef, bool vertical,
                                               bool use_extension)
  {
    int            kBlockWidth    = this->get_block_width();
    auto           sz             = this->get_sample_size();
    std::ptrdiff_t kBlockByteSize = sz * kBlockWidth;

    const int x0     = (vertical) ? roi.x() : roi.y();
    const int y0     = (vertical) ? roi.y() : roi.x();
    const int width  = (vertical) ? roi.width() : roi.height();
    const int height = (vertical) ? roi.height() : roi.width();


    std::byte* f = (std::byte*)std::malloc(kBlockByteSize * (height + 2 * k));
    std::byte* g = (std::byte*)std::malloc(kBlockByteSize * (height + 2 * k));
    std::byte* h = (std::byte*)std::malloc(kBlockByteSize * (height + 2 * k));

    std::byte* f_shifted = f + k * kBlockByteSize;
    std::byte* g_shifted = g + k * kBlockByteSize;
    std::byte* h_shifted = h + k * kBlockByteSize;

    for (int x = 0; x < width; x += kBlockWidth)
    {
      int w = std::min(kBlockWidth, width - x);


      mln::box2d region(x0 + x, y0, w, height);
      this->running_gaussian_block2d(f_shifted, g_shifted, h_shifted, kBlockByteSize, kBlockByteSize, kBlockByteSize,
                                     region, coef, use_extension);
    }

    std::free(f);
    std::free(g);
    std::free(h);
  }

} // namespace mln::morpho::experimental::details
