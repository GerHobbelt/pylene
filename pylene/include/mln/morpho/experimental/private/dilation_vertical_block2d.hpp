#pragma once

#include <mln/core/box.hpp>
#include <mln/core/image/ndimage_fwd.hpp>
#include <mln/morpho/experimental/private/tile_tools.hpp>

#include <range/v3/functional/concepts.hpp>

#include <experimental/simd>

#include <cassert>

namespace mln::morpho::experimental::details
{

  template <class T, class I, class J, class BinaryFunction>
  void running_max_2d(I& input, J& output, BinaryFunction sup, mln::box2d roi, int k, bool use_extension,
                      bool vertical);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  class vertical_running_max_algo_base_t
  {
  private:
    // Accumulate the supremum column-wise (eq to the python A.cumsum(axis=0))
    virtual void partial_sum_block2d(const std::byte* __restrict in, std::byte* __restrict out, int width, int height,
                                     std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride) = 0;

    // Apply PW OUT[x] = SUP(A[x], B[x])
    virtual void apply_sup(std::byte* __restrict A, std::byte* __restrict B, std::byte* __restrict OUT, int width) = 0;

    virtual int         get_block_width() const = 0;
    virtual std::size_t get_sample_size() const = 0;

    TileLoaderBase* m_tile_loader = nullptr;
    TileWriterBase* m_tile_writer = nullptr;


  public:
    // Apply the running max algorithm over a block
    // Memory has already been allocated
    void running_max_block2d(std::byte* f, std::byte* g, std::byte* h, std::ptrdiff_t f_byte_stride,
                             std::ptrdiff_t g_byte_stride, std::ptrdiff_t h_byte_stride, mln::box2d roi, int k,
                             bool use_extension);

    // Apply the running max algorithm over a roi using tiling
    void execute(mln::box2d roi, int k, bool use_extension, bool vertical = true);

    void set_tile_reader(TileLoaderBase* r) { m_tile_loader = r; }
    void set_tile_writer(TileWriterBase* w) { m_tile_writer = w; }
  };


  template <class T, class BinaryFunction>
  class vertical_running_max_algo_t : public vertical_running_max_algo_base_t
  {
    using simd_t                     = std::experimental::simd<T>;
    static constexpr int WARP_SIZE   = simd_t::size();
    static constexpr int BLOCK_WIDTH = WARP_SIZE * 4;

    static_assert(::ranges::regular_invocable<BinaryFunction, simd_t, simd_t>);
    static_assert(::ranges::regular_invocable<BinaryFunction, T, T>);

    void apply_sup(std::byte* __restrict A, std::byte* __restrict B, std::byte* __restrict OUT, int n) final;
    void partial_sum_block2d(const std::byte* __restrict in, std::byte* __restrict out, int width, int height,
                             std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride) final;

    int         get_block_width() const final { return BLOCK_WIDTH; }
    std::size_t get_sample_size() const final { return sizeof(T); }


  public:
    vertical_running_max_algo_t(BinaryFunction sup)
      : m_sup{std::move(sup)}
    {
    }

  protected:
    BinaryFunction m_sup;
  };


  template <class T, class BinaryFunction>
  void vertical_running_max_algo_t<T, BinaryFunction>::apply_sup(std::byte* __restrict A_, std::byte* __restrict B_,
                                                                 std::byte* __restrict out_, int width)
  {
    const T* A   = (T*)A_;
    const T* B   = (T*)B_;
    T*       out = (T*)out_;

    const int K   = width / WARP_SIZE;
    const int rem = width % WARP_SIZE;

    assert(width <= BLOCK_WIDTH);
    for (int k = 0; k < K; k++)
    {
      simd_t a;
      simd_t b;
      simd_t c;
      a.copy_from(A + k * WARP_SIZE, std::experimental::element_aligned);
      b.copy_from(B + k * WARP_SIZE, std::experimental::element_aligned);
      c = m_sup(a, b);
      c.copy_to(out + k * WARP_SIZE, std::experimental::element_aligned);
    }

    if (rem > 0)
    {
      A += K * WARP_SIZE;
      B += K * WARP_SIZE;
      out += K * WARP_SIZE;
      std::transform(A, A + rem, B, out, m_sup);
    }
  }


  template <class T, class BinaryFunction>
  void vertical_running_max_algo_t<T, BinaryFunction>::partial_sum_block2d(const std::byte* __restrict in,
                                                                           std::byte* __restrict out, int width,
                                                                           int height, std::ptrdiff_t in_byte_stride,
                                                                           std::ptrdiff_t out_byte_stride)
  {
    using simd_t = std::experimental::simd<T>;


    constexpr int MAX_WARP_COUNT = BLOCK_WIDTH / WARP_SIZE;
    const int     K              = width / WARP_SIZE;
    const int     rem            = width % WARP_SIZE;

    // fmt::print("BLOCK_WIDTH={} width={}\n", BLOCK_WIDTH, width);
    // fmt::print("WARP_SIZE={}\n", WARP_SIZE);

    assert(width <= BLOCK_WIDTH);
    std::memcpy(out, in, sizeof(T) * width);

    // By block
    if (K > 0)
    {

      simd_t xsum[MAX_WARP_COUNT];

      for (int k = 0; k < K; k++)
        xsum[k].copy_from((T*)in + k * WARP_SIZE, std::experimental::element_aligned);

      // Next lines
      for (int y = 1; y < height; ++y)
      {
        const T* in_lineptr  = (const T*)(in + y * in_byte_stride);
        T*       out_lineptr = (T*)(out + y * out_byte_stride);

        for (int k = 0; k < K; k++)
        {
          simd_t v;
          v.copy_from(in_lineptr + k * WARP_SIZE, std::experimental::element_aligned);
          xsum[k] = m_sup(xsum[k], v);
          xsum[k].copy_to(out_lineptr + k * WARP_SIZE, std::experimental::element_aligned);
        }
      }
    }

    if (rem > 0)
    {
      in += K * WARP_SIZE * sizeof(T);
      out += K * WARP_SIZE * sizeof(T);

      T xsum[WARP_SIZE];
      std::memcpy(xsum, in, sizeof(T) * rem);
      for (int y = 1; y < height; ++y)
      {
        const T* in_lineptr  = (const T*)(in + y * in_byte_stride);
        T*       out_lineptr = (T*)(out + y * out_byte_stride);

        for (int c = 0; c < rem; ++c)
        {
          xsum[c]        = m_sup(xsum[c], in_lineptr[c]);
          out_lineptr[c] = xsum[c];
        }
      }
    }
  }

  template <class T, class I, class J, class BinaryFunction>
  void running_max_2d(I& input, J& output, BinaryFunction sup, mln::box2d roi, int k, bool use_extension, bool vertical)
  {
    TileLoader<I, T> r(input, vertical);
    TileWriter<J, T> w(output, vertical);

    vertical_running_max_algo_t<T, BinaryFunction> alg(sup);
    alg.set_tile_reader(&r);
    alg.set_tile_writer(&w);
    alg.execute(roi, k, use_extension, vertical);
  }

} // namespace mln::morpho::experimental::details
