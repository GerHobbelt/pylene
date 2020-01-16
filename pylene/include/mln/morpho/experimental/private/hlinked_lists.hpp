#pragma once

#include <mln/core/concept/new/images.hpp>

#include <mln/core/assert.hpp>

namespace  mln::morpho::experimental::details
{


  /// \brief Set of points ordered hierarchically into a list of points
  ///
  /// \tparam N                 Number of levels
  /// \tparam P                 Type of point
  /// \tparam LinkImage         Type of the link image
  template <int N, class P, class LinkImage>
  class hlinked_lists
  {
    static_assert(N <= (1 << 24), "Too many numbers of level");
    static_assert(std::is_convertible_v<P, image_point_t<LinkImage>>);
  public:
    template <class J>
    hlinked_lists(J&& f);


    void                push_front(int level, P p) noexcept;
    void                push_back(int level, P p) noexcept;
    P                   pop_front(int level) noexcept;

    P                   back(int level) const noexcept;
    P                   front(int level) const noexcept;
    bool                empty(int level) const noexcept;

    /// Get the level 𝑙 of the lowest non-empty queue with level ≤ 𝑙
    /// such that every queue at level x with level ≤ x < 𝑙 are empty.
    int lower_bound(int level) const noexcept;

    /// Get the level 𝑙 of the highest non-empty queue with 𝑙 < level
    /// such that every queue at level x with  𝑙 < x ≤ level are empty.
    int upper_bound(int level) const noexcept;

  private:
    struct node_t
    {
      P    head;
      P    tail;
      int  size = 0;
    };

    std::array<node_t, N> m_lists;
    LinkImage             m_next;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <int N, class P, class LinkImage>
  template <class J>
  inline hlinked_lists<N, P, LinkImage>::hlinked_lists(J&& f)
    : m_next(std::forward<J>(f), image_build_params{})
  {
  }

  template <int N, class P, class LinkImage>
  inline bool hlinked_lists<N, P, LinkImage>::empty(int level) const noexcept
  {
    mln_precondition(level < N);
    return m_lists[level].size == 0;
  }


  template <int N, class P, class LinkImage>
  inline void hlinked_lists<N, P, LinkImage>::push_front(int level, P p) noexcept
  {
    mln_precondition(level < N);
    if (m_lists[level].size == 0)
    {
      m_lists[level].head  = p;
      m_lists[level].tail  = p;
    }
    else
    {
      m_next(p)            = m_lists[level].head;
      m_lists[level].head  = p;
    }

    m_lists[level].size++;
  }


  template <int N, class P, class LinkImage>
  inline void hlinked_lists<N, P, LinkImage>::push_back(int level, P p) noexcept
  {
    mln_precondition(level < N);
    if (m_lists[level].size == 0)
    {
      m_lists[level].head  = p;
      m_lists[level].tail  = p;
    }
    else
    {
      m_next(m_lists[level].tail) = p;
      m_lists[level].tail         = p;
    }
    m_lists[level].size++;
  }

  template <int N, class P, class LinkImage>
  inline P hlinked_lists<N, P, LinkImage>::pop_front(int level) noexcept
  {
    mln_precondition(level < N);
    mln_precondition(m_lists[level].size > 0 && "Empty list");
    P head = m_lists[level].head;
    m_lists[level].head = m_next(head);
    m_lists[level].size--;

    return head;
  }


  template <int N, class P, class LinkImage>
  inline P hlinked_lists<N, P, LinkImage>::front(int level) const noexcept
  {
    mln_precondition(level < N);
    mln_precondition(m_lists[level].size > 0 && "Empty list");
    return m_lists[level].head;
  }

  template <int N, class P, class LinkImage>
  inline P hlinked_lists<N, P, LinkImage>::back(int level) const noexcept
  {
    mln_precondition(level < N);
    mln_precondition(m_lists[level].size > 0 && "Empty list");
    return m_lists[level].tail;
  }

  template <int N, class P, class LinkImage>
  inline int hlinked_lists<N, P, LinkImage>::lower_bound(int level) const noexcept
  {
    mln_precondition(level < N);

    while (level < N && m_lists[level].size == 0)
      ++level;
    return level;
  }

  template <int N, class P, class LinkImage>
  inline int hlinked_lists<N, P, LinkImage>::upper_bound(int level) const noexcept
  {
    mln_precondition(level < N);

    while (level >= 0 && m_lists[level].size == 0)
      --level;
    return level;
  }




} // namespace mln::morpho::experimental::detail
