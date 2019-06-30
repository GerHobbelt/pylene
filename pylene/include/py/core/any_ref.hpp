#pragma once

#include <py/core/any_ref_placeholder.hpp>
#include <py/core/type_info.hpp>

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
    any_ref(const any_ref& other) = default;
    any_ref(any_ref&& other) noexcept = default;
    any_ref(std::byte* buf, Info i);

    template <typename T, class = std::enable_if_t<!std::is_same_v<any_ref, T>>>
    any_ref(T& elm)
    {
      new (&m_held) placeholder<T>(elm);
    }

    void*                 data();
    const std::type_info& type() const;

    template <typename T>
    T as()
    {
      placeholder_base* tmp = reinterpret_cast<placeholder_base*>(&m_held);
      if (placeholder<std::any>* ptr = dynamic_cast<placeholder<std::any>*>(tmp))
      {
        return std::any_cast<T>(*static_cast<std::any*>(ptr->m_data));
      }
      else if (placeholder<T>* ptr = dynamic_cast<placeholder<T>*>(tmp))
      {
        return *static_cast<T*>(ptr->m_data);
      }
      else
      {
        throw std::bad_cast();
      }
    }

  private:
    std::aligned_storage_t<sizeof(placeholder_base), alignof(placeholder_base)> m_held;
  };
} // namespace mln::py
