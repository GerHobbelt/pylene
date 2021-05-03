#pragma once

#include <mln/morpho/component_tree.hpp>

#include <vector>

namespace mln::morpho
{
  template <typename V, typename Nodemap, typename L>
  auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm, V th, const std::vector<L>& vals);

  /******************
   * Implementation *
   ******************/

  template <typename V, typename Nodemap, typename L>
  auto horizontal_cut_labelization_from(const component_tree<V>& t, Nodemap nm, V th, const std::vector<L>& vals)
  {
    static_assert(mln::is_a_v<Nodemap, mln::details::Image>);
    static_assert(std::is_same_v<image_value_t<Nodemap>, int>);

    assert(vals.size() == t.parent.size());

    image_ch_value_t<Nodemap, L> lbl = imchvalue<L>(nm);
    image_concrete_t<Nodemap> cut_nm = imconcretize(nm);

    mln_foreach(auto p, nm.domain())
    {
        cut_nm(p) = nm(p);
        while (t.parent[cut_nm(p)] != cut_nm(p) && t.values[t.parent[cut_nm(p)]] <= th)
            cut_nm(p) = t.parent[cut_nm(p)];
    }

    mln_foreach(auto px, cut_nm.pixels())
        lbl(px.point()) = vals[px.val()];

    return lbl;
  }
} // namespace mln::morpho