#include <py/core/any_ref_placeholder.hpp>

#include <typeinfo>

namespace mln::py
{
    const std::type_info& placeholder_base::type() const
    {
      return typeid(placeholder_base);
    }
}
