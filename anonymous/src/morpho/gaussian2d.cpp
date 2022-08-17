#include <ano/morpho/gaussian2d.hpp>
#include <ano/morpho/private/gaussian2d_coefs.hpp>
#include <experimental/simd>
#include <memory>

namespace ano::morpho::details
{
  namespace
  {
    constexpr int BLOCK_WIDTH = 32; // (Max) number of elements by block
    using simd_t              = std::experimental::fixed_size_simd<float, BLOCK_WIDTH>;
    // constexpr int WARP_SIZE   = simd_t::size();
    // constexpr int WARP_COUNT  = 4;                      // (Max) Number of warps by block


    void gaussian_vertical_block2d(const recursivefilter_coef_& coef, float* __restrict input, float* __restrict tmp,
                                   std::ptrdiff_t stride, std::ptrdiff_t t_stride, int width, int height)
    {
      recursivefilter_coef_ c = coef;

      assert(width <= BLOCK_WIDTH);

      // A line has (max) WARP_COUNT simd register
      // We cache 4 lines in registers
      simd_t in[4];
      simd_t t[4];
      simd_t sum;

      // Causal part - forward
      {
        // Load the 4 first lines
        for (int i = 0; i < 4; ++i)
          std::memcpy((void*)&in[i], input + stride * i, width * sizeof(float));


        t[0] = c.n[0] * in[0];
        t[1] = 0                //
               + c.n[0] * in[1] //
               + c.n[1] * in[0] //
               - c.d[0] * t[0];

        t[2] = 0                //
               + c.n[0] * in[2] //
               + c.n[1] * in[1] //
               + c.n[2] * in[0] //
               - c.d[0] * t[1]  //
               - c.d[1] * t[0];

        t[3] = 0                //
               + c.n[0] * in[3] //
               + c.n[1] * in[2] //
               + c.n[2] * in[1] //
               + c.n[3] * in[0] //
               - c.d[0] * t[2]  //
               - c.d[1] * t[1]  //
               - c.d[2] * t[0];

        // Commit the 4 first line
        for (int i = 0; i < 4; ++i)
          std::memcpy(tmp + t_stride * i, &t[i], width * sizeof(float));


        for (int i = 4; i < height; i++)
        {
          // Load the line i
          std::memcpy((void*)&in[i % 4], input + stride * i, width * sizeof(float));


          t[i % 4] = c.n[0] * in[(i - 0) % 4] + c.n[1] * in[(i - 1) % 4] + //
                     c.n[2] * in[(i - 2) % 4] + c.n[3] * in[(i - 3) % 4] - //
                     c.d[0] * t[(i - 1) % 4] - c.d[1] * t[(i - 2) % 4] -   //
                     c.d[2] * t[(i - 3) % 4] - c.d[3] * t[(i - 4) % 4];    //

          // Write the line i
          std::memcpy(tmp + t_stride * i, &t[i % 4], width * sizeof(float));
        }
      }

      // Non causal part - backward
      {
        // Load the 4 last lines
        for (int i = 0; i < 4; ++i)
          std::memcpy((void*)&in[i], input + stride * (height - 1 - i), width * sizeof(float));


        t[0] = 0;

        t[1] = c.nm[0] * in[0]; //

        t[2] = c.nm[0] * in[1] + //
               c.nm[1] * in[0] - //
               c.dm[0] * t[1];   //

        t[3] = c.nm[0] * in[2] + //
               c.nm[1] * in[1] + //
               c.nm[2] * in[0] - //
               c.dm[0] * t[2] -  //
               c.dm[1] * t[1];   //


        // Commit the 4 last line
        for (int i = 0; i < 4; ++i)
        {
          std::memcpy((void*)&sum, tmp + t_stride * (height - 1 - i), width * sizeof(float));
          sum += t[i];
          std::memcpy(input + stride * (height - 1 - i), &sum, width * sizeof(float));
        }


        int j = 4;
        for (int i = height - 5; i >= 0; --i, ++j)
        {
          // Load the line i
          std::memcpy((void*)&in[j % 4], input + stride * i, width * sizeof(float));
          std::memcpy((void*)&sum, tmp + t_stride * i, width * sizeof(float));

          t[j % 4] = c.nm[0] * in[(j - 0) % 4] + //
                     c.nm[1] * in[(j - 1) % 4] + //
                     c.nm[2] * in[(j - 2) % 4] + //
                     c.nm[3] * in[(j - 3) % 4] - //
                     c.dm[0] * t[(j - 1) % 4] -  //
                     c.dm[1] * t[(j - 2) % 4] -  //
                     c.dm[2] * t[(j - 3) % 4] -  //
                     c.dm[3] * t[(j - 4) % 4];   //
          sum += t[j % 4];

          // Commit the line i
          std::memcpy(input + stride * i, &sum, width * sizeof(float));
        }
      }
    }
  } // namespace

  vgaussian2d_algo_t::vgaussian2d_algo_t() {}

  int vgaussian2d_algo_t::get_max_tile_width() const
  {
    return BLOCK_WIDTH;
  }


  void vgaussian2d_algo_t::execute(ano::box2d roi, float h_sigma, float v_sigma)
  {
    if (h_sigma == 0.f && v_sigma == 0.f)
      throw std::runtime_error("Sigma parameters cannot be both null");


    int kBlockWidth = this->get_max_tile_width();

    int x0     = roi.x();
    int y0     = roi.y();
    int width  = roi.width();
    int height = roi.height();


    float sigma       = std::max(h_sigma, v_sigma);
    int   tile_border = 5 * static_cast<int>(sigma + 0.5f);
    int   tile_height = std::max(width, height) + 2 * tile_border;
    int   tile_stride = kBlockWidth;

    float* tile_data =
        (float*)std::malloc(tile_height * kBlockWidth * sizeof(float)); // FIXME: think about line memory alignment
    float* aux_data =
        (float*)std::malloc(tile_height * kBlockWidth * sizeof(float)); // FIXME: think about line memory alignment
    float* tile_ptr = tile_data + tile_border * tile_stride;

    // Vertical processing
    if (v_sigma != 0.f)
    {
      recursivefilter_coef_ c(1.68f, 3.735f, 1.783f, 1.723f, -0.6803f, -0.2598f, 0.6318f, 1.997f, v_sigma,
                              recursivefilter_coef_::DericheGaussian);

      for (int x = 0; x < width; x += kBlockWidth)
      {
        int w = std::min(kBlockWidth, width - x);

        ano::box2d region(x0 + x, y0, w, y0 + height);

        this->m_tile_loader_1->copy_block((std::byte*)tile_ptr, tile_stride, region);
        gaussian_vertical_block2d(c, tile_data, aux_data, tile_stride, tile_stride, w, height + 2 * tile_border);
        this->m_tile_writer_1->copy_block((std::byte*)tile_ptr, tile_stride, region);
      }
    }

    // Horizontal processing
    if (h_sigma != 0.f)
    {
      recursivefilter_coef_ c(1.68f, 3.735f, 1.783f, 1.723f, -0.6803f, -0.2598f, 0.6318f, 1.997f, h_sigma,
                              recursivefilter_coef_::DericheGaussian);

      std::swap(x0, y0);
      std::swap(width, height);


      for (int x = 0; x < width; x += kBlockWidth)
      {
        int        w = std::min(kBlockWidth, width - x);
        ano::box2d region(x0 + x, y0, w, y0 + height);

        this->m_tile_loader_2->transpose_block((std::byte*)tile_ptr, tile_stride, region);
        gaussian_vertical_block2d(c, tile_data, aux_data, tile_stride, tile_stride, w, height + 2 * tile_border);
        this->m_tile_writer_2->transpose_block((std::byte*)tile_ptr, tile_stride, region);
      }
    }


    std::free(tile_data);
    std::free(aux_data);
  }

} // namespace ano::morpho::details
