#ifndef MLN_MORPHO_TOS_PRIVATE_PROPAGATION_HPP
# define MLN_MORPHO_TOS_PRIVATE_PROPAGATION_HPP

#include <mln/core/image/image.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/neighb3d.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/wrt_offset.hpp>
#include <mln/morpho/tos/private/pset.hpp>

namespace mln
{
  namespace morpho
  {
    namespace ToS
    {
      namespace impl
      {

        template <class I>
        mln_ch_value(I, int)
        propagation(const Image<I>& f,
                    mln_point(I) start_point,
                    int& max_depth,
                    std::vector<typename I::index_type>* sorted_indexes = nullptr);

        /******************************************/
        /****          Implementation          ****/
        /******************************************/


        template <class I, bool compute_indexes>
        mln_ch_value(I, int)
        propagation_impl(const Image<I>& f_,
                         mln_point(I) start_point,
                         int& max_depth,
                         std::vector<typename I::index_type>* sorted_indexes)
        {
          mln_entering("mln::morpho::tos::impl::propagation");

          using range_t = mln_value(I);
          using V = typename range_t::value_type;
          using P = typename I::size_type;
          using point_t = mln_point(I);
          using connectivity_t = std::conditional_t<point_t::ndim == 2, c4_t, c6_t>;

          enum { UNPROCESSED = -1, PROCESSED = 0 };

          const auto& f = exact(f_);

          // Ord will hold the depth of the pixel during
          // the propagation, i.e. when it is poped from the queue
          connectivity_t conn;

          mln_ch_value(I, int) ord = imchvalue<int>(f).init(UNPROCESSED).adjust(conn);
          extension::fill(ord, PROCESSED);

          if (compute_indexes)
            sorted_indexes->reserve(ord.domain().size());

          pset<mln_ch_value(I, V)> queue(f);

          auto p = f.index_of_point(start_point);
          V previous_level = f[p].lower;
          queue.insert(previous_level, p);
          ord[p] = 0;

          if (compute_indexes)
            sorted_indexes->push_back(p);

          auto dindexes = wrt_delta_index(f, conn.dpoints);
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

        template <class I>
        mln_ch_value(I, int)
        propagation(const Image<I>& f,
                    mln_point(I) start_point,
                    int& max_depth,
                    std::vector<typename I::index_type>* sorted_indexes)
        {
          if (sorted_indexes == nullptr)
            return propagation_impl<I, false>(f, start_point, max_depth, sorted_indexes);
          else
            return propagation_impl<I, true>(f, start_point, max_depth, sorted_indexes);
        }

      } // end of namespace mln::morpho::tos::impl
    }   // end of namespace mln::morpho::tos
  }     // end of namespace mln::morpho
}       // end of namespace mln

#endif //!MLN_MORPHO_TOS_PRIVATE_PROPAGATION_HPP
