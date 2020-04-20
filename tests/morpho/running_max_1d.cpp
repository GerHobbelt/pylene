#include <mln/morpho/canvas/private/running_max_1d.hpp>

#include <functional>
#include <vector>
#include <numeric>
#include <gtest/gtest.h>

template <class T, class Compare>
void running_max_1d_naive(const T* input, int size, int k, int offset, Compare cmp, T zero, T* output)
{
  for (int i = 0; i < size; ++i)
  {
    // Compute the Max input on the range [i - offset, i - offset + k)
    const T* begin = input + std::max(i + offset, 0);
    const T* end = input + std::min(i + offset + k, size);
    T v = (begin < end) ? *(std::max_element(begin, end, cmp)) : zero;
    output[i] = v;
  }
}

template <class T, class Compare>
void running_max_1d_g(const T* input, int size, int k, Compare cmp, T* output)
{
  for (int i = 0; i < size; ++i)
  {
    // Compute the Max input on the range (floor(i / k) * k, i)
    const T* begin = input + k * (i / k);
    const T* end = input + std::min(i + 1, size);
    const T* v = std::max_element(begin, end, cmp);
    output[i] = *v;
  }
}

template <class T, class Compare>
void running_max_1d_h(const T* input, int size, int k, Compare cmp, T* output)
{
  for (int i = 0; i < size; ++i)
  {
    // Compute the Max input on the range [i, k * ceil(i / k))
    const T* begin = input + i;
    const int ceil = (i % k == 0) ? i : (i / k + 1) * k;
    const T* end = input + std::min(ceil + 1, size);
    const T* v = std::max_element(begin, end, cmp);
    output[i] = *v;
  }
}




class RunningMax1d : public ::testing::TestWithParam< std::tuple<int, int, int> >
{
public:
  RunningMax1d(bool compute_max = true)
  {
    if (compute_max)
    {
      m_sup = [](int x, int y) { return std::max(x,y); };
      m_zero = 0;
      m_cmp = std::less<int>();
    }
    else
    {
      m_sup = [](int x, int y) { return std::min(x,y); };
      m_zero = 255;
      m_cmp = std::greater<int>();
    }
  }

  void Check(bool increasing) const
  {
    int size = std::get<0>(this->GetParam());
    int k = std::get<1>(this->GetParam());
    int offset = std::get<2>(this->GetParam());

    std::vector<int> input(size);
    if (increasing)
      std::iota(input.begin(), input.end(), 42);
    else
      std::iota(input.rbegin(), input.rend(), 42);

    std::vector<int> f = input;
    std::vector<int> g(size);
    std::vector<int> h(size);


    // Run algorithm
    mln::morpho::internal::running_max_1d(f.data(), g.data(), h.data(), size, k, offset, m_sup, m_zero);

    // Generate references
    std::vector<int> gref(size);
    std::vector<int> href(size);
    std::vector<int> fref(size);
    running_max_1d_g(input.data(), size, k, m_cmp, gref.data());
    running_max_1d_h(input.data(), size, k, m_cmp, href.data());
    running_max_1d_naive(input.data(), size, k, offset, m_cmp, m_zero, fref.data());


    EXPECT_EQ(gref, g);
    EXPECT_EQ(href, h);
    EXPECT_EQ(fref, f);
  }

private:
  std::function<int (int, int)> m_sup;
  int m_zero;
  std::function<bool (int, int)> m_cmp;
};

class RunningMin1d : public RunningMax1d
{
public:
  RunningMin1d() : RunningMax1d(false) {}
};

TEST_P(RunningMax1d, check)
{
  this->Check(true);
  this->Check(false);
}

TEST_P(RunningMin1d, check)
{
  this->Check(true);
  this->Check(false);
}

INSTANTIATE_TEST_CASE_P(se_leq_size, RunningMax1d, ::testing::Values(std::make_tuple(1, 3, 0),    // Identity
                                                                     std::make_tuple(12, 1, 0),    // Identity
                                                                     std::make_tuple(12, 3, 0),    // n % k == 0
                                                                     std::make_tuple(13, 3, 0),    // n % k == 1
                                                                     std::make_tuple(14, 3, 0),    // n % 2 == 1
                                                                     std::make_tuple(12, 12, 0))); // n == k

INSTANTIATE_TEST_CASE_P(se_ge_size_, RunningMax1d, ::testing::Values(std::make_tuple(12, 13, 0))); // Identity

INSTANTIATE_TEST_CASE_P(negative_offset, RunningMax1d, ::testing::Values(std::make_tuple(1, 3, -1),  // Identity
                                                                         std::make_tuple(12, 1, -2), // Identity
                                                                         std::make_tuple(5, 7, -3),  // n multiple of k
                                                                         std::make_tuple(12, 3, -2), // n % k == 0
                                                                         std::make_tuple(13, 3, -2), // n % k == 1
                                                                         std::make_tuple(14, 3, -2), // n % k == 2
                                                                         std::make_tuple(12, 12, -2))); // n == k

INSTANTIATE_TEST_CASE_P(positive_offset, RunningMax1d, ::testing::Values(std::make_tuple(1, 3, 1),      // Identity
                                                                         std::make_tuple(12, 1, 10),    // Identity
                                                                         std::make_tuple(12, 3, 10),    // n % k = 0
                                                                         std::make_tuple(13, 3, 10),    // n % k = 1
                                                                         std::make_tuple(14, 3, 10),    // n % k = 2
                                                                         std::make_tuple(12, 12, 10))); // n == k

INSTANTIATE_TEST_CASE_P(offset_bigger_than_size, RunningMax1d,
                        ::testing::Values(std::make_tuple(13, 3, 14), std::make_tuple(13, 3, -14)));

INSTANTIATE_TEST_CASE_P(se_leq_size, RunningMin1d, ::testing::Values(std::make_tuple(1, 3, 0),     // Identity
                                                                     std::make_tuple(12, 1, 0),    // Identity
                                                                     std::make_tuple(12, 3, 0),    // n % k = 0
                                                                     std::make_tuple(13, 3, 0),    // n % k = 1
                                                                     std::make_tuple(14, 3, 0),    // n % k = 2
                                                                     std::make_tuple(12, 12, 0))); // n == k

INSTANTIATE_TEST_CASE_P(se_ge_size_, RunningMin1d, ::testing::Values(std::make_tuple(12, 13, 0))); // Identity

INSTANTIATE_TEST_CASE_P(negative_offset, RunningMin1d, ::testing::Values(std::make_tuple(1, 3, -1),  // Identity
                                                                         std::make_tuple(12, 1, -2), // Identity
                                                                         std::make_tuple(5, 7, -3),  // n multiple of k
                                                                         std::make_tuple(12, 3, -2), // n multiple of k
                                                                         std::make_tuple(13, 3, -2), // n multiple of k
                                                                         std::make_tuple(14, 3, -2), // n multiple of k
                                                                         std::make_tuple(12, 12, -2))); // n == k

INSTANTIATE_TEST_CASE_P(positive_offset, RunningMin1d, ::testing::Values(std::make_tuple(1, 3, 1),  // Identity
                                                                         std::make_tuple(12, 1, 10), // Identity
                                                                         std::make_tuple(12, 3, 10), // n multiple of k
                                                                         std::make_tuple(13, 3, 10), // n multiple of k
                                                                         std::make_tuple(14, 3, 10), // n multiple of k
                                                                         std::make_tuple(12, 12, 10))); // n == k

INSTANTIATE_TEST_CASE_P(offset_bigger_than_size, RunningMin1d,
                        ::testing::Values(std::make_tuple(13, 3, 14), std::make_tuple(13, 3, -14)));

