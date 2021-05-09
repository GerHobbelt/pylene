#pragma once

#include <mln/morpho/component_tree.hpp>

namespace mln::morpho
{
  template <typename V, typename Nodemap, typename Th, typename L>
  auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm, Th th, ::ranges::span<L> vals);

  template <typename V, typename Nodemap, typename Th, typename L>
  auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm, Th th, const std::vector<L>& vals);

  /******************
   * Implementation *
   ******************/

  template <typename V, typename Nodemap, typename Th, typename L>
  requires(std::is_convertible_v<Th, V>) auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm,
                                                                               Th th, ::ranges::span<L> vals)
  {
    static_assert(mln::is_a_v<Nodemap, mln::details::Image>);
    static_assert(std::is_same_v<image_value_t<Nodemap>, int>);
    using ValueType = std::remove_cv_t<L>;

    assert(static_cast<std::size_t>(vals.size()) == t.parent.size());

    image_ch_value_t<Nodemap, ValueType> lbl = imchvalue<ValueType>(nm);

    std::size_t n = t.parent.size();

    // Root of cut connected component
    std::vector<std::size_t> root_cut_cc(n);
    for (std::size_t node = 0; node < n; ++node)
    {
      std::size_t parent_node = t.parent[node];
      root_cut_cc[node]       = t.values[parent_node] > th ? node : root_cut_cc[parent_node];
    }

    // Reconstruction of image
    mln_foreach (auto px, nm.pixels())
      lbl(px.point()) = vals[root_cut_cc[px.val()]];

    return lbl;
  }

  template <typename V, typename Nodemap, typename Th, typename L>
  requires(std::is_convertible_v<Th, V>) auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm,
                                                                               Th th, const std::vector<L>& vals)
  {
    return horizontal_cut_labelization_from(t, nm, th, ::ranges::make_span(vals.data(), vals.size()));
  }
} // namespace mln::morpho