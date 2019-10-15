#include <benchmark/benchmark.h>

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/colors.hpp>
#include <mln/io/experimental/imread.hpp>

#include <vector>

void Mult_Inplace(mln::image2d<uint8_t>& img);
void Threshold_Inplace(mln::image2d<uint8_t>& img);
void LUT_Inplace(const uint8_t* LUT, mln::image2d<uint8_t>& img);

void Mult_Inplace_C(uint8_t* buffer, int width, int height, std::ptrdiff_t stride);
void Threshold_Inplace_C(uint8_t* buffer, int width, int height, std::ptrdiff_t stride);
void LUT_Inplace_C(const uint8_t* LUT, uint8_t* buffer, int width, int height, std::ptrdiff_t stride);

void Mult(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Threshold(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void LUT(const uint8_t* LUT, const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void Mult_C(const uint8_t* ibuffer, uint8_t* obuffer, int width, int height, std::ptrdiff_t stride);
void Threshold_C(const uint8_t* ibuffer, uint8_t* obuffer, int width, int height, std::ptrdiff_t stride);
void LUT_C(const uint8_t* LUT, const uint8_t* ibuffer, uint8_t* obuffer, int width, int height,
           std::ptrdiff_t stride);

void Mult_New_Values(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);
void Mult_New_Pixels(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);

void Threshold_New_Values(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);
void Threshold_New_Pixels(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);

void LUT_New_Values(const uint8_t* LUT, const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);
void LUT_New_Pixels(const uint8_t* LUT, const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);

void Mult_Inplace_New_Values(mln::experimental::image2d<uint8_t>& img);
void Threshold_Inplace_New_Values(mln::experimental::image2d<uint8_t>& img);
void LUT_Inplace_New_Values(const uint8_t LUT[], mln::experimental::image2d<uint8_t>& img);

void Mult_Inplace_New_Pixels(mln::experimental::image2d<uint8_t>& img);
void Threshold_Inplace_New_Pixels(mln::experimental::image2d<uint8_t>& img);
void LUT_Inplace_New_Pixels(const uint8_t LUT[], mln::experimental::image2d<uint8_t>& img);


void Mult_Inplace_Reversed(mln::image2d<uint8_t>& img);
void Threshold_Inplace_Reversed(mln::image2d<uint8_t>& img);
void LUT_Inplace_Reversed(const uint8_t* LUT, mln::image2d<uint8_t>& img);

void Mult_Inplace_C_Reversed(uint8_t* buffer, int width, int height, std::ptrdiff_t stride);
void Threshold_Inplace_C_Reversed(uint8_t* buffer, int width, int height, std::ptrdiff_t stride);
void LUT_Inplace_C_Reversed(const uint8_t* LUT, uint8_t* buffer, int width, int height, std::ptrdiff_t stride);

void Mult_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Threshold_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void LUT_Reversed(const uint8_t* LUT, const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void Mult_C_Reversed(const uint8_t* ibuffer, uint8_t* obuffer, int width, int height, std::ptrdiff_t stride);
void Threshold_C_Reversed(const uint8_t* ibuffer, uint8_t* obuffer, int width, int height, std::ptrdiff_t stride);
void LUT_C_Reversed(const uint8_t* LUT, const uint8_t* ibuffer, uint8_t* obuffer, int width, int height,
                    std::ptrdiff_t stride);

void Mult_New_Values_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Mult_New_Pixels_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void Threshold_New_Values_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Threshold_New_Pixels_Reversed(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

void LUT_New_Values_Reversed(const uint8_t* LUT, const mln::image2d<uint8_t>& input,
                             mln::image2d<uint8_t>& output);
void LUT_New_Pixels_Reversed(const uint8_t* LUT, const mln::image2d<uint8_t>& input,
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
      mln::experimental::image2d<mln::rgb8> ima;
      mln::io::experimental::imread(filename, ima);

      mln::resize(m_exp_input, ima);
      mln::resize(m_exp_output, ima);
      mln::transform(ima, m_exp_input, [](mln::rgb8 x) -> uint8_t { return x[0]; });


      m_lut.resize(256);
      for (int i = 0; i < 256; i++)
        m_lut[i] = i;

      m_exp_input.to(m_input, true);
      mln::resize(m_output, m_input);

      m_initialized = true;
    }

    m_height = m_exp_input.height();
    m_width  = m_exp_input.width();
    m_stride = m_exp_input.strides();
  }

protected:
  using fun1_t = std::function<void(mln::image2d<uint8_t>&)>;
  using fun2_t = std::function<void(const mln::image2d<uint8_t>&, mln::image2d<uint8_t>&)>;

  using fun5_t = std::function<void(mln::experimental::image2d<uint8_t>&)>;
  using fun6_t = std::function<void(const mln::experimental::image2d<uint8_t>&, mln::experimental::image2d<uint8_t>&)>;

  using fun3_t = std::function<void(uint8_t*, int, int, std::ptrdiff_t)>;
  using fun4_t = std::function<void(const uint8_t*, uint8_t*, int, int, std::ptrdiff_t)>;

  void runit_impl(benchmark::State& st, std::function<void()> f)
  {
    while (st.KeepRunning())
      f();
    st.SetBytesProcessed(st.iterations() * m_input.domain().size());
  }

  void runit(benchmark::State& st, fun1_t f) { runit_impl(st, std::bind(f, m_input)); }
  void runit(benchmark::State& st, fun2_t f) { runit_impl(st, std::bind(f, m_input, m_output)); }

  void runit(benchmark::State& st, fun5_t f) { runit_impl(st, std::bind(f, m_exp_input)); }
  void runit(benchmark::State& st, fun6_t f) { runit_impl(st, std::bind(f, m_exp_input, m_exp_output)); }

  void runit(benchmark::State& st, fun3_t f)
  {
    runit_impl(st, std::bind(f, &m_input.at(0, 0), m_width, m_height, m_stride));
  }
  void runit(benchmark::State& st, fun4_t f)
  {
    runit_impl(st, std::bind(f, &m_input.at(0, 0), &m_output.at(0, 0), m_width, m_height, m_stride));
  }


protected:
  int            m_width;
  int            m_height;
  std::ptrdiff_t m_stride;

  static bool                                m_initialized;
  static mln::image2d<uint8_t>               m_input;
  static mln::image2d<uint8_t>               m_output;
  static mln::experimental::image2d<uint8_t> m_exp_input;
  static mln::experimental::image2d<uint8_t> m_exp_output;
  static std::vector<uint8_t>                m_lut;
};

bool                                BMReferenceLinear::m_initialized = false;
mln::image2d<uint8_t>               BMReferenceLinear::m_input;
mln::image2d<uint8_t>               BMReferenceLinear::m_output;
mln::experimental::image2d<uint8_t> BMReferenceLinear::m_exp_input;
mln::experimental::image2d<uint8_t> BMReferenceLinear::m_exp_output;
std::vector<uint8_t>                BMReferenceLinear::m_lut;


BENCHMARK_F(BMReferenceLinear, Mult_C)(benchmark::State& st)
{
  runit(st, Mult_C);
}

BENCHMARK_F(BMReferenceLinear, Mult)(benchmark::State& st)
{
  runit(st, Mult);
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

BENCHMARK_F(BMReferenceLinear, Mult_Inplace)(benchmark::State& st)
{
  runit(st, Mult_Inplace);
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

BENCHMARK_F(BMReferenceLinear, Threshold)(benchmark::State& st)
{
  runit(st, Threshold);
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

BENCHMARK_F(BMReferenceLinear, Threshold_Inplace)(benchmark::State& st)
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

BENCHMARK_F(BMReferenceLinear, LUT)(benchmark::State& st)
{
  runit(st, fun2_t([&](auto&&... args) { LUT(m_lut.data(), args...); }));
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

BENCHMARK_F(BMReferenceLinear, LUT_Inplace)(benchmark::State& st)
{
  runit(st, fun1_t([&](auto&&... args) { LUT_Inplace(m_lut.data(), args...); }));
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

BENCHMARK_F(BMReferenceLinear, Mult_Reversed)(benchmark::State& st)
{
  runit(st, Mult_Reversed);
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

BENCHMARK_F(BMReferenceLinear, Mult_Inplace_Reversed)(benchmark::State& st)
{
  runit(st, Mult_Inplace_Reversed);
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

BENCHMARK_F(BMReferenceLinear, Threshold_Reversed)(benchmark::State& st)
{
  runit(st, Threshold_Reversed);
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

BENCHMARK_F(BMReferenceLinear, Threshold_Inplace_Reversed)(benchmark::State& st)
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

BENCHMARK_F(BMReferenceLinear, LUT_Reversed)(benchmark::State& st)
{
  runit(st, fun2_t([&](auto&&... args) { LUT_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_New_Values_Reversed)(benchmark::State& st)
{
  runit(st, fun2_t([&](auto&&... args) { LUT_New_Values_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_New_Pixels_Reversed)(benchmark::State& st)
{
  runit(st, fun2_t([&](auto&&... args) { LUT_New_Pixels_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_C_Reversed)(benchmark::State& st)
{
  runit(st, fun3_t([&](auto&&... args) { LUT_Inplace_C_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_Reversed)(benchmark::State& st)
{
  runit(st, fun1_t([&](auto&&... args) { LUT_Inplace_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_New_Values_Reversed)(benchmark::State& st)
{
  runit(st, fun1_t([&](auto&&... args) { LUT_Inplace_New_Values_Reversed(m_lut.data(), args...); }));
}

BENCHMARK_F(BMReferenceLinear, LUT_Inplace_New_Pixels_Reversed)(benchmark::State& st)
{
  runit(st, fun1_t([&](auto&&... args) { LUT_Inplace_New_Pixels_Reversed(m_lut.data(), args...); }));
}


BENCHMARK_MAIN();
