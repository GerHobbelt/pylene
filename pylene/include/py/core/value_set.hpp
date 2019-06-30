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

    template <typename Dest_type>
    Dest_type cast(any_ref v) const
    {
        return std::any_cast<Dest_type>(_cast(v, Trait<Dest_type>::value));
    }

  private:
    virtual std::any _max() const                             = 0;
    virtual std::any _cast(any_ref v, const Info& dest_type) const = 0;
  };

  template <typename T>
  struct value_set : value_set<void>
  {

    template <typename U>
    U cast(any_ref v) const
    {
      return static_cast<U>(v.as<T>());
    }

    template <typename U>
    U cast(T v) const
    {
      return static_cast<U>(v);
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

    struct cast_dispatcher_t
    {
      explicit cast_dispatcher_t(const value_set* vs)
        : m_vs(vs)
      {
      }

      template <typename Dest_type>
      std::any call(any_ref v)
      {
        return m_vs->template cast<Dest_type>(v);
      }

    private:
      const value_set<T>* m_vs;
    };

    std::any _cast(any_ref val, const Info& dest_type) const override final
    {
      return visit_f(dest_type.tid(), cast_dispatcher_t{this}, val);
    }
  }; // value_set
} // namespace mln::py
