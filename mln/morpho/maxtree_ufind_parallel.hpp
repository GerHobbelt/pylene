#ifndef MAXTREE_UFIND_HPP
# define MAXTREE_UFIND_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/sub_image.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/algorithm/sort_indexes.hpp>
# include <mln/core/wrt_offset.hpp>
# include <tbb/parallel_reduce.h>
# include <mln/io/imprint.hpp>
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
      }


      MaxTreeAlgorithmUF(MaxTreeAlgorithmUF& other, tbb::split)
	: m_ima(other.m_ima), m_nbh(other.m_nbh), m_cmp(other.m_cmp), m_parent(other.m_parent),
	  m_zpar(other.m_zpar), m_has_previous(false)
      {
      }

      void
      operator() (const box2d& domain)
      {
        image2d<V> ima = m_ima | domain;
        image2d<point2d> parent = m_parent | domain;
        image2d<point2d> zpar = m_zpar | domain;
        image2d<bool> deja_vu;
        resize(deja_vu, ima, 1, false);

	m_current_domain = domain;

        m_S = sort_sites(ima, m_cmp);
	point2d p;
	mln_iter(n, m_nbh(p));

        for (int i = S.size()-1; i >= 0; --i)
          {
            p = m_S[i];
            // make set
            {
              parent(p) = p;
              zpar(p) = p;
              deja_vu(p) = true;
            }

	    mln_forall(n)
              {
                if (deja_vu.at(n))
                  {
                    point2d r = internal::zfind_root(zpar, n);
                    if (r != p) // make union
                      {
                        parent(r) = p;
                        zpar(r) = p;
                      }
                  }
              }
            //io::imprint(pare
          }

        // canonization
        for (auto p: m_S)
          {
            point2d q = parent(p);
            if (ima(parent(q)) == ima(q))
              parent(p) = parent(q);
          }


	if (m_has_previous)
	  this->join(*this);

	m_has_previous = true;
      }

      void join(MaxTreeAlgorithmUF& other)
      {
	merge_tree(m_ima, m_parent, other.m_current_domain, m_cmp);

        // Recanonization
        for (auto p: m_S)
          {
            point2d q = m_parent(p);
            if (m_ima(m_parent(q)) == m_ima(q))
              m_parent(p) = m_parent(q);
          }
      }


    private:
      const image2d<V>& m_ima;
      Neighborhood	m_nbh;
      image2d<point2d>  m_parent;
      image2d<point2d>  m_zpar;
      bool	        m_has_previous;
      box2d	        m_current_domain;
      std::vector<std::site_type> m_S;
    };

  }


    template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
    image2d<point2d>
    maxtree_ufind_parallel(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
    {
      impl::MaxTreeAlgorithmUF<V, Neighborhood, StrictWeakOrdering> algo(ima, nbh, cmp);
      tbb::parallel_reduce(ima.domain(), algo, tbb::auto_partitioner());
      return algo.m_parent;
    }



  }

}

#endif // !MLN_MORPHO_MAXTREE_UFIND_HPP
