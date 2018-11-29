#ifndef MLN_CORE_MORPHO_MAXTREE_PQUEUE_2_HPP
#define MLN_CORE_MORPHO_MAXTREE_PQUEUE_2_HPP

#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/value/value_traits.hpp>
#include <mln/core/wrt_offset.hpp>
#include <mln/morpho/maxtree_node.hpp>
#include <mln/morpho/pqueue_fast.hpp>
#include <queue>
#include <stack>
#include <vector>

#include <mln/io/imprint.hpp>

namespace mln
{

  namespace morpho
  {

    namespace internal
    {

      template <typename V, typename Neighborhood, typename StrictWeakOrdering>
      typename image2d<V>::size_type maxtree_flood_pqueue_algorithm_2(const image2d<V>&      ima,
                                                                      image2d<maxtree_node>& tree, const Neighborhood&,
                                                                      StrictWeakOrdering cmp, unsigned cpos = 0)
      {
        typedef typename image2d<V>::size_type size_type;
        static constexpr bool                  use_dejavu    = true;
        static constexpr size_type             UNINITIALIZED = std::numeric_limits<size_type>::max();
        static constexpr size_type             INQUEUE       = 0;
        static constexpr std::size_t           nlevels       = (std::size_t)1 << value_traits<V>::quant;

        image2d<bool>                  deja_vu;
        typedef std::vector<size_type> vec_t;
        vec_t                          v_stack;
        v_stack.reserve(std::min<std::size_t>(ima.domain().size(), nlevels));

        std::stack<size_type, vec_t>              stack(std::move(v_stack));
        priority_queue_ima<V, StrictWeakOrdering> pqueue(ima, cmp);
        auto                                      nbh_delta_indexes = wrt_delta_index(ima, Neighborhood::dpoints);

        size_type first_index = ima.index_of_point(ima.domain().pmin);
        size_type last_index  = ima.index_of_point(ima.domain().pmax) - ima.index_strides()[0];

        // INIT
        {
          if (use_dejavu)
          {
            resize(deja_vu, ima).init(false);
            extension::fill(deja_vu, true);
          }

          // Get min element and reserve queue
          size_type pmin = ima.index_of_point(ima.domain().pmin);
          {
            pqueue.push(pmin);
            stack.push(pmin);
            if (use_dejavu)
              deja_vu[pmin] = true;
            else
              tree[pmin].m_parent = 0;
          }
        }

        size_type Send  = UNINITIALIZED;
        size_type lastp = UNINITIALIZED;

        cpos += ima.domain().size();
        assert((double)cpos != (double)(cpos - 1));

        while (!pqueue.empty())
        {
        flood:
          size_type p    = pqueue.top();
          size_type repr = stack.top();
          assert(ima[p] == ima[repr]);

          mln_foreach (auto k, nbh_delta_indexes)
          {
            auto q = p + k;
            bool processed;
            if (use_dejavu)
              processed = deja_vu[q];
            else
              processed = !(first_index <= q and q < last_index) or (tree[q].m_parent != UNINITIALIZED);

            if (!processed)
            {
              pqueue.push(q);
              if (use_dejavu)
                deja_vu[q] = true;
              else
                tree[q].m_parent = INQUEUE;
              if (cmp(ima[p], ima[q]))
              {
                stack.push(q);
                goto flood;
              }
            }
          }

          // p done
          pqueue.pop();
          tree[p].m_parent = repr;
          if (p != repr)
          {
            // Insert p in the linked list of point
            if (lastp != UNINITIALIZED)
              tree[lastp].m_prev = p;
            else
              Send = p;
            tree[p].m_next = lastp;
            tree[p].m_pos  = --cpos;
            lastp          = p;
          }

          // Stack handle : attach to parent
          if (pqueue.empty())
            break;
          size_type next = pqueue.top();
          assert(!cmp(ima[repr], ima[next]));
          if (cmp(ima[next], ima[repr]))
          {
            // size_type par;
            stack.pop();
            while (!stack.empty() and cmp(ima[next], ima[stack.top()]))
            {
              // Add the repr
              {
                if (lastp != UNINITIALIZED)
                  tree[lastp].m_prev = repr;
                else
                  Send = repr;

                tree[lastp].m_prev = repr;
                tree[repr].m_next  = lastp;
                tree[repr].m_pos   = --cpos;
                lastp              = repr;
              }
              repr = (tree[repr].m_parent = stack.top());
              stack.pop();
            }
            if (stack.empty() or cmp(ima[stack.top()], ima[next]))
              stack.push(next);

            tree[repr].m_parent = stack.top();
            // Add the repr
            {
              if (lastp != UNINITIALIZED)
                tree[lastp].m_prev = repr;
              else
                Send = repr;

              tree[repr].m_next = lastp;
              tree[repr].m_pos  = --cpos;
              lastp             = repr;
            }
          }
        }

        assert(stack.size() == 1);

        size_type root = stack.top();
        if (lastp != UNINITIALIZED)
          tree[lastp].m_prev = root;
        else
          Send = root;
        tree[root].m_next = lastp;
        tree[root].m_pos  = --cpos;
        tree[root].m_prev = Send;

        // return the root point
        return root;
      }

    } // end of namespace mln::morpho::internal

    template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V>>
    std::pair<image2d<maxtree_node>, typename image2d<V>::size_type>
        maxtree_pqueue_2(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
    {
      typedef typename image2d<V>::size_type size_type;
      image2d<maxtree_node>                  tree;
      resize(tree, ima);

      size_type root = internal::maxtree_flood_pqueue_algorithm_2(ima, tree, nbh, cmp);
      return std::make_pair(tree, root);
    }

  } // end of namespace mln::morpho

} // end of namespace mln

#endif // ! MLN_CORE_MORPHO_MAXTREE_PQUEUE_2_HPP
