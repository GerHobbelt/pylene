#include <mln/accu/accumulators/sum.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/morphers/casted_image.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>


// FIXME:
namespace to_migrate
{
  template <typename I, class AccuLike, class Extractor = mln::accu::default_extractor>
  [[deprecated]] typename mln::accu::result_of<AccuLike, mln_value(I), Extractor>::type
      __accumulate(const mln::Image<I>& input, const mln::AccumulatorLike<AccuLike>& accu_,
                   const Extractor& ex = Extractor())
  {
    const I& ima = exact(input);
    auto     a   = mln::accu::make_accumulator(exact(accu_), mln_value(I)());

    mln_foreach (const auto& v, ima.values())
      a.take(v);

    return ex(a);
  }
} // namespace to_migrate

int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " input1 input2\n";
    std::exit(1);
  }


  image2d<rgb8> f, g;
  io::imread(argv[1], f);
  io::imread(argv[2], g);

  auto f_   = mln::imcast<rgb<float>>(f);
  auto g_   = mln::imcast<rgb<float>>(g);
  auto diff = imtransform(f_ - g_, [](rgb<float> x) -> double { return l2norm_sqr(x); });

  auto dims = f.domain().shape();
  // FIXME:
  double sum = ::to_migrate::__accumulate(diff, accu::features::sum<double>());
  double MSE = sum / (3 * dims[0] * dims[1]);
  std::cout << "MSE: " << MSE << "\n";

  if (MSE == 0)
    std::cout << "PSNR: " << -1 << "\n";
  else
  {
    float psnr = 20 * std::log10(255) - 10 * std::log10(MSE);
    std::cout << "PSNR: " << psnr << "\n";
  }
}
