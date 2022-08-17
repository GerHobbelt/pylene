#pragma once


#include <ano/morpho/experimental/private/hsimple_linked_lists.hpp>
#include <ano/morpho/experimental/private/hvector_unbounded.hpp>
#include <limits>
#include <optional>

namespace ano::morpho::experimental::details
{

  /// \brief A point set allows to keep a ordered set of points or indexes
  /// sorted by a given key (generally the pixel value)
  /// For now, it only supports indexes
  ///
  /// \warning For two entries with identical keys (levels), this data structure acts
  /// like a LIFO (last in first out)
  template <class I>
  class pset
  {
  public:
    using Key   = image_value_t<I>;
    using Point = image_point_t<I>;

    template <class J>
    pset(J&& ima);

    /// \brief Insert a point \p p at the given \p level
    void insert(int level, Point p) noexcept;

    /// \brief Return and remove the closest level pair (key,point) such that:
    /// * there is a pair (l₁,p₁) in the set with l₁ ≥ level then (l₁,p₁)
    /// * there is a pair (l₁,p₂) in the set with l₂ < level then (l₂,p₂)
    std::pair<int, Point> pop(int level) noexcept;

    /// \brief Return and remove the point if the queue is not empty
    std::optional<Point> try_pop(int level) noexcept;

    /// \brief Return true if the set is empty
    bool empty() const noexcept;

  private:
    static_assert(std::is_integral_v<Key>);
    static_assert(!std::numeric_limits<Key>::is_signed);
    static_assert(std::numeric_limits<Key>::digits <= 16);
    static constexpr int nLevel = 1 << std::numeric_limits<Key>::digits;

    // using link_image_t = image_ch_value_t<I, Point>;
    // using delegate_t = hsimple_linked_lists<nLevel, Point, link_image_t>;
    using delegate_t = hvectors_unbounded<Point>;

    delegate_t  m_delegate;
    std::size_t m_size;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  template <class J>
  pset<I>::pset(J&&)
    : m_delegate{nLevel}
    , m_size{0}
  {
  }

  template <class I>
  void pset<I>::insert(int level, Point p) noexcept
  {
    m_delegate.push_front(level, p);
    m_size++;
  }

  template <class I>
  auto pset<I>::pop(int level) noexcept -> std::pair<int, Point>
  {
    assert(m_size > 0);
    int l = m_delegate.lower_bound(level);
    if (l == nLevel)
    {
      l = m_delegate.upper_bound(level);
      assert(l >= 0);
    }
    m_size--;
    return {l, m_delegate.pop_front(l)};
  }

  template <class I>
  auto pset<I>::try_pop(int level) noexcept -> std::optional<Point>
  {
    if (m_delegate.empty(level))
      return std::nullopt;
    m_size--;
    return m_delegate.pop_front(level);
  }


  template <class I>
  bool pset<I>::empty() const noexcept
  {
    return m_size == 0;
  }


} // namespace ano::morpho::experimental::details
