#include <benchmark/benchmark.h>

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>

#include <vector>

void Mult_Inplace(mln::image2d<mln::uint8>& img);
void Threshold_Inplace(mln::image2d<mln::uint8>& img);
void LUT_Inplace(const mln::uint8* LUT, mln::image2d<mln::uint8>& img);

void Mult_Inplace_C(mln::uint8* buffer, int width, int height, std::ptrdiff_t stride);
void Threshold_Inplace_C(mln::uint8* buffer, int width, int height, std::ptrdiff_t stride);
void LUT_Inplace_C(const mln::uint8* LUT, mln::uint8* buffer, int width, int height, std::ptrdiff_t stride);

void Mult(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Threshold(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void LUT(const mln::uint8* LUT, const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);

void Mult_C(const mln::uint8* ibuffer, mln::uint8* obuffer, int width, int height, std::ptrdiff_t stride);
void Threshold_C(const mln::uint8* ibuffer, mln::uint8* obuffer, int width, int height, std::ptrdiff_t stride);
void LUT_C(const mln::uint8* LUT, const mln::uint8* ibuffer, mln::uint8* obuffer, int width, int height,
           std::ptrdiff_t stride);

void Mult_New_Values(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Mult_New_Pixels(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);

void Mult_Inplace_New_Values(mln::image2d<mln::uint8>& img);
void Threshold_Inplace_New_Values(mln::image2d<mln::uint8>& img);
void LUT_Inplace_New_Values(const mln::uint8 LUT[], mln::image2d<mln::uint8>& img);

void Mult_Inplace_New_Pixels(mln::image2d<mln::uint8>& img);
void Threshold_Inplace_New_Pixels(mln::image2d<mln::uint8>& img);
void LUT_Inplace_New_Pixels(const mln::uint8 LUT[], mln::image2d<mln::uint8>& img);

class Bench_Ref_Linear : public benchmark::Fixture
{
  const char* filename = "../../img/exif.jpg";

  virtual void SetUp(const benchmark::State&) override
  {
    mln::image2d<mln::rgb8> tmp(0, 0);
    mln::io::imread(filename, tmp);
    mln::resize(m_input, tmp);
    mln::resize(m_output, tmp);
    mln::copy(mln::red(tmp), m_input);

    m_lut.resize(256);
    for (int i = 0; i < 256; i++)
      m_lut[i] = i;

    m_height = m_input.nrows();
    m_width = m_input.ncols();
    m_stride = m_input.strides()[0];
  }

protected:
  using fun1_t = std::function<void(mln::image2d<mln::uint8>&)>;
  using fun2_t = std::function<void(const mln::image2d<mln::uint8>&, mln::image2d<mln::uint8>&)>;
  using fun3_t = std::function<void(mln::uint8*, int, int, std::ptrdiff_t)>;
  using fun4_t = std::function<void(const mln::uint8*, mln::uint8*, int, int, std::ptrdiff_t)>;

  void runit_impl(benchmark::State& st, std::function<void()> f)
  {
    while (st.KeepRunning())
      f();
    st.SetBytesProcessed(st.iterations() * m_input.domain().size());
  }

  void runit(benchmark::State& st, fun1_t f) { runit_impl(st, std::bind(f, m_input)); }
  void runit(benchmark::State& st, fun2_t f) { runit_impl(st, std::bind(f, m_input, m_output)); }
  void runit(benchmark::State& st, fun3_t f)
  {
    runit_impl(st, std::bind(f, &m_input.at(0, 0), m_width, m_height, m_stride));
  }
  void runit(benchmark::State& st, fun4_t f)
  {
    runit_impl(st, std::bind(f, &m_input.at(0, 0), &m_output.at(0, 0), m_width, m_height, m_stride));
  }

protected:
  int m_width;
  int m_height;
  std::ptrdiff_t m_stride;

  mln::image2d<mln::uint8> m_input;
  mln::image2d<mln::uint8> m_output;
  std::vector<mln::uint8> m_lut;
};

BENCHMARK_F(Bench_Ref_Linear, Mult_C)(benchmark::State& st)
{
  runit(st, Mult_C);
}

BENCHMARK_F(Bench_Ref_Linear, Mult)(benchmark::State& st)
{
  runit(st, Mult);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_New_Values)(benchmark::State& st)
{
  runit(st, Mult_New_Values);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_New_Pixels)(benchmark::State& st)
{
  runit(st, Mult_New_Pixels);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_C)(benchmark::State& st)
{
  runit(st, Mult_Inplace_C);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace)(benchmark::State& st)
{
  runit(st, Mult_Inplace);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_New_Values)(benchmark::State& st)
{
  runit(st, Mult_Inplace_New_Values);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_New_Pixels)(benchmark::State& st)
{
  runit(st, Mult_Inplace_New_Pixels);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_C)(benchmark::State& st)
{
  runit(st, Threshold_C);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold)(benchmark::State& st)
{
  runit(st, Threshold);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Inplace_C)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_C);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Inplace)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_C);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Inplace_New_Values)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_New_Values);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Inplace_New_Pixels)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_New_Pixels);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_C)(benchmark::State& st)
{
  runit(st, fun4_t([&](auto&&... args) { LUT_C(m_lut.data(), args...); }));
}

BENCHMARK_F(Bench_Ref_Linear, LUT)(benchmark::State& st)
{
  runit(st, fun2_t([&](auto&&... args) { LUT(m_lut.data(), args...); }));
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace_C)(benchmark::State& st)
{
  runit(st, fun3_t([&](auto&&... args) { LUT_Inplace_C(m_lut.data(), args...); }));
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace)(benchmark::State& st)
{
  runit(st, fun1_t([&](auto&&... args) { LUT_Inplace(m_lut.data(), args...); }));
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace_New_Values)(benchmark::State& st)
{
  runit(st, fun1_t([&](auto&&... args) { LUT_Inplace_New_Values(m_lut.data(), args...); }));
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace_New_Pixels)(benchmark::State& st)
{
  runit(st, fun1_t([&](auto&&... args) { LUT_Inplace_New_Pixels(m_lut.data(), args...); }));
}

BENCHMARK_MAIN();
