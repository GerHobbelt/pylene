#pragma once

#include <mln/core/concept/images.hpp>

#include <mln/core/assert.hpp>
#include <mln/core/algorithm/fill.hpp>

namespace  mln::morpho::experimental::details
{


  /// \brief Set of points ordered hierarchically into a simple list of points (only access to the front)
  ///
  /// \tparam P                 Type of point
  /// \tparam LinkImage         Type of the link image
  template <int N, class P, class LinkImage>
  class hsimple_linked_lists
  {
    static_assert(N <= (1 << 24), "Too many numbers of level");
    static_assert(std::is_convertible_v<P, image_point_t<LinkImage>>);

    template <class J>
    hsimple_linked_lists(J&& f);

    template <class J>
    hsimple_linked_lists(J&& f, P sentinel);


    void                push_front(int level, P p) noexcept;
    P                   pop_front(int level) noexcept;

    P                   front(int level) const noexcept;
    bool                empty(int level) const noexcept;

    /// Get the level 𝑙 of the lowest non-empty queue with level ≤ 𝑙
    /// such that every queue at level x with level ≤ x < 𝑙 are empty.
    int lower_bound(int level) const noexcept;

    /// Get the level 𝑙 of the highest non-empty queue with 𝑙 < level
    /// such that every queue at level x with  𝑙 < x ≤ level are empty.
    int upper_bound(int level) const noexcept;

  private:
    const P               m_sentinel;
    std::array<P, N>      m_heads;
    LinkImage             m_next;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <int N, class P, class LinkImage>
  template <class J>
  inline hsimple_linked_lists<N, P, LinkImage>::hsimple_linked_lists(J&& f, P sentinel)
    : m_sentinel{sentinel}
    , m_next(std::forward<J>(f), image_build_params{})
  {
    m_heads.fill(m_sentinel);
  }

  template <int N, class P, class LinkImage>
  template <class J>
  inline hsimple_linked_lists<N, P, LinkImage>::hsimple_linked_lists(J&& f)
    : hsimple_linked_lists{std::forward<J>(f), f.domain().br()}
  {
  }


  template <int N, class P, class LinkImage>
  inline bool hsimple_linked_lists<N, P, LinkImage>::empty(int level) const noexcept
  {
    mln_precondition(level < N);
    return m_heads[level] == m_sentinel;
  }


  template <int N, class P, class LinkImage>
  inline void hsimple_linked_lists<N, P, LinkImage>::push_front(int level, P p) noexcept
  {
    mln_precondition(level < N);
    m_next(p)            = m_heads[level];
    m_heads[level]       = p;
  }



  template <int N, class P, class LinkImage>
  inline P hsimple_linked_lists<N, P, LinkImage>::pop_front(int level) noexcept
  {
    mln_precondition(level < N);
    mln_precondition(m_heads[level] != m_sentinel && "Empty list");

    P head = m_heads[level];
    m_heads[level] = m_next(head);
    return head;
  }


  template <int N, class P, class LinkImage>
  inline P hsimple_linked_lists<N, P, LinkImage>::front(int level) const noexcept
  {
    mln_precondition(level < N);
    mln_precondition(m_heads[level].size > 0 && "Empty list");
    return m_heads[level].head;
  }


  template <int N, class P, class LinkImage>
  inline int hsimple_linked_lists<N, P, LinkImage>::lower_bound(int level) const noexcept
  {
    mln_precondition(level < N);

    while (level < N && m_heads[level] == m_sentinel)
      ++level;
    return level;
  }

  template <int N, class P, class LinkImage>
  inline int hsimple_linked_lists<N, P, LinkImage>::upper_bound(int level) const noexcept
  {
    mln_precondition(level < N);

    while (level >= 0 && m_heads[level] == m_sentinel)
      --level;
    return level;
  }




} // namespace mln::morpho::experimental::detail
