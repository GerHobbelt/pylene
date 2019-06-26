#include <py/core/any_ref.hpp>
#include <py/core/any_ref_placeholder.hpp>

#include <any>

namespace mln::py
{
  struct cast_dispatcher_t
  {
    template <typename U> // U return type
    void call(void* held, std::byte* ptr)
    {
      new (held) placeholder<U>(reinterpret_cast<U*>(ptr));
    }
  };

  any_ref::any_ref(std::byte* ptr, Info i)
  {
    visit_f(i.tid(), cast_dispatcher_t(), reinterpret_cast<void*>(&m_held), ptr);
  }

  any_ref::any_ref(std::any& elm) { new (&m_held) placeholder<std::any>(elm); }

  void* any_ref::data() { return reinterpret_cast<placeholder_base*>(&m_held)->m_data; }

  const std::type_info& any_ref::type() const { return reinterpret_cast<const placeholder_base*>(&m_held)->type(); }
} // namespace mln::py
