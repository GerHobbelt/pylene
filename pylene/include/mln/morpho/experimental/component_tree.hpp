#pragma once


#include <mln/core/image/view/zip.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <mln/accu/accumulator.hpp>

#include <vector>

#include <range/v3/view/span.hpp>

namespace mln::morpho::experimental
{

  // namespace details
  // {
  //   void filter_treeT(const int* parent, std::size_t n, e_filtering strategy, F predicate);
  //   void filter_tree(const int* parent, std::size_t n, e_filtering strategy, std::function<bool(int)> predicate);
  // }

  enum ct_filtering
  {
    CT_FILTER_DIRECT,
    CT_FILTER_MIN,
    CT_FILTER_MAX,
    CT_FILTER_SUBTRACTIVE
  };


  template <class T = void>
  class component_tree;



  template <>
  class component_tree<void>
  {
  public:
    using node_id_type = int;
    using node_map_t = int;




    template <class F>
    void filter(ct_filtering strategy, F pred);

    template <class I, class F>
    void filter(ct_filtering strategy, I node_map, F pred);



    /// \brief Compute the depth attribute over a tree
    std::vector<int> compute_depth_map() const;


    /// \brief Compute attribute on values
    template <class I, class J, class Accu>
    std::vector<typename accu::result_of<Accu, image_value_t<J>>::type> //
    compute_attribute_on_values(I node_map, J input, Accu acc);

    /// \brief Compute attribute on points
    template <class I, class Accu>
    std::vector<typename accu::result_of<Accu, image_point_t<I>>::type> //
    compute_attribute_on_points(I node_map, Accu acc);

    /// \brief Compute attribute on values
    template <class I, class J, class Accu>
    void compute_attribute_on_pixels(I node_map, J input, Accu acc);



    /// \brief Reconstruct an image from an attribute map
    template <class I, class V>
    image_ch_value_t<I, V> reconstruct_from(I node_map, ::ranges::span<V> values) const;

    template <class I, class F>
    void update_node_map(I node_map, F pred) const;

    /// \brief Filter an image

    // template <class I, class J, class F>
    // void filter(e_filtering strategy, I node_map, J input, ::ranges::span<V> values, F pred) const;



    using node_t = int;
    std::vector<node_t> parent;

  private:
    void filter_direct(const std::vector<bool>& pred);

    template <class F>
    void filter_direct_T(F pred);
  };


  template <class T>
  class component_tree : public component_tree<void>
  {
  public:

    template <class I>
    image_ch_value_t<I, T> reconstruct(I node_map)
    {
      return this->reconstruct_from(std::move(node_map), ::ranges::make_span(values.data(), values.size()));
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
      if (!pred(parent[i]))
        parent[i] = parent[parent[i]];
  }


  template <class I, class F>
  void component_tree<void>::update_node_map(I node_map, F pred) const
  {
    mln_foreach_new(auto& id, node_map.new_values())
    {
      if (!pred(id))
        id = this->parent[id];
    }
  }



  template <class F>
  void component_tree<void>::filter(ct_filtering strategy, F pred)
  {
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
    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<Accu, mln::Accumulator>());
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
    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<Accu, mln::Accumulator>());
    using R = typename accu::result_of<Accu, image_value_t<J>>::type;

    auto a = accu::make_accumulator(acc, image_value_t<J>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    auto zz = mln::view::zip(node_map, input);
    mln_foreach_new((auto [node_id, val]), zz.values())
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




  template <class I, class V>
  image_ch_value_t<I, V> component_tree<void>::reconstruct_from(I node_map, ::ranges::span<V> values) const
  {
    image_ch_value_t<I, V> out = imchvalue<V>(node_map);

    auto zz = mln::view::zip(node_map, out);
    mln_foreach_new ((auto&& [node_id, val]), zz.new_values())
      val = values[node_id];

    return out;
  }


} // namespace mln::morpho::experimental
