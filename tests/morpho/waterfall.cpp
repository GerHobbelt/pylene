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
    void init(int nlbl)
    {
      m_nlbl = nlbl + 1;
      m_zpar.resize(m_nlbl);
      std::iota(m_zpar.begin(), m_zpar.end(), 0);
    }
    void visit(int p, int q)
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
    }
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
  std::vector<int> parent_ref{0, 0, 0, 1, 1, 2, 2, 7};
  std::vector<int> values_ref{2, 1, 1, 0, 0, 0, 0, 0};

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
  std::vector<int> parent_ref{0, 0, 0, 1, 1, 2, 2, 7};
  std::vector<int> values_ref{2, 1, 1, 0, 0, 0, 0, 0};

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