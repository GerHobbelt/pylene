#pragma once

#include <mln/core/concept/new/images.hpp>

#include <mln/morpho/experimental/private/pqueue_hqueue_fifo.hpp>


namespace mln::morpho::experimental::details
{

  /// Provides a priority queue of points with a the fifo property
  template <class I>
  class pqueue_fifo
  {
    using key_type   = image_value_t<I>;
    using value_type = image_point_t<I>;

  public:
    template <class J>
    pqueue_fifo(J&& f);

    void                            push(const key_type& priority, const value_type& element);
    void                            pop();
    std::pair<key_type, value_type> top() const;
    bool                            empty() const;

  private:
    static_assert(value_traits<key_type>::quant <= 16, "Only low quantized type supported.");

    pqueue_hqueue_fifo<image_concrete_t<I>> m_delegate;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class I>
  template <class J>
  inline pqueue_fifo<I>::pqueue_fifo(J&& f)
    : m_delegate{std::forward<J>(f)}
  {
  }

  template <class I>
  inline void pqueue_fifo<I>::push(const key_type& k, const value_type& v)
  {
    m_delegate.push(k, v);
  }

  template <class I>
  inline void pqueue_fifo<I>::pop()
  {
    m_delegate.pop();
  }

  template <class I>
  inline bool pqueue_fifo<I>::empty() const
  {
    return m_delegate.empty();
  }


  template <class I>
  inline auto pqueue_fifo<I>::top() const -> std::pair<key_type, value_type>
  {
    return m_delegate.top();
  }

} // namespace mln::morpho::experimental::details
