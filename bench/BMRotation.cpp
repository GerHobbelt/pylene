#ifdef _MSC_VER
#define __attribute__(A) __declspec A
#endif

#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <fixtures/ImagePath/image_path.hpp>

#include <benchmark/benchmark.h>

using namespace mln;

struct BMRotation : public benchmark::Fixture
{
  std::string filename = fixtures::ImagePath::concat_with_filename("lena.ppm");

  BMRotation()
  {
    io::imread(filename, m_input);

    short nr = m_input.domain().shape()[0];
    short nc = m_input.domain().shape()[1];
    m_output = image2d<rgb8>(box2d{{(short)(-nc + 1), 0}, {1, nr}});
    m_bytes  = nr * nc * sizeof(rgb8);
  }

  image2d<rgb8> m_input;
  image2d<rgb8> m_output;
  std::size_t   m_bytes;
};

template <class T>
__attribute__((noinline)) void rotate_ptr(const image2d<T>& f, image2d<T>& out)
{

  int      nr      = f.nrows();
  int      nc      = f.ncols();
  int      istride = static_cast<int>(f.strides()[0]);
  int      ostride = static_cast<int>(out.strides()[0]);
  const T* ptr_in  = &f(point2d{0, 0});

  for (int i = 0; i < nr; ++i)
  {
    T* ptr_out = &out(point2d{0, (short)i});
    for (int j = 0; j < nc; ++j)
    {
      *ptr_out = ptr_in[j];
      ptr_out  = (T*)((char*)ptr_out - ostride);
    }
    ptr_in = (T*)((char*)ptr_in + istride);
  }
}

template <class T>
__attribute__((noinline)) void rotate_p(const image2d<T>& f, image2d<T>& out)
{
  mln_foreach (point2d p, f.domain())
  {
    point2d q{(short)(-p[1]), p[0]};
    out(q) = f(p);
  }
}

template <class T>
__attribute__((noinline)) void rotate_pix(const image2d<T>& f, image2d<T>& out)
{
  mln_foreach (auto px, f.pixels())
  {
    point2d q{(short)(-px.point()[1]), px.point()[0]};
    out(q) = px.val();
  }
}

BENCHMARK_F(BMRotation, WithPtr)(benchmark::State& st)
{
  while (st.KeepRunning())
  {
    rotate_ptr(m_input, m_output);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMRotation, WithPoint)(benchmark::State& st)
{
  while (st.KeepRunning())
  {
    rotate_p(m_input, m_output);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMRotation, WithPixel)(benchmark::State& st)
{
  while (st.KeepRunning())
  {
    rotate_pix(m_input, m_output);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_MAIN();
