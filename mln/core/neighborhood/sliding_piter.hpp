#ifndef MLN_CORE_NEIGHBORHOOD_SLIDING_PITER_HPP
#define MLN_CORE_NEIGHBORHOOD_SLIDING_PITER_HPP

#include <mln/core/iterator/iterator_base.hpp>
#include <mln/core/range/range.hpp>
#include <mln/core/utils/wrapper.hpp>

namespace mln
{

  /// \brief Define an iterator other a siteset centered on a site.
  ///
  /// Define an iterator other a siteset centered on a site.
  ///
  /// \p Point can be a plain type, in that case, the current iterator will
  /// be binded to the point.
  /// \p Point can be an iterator, in that case, the current iterator will
  /// be binded to the iterator.
  ///
  /// Note that the siteset is not copied and its lifetime should be longer that
  /// the iterator's one.
  ///
  /// \tparam Point
  /// \tparam SiteSet
  template <class Point, class SiteSet>
  struct sliding_piter;

  template <class Point, class SiteSet>
  sliding_piter<Point, SiteSet> make_sliding_piter(const Point& p, const SiteSet& s);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class PointProxy, class SiteSet>
  struct sliding_piter
      : iterator_base<sliding_piter<PointProxy, SiteSet>,
                      typename SiteSet::value_type,
                      typename SiteSet::value_type>
  {
    using reference = typename SiteSet::value_type;

    sliding_piter(const PointProxy& p, const SiteSet& s) : m_set(s), m_point(p), m_it(rng::iter(m_set)) {}

    sliding_piter(const sliding_piter& other)
        : m_set(other.m_set), m_point(other.m_point), m_it(rng::iter(m_set))
    {
    }


    sliding_piter(sliding_piter&& other)
        : m_set(std::move(other.m_set)), m_point(std::move(other.m_point)), m_it(rng::iter(m_set))
    {
    }

    sliding_piter& operator= (const sliding_piter& other)
    {
      m_set = other.m_set;
      m_point = other.m_point;
      m_it = rng::iter(m_set);
    }

    sliding_piter& operator= (sliding_piter&& other)
    {
      m_set = std::move(other.m_set);
      m_point = std::move(other.m_point);
      m_it = rng::iter(m_set);
    }


    void init() { m_it.init(); }
    void next() { m_it.next(); }
    bool finished() const { return m_it.finished(); }
    reference dereference() const {  return m_point.get() + *m_it; }

  private:
    SiteSet m_set;
    const PointProxy m_point;
    typename range_const_iterator<SiteSet>::type m_it;
  };

  template <class PointProxy, class SiteSet>
  sliding_piter<PointProxy, SiteSet> make_sliding_piter(const PointProxy& p, const SiteSet& s)
  {
    return {p, s};
  }
}

#endif // ! MLN_CORE_NEIGHBORHOOD_SLIDING_PITER_HPP
