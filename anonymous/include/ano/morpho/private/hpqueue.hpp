#pragma once

#include <ano/morpho/private/hlinked_lists.hpp>

namespace ano::morpho::detail
{

  /// \class hpqueue
  /// \brief Hierarchical Priority Queue
  /// Set of points ordered by increasing priority (highest first by default, lowest first if reverse=true) into a list
  /// of points
  ///
  /// \tparam N                 Number of levels
  /// \tparam P                 Type of point
  /// \tparam LinkImage         Type of the link image
  template <int N, class P, class Impl, bool reverse>
  class hpqueue : private Impl
  {
    using base = Impl;

  public:
    template <class J>
    hpqueue(J&& f)
      : base(std::forward<J>(f))
    {
    }

    ~hpqueue() = default;

    // Push in last position at given level
    void push_last(int level, P p) noexcept
    {
      ano_precondition(0 <= level && level < N);

      base::push_back(level, p);
      if constexpr (reverse)
        m_current_level = std::min(m_current_level, level);
      else
        m_current_level = std::max(m_current_level, level);
    }


    // Push in first position at givel level
    // Not supported by all implementation
    /*
    void push_first(int level, P p) noexcept
    {
      ano_precondition(0 <= level && level < N);

      base::push_front(level, p);
      if constexpr (reverse)
        m_current_level = std::min(m_current_level, level);
      else
        m_current_level = std::max(m_current_level, level);
    }
    */

    bool has_key(int level) const noexcept { return !base::empty(level); }
    bool empty() const noexcept { return base::empty(m_current_level); }

    std::pair<int, P> top() const noexcept { return {m_current_level, base::front(m_current_level)}; }

    void pop() noexcept;

  private:
    int m_current_level = reverse ? (N - 1) : 0;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <int N, class P, class LinkImage, bool reverse>
  inline void hpqueue<N, P, LinkImage, reverse>::pop() noexcept
  {
    base::pop_front(m_current_level);
    if (!base::empty(m_current_level))
      return;

    // Try to go down
    if constexpr (reverse)
    {
      while (base::empty(m_current_level) && m_current_level < (N - 1))
        m_current_level++;
    }
    else
    {
      while (base::empty(m_current_level) && m_current_level > 0)
        m_current_level--;
    }
  }


} // namespace ano::morpho::detail
