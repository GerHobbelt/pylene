#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/neighborhood.hpp>
#include <mln/morpho/private/hvector_unbounded.hpp>

#include <array>
#include <memory>

namespace mln::morpho::details
{
  /// \brief Directional hierarchical queue
  /// \tparam I The image the edges are calculated from
  /// \tparam N The neighborhood used to compute the edges
  /// \tparam K Value type of the level (Should be unsigned integer of at least a size of 2)
  template <typename P, typename N, typename K>
  class directional_hqueue
  {
    static_assert(is_a_v<N, mln::details::Neighborhood>, "N should be a neighborhood");
    using queue_type = hvectors_unbounded<P>;

  public:
    /// \brief Constructor
    directional_hqueue() noexcept;

    /// \brief Insert an point in the directional hierarchical queue
    /// \param dir The hierarchical queue index (from after_offset() in N)
    /// \param w The weight of the edge
    /// \param p The point the edge weight is calculated from
    void insert(std::size_t dir, int w, P p) noexcept;

    /// \brief Return the tuple (p, q, w) where p and q are the vertex of the edge and w its weight
    std::tuple<P, P, int> pop() noexcept;

    /// \brief Return the lower weight of the queue
    int current_level() const noexcept;

    /// \brief Return true if the directional queue is empty
    bool empty() const noexcept;

  private:
    static_assert(std::is_integral_v<K> && std::is_unsigned_v<K>, "Key should be an unsigned integral type");
    static_assert(sizeof(K) <= 2, "Key should have a size of at least 2");

  private:
    static constexpr std::size_t                    m_ndir    = decltype(N::after_offsets())::extent;
    static constexpr int                            m_nlevels = 1 << std::numeric_limits<K>::digits;
    std::array<std::shared_ptr<queue_type>, m_ndir> m_queues;
    std::size_t                                     m_current_dir;
    int                                             m_current_level;
    std::size_t                                     m_size;
  };

  /******************
   * Implementation *
   ******************/

  template <typename P, typename N, typename K>
  directional_hqueue<P, N, K>::directional_hqueue() noexcept
    : m_current_dir(0)
    , m_current_level(0)
    , m_size(0)
  {
    for (std::size_t i = 0; i < m_ndir; i++)
      m_queues[i] = std::make_shared<queue_type>(m_nlevels);
  }

  template <typename P, typename N, typename K>
  void directional_hqueue<P, N, K>::insert(std::size_t dir, int w, P p) noexcept
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

  template <typename P, typename N, typename K>
  std::tuple<P, P, int>
  directional_hqueue<P, N, K>::pop() noexcept
  {
    assert(m_size > 0);
    const auto p = m_queues[m_current_dir]->pop_front(m_current_level);
    const auto q = p + N::after_offsets()[m_current_dir];
    const int  w = m_current_level;
    m_size--;

    // Update the current level and the current dir if needed
    if (m_size > 0 && m_queues[m_current_dir]->empty(m_current_level))
    {
      int         lvl = m_queues[0]->lower_bound(m_current_level);
      std::size_t dir = 0;
      for (std::size_t i = 1; i < m_ndir; i++)
      {
        int tmp = m_queues[i]->lower_bound(m_current_level);
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

  template <typename P, typename N, typename K>
  int directional_hqueue<P, N, K>::current_level() const noexcept
  {
    assert(m_size > 0);
    return m_current_level;
  }

  template <typename P, typename N, typename K>
  bool directional_hqueue<P, N, K>::empty() const noexcept
  {
    return m_size == 0;
  }

} // namespace mln::morpho::details