#pragma once

#include <algorithm>
#include <numeric>

namespace mln::morpho::details
{
  /// \brief The Van-herk algorithm to compute the maximum over a sliding windows
  /// The algorithm runs inplace over the \p f array
  ///
  /// It computes: f[x] = Max f[y] y ∈ [x-k:x+k]
  /// Let α = 2.k+1
  /// Intermediate arrays are used to store:
  /// g[x] = Max f[y] y ∈ [⌊x/α⌋*α:x]
  /// h[x] = Max f[y] y ∈ [x:(⌊x/α⌋+1)*α)
  ///
  /// \param[in,out] f Input array of size \p n + 2.k
  /// \param[in,out] g Temporary array of size \p n + 2.k
  /// \param[in,out] h Temporary array of size \p n + 2.k
  /// \param[in] n Size of the array
  /// \param[in] k Radius of the sliding windows
  /// \param[in] sup Supremum operator
  /// \param[in] use_extension If true, use values from extension. if false assume that they have no influence
  template <class T, class BinaryFunction>
  [[gnu::noinline]] void running_max_1d(T* __restrict f, T* __restrict g, T* __restrict h, int n, int k, BinaryFunction sup, bool use_extension = true)
  {
    if (n == 0)
      return;

    const int alpha = 2 * k + 1;
    const int size  = n + 2 * k;

    // Forward pass
    // Compute g[x] = Max f(y), y ∈ [α * ⌊x / α⌋ : x]
    {
      int chunk_start = use_extension ? -k : 0;
      int rem         = use_extension ? size : n;

      for (; rem > 0; chunk_start += alpha, rem -= alpha)
      {
        int chunk_size = std::min(rem, alpha);
        std::partial_sum(f + chunk_start, f + chunk_start + chunk_size, g + chunk_start, sup);
      }
    }

    // Backward pass
    // Compute h[x] = Max f(y) y ∈ [x : α * (⌊x/α⌋+1))
    {
      int chunk_start = use_extension ? -k : 0;
      int rem         = use_extension ? size : n;

      for (; rem > 0; chunk_start += alpha, rem -= alpha)
      {
        int chunk_size = std::min(alpha, rem);
        std::partial_sum(std::make_reverse_iterator(f + chunk_start + chunk_size),
                         std::make_reverse_iterator(f + chunk_start),
                         std::make_reverse_iterator(h + chunk_start + chunk_size), sup);
      }
    }

    // Compute local maximum out[x] = Max f(y) y ∈ [x-k:x+k]
    // out[x] = Max   (Max f[x-k:b),  Max f[b:x+k]) with b = α.⌈(x-k)/α⌉ = α.⌊(x+k)/α⌋
    //        = Max( h[x-k], g[x+k] )
    {
      for (int i = 0; i < n; ++i)
        f[i] = sup(h[i-k], g[i+k]);
    }
  }
} // namespace mln::morpho::details
