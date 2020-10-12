#include <mln/morpho/private/block_running_max.hpp>

#include <memory>

namespace mln::morpho::details
{

  void block_running_max_algo_base::run(int block_width, int e_size, std::byte* in, int width, int height, std::ptrdiff_t byte_stride, int k, void* sup)
  {
    bool use_extension = true;

    //
    std::size_t kBlockLineByteSize = e_size * block_width;
    std::byte* g0 = (std::byte*)std::malloc(kBlockLineByteSize * (height + 2 * k));
    std::byte* h0 = (std::byte*)std::malloc(kBlockLineByteSize * (height + 2 * k));
    std::byte* g = g0 + k * kBlockLineByteSize;
    std::byte* h = h0 + k * kBlockLineByteSize;

    const int alpha = 2 * k + 1;
    {
      int chunk_start = use_extension ? -k : 0;
      int rem         = use_extension ? (height + 2 * k) : height;

      for (; rem > 0; chunk_start += alpha, rem -= alpha)
      {
        int chunk_size = std::min(rem, alpha);

        // Forward pass
        // Compute g[x] = Max f(y), y ∈ [α * ⌊x / α⌋ : x]
        this->partial_sum(in + chunk_start * byte_stride,    //
                           g + chunk_start * kBlockLineByteSize, //
                           width, chunk_size, byte_stride, kBlockLineByteSize, sup);

        // Backward pass
        // Compute h[x] = Max f(y) y ∈ [x : α * (⌊x/α⌋+1))
        this->partial_sum(in + (chunk_start + chunk_size - 1) * byte_stride,    //
                           h + (chunk_start + chunk_size - 1) * kBlockLineByteSize, //
                           width, chunk_size, -byte_stride, -kBlockLineByteSize, sup);
      }
    }

    // Compute local maximum out[x] = Max f(y) y ∈ [x-k:x+k]
    // out[x] = Max   (Max f[x-k:b),  Max f[b:x+k]) with b = α.⌈(x-k)/α⌉ = α.⌊(x+k)/α⌋
    //        = Max( h[x-k], g[x+k] )
    this->transform(g + k * kBlockLineByteSize, h - k * kBlockLineByteSize, in, width, height, kBlockLineByteSize, kBlockLineByteSize, byte_stride, sup);
    std::free(g0);
    std::free(h0);
  }

}
