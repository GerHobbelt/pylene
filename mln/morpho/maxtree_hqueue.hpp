#ifndef MLN_CORE_MORPHO_MAXTREE_HQUEUE_HPP
# define MLN_CORE_MORPHO_MAXTREE_HQUEUE_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/wrt_offset.hpp>
# include <mln/core/value/indexer.hpp>
# include <vector>

#include <mln/io/imprint.hpp>

namespace mln
{

  namespace morpho
  {

    namespace internal
    {

      template <typename I, typename Neighborhood, typename StrictWeakOrdering>
      struct maxtree_flood_algorithm
      {
	typedef mln_value(I) V;
	typedef typename indexer<V,StrictWeakOrdering>::index_type level_t;
	typedef std::size_t  elt_type; // either point or index
	enum { nlevels = 1 << value_traits<level_t>::quant};


	level_t flood(level_t level)
	{
	  mln_precondition(m_has_repr[level]);
	  mln_precondition(not m_q[level].empty());
	  elt_type r = m_repr[level];
	  mln_precondition(m_h(m_ima[r]) == level);

	  while (!m_q[level].empty())
	    {
	      elt_type p = m_q[level].back();
	      m_q[level].pop_back();
	      m_parent[p] = r;

	      mln_foreach(auto k, m_nbh_delta_indexes)
		{
		  auto q = p + k;
		  if (!m_deja_vu[q])
		    {
		      level_t newlevel = m_h(m_ima[q]);
		      if (!m_has_repr[newlevel])
			{
			  m_repr[newlevel] = q;
			  m_has_repr[newlevel] = true;
			}
		      m_q[newlevel].push_back(q);
		      m_deja_vu[q] = true;

		      if (level < newlevel)
			do {
			  newlevel = flood(newlevel);
			} while (level < newlevel);
		    }
		}
	    }

	  // Attach to parent
	  m_has_repr[level] = false;
	  level_t old = level;
	  --level;
	  while (level > value_traits<level_t>::min() and !m_has_repr[level]) {
	    --level;
	  }
	  if (level > value_traits<level_t>::min())
	      m_parent[r] = m_repr[level];
	  return level;
	}

	maxtree_flood_algorithm(const I& ima, image2d<std::size_t>& parent, const Neighborhood& nbh,
				StrictWeakOrdering)
	  : m_ima(ima), m_parent (parent), m_nbh_delta_indexes(wrt_delta_index(ima, Neighborhood::dpoints)),
	    m_has_repr {false,}
	{
	  resize(m_deja_vu, ima, ima.border(), false);
	  extension::fill(m_deja_vu, true);

	  // Get min element and reserve queue
	  size_t pmin = ima.index_of_point(ima.domain().pmin);
	  level_t vmin = value_traits<level_t>::max();
	  {
	    unsigned h[nlevels] = {0,};

	    mln_pixter(px, ima);
	    mln_forall(px)
	    {
	      level_t l = m_h(px->val());
	      ++h[l];
	      if (l < vmin)
		{
		  vmin = l;
		  pmin = px->index();
		}
	    }

	    for (unsigned i = 0; i < nlevels; ++i)
	      m_q[i].reserve(h[i]);
	  }

	  // Start flooding
	  std::cout << (int) vmin << "@" << pmin << std::endl;
	  m_q[vmin].push_back(pmin);
	  m_repr[vmin] = pmin;
	  m_has_repr[vmin] = true;
	  m_deja_vu[pmin] = true;
	  flood(vmin);
	}

	static void run(const I& ima, image2d<std::size_t>& parent, const Neighborhood& nbh,
			StrictWeakOrdering cmp)
	{
	  maxtree_flood_algorithm x(ima, parent, nbh, cmp);
	  (void) x;
	}

      private:
	typedef mln_ch_value(I, bool) J;
	//typedef typename std::result_of< Neighborhood(mln_pixel(J)) > nbh_iter_t;

	StrictWeakOrdering	       m_cmp;
	indexer<V, StrictWeakOrdering> m_h;

	const I&		    m_ima;
	mln_ch_value(I, elt_type)&  m_parent;
	mln_ch_value(I, bool)	    m_deja_vu;
	mln::array<typename I::difference_type, Neighborhood::static_size> m_nbh_delta_indexes;

	std::vector<elt_type>   m_q[nlevels];
	bool			m_has_repr[nlevels];
	elt_type		m_repr[nlevels];
      };

    } // end of namespace mln::morpho::internal


    template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
    image2d<std::size_t>
    maxtree_hqueue(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
    {
      image2d<std::size_t> parent;
      resize(parent, ima);

      internal::maxtree_flood_algorithm<image2d<V>, Neighborhood, StrictWeakOrdering>::run(ima, parent, nbh, cmp);
      return parent;
    }

  }  // end of namespace mln::morpho

} // end of namespace mln

#endif // ! MLN_CORE_MORPHO_MAXTREE_HQUEUE_HPP
