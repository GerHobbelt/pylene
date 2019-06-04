#include <py/core/any_ref.hpp>
#include <py/core/any_ref_placeholder.hpp>

#include <any>

namespace mln::py
{
  any_ref::any_ref(std::any& elm) { new (&m_held) placeholder<std::any>(elm); }

  void* any_ref::data() { return reinterpret_cast<placeholder_base*>(&m_held)->m_data; }

  const std::type_info& any_ref::type() const { return reinterpret_cast<const placeholder_base*>(&m_held)->type(); }
} // namespace mln::py
