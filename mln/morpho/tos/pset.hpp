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

#ifndef MLN_MOPRHO_PSET_PSET_HPP
# define MLN_MOPRHO_PSET_PSET_HPP

# include <set>

namespace mln
{

  namespace morpho
  {

    //bool Enable = has_indexer<typename I::value_type, Compare>::value  >
    template <typename I,
	      typename Compare = std::less<typename I::value_type>,
	      bool Enable = false>
    struct pset;


    template <typename I, typename Compare>
    struct pset<I, Compare, false>
    {
      typedef typename I::value_type	key_type;
      typedef typename I::size_type	value_type;

      struct cmp_t
      {
	bool operator () (const value_type& a, const value_type& b)
	{
	  return m_cmp(m_ima[a], m_ima[b]);
	}

	bool cmpkey (const key_type& x, const value_type& b)
	{
	  return m_cmp(x, m_ima[b]);
	}

	const I& m_ima;
	Compare  m_cmp;
      };

      pset(const I& ima, const Compare& cmp = Compare() );

      void insert(const value_type& v);
      bool empty() const;
      bool has_previous(const value_type& v) const;
      bool has_next(const value_type& v)	  const;

      value_type find_next(const value_type& v) const;
      value_type find_previous(const value_type& v) const;
      value_type pop_next(const value_type& v);
      value_type pop_previous(const value_type& v);

    private:
      std::multiset<std::size_t, cmp_t>		m_set;
    };



    /*************************/
    /**  Implementation     **/
    /*************************/



    template <typename I, typename Compare>
    pset<I, Compare, false>::pset(const I& ima, const Compare& cmp)
      : m_set(cmp_t {ima, cmp} )
    {
    }

    template <typename I, typename Compare>
    void
    pset<I, Compare, false>::insert(const value_type& v)
    {
      m_set.insert(v);
    }

    template <typename I, typename Compare>
    bool
    pset<I, Compare, false>::empty() const
    {
      return m_set.empty();
    }

    template <typename I, typename Compare>
    bool
    pset<I, Compare, false>::has_previous(const value_type& p) const
    {
      mln_precondition(!empty());
      return m_set.key_comp()(*(m_set.begin()), p);
    }

    template <typename I, typename Compare>
    bool
    pset<I, Compare, false>::has_next(const value_type& p) const
    {
      mln_precondition(!empty());
      return !m_set.key_comp()(*(m_set.rbegin()), p);
    }

    template <typename I, typename Compare>
    typename I::size_type
    pset<I, Compare, false>::find_previous(const value_type& p) const
    {
      mln_precondition(has_previous(p));
      return *m_set.lower_bound(p);
    }

    template <typename I, typename Compare>
    typename I::size_type
    pset<I, Compare, false>::find_next(const value_type& p) const
    {
      mln_precondition(has_next(p));
      return *m_set.upper_bound(p);
    }

    template <typename I, typename Compare>
    typename I::size_type
    pset<I, Compare, false>::pop_previous(const value_type& p)
    {
      mln_precondition(has_previous(p));
      auto x = m_set.upper_bound(p);
      --x;
      mln_assertion(x != m_set.end());
      value_type v = *x;
      m_set.erase(x);
      return v;
    }

    template <typename I, typename Compare>
    typename I::size_type
    pset<I, Compare, false>::pop_next(const value_type& p)
    {
      mln_precondition(has_next(p));
      auto x = m_set.lower_bound(p);
      mln_assertion(x != m_set.end());
      value_type v = *x;
      m_set.erase(x);
      return v;
    }

  }

}


#endif // ! MLN_MOPRHO_PSET_PSET_HPP
