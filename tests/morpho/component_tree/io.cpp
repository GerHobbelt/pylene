#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/maxtree/maxtree.hpp>

#include <fixtures/ImagePath/image_path.hpp>

#include <algorithm>
#include <sstream>

#include <gtest/gtest.h>


// FIXME:
namespace to_migrate
{
  template <typename I>
  inline bool __all(const mln::Image<I>& ima)
  {
    static_assert(std::is_convertible<typename I::value_type, bool>::value,
                  "Image value type must be convertible to bool");

    mln_viter(v, exact(ima));
    mln_forall (v)
      if (!*v)
        return false;

    return true;
  }
} // namespace to_migrate

TEST(Morpho, component_tree_io)
{

  using namespace mln;

  image2d<uint8_t> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

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
  // FIXME:
  ASSERT_TRUE(::to_migrate::__all(data1->m_pmap == data2->m_pmap));
}
