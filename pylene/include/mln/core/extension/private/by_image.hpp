#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/image/image.hpp>

#include <type_traits>
#include <utility>


namespace mln::extension
{

  template <typename U, typename V>
  struct by_image
  {
    using value_type        = V;
    using support_fill      = std::false_type;
    using support_mirror    = std::false_type;
    using support_periodize = std::false_type;
    using support_clamp     = std::false_type;
    using is_finite         = std::true_type;

    explicit by_image(U base_ima)
      : m_baseima{std::move(base_ima)}
    {
    }

    template <typename SE>
    bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");
      // TODO: non-trivial
      return true;
    }
    const V& value(image_point_t<U> pnt) const
    {
      if (!m_baseima.domain().has(pnt))
        throw std::runtime_error(" Accessing point out of bound !");

      return m_baseima(pnt);
    }

  private:
    U m_baseima;
  };

} // namespace mln::extension
