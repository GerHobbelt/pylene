#include <ano/core/value/value_set.hpp>

#include <algorithm>
#include <any>
#include <cinttypes>
#include <cmath>
#include <limits>
#include <optional>
#include <type_traits>
#include <typeinfo>
#include <variant>


namespace details
{


  template <class... Args>
  auto any_to_variant_cast(std::any a) -> std::optional<std::variant<Args...>>
  {
    if (!a.has_value())
      return std::nullopt;

    std::optional<std::variant<Args...>> v = std::nullopt;

    bool found = ((a.type() == typeid(Args) && (v = std::any_cast<Args>(std::move(a)), true)) || ...);

    if (!found)
      return std::nullopt;

    return {std::move(*v)};
  }

  template <template <typename> class Op, typename... Args>
  auto call_template_op_from_type_info(const std::type_info& tid) -> std::optional<std::variant<Op<Args>...>>
  {
    std::optional<std::variant<Op<Args>...>> v = std::nullopt;

    bool found = ((tid == typeid(Args) && (v = Op<Args>{}, true)) || ...);

    if (!found)
      return std::nullopt;

    return {std::move(*v)};
  }

  template <class... Ts>
  struct type_list
  {
  };

  template <class... Ts>
  auto any_to_variant_cast_type_list(std::any a, type_list<Ts...>) -> std::optional<std::variant<Ts...>>
  {
    return any_to_variant_cast<Ts...>(a);
  }

  template <template <typename> class Op, typename... Ts>
  auto call_template_op_from_type_info_type_list(const std::type_info& tid, type_list<Ts...>)
      -> std::optional<std::variant<Op<Ts>...>>
  {
    return call_template_op_from_type_info<Op, Ts...>(tid);
  }

  using supported_types = type_list<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, std::int8_t,
                                    std::int16_t, std::int32_t, std::int64_t, float, double>;

  template <class T, class U, class C = std::common_type_t<T, U>>
  C max(const T& a, const U& b)
  {
    return std::max(static_cast<C>(a), static_cast<C>(b));
  }

  template <class T, class U, class C = std::common_type_t<T, U>>
  C min(const T& a, const U& b)
  {
    return std::min(static_cast<C>(a), static_cast<C>(b));
  }

  template <class T, class U>
  long double pow(const T& a, const U& b)
  {
    return std::pow(static_cast<long double>(a), static_cast<long double>(b));
  }

} // namespace details


namespace ano
{

  type_erased_value::type_erased_value(const std::any& v, const abstract_value_set& vs)
    : v_(v)
    , vs_(&vs)
  {
  }

  const std::any& type_erased_value::val() const
  {
    return v_;
  }
  const std::type_info& type_erased_value::tid() const
  {
    return v_.type();
  }

  template <typename T>
  struct limit_max
  {
    auto operator()() { return std::numeric_limits<T>::max(); }
  };

  type_erased_value type_erased_value::max() const
  {
    auto op = details::call_template_op_from_type_info_type_list<limit_max>(tid(), details::supported_types{});

    auto ret = std::visit([](auto op_) -> std::any { return {op_()}; }, *op);
    return {ret, *vs_};
  }

  template <typename T>
  struct limit_min
  {
    auto operator()() { return std::numeric_limits<T>::min(); }
  };

  type_erased_value type_erased_value::min() const
  {
    auto op = details::call_template_op_from_type_info_type_list<limit_min>(tid(), details::supported_types{});

    auto ret = std::visit([](auto op_) -> std::any { return {op_()}; }, *op);
    return {ret, *vs_};
  }

  type_erased_value type_erased_value::sup() const
  {
    auto op = details::call_template_op_from_type_info_type_list<limit_max>(tid(), details::supported_types{});

    auto ret = std::visit([](auto op_) -> std::any { return {op_()}; }, *op);
    return {ret, *vs_};
  }

  type_erased_value type_erased_value::inf() const
  {
    auto op = details::call_template_op_from_type_info_type_list<limit_min>(tid(), details::supported_types{});

    auto ret = std::visit([](auto op_) -> std::any { return {op_()}; }, *op);
    return {ret, *vs_};
  }

  type_erased_value type_erased_value::plus() const
  {
    return {vs_->plus(v_), *vs_};
  }
  type_erased_value type_erased_value::minus() const
  {
    return {vs_->plus(v_), *vs_};
  }

  type_erased_value type_erased_value::max(const type_erased_value& rhs) const
  {
    return {vs_->max(v_, rhs.val()), *vs_};
  }
  type_erased_value type_erased_value::min(const type_erased_value& rhs) const
  {
    return {vs_->min(v_, rhs.val()), *vs_};
  }
  type_erased_value type_erased_value::add(const type_erased_value& rhs) const
  {
    return {vs_->add(v_, rhs.val()), *vs_};
  }
  type_erased_value type_erased_value::sub(const type_erased_value& rhs) const
  {
    return {vs_->sub(v_, rhs.val()), *vs_};
  }
  type_erased_value type_erased_value::mult(const type_erased_value& rhs) const
  {
    return {vs_->mult(v_, rhs.val()), *vs_};
  }
  type_erased_value type_erased_value::div(const type_erased_value& rhs) const
  {
    return {vs_->div(v_, rhs.val()), *vs_};
  }
  /*
  type_erased_value type_erased_value::mod(const type_erased_value& rhs) const
  {
    return {vs_->mod(v_, rhs.val()), *vs_};
  }
  */
  type_erased_value type_erased_value::pow(const type_erased_value& rhs) const
  {
    return {vs_->pow(v_, rhs.val()), *vs_};
  }


  value_set<type_erased_value>::value_set(const type_erased_value& v)
    : v_(&v)
  {
  }

  value_set<type_erased_value>::~value_set() {}

  std::any value_set<type_erased_value>::max() const
  {
    return {v_->max()};
  }
  std::any value_set<type_erased_value>::min() const
  {
    return {v_->min()};
  }
  std::any value_set<type_erased_value>::sup() const
  {
    return {v_->sup()};
  }
  std::any value_set<type_erased_value>::inf() const
  {
    return {v_->inf()};
  }

  std::any value_set<type_erased_value>::plus(const std::any& v) const
  {

    auto va = details::any_to_variant_cast_type_list(v, details::supported_types{});
    if (va)
      return {std::visit([](auto _v) -> std::any { return {+_v}; }, *va)};
    else
    {
      auto absv = std::any_cast<type_erased_value>(v);
      return {absv.plus()};
    }
  }

  std::any value_set<type_erased_value>::minus(const std::any& v) const
  {
    auto va = details::any_to_variant_cast_type_list(v, details::supported_types{});
    if (va)
      return {std::visit([](auto _v) -> std::any { return {-_v}; }, *va)};
    else
    {
      auto absv = std::any_cast<type_erased_value>(v);
      return {absv.minus()};
    }
  }

  std::any value_set<type_erased_value>::max(const std::any& l, const std::any& r) const
  {
    auto lhs = details::any_to_variant_cast_type_list(l, details::supported_types{});
    auto rhs = details::any_to_variant_cast_type_list(r, details::supported_types{});

    if (lhs && rhs)
      return {std::visit([](auto lhs_, auto rhs_) -> std::any { return {details::max(lhs_, rhs_)}; }, *lhs, *rhs)};
    else
    {
      auto absl = std::any_cast<type_erased_value>(l);
      auto absr = std::any_cast<type_erased_value>(r);
      return {absl.max(absr)};
    }
  }

  std::any value_set<type_erased_value>::min(const std::any& l, const std::any& r) const
  {
    auto lhs = details::any_to_variant_cast_type_list(l, details::supported_types{});
    auto rhs = details::any_to_variant_cast_type_list(r, details::supported_types{});

    if (lhs && rhs)
      return {std::visit([](auto lhs_, auto rhs_) -> std::any { return {details::min(lhs_, rhs_)}; }, *lhs, *rhs)};
    else
    {
      auto absl = std::any_cast<type_erased_value>(l);
      auto absr = std::any_cast<type_erased_value>(r);
      return {absl.min(absr)};
    }
  }

  std::any value_set<type_erased_value>::add(const std::any& l, const std::any& r) const
  {
    auto lhs = details::any_to_variant_cast_type_list(l, details::supported_types{});
    auto rhs = details::any_to_variant_cast_type_list(r, details::supported_types{});

    if (lhs && rhs)
      return {std::visit([](auto lhs_, auto rhs_) -> std::any { return {lhs_ + rhs_}; }, *lhs, *rhs)};
    else
    {
      auto absl = std::any_cast<type_erased_value>(l);
      auto absr = std::any_cast<type_erased_value>(r);
      return {absl.add(absr)};
    }
  }

  std::any value_set<type_erased_value>::sub(const std::any& l, const std::any& r) const
  {
    auto lhs = details::any_to_variant_cast_type_list(l, details::supported_types{});
    auto rhs = details::any_to_variant_cast_type_list(r, details::supported_types{});

    if (lhs && rhs)
      return {std::visit([](auto lhs_, auto rhs_) -> std::any { return {lhs_ - rhs_}; }, *lhs, *rhs)};
    else
    {
      auto absl = std::any_cast<type_erased_value>(l);
      auto absr = std::any_cast<type_erased_value>(r);
      return {absl.sub(absr)};
    }
  }

  std::any value_set<type_erased_value>::mult(const std::any& l, const std::any& r) const
  {
    auto lhs = details::any_to_variant_cast_type_list(l, details::supported_types{});
    auto rhs = details::any_to_variant_cast_type_list(r, details::supported_types{});

    if (lhs && rhs)
      return {std::visit([](auto lhs_, auto rhs_) -> std::any { return {lhs_ * rhs_}; }, *lhs, *rhs)};
    else
    {
      auto absl = std::any_cast<type_erased_value>(l);
      auto absr = std::any_cast<type_erased_value>(r);
      return {absl.mult(absr)};
    }
  }

  std::any value_set<type_erased_value>::div(const std::any& l, const std::any& r) const
  {
    auto lhs = details::any_to_variant_cast_type_list(l, details::supported_types{});
    auto rhs = details::any_to_variant_cast_type_list(r, details::supported_types{});

    if (lhs && rhs)
      return {std::visit([](auto lhs_, auto rhs_) -> std::any { return {lhs_ / rhs_}; }, *lhs, *rhs)};
    else
    {
      auto absl = std::any_cast<type_erased_value>(l);
      auto absr = std::any_cast<type_erased_value>(r);
      return {absl.div(absr)};
    }
  }
  /*
  std::any value_set<type_erased_value>::mod(const std::any& l, const std::any& r) const
  {
    auto lhs = details::any_to_variant_cast_type_list(l, details::supported_types{});
    auto rhs = details::any_to_variant_cast_type_list(r, details::supported_types{});

    if (lhs && rhs)
      return {std::visit([](auto lhs_, auto rhs_) -> std::any { return {lhs_ % rhs_}; }, *lhs, *rhs)};
    else
    {
      auto absl = std::any_cast<type_erased_value>(l);
      auto absr = std::any_cast<type_erased_value>(r);
      return {absl.mod(absr)};
    }
  }
  */
  std::any value_set<type_erased_value>::pow(const std::any& l, const std::any& r) const
  {
    auto lhs = details::any_to_variant_cast_type_list(l, details::supported_types{});
    auto rhs = details::any_to_variant_cast_type_list(r, details::supported_types{});

    if (lhs && rhs)
      return {std::visit([](auto lhs_, auto rhs_) -> std::any { return {details::pow(lhs_, rhs_)}; }, *lhs, *rhs)};
    else
    {
      auto absl = std::any_cast<type_erased_value>(l);
      auto absr = std::any_cast<type_erased_value>(r);
      return {absl.pow(absr)};
    }
  }

} // namespace ano
