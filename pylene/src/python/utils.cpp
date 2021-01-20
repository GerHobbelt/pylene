#include <fixtures/ImagePath/image_path.hpp>

#include <mln/python/utils.hpp>

namespace pln
{
  void def_utils_module(pybind11::module_& m)
  {
    auto utils_m = m.def_submodule("utils");
    utils_m.def("get_image_filename",
                [](const std::string& name) { return fixtures::ImagePath::concat_with_filename(name); });
    utils_m.def("id", [](mln::ndbuffer_image im) { return im; });
  }
} // namespace pln