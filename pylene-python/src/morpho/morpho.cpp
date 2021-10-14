#include <mln/morpho/maxtree.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <pln/core/image_cast.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

namespace pln::morpho
{

  struct Maxtree
  {
    py::array_t<uint8_t> get_values() const
    {
      py::array_t<uint8_t> r(_handle.values.size());
      std::copy(_handle.values.begin(), _handle.values.end(), r.mutable_data());
      return r;
    }

    py::array_t<int> get_parent() const
    {
      py::array_t<int> r(_handle.parent.size());
      std::copy(_handle.parent.begin(), _handle.parent.end(), r.mutable_data());
      return r;
    }

    mln::morpho::component_tree<uint8_t> _handle;
  };


  std::pair<Maxtree, mln::ndbuffer_image>
  maxtree2d(mln::ndbuffer_image input, int connectivity)
  {
    auto in = input.cast_to<std::uint8_t, 2>();
    if (in == nullptr)
      throw std::invalid_argument("input image should be a 2D uint8 image");

    if (connectivity != 4 && connectivity != 8)
      throw std::invalid_argument("Connectivity should be 4 or 8");

    auto [maxtree, nodemap] = (connectivity == 4) ? mln::morpho::maxtree(*in, mln::c4) : mln::morpho::maxtree(*in, mln::c8);
    return {Maxtree{std::move(maxtree)}, std::move(nodemap)};
  }

}



void init_morpho_module(py::module_& m)
{
  py::class_<pln::morpho::Maxtree>(m, "Maxtree")             //
      .def(py::init<>())                                     //
      .def_property_readonly("parent", &pln::morpho::Maxtree::get_parent) //
      .def_property_readonly("values", &pln::morpho::Maxtree::get_values);

  m.def("maxtree", &pln::morpho::maxtree2d, "Compute the maxtree of a 2D Image");
}
