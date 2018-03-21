#ifdef _MSC_VER
#define __attribute__(A) __declspec A
#endif

#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <benchmark/benchmark.h>

using namespace mln;

struct BMRotation : public benchmark::Fixture
{
  const char* filename = MLN_IMG_PATH "/lena.ppm";

  BMRotation()
  {
    io::imread(filename, m_input);

    short nr = m_input.domain().shape()[0];
    short nc = m_input.domain().shape()[1];
    mln::box2d domain = { {0,0}, {nc, nr} };
    m_output.resize(domain);
    m_bytes = nr * nc * sizeof(rgb8);
  }

  image2d<rgb8> m_input;
  image2d<rgb8> m_output;
  std::size_t m_bytes;
};

template <class T>
__attribute__((noinline)) void rotate_ptr(const image2d<T>& f, image2d<T>& out)
{

  int nr = f.nrows();
  int nc = f.ncols();
  auto istride = f.stride();
  auto ostride = out.stride();
  const T* ptr_in = reinterpret_cast<const T*>(f.buffer());

  for (int i = 0; i < nr; ++i)
  {
    T* ptr_out = &out.at(0,i);
    for (int j = 0; j < nc; ++j)
    {
      *ptr_out = ptr_in[j];
      ptr_out += ostride;
    }
    ptr_in += istride;
  }
}

template <class T>
__attribute__((noinline)) void rotate_p(const image2d<T>& f, image2d<T>& out)
{
  using P = typename image2d<T>::point_type;
  mln_foreach (auto p, f.domain())
  {
    P q = {p[1], p[0]};
    out(q) = f(p);
  }
}

template <class T>
__attribute__((noinline)) void rotate_pix(const image2d<T>& f, image2d<T>& out)
{
  mln_foreach (auto px, f.pixels())
  {
    using P = typename image2d<T>::point_type;
    P q = {px.point()[1], px.point()[0]};
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
