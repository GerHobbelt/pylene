#include <mln/core/image/ndimage.hpp>

#include <mln/core/range/foreach.hpp>
#include <mln/core/range/rows.hpp>
#include <mln/core/range/view/zip.hpp>




void Mult_Inplace_New_Values(mln::image2d<uint8_t>& img)
{
  mln_foreach (auto& v, img.values())
  {
    v *= 2;
  }
}

void Mult_Inplace_New_Pixels(mln::image2d<uint8_t>& img)
{
  mln_foreach (auto&& px, img.pixels())
  {
    px.val() *= 2;
  }
}

void Mult_Inplace_C(uint8_t* buffer, int width, int height, std::ptrdiff_t stride)
{
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
      buffer[x] *= 2;
    buffer += stride;
  }
}


void Mult_C(const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height,
            std::ptrdiff_t stride)
{
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
      obuffer[x] = ibuffer[x] * 2;

    ibuffer += stride;
    obuffer += stride;
  }
}

void Mult_New_Values(const mln::image2d<uint8_t>& in, mln::image2d<uint8_t>& out)
{
  auto zipped_values = mln::ranges::view::zip(in.values(), out.values());
  for (auto&& r : zipped_values.rows())
  {
    for (auto&& [in_v, out_v] : r)
    {
      out_v = in_v * 2;
    }
  }
}

void Mult_New_Pixels(const mln::image2d<uint8_t>& in, mln::image2d<uint8_t>& out)
{
  auto zipped_pixels = mln::ranges::view::zip(in.pixels(), out.pixels());
  for (auto&& r : zipped_pixels.rows())
  {
    for (auto&& [in_p, out_p] : r)
    {
      out_p.val() = in_p.val() * 2;
    }
  }
}


void Threshold_Inplace_New_Values(mln::image2d<uint8_t>& img)
{
  constexpr uint8_t t = 128;

  mln_foreach (auto& v, img.values())
  {
    v = v < t ? 0 : 255;
  }
}

void Threshold_Inplace_New_Pixels(mln::image2d<uint8_t>& img)
{
  constexpr uint8_t t = 128;

  mln_foreach (auto&& px, img.pixels())
  {
    px.val() = px.val() < t ? 0 : 255;
  }
}

void Threshold_Inplace_C(uint8_t* buffer, int width, int height, std::ptrdiff_t stride)
{
  constexpr uint8_t t = 128;

  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
      buffer[x] = buffer[x] < t ? 0 : 255;
    buffer += stride;
  }
}

void Threshold_C(const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height,
                 std::ptrdiff_t stride)
{
  constexpr uint8_t t = 128;

  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
      obuffer[x] = ibuffer[x] < t ? 0 : 255;
    ibuffer += stride;
    obuffer += stride;
  }
}


void Threshold_New_Values(const mln::image2d<uint8_t>& in, mln::image2d<uint8_t>& out)
{
  constexpr uint8_t t = 128;

  auto zipped_values = mln::ranges::view::zip(in.values(), out.values());
  for (auto&& r : zipped_values.rows())
  {
    for (auto&& [in_v, out_v] : r)
    {
      out_v = in_v < t ? 0 : 255;
    }
  }
}

void Threshold_New_Pixels(const mln::image2d<uint8_t>& in, mln::image2d<uint8_t>& out)
{
  constexpr uint8_t t = 128;

  auto zipped_pixels = mln::ranges::view::zip(in.pixels(), out.pixels());
  for (auto&& r : zipped_pixels.rows())
  {
    for (auto&& [in_p, out_p] : r)
    {
      out_p.val() = in_p.val() < t ? 0 : 255;
    }
  }
}


void LUT_Inplace_New_Values(const uint8_t LUT[], mln::image2d<uint8_t>& img)
{
  mln_foreach (auto& v, img.values())
  {
    v = LUT[v];
  }
}

void LUT_Inplace_New_Pixels(const uint8_t LUT[], mln::image2d<uint8_t>& img)
{
  mln_foreach (auto&& px, img.pixels())
  {
    px.val() = LUT[px.val()];
  }
}

void LUT_Inplace_C(const uint8_t* LUT, uint8_t* buffer, int width, int height, std::ptrdiff_t stride)
{
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
      buffer[x] = LUT[buffer[x]];
    buffer += stride;
  }
}


void LUT_C(const uint8_t* LUT, const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width,
           int height, std::ptrdiff_t stride)
{
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
      obuffer[x] = LUT[ibuffer[x]];
    ibuffer += stride;
    obuffer += stride;
  }
}

void LUT_New_Values(const uint8_t LUT[], const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output)
{
  auto zipped_values = mln::ranges::view::zip(input.values(), output.values());
  for (auto&& r : zipped_values.rows())
  {
    for (auto&& [in_v, out_v] : r)
    {
      out_v = LUT[in_v];
    }
  }
}

void LUT_New_Pixels(const uint8_t LUT[], const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output)
{
  auto zipped_pixels = mln::ranges::view::zip(input.pixels(), output.pixels());
  for (auto&& r : zipped_pixels.rows())
  {
    for (auto&& [in_p, out_p] : r)
    {
      out_p.val() = LUT[in_p.val()];
    }
  }
}
