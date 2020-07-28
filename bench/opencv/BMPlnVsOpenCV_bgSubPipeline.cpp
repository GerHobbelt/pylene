#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <bench/bg_sub.hpp>

#include <fixtures/ImagePath/image_path.hpp>


#include <benchmark/benchmark.h>
#include <opencv2/opencv.hpp>

#include <array>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <fmt/core.h>

bool ENABLE_MEMORY_LOG = false;


using namespace std::literals;


namespace details
{
  void GetMemorySize(unsigned long& size, unsigned long& rss)
  {
    size       = 0;
    rss        = 0;
    FILE* file = fopen("/proc/self/statm", "r");
    if (file == NULL)
    {
      return;
    }

    int count = fscanf(file, "%lu%lu", &size, &rss);
    if (count != 2)
    {
      fclose(file);
      return;
    }
    size *= (unsigned long)getpagesize();
    rss *= (unsigned long)getpagesize();
    fclose(file);
  }

  void log_memory_usage(const std::string& label = std::string())
  {
    struct mallinfo m = mallinfo();
    fmt::print("mallinfo: {}\n", label);
    fmt::print("  non-mmapped space allocated from system = {}\n", m.arena);
    fmt::print("  number of free chunks = {}\n", m.ordblks);
    fmt::print("  number of fastbin blocks = {}\n", m.smblks);
    fmt::print("  number of mmapped regions = {}\n", m.hblks);
    fmt::print("  space in mmapped regions = {}\n", m.hblkhd);
    fmt::print("  maximum total allocated space = {}\n", m.usmblks);
    fmt::print("  space available in freed fastbin blocks = {}\n", m.fsmblks);
    fmt::print("  total allocated space = {}\n", m.uordblks);
    fmt::print("  total free space = {}\n", m.fordblks);
  }
} // namespace details

std::vector<std::pair<std::string, std::string>> filenames = {
    {"a.jpg", "a_bg.jpg"}, //
    {"b.jpg", "b_bg.jpg"}, //
    {"c.jpg", "c_bg.jpg"}, //
    {"d.jpg", "d_bg.jpg"}, //
    {"e.jpg", "e_bg.jpg"}, //
    {"f.jpg", "f_bg.jpg"}, //
    {"g.jpg", "g_bg.jpg"}, //
    {"h.jpg", "h_bg.jpg"}, //
    {"i.jpg", "i_bg.jpg"}, //
    {"j.jpg", "j_bg.jpg"}, //
    {"k.jpg", "k_bg.jpg"}, //
    {"l.jpg", "l_bg.jpg"}, //
    {"m.jpg", "m_bg.jpg"}  //
};


constexpr std::size_t radius = 30;


class BMPlnVsOpenCV_BgSubPipeline : public benchmark::Fixture
{
public:
  using image_t      = mln::image2d<mln::rgb8>;
  using out_image_t  = mln::image2d<uint8_t>;
  using image_cv_t   = cv::Mat;
  using callback_pln = std::function<void(const image_t&, const image_t&, out_image_t&)>;
  using callback_cv  = std::function<void(const image_cv_t& input_img_cv, const image_cv_t& input_bg_cv, image_cv_t& output_cv)>;

  const std::string filepath =
      std::string{fixtures::ImagePath::get_image_path()} + "../apps/tip.2019-2020/images/bg_sub_mosaic";

  BMPlnVsOpenCV_BgSubPipeline()
  {
    if (!g_loaded)
    {
      std::size_t n = filenames.size();

      g_input_imgs.resize(n);
      g_input_bgs.resize(n);
      g_outputs.resize(n);
      g_input_imgs_cv.resize(n);
      g_input_bgs_cv.resize(n);
      g_outputs_cv.resize(n);

      for (std::size_t i = 0; i < n; ++i)
      {
        mln::io::imread(fmt::format("{}/mosaic_{}_{}", filepath, "6x6", filenames[i].first), g_input_imgs[i]);
        mln::io::imread(fmt::format("{}/mosaic_{}_{}", filepath, "6x6", filenames[i].second), g_input_bgs[i]);
        mln::resize(g_outputs[i], g_input_imgs[i]);

        int w = g_input_imgs[i].width();
        int h = g_input_imgs[i].height();

        g_input_imgs_cv[i] = cv::imread(fmt::format("{}/mosaic_{}_{}", filepath, "6x6", filenames[i].first), cv::IMREAD_COLOR);
        g_input_bgs_cv[i]  = cv::imread(fmt::format("{}/mosaic_{}_{}", filepath, "6x6", filenames[i].second), cv::IMREAD_COLOR);
        g_outputs_cv[i].create(w, h, CV_8UC1);

        g_size += w * h;
      }
      g_loaded = true;
    }
  };

  static void run_hook_before()
  {
    if (ENABLE_MEMORY_LOG)
    {
      std::size_t ms, rss;
      details::GetMemorySize(ms, rss);
      details::log_memory_usage();
      fmt::print("Start -- Amount of memory in use: {} RSS={}\n", ms, rss);
    }
  }

  static void run_hook_after()
  {
    if (ENABLE_MEMORY_LOG)
    {
      std::size_t ms, rss;
      details::GetMemorySize(ms, rss);
      fmt::print("End -- Amount of memory in use: {} RSS={}\n", ms, rss);
      details::log_memory_usage();
    }
  }

  void run(benchmark::State& st, callback_pln callback)
  {
    int img_id = st.range(0);
    run_hook_before();
    for (auto _ : st)
      callback(g_input_imgs[img_id], g_input_bgs[img_id], g_outputs[img_id]);


    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(g_size));
    run_hook_after();
  }

  void run(benchmark::State& st, callback_cv callback)
  {
    int img_id = st.range(0);

    run_hook_before();
    for (auto _ : st)
      callback(g_input_imgs_cv[img_id], g_input_bgs_cv[img_id], g_outputs_cv[img_id]);

    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(g_size));
    run_hook_after();
  }




protected:
  static bool                     g_loaded;
  static std::vector<image_t>     g_input_imgs;
  static std::vector<image_t>     g_input_bgs;
  static std::vector<out_image_t> g_outputs;
  static std::vector<image_cv_t>  g_input_imgs_cv;
  static std::vector<image_cv_t>  g_input_bgs_cv;
  static std::vector<image_cv_t>  g_outputs_cv;
  static std::size_t              g_size;
};

bool                                                  BMPlnVsOpenCV_BgSubPipeline::g_loaded = false;
std::size_t                                           BMPlnVsOpenCV_BgSubPipeline::g_size   = 0;
std::vector<BMPlnVsOpenCV_BgSubPipeline::image_t>     BMPlnVsOpenCV_BgSubPipeline::g_input_imgs;
std::vector<BMPlnVsOpenCV_BgSubPipeline::image_t>     BMPlnVsOpenCV_BgSubPipeline::g_input_bgs;
std::vector<BMPlnVsOpenCV_BgSubPipeline::out_image_t> BMPlnVsOpenCV_BgSubPipeline::g_outputs;
std::vector<BMPlnVsOpenCV_BgSubPipeline::image_cv_t>  BMPlnVsOpenCV_BgSubPipeline::g_input_imgs_cv;
std::vector<BMPlnVsOpenCV_BgSubPipeline::image_cv_t>  BMPlnVsOpenCV_BgSubPipeline::g_input_bgs_cv;
std::vector<BMPlnVsOpenCV_BgSubPipeline::image_cv_t>  BMPlnVsOpenCV_BgSubPipeline::g_outputs_cv;

// PLN

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeViewsDisc)(benchmark::State& st)
{
  const int radius = st.range(1);
  auto      se     = mln::se::disc(radius);

  callback_pln f = [se](const image_t& in1, const image_t& in2, out_image_t& out) { pln_bg_sub_pipe_views(in1, in2, out, se); };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeViewsRect)(benchmark::State& st)
{
  const int radius = st.range(1);
  auto      edge   = 2 * radius + 1;
  auto      se     = mln::se::rect2d(edge, edge);

  auto f = [se](const image_t& in1, const image_t& in2, out_image_t& out) { pln_bg_sub_pipe_views(in1, in2, out, se); };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeAlgosDisc)(benchmark::State& st)
{
  const int radius = st.range(1);
  auto      se     = mln::se::disc(radius);
  auto f = [se](const image_t& in1, const image_t& in2, out_image_t& out) { pln_bg_sub_pipe_algos(in1, in2, out, se); };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeAlgosRect)(benchmark::State& st)
{
  const int radius = st.range(1);
  auto      edge   = 2 * radius + 1;
  auto      se     = mln::se::rect2d(edge, edge);

  auto f = [se](const image_t& in1, const image_t& in2, out_image_t& out) { pln_bg_sub_pipe_algos(in1, in2, out, se); };
  this->run(st, f);
}

// CV

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, CV_PipeAlgoDisc)(benchmark::State& st)
{
  const int radius = st.range(1);
  auto se = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * radius + 1, 2 * radius + 1));
  auto f = [se](const image_cv_t& in1, const image_cv_t& in2, image_cv_t& out) { cv_bg_sub_pipe(in1, in2, out, se); };

  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, CV_PipeAlgoRect)(benchmark::State& st)
{
  const int radius = st.range(1);

  auto se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * radius + 1, 2 * radius + 1));
  auto f  = [se](const image_cv_t& in1, const image_cv_t& in2, image_cv_t& out) { cv_bg_sub_pipe(in1, in2, out, se); };
  this->run(st, f);
}

namespace
{
  void CustomArguments(benchmark::internal::Benchmark* b) {
    for (int img_id = 0; img_id < (int)(filenames.size()); ++img_id)
    {
      for (int i = 1; i < 7; ++i)
      {
        int r = 1 << i;

        b->Args({img_id, r});

        if (i >= 3)
          b->Args({img_id, r + (1 << (i - 2))});

                 b->Args({img_id, r + (1 << (i - 1))});

        if (i >= 3)
          b->Args({img_id, r + (1 << (i - 1)) + (1 << (i - 2))});
      }
    }
  }
}


BENCHMARK_REGISTER_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeViewsDisc)
    // ->RangeMultiplier(kBMRangeMultiplier)
    // ->Range(kBMRangeLower, kBMRangeUpper)
    ->Apply(CustomArguments)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeAlgosDisc)
    // ->RangeMultiplier(kBMRangeMultiplier)
    // ->Range(kBMRangeLower, kBMRangeUpper)
    ->Apply(CustomArguments)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(BMPlnVsOpenCV_BgSubPipeline, CV_PipeAlgoDisc)
    // ->RangeMultiplier(kBMRangeMultiplier)
    // ->Range(kBMRangeLower, kBMRangeUpper)
    ->Apply(CustomArguments)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeViewsRect)
    // ->RangeMultiplier(kBMRangeMultiplier)
    // ->Range(kBMRangeLower, kBMRangeUpper)
    ->Apply(CustomArguments)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeAlgosRect)
    // ->RangeMultiplier(kBMRangeMultiplier)
    // ->Range(kBMRangeLower, kBMRangeUpper)
    ->Apply(CustomArguments)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(BMPlnVsOpenCV_BgSubPipeline, CV_PipeAlgoRect)
    // ->RangeMultiplier(kBMRangeMultiplier)
    // ->Range(kBMRangeLower, kBMRangeUpper)
    ->Apply(CustomArguments)
    ->Unit(benchmark::kMillisecond);

/*BENCHMARK_MAIN();*/


class TestMemoryManager : public benchmark::MemoryManager
{
  void Start()
  {
    // std::cerr << "Start" << std::endl;
    // malloc_stats();
    // details::log_memory_usage();
    // std::cerr << "------" << std::endl;
  }
  void Stop(Result* result)
  {
    result->num_allocs     = 0;
    result->max_bytes_used = 0;
    // std::cerr << "Stop" << std::endl;
    // malloc_stats();
    // details::log_memory_usage();
    // std::cerr << "------" << std::endl;
  }
};

int main(int argc, char** argv)
{
  // std::cerr << "Before loading" << std::endl;
  // details::log_memory_usage();

  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  std::unique_ptr<benchmark::MemoryManager> mm(new TestMemoryManager());
  benchmark::RegisterMemoryManager(mm.get());
  ::benchmark::RunSpecifiedBenchmarks();
  benchmark::RegisterMemoryManager(nullptr);
}
