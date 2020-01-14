#pragma once

#include <mln/core/concept/new/images.hpp>


#include <mln/morpho/experimental/private/hpqueue.hpp>
#include <mln/morpho/experimental/private/hlinked_lists.hpp>
#include <mln/morpho/experimental/private/hvector.hpp>

namespace mln::morpho::experimental::details
{
  enum class pqueue_impl
  {
    linked_list,
    vector,
  };

  /// Provides a priority queue of points with a the fifo property
  template <class I, pqueue_impl impl_selector = pqueue_impl::linked_list, bool reversed = false>
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
    bool                            has_key(const key_type& priority) const;

  private:
    static_assert(!std::is_signed_v<key_type>, "Must not be signed");
    static_assert(value_traits<key_type>::quant <= 16, "Only low quantized type supported.");

    static constexpr int nvalues = std::numeric_limits<key_type>::max() + 1;

    using impl_t = std::conditional_t<impl_selector == pqueue_impl::linked_list,
                                      hlinked_lists<nvalues, value_type, image_ch_value_t<I, value_type>>, //
                                      hvectors<value_type>>;

    detail::hpqueue<nvalues, value_type, impl_t, reversed> m_delegate;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class I, pqueue_impl impl_selector, bool reverse>
  template <class J>
  inline pqueue_fifo<I, impl_selector, reverse>::pqueue_fifo(J&& f)
    : m_delegate{std::forward<J>(f)}
  {
  }

  template <class I, pqueue_impl impl_selector, bool reverse>
  inline void pqueue_fifo<I, impl_selector, reverse>::push(const key_type& k, const value_type& v)
  {
    m_delegate.push_last(k, v);
  }

  template <class I, pqueue_impl impl_selector, bool reverse>
  inline void pqueue_fifo<I, impl_selector, reverse>::push_first(const key_type& k, const value_type& v)
  {
    m_delegate.push_first(k, v);
  }


  template <class I, pqueue_impl impl_selector, bool reverse>
  inline void pqueue_fifo<I, impl_selector, reverse>::pop()
  {
    m_delegate.pop();
  }

  template <class I, pqueue_impl impl_selector, bool reverse>
  inline bool pqueue_fifo<I, impl_selector, reverse>::empty() const
  {
    return m_delegate.empty();
  }


  template <class I, pqueue_impl impl_selector, bool reverse>
  inline auto pqueue_fifo<I, impl_selector, reverse>::top() const -> std::pair<key_type, value_type>
  {
    return m_delegate.top();
  }

  template <class I, pqueue_impl impl_selector, bool reverse>
  inline bool pqueue_fifo<I, impl_selector, reverse>::has_key(const key_type& k) const
  {
    return m_delegate.has_key(k);
  }



} // namespace mln::morpho::experimental::details
