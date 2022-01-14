#include "py_value_set.hpp"

#include <any>

namespace mln
{

  value_set<pybind11::object>::value_set(pybind11::object python_vs_instance)
    : vs_instance_(python_vs_instance)
  {
  }

  value_set<pybind11::object>::~value_set()
  {
  }

  std::any value_set<pybind11::object>::max() const { return {vs_instance_.attr("max")()}; }

  std::any value_set<pybind11::object>::min() const { return {vs_instance_.attr("min")()}; }

  std::any value_set<pybind11::object>::sup() const { return {vs_instance_.attr("sup")()}; }

  std::any value_set<pybind11::object>::inf() const { return {vs_instance_.attr("inf")()}; }


  std::any value_set<pybind11::object>::plus(const std::any& v) const
  {
    auto pyv = std::any_cast<pybind11::object>(v);
    return {vs_instance_.attr("plus")(pyv)};
  }

  std::any value_set<pybind11::object>::minus(const std::any& v) const
  {
    auto pyv = std::any_cast<pybind11::object>(v);
    return {vs_instance_.attr("minus")(pyv)};
  }


  std::any value_set<pybind11::object>::max(const std::any& l, const std::any& r) const
  {
    auto pyl = std::any_cast<pybind11::object>(l);
    auto pyr = std::any_cast<pybind11::object>(r);
    return {vs_instance_.attr("max")(pyl, pyr)};
  }

  std::any value_set<pybind11::object>::min(const std::any& l, const std::any& r) const
  {
    auto pyl = std::any_cast<pybind11::object>(l);
    auto pyr = std::any_cast<pybind11::object>(r);
    return {vs_instance_.attr("min")(pyl, pyr)};
  }

  std::any value_set<pybind11::object>::add(const std::any& l, const std::any& r) const
  {
    auto pyl = std::any_cast<pybind11::object>(l);
    auto pyr = std::any_cast<pybind11::object>(r);
    return {vs_instance_.attr("add")(pyl, pyr)};
  }

  std::any value_set<pybind11::object>::sub(const std::any& l, const std::any& r) const
  {
    auto pyl = std::any_cast<pybind11::object>(l);
    auto pyr = std::any_cast<pybind11::object>(r);
    return {vs_instance_.attr("sub")(pyl, pyr)};
  }

  std::any value_set<pybind11::object>::mult(const std::any& l, const std::any& r) const
  {
    auto pyl = std::any_cast<pybind11::object>(l);
    auto pyr = std::any_cast<pybind11::object>(r);
    return {vs_instance_.attr("mult")(pyl, pyr)};
  }

  std::any value_set<pybind11::object>::div(const std::any& l, const std::any& r) const
  {
    auto pyl = std::any_cast<pybind11::object>(l);
    auto pyr = std::any_cast<pybind11::object>(r);
    return {vs_instance_.attr("div")(pyl, pyr)};
  }

  /*
  std::any value_set<pybind11::object>::mod(const std::any& l, const std::any& r) const
  {
    auto pyl = std::any_cast<pybind11::object>(l);
    auto pyr = std::any_cast<pybind11::object>(r);
    return {vs_instance_.attr("mod")(pyl, pyr)};
  }
  */

  std::any value_set<pybind11::object>::pow(const std::any& l, const std::any& r) const
  {
    auto pyl = std::any_cast<pybind11::object>(l);
    auto pyr = std::any_cast<pybind11::object>(r);
    return {vs_instance_.attr("pow")(pyl, pyr)};
  }

} // namespace mln
