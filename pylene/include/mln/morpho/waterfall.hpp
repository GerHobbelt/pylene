#pragma once

#include <mln/core/neighborhood/c4c8.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/canvas/kruskal.hpp>
#include <mln/morpho/watershed.hpp>

#include <map>
#include <tuple>
#include <vector>

namespace mln::morpho
{
  namespace details
  {
    class mst_waterfall_visitor
    {
    public:
      void        on_init(int n);
      void        on_union(int p, int rp, int q, int rq, mln::dontcare_t);
      inline void on_processing(mln::dontcare_t, mln::dontcare_t, mln::dontcare_t, mln::dontcare_t, mln::dontcare_t) {}
      inline void on_finish() {}

    public:
      std::vector<std::tuple<int, int, int>> mst;

    private:
      std::vector<int> m_diameter;
      std::vector<int> m_roots;
      int              m_nlbl;
    };

    class waterfall_bpt_visitor
    {
    public:
      void        on_init(int n);
      void        on_union(mln::dontcare_t, int rp, mln::dontcare_t, int rq, int w);
      inline void on_processing(mln::dontcare_t, mln::dontcare_t, mln::dontcare_t, mln::dontcare_t, mln::dontcare_t) {}
      inline void on_finish() {}

    public:
      std::vector<int> value;
      std::vector<int> parent;

    private:
      std::vector<int> m_roots;
      int              m_nlbl;
    };

    template <typename I, typename O, typename N>
    std::vector<std::vector<std::pair<int, image_value_t<I>>>> watershed_rag(I input, O output, N nbh, int& nlabel)
    {
      using V = image_value_t<I>;
      using P = image_point_t<I>;

      nlabel = mln::labeling::impl::local_minima(input, nbh, output, std::less<V>()) + 1;

      std::vector<std::vector<std::pair<int, image_value_t<I>>>> rag(nlabel);

      constexpr auto impl_type = mln::morpho::details::pqueue_impl::linked_list;
      mln::morpho::details::pqueue_fifo<I, impl_type, /* reversed = */ true> pqueue(input);
      {
        output.extension().fill(-1);
        mln_foreach (auto pix, output.pixels())
        {
          if (pix.val() <= 0)
            continue;
          for (auto p : nbh(pix))
          {
            if (p.val() == 0)
            {
              pqueue.push(input(pix.point()), pix.point());
              break;
            }
          }
        }

        const auto add_edge = [&rag, &input, &output](const P& p, const P& q) {
          const auto [lbl_min, lbl_max] = std::minmax(output(p), output(q));
          V   max_w                     = std::max(input(p), input(q));
          int i                         = 0;
          while (i < static_cast<int>(rag[lbl_min].size()))
          {
            auto [out_v, e] = rag[lbl_min][i];
            if (out_v == lbl_max)
            {
              rag[lbl_min][i].second = std::min(e, max_w);
              break;
            }
            i++;
          }
          if (i == static_cast<int>(rag[lbl_min].size()))
            rag[lbl_min].emplace_back(lbl_max, max_w);
        };

        while (!pqueue.empty())
        {
          auto [lvl, p] = pqueue.top();
          pqueue.pop();
          for (auto q : nbh(p))
          {
            if (output.at(q) == 0) // If extension then -1
            {
              output(q) = output(p);
              pqueue.push(input(q), q);
            }
            else if (output.at(q) > 0 && output.at(q) != output.at(p))
              add_edge(p, q);
          }
        }
      }

      return rag;
    }

    template <typename V>
    std::vector<std::tuple<int, int, int>> mst_waterfall(const std::vector<std::vector<std::pair<int, V>>>& rag)
    {
      std::vector<std::tuple<int, int, V>> flatten_graph;
      for (int in = 0; in < static_cast<int>(rag.size()); in++)
      {
        for (auto [out, w] : rag[in])
          flatten_graph.emplace_back(in, out, w);
      }
      std::ranges::sort(flatten_graph, [](const auto& a, const auto& b) { return std::get<2>(a) < std::get<2>(b); });

      // Kruskal
      mst_waterfall_visitor viz;
      mln::morpho::canvas::kruskal(flatten_graph.data(), flatten_graph.size(), static_cast<int>(rag.size()), viz);

      return std::move(viz.mst);
    }

    template <typename N>
    component_tree<int> waterfall_from_mst(std::vector<std::tuple<int, int, int>>& mst, int n_vertices, N& nm)
    {
      std::ranges::sort(mst, [](const auto& a, const auto& b) { return std::get<2>(a) < std::get<2>(b); });

      waterfall_bpt_visitor viz;
      mln::morpho::canvas::kruskal(mst.data(), mst.size(), n_vertices, viz);

      internal::alphatree_reorder_nodes(viz.parent.data(), viz.value.data(), viz.parent.size());
      component_tree<int> res;
      std::tie(res.parent, res.values) = internal::canonize_component_tree(viz.parent, viz.value, nm);

      return res;
    }
  } // namespace details

  namespace impl
  {
    template <typename I, typename N>
    std::pair<component_tree<int>, image_ch_value_t<I, int>> waterfall(I input, N nbh)
    {
      const auto process = [&input, &nbh](auto output) {
        int        nlbl = 0;
        const auto rag  = details::watershed_rag(input, output, nbh, nlbl);
        auto       mst  = details::mst_waterfall(rag);
        return details::waterfall_from_mst(mst, nlbl, output);
      };

      image_build_error_code err    = IMAGE_BUILD_OK;
      auto                   output = imchvalue<int>(input) //
                        .adjust(nbh)
                        .set_init_value(-1)
                        .get_status(&err)
                        .build();

      component_tree<int> t;
      if (err == IMAGE_BUILD_OK)
      {
        t = process(output);
      }
      else
      {
        mln::trace::warn("[Performance] The extension is not wide enough");
        auto out = view::value_extended(output, -1);
        t        = process(out);
      }
      return {std::move(t), output};
    }
  } // namespace impl

  template <typename I, typename N>
  auto waterfall(I ima, N nbh)
  {
    static_assert(mln::is_a_v<I, mln::details::Image>);
    static_assert(mln::is_a_v<N, mln::details::Neighborhood>);

    mln_entering("mln::morpho::waterfall");

    return impl::waterfall(ima, nbh);
  }
} // namespace mln::morpho