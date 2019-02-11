#include <mln/io/imread.hpp>

#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/io/private/freeimage_plugin.hpp>
#include <mln/io/private/io.hpp>

namespace mln::io
{
  mln::ndbuffer_image imread(const std::string& filename)
  {
    mln::ndbuffer_image out;
    imread(filename, out);
    return out;
  }

  void imread(const std::string& filename, mln::ndbuffer_image& out)
  {
    internal::freeimage_reader_plugin p;
    internal::load(&p, filename.c_str(), out);
  }
} // namespace mln::io
