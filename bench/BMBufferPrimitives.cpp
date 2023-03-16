#include <benchmark/benchmark.h>
#include <mln/bp/transpose.hpp>
#include <mln/bp/alloc.hpp>

template <class T>
class BMPrimitives : public benchmark::Fixture
{
public:

  BMPrimitives()
  {
    m_dim = 256;
    m_buffer = (T*) mln::bp::aligned_alloc_2d(m_dim, m_dim, sizeof(T), m_stride);
    m_out = (T*) mln::bp::aligned_alloc_2d(m_dim, m_dim, sizeof(T), m_stride);
    for (int y = 0; y < m_dim; ++y)
    {
      T* lineptr = mln::bp::ptr_offset(m_buffer, y * m_stride);
      for (int i = 0; i < m_dim; ++i)
        lineptr[i] = y * m_dim + i;
    }
  }
  ~BMPrimitives() override
  {
    mln::bp::aligned_free_2d(m_buffer);
    mln::bp::aligned_free_2d(m_out);
  }

  void SetBytesProcessed(benchmark::State& st, int dim) const
  {
    st.SetBytesProcessed(sizeof(T) * st.iterations() * dim * dim);
  }

protected:
  T*             m_buffer;
  T*             m_out;
  int            m_dim;
  std::ptrdiff_t m_stride;
};


namespace baseline
{
  template <class T>
  [[gnu::noinline]]
  void transpose(T* matrix, int n, std::ptrdiff_t stride)
  {
    mln::bp::impl::transpose_inplace_naive(matrix, n, stride);
  }
}


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

#define BENCHMARK_TEMPLATE_INPLACE_BASELINE(TYPE)                                                                      \
  BENCHMARK_TEMPLATE_DEFINE_F(BMPrimitives, transpose_inplace_baseline_##TYPE, TYPE)(benchmark::State & st)            \
  {                                                                                                                    \
    int dim = st.range(0);                                                                                             \
    for (auto _ : st)                                                                                                  \
      baseline::transpose(this->m_buffer, dim, this->m_stride);                                                        \
    this->SetBytesProcessed(st, dim);                                                                                  \
  }                                                                                                                    \
  BENCHMARK_REGISTER_F(BMPrimitives, transpose_inplace_baseline_##TYPE)->Arg(64)->Arg(128)->Arg(131)->Arg(256);

#define BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(TYPE)                                                                     \
  BENCHMARK_TEMPLATE_DEFINE_F(BMPrimitives, transpose_inplace_optimized_##TYPE, TYPE)(benchmark::State & st)           \
  {                                                                                                                    \
    int dim = st.range(0);                                                                                             \
    for (auto _ : st)                                                                                                  \
      mln::bp::transpose_inplace(this->m_buffer, dim, this->m_stride);                                                 \
    this->SetBytesProcessed(st, dim);                                                                                  \
  }                                                                                                                    \
  BENCHMARK_REGISTER_F(BMPrimitives, transpose_inplace_optimized_##TYPE)->Arg(64)->Arg(128)->Arg(131)->Arg(256);

#define BENCHMARK_TEMPLATE_OPTIMIZED(TYPE)                              \
  BENCHMARK_TEMPLATE_DEFINE_F(BMPrimitives, transpose_optimized_##TYPE, TYPE)(benchmark::State & st)                   \
  {                                                                                                                    \
    int dim = st.range(0);                                                                                             \
    for (auto _ : st)                                                                                                  \
      mln::bp::transpose(this->m_buffer, this->m_out, dim, dim, this->m_stride, this->m_stride);                       \
    this->SetBytesProcessed(st, dim);                                                                                  \
  }                                                                                                                    \
  BENCHMARK_REGISTER_F(BMPrimitives, transpose_optimized_##TYPE)->Arg(64)->Arg(128)->Arg(131)->Arg(256);


BENCHMARK_TEMPLATE_INPLACE_BASELINE(uint8_t);
BENCHMARK_TEMPLATE_INPLACE_BASELINE(uint16_t);
BENCHMARK_TEMPLATE_INPLACE_BASELINE(uint32_t);
BENCHMARK_TEMPLATE_INPLACE_BASELINE(uint64_t);
BENCHMARK_TEMPLATE_INPLACE_BASELINE(float);
BENCHMARK_TEMPLATE_INPLACE_BASELINE(double);
BENCHMARK_TEMPLATE_INPLACE_BASELINE(rgb8);
BENCHMARK_TEMPLATE_INPLACE_BASELINE(rgba8);

BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(uint8_t);
BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(uint16_t);
BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(uint32_t);
BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(uint64_t);
BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(float);
BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(double);
BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(rgb8);
BENCHMARK_TEMPLATE_INPLACE_OPTIMIZED(rgba8);

BENCHMARK_TEMPLATE_OPTIMIZED(uint8_t);
BENCHMARK_TEMPLATE_OPTIMIZED(uint16_t);
BENCHMARK_TEMPLATE_OPTIMIZED(uint32_t);
BENCHMARK_TEMPLATE_OPTIMIZED(uint64_t);
BENCHMARK_TEMPLATE_OPTIMIZED(float);
BENCHMARK_TEMPLATE_OPTIMIZED(double);
BENCHMARK_TEMPLATE_OPTIMIZED(rgb8);
BENCHMARK_TEMPLATE_OPTIMIZED(rgba8);

BENCHMARK_MAIN();