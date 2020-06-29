#include <mln/core/range/mdspan.hpp>


#include <range/v3/range/conversion.hpp>
#include <range/v3/view/indices.hpp>
#include <range/v3/view/reverse.hpp>

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
  for (v[3] = 0; v[3] < count[3]; ++v[3])
    for (v[2] = 0; v[2] < count[2]; ++v[2])
      for (v[1] = 0; v[1] < count[1]; ++v[1])
        for (v[0] = 0; v[0] < count[0]; ++v[0])
        {
          std::ptrdiff_t offset = std::inner_product(v.begin(), v.end(), stride.begin(), std::ptrdiff_t(0));
          ptr[offset]           = i++;
        }
}

template <class value_type>
class MdspanTest : public ::testing::Test
{
public:
  static constexpr std::size_t Rank = std::tuple_size<value_type>::value;
  using T                           = typename value_type::value_type;
  using range_type                  = mln::ranges::mdspan<T, Rank>;

  static_assert(mln::ranges::MDRange<range_type>);

  MdspanTest()
  {
    constexpr int pad  = 3;
    m_count_vec        = {1, 1, 1, 1};
    m_stride_vec       = {0, 0, 0, 0};
    m_stride[0]        = 1;
    m_count[0]         = 3;
    m_size             = m_count[0];

    for (std::size_t k = 1; k < Rank; k++)
    {
      m_count[k]  = 2 * k + 1;
      m_stride[k] = m_stride[k - 1] * (m_count[k - 1] + pad);
      m_size *= m_count[k];
    }

    m_data.resize(m_stride[Rank - 1] * (m_count[Rank - 1] + pad));
    iota(m_data.data(), m_count_vec, m_stride_vec);
  }

  template <class R>
  static std::vector<T> rng_to_container(R rng)
  {
    std::vector<T> data;
    for (auto&& rows : rng.rows())
      for (auto v : rows)
        data.push_back(v);
    return data;
  }

  template <class R>
  static std::vector<T> rng_to_container_backward(R rng)
  {
    std::vector<T> data;
    for (auto&& rows : rng.rrows())
      for (auto v : rows)
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


using MyTypes = ::testing::Types<std::array<int, 1>, std::array<int, 2>, std::array<int, 3>, std::array<int, 4>>;
TYPED_TEST_SUITE(MdspanTest, MyTypes);

TYPED_TEST(MdspanTest, forward)
{
  typename TestFixture::range_type rng(this->m_data.data(), this->m_count.data(), this->m_stride.data());

  auto ref   = ::ranges::to_vector(::ranges::views::indices(int(this->m_size)));
  auto vrng  = this->rng_to_container(rng);
  ASSERT_EQ(ref, vrng);
}


TYPED_TEST(MdspanTest, backward)
{
  typename TestFixture::range_type rng(this->m_data.data(), this->m_count.data(), this->m_stride.data());

  auto ref   = ::ranges::to_vector(::ranges::views::reverse(::ranges::views::indices(int(this->m_size))));
  auto vrng  = this->rng_to_container_backward(rng);
  ASSERT_EQ(ref, vrng);
}

