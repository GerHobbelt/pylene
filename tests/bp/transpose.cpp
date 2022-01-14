#include <mln/bp/transpose.hpp>
#include <mln/bp/alloc.hpp>


#include <gtest/gtest.h>
#include <range/v3/span.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>


template <class T>
class MatrixView
{
public:
  MatrixView(T* buffer, int width, int height, std::ptrdiff_t stride)
    : m_buffer{buffer}
    , m_width{width}
    , m_height{height}
    , m_stride{stride}
  {
  }

  MatrixView(T* buffer, int width, int height)
    : MatrixView(buffer, width, height, width * sizeof(T))
  {
  }


  bool operator==(const MatrixView& b) const noexcept
  {
    if (b.m_width != m_width || b.m_height != m_height)
      return false;

    for (int y = 0; y < m_height; ++y)
    {
      const T* lineptr_a = mln::bp::ptr_offset(m_buffer, y * m_stride);
      const T* lineptr_b = mln::bp::ptr_offset(b.m_buffer, y * b.m_stride);
      if (!std::equal(lineptr_a, lineptr_a + m_width, lineptr_b))
        return false;
    }
    return true;
  }


  friend void PrintTo(const MatrixView& m, std::ostream* os)
  {
    *os << "\n";
    for (int y = 0; y < m.m_height; ++y)
    {
      const T* lineptr = mln::bp::ptr_offset(m.m_buffer, y * m.m_stride);
      for (int x = 0; x < m.m_width - 1; ++x)
        fmt::print(*os, "{:02} ", lineptr[x]);
      fmt::print(*os, "{:02}", lineptr[m.m_width - 1]);
      *os << "\n";
    }
  }
private:
  T*             m_buffer;
  int            m_width;
  int            m_height;
  std::ptrdiff_t m_stride;
};


template <class Array>
class Transpose;

template <class T, int N>
class Transpose<T[N]> : public::testing::Test
{
protected:
  void test()
  {
    alignas(32) T data[N * N];
    alignas(32) T ref[N * N];

    for (int y = 0; y < N; ++y)
      for (int x = 0; x < N; ++x)
      {
        data[y * N + x] = y * N + x;
        ref[y * N + x]  = x * N + y;
      }

    mln::bp::transpose_inplace(data, N, sizeof(T) * N, N);
    ASSERT_EQ(MatrixView(data, N, N), MatrixView(ref, N, N));
  }
};

template <class T>
class TransposeLarge : public::testing::Test
{
public:
  void test_inplace(int n)
  {
    std::ptrdiff_t stride;
    T* data = (T*)mln::bp::aligned_alloc_2d(n, n, sizeof(T), stride);
    T* ref  = (T*)mln::bp::aligned_alloc_2d(n, n, sizeof(T), stride);

    for (int y = 0; y < n; ++y)
    {
      T* data_ptr = mln::bp::ptr_offset(data, y * stride);
      T* ref_ptr  = mln::bp::ptr_offset(ref, y * stride);
      for (int x = 0; x < n; ++x)
      {
        data_ptr[x] = T(y * n + x);
        ref_ptr[x]  = T(x * n + y);
      }
    }
    mln::bp::transpose_inplace(data, n, stride);
    ASSERT_EQ(MatrixView(data, n, n, stride), MatrixView(ref, n, n, stride));

    mln::bp::aligned_free_2d(data);
    mln::bp::aligned_free_2d(ref);
  }

  void test_out_of_place(int w, int h)
  {
    std::ptrdiff_t src_stride;
    std::ptrdiff_t dst_stride;

    T* src = (T*)mln::bp::aligned_alloc_2d(w, h, sizeof(T), src_stride);
    T* dst = (T*)mln::bp::aligned_alloc_2d(h, w, sizeof(T), dst_stride);
    T* ref = (T*)mln::bp::aligned_alloc_2d(h, w, sizeof(T), dst_stride);

    for (int y = 0; y < h; ++y)
    {
      T* lineptr = mln::bp::ptr_offset(src, y * src_stride);
      for (int x = 0; x < w; ++x)
        lineptr[x] = T(y * w + x);
    }
    for (int y = 0; y < w; ++y)
    {
      T* lineptr = mln::bp::ptr_offset(ref, y * dst_stride);
      for (int x = 0; x < h; ++x)
        lineptr[x] = T(x * w + y);
    }

    mln::bp::transpose(src, dst, h, w, src_stride, dst_stride);
    ASSERT_EQ(MatrixView(dst, h, w, dst_stride), MatrixView(ref, h, w, dst_stride));

    mln::bp::aligned_free_2d(src);
    mln::bp::aligned_free_2d(dst);
    mln::bp::aligned_free_2d(ref);
  }
};





using MyTypes = ::testing::Types<            //
    int64_t[2], int64_t[4], int64_t[8],      //
    int32_t[4], int32_t[8], int32_t[16],     //
    int16_t[8], int16_t[16], int16_t[32],    //
    int8_t[16], int8_t[32], int8_t[64],      //
    uint64_t[2], uint64_t[4], uint64_t[8],   //
    uint32_t[4], uint32_t[8], uint32_t[16],  //
    uint16_t[8], uint16_t[16], uint16_t[32], //
    uint8_t[16], uint8_t[32], uint8_t[64],   //
    double[2], double[4], double[8],         //
    float[4], float[8], float[16]            //
    >;

TYPED_TEST_SUITE(Transpose, MyTypes);
TYPED_TEST(Transpose, Transpose) { this->test(); }


struct rgb8
{
  rgb8() = default;
  rgb8(int x)
    : r{static_cast<uint8_t>(x)}
    , g{static_cast<uint8_t>(x)}
    , b{static_cast<uint8_t>(x)}
  {
  }

  bool operator==(rgb8 other) const { return std::make_tuple(r, g, b) == std::make_tuple(other.r, other.g, other.b); }

  operator int() const { return r; }

  uint8_t r, g, b;
};

struct rgba8
{

  rgba8(int x)
    : r{static_cast<uint8_t>(x)}
    , g{static_cast<uint8_t>(x)}
    , b{static_cast<uint8_t>(x)}
    , a{static_cast<uint8_t>(x)}
  {
  }

  bool operator==(rgba8 other) const { return std::make_tuple(r, g, b, a) == std::make_tuple(other.r, other.g, other.b, other.a); }

  operator int() const { return r; }

  uint8_t r, g, b, a;
};



using MyTypes2 = ::testing::Types<int64_t, int32_t, int16_t, int8_t,     //
                                  uint64_t, uint32_t, uint16_t, uint8_t, //
                                  double, float, rgb8, rgba8>;

TYPED_TEST_SUITE(TransposeLarge, MyTypes2);
TYPED_TEST(TransposeLarge, Transpose512)
{
  this->test_inplace(512);
}
TYPED_TEST(TransposeLarge, Transpose521)
{
  this->test_inplace(521);
}


TYPED_TEST(TransposeLarge, Transpose128x64)
{
  this->test_out_of_place(64, 128);
}
TYPED_TEST(TransposeLarge, Transpose131x67)
{
  this->test_out_of_place(67, 131);
}



