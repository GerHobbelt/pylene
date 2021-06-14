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
    // CT_FILTER_SUBTRACTIVE (not yet implemented)
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

    using Edge = std::tuple<int, int, double>;

    /// Generate a saliency map of the given Component Tree
    ///
    /// @return A list of edges that associates each edges to a saliency weight
    template <class I, class J>
    std::vector<Edge> saliency_map(I node_map, J values);


    /// Produce a visualization of the given Component Tree using the Khalimsky grid of the saliency map
    template <class I, class J>
    mln::image2d<double> saliency_khalimsky_grid(I node_map, J values);


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
    I horizontal_cut(const T threshold, I nodemap) const
    {
      return this->horizontal_cut_from_levels(threshold, nodemap, ::ranges::make_span(values.data(), values.size()));
    }

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
        pass[i]         = pass[i] || pred(i);
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
        pass[i] = pred(static_cast<int>(i)) && pass[parent[i]];
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
        pass[i]         = pass[i] || pred(static_cast<int>(i));
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

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_point_t<I>>::type;

    auto a = accu::make_accumulator(acc, image_point_t<I>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    mln_foreach (auto px, node_map.pixels())
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

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_value_t<J>>::type;

    auto a = accu::make_accumulator(acc, image_value_t<J>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    auto zz = mln::view::zip(node_map, input);
    mln_foreach ((auto [node_id, val]), zz.values())
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

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<Accu, mln::AccumulatorLike>());
    using R = typename accu::result_of<Accu, image_pixel_t<J>>::type;

    auto a = accu::make_accumulator(acc, image_pixel_t<J>());

    std::vector<decltype(a)> attr(parent.size(), a);

    // Accumulate for each point
    mln_foreach (auto px, values.pixels())
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

  using Edge = std::tuple<int, int, double>;

  template <class I, class J>
  std::vector<Edge> component_tree<void>::saliency_map(I node_map, J values)
  {
    std::vector<Edge> res;

    auto width = node_map.width();

    mln_foreach (auto p, node_map.domain())
    {
      for (auto q : mln::c4(p))
      {
        if ((q[0] == p[0] + 1 || q[1] == p[1] + 1) && q[0] < node_map.width() && q[1] < node_map.height())
        {
          auto id_p = 0;
          auto id_q = 0;
          for (size_t i = 1; i < this->parent.size(); i++)
          {
            if (id_p == 0 && values[i] == node_map(p))
              id_p = i;

            if (id_q == 0 && values[i] == node_map(q))
              id_q = i;

            if (id_p != 0 && id_q != 0)
              break;
          }

          auto edge = std::make_tuple(p[0] + width * p[1], q[0] + width * q[1], 0);

          std::get<2>(edge) = std::abs(node_map(p) - node_map(q));

          res.emplace_back(edge);
        }
      }
    }

    return res;
  }

  template <class I, class J>
  mln::image2d<double> component_tree<void>::saliency_khalimsky_grid(I node_map, J values)
  {
    int height = node_map.height();
    int width  = node_map.width();

    int res_height = 2 * height + 1;
    int res_width  = 2 * width + 1;

    image2d<double> res(res_width, res_height);
    fill(res, 0);

    const std::vector<Edge>& s_map = this->saliency_map(node_map, values);

    for (const auto& edge : s_map)
    {
      int    u = std::get<0>(edge);
      int    v = std::get<1>(edge);
      double w = std::get<2>(edge);

      int u_pos[2] = {u % width, u / width};
      int v_pos[2] = {v % width, v / width};

      int res_offset[2]   = {u_pos[0] - v_pos[0], u_pos[1] - v_pos[1]};
      int res_edge_pos[2] = {2 * v_pos[0] + 1 + res_offset[0], 2 * v_pos[1] + 1 + res_offset[1]};

      res({res_edge_pos[0], res_edge_pos[1]}) = w;
    }

    for (int y = 0; y < res_height; y += 2)
    {
      for (int x = 0; x < res_width; x += 2)
      {
        double max = std::numeric_limits<double>::min();

        if (y + 1 < height)
          max = std::max(max, res({x, y + 1}));
        if (x + 1 < width)
          max = std::max(max, res({x + 1, y}));
        if (y - 1 >= 0)
          max = std::max(max, res({x, y - 1}));
        if (x - 1 >= 0)
          max = std::max(max, res({x - 1, y}));

        res({x, y}) = max;
      }
    }

    for (int y = 0; y < res_height; y++)
    {
      for (int x = 0; x < res_width; x++)
      {
        res({x, y}) = 255 - res({x, y});
      }
    }

    return res;
  }


} // namespace mln::morpho
