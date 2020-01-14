#pragma once

#include <mln/core/image/experimental/ndbuffer_image.hpp>
#include <mln/core/image/experimental/ndimage_fwd.hpp>
#include <cstddef>
#include <cstdint>

namespace mln::morpho::experimental::details
{

  /// Immersion algorithm for raw-contiguous buffer
  ///
  /// Given an input buffer of size \c n
  /// [a, b, c, d, e],
  /// it computes:
  /// inf = [a; min(a,b); b; min(b,c); c; min(c,d); d; min(d,e); e]
  /// sup = [a; max(a,b); b; max(b,c); c; max(c,d); d; max(d,e); e]
  template <class T>
  void buffer_immersion(const T* __restrict input, std::size_t n, T* __restrict inf, T* __restrict sup);


  /// Interpolation algorithm for raw-contiguous buffer.
  /// Compute the PW min/max between two buffer
  ///
  /// Given two input buffers
  /// inf = [a; min(a,b); b; min(b,c); c; min(c,d); d; min(d,e); e]
  /// sup = [a; max(a,b); b; max(b,c); c; max(c,d); d; max(d,e); e]
  template <class T>
  void buffer_interpolation_max(const T* __restrict A, const T* __restrict B, std::size_t n, T* __restrict out);

  template <class T>
  void buffer_interpolation_min(const T* __restrict A, const T* __restrict B, std::size_t n, T* __restrict out);


  struct immersion_impl_table_base_t
  {
    virtual void immersion(void* input, std::size_t n, void* inf, void* sup) = 0;
    virtual void interpolation_max(void* A, void* B, std::size_t n, void* out) = 0;
    virtual void interpolation_min(void* A, void* B, std::size_t n, void* out) = 0;
  };

  /// Type-erased immersion algorithm for any buffer-encoded image
  void immersion_ndimage(ndbuffer_image& input,
                         ndbuffer_image& inf,
                         ndbuffer_image& sup,
                         immersion_impl_table_base_t* impl);


  template <class T>
  struct immersion_impl_table_t : immersion_impl_table_base_t
  {
    void immersion(void* input, std::size_t n, void* inf, void* sup) final
    {
      buffer_immersion(static_cast<const T*>(input), n, static_cast<T*>(inf), static_cast<T*>(sup));
    }

    void interpolation_max(void* A, void* B, std::size_t n, void* out) final
    {
      buffer_interpolation_max(static_cast<T*>(A), static_cast<T*>(B), n, static_cast<T*>(out));
    }
    void interpolation_min(void* A, void* B, std::size_t n, void* out) final
    {
      buffer_interpolation_min(static_cast<T*>(A), static_cast<T*>(B), n, static_cast<T*>(out));
    }
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class T>
  void buffer_immersion(const T* __restrict f, std::size_t n, T* __restrict inf, T* __restrict sup)
  {
    if (n == 0)
      return;

    inf[0] = f[0];
    sup[0] = f[0];

    for (std::size_t x = 1; x < n; ++x)
    {
      auto [m, M]    = std::minmax(f[x-1], f[x]);
      inf[2 * x - 1] = m;
      sup[2 * x - 1] = M;
      inf[2 * x]     = f[x];
      sup[2 * x]     = f[x];
    }
  }

  template <class T>
  void buffer_interpolation_max(const T* __restrict A, const T* __restrict B, std::size_t n, T* __restrict out)
  {
    for (std::size_t i = 0; i < n; ++i)
      out[i] = std::max(A[i], B[i]);
  }

  template <class T>
  void buffer_interpolation_min(const T* __restrict A, const T* __restrict B, std::size_t n, T* __restrict out)
  {
    for (std::size_t i = 0; i < n; ++i)
      out[i] = std::min(A[i], B[i]);
  }
}
