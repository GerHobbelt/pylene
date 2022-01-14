#include "morpho.hpp"

#include "ndimage.hpp"
#include "py_value_set.hpp"
#include "visit.hpp"

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/morpho/experimental/dilation.hpp>

#include <fmt/core.h>

#include <pybind11/stl.h>

#include <algorithm>
#include <string_view>
#include <variant>


namespace my
{
  template <class T>
  mln::image2d<float> slow_stretch(const mln::image2d<T>& src, const mln::value_set<pybind11::object>& py_vs)
  {
    mln::image2d<float> res = mln::transform(src, [&py_vs](auto val) -> float {
      auto anymax  = py_vs.max();                        // returns std::any of pybind11:object
      auto anyval  = std::any{pybind11::cast(val)};      // converts to std::any of pybind11::object
      auto anyret  = py_vs.div(anyval, anymax);          // returns std::any of pybind11::object
      auto anyfret = py_vs.template cast<float>(anyret); // returns std::any of float
      return std::any_cast<float>(anyfret);              // returns float
    });
    return res;
  }

  template <class T>
  mln::image2d<float> virtual_dispatch_stretch(const mln::image2d<T>& src)
  {
    auto                vs   = mln::concrete_value_set<T>{};     // value-set for T
    auto                vs_f = mln::concrete_value_set<float>{}; // value-set for float
    mln::image2d<float> res  = mln::transform(src, [&vs, &vs_f](auto val) -> float {
      auto anymax  = vs.max();                                 // returns std::any
      auto fanyval = vs.template cast<float>(val);             // cast to float in std::any
      auto fanymax = vs.template cast<float>(anymax);          // cast to float in std::any
      return std::any_cast<float>(vs_f.div(fanyval, fanymax)); // div returns float
    });
    return res;
  }

  template <class T>
  mln::image2d<float> stretch_virtual_dispatch_type_erased_value(const mln::image2d<T>& src)
  {
    auto                vs   = mln::concrete_value_set<T>{};     // value-set for T
    auto                vs_f = mln::concrete_value_set<float>{}; // value-set for float
    mln::image2d<float> res  = mln::transform(src, [&vs, &vs_f](auto val) -> float {
      // simulate having an image<type_erased_value> by contructing the type_erased_value here
      auto anyval = std::any{val}; // std::any of T
                                   // type_erased_value of std::any of T aware of value-set of T
      auto abs_anyval = mln::type_erased_value{anyval, vs};
      // instantiate a value-set for type_erased_value
      auto abs_vs = mln::value_set<mln::type_erased_value>{abs_anyval};
      auto anyabs_anymax =
          abs_vs.max(); // returns std::any of type_erased_value
                        // cast underlying std::any of type_erased_value of std::any of T into
                        // std::any of type_erased_value of std::any of float aware of value-set for float
      auto anyabs_fanyval = abs_vs.template cast<T, float>(std::any{abs_anyval}, &vs_f);
      auto anyabs_fanymax = abs_vs.template cast<T, float>(anyabs_anymax, &vs_f);
      // dispatch on known type, find a type_erased_value, then call
      // anyabs_fanyval.div(anyabs_fanymax) to perform division which will call
      // the underlying value-set for float for this operation
      auto anyabs_fanyret = abs_vs.div(anyabs_fanyval, anyabs_fanymax);
      // convert result back into float for returning to the image
      auto anyfret = std::any_cast<mln::type_erased_value>(anyabs_fanyret).val();
      return std::any_cast<float>(anyfret);
    });
    return res;
  }

  template <class T>
  mln::image2d<float> fast_stretch(const mln::image2d<T>& src)
  {
    auto                vs   = src.get_value_set();     // value-set for T
    auto                vs_f = mln::value_set<float>{}; // fast value-set for float
    mln::image2d<float> res  = mln::transform(src, [&vs, &vs_f](auto val) -> float {
      auto max  = vs.max();                     // returns T
      auto fval = vs.template cast<float>(val); // returns float
      auto fmax = vs.template cast<float>(max); // returns float
      return vs_f.div(fval, fmax);              // div directly returns float
    });
    return res;
  }
} // namespace my

namespace
{

  template <typename T>
  struct stretch_operator_t
  {
    template <typename Img>
    mln::ndbuffer_image operator()(Img&& img, const std::optional<pybind11::object>& py_vs) const
    {
      if (auto* image_ptr = std::forward<Img>(img).template cast_to<T, 2>(); image_ptr)
      {
        if (py_vs.has_value())
        {
          auto vs = mln::value_set<pybind11::object>{py_vs.value()};
          return my::slow_stretch(*image_ptr, vs);
        }
        else
        {
          return my::fast_stretch(*image_ptr);
        }
      }
      else
      {
        std::runtime_error("Unable to convert the image to the required type.");
        return {};
      }
    }
  };

  mln::ndbuffer_image stretch(mln::ndbuffer_image input, std::optional<pybind11::object> py_vs)
  {
    if (input.pdim() == 2)
    {
      return mln::py::visit<stretch_operator_t>(input.sample_type(), input, py_vs);
    }
    else
    {
      std::runtime_error("Unsupported dimension.");
      return {};
    }
  }

  template <typename T>
  struct stretch_virtual_dispatch_operator_t
  {
    template <typename Img>
    mln::ndbuffer_image operator()(Img&& img) const
    {
      if (auto* image_ptr = std::forward<Img>(img).template cast_to<T, 2>(); image_ptr)
      {
        return my::virtual_dispatch_stretch(*image_ptr);
      }
      else
      {
        std::runtime_error("Unable to convert the image to the required type.");
        return {};
      }
    }
  };

  mln::ndbuffer_image stretch_virtual_dispatch(mln::ndbuffer_image input)
  {
    if (input.pdim() == 2)
    {
      return mln::py::visit<stretch_virtual_dispatch_operator_t>(input.sample_type(), input);
    }
    else
    {
      std::runtime_error("Unsupported dimension.");
      return {};
    }
  }

  template <typename T>
  struct stretch_virtual_dispatch_type_erased_value_operator_t
  {
    template <typename Img>
    mln::ndbuffer_image operator()(Img&& img) const
    {
      if (auto* image_ptr = std::forward<Img>(img).template cast_to<T, 2>(); image_ptr)
      {
        return my::stretch_virtual_dispatch_type_erased_value(*image_ptr);
      }
      else
      {
        std::runtime_error("Unable to convert the image to the required type.");
        return {};
      }
    }
  };

  mln::ndbuffer_image stretch_virtual_dispatch_type_erased_value(mln::ndbuffer_image input)
  {
    if (input.pdim() == 2)
    {
      return mln::py::visit<stretch_virtual_dispatch_type_erased_value_operator_t>(input.sample_type(), input);
    }
    else
    {
      std::runtime_error("Unsupported dimension.");
      return {};
    }
  }

  using se_t = std::variant<mln::se::disc, mln::se::disc_non_decomp, mln::se::rect2d, mln::se::rect2d_non_decomp>;

  template <typename T>
  struct dilate2d_operator_t
  {
    template <typename Img, typename SE>
    mln::ndbuffer_image operator()(Img&& img, SE&& se) const
    {
      if (auto* image_ptr = std::forward<Img>(img).template cast_to<T, 2>(); image_ptr)
      {
        return mln::morpho::experimental::dilation(*image_ptr, std::forward<SE>(se));
      }
      else
      {
        std::runtime_error("Unable to convert the image to the required type.");
        return {};
      }
    }
  };

  mln::ndbuffer_image dilate(mln::ndbuffer_image input, const se_t& se)
  {
    if (input.pdim() == 2)
    {
      return std::visit(
          [&input](const auto& se_) { return mln::py::visit<dilate2d_operator_t>(input.sample_type(), input, se_); },
          se);
    }
    else
    {
      std::runtime_error("Unsupported dimension.");
      return {};
    }
  }

} // namespace


namespace mln::py
{

  using namespace pybind11::literals;

  void init_module_morpho(pybind11::module& m)
  {
    m.def("dilate", dilate,
          "Perform a morphological dilation.\n"
          "\n"
          "structuring element must be valid.",
          "Input"_a, "se"_a)

        .def("stretch", stretch,
             "Perform a morphological stretch.\n"
             "Input"_a,
             "vs"_a = pybind11::none())

        .def("stretch_virtual_dispatch", stretch_virtual_dispatch,
             "Perform a morphological stretch.\n"
             "Input"_a)

        .def("stretch_virtual_dispatch_type_erased_value", stretch_virtual_dispatch_type_erased_value,
             "Perform a morphological stretch.\n"
             "Input"_a);
  }

} // namespace mln::py
