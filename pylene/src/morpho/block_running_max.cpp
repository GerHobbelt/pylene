#include <mln/morpho/private/block_running_max.hpp>

namespace mln::morpho::details
{

  void block_running_max_algo_base::run(int block_width, int e_size, const std::byte* in, std::byte* out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, int k, void* sup)
  {
    bool use_extension = true;

    //
    std::size_t kBlockLineByteSize = e_size * kBlockWidth;
    std::byte* g = (std::byte*)std::malloc(kBlockByteSize * (height + 2 * k));
    std::byte* h = (std::byte*)std::malloc(kBlockByteSize * (height + 2 * k));
    std::byte* g_shifted = g + k * kBlockLineByteSize;
    std::byte* h_shifted = h + k * kBlockLineByteSize;

    const int alpha = 2 * k + 1;
    {
      int chunk_start = use_extension ? -k : 0;
      int rem         = use_extension ? (height + 2 * k) : height;

      for (; rem > 0; chunk_start += alpha, rem -= alpha)
      {
        int chunk_size = std::min(rem, alpha);

        // Forward pass
        // Compute g[x] = Max f(y), y ∈ [α * ⌊x / α⌋ : x]
        this->partial_sum_block2d(in + chunk_start * in_byte_stride, //
                                  g + chunk_start * kBlockLineByteSize,   //
                                  width, chunk_size, in_byte_stride, kBlockLineByteSize, sup);

        // Backward pass
        // Compute h[x] = Max f(y) y ∈ [x : α * (⌊x/α⌋+1))
        this->partial_sum_block2d(in + (chunk_start + chunk_size - 1) * in_byte_stride, //
                                  h + (chunk_start + chunk_size - 1) * kBlockLineByteSize,   //
                                  width, chunk_size, -in_byte_stride, -kBlockLineByteSize, sup);
      }
    }

    // Compute local maximum out[x] = Max f(y) y ∈ [x-k:x+k]
    // out[x] = Max   (Max f[x-k:b),  Max f[b:x+k]) with b = α.⌈(x-k)/α⌉ = α.⌊(x+k)/α⌋
    //        = Max( h[x-k], g[x+k] )
    this->transform(g, h, out, width, height, kBlockLineByteSize, kBlockLineByteSize, out_byte_stride, sup);
  }

}
