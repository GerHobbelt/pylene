#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/cut.hpp>

#include <iostream>

template <typename V>
void process_example(const mln::image2d<V>& img, const std::string& cut_filename, double threshold)
{
  // 2. Build the alphatree
  auto [t, nm] = mln::morpho::alphatree(img, mln::c4);

  using th_value_type = decltype(t.values)::value_type;

  // 3. Compute the mean attribute
  auto mean = t.compute_attribute_on_values(nm, img, mln::accu::accumulators::mean<V>());

  // 4. Compute a cut of the alphatree
  auto cut = mln::morpho::horizontal_cut_labelization_from(t, nm, th_value_type(threshold), mean);

  // 5. Save the output cut
  mln::io::imsave(mln::view::cast<V>(cut), cut_filename);
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Invalid number of argument\nUsage: " << argv[0] << " input_filename cut_filename\n";
    return 1;
  }
  auto in_filename  = std::string(argv[1]);
  auto cut_filename = std::string(argv[2]);

  // 1. Read the input image
  mln::ndbuffer_image in;
  in = mln::io::imread(in_filename);

  if (in.sample_type() == mln::sample_type_id::UINT8)
  {
    const auto* img = in.cast_to<std::uint8_t, 2>();
    process_example(*img, cut_filename, 3);
  }
  else if (in.sample_type() == mln::sample_type_id::RGB8)
  {
    const auto* img = in.cast_to<mln::rgb8, 2>();
    process_example(*img, cut_filename, 10);
  }
  else
  {
    std::cerr << "Unhandled sample type format\n";
    return 1;
  }

  return 0;
}