#include <mln/io/imread.hpp>
#include <mln/io/private/freeimage_plugin.hpp>
#include <mln/io/private/io.hpp>
#include <mln/python/io.hpp>

namespace pln
{
  void imsave(const std::string& filename, const mln::ndbuffer_image& img)
  {
    mln::io::internal::freeimage_writer_plugin p;
    mln::io::internal::save(img, &p, filename.c_str());
  }

  void def_io_module(pybind11::module_& m)
  {
    auto io_m = m.def_submodule("io");
    io_m.def("imread", [](const std::string& filename) { return mln::io::imread(filename); });
    io_m.def("imsave", &imsave);
  }
} // namespace pln