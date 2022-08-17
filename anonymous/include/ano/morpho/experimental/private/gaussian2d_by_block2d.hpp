#pragma once

#include <ano/core/box.hpp>
#include <ano/core/image/ndimage_fwd.hpp>
#include <ano/morpho/experimental/private/gaussian2d_coefs.hpp>
#include <ano/morpho/experimental/private/tile_tools.hpp>


#include <cassert>
#include <range/v3/functional/concepts.hpp>

namespace ano::morpho::experimental::details
{

  class running_gaussian2d_algo_base_t
  {
  private:
    virtual void partial_sum_block2d(const std::byte* __restrict in, std::byte* __restrict out, int width, int height,
                                     std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride,
                                     const recursivefilter_coef_& coef) = 0;

    // Apply PW OUT[x] = SUP(A[x], B[x])
    virtual void apply_coef(std::byte* __restrict A, std::byte* __restrict B, std::byte* __restrict OUT, int width,
                            const recursivefilter_coef_& coef) = 0;

    virtual int         get_block_width() const = 0;
    virtual std::size_t get_sample_size() const = 0;

    TileLoaderBase* m_tile_loader = nullptr;
    TileWriterBase* m_tile_writer = nullptr;


  public:
    // Apply the running max algorithm over a block
    // Memory has already been allocated
    void running_gaussian_block2d(std::byte* f, std::byte* g, std::byte* h, std::ptrdiff_t f_byte_stride,
                                  std::ptrdiff_t g_byte_stride, std::ptrdiff_t h_byte_stride, ano::box2d roi,
                                  const recursivefilter_coef_& coef, bool use_extension);

    // Apply the running max algorithm over a roi using tiling
    void execute(ano::box2d roi, const recursivefilter_coef_& coef, bool use_extension, bool vertical = true);

    void set_tile_reader(TileLoaderBase* r) { m_tile_loader = r; }
    void set_tile_writer(TileWriterBase* w) { m_tile_writer = w; }
  };


  class running_gaussian2d_algo_t : public running_gaussian2d_algo_base_t
  {
    using simd_t                     = std::experimental::simd<T>;
    static constexpr int WARP_SIZE   = simd_t::size();
    static constexpr int BLOCK_WIDTH = WARP_SIZE * 4;


    void apply_coef(float* __restrict A, float* __restrict B, std::byte* __restrict OUT, int with,
                    const recursivefilter_coef_& coef) override final;

    void partial_sum_block2d(const std::byte* __restrict in, std::byte* __restrict out, int width, int height,
                             std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride,
                             const recursivefilter_coef_& coef) override final;


    int         get_block_width() const final { return BLOCK_WIDTH; }
    std::size_t get_sample_size() const final { return sizeof(T); }


  public:
    running_gaussian2d_algo_t() = default;
  };


  template <class T>
  void running_gaussian2d_algo_t<T>::apply_coef(std::byte* __restrict A_, std::byte* __restrict B_,
                                                std::byte* __restrict out_, int width,
                                                const recursivefilter_coef_& coef)
  {
    const T* A   = (T*)A_;
    const T* B   = (T*)B_;
    T*       out = (T*)out_;

    const int K   = width / WARP_SIZE;
    const int rem = width % WARP_SIZE;

    assert(width <= BLOCK_WIDTH);
    for (int k = 0; k < K; k++)
    {
      // FIXME : review logic here
      // logic to be applied is the same as the one in gaussian1d
      // The "apply simd sup" operator doesn't exists for gaussian application
      // The simd operator of the coef application on the window needs to be written

      /*
      simd_t a;
      simd_t b;
      simd_t c;
      a.copy_from(A + k * WARP_SIZE, std::experimental::element_aligned);
      b.copy_from(B + k * WARP_SIZE, std::experimental::element_aligned);
      c = m_sup(a, b);
      c.copy_to(out + k * WARP_SIZE, std::experimental::element_aligned);
      */
    }

    if (rem > 0)
    {
      /*
      A += K * WARP_SIZE;
      B += K * WARP_SIZE;
      out += K * WARP_SIZE;
      std::transform(A, A + rem, B, out, m_sup);
      */
    }
  }


  template <class T>
  void running_gaussian2d_algo_t<T>::partial_sum_block2d(const std::byte* __restrict in, //
                                                         std::byte* __restrict out,      //
                                                         int width, int height, std::ptrdiff_t in_byte_stride,
                                                         std::ptrdiff_t               out_byte_stride,
                                                         const recursivefilter_coef_& coef)
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
          // FIXME : review logic here
          // logic to be applied is the same as the one in gaussian1d
          // The "apply simd sup" operator doesn't exists for gaussian application
          // The simd operator of the coef application on the window needs to be written
          /*
          simd_t v;
          v.copy_from(in_lineptr + k * WARP_SIZE, std::experimental::element_aligned);
          xsum[k] = m_sup(xsum[k], v);
          xsum[k].copy_to(out_lineptr + k * WARP_SIZE, std::experimental::element_aligned);
          */
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
          // FIXME : review logic here
          // logic to be applied is the same as the one in gaussian1d
          // The "apply simd sup" operator doesn't exists for gaussian application
          // The simd operator of the coef application on the window needs to be written
          /*
          xsum[c]        = m_sup(xsum[c], in_lineptr[c]);
          out_lineptr[c] = xsum[c];
          */
        }
      }
    }
  }

} // namespace ano::morpho::experimental::details

// Logic to be applied :

/*
      tmp1[0] = c.n[0] * input[0];

      tmp1[1] = 0                   //
                + c.n[0] * input[1] //
                + c.n[1] * input[0] //
                - c.d[0] * tmp1[0];

      tmp1[2] = 0                   //
                + c.n[0] * input[2] //
                + c.n[1] * input[1] //
                + c.n[2] * input[0] //
                - c.d[0] * tmp1[1]  //
                - c.d[1] * tmp1[0];

      tmp1[3] = 0                   //
                + c.n[0] * input[3] //
                + c.n[1] * input[2] //
                + c.n[2] * input[1] //
                + c.n[3] * input[0] //
                - c.d[0] * tmp1[2]  //
                - c.d[1] * tmp1[1]  //
                - c.d[2] * tmp1[0];

      for (int i = 4; i < size; i++)
      {
        tmp1[i] = c.n[0] * input[i + 0] + c.n[1] * input[i - 1] + //
                  c.n[2] * input[i - 2] + c.n[3] * input[i - 3] - //
                  c.d[0] * tmp1[i - 1] - c.d[1] * tmp1[i - 2] -   //
                  c.d[2] * tmp1[i - 3] - c.d[3] * tmp1[i - 4];    //
      }

      // Non causal part

      tmp2[size - 1] = 0;

      tmp2[size - 2] = c.nm[0] * input[size - 1]; //

      tmp2[size - 3] = c.nm[0] * input[size - 2] + //
                       c.nm[1] * input[size - 1] - //
                       c.dm[0] * tmp2[size - 2];   //

      tmp2[size - 4] = c.nm[0] * input[size - 3] + //
                       c.nm[1] * input[size - 2] + //
                       c.nm[2] * input[size - 1] - //
                       c.dm[0] * tmp2[size - 3] -  //
                       c.dm[1] * tmp2[size - 2];   //

      for (int i = size - 5; i >= 0; --i)
      {
        tmp2[i] = c.nm[0] * input[i + 1] +                        //
                  c.nm[1] * input[i + 2] +                        //
                  c.nm[2] * input[i + 3] +                        //
                  c.nm[3] * input[i + 4] -                        //
                  c.dm[0] * tmp2[i + 1] - c.dm[1] * tmp2[i + 2] - //
                  c.dm[2] * tmp2[i + 3] - c.dm[3] * tmp2[i + 4];  //
      }

      for (int i = 0; i < size; ++i)
        input[i] = tmp1[i] + tmp2[i];

*/
