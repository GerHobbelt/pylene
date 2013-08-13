#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/colors.hpp>

#include <mln/core/algorithm/accumulate.hpp>
#include <mln/accu/accumulators/infsup.hpp>

#include <mln/morpho/tos/immerse.hpp>
#include <mln/io/imread.hpp>

#include <mln/core/internal/nested_loop_iterator.hpp>

#include <apps/tos/addborder.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "relation.hpp"
#include "shape.hpp"
#include "cuts.hpp"
#include "make_graph.hpp"
#include "export.hpp"

 using namespace boost::numeric;

struct params_t
{
  bool addborder = true;
  bool immerse = false;
  int subquant = 0;
  bool display = false;
  bool stat = false;
  int export_format = 0; //FMT_IMAGE;
  std::string export_filename;
  std::string export_dir;
};


template <class Vec, class Compare>
struct vset;

template <class Vec>
struct vset< Vec, mln::productorder_less<Vec> >
{
  typedef mln::__nested_loop_iterator
  <Vec, mln::__iterate_from_to<Vec, false> > iterator;

  typedef iterator const_iterator;

  vset(const Vec& from, const Vec& to)
    : m_from (from), m_to (to)
  {
  }

  iterator iter() const
  {
    return iterator(Vec(),
		    mln::__iterate_from_to<Vec,false> (m_from, m_to));
  }


  unsigned size() const
  {
    return (m_to - m_from + 1).sum();
  }

private:
  Vec m_from, m_to;
};


void showstat(const ublas::triangular_matrix<bool, ublas::upper>&  mat)
{
  int n = mat.size1();

  std::vector<bool> isleaf(n, true);
  std::vector<int> profmax(n, 0);
  std::vector<int> profmin(n, 0);
  int nleaves = n;

  const auto M = trans(mat); // mat = included   M = includes

  for (int i = n-1; i >= 0; --i)
    for (int j = 0; j < i; ++j)
      if (M(i,j))
	{
	  if (isleaf[i]) {
	    isleaf[i] = false;
	    --nleaves;
	  }
	  if (profmin[j] == 0)
	    profmin[j] = profmin[i] + 1;
	  if (profmax[j] <= profmax[i])
	    profmax[j] = profmax[i] + 1;
	}

  int npar[6] = {0,};
  int nparent = 0;
  for (int i = 0; i < n; ++i)
    {
      int s = 0;
      for (int j = i+1; j < n; ++j)
	s += mat(i,j);
      int k = std::min(5, s);
      npar[k]++;
      nparent++;
    }

  std::cout << "Nombre de shapes: " << n << std::endl
	    << "Nombre de feuilles: " << nleaves << std::endl
	    << "Nombre de parents (moy): " << ((float) nparent / n) << std::endl;
  for (int i = 0; i < 6; ++i)
    std::cout << "  Nombre de noeuds avec " << i << " parent(s): " << npar[i] << std::endl;


  float n_ = n;
  auto sqr = [] (float x) { return x*x; };

  {
    unsigned s = 0, s_2 = 0;
    for (int v: profmin) {
      s += v;
      s_2 += v*v;
    }
    std::cout << "Profondeur min moyenne: " << (s / n_)
	      << " - std: " << std::sqrt(s_2 / n_ - sqr(s / n_)) << std::endl;
  }
  {
    unsigned s = 0, s_2 = 0;
    for (int v: profmax) {
      s += v;
      s_2 += v*v;
    }
    std::cout << "Profondeur max moyenne: " << (s / n_)
	      << " - std: " << std::sqrt(s_2 / n_ - sqr(s / n_)) << std::endl;
  }

}



template <class LowerCompare, class UpperCompare>
void compute(const mln::image2d<mln::rgb8>& ima,
	     const params_t& params = params_t(),
	     LowerCompare cmp_1 = LowerCompare(),
	     UpperCompare cmp_2 = UpperCompare ())
{
  using namespace mln;
  typedef rgb8 Vec;
  image2d<rgb8> f;

  // 1.Subquantize
  if (params.subquant) {
    resize(f, ima);
    copy(ima / params.subquant, f);
  } else {
    f = ima;
  }

  // 2. Add border
  if (params.addborder)
    f = addborder(f, lexicographicalorder_less<rgb8> ());

  // 3. Interpolate


  // 4. Immerse
  image2d< morpho::tos::irange<Vec> > F;
  box2d dom = f.domain();
  if (params.immerse) {
    F = morpho::tos::internal::immerse(f, rng_porder_less<Vec> ());
    dom = F.domain();
  }

  // 5. Compute Inf/sup
  rgb8 vmin, vmax;
  typedef productorder_less<rgb8> Compare;
  std::tie(vmin, vmax) = accumulate(f, accu::accumulators::infsup<rgb8, Compare > ());


  if (params.display)
    {
      std::cout << "===   Original image (inf=";
      format(std::cout, vmin) << "  sup=";
      format(std::cout, vmax) << ")  ===" << std::endl;
      if (params.immerse)
	io::imprint(F);
      else
	io::imprint(f);
    }

  // 6. Compute shapes
  typedef shape< Vec, LowerCompare, UpperCompare > shape_t;
  shape_set<shape_t> shapes;

  int i = 0;
  auto diff = (vmax-vmin+1);
  int sz = diff[0] * diff[1] * diff[2];
  mln_foreach(const rgb8& v, (vset<rgb8, Compare>(vmin, vmax)) ){
    std::cerr << "Avancement: " << (i++ * 100.0 / sz) << "% \r";
    std::cerr.flush();
    if (params.immerse) {
      cut_and_get_shapes(F, c8, c4, v, cmp_1, shapes);
      if (! std::is_same<LowerCompare, UpperCompare>::value)
	cut_and_get_shapes(F, c8, c4, v, cmp_2, shapes);
    } else  {
      cut_and_get_shapes(f, c8, c4, v, cmp_1, shapes);
      if (! std::is_same<LowerCompare, UpperCompare>::value)
	cut_and_get_shapes(f, c8, c4, v, cmp_2, shapes);
    }
  }
  //

  // Copy shapes in a vector
  std::vector<shape_t> vshapes;
  vshapes.reserve(shapes.size());
  {
    auto it1 = shapes.begin();
    auto itend = shapes.end();
    auto it2 = std::back_inserter(vshapes);
    for (; it1 != itend; ++it1, ++it2) {
      *it2 = std::move<shape_t>( const_cast<shape_t&&>(*it1) );
    }
  }

  // Display shapes
  if (params.display)
    {
      std::sort(vshapes.begin(), vshapes.end(),
		[](const shape_t& x, const shape_t& y) {return x.size() < y.size(); });

      for (unsigned i = 0; i < vshapes.size(); ++i) {
	std::cout << "=============================================" << std::endl;
	prettyprint_shape(vshapes[i], dom);
      }
    }

  // 7. Calcul des inclusions
  ublas::triangular_matrix<bool, ublas::upper> clos, mat;
  std::tie(clos, mat) = graph_transitive_reduction(vshapes);

  if (params.display)
    {
      std::cout << "=============== Inclusions ====================" << std::endl;
      std::hash<shape_t> h;
      int sz = vshapes.size();
      for (int i = 0; i < sz; ++i)
	for (int j = i+1; j < sz; ++j)
	  if (mat(i,j))
	    std::cout << std::hex << "0x" << h(vshapes[i]) << " C  0x" << h(vshapes[j]) << std::endl;
      std::cout << std::dec;
    }

  // 8. Exportation
  if (params.export_format)
    {
      if (params.immerse)
	save_graph(mat, vshapes, F, params.export_filename, params.export_dir);
      else
	save_graph(mat, vshapes, f, params.export_filename, params.export_dir);

      std::string dotcmd = "dot -Tsvg ";
      dotcmd += params.export_filename;
      dotcmd += " -o ";
      dotcmd += boost::filesystem::path(params.export_filename).replace_extension(".svg").c_str();
      std::system(dotcmd.c_str());
    }

  // 9. stat
  if (params.stat)
    showstat(mat);
}



int main(int argc, char** argv)
{
  using namespace mln;
  namespace po = boost::program_options;
  namespace fs = boost::filesystem;

  params_t params;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("noborder", po::bool_switch(), "add a border to the image")
    ("immerse", po::bool_switch(&params.immerse), "immerse in range value")
    ("subquant", po::value<int>(&params.subquant)->default_value(0), "subquantization factor")
    ("display", po::bool_switch(&params.display), "Display shape at the end")
    ("stat", po::bool_switch(&params.stat), "Display stat at the end")
    ("export", po::bool_switch(), "Export the lattice as a dot graph.")
    ("export-filename", po::value<std::string>(&params.export_filename)->default_value("output.dot"),
     "Dot filename (if it exists, override)")
    ("export-dir", po::value<std::string>(&params.export_dir), "Directory to store shape images (if it exists, override).")
    ("input", po::value<std::string>()->required(), "Input image (color)")
    ("compare", po::value<std::string>()->required(), "Comparison function for cuts (either '==', '<=' or '<')")
    ;

  po::positional_options_description pd;
  pd.add("input",1).add("compare", 1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(desc).positional(pd).run(), vm);
    po::notify(vm);
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << desc;
    return 1;
  }

  if (vm.count("help")) {
    std::cerr << desc;
    std::terminate();
  }

  params.addborder = not vm["noborder"].as<bool>();

  if (vm["export"].as<bool>()) {
    params.export_format = FMT_IMAGE;
    if (!vm.count("export-dir")) {
      fs::path p = vm["export-filename"].as<std::string>();
      params.export_dir = (p.parent_path() /= p.stem()).native() + std::string("-shapes/");
    }

    if (!fs::exists(params.export_dir))
      fs::create_directories(params.export_dir);
  }

  typedef rgb8 Vec;
  typedef morpho::tos::irange<Vec> R;
  image2d<rgb8> ima;
  image2d<R> f;

  std::string filename = vm["input"].as<std::string>();
  io::imread(filename.c_str(), ima);


  std::string cmp = vm["compare"].as<std::string>();
  if (cmp == "==")
    compute(ima, params, rng_porder_equal_to<Vec>(), rng_porder_equal_to<Vec>() );
  else if (cmp == "<")
    compute(ima, params, rng_porder_less<Vec>(), rng_porder_greater<Vec>() );
  else if (cmp == "<=")
    compute(ima, params, rng_porder_less_equal<Vec>(), rng_porder_greater_equal<Vec>() );
  else {
    std::cerr << "Invalid comparison operator: " << cmp << std::endl;
    std::cerr << desc;
    return 1;
  }
}
