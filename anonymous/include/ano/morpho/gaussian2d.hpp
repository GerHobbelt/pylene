#pragma once


#include <ano/core/box.hpp>
#include <ano/core/image/ndimage.hpp>
#include <ano/core/trace.hpp>
#include <ano/morpho/private/tile_tools.hpp>


/// Perform a 2D gaussian filter on a 2D image
///
/// \param h_sigma Horizontal stddev of the filter (0 to disable horizontal filtering)
/// \param v_sigma Vertical stddev of the filter (0 to disable vertival filtering)

namespace ano::morpho
{

  template <class InputImage>
  ano::image2d<float> gaussian2d(InputImage input, float h_sigma, float v_sigma);

  template <class InputImage>
  void gaussian2d(InputImage input, float h_sigma, float v_sigma, ano::image2d<float>& out);

  namespace details
  {

    class vgaussian2d_algo_t
    {
    private:
      int         get_max_tile_width() const;
      int         get_max_tile_height() const;
      std::size_t get_sample_size() const;

      TileLoaderBase* m_tile_loader_1 = nullptr;
      TileWriterBase* m_tile_writer_1 = nullptr;
      TileLoaderBase* m_tile_loader_2 = nullptr;
      TileWriterBase* m_tile_writer_2 = nullptr;


    public:
      vgaussian2d_algo_t();

      // Apply the running max algorithm over a roi using tiling
      void execute(ano::box2d roi, float h_sigma, float v_sigma);

      void set_tile_reader_1(TileLoaderBase* r) { m_tile_loader_1 = r; }
      void set_tile_writer_1(TileWriterBase* w) { m_tile_writer_1 = w; }
      void set_tile_reader_2(TileLoaderBase* r) { m_tile_loader_2 = r; }
      void set_tile_writer_2(TileWriterBase* w) { m_tile_writer_2 = w; }
    };

  } // namespace details

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  /*

    template <class InOutImage, class T>
    void gaussian2d_generic(InOutImage& input, float h_sigma, float v_sigma, T border_value)
    {
      int b      = 5 * static_cast<int>(std::max(h_sigma, v_sigma) + 0.5f);
      int width  = input.width();
      int height = input.height();

      int tmp_size = std::max(width, height) + 2 * b;

      // Allocate temporary buffer
      std::vector<float> tmp(3 * tmp_size);
      float*             i_buffer = tmp.data();
      float*             tmp1     = tmp.data() + tmp_size;
      float*             tmp2     = tmp.data() + 2 * tmp_size;

      // Fill border
      std::fill_n(i_buffer, tmp_size, border_value);

      // horizontal pass
      const int x0 = input.domain().x();
      if (v_sigma != 0.f)
      {
        recursivefilter_coef_ coef(1.68f, 3.735f, 1.783f, 1.723f, -0.6803f, -0.2598f, 0.6318f, 1.997f, v_sigma,
                                   recursivefilter_coef_::DericheGaussian);


        int size = height + 2 * b;
        for (int x = 0; x < width; ++x)
        {
          copy_from_column(input, x + x0, i_buffer + b);
          gaussian1d(coef, i_buffer, size, tmp1, tmp2);
          copy_to_column(i_buffer + b, x + x0, input);
        }
      }

      // vertical pass
      if (h_sigma != 0.f)
      {
        recursivefilter_coef_ coef(1.68f, 3.735f, 1.783f, 1.723f, -0.6803f, -0.2598f, 0.6318f, 1.997f, h_sigma,
                                   recursivefilter_coef_::DericheGaussian);

        int   size    = width + 2 * b;
        auto* lineptr = input.buffer();
        for (int y = 0; y < height; ++y)
        {
          std::copy_n(lineptr, width, i_buffer + b);
          gaussian1d(coef, i_buffer, size, tmp1, tmp2);
          std::copy_n(i_buffer + b, width, lineptr);
          lineptr += input.stride();
        }
      }
    }
  } // namespace details

  */


  template <class InputImage>
  ano::image2d<float> gaussian2d(InputImage input, float h_sigma, float v_sigma)
  {
    ano::box2d          domain = input.domain();
    ano::image2d<float> out(domain);

    gaussian2d(input, h_sigma, v_sigma, out);
    return out;
  }

  template <class InputImage>
  void gaussian2d(InputImage input, float h_sigma, float v_sigma, ano::image2d<float>& out)
  {
    static_assert(std::is_convertible_v<ano::image_value_t<InputImage>, float>);

    ano_entering("ano::morpho::gaussian2d");


    details::TileLoader<InputImage, float>          l_in(input, false);
    details::TileLoader<ano::image2d<float>, float> l_out(out, false);
    details::TileWriter<ano::image2d<float>, float> w(out, false);

    ano::box2d domain = input.domain();

    details::vgaussian2d_algo_t algo;

    if (v_sigma == 0.f)
    {
      algo.set_tile_reader_2(&l_in);
      algo.set_tile_writer_2(&w);
    }
    else if (h_sigma == 0.f)
    {
      algo.set_tile_reader_1(&l_in);
      algo.set_tile_writer_1(&w);
    }
    else
    {
      algo.set_tile_reader_1(&l_in);
      algo.set_tile_writer_1(&w);
      algo.set_tile_reader_2(&l_out);
      algo.set_tile_writer_2(&w);
    }
    algo.execute(domain, h_sigma, v_sigma);
  }

} // namespace ano::morpho
