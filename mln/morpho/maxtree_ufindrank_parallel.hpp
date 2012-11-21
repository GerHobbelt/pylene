#ifndef MAXTREE_UFINDRANK_PARALLEL_HPP
# define MAXTREE_UFINDRANK_PARALLEL_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/sub_image.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/algorithm/sort_sites.hpp>
# include <mln/core/wrt_offset.hpp>

# include <mln/morpho/merge_tree.hpp>
# include <mln/morpho/canonize.hpp>

# include <tbb/parallel_reduce.h>
# include <tbb/parallel_for.h>


namespace mln
{

  namespace morpho
  {

    namespace impl
    {

      template <typename V, typename Neighborhood, typename StrictWeakOrdering, bool parallel>
      struct MaxTreeAlgorithmUFRank
      {
	static constexpr bool level_compression = false;// value_traits<V>::quant < 18;
	static constexpr bool use_dejavu = false;
	static constexpr std::size_t UNINITIALIZED = std::numeric_limits<std::size_t>::max();

	struct aux_data
	{
	  unsigned	rank;
	  std::size_t	zpar;
	  std::size_t	repr;
	};


        MaxTreeAlgorithmUFRank(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp)
          : m_ima (ima), m_nbh (nbh), m_cmp(cmp), m_has_previous(false)
        {
          resize(m_parent, ima);
	  aux_data x = {0, UNINITIALIZED, 0};
	  resize(m_aux, ima, ima.border(), x);

	  std::size_t n = m_ima.domain().size();
	  m_S = new std::vector<std::size_t>(n);
	  m_nsplit = 0;
        }


        MaxTreeAlgorithmUFRank(MaxTreeAlgorithmUFRank& other, tbb::split)
          : m_ima(other.m_ima), m_nbh(other.m_nbh), m_cmp(other.m_cmp), m_parent(other.m_parent),
            m_aux(other.m_aux), m_has_previous(false), m_S (other.m_S)
        {
	  m_nsplit = 0;
        }

	std::size_t
	zfindroot(image2d<aux_data>& aux, std::size_t p)
	{
	  std::size_t q = aux[p].zpar;
	  if (p != q)
	    return aux[p].zpar = zfindroot(aux, q);
	  else
	    return q;
	}


	void
	unionfind(const grain_box2d& domain)
	{
          image2d<V> ima = m_ima | domain;
          image2d<std::size_t>  parent = m_parent | domain;
          image2d<aux_data>  aux = m_aux | domain;
          image2d<bool> deja_vu;

	  if (use_dejavu)
	    resize(deja_vu, ima, m_ima.border(), false);

	  const box2d& d = m_ima.domain();
	  std::size_t i = (d.pmax[1] - d.pmin[1]) * (domain.pmin[0] - d.pmin[0]);
	  std::size_t* S = m_S->data() + i;

	  //int djvu_offset = deja_vu.index_of_point(domain.pmin) - ima.index_of_point(domain.pmin);
          sort_indexes_it(ima, S, m_cmp);
	  auto dindexes = wrt_delta_index(ima, m_nbh.dpoints);
	  std::size_t* R = S + ima.domain().size();

	  std::size_t first_index = ima.index_of_point(ima.domain().pmin);
	  std::size_t last_index = ima.index_of_point(ima.domain().pmax) - ima.index_strides()[0];

          for (int i = ima.domain().size()-1; i >= 0; --i)
            {
	      std::size_t p = S[i];
              // make set
              {
                parent[p] = p;
                aux[p].zpar = p;
		aux[p].repr = p;
		if (use_dejavu)
		  deja_vu[p] = true;
              }

	      std::size_t z = p; // zpar of p
	      std::size_t x = p; // aux[z].repr
              for (unsigned j = 0; j < dindexes.size(); ++j)
		{
		  std::size_t n = p + dindexes[j];

		  bool processed;
		  if (use_dejavu)
		    processed = deja_vu[n];
		  else if (!parallel) // no bound checking
		    processed = (aux[n].zpar != UNINITIALIZED);
		  else
		    processed = (aux[n].zpar != UNINITIALIZED) and (first_index <= n and n < last_index);

		  if (processed)
                  {
		    std::size_t r = zfindroot(aux, n);
                    if (r != z) // make union
                      {
			if (level_compression and ima[aux[r].repr] == ima[x]) // FIXME: use !(b>a) for a==b because ima[x]<=ima[repr[r]]
			  std::swap(x, aux[r].repr);
			parent[aux[r].repr] = x;

			if (level_compression and !parallel)
			  *(--R) = aux[r].repr;

			if (aux[z].rank < aux[r].rank) { //we merge z to r
			  aux[z].zpar = r;
			  aux[r].repr = x;
			  z = r;
			} else if (aux[z].rank > aux[r].rank) { // merge r to z
			  aux[r].zpar = z;
			  aux[z].repr = x;
			} else { // same height
			  aux[r].zpar = z;
			  aux[z].repr = x;
			  aux[z].rank += 1;
			}
                      }
                  }
		}
            }
	  if (level_compression and !parallel) {
	    *(--R) = parent[S[0]];
	    assert(R == S);
	    check_S(parent, S, S + ima.domain().size());
	  }
	}

        void
        operator() (const box2d& domain)
        {
	  //std::cout << domain << std::endl;
	  unionfind(domain);

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
        image2d<aux_data>	     m_aux;

        bool	          m_has_previous;
        box2d	          m_current_domain;

        std::vector<std::size_t>* m_S;
	unsigned	     m_nsplit;
      };


      namespace parallel
      {
	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	std::pair< image2d<std::size_t>, std::vector<std::size_t> >
	maxtree_ufindrank(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUFRank<V, Neighborhood, StrictWeakOrdering, true> algo(ima, nbh, cmp);
	  int grain = std::max(ima.nrows() / 64, 1u);
	  std::cout << "Grain: " << grain << std::endl;
	  tbb::parallel_reduce(grain_box2d(ima.domain(), grain), algo, tbb::auto_partitioner());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;

	  image2d<std::size_t>& parent = algo.m_parent;
	  std::vector<std::size_t> S = std::move(*(algo.m_S));
	  delete algo.m_S;

	  canonize(ima, parent, &S[0]);
	  return std::make_pair(std::move(parent), std::move(S));
	}

      }


      namespace serial
      {

	template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
	std::pair<image2d<std::size_t>, std::vector<std::size_t> >
	maxtree_ufindrank(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
	{
	  MaxTreeAlgorithmUFRank<V, Neighborhood, StrictWeakOrdering, false> algo(ima, nbh, cmp);
	  algo(ima.domain());

	  std::cout << "Number of split: " << algo.m_nsplit << std::endl;

	  std::vector<std::size_t> S = std::move(*(algo.m_S));
	  delete algo.m_S;

	  // canonization
	  // image2d<std::size_t>& parent = algo.m_parent;
	  // for (std::size_t p : S)
	  //   {
	  //     std::size_t q = parent[p];
	  //     if (ima[parent[q]] == ima[q])
	  // 	parent[p] = parent[q];
	  //   }
	  canonize(ima, S, algo.m_parent);
	  return std::make_pair(std::move(algo.m_parent), std::move(S));
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
