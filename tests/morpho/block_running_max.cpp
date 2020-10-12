#include <mln/morpho/private/block_running_max.hpp>


#include <functional>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>
#include <range/v3/view/span.hpp>


template <class T, class Compare>
void running_max_v_2d_naive(const T* input, int width, int height, int k, Compare cmp, T* output)
{

  for (int y = 0; y < height; ++y)
  {
    // Compute the Max input on the range [i - k, i + k]
    const T* begin = input + std::max(y - k, 0) * width;
    const T* end   = input + std::min(y + k + 1, height) * width;
    T*       out   = output + y * width;

    std::memcpy(out, begin, width * sizeof(T));
    begin += width;

    for (; begin != end; begin += width)
      std::transform(begin, begin + width, out, out, [cmp](auto a, auto b) { return std::max(a, b, cmp); });
  }
}

template <class T, class Compare>
void running_max_v_2d_g(const T* input, int width, int height, int k, Compare cmp, T* output)
{
  for (int y = 0; y < height; ++y)
  {
    // Compute the Max input on the range [⌊i/k⌋*k, i]
    const T* begin = input + (y / k) * k * width;
    const T* end   = input + std::min(height, y + 1) * width;
    T*       out   = output + y * width;

    std::memcpy(out, begin, width * sizeof(T));
    begin += width;

    for (; begin != end; begin += width)
      std::transform(begin, begin + width, out, out, [cmp](auto a, auto b) { return std::max(a, b, cmp); });
  }
}

template <class T, class Compare>
void running_max_v_2d_h(const T* input, int width, int height, int k, Compare cmp, T* output)
{
  for (int y = 0; y < height; ++y)
  {
    // Compute the Max input on the range [⌊i/k⌋*k, i]
    const T* begin = input + y * width;
    const T* end   = input + std::min(height, (y / k + 1) * k) * width;
    T*       out   = output + y * width;

    std::memcpy(out, begin, width * sizeof(T));
    begin += width;

    for (; begin != end; begin += width)
      std::transform(begin, begin + width, out, out, [cmp](auto a, auto b) { return std::max(a, b, cmp); });
  }
}

template <class T>
void compare_span2d(const T* a, const T* b, int width, int height)
{
  for (int y = 0; y < height; ++y)
    EXPECT_EQ(::ranges::make_span(a + y * width, width), ::ranges::make_span(b + y * width, width)) << "at line: " << y;
}


class RunningMax2dBase : public ::testing::TestWithParam<std::tuple<int, int>>
{
public:


  void Check(bool increasing) const
  {
    int n      = std::get<0>(this->GetParam());
    int radius = std::get<1>(this->GetParam());

    int width      = n;
    int height     = n;
    int stride     = width;

    int size       = width * height;
    int total_size = width * (height + 2 * radius);
    //int chunk_size = 2 * radius + 1;

    std::vector<int> input(size);
    if (increasing)
      std::iota(input.begin(), input.end(), 42);
    else
      std::iota(input.rbegin(), input.rend(), 42);

    std::vector<int> f(total_size, m_zero);
    std::copy(input.begin(), input.end(), f.data() + radius * width);

    // Generate references
    //std::vector<int> gref(total_size);
    //std::vector<int> href(total_size);
    //running_max_v_2d_g(f.data(), width, 2 * radius + height, chunk_size, m_cmp, gref.data());
    //running_max_v_2d_h(f.data(), width, 2 * radius + height, chunk_size, m_cmp, href.data());

    std::vector<int> out_ref(size);
    running_max_v_2d_naive(input.data(), width, height, radius, m_cmp, out_ref.data());

    // Run algorithm
    //std::vector<int> g(total_size);
    //std::vector<int> h(total_size);


    std::vector<int> out(total_size);
    sup_t sup = {m_sup, m_sup_vec};
    mln::morpho::details::block_running_max<128>(f.data() + radius * stride, width, height, stride * sizeof(int), radius, sup);


    // compare_span2d(gref.data(), g.data(), width, height + 2 * radius);
    // compare_span2d(href.data(), h.data(), width, height + 2 * radius);
    compare_span2d(out_ref.data(), f.data() + radius * stride, width, height);
    //EXPECT_EQ(href, h);
    //EXPECT_EQ(::ranges::make_span(fref.data(), size), ::ranges::make_span(f.data() + radius * width, size));
  }


protected:
  using simd_t = xsimd::simd_type<int>;

  struct sup_t
  {
    int    operator()(int x, int y) const { return m_sup(x, y); }
    simd_t operator()(const simd_t& x, const simd_t& y) const { return m_sup_vec(x, y); }

    std::function<int(int, int)>                         m_sup;
    std::function<simd_t(const simd_t&, const simd_t&)>  m_sup_vec;
  };


  std::function<int(int, int)>                         m_sup;
  std::function<simd_t(const simd_t&, const simd_t&)>  m_sup_vec;
  std::function<bool(int, int)>                        m_cmp;
  int                                                  m_zero;
};

class RunningMax2D : public RunningMax2dBase
{
public:


  RunningMax2D()
  {
    m_sup  = [](int x, int y) { return std::max(x, y); };
    m_sup_vec  = [](const simd_t& x, const simd_t& y) { return xsimd::max(x, y); };
    m_zero = 0;
    m_cmp  = std::less<int>();
  }
};

class RunningMin2D : public RunningMax2dBase
{
public:
  RunningMin2D()
  {
    m_sup  = [](int x, int y) { return std::min(x, y); };
    m_sup_vec  = [](const simd_t& x, const simd_t& y) { return xsimd::min(x, y); };
    m_zero = 255;
    m_cmp  = std::greater<int>();
  }
};

TEST_P(RunningMax2D, check)
{
  this->Check(true);
  this->Check(false);
}

TEST_P(RunningMin2D, check)
{
  this->Check(true);
  this->Check(false);
}




INSTANTIATE_TEST_CASE_P(se_leq_size, RunningMax2D,
                        ::testing::Values(std::make_tuple(0, 0),    // Identity
                                          std::make_tuple(12, 0),   // Identity
                                          std::make_tuple(12, 1),   // radius = 1
                                          std::make_tuple(13, 1),   // radius = 1
                                          std::make_tuple(14, 1),   // radius = 1
                                          std::make_tuple(12, 3),   // radius = 3
                                          std::make_tuple(13, 3),   // radius = 3
                                          std::make_tuple(14, 3),   // radius = 3
                                          std::make_tuple(13, 6))); // n == k

INSTANTIATE_TEST_CASE_P(se_ge_size_, RunningMax2D, ::testing::Values(std::make_tuple(12, 6)));

INSTANTIATE_TEST_CASE_P(se_leq_size, RunningMin2D,
                        ::testing::Values(std::make_tuple(0, 0),    // Identity
                                          std::make_tuple(12, 0),   // Identity
                                          std::make_tuple(12, 1),   // radius = 1
                                          std::make_tuple(13, 1),   // radius = 1
                                          std::make_tuple(14, 1),   // radius = 1
                                          std::make_tuple(12, 3),   // radius = 3
                                          std::make_tuple(13, 3),   // radius = 3
                                          std::make_tuple(14, 3),   // radius = 3
                                          std::make_tuple(13, 6))); // n == k

INSTANTIATE_TEST_CASE_P(se_ge_size_, RunningMin2D, ::testing::Values(std::make_tuple(12, 6)));

