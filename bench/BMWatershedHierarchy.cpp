#include <benchmark/benchmark.h>
#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/watershed_hierarchy.hpp>

using image_t = mln::image2d<mln::rgb8>;

static const std::vector<std::string> bench_images = {"Aerial_view_of_Olbia.jpg", "Space1_20MB.jpg"};

static const auto watershed_hierarchy_by_area_function = [](const image_t& input) {
  mln::morpho::watershed_hierarchy(
      input,
      [](auto tree, auto nm) -> std::vector<size_t> {
        return tree.compute_attribute_on_points(nm, mln::accu::features::count<>());
      },
      mln::c4, [](const auto& a, const auto& b) -> std::float_t { return mln::l2dist(a, b); });
};

static const auto hierarchical_segmentation_function = [](const image_t& input) {
  auto [tree, node_map] = mln::morpho::watershed_hierarchy(
      input,
      [](auto tree, auto nm) -> std::vector<size_t> {
        return tree.compute_attribute_on_points(nm, mln::accu::features::count<>());
      },
      mln::c4, [](const auto& a, const auto& b) -> std::float_t { return mln::l2dist(a, b); });

  auto mean = tree.compute_attribute_on_values(node_map, input, mln::accu::accumulators::mean<mln::rgb8>());

  // Threshold cut at 10
  auto node_map_cut = tree.horizontal_cut(10, node_map);
  tree.reconstruct_from(node_map_cut, ranges::make_span(mean));
};

class BMWatershedHierarchy : public benchmark::Fixture
{
public:
  BMWatershedHierarchy()
  {
    if (inputs.empty())
    {
      inputs = std::vector<image_t>(bench_images.size());
      for (std::size_t i = 0; i < bench_images.size(); ++i)
        mln::io::imread(bench_images[i], inputs[i]);
    }
  }

  void run(benchmark::State& st, const std::function<void(const image_t& input)>& callback, std::size_t i) const
  {
    for (auto _ : st)
      callback(inputs[i]);
  }

  static std::vector<image_t> inputs;
};
std::vector<image_t> BMWatershedHierarchy::inputs;

BENCHMARK_F(BMWatershedHierarchy, WatershedHierarchyAreaOlbia)(benchmark::State& st)
{
  this->run(st, watershed_hierarchy_by_area_function, 0);
}

BENCHMARK_F(BMWatershedHierarchy, HierarchicalSegmentationOlbia)(benchmark::State& st)
{
  this->run(st, hierarchical_segmentation_function, 0);
}

BENCHMARK_F(BMWatershedHierarchy, WatershedHierarchyAreaSpace)(benchmark::State& st)
{
  this->run(st, watershed_hierarchy_by_area_function, 1);
}

BENCHMARK_F(BMWatershedHierarchy, HierarchicalSegmentationSpace)(benchmark::State& st)
{
  this->run(st, hierarchical_segmentation_function, 1);
}


BENCHMARK_MAIN();