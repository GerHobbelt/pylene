#ifndef MLN_MORPHO_TOS_PRIVATE_PROPAGATION_HPP
# define MLN_MORPHO_TOS_PRIVATE_PROPAGATION_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/core/neighb2d.hpp>
# include <mln/core/trace.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/wrt_offset.hpp>
# include "pset.hpp"

namespace mln
{
  namespace morpho
  {
    namespace tos
    {
      namespace impl
      {

        template <class V>
        image2d<int>
        propagation(const image2d<irange<V>>& f,
                    point2d start_point,
                    int& max_depth,
                    std::vector<unsigned>* sorted_indexes = nullptr);


        template <class V, bool compute_indexes>
        image2d<int>
        __propagation(const image2d<irange<V>>& f,
                      point2d start_point,
                      int& max_depth,
                      std::vector<unsigned>* sorted_indexes)
        {
          mln_entering("mln::morpho::tos::impl::propagation");

          using P = typename image2d<V>::size_type;

          enum { UNPROCESSED = -1, PROCESSED = 0 };

          // Ord will hold the depth of the pixel during
          // the propagation, i.e. when it is poped from the queue
          image2d<int> ord = imchvalue<int>(f).init(UNPROCESSED).adjust(c4);
          extension::fill(ord, PROCESSED);

          if (compute_indexes)
            sorted_indexes->reserve(ord.domain().size());

          pset<image2d<V>> queue(f);

          auto p = f.index_of_point(start_point);
          V previous_level = f[p].lower;
          queue.insert(previous_level, p);
          ord[p] = 0;

          if (compute_indexes)
            sorted_indexes->push_back(p);

          auto dindexes = wrt_delta_index(f, c4.dpoints);
          int depth = 0;
          while (!queue.empty())
            {
              V cur_level;
              P p;

              auto tmp = queue.try_pop(previous_level);
              if (tmp.first)
                {
                  cur_level = previous_level;
                  p = tmp.second;
                }
              else
                {
                  std::tie(cur_level, p) = queue.pop(previous_level);
                  ++depth;
                }
              ord[p] = depth;

              if (compute_indexes)
                sorted_indexes->push_back(p);

              mln_foreach (int k, dindexes)
                {
                  P q = p + k;
                  if (ord[q] == UNPROCESSED)
                    {
                      if (f[q].upper < cur_level)
                        queue.insert(f[q].upper, q);
                      else if (cur_level < f[q].lower)
                        queue.insert(f[q].lower, q);
                      else
                        queue.insert(cur_level, q);
                      ord[q] = PROCESSED;
                    }
                }
              previous_level = cur_level;
            }
          max_depth = depth;
          return ord;
        }

        template <class V>
        image2d<int>
        propagation(const image2d<irange<V>>& f,
                    point2d start_point,
                    int& max_depth,
                    std::vector<unsigned>* sorted_indexes)
        {
          if (sorted_indexes == nullptr)
            return __propagation<V, false>(f, start_point, max_depth, sorted_indexes);
          else
            return __propagation<V, true>(f, start_point, max_depth, sorted_indexes);
        }


      } // end of namespace mln::morpho::tos::impl
    }   // end of namespace mln::morpho::tos
  }     // end of namespace mln::morpho
}       // end of namespace mln

#endif //!MLN_MORPHO_TOS_PRIVATE_PROPAGATION_HPP
