#include <mln/morpho/experimental/private/dilation_vertical_block2d.hpp>

#include <memory>

namespace mln::morpho::experimental::details
{


  void vertical_running_max_algo_base_t::running_max_block2d(std::byte* __restrict f, std::byte* __restrict g, std::byte* __restrict h,
                                                             int width, int height, std::ptrdiff_t f_byte_stride,
                                                             std::ptrdiff_t g_byte_stride, std::ptrdiff_t h_byte_stride,
                                                             int k, bool use_extension)
  {
    assert(width <= this->get_block_width());



    const int alpha = 2 * k + 1;

    {
      int chunk_start = use_extension ? -k : 0;
      int rem         = use_extension ? (height + 2 * k) : height;

      for (; rem > 0; chunk_start += alpha, rem -= alpha)
      {
        int chunk_size = std::min(rem, alpha);

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
      for (int i = 0; i < height; ++i)
      {
        this->apply_sup(h + (i - k) * h_byte_stride, //
                        g + (i + k) * g_byte_stride, //
                        f + i * f_byte_stride, static_cast<std::size_t>(width));
      }
    }
  }



} // namespace mln::morpho::experimental::details
