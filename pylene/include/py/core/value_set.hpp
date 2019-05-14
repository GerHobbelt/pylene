#pragma once

#include <any>
#include <limits>
#include <iostream>

namespace mln::py
{

  template <typename T = void>
  struct value_set;

  template <>
  struct value_set<void>
  {
    virtual float normalize(void* val) const = 0; //abstract normalize function for vs<void>
    virtual std::any max() const = 0;
    virtual std::any divide(void* val, std::any any) const = 0;
    virtual std::any divide(void* val, void* any) const = 0;
  };

  template <typename T>
  struct value_set : value_set<void>
  {
    float normalize(void* val) const override final
    {
      return static_cast<float>(*static_cast<T*>(val)) / std::numeric_limits<T>::max();
    }

    float normalize(T val) const { return static_cast<float>(val) / std::numeric_limits<T>::max(); }

    std::any max() const override final
    {
        return std::any(std::numeric_limits<T>::max());
    }

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

    std::any divide(void* val, void *denominator) const override final
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
  };
} // namespace mln::py