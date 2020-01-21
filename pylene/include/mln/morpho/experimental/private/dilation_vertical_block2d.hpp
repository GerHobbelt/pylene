#pragma once
#include <experimental/simd>
#include <cassert>
#include <range/v3/functional/concepts.hpp>

#include <mln/core/image/experimental/ndimage_fwd.hpp>
#include <mln/core/box.hpp>

namespace mln::morpho::experimental::details
{

  class vertical_running_max_algo_base_t
  {

    // Accumulate the supremum column-wise (eq to the python A.cumsum(axis=0))
    virtual void partial_sum_block2d(const std::byte* __restrict in, std::byte* __restrict out, int width, int height,
                                     std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride) = 0;


    // Apply PW OUT[x] = SUP(A[x], B[x])
    virtual void apply_sup(std::byte* __restrict A, std::byte* __restrict B, std::byte* __restrict OUT, std::size_t n) = 0;

    virtual int         get_block_width() const = 0;
    virtual std::size_t get_sample_size() const = 0;

  public:
    // Apply the running max algorithm over a block
    // Memory has already been allocated
    void running_max_block2d(std::byte* f, std::byte* g, std::byte* h, int width, int height, std::ptrdiff_t f_byte_stride,
                             std::ptrdiff_t g_byte_stride, std::ptrdiff_t h_byte_stride, int k, bool use_extension);


    // Running the 2D dilation vertically using tiling
    template <class I, class T>
    void running_max_v2d(I& in, mln::experimental::image2d<T>& out, mln::experimental::box2d roi, int k, bool use_extension);

    // Running the 2D dilation vertically using tiling
    template <class I, class T>
    void running_max_h2d(I& in, mln::experimental::image2d<T>& out, mln::experimental::box2d roi, int k, bool use_extension);
  };


  template <class T, class BinaryFunction>
  class vertical_running_max_algo_t : public vertical_running_max_algo_base_t
  {
    using simd_t                     = std::experimental::simd<T>;
    static constexpr int WARP_SIZE   = simd_t::size();
    static constexpr int BLOCK_WIDTH = WARP_SIZE * 4;

    static_assert(::ranges::regular_invocable<BinaryFunction, simd_t, simd_t>);
    static_assert(::ranges::regular_invocable<BinaryFunction, T, T>);

    void apply_sup(std::byte* __restrict A, std::byte* __restrict B, std::byte* __restrict OUT, std::size_t n) final;
    void partial_sum_block2d(const std::byte* __restrict in, std::byte* __restrict out, int width, int height,
                             std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride) final;

    int         get_block_width() const final { return BLOCK_WIDTH; }
    std::size_t get_sample_size() const final { return sizeof(T); }

    BinaryFunction m_sup;

  public:
    vertical_running_max_algo_t(BinaryFunction sup)
      : m_sup{std::move(sup)}
    {
    }
  };

  template <class T, class BinaryFunction>
  void vertical_running_max_algo_t<T, BinaryFunction>::apply_sup(std::byte* __restrict A, std::byte* __restrict B, std::byte* __restrict out, std::size_t n)
  {
    std::transform((T*)A, (T*)A + n, (T*)B, (T*)out, m_sup);
  }


  template <class T, class BinaryFunction>
  void vertical_running_max_algo_t<T, BinaryFunction>::partial_sum_block2d(const std::byte* __restrict in, std::byte* __restrict out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride)
  {
    using simd_t = std::experimental::simd<T>;


    constexpr int MAX_WARP_COUNT  = BLOCK_WIDTH / WARP_SIZE;

    const int K   = width / WARP_SIZE;
    const int rem = width % WARP_SIZE;

    //fmt::print("BLOCK_WIDTH={} width={}\n", BLOCK_WIDTH, width);
    //fmt::print("WARP_SIZE={}\n", WARP_SIZE);

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

      T  xsum[WARP_SIZE];
      std::memcpy(xsum, in, sizeof(T) * rem);
      for (int y = 1; y < height; ++y)
      {
        const T* in_lineptr  = (const T*)(in + y * in_byte_stride);
        T*       out_lineptr = (T*)(out + y * out_byte_stride);

        for (int c = 0; c < rem; ++c)
        {
          xsum[c] = m_sup(xsum[c], in_lineptr[c]);
          out_lineptr[c] = xsum[c];
        }
      }
    }
  }

  template <class I, class T>
  [[gnu::noinline]] void copy_block(I& in, mln::experimental::box2d roi, T* __restrict out, std::ptrdiff_t out_stride)
  {
    const int x0     = roi.tl().x();
    const int y0     = roi.tl().y();

    for (int y = 0; y < roi.height(); ++y)
    {
      T* lineptr = out + y * out_stride;
      for (int x = 0; x < roi.width(); ++x)
        lineptr[x] = in.at({x0 + x, y0 + y});
    }
  }

  template <class I, class T>
  [[gnu::noinline]] void copy_block(T* __restrict in, std::ptrdiff_t istride, mln::experimental::box2d roi, I& out)
  {
    const int x0     = roi.tl().x();
    const int y0     = roi.tl().y();

    for (int y = 0; y < roi.height(); ++y)
    {
      const T* lineptr = in + y * istride;
      for (int x = 0; x < roi.width(); ++x)
        out.at({x0 + x, y0 + y}) = lineptr[x];
    }
  }

  template <class I, class T>
  [[gnu::noinline]] void transpose_block2d(I& in, mln::experimental::box2d input_roi, T* __restrict out, std::ptrdiff_t out_stride)
  {
    const int x0     = input_roi.tl().x();
    const int y0     = input_roi.tl().y();


    for (int y = 0; y < input_roi.height(); ++y)
      for (int x = 0; x < input_roi.width(); ++x)
        *(out + x * out_stride + y) = in.at({x0 + x, y0 + y});
  }

  template <class I, class T>
  [[gnu::noinline]] void transpose_block2d(T* __restrict in, std::ptrdiff_t istride, mln::experimental::box2d output_roi, I& out)
  {
    const int x0     = output_roi.tl().x();
    const int y0     = output_roi.tl().y();

    for (int y = 0; y < output_roi.height(); ++y)
      for (int x = 0; x < output_roi.width(); ++x)
        out.at({x0 + x, y0 + y}) =  *(in + x * istride + y); 
  }




  template <class I, class T>
  void vertical_running_max_algo_base_t::running_max_v2d(I& in, mln::experimental::image2d<T>& out, mln::experimental::box2d roi, int k, bool use_extension)
  {
    int            kBlockWidth    = this->get_block_width();
    auto           sz             = this->get_sample_size();
    std::ptrdiff_t kBlockByteSize = sz * kBlockWidth;

    const int x0     = roi.tl().x();
    const int y0     = roi.tl().y();
    const int y1     = roi.br().y();
    const int width  = roi.width();
    const int height = roi.height();

    std::byte* f = (std::byte*) std::malloc(sz * kBlockWidth * (height + 2 * k));
    std::byte* g = (std::byte*) std::malloc(sz * kBlockWidth * (height + 2 * k));
    std::byte* h = (std::byte*) std::malloc(sz * kBlockWidth * (height + 2 * k));

    for (int x = 0; x < width; x += kBlockWidth)
    {
      int w = std::min(kBlockWidth, width - x);


      // Copy the block
      {
        mln::experimental::box2d region = {{x + x0, y0 - k}, {x + x0 + w, y1 + k}};
        copy_block(in, region, (T*)f, kBlockWidth);
      }

      this->running_max_block2d(f + k * kBlockByteSize, //
                                g + k * kBlockByteSize, //
                                h + k * kBlockByteSize, //
                                w, height, kBlockByteSize, kBlockByteSize, kBlockByteSize, k, use_extension);

      // Copy back
      {
        mln::experimental::box2d region = {{x + x0, y0}, {x + x0 + w, y1}};
        copy_block((T*)f + kBlockWidth * k, kBlockWidth, region, out);
      }
    }

    std::free(f);
    std::free(g);
    std::free(h);
  }


  template <class I, class T>
  void vertical_running_max_algo_base_t::running_max_h2d(I& in, mln::experimental::image2d<T>& out, mln::experimental::box2d roi, int k, bool use_extension)
  {
    int            kBlockWidth    = this->get_block_width();
    auto           sz             = this->get_sample_size();
    std::ptrdiff_t kBlockByteSize = sz * kBlockWidth;

    const int x0     = roi.tl().x();
    const int y0     = roi.tl().y();
    const int x1     = roi.br().x();
    const int width  = roi.width();
    const int height = roi.height();

    std::byte* f = (std::byte*) std::malloc(sz * kBlockWidth * (width + 2 * k));
    std::byte* g = (std::byte*) std::malloc(sz * kBlockWidth * (width + 2 * k));
    std::byte* h = (std::byte*) std::malloc(sz * kBlockWidth * (width + 2 * k));

    for (int y = 0; y < height; y += kBlockWidth)
    {
      int H = std::min(kBlockWidth, height - y);

      // Copy the block
      {
        mln::experimental::box2d region = {{x0 - k, y0 + y}, {x1 + k, y0 + y + H}};
        transpose_block2d(in, region, (T*)f, kBlockWidth);
      }

      this->running_max_block2d(f + k * kBlockByteSize, //
                                g + k * kBlockByteSize, //
                                h + k * kBlockByteSize, //
                                H, width, kBlockByteSize, kBlockByteSize, kBlockByteSize, k, use_extension);

      // Copy back
      {
        mln::experimental::box2d region = {{x0, y0 + y}, {x1, y0 + y + H}};
        transpose_block2d((T*)f + kBlockWidth * k, kBlockWidth, region, out);
      }
    }

    std::free(f);
    std::free(g);
    std::free(h);
  }


} // namespace mln::morpho::details
