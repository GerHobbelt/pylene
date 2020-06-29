#include "morpho.hpp"

#include "ndimage_buffer_helper.hpp"
#include "visit.hpp"

#include <mln/core/image/ndimage.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/core/trace.hpp>
#include <mln/morpho/experimental/dilation.hpp>

#include <fmt/core.h>

#include <pybind11/stl.h>

#include <variant>


namespace py = pybind11;

namespace
{

  using se_t = std::variant<mln::se::disc, mln::se::disc_non_decomp,
                            mln::se::rect2d, mln::se::rect2d_non_decomp>;

  template <typename T>
  struct dilate2d_operator_t
  {
    template <typename Img, typename SE>
    mln::ndbuffer_image operator()(Img&& img, SE se) const
    {
      if (auto* image_ptr = std::forward<Img>(img).template cast_to<T, 2>(); image_ptr)
        return mln::morpho::experimental::dilation(*image_ptr, se);
      else
      {
        std::runtime_error("Unable to convert the image to the required type.");
        return {};
      }
    }
  };

  mln::ndbuffer_image dilate(::py::buffer buffer, const se_t& se)
  {
    auto input = mln::py::ndimage_from_buffer(buffer);

    if (input.pdim() == 2)
    {
      return std::visit(
          [&input](const auto& se_) { return mln::py::visit<dilate2d_operator_t>(input.sample_type(), input, se_); }, se);
    }
    else
    {
      std::runtime_error("Unsupported dimension.");
      return {};
    }
  }

} // namespace


using namespace pybind11::literals;

void init_module_morpho(::py::module& m)
{
  m.def("dilate", dilate,
        "Perform a morphological dilation.\n"
        "\n"
        "structuring element must be valid.",
        "Input"_a, "se"_a);
}
