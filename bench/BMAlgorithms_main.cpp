#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/io/experimental/imread.hpp>

#include <benchmark/benchmark.h>


void fill_baseline(mln::experimental::image2d<uint8_t>& ima, uint8_t v);
void fill_baseline(mln::experimental::image2d<mln::rgb8>& ima, mln::rgb8 v);
void fill(mln::experimental::image2d<uint8_t>& ima, uint8_t v);
void fill(mln::experimental::image2d<mln::rgb8>& ima, mln::rgb8 v);

void copy_baseline(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out);
void copy_baseline(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out);
void copy(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out);
void copy(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out);

std::ptrdiff_t count_if_baseline(const mln::experimental::image2d<uint8_t>& in);
std::ptrdiff_t count_if_baseline(const mln::experimental::image2d<mln::rgb8>& in);
std::ptrdiff_t count_if(const mln::experimental::image2d<uint8_t>& in);
std::ptrdiff_t count_if(const mln::experimental::image2d<mln::rgb8>& in);

std::ptrdiff_t count_baseline(const mln::experimental::image2d<uint8_t>& in);
std::ptrdiff_t count_baseline(const mln::experimental::image2d<mln::rgb8>& in);
std::ptrdiff_t count(const mln::experimental::image2d<uint8_t>& in);
std::ptrdiff_t count(const mln::experimental::image2d<mln::rgb8>& in);

bool equal_baseline(const mln::experimental::image2d<uint8_t>& lhs, const mln::experimental::image2d<uint8_t>& rhs);
bool equal_baseline(const mln::experimental::image2d<mln::rgb8>& lhs, const mln::experimental::image2d<mln::rgb8>& rhs);
bool equal(const mln::experimental::image2d<uint8_t>& lhs, const mln::experimental::image2d<uint8_t>& rhs);
bool equal(const mln::experimental::image2d<mln::rgb8>& lhs, const mln::experimental::image2d<mln::rgb8>& rhs);

void paste_baseline(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out);
void paste_baseline(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out);
void paste(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out);
void paste(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out);

void transform_baseline(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out);
void transform_baseline(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out);
void transform(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out);
void transform(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out);

void for_each_baseline(mln::experimental::image2d<uint8_t>& in);
void for_each_baseline(mln::experimental::image2d<mln::rgb8>& in);
void for_each(mln::experimental::image2d<uint8_t>& in);
void for_each(mln::experimental::image2d<mln::rgb8>& in);


void generate_baseline(mln::experimental::image2d<uint8_t>& ima);
void generate(mln::experimental::image2d<uint8_t>& ima);

int accumulate_baseline(const mln::experimental::image2d<uint8_t>& ima);
int accumulate_accu(const mln::experimental::image2d<uint8_t>& ima);
int accumulate(const mln::experimental::image2d<uint8_t>& ima);

std::vector<mln::experimental::point2d> sort_points(const mln::experimental::image2d<uint8_t>& ima);
std::vector<mln::experimental::point2d> sort_points(const mln::experimental::image2d<int>& ima);
std::vector<mln::experimental::point2d> sort_points(const mln::experimental::image2d<mln::rgb8>& ima);

class BMAlgorithms : public benchmark::Fixture
{
  virtual void SetUp(const benchmark::State&) override
  {
    const char* filename = "Space1_20MB.jpg";


    mln::io::experimental::imread(filename, m_input_rgb8);
    m_input_uint8 = mln::transform(m_input_rgb8, [](mln::rgb8 x) -> uint8_t { return x[0]; });
    m_pixel_count = m_input_rgb8.domain().size();
  }

protected:
  std::size_t                            m_pixel_count;
  mln::experimental::image2d<uint8_t>    m_input_uint8;
  mln::experimental::image2d<mln::rgb8>  m_input_rgb8;
};

//////// FILL //////////////////

BENCHMARK_F(BMAlgorithms, fill_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    fill_baseline(m_input_uint8, 69);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, fill_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    fill(m_input_uint8, 69);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, fill_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    fill_baseline(m_input_rgb8, mln::rgb8{2, 3, 4});
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, fill_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    fill(m_input_rgb8, mln::rgb8{2, 3, 4});
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// COPY //////////////////

BENCHMARK_F(BMAlgorithms, copy_buffer2d_uint8_baseline)(benchmark::State& st)
{
  mln::experimental::image2d<uint8_t> output_uint8(m_input_uint8, mln::image_build_params{});
  while (st.KeepRunning())
    copy_baseline(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, copy_buffer2d_uint8)(benchmark::State& st)
{
  mln::experimental::image2d<uint8_t> output_uint8(m_input_uint8, mln::image_build_params{});
  while (st.KeepRunning())
    copy(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, copy_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  mln::experimental::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::image_build_params{});
  while (st.KeepRunning())
    copy_baseline(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, copy_ibuffer2d_rgb8)(benchmark::State& st)
{
  mln::experimental::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::image_build_params{});
  while (st.KeepRunning())
    copy(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// COUNT_IF //////////////////

BENCHMARK_F(BMAlgorithms, count_if_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_if_baseline(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, count_if_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_if(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, count_if_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_if_baseline(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, count_if_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_if(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// COUNT //////////////////

BENCHMARK_F(BMAlgorithms, count_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_baseline(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, count_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    count(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, count_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_baseline(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, count_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    count(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// EQUAL //////////////////

BENCHMARK_F(BMAlgorithms, equal_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    equal_baseline(m_input_uint8, m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, equal_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    equal(m_input_uint8, m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, equal_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    equal_baseline(m_input_rgb8, m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, equal_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    equal(m_input_rgb8, m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// PASTE //////////////////

BENCHMARK_F(BMAlgorithms, paste_buffer2d_uint8_baseline)(benchmark::State& st)
{
  mln::experimental::image2d<uint8_t> output_uint8(m_input_uint8, mln::image_build_params{});
  while (st.KeepRunning())
    paste_baseline(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, paste_buffer2d_uint8)(benchmark::State& st)
{
  mln::experimental::image2d<uint8_t> output_uint8(m_input_uint8, mln::image_build_params{});
  while (st.KeepRunning())
    paste(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, paste_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  mln::experimental::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::image_build_params{});
  while (st.KeepRunning())
    paste_baseline(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, paste_ibuffer2d_rgb8)(benchmark::State& st)
{
  mln::experimental::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::image_build_params{});
  while (st.KeepRunning())
    paste(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// TRANSFORM //////////////////

BENCHMARK_F(BMAlgorithms, transform_buffer2d_uint8_baseline)(benchmark::State& st)
{
  mln::experimental::image2d<uint8_t> output_uint8(m_input_uint8, mln::image_build_params{});
  while (st.KeepRunning())
    transform_baseline(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, transform_buffer2d_uint8)(benchmark::State& st)
{
  mln::experimental::image2d<uint8_t> output_uint8(m_input_uint8, mln::image_build_params{});
  while (st.KeepRunning())
    transform(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, transform_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  mln::experimental::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::image_build_params{});
  while (st.KeepRunning())
    transform_baseline(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, transform_ibuffer2d_rgb8)(benchmark::State& st)
{
  mln::experimental::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::image_build_params{});
  while (st.KeepRunning())
    transform(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


//////// FOR_EACH //////////////////

BENCHMARK_F(BMAlgorithms, for_each_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    for_each_baseline(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, for_each_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    for_each(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, for_each_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    for_each_baseline(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, for_each_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    for_each(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


///////////   GENERATE /////////////////

BENCHMARK_F(BMAlgorithms, generate_buffer2d_uint8_baseline)(benchmark::State& st)
{
  mln::experimental::image2d<uint8_t> output_uint8(m_input_uint8, mln::image_build_params{});
  while (st.KeepRunning())
    generate_baseline(output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, generate_buffer2d_uint8)(benchmark::State& st)
{
  mln::experimental::image2d<uint8_t> output_uint8(m_input_uint8, mln::image_build_params{});
  while (st.KeepRunning())
    generate(output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


///////////   ACCUMULATE /////////////////

BENCHMARK_F(BMAlgorithms, accumulate_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    accumulate_baseline(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, accumulate_buffer2d_uint8_accu)(benchmark::State& st)
{
  while (st.KeepRunning())
    accumulate_accu(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


BENCHMARK_F(BMAlgorithms, accumulate_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    accumulate(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

///////////   SORT /////////////////

BENCHMARK_F(BMAlgorithms, sort_points_buffer2d_small_int)(benchmark::State& st)
{
  while (st.KeepRunning())
    sort_points(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMAlgorithms, sort_points_buffer2d_large_int)(benchmark::State& st)
{
  auto bit_mix = [](mln::rgb8 x) -> int {
    int res = 0;
    res |= ((x[0] & (1 << 7)) << 16) | ((x[1] & (1 << 7)) << 15) | ((x[2] & (1 << 7)) << 14);
    res |= ((x[0] & (1 << 6)) << 14) | ((x[1] & (1 << 6)) << 13) | ((x[2] & (1 << 6)) << 12);
    res |= ((x[0] & (1 << 5)) << 12) | ((x[1] & (1 << 5)) << 11) | ((x[2] & (1 << 5)) << 10);
    res |= ((x[0] & (1 << 4)) << 10) | ((x[1] & (1 << 4)) << 9) | ((x[2] & (1 << 4)) << 8);
    res |= ((x[0] & (1 << 3)) << 8) | ((x[1] & (1 << 3)) << 7) | ((x[2] & (1 << 3)) << 6);
    res |= ((x[0] & (1 << 2)) << 6) | ((x[1] & (1 << 2)) << 5) | ((x[2] & (1 << 2)) << 4);
    res |= ((x[0] & (1 << 1)) << 4) | ((x[1] & (1 << 1)) << 3) | ((x[2] & (1 << 1)) << 2);
    res |= ((x[0] & (1 << 0)) << 2) | ((x[1] & (1 << 0)) << 1) | ((x[2] & (1 << 1)) << 0);
    return res;
  };

  mln::experimental::image2d<int> tmp = mln::transform(m_input_rgb8, bit_mix);
  while (st.KeepRunning())
    sort_points(tmp);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


BENCHMARK_F(BMAlgorithms, sort_points_buffer2d_rgb8_lex)(benchmark::State& st)
{
  while (st.KeepRunning())
    sort_points(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


BENCHMARK_MAIN();
