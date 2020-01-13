#pragma once


#include <mln/core/image/view/zip.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <mln/accu/accumulator.hpp>

#include <vector>

#include <range/v3/view/span.hpp>

namespace mln::morpho::experimental
{

  enum ct_filtering
  {
    CT_FILTER_DIRECT,
    CT_FILTER_MIN,
    CT_FILTER_MAX,
    // CT_FILTER_SUBTRACTIVE (not yet implemented)
  };


  template <class T = void>
  class component_tree;



  template <>
  class component_tree<void>
  {
  public:
    using node_id_type = int;
    using node_map_t = int;


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
    template <class I, class J, class Accu>
    std::vector<typename accu::result_of<Accu, image_value_t<J>>::type> //
    compute_attribute_on_values(I node_map, J values, Accu acc);

    /// \brief Compute attribute on values
    ///
    /// Compute attribute on points
    ///
    /// \param node_map Image point -> node_id mapping
    /// \param values Image point -> value mapping
    /// \param acc Accumulator to apply on points
    template <class I, class Accu>
    std::vector<typename accu::result_of<Accu, image_point_t<I>>::type> //
    compute_attribute_on_points(I node_map, Accu acc);


    /// \brief Compute attribute on pixels
    ///
    /// Compute attribute with pixels (point + value) given from an image
    ///
    /// \param node_map Image point -> node_id mapping
    /// \param values Image point -> value mapping
    /// \param acc Accumulator to apply on values
    template <class I, class J, class Accu>
    std::vector<typename accu::result_of<Accu, image_pixel_t<J>>::type> //
    compute_attribute_on_pixels(I node_map, J values, Accu acc);




    /// \brief Reconstruct an image from an attribute map
    ///
    /// \param node_map Image point -> node_id mapping
    /// \param values node_id -> value mapping
    template <class I, class V>
    image_ch_value_t<I, V> reconstruct_from(I node_map, ::ranges::span<V> values) const;



    using node_t = int;
    std::vector<node_t> parent;

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
    mln_foreach_new(auto& id, node_map.new_values())
    {
      if (id > 0 && !pred(id))
        id = this->parent[id];
    }
  }



  template <class F>
  void component_tree<void>::filter(ct_filtering strategy, F pred)
  {
    mln_entering("mln::morpho::component_tree::filter");

    std::size_t n = parent.size();

    // Node pass status
    std::vector<bool> pass;

    if (strategy == CT_FILTER_DIRECT)
    {
      this->filter_direct_T(pred);
    }
    else if (strategy == CT_FILTER_MIN)
    {
      // A node is removed if one ancestror is remove
      pass.resize(n);
      pass[0] = true;


      // Propagate downward
      for (std::size_t i = 1; i < n; ++i)
      {
        pass[i] = pred(i) && pass[parent[i]];
        if (!pass[parent[i]])
          parent[i] = parent[parent[i]];
      }
    }
    else if (strategy == CT_FILTER_MAX)
    {
      // A node is removed if all its descandants are removed
      pass.assign(n, false);

      // Propagate upward
      for (std::size_t i = n - 1; i > 0; --i)
      {
        pass[i] = pass[i] || pred(i);
        pass[parent[i]] = pass[parent[i]] || pass[i];
      }

      this->filter_direct(pass);
    }
  }

  template <class I, class F>
  void component_tree<void>::filter(ct_filtering strategy, I node_map, F pred)
  {
    mln_entering("mln::morpho::component_tree::filter");

    std::size_t n = parent.size();

    // Node pass status
    std::vector<bool> pass;

    if (strategy == CT_FILTER_DIRECT)
    {
      this->filter_direct_T(pred);
      this->update_node_map(node_map, pred);
      return;
    }
    else if (strategy == CT_FILTER_MIN)
    {
      // A node is removed if one ancestror is removed
      pass.resize(n);
      pass[0] = true;


      // Propagate downward
      for (std::size_t i = 1; i < n; ++i)
      {
        pass[i] = pred(i) && pass[parent[i]];
        if (!pass[parent[i]])
          parent[i] = parent[parent[i]];
      }
    }
    else if (strategy == CT_FILTER_MAX)
    {
      // A node is removed if all its descandants are removed
      pass.assign(n, false);

      // Propagate upward
      for (std::size_t i = n - 1; i > 0; --i)
      {
        pass[i] = pass[i] || pred(i);
        pass[parent[i]] = pass[parent[i]] || pass[i];
      }
      this->filter_direct(pass);
    }

    this->update_node_map(node_map, [&pass](int x) { return pass[x]; });
  }





  template <class I, class Accu>
  std::vector<typename accu::result_of<Accu, image_point_t<I>>::type>
  component_tree<void>::compute_attribute_on_points(I node_map, Accu acc)
  {
    mln_entering("mln::morpho::component_tree::compute_attribute_on_points");

    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_point_t<I>>::type;

    auto a = accu::make_accumulator(acc, image_point_t<I>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    mln_foreach_new(auto px, node_map.new_pixels())
      attr[px.val()].take(px.point());


    // Propgate to parent
    std::size_t n = parent.size();
    for (std::size_t i = n - 1; i > 0; --i)
      attr[parent[i]].take(attr[i]);


    // Extract values
    std::vector<R> out(n);
    for (std::size_t i = 0; i < n; ++i)
      out[i] = attr[i].to_result();

    return out;
  }

  template <class I, class J, class Accu>
  std::vector<typename accu::result_of<Accu, image_value_t<J>>::type> //
  component_tree<void>::compute_attribute_on_values(I node_map, J input, Accu acc)
  {
    mln_entering("mln::morpho::component_tree::compute_attribute_on_values");

    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_value_t<J>>::type;

    auto a = accu::make_accumulator(acc, image_value_t<J>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    auto zz = mln::view::zip(node_map, input);
    mln_foreach_new((auto [node_id, val]), zz.new_values())
      attr[node_id].take(val);


    // Propgate to parent
    std::size_t n = parent.size();
    for (std::size_t i = n - 1; i > 0; --i)
      attr[parent[i]].take(attr[i]);


    // Extract values
    std::vector<R> out(n);
    for (std::size_t i = 0; i < n; ++i)
      out[i] = attr[i].to_result();

    return out;
  }

  template <class I, class J, class Accu>
  std::vector<typename accu::result_of<Accu, image_pixel_t<J>>::type> //
  component_tree<void>::compute_attribute_on_pixels(I node_map, J values, Accu acc)
  {
    mln_entering("mln::morpho::component_tree::compute_attribute_on_pixels");

    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_pixel_t<J>>::type;

    auto a = accu::make_accumulator(acc, image_pixel_t<J>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    mln_foreach_new (auto px, values.new_pixels())
      attr[node_map(px.point())].take(px);


    // Propgate to parent
    std::size_t n = parent.size();
    for (std::size_t i = n - 1; i > 0; --i)
      attr[parent[i]].take(attr[i]);


    // Extract values
    std::vector<R> out(n);
    for (std::size_t i = 0; i < n; ++i)
      out[i] = attr[i].to_result();

    return out;
  }



  template <class I, class V>
  image_ch_value_t<I, V> component_tree<void>::reconstruct_from(I node_map, ::ranges::span<V> values) const
  {
    mln_entering("mln::morpho::component_tree::reconstruction");

    image_ch_value_t<I, V> out = imchvalue<V>(node_map);

    auto zz = mln::view::zip(node_map, out);
    mln_foreach_new ((auto&& [node_id, val]), zz.new_values())
      val = values[node_id];

    return out;
  }


} // namespace mln::morpho::experimental
