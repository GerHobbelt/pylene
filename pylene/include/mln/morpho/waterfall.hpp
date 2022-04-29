#pragma once

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
      void visit(int, int) noexcept;
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
    image_build_error_code err = IMAGE_BUILD_OK;
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
    //std::tie(t.parent, t.values) = internal::canonize_component_tree(viz.parent, viz.values, output);
    t.parent = std::move(viz.parent);
    t.values = std::move(viz.values);
    internal::alphatree_reorder_nodes(t.parent.data(), t.values.data(), t.parent.size());
    mln::for_each(output, [node_count=t.parent.size()](int& id) { id = static_cast<int>(node_count) - id - 1; });

    return std::make_pair(std::move(t), std::move(output));
  }

  image2d<int> waterfall_saliency(const component_tree<int>&, const image2d<int>&);
} // namespace mln::morpho