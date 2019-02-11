#pragma once


#include <mln/accu/accumulator.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/core/trace.hpp>

#include <vector>

#include <range/v3/view/span.hpp>

namespace mln::morpho
{

  enum ct_filtering
  {
    CT_FILTER_DIRECT,
    CT_FILTER_MIN,
    CT_FILTER_MAX,
    CT_FILTER_SUBTRACTIVE
  };

  template <typename P, typename W>
  struct edge_t
  {
    P p;
    P q;
    W w;
  };

  template <class T = void>
  class component_tree;


  template <>
  class component_tree<void>
  {
  public:
    using node_id_type = int;
    using node_map_t   = int;


    /// \brief Filter the tree given a predicate that removes some nodes according to the selected strategy.
    ///
    /// The parent array is updated, as well as the node_map if provided. It does not invalidate the node id, i.e. node
    /// that remain keeps the same position.
    ///
    /// The strategy is one of DIRECT, MIN, MAX, SUBTRACTIVE
    ///
    /// * Min
    /// A node is remove if it does not pass the predicate and
    /// all its descendant are removed as well.
    /// Formally, a component Γ is kept if 𝓒(Γ) with
    /// 𝓒(Γ) = ⋁ { 𝓟(Γ'), Γ ⊆ Γ' }.
    ///
    /// * Max
    /// A node is remove if every node in its childhood does not pass
    /// the predicate. All its descendant are removed as well.  Formally, a
    /// component Γ is kept if 𝓒(Γ) with 𝓒(Γ) = ⋀ { 𝓟(Γ'), Γ' ⊆ Γ }.
    ///
    /// * Direct
    /// A node is remove if it does not pass the predicate, the others
    /// remain.
    ///
    ///
    /// \param strategy The filtering strategy
    /// \param pred A boolean predicate for each node id
    template <class F>
    void filter(ct_filtering strategy, F pred);

    template <class I, class F>
    void filter(ct_filtering strategy, I node_map, F pred);


    /// \brief Compute the depth attribute over a tree
    std::vector<int> compute_depth() const;


    /// \brief Compute attribute on values
    ///
    /// Compute attribute with values given from an image
    ///
    /// \param node_map Image point -> node_id mapping
    /// \param values Image point -> value mapping
    /// \param acc Accumulator to apply on values
    /// \param propagate Boolean to propagate the values to the parent
    template <class I, class J, class Accu>
    std::vector<typename accu::result_of<Accu, image_value_t<J>>::type> //
    compute_attribute_on_values(I node_map, J values, Accu acc, bool propagate = true) const;

    /// \brief Compute attribute on values
    ///
    /// Compute attribute on points
    ///
    /// \param node_map Image point -> node_id mapping
    /// \param values Image point -> value mapping
    /// \param acc Accumulator to apply on points
    /// \param propagate Boolean to propagate the values to the parent
    template <class I, class Accu>
    std::vector<typename accu::result_of<Accu, image_point_t<I>>::type> //
    compute_attribute_on_points(I node_map, Accu acc, bool propagate = true) const;


    /// \brief Compute attribute on pixels
    ///
    /// Compute attribute with pixels (point + value) given from an image
    ///
    /// \param node_map Image point -> node_id mapping
    /// \param values Image point -> value mapping
    /// \param acc Accumulator to apply on values
    /// \param propagate Boolean to propagate the values to the parent
    template <class I, class J, class Accu>
    std::vector<typename accu::result_of<Accu, image_pixel_t<J>>::type> //
    compute_attribute_on_pixels(I node_map, J values, Accu acc, bool propagate = true) const;

    /// \brief Compute the horizontal cut of a hierarchie at level `threshold` and return a nodemap
    /// valued with the node indices of the lowest nodes satisfying levels[n] > threshold
    ///
    /// \param threshold The threshold of the cut
    /// \param nodemap Image point -> node_id mapping
    /// \param levels Altitude of each node in the tree
    template <class T, class I, class V>
    I horizontal_cut_from_levels(const T threshold, I nodemap, ::ranges::span<V> levels) const;


    /// \brief Reconstruct an image from an attribute map
    ///
    /// \param node_map Image point -> node_id mapping
    /// \param values node_id -> value mapping
    template <class I, class V>
    image_ch_value_t<I, V> reconstruct_from(I node_map, ::ranges::span<V> values) const;

    using node_t = int;

    /// \brief Produce a visualization of the given Component Tree using the Khalimsky grid of the saliency map
    ///        The component_tree must be built on a 2D image with a 4-connectivity.
    ///
    /// \param node_map Image point -> node_id mapping
    mln::image2d<double> saliency(mln::image2d<int> node_map, ::ranges::span<double> values) const;

    std::vector<node_t> parent;

  protected:
    template <class F, class V, class N = std::nullptr_t>
    void filter_impl(ct_filtering strategy, F pred, V* values, N nodemap = nullptr);

  private:
    template <class I, class F>
    void update_node_map(I node_map, F pred) const;


    void filter_direct(const std::vector<bool>& pred);

    template <class F>
    void filter_direct_T(F pred);
  };


  template <class T>
  class component_tree : public component_tree<void>
  {
  public:
    template <class I>
    I horizontal_cut(const T threshold, I nodemap) const
    {
      return this->horizontal_cut_from_levels(threshold, nodemap, ::ranges::make_span(values.data(), values.size()));
    }

    /// \brief Filter the tree given a predicate that removes some nodes according to the selected strategy.
    ///
    /// The parent array is updated, as well as the node_map if provided. It does not invalidate the node id, i.e. node
    /// that remain keeps the same position.
    ///
    /// The strategy is one of DIRECT, MIN, MAX, SUBTRACTIVE
    ///
    /// * Min
    /// A node is remove if it does not pass the predicate and
    /// all its descendant are removed as well.
    /// Formally, a component Γ is kept if 𝓒(Γ) with
    /// 𝓒(Γ) = ⋁ { 𝓟(Γ'), Γ ⊆ Γ' }.
    ///
    /// * Max
    /// A node is remove if every node in its childhood does not pass
    /// the predicate. All its descendant are removed as well.  Formally, a
    /// component Γ is kept if 𝓒(Γ) with 𝓒(Γ) = ⋀ { 𝓟(Γ'), Γ' ⊆ Γ }.
    ///
    /// * Direct
    /// A node is remove if it does not pass the predicate, the others
    /// remain.
    ///
    ///
    /// \param strategy The filtering strategy
    /// \param node_map (optional) If given, the nodemap is updated to point to valid nodes
    /// \param pred A boolean predicate for each node id
    template <class F>
    void filter(ct_filtering strategy, F pred);

    template <class I, class F>
    void filter(ct_filtering strategy, I node_map, F pred);

    template <class I>
    image_ch_value_t<std::remove_reference_t<I>, T> reconstruct(I&& node_map)
    {
      return this->reconstruct_from(std::forward<I>(node_map), ::ranges::make_span(values.data(), values.size()));
    }


    std::vector<T> values;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class F>
  void component_tree<void>::filter_direct_T(F pred)
  {
    std::size_t n = parent.size();

    for (std::size_t i = 1; i < n; ++i)
      if (parent[i] > 0 && !pred(parent[i])) // If the parent[i] > 0 <=> not the root
        parent[i] = parent[parent[i]];
  }


  template <class I, class F>
  void component_tree<void>::update_node_map(I node_map, F pred) const
  {
    mln_foreach (auto& id, node_map.values())
    {
      if (id > 0 && !pred(id))
        id = this->parent[id];
    }
  }



  template <class F, class V, class N>
  void component_tree<void>::filter_impl(ct_filtering strategy, F pred, V* values, N nodemap)
  {
    int n = static_cast<int>(parent.size());

    constexpr bool _support_subtractive = requires (V a, V b) {
      { a + b } -> std::convertible_to<V>;
      { a - b } -> std::convertible_to<V>;
      { a += b};
      { a -= b};
    };

    if (!_support_subtractive && strategy == CT_FILTER_SUBTRACTIVE)
      throw std::runtime_error("Subtractive strategy not supported for non-arithmetic types.");

    if (strategy == CT_FILTER_SUBTRACTIVE && values == nullptr)
      throw std::runtime_error("Values ptr cannot be null with the SUBTRACTIVE strategy.");


    // Node pass status
    std::vector<bool> pass;
    std::vector<V> diff;


 
    if (strategy == CT_FILTER_MAX)
    {
      // A node is removed if all its descandants are removed
      pass.resize(n, false);
      // Propagate upward
      for (int i = n - 1; i > 0; --i)
      {
        pass[i]         = pass[i]         || pred(i);
        pass[parent[i]] = pass[parent[i]] || pass[i];
      }
    }
    else
    {
      pass.resize(n);
      pass[0] = true;
    }

    if (strategy == CT_FILTER_SUBTRACTIVE)
    {
      pass.resize(n);
      diff.resize(n);
      if constexpr (_support_subtractive)
        for (int i = 1; i < n; ++i)
          diff[i] = values[i] - values[parent[i]];
    }

    parent[0] = 0;
    for (int i = 1; i < n; ++i)
    {
      int q = parent[i];
      bool C;
      switch (strategy)
      {
        case CT_FILTER_DIRECT:
        case CT_FILTER_SUBTRACTIVE:
          C = pass[i] = pred(i); break;
        case CT_FILTER_MIN:
          C = pass[i] = (pred(i) && pass[q]); break;
        case CT_FILTER_MAX:
          C = pass[i]; break;
      }

      parent[i] = pass[q] ? q : parent[q];
      if (strategy == CT_FILTER_SUBTRACTIVE)
      {   
        if constexpr (_support_subtractive)
          values[i] = values[q] + (C ? diff[i] : 0); 
      }
      else if (values)
        values[i] = C ? values[i] : values[q];
    }
    parent[0] = -1;

    // 3. Update the nodemap (if required)
    if constexpr (!std::is_null_pointer_v<N>)
    {
      switch (strategy)
      {
        case CT_FILTER_DIRECT: // Non-pruning
        case CT_FILTER_SUBTRACTIVE:
          this->update_node_map(nodemap, pred);
          break;
        case CT_FILTER_MIN: // Pruning
        case CT_FILTER_MAX:
          this->update_node_map(nodemap, [&pass](int x) { return pass[x]; });
          break;
      }
    }
  }

  template <class V>
  template <class F>
  void component_tree<V>::filter(ct_filtering strategy, F pred)
  {
    mln_entering("mln::morpho::component_tree::filter");
    this->filter_impl(strategy, pred, this->values.data());
  }

  template <class V>
  template <class I, class F>
  void component_tree<V>::filter(ct_filtering strategy, I node_map, F pred)
  {
    mln_entering("mln::morpho::component_tree::filter");
    this->filter_impl(strategy, pred, this->values.data(), node_map);
  }


  template <class F>
  void component_tree<void>::filter(ct_filtering strategy, F pred)
  {
    mln_entering("mln::morpho::component_tree::filter");
    this->filter_impl(strategy, pred, (int*)nullptr, nullptr);
  }

  template <class I, class F>
  void component_tree<void>::filter(ct_filtering strategy, I node_map, F pred)
  {
    mln_entering("mln::morpho::component_tree::filter");
    this->filter_impl(strategy, pred, (int*)nullptr, node_map);
  }


  template <class I, class Accu>
  std::vector<typename accu::result_of<Accu, image_point_t<I>>::type>
  component_tree<void>::compute_attribute_on_points(I node_map, Accu acc, bool propagate) const
  {
    mln_entering("mln::morpho::component_tree::compute_attribute_on_points");

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_point_t<I>>::type;

    auto a = accu::make_accumulator(acc, image_point_t<I>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    mln_foreach (auto px, node_map.pixels())
      attr[px.val()].take(px.point());

    const std::size_t n = parent.size();
    if (propagate)
    {
      // Propgate to parent
      for (std::size_t i = n - 1; i > 0; --i)
        attr[parent[i]].take(attr[i]);
    }

    // Extract values
    std::vector<R> out(n);
    for (std::size_t i = 0; i < n; ++i)
      out[i] = attr[i].to_result();

    return out;
  }

  template <class I, class J, class Accu>
  std::vector<typename accu::result_of<Accu, image_value_t<J>>::type> //
  component_tree<void>::compute_attribute_on_values(I node_map, J input, Accu acc, bool propagate) const
  {
    mln_entering("mln::morpho::component_tree::compute_attribute_on_values");

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_value_t<J>>::type;

    auto a = accu::make_accumulator(acc, image_value_t<J>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    auto zz = mln::view::zip(node_map, input);
    mln_foreach ((auto [node_id, val]), zz.values())
      attr[node_id].take(val);

    const std::size_t n = parent.size();
    if (propagate)
    {
      // Propgate to parent
      for (std::size_t i = n - 1; i > 0; --i)
        attr[parent[i]].take(attr[i]);
    }

    // Extract values
    std::vector<R> out(n);
    for (std::size_t i = 0; i < n; ++i)
      out[i] = attr[i].to_result();

    return out;
  }

  template <class I, class J, class Accu>
  std::vector<typename accu::result_of<Accu, image_pixel_t<J>>::type> //
  component_tree<void>::compute_attribute_on_pixels(I node_map, J values, Accu acc, bool propagate) const
  {
    mln_entering("mln::morpho::component_tree::compute_attribute_on_pixels");

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_pixel_t<J>>::type;

    auto a = accu::make_accumulator(acc, image_pixel_t<J>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    mln_foreach (auto px, values.pixels())
      attr[node_map(px.point())].take(px);

    const std::size_t n = parent.size();
    if (propagate)
    {
      // Propgate to parent
      for (std::size_t i = n - 1; i > 0; --i)
        attr[parent[i]].take(attr[i]);
    }

    // Extract values
    std::vector<R> out(n);
    for (std::size_t i = 0; i < n; ++i)
      out[i] = attr[i].to_result();

    return out;
  }

  template <class T, class I, class V>
  I component_tree<void>::horizontal_cut_from_levels(const T threshold, I nodemap, ::ranges::span<V> levels) const
  {
    mln_entering("mln::morpho::component_tree::horizontal_cut_from_levels");

    auto root_cut_cc = std::vector<int>(parent.size());
    for (std::size_t node = 0; node < parent.size(); ++node)
    {
      int parent_node   = parent[node];
      root_cut_cc[node] = levels[parent_node] > threshold ? static_cast<int>(node) : root_cut_cc[parent_node];
    }

    auto out = mln::clone(nodemap);
    mln_foreach (auto px, out.pixels())
      out(px.point()) = root_cut_cc[px.val()];
    return out;
  }

  template <class I, class V>
  image_ch_value_t<I, V> component_tree<void>::reconstruct_from(I node_map, ::ranges::span<V> values) const
  {
    mln_entering("mln::morpho::component_tree::reconstruction");

    image_ch_value_t<I, V> out = imchvalue<V>(node_map);

    auto zz = mln::view::zip(node_map, out);
    mln_foreach ((auto&& [node_id, val]), zz.values())
      val = values[node_id];

    return out;
  }
} // namespace mln::morpho
