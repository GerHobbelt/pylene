#pragma once

#include <mln/core/neighborhood/c4c8.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/watershed.hpp>

#include <tuple>
#include <vector>

namespace mln::morpho
{
  namespace details
  {
    class waterfall_visitor
    {
    public:
      void init(int) noexcept;
      void visit(int, int, mln::dontcare_t, mln::dontcare_t) noexcept;
      void finalize() noexcept;

      std::vector<int> values;
      std::vector<int> parent;

    private:
      std::vector<int>                       m_zpar;
      std::vector<int>                       m_diameter;
      std::vector<std::tuple<int, int, int>> m_mst;
      int                                    m_nlbl;
      int                                    m_save;
    };
  } // namespace details

  template <typename I, typename N>
  auto waterfall(I ima, N nbh)
  {
    static_assert(mln::is_a_v<I, mln::details::Image>);
    static_assert(mln::is_a_v<N, mln::details::Neighborhood>);

    mln_entering("mln::morpho::waterfall");
    image_build_error_code     err = IMAGE_BUILD_OK;
    details::waterfall_visitor viz{};
    auto                       output = imchvalue<int>(ima) //
                      .adjust(nbh)
                      .set_init_value(-1)
                      .get_status(&err)
                      .build();

    if (err == IMAGE_BUILD_OK)
    {
      impl::watershed(ima, nbh, output, viz);
    }
    else
    {
      mln::trace::warn("[Performance] The extension is not wide enough");
      auto out = view::value_extended(output, -1);
      impl::watershed(ima, nbh, out, viz);
    }

    component_tree<int> t;
    internal::alphatree_reorder_nodes(viz.parent.data(), viz.values.data(), viz.parent.size());
    std::tie(t.parent, t.values) = internal::canonize_component_tree(viz.parent, viz.values, output);
    t.parent.push_back(t.parent.size());
    t.values.push_back(0);
    mln::for_each(output, [waterline = t.parent.size() - 1](int& v) { v = v == 0 ? waterline : v; });

    return std::make_pair(std::move(t), std::move(output));
  }

  image2d<int> waterfall_saliency(const component_tree<int>&, const image2d<int>&, const mln::c4c8_t&);
} // namespace mln::morpho