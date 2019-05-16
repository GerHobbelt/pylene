#pragma once

#include <mln/core/concept/new/domains.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>

#include <type_traits>
#include <utility>


namespace mln::extension
{

  namespace detail
  {
    template <class T>
    struct remove_cvref
    {
      using type = std::remove_cv_t<std::remove_reference_t<T>>;
    };
  } // namespace detail

  template <typename V>
  struct by_pattern
  {
    enum class Pattern
    {
      Mirror,
      Periodize,
      Clamp
    };

    using value_type        = V;
    using support_fill      = std::false_type;
    using support_mirror    = std::true_type;
    using support_periodize = std::true_type;
    using support_clamp     = std::true_type;
    using is_finite         = std::false_type;

    by_pattern(Pattern p, std::size_t padding = 0)
      : m_pattern(p)
      , m_padding(padding)
    {
    }

    template <typename SE>
    bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    void mirror(std::size_t padding)
    {
      m_pattern = Pattern::Mirror;
      m_padding = padding;
    }

    void periodize() { m_pattern = Pattern::Periodize; }

    void clamp() { return m_pattern = Pattern::Clamp; }

    Pattern pattern() const { return m_pattern; }

    std::size_t padding() const { return m_padding; }

    template <typename Pnt, typename Ima>
    const V& yield(Pnt pnt, const Ima& ima) const
    {
      switch (m_pattern)
      {
      case Pattern::Mirror:
        return yield_mirror(std::move(pnt), ima);
      case Pattern::Periodize:
        return yield_periodize(std::move(pnt), ima);
      case Pattern::Clamp:
        return yield_clamp(std::move(pnt), ima);
      }
    }

  private:
    template <typename Pnt, typename Ima>
    const V& yield_mirror(Pnt pnt, const Ima& ima) const
    {
      auto dom = ima.domain();

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<detail::remove_cvref<decltype(dom)>>,
                               "Domain must be shaped to allow pattern-based extension!");

      (void)pnt;
    }

    template <typename Pnt, typename Ima>
    const V& yield_periodize(Pnt pnt, const Ima& ima) const
    {
      auto dom = ima.domain();

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<detail::remove_cvref<decltype(dom)>>,
                               "Domain must be shaped to allow pattern-based extension!");

      (void)pnt;
    }

    template <typename Pnt, typename Ima>
    const V& yield_clamp(Pnt pnt, const Ima& ima) const
    {
      auto dom = ima.domain();

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<detail::remove_cvref<decltype(dom)>>,
                               "Domain must be shaped to allow pattern-based extension!");

      (void)pnt;
    }

    Pattern     m_pattern;
    std::size_t m_padding;
  };

} // namespace mln::extension
