#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>

#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighb2d.hpp>

#include <mln/io/experimental/imread.hpp>

#include <mln/morpho/experimental/tos.hpp>
#include <mln/morpho/tos/tos.hpp>


#include <benchmark/benchmark.h>

#include <fixtures/ImagePath/image_path.hpp>

class BMMorpho : public benchmark::Fixture
{
public:
  using image_t = mln::experimental::image2d<uint8_t>;
  using image_ref_t = mln::image2d<uint8_t>;

  BMMorpho()
  {
    if (!g_loaded)
    {
      const char* filename = "Aerial_view_of_Olbia.jpg";
      mln::experimental::image2d<mln::rgb8> input;
      mln::io::experimental::imread(filename, input);

      g_input = mln::transform(input, [](mln::rgb8 x) -> uint8_t { return x[0]; });
      g_loaded = true;
    }
    m_input = g_input;
    int nr = m_input.width();
    int nc = m_input.height();
    mln::resize(m_output, m_input);
    m_size = nr * nc;

    m_input.to(m_input_, false);
    m_output.to(m_output_, false);

  }


  void run(benchmark::State& st, std::function<void(const image_t& input)> callback)
  {
    for (auto _ : st)
      callback(m_input);
    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(m_size));
  }

  void run2(benchmark::State& st, std::function<void(const image_ref_t& input)> callback)
  {
    for (auto _ : st)
      callback(m_input_);
    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(m_size));
  }


protected:
  static bool                                g_loaded;
  static mln::experimental::image2d<uint8_t> g_input;
  mln::experimental::image2d<uint8_t>        m_input;
  mln::experimental::image2d<uint8_t>        m_output;
  std::size_t                                m_size;

  mln::image2d<uint8_t>               m_input_;
  mln::image2d<uint8_t>               m_output_;
};

bool                                BMMorpho::g_loaded = false;
mln::experimental::image2d<uint8_t> BMMorpho::g_input;



BENCHMARK_F(BMMorpho, ToSNew)(benchmark::State& st)
{
  auto f = [](const image_t& input) { mln::morpho::experimental::tos(input, input.domain().tl()); };
  this->run(st, f);
}

BENCHMARK_F(BMMorpho, ToSRef)(benchmark::State& st)
{
  auto f = [](const image_ref_t& input) { mln::morpho::tos(input); };
  this->run2(st, f);
}

BENCHMARK_MAIN();
