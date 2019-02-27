#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>

#include <benchmark/benchmark.h>


void fill_baseline(mln::image2d<mln::uint8>& ima, mln::uint8 v);
void fill_baseline(mln::image2d<mln::rgb8>& ima, mln::rgb8 v);
void fill(mln::image2d<mln::uint8>& ima, mln::uint8 v);
void fill(mln::image2d<mln::rgb8>& ima, mln::rgb8 v);

void copy_baseline(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out);
void copy_baseline(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out);
void copy(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out);
void copy(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out);

std::ptrdiff_t count_if_baseline(const mln::image2d<mln::uint8>& in);
std::ptrdiff_t count_if_baseline(const mln::image2d<mln::rgb8>& in);
std::ptrdiff_t count_if(const mln::image2d<mln::uint8>& in);
std::ptrdiff_t count_if(const mln::image2d<mln::rgb8>& in);

std::ptrdiff_t count_baseline(const mln::image2d<mln::uint8>& in);
std::ptrdiff_t count_baseline(const mln::image2d<mln::rgb8>& in);
std::ptrdiff_t count(const mln::image2d<mln::uint8>& in);
std::ptrdiff_t count(const mln::image2d<mln::rgb8>& in);

bool equal_baseline(const mln::image2d<mln::uint8>& lhs, const mln::image2d<mln::uint8>& rhs);
bool equal_baseline(const mln::image2d<mln::rgb8>& lhs, const mln::image2d<mln::rgb8>& rhs);
bool equal(const mln::image2d<mln::uint8>& lhs, const mln::image2d<mln::uint8>& rhs);
bool equal(const mln::image2d<mln::rgb8>& lhs, const mln::image2d<mln::rgb8>& rhs);

void paste_baseline(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out);
void paste_baseline(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out);
void paste(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out);
void paste(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out);

void transform_baseline(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out);
void transform_baseline(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out);
void transform(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out);
void transform(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out);

void for_each_baseline(mln::image2d<mln::uint8>& in);
void for_each_baseline(mln::image2d<mln::rgb8>& in);
void for_each(mln::image2d<mln::uint8>& in);
void for_each(mln::image2d<mln::rgb8>& in);


void generate_baseline(mln::image2d<mln::uint8>& ima);
void generate(mln::image2d<mln::uint8>& ima);

int accumulate_baseline(const mln::image2d<mln::uint8>& ima);
int accumulate_accu(const mln::image2d<mln::uint8>& ima);
int accumulate(const mln::image2d<mln::uint8>& ima);

std::vector<mln::point2d> sort_points(const mln::image2d<mln::uint8>& ima);
std::vector<mln::point2d> sort_points(const mln::image2d<int>& ima);
std::vector<mln::point2d> sort_points(const mln::image2d<mln::rgb8>& ima);

class Bench : public benchmark::Fixture
{
  virtual void SetUp(const benchmark::State&) override
  {
    const char* filename = "Space1_20MB.jpg";


    mln::io::imread(filename, m_input_rgb8);
    m_input_uint8 = mln::clone(mln::red(m_input_rgb8));

    m_pixel_count = m_input_rgb8.domain().size();
  }

protected:
  std::size_t              m_pixel_count;
  mln::image2d<mln::uint8> m_input_uint8;
  mln::image2d<mln::rgb8>  m_input_rgb8;
};

//////// FILL //////////////////

BENCHMARK_F(Bench, fill_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    fill_baseline(m_input_uint8, 69);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, fill_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    fill(m_input_uint8, 69);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, fill_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    fill_baseline(m_input_rgb8, mln::rgb8{2, 3, 4});
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, fill_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    fill(m_input_rgb8, mln::rgb8{2, 3, 4});
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// COPY //////////////////

BENCHMARK_F(Bench, copy_buffer2d_uint8_baseline)(benchmark::State& st)
{
  mln::image2d<mln::uint8> output_uint8(m_input_uint8, mln::init());
  while (st.KeepRunning())
    copy_baseline(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, copy_buffer2d_uint8)(benchmark::State& st)
{
  mln::image2d<mln::uint8> output_uint8(m_input_uint8, mln::init());
  while (st.KeepRunning())
    copy(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, copy_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  mln::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::init());
  while (st.KeepRunning())
    copy_baseline(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, copy_ibuffer2d_rgb8)(benchmark::State& st)
{
  mln::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::init());
  while (st.KeepRunning())
    copy(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// COUNT_IF //////////////////

BENCHMARK_F(Bench, count_if_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_if_baseline(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, count_if_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_if(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, count_if_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_if_baseline(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, count_if_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_if(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// COUNT //////////////////

BENCHMARK_F(Bench, count_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_baseline(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, count_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    count(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, count_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    count_baseline(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, count_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    count(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// EQUAL //////////////////

BENCHMARK_F(Bench, equal_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    equal_baseline(m_input_uint8, m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, equal_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    equal(m_input_uint8, m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, equal_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    equal_baseline(m_input_rgb8, m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, equal_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    equal(m_input_rgb8, m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// PASTE //////////////////

BENCHMARK_F(Bench, paste_buffer2d_uint8_baseline)(benchmark::State& st)
{
  mln::image2d<mln::uint8> output_uint8(m_input_uint8, mln::init());
  while (st.KeepRunning())
    paste_baseline(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, paste_buffer2d_uint8)(benchmark::State& st)
{
  mln::image2d<mln::uint8> output_uint8(m_input_uint8, mln::init());
  while (st.KeepRunning())
    paste(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, paste_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  mln::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::init());
  while (st.KeepRunning())
    paste_baseline(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, paste_ibuffer2d_rgb8)(benchmark::State& st)
{
  mln::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::init());
  while (st.KeepRunning())
    paste(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

//////// TRANSFORM //////////////////

BENCHMARK_F(Bench, transform_buffer2d_uint8_baseline)(benchmark::State& st)
{
  mln::image2d<mln::uint8> output_uint8(m_input_uint8, mln::init());
  while (st.KeepRunning())
    transform_baseline(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, transform_buffer2d_uint8)(benchmark::State& st)
{
  mln::image2d<mln::uint8> output_uint8(m_input_uint8, mln::init());
  while (st.KeepRunning())
    transform(m_input_uint8, output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, transform_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  mln::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::init());
  while (st.KeepRunning())
    transform_baseline(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, transform_ibuffer2d_rgb8)(benchmark::State& st)
{
  mln::image2d<mln::rgb8> output_rgb8(m_input_rgb8, mln::init());
  while (st.KeepRunning())
    transform(m_input_rgb8, output_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


//////// FOR_EACH //////////////////

BENCHMARK_F(Bench, for_each_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    for_each_baseline(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, for_each_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    for_each(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, for_each_ibuffer2d_rgb8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    for_each_baseline(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, for_each_ibuffer2d_rgb8)(benchmark::State& st)
{
  while (st.KeepRunning())
    for_each(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


///////////   GENERATE /////////////////

BENCHMARK_F(Bench, generate_buffer2d_uint8_baseline)(benchmark::State& st)
{
  mln::image2d<mln::uint8> output_uint8(m_input_uint8, mln::init());
  while (st.KeepRunning())
    generate_baseline(output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, generate_buffer2d_uint8)(benchmark::State& st)
{
  mln::image2d<mln::uint8> output_uint8(m_input_uint8, mln::init());
  while (st.KeepRunning())
    generate(output_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


///////////   ACCUMULATE /////////////////

BENCHMARK_F(Bench, accumulate_buffer2d_uint8_baseline)(benchmark::State& st)
{
  while (st.KeepRunning())
    accumulate_baseline(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, accumulate_buffer2d_uint8_accu)(benchmark::State& st)
{
  while (st.KeepRunning())
    accumulate_accu(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


BENCHMARK_F(Bench, accumulate_buffer2d_uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
    accumulate(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

///////////   SORT /////////////////

BENCHMARK_F(Bench, sort_points_buffer2d_small_int)(benchmark::State& st)
{
  while (st.KeepRunning())
    sort_points(m_input_uint8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(Bench, sort_points_buffer2d_large_int)(benchmark::State& st)
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

  mln::image2d<int> tmp = mln::experimental::transform(m_input_rgb8, bit_mix);
  while (st.KeepRunning())
    sort_points(tmp);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


BENCHMARK_F(Bench, sort_points_buffer2d_rgb8_lex)(benchmark::State& st)
{
  while (st.KeepRunning())
    sort_points(m_input_rgb8);
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


BENCHMARK_MAIN();
