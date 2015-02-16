#ifndef MLN_MORPHO_CTOS_IMPL_CTOS_PARALLEL_HPP
# define MLN_MORPHO_CTOS_IMPL_CTOS_PARALLEL_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/trace.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/wrt_offset.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/algorithm/fill.hpp>
# include <mln/morpho/tos/irange.hpp>
# include <mln/morpho/tos/immerse.hpp>
# include <mln/morpho/tos/pset.hpp>
# include <mln/morpho/tos/pset_priority.hpp>
# include <mln/morpho/maxtree/maxtree.hpp>
# include <mln/morpho/datastruct/component_tree.hpp>
# include <mln/core/image/morphers/casted_image.hpp>

namespace mln
{

  namespace morpho
  {

    namespace impl
    {

      namespace parallel
      {


        template <typename I,
                  typename Neighborhood,
                  typename Compare,
                  typename Equiv,
                  bool use_priority = false>
        morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
        cToS(const Image<I>& ima,
             const Neighborhood& nbh,
             mln_point(I) pmin,
             const Compare& cmp,
             const Equiv& eq);



        /********************/
        /** Implementation **/
        /********************/


        template <typename I,
                  typename Neighborhood,
                  typename Compare,
                  typename Equiv,
                  bool use_priority>
        morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
        cToS(const Image<I>& ima_,
             const Neighborhood& nbh,
             mln_point(I) pmin,
             const Compare& cmp,
             const Equiv& eq)
        {
          mln_entering("mln::morpho:impl::parallel::tos");
          using namespace mln::morpho::tos;

          typedef mln_value(I) V;
          typedef irange<V> R;
          typedef typename I::size_type size_type;

          static constexpr size_type UNPROCESSED = value_traits<size_type>::max();
          static constexpr size_type PROCESSED = 0;

          const I& ima = exact(ima_);

          // f: image of interval in Khalimsky space
          // K: image of value in Khalimsky that tells at which a level a point is inserted
          // D: depth image
          mln_ch_value(I, R)              f = tos::internal::immerse(ima, cmp);
          mln_ch_value(I, size_type)      D = imchvalue<size_type>(f).init(UNPROCESSED);

          extension::fill(D, PROCESSED);


          typedef typename std::conditional<not use_priority,
                                            pset<mln_concrete(I), Compare>,
                                            pset_priority<mln_concrete(I), Compare> >::type pset_t;

        auto dindexes = wrt_delta_index(f, nbh.dpoints);
        size_type depth = 0;

        // 1. Propagation
        {
          mln_entering("mln::morpho::impl::parallel::tos - propagation");

          mln_concrete(I)                 K(f, mln::init());

          pset_t W(K, cmp);
          size_type p = f.index_of_point(pmin * 2);
          W.insert(p);
          D[p] = 0;
          K[p] = f[p].lower;

          V prev_level = K[p];
          while (!W.empty())
            {
              p = W.has_next(p) ? W.pop_next(p) : W.pop_previous(p);
              V curlevel = K[p];
              if (not eq(curlevel, prev_level))
                ++depth;
              D[p] = depth;

              mln_foreach (int k, dindexes)
                {
                  size_type q = p + k;
                  if (D[q] == UNPROCESSED)
                    {
                      if (cmp(f[q].upper, curlevel))
                        K[q] = f[q].upper;
                      else if (cmp(curlevel, f[q].lower))
                        K[q] = f[q].lower;
                      else
                        K[q] = curlevel;

                      D[q] = PROCESSED;
                      W.insert(q);
                    }
                }
              prev_level = curlevel;
            }

          // Release f memory (no more needed)
          f = decltype(f) ();

          mln_exiting();
        }

        // 2. Max-tree of the depth image
        morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)> tree;
        {
          if (depth < value_traits<uint16>::max())
            {
              auto D2 = eval(imcast<uint16>(D));
              tree = morpho::maxtree_indexes(D2, nbh);
            }
          else
            {
              tree = morpho::maxtree_indexes(D, nbh);
            }
        }

        // 3. Ensure that the first point in node pset is a 2F
        {
          mln_entering("mln::morpho::impl::parallel::tos - postprocessing");
          auto& pmap = tree._get_data()->m_pmap;
          auto& nodes = tree._get_data()->m_nodes;
          auto& S = tree._get_data()->m_S;

          mln_ch_value(I, bool) is2F = imchvalue<bool>(D).init(false);
          auto dom = is2F.domain();
          fill(is2F | sbox2d{dom.pmin, dom.pmax, {2,2}}, true);

          for (unsigned p = 0; p < S.size(); ++p)
            {
              if (is2F[S[p]]) {
                unsigned q = nodes[pmap[S[p]]].m_point_index;
                if (not is2F[S[q]]) {
                  std::swap(S[p], S[q]);
                }
              }
            }
          mln_exiting();
        }

        mln_exiting();
        return tree;
        }

      }

    }

  }

}

#endif // ! MLN_MORPHO_CTOS_IMPL_CTOS_PARALLEL_HPP
