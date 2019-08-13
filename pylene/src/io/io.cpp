#include <mln/io/private/io.hpp>


#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/io/private/plugin.hpp>

#include <fmt/core.h>




namespace mln::io::internal
{

  void load2d(plugin2d_reader* p, const char* filename, mln::ndbuffer_image& output)
  {
    // Read header
    p->open(filename);

    // Allocate image
    int            height = p->height();
    int            width  = p->width();
    sample_type_id tid    = p->get_sample_type_id();

    if (output.sample_type() != sample_type_id::OTHER && output.sample_type() != tid)
      throw std::runtime_error(
        fmt::format("Sample type mismatch. The input is {} (requested: {}).", (int)tid, (int)output.sample_type()));

    if (output.pdim() != 0 && output.pdim() != 2)
      throw std::runtime_error(fmt::format("Number of dimensions mismath (={}) (should be 2).", output.pdim()));

    output.resize(tid, width, height);

    // Fill content
    {
      std::byte*     lineptr = output.buffer();
      std::ptrdiff_t stride  = output.byte_stride();
      for (int y = 0; y < height; ++y)
      {
        p->read_next_line(lineptr);
        lineptr += stride;
      }
    }

    // Close handle
    p->close();
  }


  void save2d(const mln::ndbuffer_image& input, plugin2d_writer* p, const char* filename)
  {
    if (input.pdim() != 2)
      throw std::runtime_error(fmt::format("Invalid number of dimensions (={}) (should be 2).", input.pdim()));

    // Allocate image
    int            height = input.size(1);
    int            width  = input.size(0);
    sample_type_id tid    = input.sample_type();

    // Read header
    p->open(filename, tid, width, height);

    // Fill content
    {
      const std::byte* lineptr = input.buffer();
      std::ptrdiff_t stride  = input.byte_stride();
      for (int y = 0; y < height; ++y)
      {
        p->write_next_line(lineptr);
        lineptr += stride;
      }
    }

    // Close handle
    p->close();
  }


}
