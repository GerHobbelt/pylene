#pragma once

#include <mln/core/concept/new/se.hpp>

#include <type_traits>
#include <utility>


namespace mln::extension
{

  template <typename V>
  struct by_value
  {
    using value_type = V;
    using is_finite  = std::false_type;

    by_value(V val)
      : m_val(std::move(val))
    {
    }

    template <typename SE>
    bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    void     fill(V val) { m_val = std::move(val); }
    const V& value() const { return m_val; }

  private:
    V m_val;
  };

} // namespace mln::extension
