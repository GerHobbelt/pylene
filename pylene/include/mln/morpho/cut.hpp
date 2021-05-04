#pragma once

#include <mln/morpho/component_tree.hpp>

namespace mln::morpho
{
  template <typename V, typename Nodemap, typename L>
  auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm, V th, ::ranges::span<L> vals);

  template <typename V, typename Nodemap, typename L>
  auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm, V th, const std::vector<L>& vals);

  /******************
   * Implementation *
   ******************/

  template <typename V, typename Nodemap, typename L>
  auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm, V th, ::ranges::span<L> vals)
  {
    static_assert(mln::is_a_v<Nodemap, mln::details::Image>);
    static_assert(std::is_same_v<image_value_t<Nodemap>, int>);
    using ValueType = std::remove_cv_t<L>;

    assert(static_cast<std::size_t>(vals.size()) == t.parent.size());

    image_ch_value_t<Nodemap, ValueType> lbl    = imchvalue<ValueType>(nm);
    image_concrete_t<Nodemap>            cut_nm = imconcretize(nm);

    mln_foreach (auto p, nm.domain())
    {
      cut_nm(p) = nm(p);
      while (t.parent[cut_nm(p)] != cut_nm(p) && t.values[t.parent[cut_nm(p)]] <= th)
        cut_nm(p) = t.parent[cut_nm(p)];
    }

    mln_foreach (auto px, cut_nm.pixels())
      lbl(px.point()) = vals[px.val()];

    return lbl;
  }

  template <typename V, typename Nodemap, typename L>
  auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm, V th, const std::vector<L>& vals)
  {
    return horizontal_cut_labelization_from(t, nm, th, ::ranges::make_span(vals.data(), vals.size()));
  }
} // namespace mln::morpho