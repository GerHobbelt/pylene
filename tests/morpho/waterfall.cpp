#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/utils/dontcare.hpp>
#include <mln/morpho/canvas/unionfind.hpp>
#include <mln/morpho/waterfall.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

#include <vector>

namespace
{
  struct mst_watershed_visitor
  {
    void init(int nlbl, mln::dontcare_t)
    {
      m_nlbl = nlbl + 1;
      m_zpar.resize(m_nlbl);
      std::iota(m_zpar.begin(), m_zpar.end(), 0);
    }
    bool on_waterline(int p, int q, mln::dontcare_t, mln::dontcare_t)
    {
      using mln::morpho::canvas::impl::zfindroot;
      auto rp = zfindroot(m_zpar.data(), p);
      auto rq = zfindroot(m_zpar.data(), q);
      if (rp != rq)
      {
        mst.emplace_back(p, q);
        m_zpar.push_back(m_nlbl);
        m_zpar[rp] = m_nlbl;
        m_zpar[rq] = m_nlbl;
        m_nlbl++;
      }
      return true;
    }
    void on_label(mln::dontcare_t, mln::dontcare_t) {}
    void finalize() {}

    std::vector<std::pair<int, int>> mst;

  private:
    std::vector<int> m_zpar;
    int              m_nlbl;
  };
} // namespace

TEST(Morpho, watershed_mst_c4)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 1, 0}, //
      {3, 3, 3, 2, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };
  std::vector<std::pair<int, int>> ref           = {{1, 2}, {3, 4}, {1, 3}};
  constexpr int                    kUnintialized = -1;
  mln::image2d<int>                out           = mln::imchvalue<int>(grad).set_init_value(kUnintialized).build();

  mst_watershed_visitor viz{};
  mln::morpho::impl::watershed(grad, mln::c4, mln::view::value_extended(out, kUnintialized), viz);
  int i = 0;
  for (auto [a, b] : viz.mst)
  {
    auto [min, max] = std::minmax(a, b);
    ASSERT_EQ(min, ref[i].first);
    ASSERT_EQ(max, ref[i++].second);
  }
}

TEST(Morpho, watershed_mst_c8)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 2, 0}, //
      {3, 3, 3, 1, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };
  constexpr int                    kUnintialized = -1;
  std::vector<std::pair<int, int>> ref           = {{1, 2}, {3, 4}, {1, 3}};
  mln::image2d<int>                out           = mln::imchvalue<int>(grad).set_init_value(kUnintialized).build();

  mst_watershed_visitor viz{};
  mln::morpho::impl::watershed(grad, mln::c8, mln::view::value_extended(out, kUnintialized), viz);
  int i = 0;
  for (auto [a, b] : viz.mst)
  {
    auto [min, max] = std::minmax(a, b);
    ASSERT_EQ(min, ref[i].first);
    ASSERT_EQ(max, ref[i++].second);
  }
}

TEST(Morpho, waterfall_c4)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 1, 0}, //
      {3, 3, 3, 2, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };
  const mln::image2d<int> nodemap_ref{
      {6, 6, 6, 7, 5}, //
      {6, 6, 6, 7, 5}, //
      {7, 7, 7, 3, 7}, //
      {4, 4, 7, 3, 3}, //
      {4, 4, 7, 3, 3}  //
  };
  std::vector<int> parent_ref{0, 0, 0, 1, 1, 2, 2};
  std::vector<int> values_ref{2, 1, 1, 0, 0, 0, 0};

  const auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c4);

  ASSERT_IMAGES_EQ_EXP(nodemap, nodemap_ref);
  ASSERT_EQ(t.parent, parent_ref);
  ASSERT_EQ(t.values, values_ref);
}

TEST(Morpho, waterfall_c8)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 2, 0}, //
      {3, 3, 3, 1, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };

  const mln::image2d<int> nodemap_ref{
      {6, 6, 6, 7, 5}, //
      {6, 6, 6, 7, 5}, //
      {7, 7, 7, 7, 7}, //
      {4, 4, 7, 3, 3}, //
      {4, 4, 7, 3, 3}  //
  };
  std::vector<int> parent_ref{0, 0, 0, 1, 1, 2, 2};
  std::vector<int> values_ref{2, 1, 1, 0, 0, 0, 0};

  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);

  ASSERT_IMAGES_EQ_EXP(nodemap, nodemap_ref);
  ASSERT_EQ(t.parent, parent_ref);
  ASSERT_EQ(t.values, values_ref);
}

TEST(Morpho, waterfall_saliency_c4)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 1, 0}, //
      {3, 3, 3, 2, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };

  const mln::image2d<int> sal_ref{
      {0, 0, 0, 1, 0}, //
      {0, 0, 0, 2, 0}, //
      {2, 2, 2, 0, 2}, //
      {0, 0, 1, 0, 0}, //
      {0, 0, 1, 0, 0}  //
  };

  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c4);
  auto sal          = mln::morpho::waterfall_saliency(t, nodemap, mln::c4);
  ASSERT_IMAGES_EQ_EXP(sal, sal_ref);
}

TEST(Morpho, waterfall_saliency_c8)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 2, 0}, //
      {3, 3, 3, 1, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };

  const mln::image2d<int> sal_ref{
      {0, 0, 0, 1, 0}, //
      {0, 0, 0, 1, 0}, //
      {2, 2, 2, 2, 2}, //
      {0, 0, 1, 0, 0}, //
      {0, 0, 1, 0, 0}  //
  };

  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);
  auto sal          = mln::morpho::waterfall_saliency(t, nodemap, mln::c8);
  ASSERT_IMAGES_EQ_EXP(sal, sal_ref);
}

TEST(Morpho, waterfall_saliency_c8_2)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 1, 0, 0, 3, 0, 0}, //
      {0, 0, 1, 0, 0, 3, 0, 0}, //
      {4, 4, 2, 2, 2, 4, 4, 4}, //
      {0, 0, 3, 0, 0, 1, 0, 0}, //
      {0, 0, 3, 0, 0, 1, 0, 0}  //
  };

  const mln::image2d<int> sal_ref{
      {0, 0, 1, 0, 0, 1, 0, 0}, //
      {0, 0, 1, 0, 0, 1, 0, 0}, //
      {2, 2, 2, 2, 2, 2, 2, 2}, //
      {0, 0, 1, 0, 0, 1, 0, 0}, //
      {0, 0, 1, 0, 0, 1, 0, 0}  //
  };

  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);
  auto sal          = mln::morpho::waterfall_saliency(t, nodemap, mln::c8);
  ASSERT_IMAGES_EQ_EXP(sal, sal_ref);
}

TEST(Morpho, waterfall_saliency_c8_3)
{
  // const mln::image2d<std::uint8_t> grad{
  //    {58, 104, 129, 128, 125, 115, 88}, //
  //    {60, 104, 133, 131, 125, 114, 82}, //
  //    {60, 101, 133, 135, 131, 113, 84}, //
  //    {60, 101, 131, 135, 131, 115, 93}, //
  //    {51, 101, 131, 131, 131, 123, 93}  //
  //};

  const mln::image2d<std::uint8_t> grad{
      {1, 8, 15, 14, 13, 11, 5}, //
      {2, 8, 17, 16, 13, 10, 3}, //
      {2, 7, 17, 18, 16, 9, 4},  //
      {2, 7, 16, 18, 16, 11, 6}, //
      {0, 7, 16, 16, 16, 12, 6}  //
  };

  const mln::image2d<int> sal_ref{
      {0, 0, 1, 0, 0, 0, 0}, //
      {0, 0, 1, 0, 0, 0, 0}, //
      {1, 1, 1, 1, 0, 0, 0}, //
      {0, 0, 0, 1, 0, 0, 0}, //
      {0, 0, 0, 1, 0, 0, 0}  //
  };

  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);
  auto sal          = mln::morpho::waterfall_saliency(t, nodemap, mln::c8);
  ASSERT_IMAGES_EQ_EXP(sal, sal_ref);
}

TEST(Morpho, waterfall_saliency_c8_4)
{
  const mln::image2d<std::uint8_t> grad{
      {1, 8, 15, 14, 13, 11, 5}, //
      {2, 8, 17, 16, 13, 10, 3}, //
      {2, 7, 17, 18, 16, 9, 4},  //
      {2, 7, 16, 18, 16, 11, 0}, //
      {0, 7, 16, 16, 16, 12, 6}  //
  };

  const mln::image2d<int> sal_ref{
      {0, 0, 2, 0, 0, 0, 0}, //
      {0, 0, 2, 0, 0, 0, 0}, //
      {1, 1, 2, 2, 1, 1, 1}, //
      {0, 0, 0, 2, 0, 0, 0}, //
      {0, 0, 0, 2, 0, 0, 0}  //
  };

  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);
  auto sal          = mln::morpho::waterfall_saliency(t, nodemap, mln::c8);
  ASSERT_IMAGES_EQ_EXP(sal, sal_ref);
}

TEST(Morpho, waterfall_saliency_c8_5)
{
  const mln::image2d<std::uint8_t> grad{
      {1, 8, 0, 14, 13, 11, 5},  //
      {2, 8, 17, 16, 13, 10, 3}, //
      {2, 7, 17, 18, 16, 9, 4},  //
      {2, 7, 16, 18, 16, 11, 0}, //
      {0, 7, 16, 16, 16, 12, 6}  //
  };

  const mln::image2d<int> sal_ref{
      {0, 1, 0, 2, 0, 0, 0}, //
      {0, 1, 0, 2, 0, 0, 0}, //
      {1, 1, 1, 2, 1, 1, 1}, //
      {0, 0, 0, 2, 0, 0, 0}, //
      {0, 0, 0, 2, 0, 0, 0}  //
  };

  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);
  auto sal          = mln::morpho::waterfall_saliency(t, nodemap, mln::c8);
  ASSERT_IMAGES_EQ_EXP(sal, sal_ref);
}

TEST(Morpho, waterfall_saliency_c8_6)
{
  const mln::image2d<int> nodemap{
      {6, 7, 5, 5, 5, 5}, //
      {6, 7, 7, 5, 5, 5}, //
      {6, 6, 7, 7, 7, 7}, //
      {6, 7, 7, 7, 3, 3}, //
      {7, 7, 4, 7, 3, 3}, //
      {4, 4, 4, 7, 3, 3}  //
  };

  mln::morpho::component_tree<int> t;
  t.parent = std::vector<int>{0, 0, 0, 1, 2, 1, 2, 7};
  t.values = std::vector<int>{2, 1, 1, 0, 0, 0, 0, 0};

  const mln::image2d<int> sal_ref{
      {0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0}  //
  };

  auto sal = mln::morpho::waterfall_saliency(t, nodemap, mln::c8);
  ASSERT_IMAGES_EQ_EXP(sal, sal_ref);
}