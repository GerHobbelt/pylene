#pragma once

#include <py/core/any_ref.hpp>
#include <py/core/type_info.hpp>

#include <any>
#include <iostream>
#include <limits>


namespace mln::py
{

  template <typename T = void>
  struct value_set;

  template <>
  struct value_set<void>
  {
    std::any         max() const { return _max(); }
    virtual std::any divide(any_ref val1, any_ref val2) const = 0;

    std::any cast(any_ref v, const Info& dest_type) const { return _cast(v, dest_type); }

  private:
    virtual std::any _max() const                             = 0;
    virtual std::any _cast(any_ref v, const Info& info) const = 0;
  };

  template <typename T>
  struct value_set : value_set<void>
  {

    template <typename U>
    U cast(any_ref v) const
    {
      return static_cast<U>(v.as<T>());
    }


    T max() const { return std::numeric_limits<T>::max(); }

    std::any divide(any_ref val1, any_ref denominator) const override final
    {
      return val1.as<T>() / denominator.as<T>();
    }


    T divide(T val, T denominator) const
    {
      return val / denominator;
    }

  private:
    std::any _max() const override final { return max(); }

    template <typename VT>
    struct cast_dispatcher_t
    {
      explicit cast_dispatcher_t(const value_set<VT>* vs)
        : m_vs(vs)
      {
      }
      template <typename U> // U return type
      std::any call(any_ref v)
      {
        return m_vs->template cast<U>(v);
      }

    private:
      const value_set<T>* m_vs;
    };

    std::any _cast(any_ref val, const Info& info) const override final
    {
      return visit_f(info.tid(), cast_dispatcher_t{this}, val);
    }
  }; // value_set

  template <typename T, typename Arg>
  T generic_cast(Arg&& val)
  {
    return static_cast<T>(std::forward<Arg>(val));
  }

  template <typename T>
  T generic_cast(any_ref val)
  {
    return val.as<T>();
  }

  template <typename T> //TODO constness of any?
  T generic_cast(std::any val)
  {
    return std::any_cast<T>(val);
  }
} // namespace mln::py
