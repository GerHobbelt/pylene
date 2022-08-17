#pragma once

#include <ano/core/concepts/image.hpp>

#include <array>
#include <limits>
#include <type_traits>

namespace ano::morpho::experimental::details
{

  template <class I>
  class pqueue_hqueue_fifo
  {
    using key_type   = image_value_t<I>;
    using value_type = image_point_t<I>;

    static_assert(std::is_integral_v<key_type>);
    static_assert(!std::is_signed<key_type>::value, "Key type must be unsigned.");
    static_assert(sizeof(key_type) <= 2, "Only low-quantized value supported.");
    static_assert(sizeof(key_type) < sizeof(int), "Unsupported architecure.");


  public:
    template <class J>
    pqueue_hqueue_fifo(J&& f);

    void                            push(const key_type& k, const value_type& v);
    void                            pop();
    std::pair<key_type, value_type> top() const;
    bool                            empty() const;

  private:
    static constexpr int nvalues = std::numeric_limits<key_type>::max() + 1;
    struct node_t
    {
      int        size = 0;
      value_type head;
      value_type tail;
    };

    // Pointer to head and tail element in the queue
    std::array<node_t, nvalues> m_queues;

    // Next pointer image
    image_ch_value_t<I, value_type> m_next;

    int m_current_level;
  };

  template <class I>
  template <class J>
  inline pqueue_hqueue_fifo<I>::pqueue_hqueue_fifo(J&& f)
    : m_next(std::forward<J>(f), image_build_params{})
    , m_current_level{std::numeric_limits<key_type>::max()}
  {
  }

  template <class I>
  inline void pqueue_hqueue_fifo<I>::pop()
  {
    ano_precondition(m_queues[m_current_level].size > 0);

    auto& head = m_queues[m_current_level].head;
    head       = m_next(head);
    m_queues[m_current_level].size--;

    if (m_queues[m_current_level].size > 0)
      return;

    // Try to go up
    {
      do
      {
        m_current_level++;
      } while (m_current_level < std::numeric_limits<key_type>::max() && m_queues[m_current_level].size == 0);
    }
  }

  template <class I>
  inline void pqueue_hqueue_fifo<I>::push(const key_type& k, const value_type& v)
  {
    if (m_queues[k].size == 0)
    {
      m_queues[k].head = v;
      m_queues[k].tail = v;
    }
    else
    {
      m_next(m_queues[k].tail) = v;
      m_queues[k].tail         = v;
    }
    m_queues[k].size++;

    if (k < m_current_level)
      m_current_level = k;
  }

  template <class I>
  inline auto pqueue_hqueue_fifo<I>::top() const -> std::pair<key_type, value_type>
  {
    ano_precondition(m_queues[m_current_level].size > 0);
    return std::make_pair(static_cast<key_type>(m_current_level), m_queues[m_current_level].head);
  }

  template <class I>
  inline bool pqueue_hqueue_fifo<I>::empty() const
  {
    return m_queues[m_current_level].size == 0;
  }

} // namespace ano::morpho::experimental::details
