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
    virtual float    normalize(void* val) const = 0; // abstract normalize function for vs<void>
    std::any         max() const { return _max(); }
    virtual float divide(any_ref val1, any_ref val2) const = 0;

    std::any cast(any_ref v, const Info& dest_type) const { return _cast(v, dest_type); }

  private:
    virtual std::any _max() const                             = 0;
    virtual std::any _cast(any_ref v, const Info& info) const = 0;
  };

  template <typename T>
  struct value_set : value_set<void>
  {
    float normalize(void* val) const override final
    {
      return static_cast<float>(*static_cast<T*>(val)) / std::numeric_limits<T>::max();
    }

    float normalize(T val) const { return static_cast<float>(val) / std::numeric_limits<T>::max(); }

    template <typename U>
    U cast(any_ref v) const
    {
      return static_cast<U>(v.as<T>());
    }


    T max() const { return std::numeric_limits<T>::max(); }

    float divide(any_ref val1, any_ref denominator) const override final
    {
      return static_cast<float>(val1.as<T>()) / denominator.as<T>();
    }


    float divide(T val, T denominator) const
    {
      return static_cast<float>(val) / denominator;
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
  }; // namespace mln::py
} // namespace mln::py
