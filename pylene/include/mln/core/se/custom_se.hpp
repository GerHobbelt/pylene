#pragma once

#include <mln/core/se/private/se_facade.hpp>

/// \file

namespace mln
{

  /// \brief Define a custom neighborhood from a range of delta points.
  template <class R>
  struct custom_se : se_facade<custom_se<R>>
  {
    using category = dynamic_neighborhood_tag;
    using separable = std::false_type;
    using decomposable = std::false_type;
    using incremental = std::false_type;

    /// \brief Constructs a SE from the set of delta points \p rng
    custom_se(R rng)
      : m_rng{std::move(rng)}
    {
    }

    const R& offsets() const { return m_rng; }
    const R& before_offsets() const { return m_rng; }
    const R& after_offsets() const { return m_rng; }

  private:
    R m_rng;
  };

}

