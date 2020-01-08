#include "morpho.hpp"

#include "ndimage_buffer_helper.hpp"
#include "visit.hpp"

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/morpho/experimental/dilation.hpp>

#include <mln/core/trace.hpp>

#include <fmt/core.h>


namespace py = pybind11;

namespace
{

  template <typename T, int D>
  struct dilate_operator_impl_t
  {
    template <typename Img, typename SE>
    mln::ndbuffer_image operator()(Img&& img, const SE& se) const
    {
      if (auto* image_ptr = std::forward<Img>(img).template cast_to<T, D>(); image_ptr)
        return mln::morpho::experimental::dilation(*image_ptr, se);
      else
        std::runtime_error("Unable to convert the image to the required type.");
    }
  };

  template <int D>
  struct dilate_operator_impl_t<mln::rgb8, D>
  {
    template <typename Img, typename SE>
    mln::ndbuffer_image operator()(Img&& img, const SE& se) const
    {
      std::runtime_error("Unsupported type.");
      return {};
    }
  };

  template <int D>
  struct dilate_operator_t;

  template <>
  struct dilate_operator_t<1>
  {
    template <typename T>
    using type = dilate_operator_impl_t<T, 1>;
  };

  template <>
  struct dilate_operator_t<2>
  {
    template <typename T>
    using type = dilate_operator_impl_t<T, 2>;
  };

  template <>
  struct dilate_operator_t<3>
  {
    template <typename T>
    using type = dilate_operator_impl_t<T, 3>;
  };

  template <>
  struct dilate_operator_t<4>
  {
    template <typename T>
    using type = dilate_operator_impl_t<T, 4>;
  };

  template <>
  struct dilate_operator_t<5>
  {
    template <typename T>
    using type = dilate_operator_impl_t<T, 5>;
  };


  mln::ndbuffer_image dilate(py::buffer buffer, int disc_radius = 1)
  {
    mln::ndbuffer_image input;
    mln::py::ndimage_from_buffer(input, buffer);

    if (input.border() < disc_radius * 2)
    {
      mln::trace::warn("The border size is not large enough. The processing roi will be shrinked (2px).");
      input.inflate_domain(-2 * disc_radius);
    }

    auto disc = mln::experimental::se::disc(disc_radius);
    switch (input.pdim())
    {
    // case 1:
    //   return mln::py::visit<dilate_operator_t<1>::type>(input.sample_type(), input, disc);
    case 2:
      return mln::py::visit<dilate_operator_t<2>::type>(input.sample_type(), input, disc);
    case 3:
      return mln::py::visit<dilate_operator_t<3>::type>(input.sample_type(), input, disc);
    case 4:
      return mln::py::visit<dilate_operator_t<4>::type>(input.sample_type(), input, disc);
    // case 5:
    //   return mln::py::visit<dilate_operator_t<5>::type>(input.sample_type(), input, disc);
    default:
      std::runtime_error("Unsupported dimension.");
    }
  }

} // namespace


using namespace pybind11::literals;

void init_module_morpho(py::module& m)
{
  m.def("dilate", dilate,
        "Perform a morphological dilation.\n"
        "\n"
        "disc_radius must be strictly superior to 0.",
        "Input"_a, "disc_radius"_a = 1);
}
