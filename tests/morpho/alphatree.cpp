#include <mln/morpho/alphatree.hpp>

#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>

#include <fixtures/ImagePath/image_path.hpp>
#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>


mln::image2d<int> //
cut(const mln::morpho::component_tree<int>& t, mln::image2d<int>& node_map, int alpha)
{
  std::size_t n = t.parent.size();

  std::vector<int> labels(n, -1);

  int nlabel = 0;
  {
    mln_foreach_new(auto px, node_map.pixels())
      if (labels[px.val()] == -1)
        labels[px.val()] = nlabel++;
  }

  for (std::size_t i = n - 1; i > 0; --i)
    if (t.values[t.parent[i]] <= alpha) // progate to parent
      if (labels[t.parent[i]] == -1)
        labels[t.parent[i]] = labels[i];

  // Propagation downward
  for (std::size_t i = n - 1; i > 0; --i)
    if (t.values[t.parent[i]] <= alpha) // progate to parent
      labels[i] = labels[t.parent[i]];

  mln::image2d<int> imlabel;
  imlabel.resize(node_map.domain());

  mln_foreach_new(auto px, imlabel.pixels())
    px.val() = labels[node_map(px.point())];

  return imlabel;
}


TEST(Morpho, AlphaTree)
{
  mln::image2d<uint8_t>    ima = {
    {128, 124, 150, 137, 106}, //
    {116, 128, 156, 165, 117}, //
    {117,  90, 131, 108, 151}, //
    {107,  87, 118, 109, 167}, //
    {107,  73, 125, 157, 117}, //
  };
  auto [t, node_map] = mln::morpho::alphatree(ima, mln::c4);

  mln::image2d<int> ref0 = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {15, 20, 21, 22, 23}, //
  };

  mln::image2d<int> ref1 = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {5, 11, 12, 13, 14},  //
      {15, 16, 17, 13, 19}, //
      {15, 20, 21, 22, 23}, //
  };

  mln::image2d<int> ref10 = {
      {0, 0, 2, 3, 4},      //
      {5, 0, 2, 8, 9},      //
      {5, 11, 12, 13, 14},  //
      {15, 11, 17, 13, 19}, //
      {15, 20, 17, 22, 23}, //
  };


  mln::image2d<int> ref11 = {
      {0, 0, 2, 3, 4},      //
      {5, 0, 2, 8, 4},      //
      {5, 11, 12, 13, 14},  //
      {15, 11, 17, 13, 19}, //
      {15, 20, 17, 22, 23}, //
  };


  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 0), ref0);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 1), ref1);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 10), ref10);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 11), ref11);
}
