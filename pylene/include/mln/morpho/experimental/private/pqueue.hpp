#pragma once

#include <mln/core/concept/new/images.hpp>


#include <mln/morpho/experimental/private/hpqueue.hpp>

namespace mln::morpho::experimental::details
{

  /// Provides a priority queue of points with a the fifo property
  template <class I, bool reversed = false>
  class pqueue_fifo
  {
    using key_type   = image_value_t<I>;
    using value_type = image_point_t<I>;

  public:
    template <class J>
    pqueue_fifo(J&& f);

    void                            push(const key_type& priority, const value_type& element);
    void                            push_first(const key_type& priority, const value_type& element);
    void                            pop();
    std::pair<key_type, value_type> top() const;
    bool                            empty() const;

  private:
    static_assert(!std::is_signed_v<key_type>, "Must not be signed");
    static_assert(value_traits<key_type>::quant <= 16, "Only low quantized type supported.");

    static constexpr int nvalues = std::numeric_limits<key_type>::max() + 1;

    detail::hpqueue<nvalues, value_type, image_ch_value_t<I, value_type>, reversed> m_delegate;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class I, bool reverse>
  template <class J>
  inline pqueue_fifo<I, reverse>::pqueue_fifo(J&& f)
    : m_delegate{std::forward<J>(f)}
  {
  }

  template <class I, bool reverse>
  inline void pqueue_fifo<I, reverse>::push(const key_type& k, const value_type& v)
  {
    m_delegate.push_last(k, v);
  }

  template <class I, bool reverse>
  inline void pqueue_fifo<I, reverse>::push_first(const key_type& k, const value_type& v)
  {
    m_delegate.push_first(k, v);
  }


  template <class I, bool reverse>
  inline void pqueue_fifo<I, reverse>::pop()
  {
    m_delegate.pop();
  }

  template <class I, bool reverse>
  inline bool pqueue_fifo<I, reverse>::empty() const
  {
    return m_delegate.empty();
  }


  template <class I, bool reverse>
  inline auto pqueue_fifo<I, reverse>::top() const -> std::pair<key_type, value_type>
  {
    return m_delegate.top();
  }


} // namespace mln::morpho::experimental::details
