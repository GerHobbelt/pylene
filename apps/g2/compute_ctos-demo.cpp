#include "compute_ctos.hpp"

#include <apps/tos/croutines.hpp>

#include <mln/core/colors.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/data/stretch.hpp>
#include <mln/morpho/component_tree/compute_depth.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>

#include <boost/program_options.hpp>


#ifndef MLN_INPUT_VALUE_TYPE
#define MLN_INPUT_VALUE_TYPE rgb8
#endif

int main(int argc, char** argv)
{
  using namespace mln;
  namespace po = boost::program_options;

  const char* usage = "Compute the cToS of an image. It ouputs the depth image (16bits) "
                      "and a normalized 8bits depth image. The tree is computed on "
                      "K1 (doubled-sized image + border) and so is the depth image\n"
                      "To get the MTOS in your code, you just need to compute the maxtree "
                      " of the depth image (16bits).\n"
                      "If a grain is given, a grain filter is applied.\n";


  po::options_description hidden("Allowed options");
  hidden.add_options()("input_path", po::value<std::string>()->required(), "Input file (rgb8)")(
      "depth_path", po::value<std::string>()->required(),
      "Output depth map (uint16)")("depth_path_8", po::value<std::string>()->required(), "Output depth map (uint8)");

  po::options_description visible("Allowed options");
  visible.add_options()("help", "Help message")("grain,g", po::value<int>(), "Grain filter")(
      "attr,a", po::value<std::string>()->default_value("depth"),
      "The type of attribute used to compute the inclusion map:\n"
      "depth: Length of the path to A\n"
      "count: Number of nodes including A\n"
      "pwcount: Number of nodes including x\n")("export-mdepth", po::value<std::string>(),
                                                "Export marginal depth to stem-{0,1...}.tiff");

  po::positional_options_description pd;
  pd.add("input_path", 1).add("depth_path", 1).add("depth_path_8", 1);

  po::options_description all("Allowed options");
  all.add(hidden).add(visible);

  po::variables_map vm;
  bool              err = false;
  try
  {
    po::store(po::command_line_parser(argc, argv).options(all).positional(pd).run(), vm);

    po::notify(vm);
    if (vm.count("help"))
      err = true;
  }
  catch (...)
  {
    err = true;
  }

  e_ctos_attribute attr;
  std::string      str_attr = vm["attr"].as<std::string>();
  if (str_attr == "depth")
    attr = CTOS_DEPTH;
  else if (str_attr == "count")
    attr = CTOS_COUNT;
  else if (str_attr == "pwcount")
    attr = CTOS_PW_COUNT;
  else
    err = true;

  if (err)
  {
    std::cerr << "Usage: " << argv[0] << " [options] input.tiff depth16.tiff depth8.png\n" << usage << visible;
    std::exit(1);
  }


  /***********************************/
  /*** REAL STUFF STARTS HERE      ***/
  /***********************************/
  typedef MLN_INPUT_VALUE_TYPE V;

  image2d<V> f;
  io::imread(vm["input_path"].as<std::string>(), f);

  ctos_extra_params_t params;
  if (vm.count("export-mdepth"))
  {
    params.export_marginal_depth      = true;
    params.export_marginal_depth_path = vm["export-mdepth"].as<std::string>();
  }

  auto tree = compute_ctos(f, NULL, attr, params);
  if (vm.count("grain"))
    grain_filter_inplace(tree, vm["grain"].as<int>());

  // morpho::save(tree, vm["tree_path"].as<std::string>());

  {
    image2d<uint16> depth;
    depth.resize(tree._get_data()->m_pmap.domain());
    auto dmap = morpho::compute_depth(tree);
    morpho::reconstruction(tree, dmap, depth);
    io::imsave(depth, vm["depth_path"].as<std::string>());


    image2d<uint8> depth8 = data::stretch<uint8>(depth);
    io::imsave(depth8, vm["depth_path_8"].as<std::string>());
  }
}
