#ifndef MLN_CORE_MORPHO_MAXTREE_HQUEUE_HPP
# define MLN_CORE_MORPHO_MAXTREE_HQUEUE_HPP

namespace mln
{

  namespace morpgo
  {

    namespace internal
    {

      template <typename I, typename Neighborhood>
      struct maxtree_flood_algorithm
      {
	typedef mln_value(I) level_t;
	typedef std::size_t  elt_type; // either point or index
	enum { nvalues = value_traits<level_t>::sup() + 1};


	int flood(int level)
	{
	  mln_precondition(m_has_repr[level]);
	  mln_precondition(not m_q[level].empty());
	  elt_type r = m_repr[level];
	  while (!m_q[level].empty())
	    {
	      elt_type p = m_q[level].back();
	      m_q[level].pop_back();
	      m_parent[p] = r;
	      m_deja_vu[p] = true;

	      mln_foreach(auto k, m_nbh_delta_indexes)
		{
		  auto q = p + k;
		  if (!m_deja_vu[q])
		    {
		      int newlevel = m_ima[q];
		      if (m_q[newlevel].empty())
			m_repr[newlevel] = q;
		      m_q[newlevel].push_back(q);

		      if (newlevel > level)
			do {
			  newlevel = flood(newlevel);
			} while (newlevel > level);
		    }
		}
	    }

	  // Attach to parent
	  --level;
	  while (level > 0 and m_q[level].empty())
	    --level;
	  m_parent[r] = m_repr[level];
	}

      public:
	typedef mln_ch_value(I, bool) J;
	typedef typename std::result_of< Neighborhood(mln_pixel(J)) > nbh_iter_t;

	const I&		   m_ima;
	mln_ch_value(I, bool)	   m_deja_vu;
	mln_ch_value(I, elt_type)  m_parent;

	hqueue<elt_type, nlevels>  m_q;
	elt_type  m_repr[nlevels];
      };

    }


    template <typename V, typename Neighborhood>
    image2d<std::size_type>
    maxtree_hqueue(const image2d<V>& ima, const Neighborhood& nbh)
    {
      image2d<std::size_type> parent;
      


    }

  }

}

#endif // ! MLN_CORE_MORPHO_MAXTREE_HQUEUE_HPP
