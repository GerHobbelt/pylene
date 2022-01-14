#pragma once

#include <mln/core/value/value_set.hpp>
#include <pybind11/pybind11.h>

#include <any>
#include <cinttypes>
#include <memory>
#include <string>
#include <string_view>
#include <typeinfo>
#include <variant>


namespace mln
{
  template <>
  struct value_set<pybind11::object> : abstract_value_set
  {
    value_set(pybind11::object python_vs_instance);

    ~value_set() override;

    template <typename U>
    std::any cast(const std::any& v) const;

    std::any max() const override;
    std::any min() const override;
    std::any sup() const override;
    std::any inf() const override;

    std::any plus(const std::any& v) const override;
    std::any minus(const std::any& v) const override;

    std::any max(const std::any& l, const std::any& r) const override;
    std::any min(const std::any& l, const std::any& r) const override;
    std::any add(const std::any& l, const std::any& r) const override;
    std::any sub(const std::any& l, const std::any& r) const override;
    std::any mult(const std::any& l, const std::any& r) const override;
    std::any div(const std::any& l, const std::any& r) const override;
    /* std::any mod(const std::any& l, const std::any& r) const override; */
    std::any pow(const std::any& l, const std::any& r) const override;

  private:
    pybind11::object vs_instance_;
  };

} // namespace mln


namespace details
{

  template <class U>
  auto get_python_type()
  {
    // C++ type -> Python type
    static std::unordered_map<std::type_index, std::string> type_names{
        {std::type_index(typeid(bool{})), "bool"},         //
        {std::type_index(typeid(int8_t{})), "int"},        //
        {std::type_index(typeid(int16_t{})), "int"},       //
        {std::type_index(typeid(int32_t{})), "int"},       //
        {std::type_index(typeid(int64_t{})), "int"},       //
        {std::type_index(typeid(uint8_t{})), "int"},       //
        {std::type_index(typeid(uint16_t{})), "int"},      //
        {std::type_index(typeid(uint32_t{})), "int"},      //
        {std::type_index(typeid(uint64_t{})), "int"},      //
        {std::type_index(typeid(float{})), "float"},       //
        {std::type_index(typeid(double{})), "float"},      //
        {std::type_index(typeid((char*){})), "str"},       //
        {std::type_index(typeid((const char*){})), "str"}, //
        {std::type_index(typeid(std::string{})), "str"}};
    return type_names[std::type_index(typeid(U{}))];
  }

} // namespace details


namespace mln
{
  template <typename U>
  std::any value_set<pybind11::object>::cast(const std::any& v) const
  {
    pybind11::object v_ = std::any_cast<pybind11::object>(v);
    // if constexpr (std::is_arithmetic_v<U>) // bool, signed/unsigned int, floating point
    if constexpr (std::is_same_v<U, bool>) // bool
    {
      pybind11::bool_ pyv = vs_instance_.attr("cast")(v_, ::details::get_python_type<U>());
      return {static_cast<U>(pyv)};
    }
    else if constexpr (std::is_integral_v<U>) // signed/unsigned int
    {
      pybind11::int_ pyv = vs_instance_.attr("cast")(v_, ::details::get_python_type<U>());
      return {static_cast<U>(pyv)};
    }
    else if constexpr (std::is_floating_point_v<U>) // floating point
    {
      pybind11::float_ pyv = vs_instance_.attr("cast")(v_, ::details::get_python_type<U>());
      return {static_cast<U>(pyv)};
    }
    else
    {
      pybind11::object ret = vs_instance_.attr("cast")(v, ::details::get_python_type<U>());
      return {*(ret.cast<U*>())};
    }
  }

} // namespace mln
