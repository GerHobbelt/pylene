#pragma once

#include <mln/core/concept/new/neighborhoods.hpp>
#include <mln/core/concept/new/images.hpp>

#include <mln/morpho/experimental/canvas/depthfirst.hpp>
#include <mln/morpho/experimental/component_tree.hpp>
#include <mln/core/dontcare.hpp>
#include <mln/core/algorithm/for_each.hpp>

#include <vector>

namespace mln::morpho::experimental
{

  template <class I, class N>
  std::pair<component_tree<image_value_t<I>>, image_ch_value_t<I, component_tree<>::node_id_type>> //
  maxtree(I input, N nbh);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace details
  {
    template <class I>
    struct maxtree_visitor
    {
      using point_t  = image_point_t<I>;
      using level_t = int;
      using node_id_t = component_tree<>::node_id_type;

      constexpr void on_flood_start(level_t level, dontcare_t) noexcept
      {
        nodes.push_back({-1});
        lvlnodes.push_back(level);
        roots.push_back({static_cast<node_id_t>(nodes.size() - 1), level});
      }

      void on_flood_end([[maybe_unused]] level_t old_level, level_t new_level) noexcept
      {
        // Attach to parent
        assert(!roots.empty());

        auto current = roots.back();
        roots.pop_back();

        assert(old_level == current.level);
        assert(old_level > new_level);

        // Fixme: optimize this test with a sentinel value

        root_info par;
        if (!roots.empty())
          par = roots.back();

        if (roots.empty() || par.level != new_level)
        {
          nodes.push_back({-1});
          lvlnodes.push_back(new_level);
          par = {static_cast<node_id_t>(nodes.size() - 1), new_level};
          roots.push_back(par);
        }
        assert(par.level <= new_level);

        nodes[current.node_root_id] = par.node_root_id;
      }

      void on_flood_end(dontcare_t) noexcept
      {
        assert(!roots.empty());
        // Attach to parent
        node_id_t root = roots.back().node_root_id;
        roots.pop_back();

        assert(roots.empty());
        nodes[root] = -1;
      }

      void on_done(level_t, point_t p) { node_map(p) = roots.back().node_root_id; }


      constexpr bool has_level_flooding_started(dontcare_t) const noexcept { return true; }

      struct root_info
      {
        node_id_t node_root_id;
        level_t   level;
      };




      std::vector<root_info>                        roots;
      std::vector<int>                              nodes;
      std::vector<image_value_t<I>>                 lvlnodes;
      image_ch_value_t<I, node_id_t>                node_map;
    };
  } // namespace details

  namespace details
  {

    /// Permute the array \p tab considering the permutation array \p permut
    ///
    /// \param count Number of elements
    /// \param size Size of the elements (in bytes)
    void permute_array(int* permut, void* tab, std::size_t count, std::size_t size);


    /// Sort the array \c par so that par[i] < i
    /// where `par` encodes a DAG relation
    /// The parent of the root node is supposed to be -1;
    /// permut[-1] is supposed to be a valid memory location
    void permute_parent(int* par, int* permut, std::size_t n);


    template <class I, class V>
    [[gnu::noinline]] //
    void
    permute_parent_and_node_map(I&& node_map, int* parent, V* levels, std::size_t n)
    {
      auto permutation_arr = std::make_unique<int[]>(n + 1);
      int* perm = permutation_arr.get() + 1;

      permute_parent(parent, perm, n);
      permute_array(perm, levels, n, sizeof(V));
      mln::for_each(node_map, [perm](int& i) { i = perm[i]; });
    }
  } // namespace details

  template <class I, class N>
  std::pair<component_tree<image_value_t<I>>, image_ch_value_t<I, component_tree<>::node_id_type>> //
  maxtree(I input, N nbh)
  {
    using V = image_value_t<I>;
    constexpr std::size_t kStackReserve = 256;

    details::maxtree_visitor<I> viz;
    viz.roots.reserve(kStackReserve);
    mln::resize(viz.node_map, input);

    // FIXME: implement ::ranges::first() that return the first element of the domain
    image_point_t<I> pstart = input.domain().tl();
    canvas::depthfirst(input, nbh, viz, pstart);

    // Reoder the parent array
    details::permute_parent_and_node_map(viz.node_map, viz.nodes.data(), viz.lvlnodes.data(), viz.nodes.size());

    component_tree<V> ct;
    ct.parent = std::move(viz.nodes);
    ct.values = std::move(viz.lvlnodes);

    return { std::move(ct), std::move(viz.node_map) };
  }



} // namespace mln::morpho::experimental
