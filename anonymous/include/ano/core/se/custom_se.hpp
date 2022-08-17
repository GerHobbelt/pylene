#pragma once

#include <ano/core/se/private/se_facade.hpp>

/// \file

namespace ano::se
{

  /// \brief Define a custom neighborhood from a range of delta points.
  template <class R>
  struct custom_se : se_facade<custom_se<R>>
  {
    using category     = dynamic_neighborhood_tag;
    using separable    = std::false_type;
    using decomposable = std::false_type;
    using incremental  = std::false_type;

    /// \brief Constructs a SE from the set of delta points \p rng
    custom_se(R rng, int radial_extent)
      : m_rng{std::move(rng)}
      , m_radial_extent{radial_extent}
    {
    }

    const R& offsets() const { return m_rng; }
    const R& before_offsets() const { return m_rng; }
    const R& after_offsets() const { return m_rng; }
    int      radial_extent() const { return m_radial_extent; }

  private:
    R   m_rng;
    int m_radial_extent;
  };

} // namespace ano::se
