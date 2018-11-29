#include <mln/core/rangev3/multi_span.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <mln/core/concept/new/concepts.hpp>

#include <range/v3/to_container.hpp>
#include <range/v3/view/indices.hpp>

#include <numeric>
#include <vector>

#include <gtest/gtest.h>

using vec_t = std::array<std::ptrdiff_t, 4>;

std::size_t number_from_vec_t(vec_t x)
{
  return x[0] + x[1] * 10e2 + x[2] * 10e4 + x[3] * 10e4;
}

template <class T>
void iota(T* ptr, vec_t count, vec_t stride)
{
  vec_t v;
  int   i = 0;
  for (v[0] = 0; v[0] < count[0]; ++v[0])
    for (v[1] = 0; v[1] < count[1]; ++v[1])
      for (v[2] = 0; v[2] < count[2]; ++v[2])
        for (v[3] = 0; v[3] < count[3]; ++v[3])
        {
          std::ptrdiff_t offset = std::inner_product(v.begin(), v.end(), stride.begin(), std::ptrdiff_t(0));
          ptr[offset]           = i++;
        }
}

template <class value_type>
class MultiSpanTest : public ::testing::Test
{
public:
  static constexpr std::size_t Rank = std::tuple_size<value_type>::value;
  using T                           = typename value_type::value_type;
  using range_type                  = mln::ranges::multi_span<T, Rank>;

  static_assert(Rank == 1 || mln::ranges::is_multidimensional_range<range_type>::value);
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::OutputRange<range_type, const T&>);
  static_assert(mln::concepts::SegmentedRange<range_type>);
  static_assert(mln::concepts::ReversibleRange<range_type>);
#else
  static_assert(::ranges::ForwardRange<range_type>());
  static_assert(::ranges::OutputRange<range_type, const T&>());
#endif // PYLENE_CONCEPT_TS_ENABLED


  MultiSpanTest()
  {
    constexpr int pad  = 3;
    m_count_vec        = {1, 1, 1, 1};
    m_stride_vec       = {0, 0, 0, 0};
    m_stride[Rank - 1] = 1;
    m_count[Rank - 1]  = 2 * (Rank - 1) + 1;
    m_size             = m_count[Rank - 1];

    for (std::size_t k = Rank - 1; k-- > 0;)
    {
      m_count[k]  = 2 * k + 1;
      m_stride[k] = m_stride[k + 1] * (m_count[k + 1] + pad);
      m_size *= m_count[k];
    }
    m_data.resize(m_stride[0] * (m_count[0] + pad));
    iota(m_data.data(), m_count_vec, m_stride_vec);
  }

  template <class R>
  static std::vector<T> rng_to_container_row_wise(R rng)
  {
    std::vector<T> data;
    for (auto&& rows : mln::ranges::rows(rng))
      for (auto v : rows)
        data.push_back(v);
    return data;
  }

  template <class R>
  static std::vector<T> rng_to_container(R rng)
  {
    std::vector<T> data;
    for (auto v : rng)
      data.push_back(v);
    return data;
  }

  std::size_t    m_size;
  std::vector<T> m_data;
  union
  {
    std::array<std::size_t, Rank> m_count;
    vec_t                         m_count_vec;
  };
  union
  {
    std::array<std::ptrdiff_t, Rank> m_stride;
    vec_t                            m_stride_vec;
  };
};


using MyTypes = ::testing::Types<std::array<int, 2>, std::array<int, 3>, std::array<int, 4>>;
TYPED_TEST_CASE(MultiSpanTest, MyTypes);

TYPED_TEST(MultiSpanTest, forward)
{
  typename TestFixture::range_type rng(this->m_data.data(), this->m_count, this->m_stride);

  auto ref   = ::ranges::to_vector(::ranges::view::indices(int(this->m_size)));
  auto vrng  = this->rng_to_container(rng);
  auto vrng2 = this->rng_to_container_row_wise(rng);
  ASSERT_EQ(ref, vrng);
  ASSERT_EQ(ref, vrng2);
}


TYPED_TEST(MultiSpanTest, backward)
{
  typename TestFixture::range_type rng(this->m_data.data(), this->m_count, this->m_stride);

  auto ref   = ::ranges::to_vector(::ranges::view::reverse(::ranges::view::indices(int(this->m_size))));
  auto vrng  = this->rng_to_container(rng.reversed());
  auto vrng2 = this->rng_to_container_row_wise(rng.reversed());
  ASSERT_EQ(ref, vrng);
  ASSERT_EQ(ref, vrng2);
}
