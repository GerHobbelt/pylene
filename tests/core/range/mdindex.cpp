#include <mln/core/range/mdindex.hpp>

#include <vector>
#include <gtest/gtest.h>


using vec_t = std::array<int, 4>;



std::vector<vec_t> generate_forward_reference(vec_t from, vec_t to)
{
  std::vector<vec_t> data;
  vec_t              v;

  for (v[3] = from[3]; v[3] < to[3]; ++v[3])
    for (v[2] = from[2]; v[2] < to[2]; ++v[2])
      for (v[1] = from[1]; v[1] < to[1]; ++v[1])
        for (v[0] = from[0]; v[0] < to[0]; ++v[0])
          data.push_back(v);

  return data;
}

std::vector<vec_t> generate_backward_reference(vec_t from, vec_t to)
{
  std::vector<vec_t> data;
  vec_t              v;


  for (v[3] = to[3] - 1; v[3] >= from[3]; --v[3])
    for (v[2] = to[2] - 1; v[2] >= from[2]; --v[2])
      for (v[1] = to[1] - 1; v[1] >= from[1]; --v[1])
        for (v[0] = to[0] - 1; v[0] >= from[0]; --v[0])
          data.push_back(v);

  return data;
}


template <class value_type>
class MultiIndicesTest : public ::testing::Test
{
public:
  static constexpr std::size_t Rank = std::tuple_size<value_type>::value;
  using T                           = typename value_type::value_type;
  using range_type                  = mln::ranges::mdindex<Rank, T>;


  static_assert(mln::ranges::MDRange<range_type>);

  MultiIndicesTest()
  {
    for (int k = 0; k < static_cast<int>(Rank); ++k)
    {
      from[k]  = k;
      from_[k] = k;
      to[k]    = 10 - k;
      to_[k]   = 10 - k;
    }
  }

  template <class R>
  static std::vector<value_type> rng_to_container(R rng)
  {
    std::vector<value_type> data;
    for (auto&& rows : rng.rows())
      for (auto v : rows)
        data.push_back(v);
    return data;
  }

    template <class R>
  static std::vector<value_type> rng_to_container_backward(R rng)
  {
    std::vector<value_type> data;
    for (auto&& rows : rng.rrows())
      for (auto v : rows)
        data.push_back(v);
    return data;
  }



  static std::vector<value_type> to_container(const std::vector<vec_t>& from)
  {
    std::vector<value_type> to;
    to.resize(from.size());
    for (std::size_t i = 0; i < from.size(); ++i)
      for (std::size_t k = 0; k < Rank; ++k)
        to[i][k] = from[i][k];
    return to;
  }

  value_type from_;
  value_type to_;
  vec_t      from = {0, 0, 0, 0};
  vec_t      zero = {0, 0, 0, 0};
  vec_t      to   = {1, 1, 1, 1};
};


using MyTypes = ::testing::Types<std::array<int, 1>, std::array<int, 2>, std::array<int, 3>, std::array<int, 4>,
                                 std::array<std::uint8_t, 4>>;
TYPED_TEST_SUITE(MultiIndicesTest, MyTypes);



TYPED_TEST(MultiIndicesTest, origin_centered_forward)
{
  using T = typename TestFixture::T;

  std::vector<vec_t>               ref_ = generate_forward_reference(this->zero, this->to);
  mln::ranges::mdindex<TestFixture::Rank, T> rng(this->to_);

  auto ref  = this->to_container(ref_);
  auto res  = this->rng_to_container(rng);
  ASSERT_EQ(ref, res);
}




TYPED_TEST(MultiIndicesTest, origin_centered_backward)
{
  using T = typename TestFixture::T;

  std::vector<vec_t> ref_ = generate_backward_reference(this->zero, this->to);
  mln::ranges::mdindex<TestFixture::Rank, T> rng(this->to_);

  auto ref  = this->to_container(ref_);
  auto res  = this->rng_to_container_backward(rng);
  ASSERT_EQ(ref, res);
}



TYPED_TEST(MultiIndicesTest, forward)
{
  using T = typename TestFixture::T;

  std::vector<vec_t>               ref_ = generate_forward_reference(this->from, this->to);
  mln::ranges::mdindex<TestFixture::Rank, T> rng(this->from_, this->to_);

  auto ref  = this->to_container(ref_);
  auto res  = this->rng_to_container(rng);
  ASSERT_EQ(ref, res);
}



TYPED_TEST(MultiIndicesTest, backward)
{
  using T = typename TestFixture::T;

  std::vector<vec_t>               ref_ = generate_backward_reference(this->from, this->to);
  mln::ranges::mdindex<TestFixture::Rank, T> rng(this->from_, this->to_);

  auto ref  = this->to_container(ref_);
  auto res  = this->rng_to_container_backward(rng);
  ASSERT_EQ(ref, res);
}
