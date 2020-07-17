#pragma once

#include <mln/core/algorithm/copy.hpp>
#include <mln/core/box.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/trace.hpp>
#include <mln/morpho/experimental/private/gaussian2d_by_block2d.hpp>
#include <mln/morpho/experimental/private/gaussian2d_coefs.hpp>


#include <algorithm>
#include <cmath>
#include <cstdint>


/// Perform a 2D gaussian filter on a 2D image
///
/// \param h_sigma Horizontal stddev of the filter (0 to disable horizontal filtering)
/// \param v_sigma Vertical stddev of the filter (0 to disable vertival filtering)

namespace mln::morpho::experimental
{

  template <class InputImage, class T>
  image_concrete_t<std::remove_reference_t<InputImage>> gaussian2d(InputImage&& input, float h_sigma, float v_sigma,
                                                                   T border_value);

  template <class InputImage, class T, class OutputImage>
  void gaussian2d(InputImage&& input, float h_sigma, float v_sigma, T border_value, OutputImage&& out);

  namespace details
  {
    template <class T, class InputImage, class OutputImage, class BinaryFunction>
    void running_gaussian2d(InputImage&& input, OutputImage& output, mln::box2d roi, float h_sigma, float v_sigma,
                            T border_value, bool use_extension);
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    template <class T>
    void gaussian1d(recursivefilter_coef_ c, T* input, std::ptrdiff_t size, T* tmp1, T* tmp2)
    {
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
    }


    template <class InputImage, class T>
    void copy_from_column(InputImage&& f, int c, T* buffer)
    {
      int h  = f.height();
      int y0 = f.domain().y();
      for (int y = 0; y < h; ++y)
        buffer[y] = f({c, y + y0});
    }

    template <class OutputImage, class T>
    void copy_to_column(const T* buffer, int c, OutputImage&& f)
    {
      int h  = f.height();
      int y0 = f.domain().y();
      for (int y = 0; y < h; ++y)
        f({c, y + y0}) = buffer[y];
    }


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


  template <class InputImage, class T>
  image_concrete_t<std::remove_reference_t<InputImage>> gaussian2d(InputImage&& input, float h_sigma, float v_sigma,
                                                                   T border_value)
  {
    using I = std::remove_reference_t<InputImage>;

    image_concrete_t<I> out = imconcretize(input);
    gaussian2d(input, h_sigma, v_sigma, border_value, out);
    return out;
  }

  template <class InputImage, class T, class OutputImage>
  void gaussian2d(InputImage&& input, float h_sigma, float v_sigma, T border_value, OutputImage&& out)
  {
    mln_entering("mln::morpho::experimental::gaussian2d");

    mln::copy(input, out);
    details::gaussian2d_generic(out, h_sigma, v_sigma, border_value);
  }

  template <class InputImage, class T, class OutputImage>
  std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>
  gaussian2d(InputImage&& input, float h_sigma, float v_sigma, T border_value, mln::image2d<T>& out)
  {
    auto roi = input.domain();
    mln_entering("Running specialization for gaussian2d over 2d buffer with arithmetic types");
    details::running_gaussian2d<T>(input, out, roi, h_sigma, v_sigma, border_value, /* use_extension = */ true);
  }

  namespace details
  {
    template <class T, class InputImage, class OutputImage, class BinaryFunction>
    void running_gaussian2d(InputImage&& input, OutputImage& output, mln::box2d roi, float h_sigma, float v_sigma,
                            T border_value, bool use_extension)
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

      // vertical pass
      const int x0 = input.domain().x();
      if (v_sigma != 0.f)
      {
        bool vertical = true;

        recursivefilter_coef_ coef(1.68f, 3.735f, 1.783f, 1.723f, -0.6803f, -0.2598f, 0.6318f, 1.997f, v_sigma,
                                   recursivefilter_coef_::DericheGaussian);

        TileLoader<InputImage, T>  r(input, vertical);
        TileWriter<OutputImage, T> w(output, vertical);

        running_gaussian2d_algo_t<T> alg{};
        alg.set_tile_reader(&r);
        alg.set_tile_writer(&w);
        alg.execute(roi, coef, use_extension, vertical);
      }

      // horizontal pass
      if (h_sigma != 0.f)
      {
        bool vertical = false;

        recursivefilter_coef_ coef(1.68f, 3.735f, 1.783f, 1.723f, -0.6803f, -0.2598f, 0.6318f, 1.997f, h_sigma,
                                   recursivefilter_coef_::DericheGaussian);

        TileLoader<InputImage, T>  r(input, vertical);
        TileWriter<OutputImage, T> w(output, vertical);

        running_gaussian2d_algo_t<T> alg{};
        alg.set_tile_reader(&r);
        alg.set_tile_writer(&w);
        alg.execute(roi, coef, use_extension, vertical);
      }
    }
  } // namespace details

} // namespace mln::morpho::experimental
