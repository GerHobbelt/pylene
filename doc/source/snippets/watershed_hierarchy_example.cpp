#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/watershed_hierarchy.hpp>

#include <iostream>

template <typename V>
void process_example(const mln::image2d<V>& img, const std::string& output_filename, const double threshold)
{
  // 2. Build the watershed hierarchy
  auto area_attribute_func = [](auto tree, auto nm) -> std::vector<unsigned long> {
    return tree.compute_attribute_on_points(nm, mln::accu::features::count<>());
  };
  auto [t, nm] = mln::morpho::watershed_hierarchy(img, area_attribute_func, mln::c4);

  // 3. Compute the mean attribute
  auto mean = t.compute_attribute_on_values(nm, img, mln::accu::accumulators::mean<V>());

  // 4. Compute a cut of the watershed hierarchy
  auto cut_nm = t.horizontal_cut(threshold, nm);

  // 5. Label the cut
  auto cut = t.reconstruct_from(cut_nm, ::ranges::make_span(mean));

  // 5. Save the output
  mln::io::imsave(mln::view::cast<V>(cut), output_filename);
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Invalid number of argument\nUsage: " << argv[0] << " input_filename output_filename\n";
    return 1;
  }
  auto in_filename  = std::string(argv[1]);
  auto out_filename = std::string(argv[2]);

  // 1. Read the input image
  mln::ndbuffer_image in;
  in = mln::io::imread(in_filename);

  if (in.sample_type() == mln::sample_type_id::UINT8)
  {
    const auto* img = in.cast_to<std::uint8_t, 2>();
    process_example(*img, out_filename, 25);
  }
  else if (in.sample_type() == mln::sample_type_id::RGB8)
  {
    const auto* img = in.cast_to<mln::rgb8, 2>();
    process_example(*img, out_filename, 100);
  }
  else
  {
    std::cerr << "Unhandled sample type format\n";
    return 1;
  }

  return 0;
}