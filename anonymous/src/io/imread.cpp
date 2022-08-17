#include <ano/io/imread.hpp>

#include <ano/core/image/ndbuffer_image.hpp>
#include <ano/io/private/freeimage_plugin.hpp>
#include <ano/io/private/io.hpp>

namespace ano::io
{
  ano::ndbuffer_image imread(const std::string& filename)
  {
    ano::ndbuffer_image out;
    imread(filename, out);
    return out;
  }

  void imread(const std::string& filename, ano::ndbuffer_image& out)
  {
    internal::freeimage_reader_plugin p;
    internal::load(&p, filename.c_str(), out);
  }
} // namespace ano::io
