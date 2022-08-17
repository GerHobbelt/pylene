#include <ano/io/private/io.hpp>


#include <ano/core/canvas/private/traverse2d.hpp>
#include <ano/core/image/ndimage.hpp>
#include <ano/io/private/plugin.hpp>

#include <fmt/core.h>
#include <functional>


namespace ano::io::internal
{

  void load(plugin_reader* p, const char* filename, ano::ndbuffer_image& output)
  {
    // Read header
    p->open(filename);

    // Allocate image
    int            pdim = p->get_ndim();
    sample_type_id tid  = p->get_sample_type_id();

    if (output.sample_type() != sample_type_id::OTHER && output.sample_type() != tid)
      throw std::runtime_error(
          fmt::format("Sample type mismatch. The input is {} (requested: {}).", (int)tid, (int)output.sample_type()));

    if (output.pdim() != 0 && output.pdim() != pdim)
      throw std::runtime_error(fmt::format("Number of dimensions mismath (={}) (should be {}).", output.pdim(), pdim));

    output.resize(tid, pdim, p->get_dim_array(), image_build_params{});

    // Fill content (up to 4 dimensions for now)
    ano::canvas::details::apply_line(output, std::bind(&plugin_reader::read_next_line, p, std::placeholders::_1));

    // Close handle
    p->close();
  }

  void save(const ano::ndbuffer_image& input, plugin_writer* p, const char* filename)
  {
    int ndim = input.pdim();
    int sizes[16];

    for (int k = 0; k < ndim; ++k)
      sizes[k] = input.size(k);

    // Read header
    p->open(filename, input.sample_type(), ndim, sizes);

    // Fill content
    ano::canvas::details::apply_line(const_cast<ano::ndbuffer_image&>(input),
                                     std::bind(&plugin_writer::write_next_line, p, std::placeholders::_1));

    // Close handle
    p->close();
  }


} // namespace ano::io::internal
