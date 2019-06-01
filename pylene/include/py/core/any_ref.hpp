#pragma once

#include <py/core/any_ref_placeholder.hpp>

#include <any>
#include <memory>
#include <typeinfo>

namespace mln::py
{
  /*
  ** std::any has a dynamic allocation mechanism, hence a lack of performance.
  ** to fix this, we use the custom `any_ref` class, which is a reference to a value of `any` type
  */
  class any_ref
  {
    public:
      any_ref() = default;
      any_ref(std::any& elm);
      any_ref(void*& elm);
      void* data();

      template <typename T>
      T as()
      {
        if (dynamic_cast<placeholder<std::any>>(m_held))
        {
          return std::any_cast<T>(*dynamic_cast<std::any*>(&m_held));
        }
        return static_cast<T>(m_held);
      }

      template <typename T>
      any_ref(T& elm)
      {
        new (&m_held) placeholder<T>(elm);
      }

    private:
      std::aligned_storage_t<sizeof(placeholder_base), alignof(placeholder_base)> m_held;
  };
} // namespace mln::py
