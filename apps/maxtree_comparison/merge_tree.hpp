#pragma once

#include "maxtree_routines.hpp"

#include <mln/core/neighb2d.hpp>

#include <thread>


namespace mln
{

  namespace morpho
  {

    // tree merging
    template <typename V, typename StrictWeakOrdering>
    void merge_tree(const image2d<V>& ima, const c4_t& _nbh, image2d<typename image2d<V>::size_type>& parent,
                    box2d domain, StrictWeakOrdering cmp)
    {
      typedef typename image2d<V>::size_type size_type;
      mln_precondition(!domain.empty());

      (void)_nbh;

      point2d p_  = domain.pmin;
      point2d q_  = domain.pmin;
      p_[0]       = domain.pmax[0] - 1;
      q_[0]       = domain.pmax[0];
      size_type p = ima.index_of_point(p_);
      size_type q = ima.index_of_point(q_);

      unsigned ncols = ima.ncols();
      for (unsigned i = 0; i < ncols; ++i, ++p, ++q)
      {
        size_type x = internal::zfind_repr(ima, parent, p);
        size_type y = internal::zfind_repr(ima, parent, q);
        if (cmp(ima[x], ima[y]))
          std::swap(x, y);

        while (x != y)
        {
          // std::cout << "-- Merge: " << x << " @ " << y << std::endl;
          // check that x and y are representative
          mln_assertion(x == parent[x] or ima[parent[x]] != ima[x]);
          mln_assertion(y == parent[y] or ima[parent[y]] != ima[y]);
          mln_assertion(!cmp(ima[x], ima[y])); // ima(y) <= ima(x)

          // we want to attach x to y
          if (parent[x] == x)
          {
            parent[x] = y;
            break;
          }
          else
          {
            size_type z = internal::zfind_parent(ima, parent, x);
            if (!cmp(ima[z], ima[y])) // ima(y) <= ima(z)
              x = z;
            else
            {
              parent[x] = y;
              x         = y;
              y         = z;
            }
          }
        }
      }
    }

    template <typename V, typename StrictWeakOrdering>
    void merge_tree(const image2d<V>& ima, const c8_t& _nbh, image2d<typename image2d<V>::size_type>& parent,
                    box2d domain, StrictWeakOrdering cmp)
    {
      typedef typename image2d<V>::size_type size_type;
      mln_precondition(!domain.empty());
      (void)_nbh;

      point2d p_  = domain.pmin;
      point2d q_  = domain.pmin;
      p_[0]       = domain.pmax[0] - 1;
      q_[0]       = domain.pmax[0];
      size_type p = ima.index_of_point(p_);
      size_type q = ima.index_of_point(q_);

      int ncols = ima.ncols();
      for (int i = 0; i < ncols; ++i, ++p, ++q)
      {
        for (int j = -1; j <= 1; ++j)
          if ((i + j) >= 0 and (i + j) < ncols)
          {

            size_type x = internal::zfind_repr(ima, parent, p);
            size_type y = internal::zfind_repr(ima, parent, q + j);
            if (cmp(ima[x], ima[y]))
              std::swap(x, y);

            while (x != y)
            {
              // std::cout << "-- Merge: " << x << " @ " << y << std::endl;
              // check that x and y are representative
              mln_assertion(x == parent[x] or ima[parent[x]] != ima[x]);
              mln_assertion(y == parent[y] or ima[parent[y]] != ima[y]);
              mln_assertion(!cmp(ima[x], ima[y])); // ima(y) <= ima(x)

              // we want to attach x to y
              if (parent[x] == x)
              {
                parent[x] = y;
                break;
              }
              else
              {
                size_type z = internal::zfind_parent(ima, parent, x);
                if (!cmp(ima[z], ima[y])) // ima(y) <= ima(z)
                  x = z;
                else
                {
                  parent[x] = y;
                  x         = y;
                  y         = z;
                }
              }
            }
          }
      }
    }

    // tree merging
    template <typename V, typename StrictWeakOrdering>
    void merge_tree_3(const image2d<V>& ima, image2d<typename image2d<V>::size_type>& parent, image2d<unsigned>& pos,
                      std::vector<unsigned>& S, box2d domain, StrictWeakOrdering cmp)
    {
      typedef typename image2d<V>::size_type size_type;
      mln_precondition(!domain.empty());

      point2d p_  = domain.pmin;
      point2d q_  = domain.pmin;
      p_[0]       = domain.pmax[0] - 1;
      q_[0]       = domain.pmax[0];
      size_type p = ima.index_of_point(p_);
      size_type q = ima.index_of_point(q_);

      auto move_insert_at = [&pos, &S](unsigned x, int i) {
        assert(pos[x] > (unsigned)i);
        for (int j = pos[x] - 1; j >= i; --j)
        {
          S[j + 1]      = S[j];
          pos[S[j + 1]] = j + 1;
        }
        S[i]   = x;
        pos[x] = i;
      };

      unsigned ncols = ima.ncols();
      for (unsigned i = 0; i < ncols; ++i, ++p, ++q)
      {
        size_type x = internal::zfind_repr(ima, parent, p);
        size_type y = internal::zfind_repr(ima, parent, q);
        if (cmp(ima[x], ima[y]))
          std::swap(x, y);

        while (x != y)
        {
          mln_assertion(x == parent[x] or ima[parent[x]] != ima[x]);
          mln_assertion(y == parent[y] or ima[parent[y]] != ima[y]);
          mln_assertion(!cmp(ima[x], ima[y])); // ima(y) <= ima(x)

          // we want to attach x to y
          if (parent[x] == x)
          {
            parent[x] = y;
            if (pos[x] < pos[y])
              move_insert_at(y, pos[x]);
            break;
          }
          else
          {
            size_type z = internal::zfind_parent(ima, parent, x);
            if (!cmp(ima[z], ima[y])) // ima(y) <= ima(z)
            {
              if (pos[x] < pos[z])
                move_insert_at(z, pos[x]);
              x = z;
            }
            else
            {
              if (pos[x] < pos[y])
                move_insert_at(y, pos[x]);
              parent[x] = y;
              x         = y;
              y         = z;
            }
          }
        }
      }
    }

    template <typename size_type>
    bool check_S(const image2d<size_type>& parent, const size_type* begin, const size_type* end)
    {
      image2d<bool> dejavu;
      resize(dejavu, parent).init(false);

      dejavu[*begin] = true;
      for (; begin != end; ++begin)
      {
        assert(dejavu[parent[*begin]]);
        if (!dejavu[parent[*begin]])
          return false;
        dejavu[*begin] = true;
      }
      return true;
    }
  } // namespace morpho
} // namespace mln
