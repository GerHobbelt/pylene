#include <mln/core/image/ndimage.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/io/imread.hpp>

#include <vector>

#include <benchmark/benchmark.h>

#ifdef _MSC_VER
# define __restrict__ __restrict
#endif

void Mult_Inplace_C(uint8_t* buffer, int width, int height, std::ptrdiff_t stride);
void Threshold_Inplace_C(uint8_t* buffer, int width, int height, std::ptrdiff_t stride);
void LUT_Inplace_C(const uint8_t* LUT, uint8_t* buffer, int width, int height, std::ptrdiff_t stride);

void Mult_C(const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height, std::ptrdiff_t stride);
void Threshold_C(const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height, std::ptrdiff_t stride);
void LUT_C(const uint8_t* LUT, const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height,
           std::ptrdiff_t stride);

void Mult_New_Values(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Mult_New_Pixels(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void Threshold_New_Values(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Threshold_New_Pixels(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void LUT_New_Values(const uint8_t LUT[], const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void LUT_New_Pixels(const uint8_t LUT[], const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void Mult_Inplace_New_Values(mln::image2d<uint8_t>& img);
void Threshold_Inplace_New_Values(mln::image2d<uint8_t>& img);
void LUT_Inplace_New_Values(const uint8_t LUT[], mln::image2d<uint8_t>& img);

void Mult_Inplace_New_Pixels(mln::image2d<uint8_t>& img);
void Threshold_Inplace_New_Pixels(mln::image2d<uint8_t>& img);
void LUT_Inplace_New_Pixels(const uint8_t LUT[], mln::image2d<uint8_t>& img);





void Mult_Inplace_C_Reversed(uint8_t* buffer, int width, int height, std::ptrdiff_t stride);
void Threshold_Inplace_C_Reversed(uint8_t* buffer, int width, int height, std::ptrdiff_t stride);
void LUT_Inplace_C_Reversed(const uint8_t* LUT, uint8_t* buffer, int width, int height, std::ptrdiff_t stride);


void Mult_C_Reversed(const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height, std::ptrdiff_t stride);
void Threshold_C_Reversed(const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height, std::ptrdiff_t stride);
void LUT_C_Reversed(const uint8_t* LUT, const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height,
                    std::ptrdiff_t stride);

void Mult_New_Values_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Mult_New_Pixels_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void Threshold_New_Values_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Threshold_New_Pixels_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void LUT_New_Values_Reversed(const uint8_t LUT[], const mln::image2d<uint8_t>& input,
                             mln::image2d<uint8_t>& output);
void LUT_New_Pixels_Reversed(const uint8_t LUT[], const mln::image2d<uint8_t>& input,
                             mln::image2d<uint8_t>& output);

void Mult_Inplace_New_Values_Reversed(mln::image2d<uint8_t>& img);
void Threshold_Inplace_New_Values_Reversed(mln::image2d<uint8_t>& img);
void LUT_Inplace_New_Values_Reversed(const uint8_t LUT[], mln::image2d<uint8_t>& img);

void Mult_Inplace_New_Pixels_Reversed(mln::image2d<uint8_t>& img);
void Threshold_Inplace_New_Pixels_Reversed(mln::image2d<uint8_t>& img);
void LUT_Inplace_New_Pixels_Reversed(const uint8_t LUT[], mln::image2d<uint8_t>& img);


class BMReferenceLinear : public benchmark::Fixture
{
  const char* filename = "Space1_20MB.jpg";

  virtual void SetUp(const benchmark::State&) override
  {
    if (!m_initialized)
    {
      mln::image2d<mln::rgb8> ima;
      mln::io::imread(filename, ima);

      mln::resize(m_input, ima);
      mln::resize(m_output, ima);
      mln::transform(ima, m_input, [](mln::rgb8 x) -> uint8_t { return x[0]; });


      m_lut.resize(256);
      for (int i = 0; i < 256; i++)
        m_lut[i] = i;

      m_initialized = true;
    }

    m_height = m_input.height();
    m_width  = m_input.width();
    m_stride = m_input.stride();
  }

protected:
  using fun5_t = std::function<void(mln::image2d<uint8_t>&)>;
  using fun6_t = std::function<void(const mln::image2d<uint8_t>&, mln::image2d<uint8_t>&)>;

  using fun3_t = std::function<void(uint8_t*, int, int, std::ptrdiff_t)>;
  using fun4_t = std::function<void(const uint8_t*, uint8_t*, int, int, std::ptrdiff_t)>;

  void runit_impl(benchmark::State& st, std::function<void()> f)
  {
    while (st.KeepRunning())
      f();
    st.SetBytesProcessed(st.iterations() * m_input.domain().size());
  }

  void runit(benchmark::State& st, fun5_t f) { runit_impl(st, std::bind(f, m_input)); }
  void runit(benchmark::State& st, fun6_t f) { runit_impl(st, std::bind(f, m_input, m_output)); }

  void runit(benchmark::State& st, fun3_t f)
  {
    runit_impl(st, std::bind(f, m_input.buffer(), m_width, m_height, m_stride));
  }
  void runit(benchmark::State& st, fun4_t f)
  {
    runit_impl(st, std::bind(f, m_input.buffer(), m_output.buffer(), m_width, m_height, m_stride));
  }


protected:
  int            m_width;
  int            m_height;
  std::ptrdiff_t m_stride;

  static bool                                m_initialized;
  static mln::image2d<uint8_t> m_input;
  static mln::image2d<uint8_t> m_output;
  static std::vector<uint8_t>                m_lut;
};

bool                                BMReferenceLinear::m_initialized = false;
mln::image2d<uint8_t> BMReferenceLinear::m_input;
mln::image2d<uint8_t> BMReferenceLinear::m_output;
std::vector<uint8_t>                BMReferenceLinear::m_lut;


BENCHMARK_F(BMReferenceLinear, Mult_C)(benchmark::State& st)
{
  runit(st, Mult_C);
}

BENCHMARK_F(BMReferenceLinear, Mult_New_Values)(benchmark::State& st)
{
  runit(st, Mult_New_Values);
}

BENCHMARK_F(BMReferenceLinear, Mult_New_Pixels)(benchmark::State& st)
{
  runit(st, Mult_New_Pixels);
}

BENCHMARK_F(BMReferenceLinear, Mult_Inplace_C)(benchmark::State& st)
{
  runit(st, Mult_Inplace_C);
}

BENCHMARK_F(BMReferenceLinear, Mult_Inplace_New_Values)(benchmark::State& st)
{
  runit(st, Mult_Inplace_New_Values);
}

BENCHMARK_F(BMReferenceLinear, Mult_Inplace_New_Pixels)(benchmark::State& st)
{
  runit(st, Mult_Inplace_New_Pixels);
}

BENCHMARK_F(BMReferenceLinear, Threshold_C)(benchmark::State& st)
{
  runit(st, Threshold_C);
}

BENCHMARK_F(BMReferenceLinear, Threshold_New_Values)(benchmark::State& st)
{
  runit(st, Threshold_New_Values);
}

BENCHMARK_F(BMReferenceLinear, Threshold_New_Pixels)(benchmark::State& st)
{
  runit(st, Threshold_New_Pixels);
}

BENCHMARK_F(BMReferenceLinear, Threshold_Inplace_C)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_C);
}

BENCHMARK_F(BMReferenceLinear, Threshold_Inplace_New_Values)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_New_Values);
}

BENCHMARK_F(BMReferenceLinear, Threshold_Inplace_New_Pixels)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_New_Pixels);
}

BENCHMARK_F(BMReferenceLinear, LUT_C)(benchmark::State& st)
{
  runit(st, fun4_t([&](auto&&... args) { LUT_C(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_New_Values)(benchmark::State& st)
{
  runit(st, fun6_t([&](auto&&... args) { LUT_New_Values(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_New_Pixels)(benchmark::State& st)
{
  runit(st, fun6_t([&](auto&&... args) { LUT_New_Pixels(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_C)(benchmark::State& st)
{
  runit(st, fun3_t([&](auto&&... args) { LUT_Inplace_C(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_New_Values)(benchmark::State& st)
{
  runit(st, fun5_t([&](auto&&... args) { LUT_Inplace_New_Values(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_New_Pixels)(benchmark::State& st)
{
  runit(st, fun5_t([&](auto&&... args) { LUT_Inplace_New_Pixels(m_lut.data(), args...); }));
}


// Reversed


BENCHMARK_F(BMReferenceLinear, Mult_C_Reversed)(benchmark::State& st)
{
  runit(st, Mult_C_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Mult_New_Values_Reversed)(benchmark::State& st)
{
  runit(st, Mult_New_Values_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Mult_New_Pixels_Reversed)(benchmark::State& st)
{
  runit(st, Mult_New_Pixels_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Mult_Inplace_C_Reversed)(benchmark::State& st)
{
  runit(st, Mult_Inplace_C_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Mult_Inplace_New_Values_Reversed)(benchmark::State& st)
{
  runit(st, Mult_Inplace_New_Values_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Mult_Inplace_New_Pixels_Reversed)(benchmark::State& st)
{
  runit(st, Mult_Inplace_New_Pixels_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Threshold_C_Reversed)(benchmark::State& st)
{
  runit(st, Threshold_C_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Threshold_New_Values_Reversed)(benchmark::State& st)
{
  runit(st, Threshold_New_Values_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Threshold_New_Pixels_Reversed)(benchmark::State& st)
{
  runit(st, Threshold_New_Pixels_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Threshold_Inplace_C_Reversed)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_C_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Threshold_Inplace_New_Values_Reversed)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_New_Values_Reversed);
}

BENCHMARK_F(BMReferenceLinear, Threshold_Inplace_New_Pixels_Reversed)(benchmark::State& st)
{
  runit(st, Threshold_Inplace_New_Pixels_Reversed);
}

BENCHMARK_F(BMReferenceLinear, LUT_C_Reversed)(benchmark::State& st)
{
  runit(st, fun4_t([&](auto&&... args) { LUT_C_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_New_Values_Reversed)(benchmark::State& st)
{
  runit(st, fun6_t([&](auto&&... args) { LUT_New_Values_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_New_Pixels_Reversed)(benchmark::State& st)
{
  runit(st, fun6_t([&](auto&&... args) { LUT_New_Pixels_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_C_Reversed)(benchmark::State& st)
{
  runit(st, fun3_t([&](auto&&... args) { LUT_Inplace_C_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_New_Values_Reversed)(benchmark::State& st)
{
  runit(st, fun5_t([&](auto&&... args) { LUT_Inplace_New_Values_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_New_Pixels_Reversed)(benchmark::State& st)
{
  runit(st, fun5_t([&](auto&&... args) { LUT_Inplace_New_Pixels_Reversed(m_lut.data(), args...); }));
}


BENCHMARK_MAIN();
