#include <py/core/any_ref.hpp>

namespace mln
{
  any_ref::any_ref(void*& elm)
  {
    m_data = elm;
  }

  any_ref::any_ref(std::any& elm)
  {
    m_data = &elm;
  }

  void* any_ref::data()
  {
    return m_data;
  }
} // namespace mln
