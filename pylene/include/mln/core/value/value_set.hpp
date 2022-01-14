#pragma once

#include <algorithm>
#include <any>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>
#include <utility>


namespace mln
{

  template <class T = void>
  struct value_set
  {
    template <class U>
    U cast(T v) const
    {
      return static_cast<U>(v);
    }

    T max() const noexcept { return std::numeric_limits<T>::max(); }
    T min() const noexcept { return std::numeric_limits<T>::min(); }
    T sup() const noexcept { return std::numeric_limits<T>::max(); }
    T inf() const noexcept { return std::numeric_limits<T>::min(); }

    T plus(T v) const noexcept { return +v; }
    T minus(T v) const noexcept { return -v; }

    auto max(T l, T r) const noexcept { return std::max(l, r); }
    auto min(T l, T r) const noexcept { return std::min(l, r); }
    auto add(T l, T r) const noexcept { return l + r; }
    auto sub(T l, T r) const noexcept { return l - r; }
    auto mult(T l, T r) const noexcept { return l * r; }
    auto div(T l, T r) const noexcept { return l / r; }
    auto mod(T l, T r) const noexcept { return l % r; }
    auto pow(T l, T r) const noexcept { return std::pow(l, r); }
  };

  template <>
  struct value_set<void>
  {

    template <class U, class T>
    U cast(T&& t) const
    {
      return static_cast<U>(std::forward<T>);
    }

    template <class T>
    T max() const noexcept
    {
      return std::numeric_limits<T>::max();
    }
    template <class T>
    T min() const noexcept
    {
      return std::numeric_limits<T>::min();
    }
    template <class T>
    T sup() const noexcept
    {
      return std::numeric_limits<T>::max();
    }
    template <class T>
    T inf() const noexcept
    {
      return std::numeric_limits<T>::min();
    }

    template <class T>
    T plus(T&& v) const noexcept
    {
      return +std::forward<T>(v);
    }
    template <class T>
    T minus(T&& v) const noexcept
    {
      return -std::forward<T>(v);
    }

    template <class T, class U>
    auto max(T&& l, T&& r) const noexcept
    {
      return std::max(std::forward<T>(l), std::forward<T>(r));
    }
    template <class T, class U>
    auto min(T&& l, T&& r) const noexcept
    {
      return std::min(std::forward<T>(l), std::forward<T>(r));
    }

    template <class T, class U>
    auto add(T&& l, U&& r) const noexcept
    {
      return std::forward<T>(l) + std::forward<T>(r);
    }
    template <class T, class U>
    auto sub(T&& l, U&& r) const noexcept
    {
      return std::forward<T>(l) - std::forward<T>(r);
    }
    template <class T, class U>
    auto mult(T&& l, U&& r) const noexcept
    {
      return std::forward<T>(l) * std::forward<T>(r);
    }
    template <class T, class U>
    auto div(T&& l, U&& r) const noexcept
    {
      return std::forward<T>(l) / std::forward<T>(r);
    }
    template <class T, class U>
    auto mod(T&& l, U&& r) const noexcept
    {
      return std::forward<T>(l) % std::forward<T>(r);
    }
    template <class T, class U>
    auto pow(T&& l, U&& r) const noexcept
    {
      return std::pow(std::forward<T>(l), std::forward<T>(r));
    }
  };

  struct abstract_value_set
  {
    virtual ~abstract_value_set() {}

    /*
    template <class U>
    virtual std::any cast(std::any v) const = 0;
    */

    virtual std::any max() const = 0;
    virtual std::any min() const = 0;
    virtual std::any sup() const = 0;
    virtual std::any inf() const = 0;

    virtual std::any plus(const std::any& v) const  = 0;
    virtual std::any minus(const std::any& v) const = 0;

    virtual std::any max(const std::any& l, const std::any& r) const  = 0;
    virtual std::any min(const std::any& l, const std::any& r) const  = 0;
    virtual std::any add(const std::any& l, const std::any& r) const  = 0;
    virtual std::any sub(const std::any& l, const std::any& r) const  = 0;
    virtual std::any mult(const std::any& l, const std::any& r) const = 0;
    virtual std::any div(const std::any& l, const std::any& r) const  = 0;
    // virtual std::any mod(const std::any& l, const std::any& r) const  = 0;
    virtual std::any pow(const std::any& l, const std::any& r) const = 0;
  };

  template <typename T>
  struct concrete_value_set : abstract_value_set
  {
    ~concrete_value_set() override {}

    template <class U>
    std::any cast(std::any v) const /* override */
    {
      return {static_cast<U>(std::any_cast<T>(v))};
    }

    std::any max() const override { return {std::numeric_limits<T>::max()}; };
    std::any min() const override { return {std::numeric_limits<T>::min()}; };
    std::any sup() const override { return {std::numeric_limits<T>::max()}; };
    std::any inf() const override { return {std::numeric_limits<T>::min()}; };

    std::any plus(const std::any& v) const override { return {+std::any_cast<T>(v)}; };
    std::any minus(const std::any& v) const override { return {-std::any_cast<T>(v)}; };

    std::any max(const std::any& l, const std::any& r) const override
    {
      return {std::max(std::any_cast<T>(l), std::any_cast<T>(r))};
    }
    std::any min(const std::any& l, const std::any& r) const override
    {
      return {std::min(std::any_cast<T>(l), std::any_cast<T>(r))};
    }
    std::any add(const std::any& l, const std::any& r) const override
    {
      return {std::any_cast<T>(l) + std::any_cast<T>(r)};
    }
    std::any sub(const std::any& l, const std::any& r) const override
    {
      return {std::any_cast<T>(l) - std::any_cast<T>(r)};
    }
    std::any mult(const std::any& l, const std::any& r) const override
    {
      return {std::any_cast<T>(l) * std::any_cast<T>(r)};
    }
    std::any div(const std::any& l, const std::any& r) const override
    {
      return {std::any_cast<T>(l) / std::any_cast<T>(r)};
    }
    /*std::any mod(const std::any& l, const std::any& r) const override
    {
      return {std::any_cast<T>(l) % std::any_cast<T>(r)};
    }*/
    std::any pow(const std::any& l, const std::any& r) const override
    {
      return {std::pow(std::any_cast<T>(l), std::any_cast<T>(r))};
    }
  };

  struct type_erased_value
  {
    type_erased_value(const std::any& v, const abstract_value_set& vs);

    const std::any&       val() const;
    const std::type_info& tid() const;

    template <typename From, typename To>
    type_erased_value cast(abstract_value_set* new_vs = nullptr) const
    {
      assert(typeid(From) == tid());
      auto new_val = std::any{static_cast<To>(std::any_cast<From>(v_))};
      return {new_val, new_vs != nullptr ? *new_vs : *vs_};
    }

    type_erased_value max() const;
    type_erased_value min() const;
    type_erased_value sup() const;
    type_erased_value inf() const;

    type_erased_value plus() const;
    type_erased_value minus() const;

    type_erased_value max(const type_erased_value& rhs) const;
    type_erased_value min(const type_erased_value& rhs) const;
    type_erased_value add(const type_erased_value& rhs) const;
    type_erased_value sub(const type_erased_value& rhs) const;
    type_erased_value mult(const type_erased_value& rhs) const;
    type_erased_value div(const type_erased_value& rhs) const;
    /*type_erased_value mod(const type_erased_value& rhs) const; */
    type_erased_value pow(const type_erased_value& rhs) const;

  private:
    std::any                  v_;
    const abstract_value_set* vs_;
  };

  template <>
  struct value_set<type_erased_value> : abstract_value_set
  {
    value_set(const type_erased_value& v);
    ~value_set() override;

    template <typename From, typename To>
    std::any cast(const std::any& v, abstract_value_set* new_vs = nullptr) const
    {
      auto absv = std::any_cast<type_erased_value>(v);
      return {absv.template cast<From, To>(new_vs)};
    }

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
    const type_erased_value* v_;
  };
} // namespace mln
