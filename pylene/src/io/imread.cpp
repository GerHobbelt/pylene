#include <mln/io/experimental/imread.hpp>

#include <mln/io/private/io.hpp>
#include <mln/io/private/freeimage_plugin.hpp>
#include <mln/core/image/experimental/ndbuffer_image.hpp>

namespace mln::io::experimental
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
}
