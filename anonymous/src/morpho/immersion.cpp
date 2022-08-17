#include <ano/morpho/private/immersion.spe.hpp>

namespace ano::morpho::details
{
  namespace
  {
    void immersion_image2d(std::byte*                   i_buffer,   //
                           std::byte*                   inf_buffer, //
                           std::byte*                   sup_buffer, //
                           std::size_t                  width,      //
                           std::size_t                  height,     //
                           std::ptrdiff_t               i_stride,   //
                           std::ptrdiff_t               inf_stride, //
                           std::ptrdiff_t               sup_stride, //
                           immersion_impl_table_base_t* impl)
    {
      impl->immersion(i_buffer, width, inf_buffer, sup_buffer);
      for (std::size_t y = 1; y < height; ++y)
      {
        impl->immersion(i_buffer + y * i_stride, width, inf_buffer + 2 * y * inf_stride,
                        sup_buffer + 2 * y * sup_stride);
        impl->interpolation_min(inf_buffer + (2 * (y - 1)) * inf_stride, inf_buffer + (2 * y) * inf_stride,
                                2 * width - 1, inf_buffer + (2 * y - 1) * inf_stride);
        impl->interpolation_max(sup_buffer + (2 * (y - 1)) * sup_stride, sup_buffer + (2 * y) * sup_stride,
                                2 * width - 1, sup_buffer + (2 * y - 1) * sup_stride);
      }
    }

    void interpolate_image2d_min(std::byte* buffer, std::size_t width, std::size_t height, std::ptrdiff_t line_stride,
                                 std::ptrdiff_t slice_stride, immersion_impl_table_base_t* impl)
    {
      std::byte* prev = buffer - slice_stride;
      std::byte* next = buffer + slice_stride;

      for (std::size_t y = 0; y < height; ++y)
        impl->interpolation_min(prev + y * line_stride, next + y * line_stride, width, buffer + y * line_stride);
    }

    void interpolate_image2d_max(std::byte* buffer, std::size_t width, std::size_t height, std::ptrdiff_t line_stride,
                                 std::ptrdiff_t slice_stride, immersion_impl_table_base_t* impl)
    {
      std::byte* prev = buffer - slice_stride;
      std::byte* next = buffer + slice_stride;

      for (std::size_t y = 0; y < height; ++y)
        impl->interpolation_max(prev + y * line_stride, next + y * line_stride, width, buffer + y * line_stride);
    }

  } // namespace

  void immersion_ndimage(ndbuffer_image& input, ndbuffer_image& inf, ndbuffer_image& sup,
                         immersion_impl_table_base_t* impl)
  {
    [[maybe_unused]] int pdim = input.pdim();
    assert(pdim == 2 || pdim == 3);
    assert(input.pdim() == pdim);
    assert(inf.pdim() == pdim);
    assert(sup.pdim() == pdim);

    const int depth  = input.depth();
    const int height = input.height();
    const int width  = input.width();

    std::byte* i_buffer   = input.buffer();
    std::byte* inf_buffer = inf.buffer();
    std::byte* sup_buffer = sup.buffer();

    const std::ptrdiff_t i_stride   = input.byte_stride();
    const std::ptrdiff_t inf_stride = inf.byte_stride();
    const std::ptrdiff_t sup_stride = sup.byte_stride();


    immersion_image2d(i_buffer, inf_buffer, sup_buffer, width, height, i_stride, inf_stride, sup_stride, impl);
    for (int z = 1; z < depth; ++z)
    {
      immersion_image2d(i_buffer + z * input.byte_stride(2),       //
                        inf_buffer + (2 * z) * inf.byte_stride(2), //
                        sup_buffer + (2 * z) * sup.byte_stride(2), //
                        width, height, i_stride, inf_stride, sup_stride, impl);

      interpolate_image2d_min(inf_buffer + (2 * z - 1) * inf.byte_stride(2), 2 * width - 1, 2 * height - 1, inf_stride,
                              inf.byte_stride(2), impl);
      interpolate_image2d_max(sup_buffer + (2 * z - 1) * sup.byte_stride(2), 2 * width - 1, 2 * height - 1, sup_stride,
                              sup.byte_stride(2), impl);
    }
  }

} // namespace ano::morpho::details
