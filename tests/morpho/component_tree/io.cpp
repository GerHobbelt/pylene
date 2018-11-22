#include <algorithm>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/maxtree/maxtree.hpp>
#include <sstream>

#include <gtest/gtest.h>

#define MLN_IMG_PATH "../../img"

TEST(Morpho, component_tree_io)
{

  using namespace mln;

  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  auto ctree = morpho::maxtree_indexes(ima, c4);

  // std::stringstream stream(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  {
    std::ofstream stream("tree.out", std::ios_base::binary);
    morpho::save(ctree, stream);
  }

  morpho::component_tree<unsigned, image2d<unsigned>> tree2;
  {
    std::ifstream stream("tree.out", std::ios_base::binary);
    morpho::load(stream, tree2);
  }

  // Compare the tree.
  auto data1 = ctree._get_data();
  auto data2 = tree2._get_data();

  ASSERT_EQ(data1->m_nodes.size(), data2->m_nodes.size());
  ASSERT_TRUE(
      (std::equal(data1->m_nodes.begin(), data1->m_nodes.end(), data2->m_nodes.begin(),
                  [](const morpho::internal::component_tree_node& a, const morpho::internal::component_tree_node& b) {
                    return std::memcmp(&a, &b, sizeof(morpho::internal::component_tree_node)) == 0;
                  })));

  ASSERT_TRUE(data1->m_S == data2->m_S);
  ASSERT_TRUE(all(data1->m_pmap == data2->m_pmap));
}
