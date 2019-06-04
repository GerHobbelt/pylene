#pragma once

#include <py/core/any_ref.hpp>
#include <py/core/type_info.hpp>

#include <any>
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
    virtual std::any divide(void* val, std::any any) const = 0;
    virtual std::any divide(void* val, void* any) const    = 0;

    std::any cast(any_ref v, const Info& info) const { return _cast(v, info); }

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


    std::any max() const { return std::any(std::numeric_limits<T>::max()); }

    std::any divide(void* val, std::any denominator) const override final
    {
      std::any res;
      try
      {
        res = static_cast<float>(*static_cast<T*>(val)) / std::any_cast<T>(denominator);
      }
      catch (const std::bad_any_cast& e)
      {
        std::cerr << "Error in 'divide(void*, std::any)': unhandled type.\n";
      }
      return res;
    }

    std::any divide(void* val, void* denominator) const override final
    {
      std::any res;
      try
      {
        res = static_cast<float>(*static_cast<T*>(val)) / *static_cast<T*>(denominator);
      }
      catch (const std::bad_any_cast& e)
      {
        std::cerr << "Error in 'divide(void*, void*)': unhandled type.\n";
      }
      return res;
    }

    std::any divide(T val, std::any denominator) const
    {
      std::any res;
      try
      {
        res = static_cast<float>(val) / std::any_cast<T>(denominator);
      }
      catch (const std::bad_any_cast& e)
      {
        std::cerr << "Error in 'divide(T, std::any)': unhandled type.\n";
      }
      return res;
    }


    std::any divide(T val, T denominator) const
    {
      std::any res;
      try
      {
        res = static_cast<float>(val) / denominator;
      }
      catch (const std::bad_any_cast& e)
      {
        std::cerr << "Error in 'divide(T, T)': unhandled type.\n";
      }
      return res;
    }

  private:
    std::any _max() const override final { return max(); }

    template <typename T>
    struct cast_dispatcher_t
    {
      explicit cast_dispatcher_t(const value_set<T>* vs)
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
      return visit_f(info, cast_dispatcher_t{this}, val);
    }
  }; // namespace mln::py
} // namespace mln::py
