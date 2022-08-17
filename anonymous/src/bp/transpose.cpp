#include <ano/bp/copy.hpp>
#include <ano/bp/swap.hpp>
#include <ano/bp/transpose.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <xsimd/xsimd.hpp>


namespace
{
  /// N = Taille des éléments des blocs (matrice NxN)
  /// W = Largeur de la grille (=Hauteur de grille)
  template <class T, int N, int W>
  struct Transposer2x2;


#if XSIMD_X86_INSTR_SET >= XSIMD_X86_AVX_VERSION

  template <int nbytes>
  struct avx_traits;

  template <>
  struct avx_traits<0>
  {
    static __m128i load(__m128i* a) { return _mm_loadu_si128(a); }
    static __m256i load(__m256i* a) { return _mm256_loadu_si256(a); }
    static void    store(__m128i* a, __m128i b) { return _mm_storeu_si128(a, b); }
    static void    store(__m256i* a, __m256i b) { return _mm256_storeu_si256(a, b); }
  };

  template <>
  struct avx_traits<1> : avx_traits<0>
  {
    static const __m128i shuffle_v;

    static __m128i unpackhi(__m128i a, __m128i b) { return _mm_unpackhi_epi8(a, b); };
    static __m128i unpacklo(__m128i a, __m128i b) { return _mm_unpacklo_epi8(a, b); };
    static __m256i unpackhi(__m256i a, __m256i b) { return _mm256_unpackhi_epi8(a, b); };
    static __m256i unpacklo(__m256i a, __m256i b) { return _mm256_unpacklo_epi8(a, b); };
    static __m128i desinterleave(__m128i a) { return _mm_shuffle_epi8(a, shuffle_v); }
    static __m256i desinterleave(__m256i a)
    {
      const __m256i v = _mm256_broadcastsi128_si256(shuffle_v);
      return _mm256_shuffle_epi8(a, v);
    }
  };

  const __m128i avx_traits<1>::shuffle_v = _mm_set_epi8(15, 13, 11, 9, 7, 5, 3, 1, 14, 12, 10, 8, 6, 4, 2, 0);


  template <>
  struct avx_traits<2> : avx_traits<0>
  {
    static const __m128i shuffle_v;

    static __m128i unpackhi(__m128i a, __m128i b) { return _mm_unpackhi_epi16(a, b); };
    static __m128i unpacklo(__m128i a, __m128i b) { return _mm_unpacklo_epi16(a, b); };
    static __m128i desinterleave(__m128i a) { return _mm_shuffle_epi8(a, shuffle_v); }

    static __m256i unpackhi(__m256i a, __m256i b) { return _mm256_unpackhi_epi16(a, b); };
    static __m256i unpacklo(__m256i a, __m256i b) { return _mm256_unpacklo_epi16(a, b); };
    static __m256i desinterleave(__m256i a)
    {
      const __m256i v = _mm256_broadcastsi128_si256(shuffle_v);
      return _mm256_shuffle_epi8(a, v);
    }

    /*
    static __m128i desinterleave(__m128i a) {
      // FIXME ADD AVX512 specialization here
      a = _mm_shufflelo_epi16(a, 0b11011000);
      a = _mm_shufflehi_epi16(a, 0b11011000);
      return _mm_shuffle_epi32(a, 0b11011000);
    };
    */
  };
  const __m128i avx_traits<2>::shuffle_v = _mm_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0);

  template <>
  struct avx_traits<4> : avx_traits<0>
  {
    static __m128i unpackhi(__m128i a, __m128i b) { return _mm_unpackhi_epi32(a, b); };
    static __m128i unpacklo(__m128i a, __m128i b) { return _mm_unpacklo_epi32(a, b); };
    static __m128i desinterleave(__m128i a) { return _mm_shuffle_epi32(a, 0b11011000); };

    static __m256i unpackhi(__m256i a, __m256i b) { return _mm256_unpackhi_epi32(a, b); };
    static __m256i unpacklo(__m256i a, __m256i b) { return _mm256_unpacklo_epi32(a, b); };
    static __m256i desinterleave(__m256i a) { return _mm256_shuffle_epi32(a, 0b11011000); };
  };

  template <>
  struct avx_traits<8> : avx_traits<0>
  {
    static __m128i unpackhi(__m128i a, __m128i b) { return _mm_unpackhi_epi64(a, b); };
    static __m128i unpacklo(__m128i a, __m128i b) { return _mm_unpacklo_epi64(a, b); };
    static __m128i desinterleave(__m128i a) { return a; };

    static __m256i unpackhi(__m256i a, __m256i b) { return _mm256_unpackhi_epi64(a, b); };
    static __m256i unpacklo(__m256i a, __m256i b) { return _mm256_unpacklo_epi64(a, b); };
    static __m256i desinterleave(__m256i a) { return a; };
  };

  template <int N>
  struct best_simd_vector
  {
    using type = __m128i;
  };


#if XSIMD_X86_INSTR_SET >= XSIMD_X86_AVX2_VERSION
  template <>
  struct best_simd_vector<32>
  {
    using type = __m256i;
  };
#endif

  template <int nbytes>
  using best_simd_vector_t = typename best_simd_vector<nbytes>::type;


  /// Terminal case with AVX/AVX2 support
  /// a₁ b₁ a₂ b₂ a₃ b₃ a₄ b₄
  /// c₁ d₁ c₂ d₂ c₃ d₃ c₄ d₄
  /// becomes
  /// a₁ c₁ a₂ c₂ a₃ c₃ a₄ c₄
  /// b₁ d₁ b₂ d₂ b₃ d₃ b₄ d₄
  template <class T, int N, int W>
  requires((W * sizeof(T) == 1) || (W * sizeof(T) == 2) || (W * sizeof(T) == 4) || (W * sizeof(T) == 8)) &&
      (N * sizeof(T) >= 16) //
      struct Transposer2x2<T, N, W>
  {
    [[gnu::noinline]] void operator()(std::byte* src, std::ptrdiff_t byte_stride) const noexcept
    {
      using simd_t                 = best_simd_vector_t<(N * sizeof(T) >= 32) ? 32 : 16>;
      constexpr int WARP_SIZE      = sizeof(simd_t) / sizeof(T);
      constexpr int WARP_PER_BLOCK = N / WARP_SIZE;

      using op_t = avx_traits<sizeof(T) * W>;

      for (int by = 0; by < N; by += 2 * W)
      {
        std::byte* block_ptr = src + by * byte_stride;

        for (int y = 0; y < W; ++y)
        {
          simd_t* l0 = (simd_t*)(block_ptr + y * byte_stride);
          simd_t* l1 = (simd_t*)(block_ptr + (W + y) * byte_stride);
          for (int wid = 0; wid < WARP_PER_BLOCK; wid++)
          {
            simd_t r1 = op_t::load(l0 + wid);
            simd_t r2 = op_t::load(l1 + wid);

            r1 = op_t::desinterleave(r1);
            r2 = op_t::desinterleave(r2);


            simd_t new_r1 = op_t::unpacklo(r1, r2);
            simd_t new_r2 = op_t::unpackhi(r1, r2);
            op_t::store(l0 + wid, new_r1);
            op_t::store(l1 + wid, new_r2);
          }
        }
      }
    }
  };

#endif // AVX Support


  template <class T, int N, int W>
  struct Transposer2x2
  {
    [[gnu::noinline]] void operator()(std::byte* src, std::ptrdiff_t byte_stride) const noexcept
    {
      alignas(32) T tmp[W];
      for (int by = 0; by < N; by += 2 * W)
      {
        T* block_A = (T*)(src + by * byte_stride) + W;
        T* block_B = (T*)(src + (by + W) * byte_stride);

        for (int bx = 0; bx < N; bx += 2 * W)
          ano::bp::swap<W, W>(block_A + bx, block_B + bx, tmp, W, W, byte_stride, byte_stride);
      }
    }
  };

  template <class T, int N, int W>
  void transposeImpl(T* src, std::ptrdiff_t byte_stride)
  {
    Transposer2x2<T, N, W> t;
    t((std::byte*)src, byte_stride);

    if constexpr (W > 1)
      transposeImpl<T, N, W / 2>(src, byte_stride);
  }


  // Use block of 512 bits by default
  template <class T, int BLOCK_SIZE = 64 / sizeof(T)>
  void transpose_inplaceT(T* src, std::size_t n, std::ptrdiff_t stride) noexcept
  {
    // Check that we are on 32 bytes boundaries
    if (n >= BLOCK_SIZE)
    {
      ano_assume(((uintptr_t)src & 0x0F) == 0 && "16-bytes alignment chech failed");
      ano_assume((stride & 0x0F) == 0 && "16-bytes alignment check failed");
    }

    alignas(32) T tmp[BLOCK_SIZE];

    std::size_t y = 0;
    for (y = 0; y + BLOCK_SIZE <= n; y += BLOCK_SIZE)
    {
      T* lineptr = (T*)((std::byte*)src + y * stride);
      for (std::size_t x = 0; x + BLOCK_SIZE <= n; x += BLOCK_SIZE)
      {
        transposeImpl<T, BLOCK_SIZE, BLOCK_SIZE / 2>(lineptr + x, stride);

        if (y > x)
          ano::bp::swap<BLOCK_SIZE, BLOCK_SIZE>(ano::bp::ptr_offset(src, y * stride) + x, //
                                                ano::bp::ptr_offset(src, x * stride) + y, //
                                                tmp, BLOCK_SIZE, BLOCK_SIZE, stride, stride);
      }
    }
    // Remaining rows
    if (y < n)
    {
      std::size_t rem = n - y;

      for (std::size_t x = 0; x + BLOCK_SIZE <= n; x += BLOCK_SIZE)
        ano::bp::impl::transpose_swap_naive(ano::bp::ptr_offset(src, y * stride) + x, //
                                            ano::bp::ptr_offset(src, x * stride) + y, //
                                            BLOCK_SIZE, rem, stride, stride);

      ano::bp::impl::transpose_inplace_naive(ano::bp::ptr_offset(src, y * stride) + y, rem, stride);
    }
  }


  template <class T>
  void transposeT(const T* __restrict src, T* __restrict dst, int width, int height, std::ptrdiff_t src_stride,
                  std::ptrdiff_t dst_stride) noexcept
  {
    // Use block of 512 bits by default
    constexpr int BLOCK_SIZE = 64 / sizeof(T);

    // Check that we are on 32 bytes boundaries
    // ano_assume(((uintptr_t)src & 0x1F) == 0 && "32-bytes alignment check failed");
    // ano_assume(((uintptr_t)dst & 0x1F) == 0 && "32-bytes alignment check failed");
    // ano_assume((src_stride & 0x1F) == 0 && "32-bytes alignment check failed");
    // ano_assume((dst_stride & 0x1F) == 0 && "32-bytes alignment check failed");

    int xrem = width % BLOCK_SIZE;
    int yrem = height % BLOCK_SIZE;


    int y = 0;
    for (y = 0; y + BLOCK_SIZE <= height; y += BLOCK_SIZE)
    {
      int x;
      for (x = 0; x + BLOCK_SIZE <= width; x += BLOCK_SIZE)
      {
        T*       dst_block = ano::bp::ptr_offset(dst, y * dst_stride) + x;
        const T* src_block = ano::bp::ptr_offset(src, x * src_stride) + y;
        ano::bp::copy<BLOCK_SIZE, BLOCK_SIZE>(src_block, dst_block, BLOCK_SIZE, BLOCK_SIZE, src_stride, dst_stride);
        transposeImpl<T, BLOCK_SIZE, BLOCK_SIZE / 2>(dst_block, dst_stride);
      }

      if (xrem > 0)
      {
        T*       dst_block = ano::bp::ptr_offset(dst, y * dst_stride) + x;
        const T* src_block = ano::bp::ptr_offset(src, x * src_stride) + y;
        ano::bp::impl::transpose_naive(src_block, dst_block, xrem, BLOCK_SIZE, src_stride, dst_stride);
      }
    }
    // Remaining rows
    if (yrem > 0)
    {
      int x;
      for (x = 0; x + BLOCK_SIZE <= width; x += BLOCK_SIZE)
      {
        T*       dst_block = ano::bp::ptr_offset(dst, y * dst_stride) + x;
        const T* src_block = ano::bp::ptr_offset(src, x * src_stride) + y;
        ano::bp::impl::transpose_naive(src_block, dst_block, BLOCK_SIZE, yrem, src_stride, dst_stride);
      }
      if (xrem > 0)
      {
        T*       dst_block = ano::bp::ptr_offset(dst, y * dst_stride) + x;
        const T* src_block = ano::bp::ptr_offset(src, x * src_stride) + y;
        ano::bp::impl::transpose_naive(src_block, dst_block, xrem, yrem, src_stride, dst_stride);
      }
    }
  }


  template <class T>
  void transpose_inplaceT(T* src, std::size_t n, std::ptrdiff_t stride, int block_size)
  {
    switch (block_size)
    {
    case 2:
      transpose_inplaceT<T, 2>(src, n, stride);
      return;
    case 4:
      transpose_inplaceT<T, 4>(src, n, stride);
      return;
    case 8:
      transpose_inplaceT<T, 8>(src, n, stride);
      return;
    case 16:
      transpose_inplaceT<T, 16>(src, n, stride);
      return;
    case 32:
      transpose_inplaceT<T, 32>(src, n, stride);
      return;
    case 64:
      transpose_inplaceT<T, 64>(src, n, stride);
      return;
    default:
      throw std::runtime_error("Invalid block size. Must be a pow of 2 between 2 and 64");
    }
  }
} // namespace


namespace ano::bp
{

#if XSIMD_X86_INSTR_SET >= XSIMD_X86_AVX_VERSION

  void transpose_inplace(uint8_t* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint8_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(int8_t* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint8_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(uint16_t* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint16_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(int16_t* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint16_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(uint32_t* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint32_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(int32_t* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint32_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(float* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint32_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(uint64_t* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint64_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(int64_t* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint64_t*)buffer, n, stride, bs);
  }
  void transpose_inplace(double* buffer, int n, std::ptrdiff_t stride, int bs)
  {
    transpose_inplaceT((uint64_t*)buffer, n, stride, bs);
  }


  void transpose_inplace(uint8_t* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint8_t*)buffer, n, stride);
  }
  void transpose_inplace(int8_t* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint8_t*)buffer, n, stride);
  }
  void transpose_inplace(uint16_t* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint16_t*)buffer, n, stride);
  }
  void transpose_inplace(int16_t* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint16_t*)buffer, n, stride);
  }
  void transpose_inplace(uint32_t* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(float* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(int32_t* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(uint64_t* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint64_t*)buffer, n, stride);
  }
  void transpose_inplace(int64_t* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint64_t*)buffer, n, stride);
  }
  void transpose_inplace(double* buffer, int n, std::ptrdiff_t stride) noexcept
  {
    transpose_inplaceT((uint64_t*)buffer, n, stride);
  }

  void transpose(const uint8_t* src, uint8_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint8_t*)src, (uint8_t*)dst, w, h, s1, s2);
  }
  void transpose(const int8_t* src, int8_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint8_t*)src, (uint8_t*)dst, w, h, s1, s2);
  }
  void transpose(const uint16_t* src, uint16_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint16_t*)src, (uint16_t*)dst, w, h, s1, s2);
  }
  void transpose(const int16_t* src, int16_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint16_t*)src, (uint16_t*)dst, w, h, s1, s2);
  }
  void transpose(const uint32_t* src, uint32_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint32_t*)src, (uint32_t*)dst, w, h, s1, s2);
  }
  void transpose(const int32_t* src, int32_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint32_t*)src, (uint32_t*)dst, w, h, s1, s2);
  }
  void transpose(const float* src, float* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint32_t*)src, (uint32_t*)dst, w, h, s1, s2);
  }
  void transpose(const uint64_t* src, uint64_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint64_t*)src, (uint64_t*)dst, w, h, s1, s2);
  }
  void transpose(const int64_t* src, int64_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint64_t*)src, (uint64_t*)dst, w, h, s1, s2);
  }
  void transpose(const double* src, double* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    transposeT((const uint64_t*)src, (uint64_t*)dst, w, h, s1, s2);
  }

#else

  void transpose_inplace(uint8_t* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint8_t*)buffer, n, stride);
  }
  void transpose_inplace(int8_t* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint8_t*)buffer, n, stride);
  }
  void transpose_inplace(uint16_t* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint16_t*)buffer, n, stride);
  }
  void transpose_inplace(int16_t* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint16_t*)buffer, n, stride);
  }
  void transpose_inplace(uint32_t* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(int32_t* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(float* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(uint64_t* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint64_t*)buffer, n, stride);
  }
  void transpose_inplace(int64_t* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint64_t*)buffer, n, stride);
  }
  void transpose_inplace(double* buffer, int n, ptrdiff_t stride, int)
  {
    impl::transpose_inplace_naive((uint64_t*)buffer, n, stride);
  }


  void transpose_inplace(uint8_t* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint8_t*)buffer, n, stride);
  }
  void transpose_inplace(int8_t* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint8_t*)buffer, n, stride);
  }
  void transpose_inplace(uint16_t* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint16_t*)buffer, n, stride);
  }
  void transpose_inplace(int16_t* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint16_t*)buffer, n, stride);
  }
  void transpose_inplace(uint32_t* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(int32_t* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(float* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint32_t*)buffer, n, stride);
  }
  void transpose_inplace(uint64_t* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint64_t*)buffer, n, stride);
  }
  void transpose_inplace(int64_t* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint64_t*)buffer, n, stride);
  }
  void transpose_inplace(double* buffer, int n, ptrdiff_t stride) noexcept
  {
    impl::transpose_inplace_naive((uint64_t*)buffer, n, stride);
  }


  void transpose(const uint8_t* src, uint8_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint8_t*)src, (uint8_t*)dst, w, h, s1, s2);
  }
  void transpose(const int8_t* src, int8_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint8_t*)src, (uint8_t*)dst, w, h, s1, s2);
  }
  void transpose(const uint16_t* src, uint16_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint16_t*)src, (uint16_t*)dst, w, h, s1, s2);
  }
  void transpose(const int16_t* src, int16_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint16_t*)src, (uint16_t*)dst, w, h, s1, s2);
  }
  void transpose(const uint32_t* src, uint32_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint32_t*)src, (uint32_t*)dst, w, h, s1, s2);
  }
  void transpose(const int32_t* src, int32_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint32_t*)src, (uint32_t*)dst, w, h, s1, s2);
  }
  void transpose(const float* src, float* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint32_t*)src, (uint32_t*)dst, w, h, s1, s2);
  }
  void transpose(const uint64_t* src, uint64_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint64_t*)src, (uint64_t*)dst, w, h, s1, s2);
  }
  void transpose(const int64_t* src, int64_t* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint64_t*)src, (uint64_t*)dst, w, h, s1, s2);
  }
  void transpose(const double* src, double* dst, int w, int h, std::ptrdiff_t s1, std::ptrdiff_t s2) noexcept
  {
    impl::transpose_naive((const uint64_t*)src, (uint64_t*)dst, w, h, s1, s2);
  }


#endif
} // namespace ano::bp
