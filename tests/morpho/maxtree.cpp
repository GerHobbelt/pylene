#include <mln/morpho/maxtree.hpp>


#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/accu/accumulators/count.hpp>


#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>


void check_parent_array_sorted(const int* parent, std::size_t n)
{
  for (int i = 0; i < static_cast<int>(n); ++i)
    ASSERT_LE(parent[i], i);
}

mln::image2d<mln::point2d> //
make_parent_image(const int* par, std::size_t n,
                  const mln::image2d<int>& node_map)
{
  using mln::point2d;

  std::vector<point2d>                repr(n);
  std::vector<bool>                   has_repr(n, false);
  mln::image2d<point2d> parent;

  mln::resize(parent, node_map);

  mln_foreach_new(auto px, node_map.pixels())
  {
    auto p = px.point();
    auto id = px.val();
    if (!has_repr[id])
    {
      has_repr[id] = true;
      repr[id] = p;
    }
    parent(p) = repr[id];
  }

  mln_foreach_new(auto px, node_map.pixels())
  {
    auto p  = px.point();
    auto id = px.val();

    if (parent(p) == p && par[id] != -1)
      parent(p) = repr[par[id]];
  }
  return parent;
}


template <>
struct fmt::formatter<mln::point2d>
{
  auto parse(format_parse_context& ctx)
  {
    int x = 0;
    auto c = ctx.begin();
    auto e = ctx.end();
    while (c != e && !((x == 0) && (*c == '}')))
    {
      if (*c == '{')
        x++;
      if (*c == '}')
        x--;
      ++c;
    }
    return c;
  }

  // parse is inherited from formatter<string_view>.
  template <typename FormatContext>
  auto format(mln::point2d p, FormatContext& ctx)
  {
    return format_to(ctx.out(), " ({:d},{:d})", p.x(), p.y());
  }
};


TEST(Morpho, Maxtree_uint8)
{
  const mln::image2d<uint8_t> input = {{10, 11, 11, 15, 16, 11, +2}, //
                                                     {+2, 10, 10, 10, 10, 10, 10}, //
                                                     {18, +2, 18, 19, 18, 14, +6}, //
                                                     {16, +2, 16, 10, 10, 10, 10}, //
                                                     {18, 16, 18, +2, +2, +2, +2}};


  mln::image2d<mln::point2d> ref_parent = {
    {{6, 2}, {0, 0}, {1, 0}, {1, 0}, {3, 0}, {1, 0}, {6, 0}},
    {{6, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
    {{0, 3}, {6, 0}, {0, 3}, {2, 2}, {2, 2}, {0, 0}, {6, 0}},
    {{5, 2}, {6, 0}, {0, 3}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
    {{0, 3}, {0, 3}, {0, 3}, {6, 0}, {6, 0}, {6, 0}, {6, 0}}};

  mln::morpho::component_tree<uint8_t> ctree;
  mln::image2d<int>                    node_map;

  std::tie(ctree, node_map) = mln::morpho::maxtree(input, mln::c4);



  auto& par    = ctree.parent;
  auto  parent = make_parent_image(par.data(), par.size(), node_map);


  check_parent_array_sorted(par.data(), par.size());
  ASSERT_IMAGES_EQ_EXP(ref_parent, parent);
}


TEST(Morpho, Maxtree_filtering_min)
{
  const mln::image2d<uint8_t> input = {{10, 11, 11, 15, 16, 11, +2}, //
                                                     {+2, 10, 10, 10, 10, 10, 10}, //
                                                     {18, +2, 18, 19, 18, 14, +6}, //
                                                     {16, +2, 16, 10, 10, 10, 10}, //
                                                     {18, 16, 18, +2, +2, +2, +2}};

  const mln::image2d<uint8_t> ref1 = {{10, 10, 10, 10, 10, 10, +2}, //
                                                    {+2, 10, 10, 10, 10, 10, 10}, //
                                                    {16, +2, 16, 16, 16, 14, +6}, //
                                                    {16, +2, 16, 10, 10, 10, 10}, //
                                                    {16, 16, 16, +2, +2, +2, +2}};


  mln::morpho::component_tree<uint8_t> ctree;
  mln::image2d<int>                    node_map;

  {
    std::tie(ctree, node_map) = mln::morpho::maxtree(input, mln::c4);

    auto attr = ctree.compute_attribute_on_points(node_map, mln::accu::features::count<>());
    ctree.filter(mln::morpho::CT_FILTER_MIN, node_map,
                 [&attr](int node_id) { return attr[node_id] > 5; });

    auto out = ctree.reconstruct(node_map);
    ASSERT_IMAGES_EQ_EXP(out, ref1);
  }

}


TEST(Morpho, Maxtree_filtering_max)
{
  const mln::image2d<uint8_t> input = {{10, 11, 11, 15, 16, 11, +2}, //
                                                     {+2, 10, 10, 10, 10, 10, 10}, //
                                                     {18, +2, 18, 19, 18, 14, +6}, //
                                                     {16, +2, 16, 10, 10, 10, 10}, //
                                                     {18, 16, 18, +2, +2, +2, +2}};

  const mln::image2d<uint8_t> ref1 = {{10, 10, 10, 10, 10, 10, +2}, //
                                                    {+2, 10, 10, 10, 10, 10, 10}, //
                                                    {16, +2, 16, 16, 16, 14, +6}, //
                                                    {16, +2, 16, 10, 10, 10, 10}, //
                                                    {16, 16, 16, +2, +2, +2, +2}};


  mln::morpho::component_tree<uint8_t> ctree;
  mln::image2d<int>                    node_map;

  {
    std::tie(ctree, node_map) = mln::morpho::maxtree(input, mln::c4);

    auto attr = ctree.compute_attribute_on_points(node_map, mln::accu::features::count<>());
    ctree.filter(mln::morpho::CT_FILTER_MAX, node_map,
                 [&attr](int node_id) { return attr[node_id] > 5; });


    auto out = ctree.reconstruct(node_map);
    ASSERT_IMAGES_EQ_EXP(out, ref1);
  }

}


TEST(Morpho, Maxtree_filtering_direct)
{
  const mln::image2d<uint8_t> input = {{10, 11, 11, 15, 16, 11, +2}, //
                                                     {+2, 10, 10, 10, 10, 10, 10}, //
                                                     {18, +2, 18, 19, 18, 14, +6}, //
                                                     {16, +2, 16, 10, 10, 10, 10}, //
                                                     {18, 16, 18, +2, +2, +2, +2}};

  const mln::image2d<uint8_t> ref1 = {{10, 10, 10, 10, 10, 10, +2}, //
                                                    {+2, 10, 10, 10, 10, 10, 10}, //
                                                    {16, +2, 16, 16, 16, 14, +6}, //
                                                    {16, +2, 16, 10, 10, 10, 10}, //
                                                    {16, 16, 16, +2, +2, +2, +2}};


  mln::morpho::component_tree<uint8_t> ctree;
  mln::image2d<int>                    node_map;

  {
    std::tie(ctree, node_map) = mln::morpho::maxtree(input, mln::c4);

    auto attr = ctree.compute_attribute_on_points(node_map, mln::accu::features::count<>());
    ctree.filter(mln::morpho::CT_FILTER_DIRECT, node_map,
                 [&attr](int node_id) { return attr[node_id] > 5; });

    auto out = ctree.reconstruct(node_map);
    ASSERT_IMAGES_EQ_EXP(out, ref1);
  }

}

