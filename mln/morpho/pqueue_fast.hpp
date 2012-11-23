// Copyright (C) 2009 EPITA Research and Development Laboratory (LRDE)
//
// This file is part of Olena.
//
// Olena is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, version 2 of the License.
//
// Olena is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Olena.  If not, see <http://www.gnu.org/licenses/>.
//
// As a special exception, you may use this file as part of a free
// software project without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to produce
// an executable, this file does not by itself cause the resulting
// executable to be covered by the GNU General Public License.  This
// exception does not however invalidate any other reasons why the
// executable file might be covered by the GNU General Public License.

#ifndef PQUEUE_FAST_HPP
# define PQUEUE_FAST_HPP

# include <mln/core/value/indexer.hpp>
# include <mln/morpho/bounded_hqueue.hpp>
# include <mln/core/image/image2d.hpp>
# include <queue>
# include <vector>

namespace mln
{

  namespace morpho
  {

    namespace internal
    {
      template <typename V, typename Compare>
      struct pqueue_cmp_t {
	const image2d<V>& m_ima;
	Compare m_cmp;

	pqueue_cmp_t(const image2d<V>& ima, const Compare& cmp)
	  : m_ima(ima), m_cmp(cmp)
	{
	}

	bool operator() (std::size_t p, std::size_t q) {
	  return m_cmp(m_ima[p], m_ima[q]);
	}
      };
    }

    template<typename V, typename Compare, typename Enable = void>
    struct priority_queue_ima :
      public std::priority_queue<std::size_t, std::vector<std::size_t>, internal::pqueue_cmp_t<V, Compare> >
    {
      typedef std::priority_queue<std::size_t, std::vector<std::size_t>, internal::pqueue_cmp_t<V, Compare> > base;

      priority_queue_ima(const image2d<V>& ima, Compare cmp)
	: base ( internal::pqueue_cmp_t<V, Compare> {ima, cmp} )
      {
	this->c.reserve(ima.domain().size());
      }
    };


    template<typename V, typename Compare>
    struct priority_queue_ima<V, Compare, typename std::enable_if< (value_traits<V>::quant <= 16) >::type>
    {

      priority_queue_ima(const image2d<V>& ima, Compare)
	: m_ima(ima)
      {
	i = value_traits<index_type>::min();
	{
	  std::size_t hist[nlevels] = {0,};

	  mln_pixter(px, ima);
	  mln_forall(px)
	  {
	    index_type l = h(px->val());
	    ++hist[l];
	  }
	  m_hq.init(hist);
	}
      }

      std::size_t top() const { return m_hq.top_at_level(i); }
      std::size_t pop()
      {
	std::size_t x = m_hq.pop_at_level(i);
	index_type b = i;
	while (i > value_traits<index_type>::min() and m_hq.empty(i))
	  --i;
	return x;
      }

      void push(std::size_t p) {
	index_type x = h(m_ima[p]);
	m_hq.push_at_level(p, x);
	if (x > i)
	  i = x;
      }

      bool empty() const { return m_hq.empty(i); }

    private:
      typedef typename indexer<V, Compare>::index_type index_type;
      static constexpr unsigned nlevels = 1 << value_traits<index_type>::quant;
      const image2d<V>& m_ima;
      indexer<V, Compare> h;
      index_type i;
      bounded_hqueue<std::size_t, nlevels> m_hq;
    };



  }

}


#endif // ! PQUEUE_FAST_HPP
