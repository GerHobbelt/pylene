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
        operator() (const grain_box2d& domain)
        {
	  //std::cout << "Running: " << domain << std::endl;

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
              //io::imprint(pare
            }

	  //std::cout << "End of process." << std::endl;
	  //io::imprint(m_parent);

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
	static tbb::mutex   m_mutex;
      };

      template <typename V, typename Neighborhood, typename StrictWeakOrdering>
      tbb::mutex MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering>::m_mutex;


      namespace parallel
      {
	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	image2d<point2d>
	maxtree_ufind(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
	  tbb::parallel_reduce(grain_box2d(ima.domain(), ima.nrows() / 64), algo, tbb::auto_partitioner());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;
	  // canonization
	  image2d<point2d>& parent = algo.m_parent;
	  mln_foreach(auto& p, parent.values())
	    p = internal::zfind_repr(ima, parent, p);

	  return algo.m_parent;
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


    template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
    image2d<point2d>
    maxtree_ufind_parallel(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
    {
      impl::MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
      tbb::parallel_reduce(ima.domain(), algo, tbb::auto_partitioner());

      // canonization
      image2d<point2d>& parent = algo.m_parent;
      mln_foreach(auto& p, parent.values())
	p = internal::zfind_repr(ima, parent, p);

      return algo.m_parent;
    }



  }

}

#endif // !MLN_MORPHO_MAXTREE_UFIND_PARALLEL_HPP
