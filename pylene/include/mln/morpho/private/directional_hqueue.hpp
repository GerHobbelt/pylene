#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/morpho/private/hvector_unbounded.hpp>

#include <array>
#include <memory>

namespace mln::morpho::details
{
  /// \brief Directional hierarchical queue
  /// \tparam I The image the edges are calculated from
  /// \tparam N The neighborhood used to compute the edges
  /// \tparam F The distance function
  template <typename I, typename N, typename F>
  class directional_hqueue
  {
    static_assert(is_a_v<I, mln::details::Image>, "I should be an image");
    static_assert(is_a_v<N, mln::details::Neighborhood>, "N should be a neighborhood");
    using V = image_value_t<I>;
    static_assert(std::is_invocable_v<F, V, V>, "F must be invocable with the value of I");
    using Key        = std::invoke_result_t<F, V, V>;
    using Point      = image_point_t<I>;
    using queue_type = hvectors_unbounded<Point>;

  public:
    /// \brief Constructor
    directional_hqueue() noexcept;

    /// \brief Insert an point in the directional hierarchical queue
    /// \param dir The hierarchical queue index (from after_offset() in N)
    /// \param w The weight of the edge
    /// \param p The point the edge weight is calculated from
    void insert(std::size_t dir, Key w, Point p) noexcept;

    /// \brief Return the tuple (p, q, w) where p and q are the vertex of the edge and w its weight
    std::tuple<Point, Point, Key> pop() noexcept;

    /// \brief Return the lower weight of the queue
    Key current_level() const noexcept;

    /// \brief Return true if the directional queue is empty
    bool empty() const noexcept;

  private:
    static_assert(std::is_integral_v<Key> && std::is_unsigned_v<Key>, "Key should be an unsigned integral type");
    static_assert(sizeof(Key) <= 2, "Key should have a size of at least 2");

  private:
    static constexpr std::size_t               m_ndir    = N::after_offsets().size();
    static constexpr std::size_t               m_nlevels = 1 << std::numeric_limits<Key>::digits;
    std::array<std::shared_ptr<queue_type>, 2> m_queues;
    std::size_t                                m_current_dir;
    Key                                        m_current_level;
    std::size_t                                m_size;
  };

  /******************
   * Implementation *
   ******************/

  template <typename I, typename N, typename F>
  directional_hqueue<I, N, F>::directional_hqueue() noexcept
    : m_current_dir(0)
    , m_current_level(0)
    , m_size(0)
  {
    for (std::size_t i = 0; i < m_ndir; i++)
      m_queues[i] = std::make_shared<queue_type>(m_nlevels);
  }

  template <typename I, typename N, typename F>
  void directional_hqueue<I, N, F>::insert(std::size_t dir, Key w, Point p) noexcept
  {
    // Update the current level and the current dir to keep the queue sorted
    if (m_size == 0 || w < m_current_level)
    {
      m_current_dir   = dir;
      m_current_level = w;
    }
    m_queues[dir]->push_front(w, p);
    m_size++;
  }

  template <typename I, typename N, typename F>
  std::tuple<typename directional_hqueue<I, N, F>::Point, typename directional_hqueue<I, N, F>::Point,
             typename directional_hqueue<I, N, F>::Key>
  directional_hqueue<I, N, F>::pop() noexcept
  {
    assert(m_size > 0);
    const auto p = m_queues[m_current_dir]->pop_front(m_current_level);
    const auto q = p + N::after_offsets()[m_current_dir];
    const auto w = m_current_level;
    m_size--;

    // Update the current level and the current dir if needed
    if (m_size > 0 && m_queues[m_current_dir]->empty(m_current_level))
    {
      std::size_t lvl = m_queues[0]->lower_bound(m_current_level);
      std::size_t dir = 0;
      for (std::size_t i = 1; i < m_ndir; i++)
      {
        std::size_t tmp = m_queues[i]->lower_bound(m_current_level);
        if (tmp < lvl && tmp < m_nlevels)
        {
          lvl = tmp;
          dir = i;
        }
      }
      m_current_level = lvl;
      m_current_dir   = dir;
    }

    return {p, q, w};
  }

  template <typename I, typename N, typename F>
  typename directional_hqueue<I, N, F>::Key directional_hqueue<I, N, F>::current_level() const noexcept
  {
    assert(m_size > 0);
    return m_current_level;
  }

  template <typename I, typename N, typename F>
  bool directional_hqueue<I, N, F>::empty() const noexcept
  {
    return m_size == 0;
  }

} // namespace mln::morpho::details