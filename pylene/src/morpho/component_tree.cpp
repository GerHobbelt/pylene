#include <mln/morpho/component_tree.hpp>


namespace mln::morpho
{
  namespace internal
  {
    template <typename P, typename W>
    struct edge_t
    {
      P p;
      P q;
      W w;
    };
  } // namespace internal

  void component_tree<void>::filter_direct(const std::vector<bool>& pred)
  {
    this->filter_direct_T([&pred](int x) { return pred[x]; });
  }


  std::vector<int> component_tree<void>::compute_depth() const
  {
    std::size_t n = parent.size();

    std::vector<int> depth(n);
    depth[0] = 0;

    for (std::size_t i = 1; i < n; ++i)
      depth[i] = depth[parent[i]] + 1;

    return depth;
  }

  static std::vector<internal::edge_t<int, double>> saliency_map(mln::image2d<uint8_t> node_map)
  {
    std::vector<internal::edge_t<int, double>> res;

    auto width = node_map.width();

    auto dom = node_map.domain();
    mln_foreach (auto p, dom)
    {
      for (auto q : mln::c4.after(p))
      {
        if (dom.has(q))
        {
          auto offset_p = p[0] + width * p[1];
          auto offset_q = q[0] + width * q[1];

          internal::edge_t<int, double> edge = {offset_p, offset_q, 0};

          edge.w = std::abs(node_map(p) - node_map(q));

          res.emplace_back(edge);
        }
      }
    }

    return res;
  }

  mln::image2d<double> component_tree<void>::saliency(mln::image2d<uint8_t> node_map)
  {
    int height = node_map.height();
    int width  = node_map.width();

    int res_height = 2 * height + 1;
    int res_width  = 2 * width + 1;

    image2d<double> res(res_width, res_height);
    fill(res, 0);

    const std::vector<internal::edge_t<int, double>>& s_map = saliency_map(node_map);

    for (const auto [u, v, w] : s_map)
    {
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
