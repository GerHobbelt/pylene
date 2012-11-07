#ifndef MAXTREE_UFIND_PARALLEL_HPP
# define MAXTREE_UFIND_PARALLEL_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/sub_image.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/algorithm/sort_indexes.hpp>
# include <mln/core/wrt_offset.hpp>

# include <mln/io/imprint.hpp>
# include <mln/morpho/merge_tree.hpp>
# include <alloca.h>

# include <tbb/parallel_reduce.h>
# include <tbb/parallel_for.h>

# include <tbb/mutex.h>

namespace mln
{

  namespace morpho
  {

    namespace impl
    {

      template <typename V, typename Neighborhood, typename StrictWeakOrdering>
      struct MaxTreeAlgorithmUF
      {
        MaxTreeAlgorithmUF(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp)
          : m_ima (ima), m_nbh (nbh), m_cmp(cmp), m_has_previous(false)
        {
          resize(m_parent, ima);
          resize(m_zpar, ima);
	  m_nsplit = 0;
        }


        MaxTreeAlgorithmUF(MaxTreeAlgorithmUF& other, tbb::split)
          : m_ima(other.m_ima), m_nbh(other.m_nbh), m_cmp(other.m_cmp), m_parent(other.m_parent),
            m_zpar(other.m_zpar), m_has_previous(false)
        {
	  m_nsplit = 0;
        }


	static
	inline
	std::size_t
	zfindroot(image2d<std::size_t>& parent, const std::size_t& p)
	{
	  if (parent[p] == p)
	    return p;
	  else
	    return parent[p] = zfindroot(parent, parent[p]);
	}

	void
	unionfind(const box2d& domain)
	{
          image2d<V> ima = m_ima | domain;
          image2d<std::size_t> parent = m_parent | domain;
          image2d<std::size_t> zpar = m_zpar | domain;
          image2d<bool> deja_vu;
	  std::size_t i = (d.pmax[1] - d.pmin[1]) * (domain.pmin[0] - d.pmin[0]);
	  std::size_t* S = &m_S[i];

          resize(deja_vu, ima, m_ima.border(), false);

	  //int djvu_offset = deja_vu.index_of_point(domain.pmin) - ima.index_of_point(domain.pmin);
	  const box2d& d = m_ima.domain();

          sort_indexes_it(ima, S, m_cmp);
	  auto dindexes = wrt_delta_index(ima, m_nbh.dpoints);


          for (int i = ima.domain().size() - 1; i >= 0; --i)
            {
	      std::size_t p = S[i];
	      //std::cout << deja_vu.point_at_index(djvu_offset + p) << std::endl;
              // make set
	      assert(domain.has(deja_vu.point_at_index(p)));
              {
                parent[p] = p;
                zpar[p] = p;
                deja_vu[p] = true;
              }

	      std::size_t z = p; // zpar of p
              for (unsigned j = 0; j < dindexes.size(); ++j)
		{
		  std::size_t n = p + dindexes[j];

		  if (deja_vu[n])
                  {
		    std::size_t r = zfindroot(zpar, n);

                    if (r != z) // make union
                      {
		    	if (ima[r] != ima[z]) {
		    	  parent[r] = z;
		    	  zpar[r] = z;
		    	} else {  // level compression
		    	  parent[z] = r;
		    	  zpar[z] = r;
		    	  z = r;
		    	}
                      }
                  }
		}
            }
	}


	void
	unionfind_line(int row)
	{
	  static constexpr std::size_t nvalues = 1ul << value_traits<V>::quant;
	  point2d p_ = m_ima.domain().pmin;
	  p_[0] = row;

	  int ncols = m_ima.ncols();
	  int sz = 0;
	  std::size_t prec = m_ima.index_of_point(p_);
	  std::size_t p = prec + 1;
	  std::size_t* stack;

	  if (value_traits<V>::quant <= 16)
	    stack = (std::size_t*) alloca(nvalues * sizeof(size_t));
	  else
	    stack = new std::size_t[nvalues];

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

	  if (value_traits<V>::quant > 16)
	    delete [] stack;
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

        void join(MaxTreeAlgorithmUF& other, bool joindomain = true)
        {
	  mln_precondition(m_has_previous);

          merge_tree(m_ima, m_parent, this->m_current_domain, m_cmp);
	  merge_S(m_parent, m_dejavu, m_S, m_Saux);

	  // {
	  //   tbb::mutex::scoped_lock lock(m_mutex);
	  //   std::cout << "Start Merge:" << this->m_current_domain << " w " << other.m_current_domain
	  // 	      << " by " << tbb::this_tbb_thread::get_id() << std::endl;
	  //   std::cout.flush();
	  // }
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
        bool	          m_has_previous;
        box2d	          m_current_domain;

        std::vector<std::size_t> m_S;
	unsigned	     m_nsplit;
      };


      template <typename V>
      struct MaxtreeCanonizationAlgorithm
      {
	MaxtreeCanonizationAlgorithm(const image2d<V>& ima,
				     image2d<std::size_t>& parent)
	  : m_ima (ima), m_parent (parent)
	{
	}


	void
	operator() (const box2d& domain) const
	{
	  image2d<std::size_t> parent = m_parent | domain;
	  mln_foreach(auto& p, parent.values())
	    p = internal::zfind_repr(m_ima, m_parent, p);
	}

	const image2d<V>&	m_ima;
	image2d<std::size_t>&   m_parent;

      };


      namespace parallel
      {
	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<std::size_t>
	maxtree_ufind(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  int grain = std::max(ima.nrows() / 64, 1u);
	  std::cout << "Grain: " << grain << std::endl;
	  tbb::parallel_reduce(grain_box2d(ima.domain(), grain), algo, tbb::auto_partitioner());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;


	  image2d<std::size_t>& parent = algo.m_parent;
	  MaxtreeCanonizationAlgorithm<V> canonizer(ima, parent);
	  tbb::parallel_for(grain_box2d(ima.domain(), grain), canonizer, tbb::auto_partitioner());

	  return parent;
	}

	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<std::size_t>
	maxtree_ufind_line(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  std::cout << "Grain: " <<  1 << std::endl;
	  tbb::parallel_reduce(ima.domain(), algo, tbb::simple_partitioner());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;


	  image2d<std::size_t>& parent = algo.m_parent;
	  MaxtreeCanonizationAlgorithm<V> canonizer(ima, parent);
	  int grain = std::max(ima.nrows() / 64, 1u);
	  tbb::parallel_for(grain_box2d(ima.domain(), grain), canonizer, tbb::auto_partitioner());

	  return parent;
	}


      }


      namespace serial
      {

	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<std::size_t>
	maxtree_ufind(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  algo(ima.domain());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;
	  // canonization
	  // image2d<std::size_t>& parent = algo.m_parent;
	  // for (std::size_t p : algo.m_S)
	  //   {
	  //     std::size_t q = parent[p];
	  //     if (ima[parent[q]] == ima[q])
	  // 	parent[p] = parent[q];
	  //   }

	  return algo.m_parent;
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

#endif // !MLN_MORPHO_MAXTREE_UFIND_PARALLEL_HPP
