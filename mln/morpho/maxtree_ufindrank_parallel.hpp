#ifndef MAXTREE_UFINDRANK_PARALLEL_HPP
# define MAXTREE_UFINDRANK_PARALLEL_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/sub_image.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/algorithm/sort_sites.hpp>
# include <mln/core/wrt_offset.hpp>

# include <mln/morpho/merge_tree.hpp>

# include <tbb/parallel_reduce.h>
# include <tbb/parallel_for.h>


namespace mln
{

  namespace morpho
  {

    namespace impl
    {

      template <typename V, typename Neighborhood, typename StrictWeakOrdering>
      struct MaxTreeAlgorithmUFRank
      {
        MaxTreeAlgorithmUFRank(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp)
          : m_ima (ima), m_nbh (nbh), m_cmp(cmp), m_has_previous(false)
        {
          resize(m_parent, ima);
          resize(m_zpar, ima);
	  resize(m_root, ima);
	  resize(m_rank, ima, ima.border(), 0);
	  m_nsplit = 0;
        }


        MaxTreeAlgorithmUFRank(MaxTreeAlgorithmUFRank& other, tbb::split)
          : m_ima(other.m_ima), m_nbh(other.m_nbh), m_cmp(other.m_cmp), m_parent(other.m_parent),
            m_zpar(other.m_zpar), m_root(other.m_root), m_rank(other.m_rank), m_has_previous(false)
        {
	  m_nsplit = 0;
        }


	void
	unionfind(const grain_box2d& domain)
	{
          image2d<V> ima = m_ima | domain;
          image2d<std::size_t>  parent = m_parent | domain;
          image2d<std::size_t>  zpar = m_zpar | domain;
	  image2d<unsigned>	rank = m_rank | domain;
	  image2d<std::size_t>  root = m_root | domain;
          image2d<bool> deja_vu;
          resize(deja_vu, ima, m_ima.border(), false);

	  //int djvu_offset = deja_vu.index_of_point(domain.pmin) - ima.index_of_point(domain.pmin);
          m_S = sort_indexes(ima, m_cmp);
	  auto dindexes = wrt_delta_index(ima, m_nbh.dpoints);


          for (int i = m_S.size()-1; i >= 0; --i)
            {
	      std::size_t p = m_S[i];
              // make set
	      assert(domain.has(deja_vu.point_at_index(p)));
              {
                parent[p] = p;
                zpar[p] = p;
		root[p] = p;
                deja_vu[p] = true;
              }

	      std::size_t x = p; // zpar of p
              for (unsigned j = 0; j < dindexes.size(); ++j)
		{
		  std::size_t n = p + dindexes[j];

		  if (deja_vu[n])
                  {
		    std::size_t r = internal::zfind_root(zpar, n);
                    if (r != x) // make union
                      {
			parent[root[r]] = p;
			if (rank[x] < rank[r]) { //we merge p to r
			  zpar[x] = r;
			  root[r] = p;
			  x = r;
			} else if (rank[r] < rank[p]) { // merge r to p
			  zpar[r] = p;
			} else { // same height
			  zpar[r] = p;
			  rank[p] += 1;
			}
                      }
                  }
		}
            }
	}


	void
	unionfind_line(int row)
	{
	  static const int nvalues = 1 << value_traits<V>::quant;
	  point2d p_ = m_ima.domain().pmin;
	  p_[0] = row;

	  int ncols = m_ima.ncols();
	  std::size_t stack[nvalues];
	  int sz = 0;
	  std::size_t prec = m_ima.index_of_point(p_);
	  std::size_t p = prec + 1;

	  for (int i = 1; i < ncols; ++i, ++p)
	    {
	      if (m_cmp(m_ima[prec],m_ima[p])) // m_ima[prec] < m_ima[p] => start new component
		{
		  stack[sz++] = prec;
		  //m_parent[prec] = prec;
		  prec = p;
		}
	      else if (not m_cmp(m_ima[p], m_ima[prec])) // m_ima[p] == m_ima[prec] => extend component
		{
		  m_parent[p] = prec;
		}
	      else // m_ima[p] < m_ima[prec] => we need to attach prec to its m_parent
		{
		  while (sz > 0 and not m_cmp(m_ima[stack[sz-1]], m_ima[p]))
		    {
		      m_parent[prec] = stack[sz-1];
		      prec = stack[sz-1];
		      --sz;
		    }
		  // we have m_ima[p] <= m_ima[prec]
		  if (m_cmp(m_ima[p], m_ima[prec])) // m_ima[p] < m_ima[prec] => attach prec to p, p new component
		    {
		      m_parent[prec] = p;
		      prec = p;
		    }
		  else                        // m_ima[p] == m_ima[prec] => attach p to prec (canonization)
		    {
		      m_parent[p] = prec;
		    }
		}
	    }

	  // Attach last point (i.e prec)
	  while (sz > 0)
	    {
	      m_parent[prec] = stack[sz-1];
	      prec = stack[sz-1];
	      --sz;
	    }
	  m_parent[prec] = prec;
	}


        void
        operator() (const box2d& domain)
        {
	  //std::cout << domain << std::endl;
	  if (domain.shape()[0] > 1)
	    unionfind(domain);
	  else
	    unionfind_line(domain.pmin[0]);

          if (m_has_previous)
	    {
	      this->join(*this, false);
	      m_current_domain.join(domain);
	      m_nsplit += 1;
	    }
	  else
	    {
	      m_current_domain = domain;
	      m_has_previous = true;
	    }
        }

        void join(MaxTreeAlgorithmUFRank& other, bool joindomain = true)
        {
	  mln_precondition(m_has_previous);

          merge_tree(m_ima, m_parent, this->m_current_domain, m_cmp);

	  if (joindomain)
	    {
	      m_current_domain.join(other.m_current_domain);
	      m_nsplit += other.m_nsplit + 1;
	    }
        }


      public:
        const image2d<V>&  m_ima;
        Neighborhood	   m_nbh;
        StrictWeakOrdering m_cmp;

        image2d<std::size_t> m_parent;
        image2d<std::size_t> m_zpar;
	image2d<std::size_t> m_root;
	image2d<unsigned>    m_rank;

        bool	          m_has_previous;
        box2d	          m_current_domain;

        std::vector<std::size_t> m_S;
	unsigned	     m_nsplit;
      };


      namespace parallel
      {
	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<std::size_t>
	maxtree_ufindrank(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUFRank<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  int grain = std::max(ima.nrows() / 64, 1u);
	  std::cout << "Grain: " << grain << std::endl;
	  tbb::parallel_reduce(grain_box2d(ima.domain(), grain), algo, tbb::auto_partitioner());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;


	  image2d<std::size_t>& parent = algo.m_parent;
	  MaxtreeCanonizationAlgorithm<V> canonizer(ima, parent);
	  tbb::parallel_for(grain_box2d(ima.domain(), grain), canonizer, tbb::auto_partitioner());

	  return parent;
	}

      }


      namespace serial
      {

	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<std::size_t>
	maxtree_ufindrank(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUFRank<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  algo(ima.domain());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;
	  // canonization
	  image2d<std::size_t>& parent = algo.m_parent;
	  for (std::size_t p : algo.m_S)
	    {
	      std::size_t q = parent[p];
	      if (ima[parent[q]] == ima[q])
		parent[p] = parent[q];
	    }

	  return parent;
	}

      }

    }


    // template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
    // image2d<point2d>
    // maxtree_ufind_parallel(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
    // {
    //   impl::MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
    //   tbb::parallel_reduce(ima.domain(), algo, tbb::auto_partitioner());

    //   // canonization
    //   image2d<point2d>& parent = algo.m_parent;
    //   mln_foreach(auto& p, parent.values())
    // 	p = internal::zfind_repr(ima, parent, p);

    //   return algo.m_parent;
    // }



  }

}

#endif // !MLN_MORPHO_MAXTREE_UFINDRANK_PARALLEL_HPP
