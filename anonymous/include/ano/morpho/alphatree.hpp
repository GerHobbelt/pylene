#pragma once


#include <ano/core/concepts/image.hpp>
#include <ano/core/concepts/neighborhood.hpp>

#include <ano/core/algorithm/for_each.hpp>
#include <ano/core/functional_ops.hpp>
#include <ano/morpho/canvas/unionfind.hpp>
#include <ano/morpho/component_tree.hpp>

#include <ano/morpho/private/directional_hqueue.hpp>

#include <algorithm>
#include <type_traits>

#include <range/v3/functional/concepts.hpp>

namespace ano::morpho
{

  /// Compute the alphatree of an image
  ///
  /// \param input The input image
  /// \param neighborhood The neighborhood relation
  /// \param distance Distance function
  template <class I, class N, class F = ano::functional::l2dist_t<>>
  std::pair<component_tree<std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>, image_ch_value_t<I, int>> //
  alphatree(I input, N nbh, F distance = F{});


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace internal
  {
    /// \brief Canvas for the edges in the alphatree. Using different data
    /// structures related to the type of the edges.
    template <typename P, typename N, typename W, bool HQ>
    class alphatree_edges;

    template <typename P, typename N, typename W, bool HQ>
    requires(std::is_integral_v<W>&& std::is_unsigned_v<W> && sizeof(W) <= 2 && HQ) class alphatree_edges<P, N, W, HQ>
    {
    public:
      void                push(int dir, W w, P p) { m_cont.insert(dir, w, p); }
      std::tuple<P, P, W> pop() { return m_cont.pop(); }
      W                   top() const { return m_cont.current_level(); }
      bool                empty() const { return m_cont.empty(); }
      void                on_finish_insert() {}

    private:
      details::directional_hqueue<P, N, W> m_cont;
    };

    template <typename P, typename W>
    struct edge_t
    {
      P p;
      P q;
      W w;
    };

    template <typename P, typename N, typename W, bool HQ>
    class alphatree_edges
    {
    public:
      void push(int dir, W w, P p) { m_cont.push_back({p, p + cn.after_offsets()[dir], w}); }

      void push(P p, P q, W w) { m_cont.push_back({p, q, w}); }

      std::tuple<P, P, W> pop()
      {
        assert(m_current < m_cont.size());
        const auto e = m_cont[m_current++];
        return {std::move(e.p), std::move(e.q), std::move(e.w)};
      }

      W top() const
      {
        assert(m_current < m_cont.size());
        return m_cont[m_current].w;
      }

      bool empty() const { return m_cont.size() == m_current; }

      void on_finish_insert()
      {
        std::stable_sort(m_cont.begin(), m_cont.end(),
                         [](const edge_t<P, W>& a, const edge_t<P, W>& b) { return a.w < b.w; });
      }

    private:
      static constexpr auto     cn = N();
      std::vector<edge_t<P, W>> m_cont;
      std::size_t               m_current = 0;
    };


    // Compute a node_id for each flat zone
    template <class I, class J>
    [[gnu::noinline]] std::size_t alphatree_create_nodemap(I node_map, J zpar)
    {
      std::size_t node_count = 0;
      ano_foreach (auto px, node_map.pixels())
      {
        auto p  = px.point();
        auto rp = canvas::impl::zfindroot(zpar, p);
        if (node_map(rp) < 0)
          node_map(rp) = static_cast<int>(node_count++);
        node_map(p) = node_map(rp);
      }
      return node_count;
    }

    // Compute flat zone of the image using union-find structures
    // It returns the number of edges processed
    template <class E, class J>
    void alphatree_compute_flatzones(E& edges, J zpar)
    {
      while (!edges.empty() && edges.top() == 0)
      {
        const auto [p, q, w] = edges.pop();
        auto rp              = canvas::impl::zfindroot(zpar, p);
        auto rq              = canvas::impl::zfindroot(zpar, q);
        if (rp != rq) // Merge set
          zpar(rq) = rp;
      }
    }

    // Inverse the node array, updating the parent relation
    template <class W>
    [[gnu::noinline]] void alphatree_reorder_nodes(int* parent, W* levels, std::size_t n)
    {
      std::size_t i = 0;
      std::size_t j = n - 1;

      while (i < j)
      {
        std::swap(levels[i], levels[j]);
        std::swap(parent[i], parent[j]);
        parent[i] = static_cast<int>(n) - parent[i] - 1;
        parent[j] = static_cast<int>(n) - parent[j] - 1;
        i++;
        j--;
      }
      if (i == j)
        parent[i] = static_cast<int>(n) - parent[i] - 1;
    }


    template <class I, class N, class F, class C>
    void alphatree_compute_edges(I input, N nbh, F distance, C& edges)
    {
      static_assert(is_a_v<I, ano::details::Image>);

      auto dom = input.domain();
      ano_foreach (auto p, dom)
      {
        int i = 0;
        for (auto n : nbh.after(p))
        {
          if (dom.has(n))
            edges.push(i, distance(input(p), input(n)), p);
          ++i;
        }
      }
      edges.on_finish_insert();
    }

    //
    //
    template <class E, class I, class W, class M>
    std::size_t alphatree_compute_hierarchy(E& edges, I node_map,         //
                                            std::size_t       node_count, //
                                            std::vector<int>& par,        //
                                            std::vector<W>&   levels,     //
                                            std::vector<M>*   mst,        //
                                            bool              canonize_tree)
    {
      static_assert(ano::is_a<I, ano::details::Image>());

      std::vector<int> zpar(node_count);  // Parent of ufind structure
      std::vector<int> links(node_count); // Node index in the alpha tree

      std::iota(std::begin(zpar), std::end(zpar), 0);
      std::iota(std::begin(links), std::end(links), 0);

      while (!edges.empty())
      {
        auto [p, q, w] = edges.pop();
        auto rp        = canvas::impl::zfindroot(zpar.data(), node_map(p));
        auto rq        = canvas::impl::zfindroot(zpar.data(), node_map(q));

        if (rp != rq)
        {
          // Merge set

          assert(levels[rp] <= w && levels[rq] <= w);
          zpar[rq] = rp;

          // Do we need to create a new node
          int rp_root = links[rp];
          int rq_root = links[rq];

          int max_root = std::max(rp_root, rq_root);
          int min_root = std::min(rp_root, rq_root);

          int new_root_id;
          if (canonize_tree && levels[max_root] == w)
          {
            new_root_id = max_root;
          }
          else if (canonize_tree && levels[min_root] == w)
          {
            new_root_id = min_root;
          }
          else
          {
            // Create a new_node
            new_root_id = static_cast<int>(node_count++);
            par.push_back(new_root_id);
            levels.push_back(w);
          }

          par[rp_root] = new_root_id;
          par[rq_root] = new_root_id;
          links[rp]    = new_root_id;

          if (mst != nullptr)
            mst->push_back({p, q, w});
        }
      }
      return node_count;
    }

    template <class W, class I>
    std::pair<std::vector<int>, std::vector<W>> canonize_component_tree(const std::vector<int>& par,    //
                                                                        const std::vector<W>&   levels, //
                                                                        I                       node_map)
    {
      std::vector<int> canonized_par;
      std::vector<W>   canonized_levels;

      // Root initialization

      canonized_par.push_back(0);
      canonized_levels.push_back(levels[0]);

      std::vector<int> translation_map(par.size());

      translation_map[0] = 0;
      int count          = 1;

      // Build canonized component tree
      for (std::size_t i = 1; i < par.size(); ++i)
      {
        if (levels[i] != levels[par[i]]) // Keep the node: Update tree
        {
          translation_map[i] = count++;
          canonized_par.push_back(translation_map[par[i]]);
          canonized_levels.push_back(levels[i]);
        }
        else // Deleted node: retrieve the parent translation
          translation_map[i] = translation_map[par[i]];
      }

      ano::for_each(node_map, [&translation_map, node_count = par.size()](int& id) {
        id = translation_map[static_cast<int>(node_count) - id - 1];
      });

      return {canonized_par, canonized_levels};
    }

    template <typename W, class E, class I, class M = edge_t<image_point_t<I>, W>>
    std::pair<component_tree<W>, image_ch_value_t<I, int>>
    alphatree_from_graph(E& edges, I node_map, std::size_t nb_leaves, bool canonize_tree, std::vector<M>* mst = nullptr)
    {
      std::size_t      node_count = nb_leaves;
      std::vector<int> par(node_count);
      std::vector<W>   levels(node_count, 0);

      std::iota(std::begin(par), std::end(par), 0);
      node_count = internal::alphatree_compute_hierarchy(edges, node_map, node_count, par, levels, mst, canonize_tree);

      // Parent / levels are ordered from leaves to root, we need to reverse
      internal::alphatree_reorder_nodes(par.data(), levels.data(), node_count);

      component_tree<W> t;
      if (canonize_tree)
      {
        auto [canonized_par, canonized_levels] = internal::canonize_component_tree(par, levels, node_map);
        t.parent                               = std::move(canonized_par);
        t.values                               = std::move(canonized_levels);
      }
      else
      {
        t.parent = std::move(par);
        t.values = std::move(levels);
        ano::for_each(node_map, [node_count](int& id) { id = static_cast<int>(node_count) - id - 1; });
      }

      return {std::move(t), std::move(node_map)};
    }

    template <bool HQ = true, class I, class N, class F,
              class M = edge_t<image_point_t<I>, std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>>
    std::pair<component_tree<std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>, image_ch_value_t<I, int>> //
    __alphatree(I input, N nbh, F distance, bool canonize_tree = true, bool compute_flatzones = true,
                std::vector<M>* mst = nullptr)
    {
      static_assert(ano::is_a<I, ano::details::Image>());
      static_assert(ano::is_a<N, ano::details::Neighborhood>());
      static_assert(::ranges::cpp20::invocable<F, image_value_t<I>, image_value_t<I>>);

      using V = image_value_t<I>;
      using P = image_point_t<I>;
      using W = std::invoke_result_t<F, V, V>;

      static_assert(::concepts::totally_ordered<W>);
      static_assert(std::is_same<M, edge_t<P, W>>());

      // 1. Get the list of edges
      auto edges = alphatree_edges<P, N, W, HQ>();
      internal::alphatree_compute_edges(std::move(input), std::move(nbh), std::move(distance), edges);

      std::size_t              flatzones_count;
      image_ch_value_t<I, int> node_map = imchvalue<int>(input).set_init_value(-1);
      {
        image_ch_value_t<I, P> zpar = imchvalue<P>(input);
        canvas::impl::union_find_init_par(zpar);

        if (compute_flatzones)
          internal::alphatree_compute_flatzones(edges, zpar);

        // 3. Compute a node_id for each flat zone
        flatzones_count = internal::alphatree_create_nodemap(node_map, zpar);
      }

      return alphatree_from_graph<W>(edges, node_map, flatzones_count, canonize_tree, mst);
    }
  } // namespace internal

  template <class I, class N, class F>
  std::pair<component_tree<std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>, image_ch_value_t<I, int>> //
  alphatree(I input, N nbh, F distance)
  {
    return internal::__alphatree(input, nbh, distance);
  }

} // namespace ano::morpho