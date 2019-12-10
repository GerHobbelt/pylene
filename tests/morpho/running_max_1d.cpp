#include <mln/morpho/experimental/private/running_max_1d.hpp>

#include <functional>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>
#include <range/v3/view/span.hpp>


template <class T, class Compare>
void running_max_1d_naive(const T* input, int size, int k, Compare cmp, T* output)
{
  for (int i = 0; i < size; ++i)
  {
    // Compute the Max input on the range [i - k, i + k]
    const T* begin = input + std::max(i - k, 0);
    const T* end   = input + std::min(i + k + 1, size);
    output[i]      = *(std::max_element(begin, end, cmp));
  }
}

template <class T, class Compare>
void running_max_1d_g(const T* input, int size, int k, Compare cmp, T* output)
{
  for (int i = 0; i < size; ++i)
  {
    // Compute the Max input on the range [⌊i/k⌋*k, i]
    const T* begin = input + k * (i / k);
    const T* end   = input + std::min(i + 1, size);
    const T* v     = std::max_element(begin, end, cmp);
    output[i]      = *v;
  }
}

template <class T, class Compare>
void running_max_1d_h(const T* input, int size, int k, Compare cmp, T* output)
{
  for (int i = 0; i < size; ++i)
  {
    // Compute the Max input on the range [i, k * (⌊i/k⌋+1) )
    const T*  begin = input + i;
    const T*  end   = input + std::min((i / k + 1) * k, size);
    const T*  v     = std::max_element(begin, end, cmp);
    output[i]       = *v;
  }
}


class RunningMax1dBase : public ::testing::TestWithParam<std::tuple<int, int>>
{
public:

  void Check(bool increasing) const
  {
    int n      = std::get<0>(this->GetParam());
    int radius = std::get<1>(this->GetParam());

    int size = n + 2 * radius;
    int chunk_size = 2 * radius + 1;

    std::vector<int> input(n);
    if (increasing)
      std::iota(input.begin(), input.end(), 42);
    else
      std::iota(input.rbegin(), input.rend(), 42);

    std::vector<int> f(size, m_zero);
    std::copy(input.begin(), input.end(), f.data() + radius);

    // Generate references
    std::vector<int> gref(size);
    std::vector<int> href(size);
    std::vector<int> fref(n);
    running_max_1d_g(f.data(), size, chunk_size, m_cmp, gref.data());
    running_max_1d_h(f.data(), size, chunk_size, m_cmp, href.data());
    running_max_1d_naive(input.data(), n, radius, m_cmp, fref.data());

    // Run algorithm
    std::vector<int> g(size);
    std::vector<int> h(size);
    mln::morpho::experimental::details::running_max_1d(f.data() + radius, g.data() + radius, h.data() + radius, n,
                                                       radius, m_sup);

    EXPECT_EQ(gref, g);
    EXPECT_EQ(href, h);
    EXPECT_EQ(::ranges::make_span(fref.data(), n), ::ranges::make_span(f.data() + radius, n));
  }

protected:
  std::function<int(int, int)>  m_sup;
  std::function<bool(int, int)> m_cmp;
  int                           m_zero;
};

class RunningMax1D : public RunningMax1dBase
{
public:
  RunningMax1D()
  {
    m_sup  = [](int x, int y) { return std::max(x, y); };
    m_zero = 0;
    m_cmp  = std::less<int>();
  }
};

class RunningMin1D : public RunningMax1dBase
{
public:
  RunningMin1D()
  {
    m_sup  = [](int x, int y) { return std::min(x, y); };
    m_zero = 255;
    m_cmp  = std::greater<int>();
  }
};

TEST_P(RunningMax1D, check)
{
  this->Check(true);
  this->Check(false);
}

TEST_P(RunningMin1D, check)
{
  this->Check(true);
  this->Check(false);
}




INSTANTIATE_TEST_CASE_P(se_leq_size, RunningMax1D,
                        ::testing::Values(std::make_tuple(0, 0),    // Identity
                                          std::make_tuple(12, 0),   // Identity
                                          std::make_tuple(12, 1),   // radius = 1
                                          std::make_tuple(13, 1),   // radius = 1
                                          std::make_tuple(14, 1),   // radius = 1
                                          std::make_tuple(12, 3),   // radius = 3
                                          std::make_tuple(13, 3),   // radius = 3
                                          std::make_tuple(14, 3),   // radius = 3
                                          std::make_tuple(13, 6))); // n == k

INSTANTIATE_TEST_CASE_P(se_ge_size_, RunningMax1D, ::testing::Values(std::make_tuple(12, 6)));

INSTANTIATE_TEST_CASE_P(se_leq_size, RunningMin1D,
                        ::testing::Values(std::make_tuple(0, 0),    // Identity
                                          std::make_tuple(12, 0),   // Identity
                                          std::make_tuple(12, 1),   // radius = 1
                                          std::make_tuple(13, 1),   // radius = 1
                                          std::make_tuple(14, 1),   // radius = 1
                                          std::make_tuple(12, 3),   // radius = 3
                                          std::make_tuple(13, 3),   // radius = 3
                                          std::make_tuple(14, 3),   // radius = 3
                                          std::make_tuple(13, 6))); // n == k

INSTANTIATE_TEST_CASE_P(se_ge_size_, RunningMin1D, ::testing::Values(std::make_tuple(12, 6)));

