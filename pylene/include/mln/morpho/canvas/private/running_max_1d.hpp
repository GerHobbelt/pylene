#ifndef MLN_MORPHO_CANVAS_RUNNING_MAX_1D_HPP
#define MLN_MORPHO_CANVAS_RUNNING_MAX_1D_HPP

#include <algorithm>
#include <mln/core/assert.hpp>

namespace mln
{
  namespace morpho
  {
    namespace internal
    {

      /// \brief The Van-herk algorithm to compute the maximum over a sliding windows
      /// The algorithm runs inplace over the \p f array
      ///
      /// It computes: f[x + offset] = Max f[y] y ∈ [x:x+k)
      /// Intermediate arrays are used to store:
      /// g[x] = Max f[y] y ∈ [⌊x/k⌋*k:x]
      /// h[x] = Max f[y] y ∈ [x:⌈x/k⌉*k]
      ///
      /// \param[in,out] f Input array of size \p n
      /// \param[in,out] g Temporary array of size \p n
      /// \param[in,out] h Temporary array of size \p n
      /// \param[in] n Size of the array
      /// \param[in] k Size of the sliding windows
      /// \param[in] offset Offset of the sliding windows
      /// \param[in] cmp Comparison function
      /// \param
      template <class T, class BinaryFunction>
      void running_max_1d(T* f, T* g, T* h, int n, int k, int offset, BinaryFunction sup, T zero)
      {
        mln_precondition(n > 0);
        mln_precondition(k > 0);

        // Forward pass
        // Compute g = Max in range (floor(x / k) * k, x)
        {
          for (int x = 0; x < n; x += k)
          {
            g[x]  = f[x];
            int r = std::min(n - x, k);
            for (int i = 1; i < r; i++)
              g[x + i] = sup(g[x + i - 1], f[x + i]);
          }
        }

        // Backward pass
        // Compute h = Max in range (x, k * ceil(x/k))
        {
          h[0] = f[0];
          for (int x = 0; x < n; x += k)
          {
            int r    = std::min(k, n - x - 1);
            h[x + r] = f[x + r];
            for (int i = r - 1; i > 0; i--)
              h[x + i] = sup(h[x + i + 1], f[x + i]);
          }
        }

        // Compute local maximum out[x] = Max f[y:y+k) with y = x + offset
        // If x % k != 0:
        // out[x] = Max( Max input[x : k * ceil(x/k))), Max input[x * ceil(x/k) : x + k)
        //        = Max( Max input[x : k * ceil(x/k))), Max input[k * floor((x+k) / k) : x + k)
        //        = Max( Max input[x : k * ceil(x/k))), Max input[k * floor((x+k) / k) : x + k)
        //        = Max( h[x], g[x+k-1] )
        // Else if x % k = 0
        // out[x] = Max input[x : x + k)
        //        = Max input[x : k * (x / k + 1)) = h[x]

        // Offset the index and pointers
        {
          int x = 0;
          int y = x + offset;

          // We store in f[x] = Max f[y:y+k)
          int ymax = n + offset; // Note: x < n <=> y < ymax
          int yend;

          // Case 1: while y + k ≤ 0 → Max f[y:y+k) = 0
          {
            yend = std::min(ymax, -k + 1);
            for (; y < yend; x++, y++)
              f[x] = zero;
          }

          // Case 2: While y < 0 && y + k < n
          {
            yend = std::min(ymax, std::min(0, n - k + 1));
            for (; y < yend; y++, x++)
              f[x] = g[y + k - 1];
            if (x == n)
              return;
          }

          // Case 2: While y < 0 && n ≤ y + k
          {
            yend = std::min(ymax, 0);
            for (; y < yend; ++y, ++x)
              f[x] = g[n - 1];
            if (x == n)
              return;
          }

          mln_assertion(y >= 0);

          // Case 4: While 0 ≤ y && y + k ≤ n
          {
            yend = std::min(ymax, n - k + 1);
            for (; y < yend; y++, x++)
              f[x] = sup(h[y], g[y + k - 1]);
            if (x == n)
              return;
          }

          // Case 4: While  0 ≤ y ≤ ⌈n-1⌉ₖ < n ≤ y + k
          {
            yend = std::min(ymax, (n - 1) / k * k + 1);
            for (; y < yend; y++, x++)
              f[x] = sup(h[y], g[n - 1]);
            if (x == n)
              return;
          }

          // Case 4: While ⌈n-1⌉ₖ < y < n ≤ y + k
          {
            yend = std::min(ymax, n);
            for (; y < yend; y++, x++)
              f[x] = h[y];
            if (x == n)
              return;
          }

          // Case 5: While n ≤ y
          for (; x < n; ++x)
            f[x] = zero;
        }
      }

    } // end of namespace mln::morpho::internal
  }   // end of namespace mln::morpho
} // end of namespace mln

#endif //! MLN_MORPHO_CANVAS_RUNNING_MAX_1D_HPP
