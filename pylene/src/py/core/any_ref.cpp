#include <py/core/any_ref.hpp>
#include <py/core/any_ref_placeholder.hpp>

#include <any>

namespace mln::py
{
  any_ref::any_ref(std::any& elm)
  {
    new (&m_held) placeholder<std::any&>(elm);
  }

  void* any_ref::data()
  {
    return static_cast<void*>(&m_held);
  }
} // namespace mln::py
