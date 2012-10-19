#ifndef MAXTREE_UFIND_PARALLEL_HPP
# define MAXTREE_UFIND_PARALLEL_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/sub_image.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/algorithm/sort_sites.hpp>
# include <mln/core/wrt_offset.hpp>

# include <mln/io/imprint.hpp>
# include <mln/morpho/merge_tree.hpp>

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
	point2d zfindroot(image2d<point2d>& parent, const point2d& p)
	{
	  if (parent(p) == p)
	    return p;
	  else
	    return parent(p) = zfindroot(parent, parent(p));
	}

	void
	unionfind(const grain_box2d& domain)
	{
          image2d<V> ima = m_ima | domain;
          image2d<point2d> parent = m_parent | domain;
          image2d<point2d> zpar = m_zpar | domain;
          image2d<bool> deja_vu;
          resize(deja_vu, ima, 1, false);

          m_S = sort_sites(ima, m_cmp);
          point2d p;
          mln_iter(n, m_nbh(p));

          for (int i = m_S.size()-1; i >= 0; --i)
            {
              p = m_S[i];
	      //std::cout << "Processing: " << p << " @ " << (int)ima(p) << std::endl;
              // make set
              {
                parent(p) = p;
                zpar(p) = p;
                deja_vu(p) = true;
              }

              mln_forall(n)
              {
                if (deja_vu.at(*n))
                  {
                    point2d r = zfindroot(zpar, *n);
                    if (r != p) // make union
                      {
                        parent(r) = p;
                        zpar(r) = p;
                      }
                  }
              }
            }
	}


	void
	unionfind_line(int row)
	{
	  static const int nvalues = 1 << value_traits<V>::quant;
	  point2d p = m_ima.domain().pmin;
	  p[0] = row;

	  int ncols = m_ima.ncols();
	  point2d stack[nvalues];
	  int sz = 0;
	  point2d prec = p;
	  ++p[1];
	  for (int i = 1; i < ncols; ++i, ++p[1])
	    {
	      if (m_cmp(m_ima(prec),m_ima(p))) // m_ima(prec) < m_ima(p) => start new component
		{
		  stack[sz++] = prec;
		  //m_parent(prec) = prec;
		  prec = p;
		}
	      else if (not m_cmp(m_ima(p), m_ima(prec))) // m_ima(p) == m_ima(prec) => extend component
		{
		  m_parent(p) = prec;
		}
	      else // m_ima(p) < m_ima(prec) => we need to attach prec to its m_parent
		{
		  while (sz > 0 and not m_cmp(m_ima(stack[sz-1]), m_ima(p)))
		    {
		      m_parent(prec) = stack[sz-1];
		      prec = stack[sz-1];
		      --sz;
		    }
		  // we have m_ima(p) <= m_ima(prec)
		  if (m_cmp(m_ima(p), m_ima(prec))) // m_ima(p) < m_ima(prec) => attach prec to p, p new component
		    {
		      m_parent(prec) = p;
		      prec = p;
		    }
		  else                        // m_ima(p) == m_ima(prec) => attach p to prec (canonization)
		    {
		      m_parent(p) = prec;
		    }
		}
	    }

	  // Attach last point (i.e prec)
	  while (sz > 0)
	    {
	      m_parent(prec) = stack[sz-1];
	      prec = stack[sz-1];
	      --sz;
	    }
	  m_parent(prec) = prec;
	}


        void
        operator() (const grain_box2d& domain)
        {
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

        image2d<point2d>   m_parent;

        image2d<point2d>  m_zpar;
        bool	          m_has_previous;
        box2d	          m_current_domain;

        std::vector<point2d> m_S;
	unsigned	    m_nsplit;
      };


      template <typename V>
      struct MaxtreeCanonizationAlgorithm
      {
	MaxtreeCanonizationAlgorithm(const image2d<V>& ima,
				     image2d<point2d>& parent)
	  : m_ima (ima), m_parent (parent)
	{
	}


	void
	operator() (const grain_box2d& domain) const
	{
	  image2d<point2d> parent = m_parent | domain;
	  mln_foreach(auto& p, parent.values())
	    p = internal::zfind_repr(m_ima, m_parent, p);
	}

	const image2d<V>& m_ima;
	image2d<point2d>&  m_parent;

      };


      namespace parallel
      {
	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<point2d>
	maxtree_ufind(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  int grain = std::max(ima.nrows() / 64, 1u);
	  std::cout << "Grain: " << grain << std::endl;
	  tbb::parallel_reduce(grain_box2d(ima.domain(), grain), algo, tbb::auto_partitioner());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;


	  image2d<point2d>& parent = algo.m_parent;
	  MaxtreeCanonizationAlgorithm<V> canonizer(ima, parent);
	  tbb::parallel_for(grain_box2d(ima.domain(), grain), canonizer, tbb::auto_partitioner());

	  return parent;
	}

	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<point2d>
	maxtree_ufind_line(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  std::cout << "Grain: " <<  1 << std::endl;
	  tbb::parallel_reduce(ima.domain(), algo, tbb::simple_partitioner());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;


	  image2d<point2d>& parent = algo.m_parent;
	  MaxtreeCanonizationAlgorithm<V> canonizer(ima, parent);
	  int grain = std::max(ima.nrows() / 64, 1u);
	  tbb::parallel_for(grain_box2d(ima.domain(), grain), canonizer, tbb::auto_partitioner());

	  return parent;
	}


      }


      namespace serial
      {

	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<point2d>
	maxtree_ufind(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  algo(ima.domain());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;
	  // canonization
	  image2d<point2d>& parent = algo.m_parent;
	  for (point2d p : algo.m_S)
	    {
	      point2d q = parent(p);
	      if (ima(parent(q)) == ima(q))
		parent(p) = parent(q);
	    }

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
