#pragma once

#include <mln/core/algorithm/paste.hpp>
#include <mln/core/box.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/value/value_traits.hpp>

namespace mln
{
  namespace details
  {
    template <class I, class T>
    [[gnu::noinline]] void copy_block(I& in, mln::box2d roi, T* __restrict out, std::ptrdiff_t out_stride)
    {
      const int x0 = roi.x();
      const int y0 = roi.y();

      for (int y = 0; y < roi.height(); ++y)
      {
        T* lineptr = out + y * out_stride;
        for (int x = 0; x < roi.width(); ++x)
          lineptr[x] = in.at({x0 + x, y0 + y});
      }
    }

    template <class I, class T>
    [[gnu::noinline]] void copy_block(T* __restrict in, std::ptrdiff_t istride, mln::box2d roi, I& out)
    {
      const int x0 = roi.x();
      const int y0 = roi.y();

      for (int y = 0; y < roi.height(); ++y)
      {
        const T* lineptr = in + y * istride;
        for (int x = 0; x < roi.width(); ++x)
          out.at({x0 + x, y0 + y}) = lineptr[x];
      }
    }

    template <class I, class T>
    [[gnu::noinline]] void transpose_block2D(I& in, mln::box2d input_roi, T* __restrict out, std::ptrdiff_t out_stride)
    {
      const int x0 = input_roi.x();
      const int y0 = input_roi.y();


      for (int y = 0; y < input_roi.height(); ++y)
        for (int x = 0; x < input_roi.width(); ++x)
          *(out + x * out_stride + y) = in.at({x0 + x, y0 + y});
    }

    template <class I, class T>
    [[gnu::noinline]] void transpose_block2D(T* __restrict in, std::ptrdiff_t istride, mln::box2d output_roi, I& out)
    {
      const int x0 = output_roi.x();
      const int y0 = output_roi.y();

      for (int y = 0; y < output_roi.height(); ++y)
        for (int x = 0; x < output_roi.width(); ++x)
          out.at({x0 + x, y0 + y}) = *(in + x * istride + y);
    }

    /// Used in mln::morpho::parallel::dilation
    template <class InputImage, class SE, class D>
    auto create_temporary_image(InputImage&& ima, const SE& se, const D& roi)
    {
      using I = ::concepts::remove_cvref_t<InputImage>;

      static_assert(mln::is_a<I, mln::details::Image>());
      static_assert(mln::is_a<SE, mln::details::StructuringElement>());

      D input_roi = se.compute_input_region(ima.domain());

      image_build_params params;
      params.init_value = mln::value_traits<image_value_t<I>>::inf();

      image_concrete_t<I> output(input_roi, params);
      mln::paste(ima, roi, output);
      return output;
    }

  } // namespace details

  /*TODO
  Erosion
  Dilation
  Opening (Erosion -> Dilation)
  Closing (Dilation -> Erosion)
  Hit Or Miss
  Rank Filter
  Median filter
  Mean (Box) filter
  Morphological gradients
  Top hat
  */

  class TileExecutorBase
  {
  public:
    virtual void execute(mln::ndbuffer_image input, mln::ndbuffer_image output) const = 0;
  };

  class TileLoaderBase
  {
  public:
    virtual void                load_tile(mln::box2d roi, mln::box2d input_roi) const = 0;
    virtual mln::ndbuffer_image get_tile() const                                      = 0;
  };

  class TileWriterBase
  {
  public:
    virtual void                write_tile(mln::box2d roi) const = 0;
    virtual mln::ndbuffer_image get_tile(mln::box2d roi) const   = 0;
  };


  class ParallelLocalCanvas2D
  {
  public:
    static constexpr int TILE_WIDTH  = 8;
    static constexpr int TILE_HEIGHT = 8;

    virtual ~ParallelLocalCanvas2D() = default;
    virtual std::unique_ptr<ParallelLocalCanvas2D> clone() const = 0;

    // Compute the input region required to compute the given output region
    virtual mln::box2d              ComputeInputRegion(mln::box2d out_roi) const noexcept = 0;
    virtual const TileLoaderBase*   GetTileLoader() const noexcept                = 0;
    virtual const TileWriterBase*   GetTileWriter() const noexcept                = 0;
    virtual const TileExecutorBase* GetTileExecutor() const noexcept              = 0;

    // The whole output roi we want to compute
    virtual mln::box2d GetOutputRegion() const = 0;

    // Execute to compute this output roi
    virtual void       ExecuteTile(mln::box2d out_roi) const final;
  };

  void parallel_execute_local2D(ParallelLocalCanvas2D& canvas);
} // namespace mln
  /*
  namespace mln::morpho::experimental::details
  {
  
    class vertical_running_max_algo_base_t
    {
    private:
      // Accumulate the supremum column-wise (eq to the python A.cumsum(axis=0))
      virtual void partial_sum_block2D(const std::byte* __restrict in, std::byte* __restrict out, int width, int height,
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
      void running_max_block2D(std::byte* f, std::byte* g, std::byte* h, std::ptrdiff_t f_byte_stride,
                               std::ptrdiff_t g_byte_stride, std::ptrdiff_t h_byte_stride, mln::experimental::box2d roi,
                               int k, bool use_extension);
  
      // Apply the running max algorithm over a roi using tiling
      void execute(mln::experimental::box2d roi, int k, bool use_extension, bool vertical = true);
  
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
      void partial_sum_block2D(const std::byte* __restrict in, std::byte* __restrict out, int width, int height,
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
    void vertical_running_max_algo_t<T, BinaryFunction>::partial_sum_block2D(const std::byte* __restrict in,
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
    void running_max_2D(I& input, J& output, BinaryFunction sup, mln::experimental::box2d roi, int k, bool use_extension,
                        bool vertical)
    {
      TileLoader<I, T> r(input, vertical);
      TileWriter<J, T> w(output, vertical);
  
      vertical_running_max_algo_t<T, BinaryFunction> alg(sup);
      alg.set_tile_reader(&r);
      alg.set_tile_writer(&w);
      alg.execute(roi, k, use_extension, vertical);
    }
  } // namespace mln::morpho::experimental::details
  
  */