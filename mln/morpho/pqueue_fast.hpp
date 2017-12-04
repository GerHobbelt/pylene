#ifndef MLN_MORPHO_PQUEUE_FAST_HPP
#define MLN_MORPHO_PQUEUE_FAST_HPP

#ifndef PQUEUE_FAST_SWITCH_HQUEUE_NBITS
#define PQUEUE_FAST_SWITCH_HQUEUE_NBITS 18
#endif

#include "bounded_hqueue.hpp"
#include <mln/core/image/image2d.hpp>
#include <mln/core/value/indexer.hpp>
#include <queue>
#include <vector>

namespace mln
{

  namespace morpho
  {

    namespace internal
    {
      template <class I, typename Compare>
      struct pqueue_cmp_t
      {
        const I& m_ima;
        Compare m_cmp;

        pqueue_cmp_t(const I& ima, const Compare& cmp) : m_ima(ima), m_cmp(cmp) {}

        bool operator()(std::size_t p, std::size_t q) { return m_cmp(m_ima[p], m_ima[q]); }
      };
    }

    template <class I, typename Compare, typename Enable = void>
    struct priority_queue_ima : public std::priority_queue<typename I::size_type, std::vector<typename I::size_type>,
                                                           internal::pqueue_cmp_t<I, Compare>>
    {
      typedef typename I::size_type size_type;
      using V = mln_value(I);

      typedef std::priority_queue<size_type, std::vector<size_type>, internal::pqueue_cmp_t<I, Compare>> base;

      priority_queue_ima(const I& ima, Compare cmp) : base(internal::pqueue_cmp_t<I, Compare>{ima, cmp})
      {
        this->c.reserve(ima.domain().size());
      }
    };

    template <typename I, typename Compare>
    struct priority_queue_ima<I, Compare, typename std::enable_if<(value_traits<mln_value(I)>::quant <
                                                                   PQUEUE_FAST_SWITCH_HQUEUE_NBITS)>::type>
    {
      typedef typename I::size_type size_type;
      using V = mln_value(I);

      priority_queue_ima(const I& ima, Compare) : m_ima(ima)
      {
        i = value_traits<index_type>::min();
        {
          std::vector<std::size_t> hist(nlevels, 0);

          mln_pixter(px, ima);
          mln_forall (px)
          {
            index_type l = h(px->val());
            ++hist[l];
          }
          m_hq.init(&hist[0]);
        }
      }

      size_type top() const { return m_hq.top_at_level(i); }
      size_type pop()
      {
        size_type x = m_hq.pop_at_level(i);
        while (i > value_traits<index_type>::min() and m_hq.empty(i))
          --i;
        return x;
      }

      void push(size_type p)
      {
        index_type x = h(m_ima[p]);
        m_hq.push_at_level(p, x);
        if (x > i)
          i = x;
      }

      bool empty() const { return m_hq.empty(i); }

    private:
      typedef typename indexer<V, Compare>::index_type index_type;
      static constexpr std::size_t nlevels = (std::size_t)1 << value_traits<index_type>::quant;
      const I& m_ima;
      indexer<V, Compare> h;
      index_type i;
      bounded_hqueue<size_type, nlevels, std::allocator<size_type>, true> m_hq;
    };
  }
}

#endif // ! MLN_MORPHO_PQUEUE_FAST_HPP
