#ifndef BENCH_MAXTREE_ALGORITHM_HPP
# define BENCH_MAXTREE_ALGORITHM_HPP

# define MLN_MAXBIT 32

# include <tbb/task_scheduler_init.h>
# include <tbb/tick_count.h>
# include <iostream>
# include <random>

# include <mln/core/image/image2d.hpp>
# include <mln/core/neighb2d.hpp>
# include <mln/core/algorithm/transform.hpp>
# include <mln/core/value/int.hpp>
# include <mln/io/imread.hpp>

#include <mln/morpho/maxtree_ufind_parallel.hpp>
#include <mln/morpho/maxtree_hqueue_parallel.hpp>
#include <mln/morpho/maxtree_pqueue_parallel.hpp>
#include <mln/morpho/maxtree_ufindrank_parallel.hpp>
#include <boost/program_options.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <mln/io/imprint.hpp>

# define NTEST 3

# define MAXTREE_ALGO(Name, Fun)				\
  struct Name							\
  {								\
    template <typename I, typename N, typename S>		\
    auto operator() (const I& x, const N& y, const S& z)	\
      -> decltype(Fun(x,y,z))					\
    {								\
      return Fun(x,y,z);					\
    }								\
  };


MAXTREE_ALGO(meta_serial_union_find, mln::morpho::impl::serial::maxtree_ufind)
MAXTREE_ALGO(meta_parallel_union_find, mln::morpho::impl::parallel::maxtree_ufind)
MAXTREE_ALGO(meta_parallel_union_find_line, mln::morpho::impl::parallel::maxtree_ufind_line)
MAXTREE_ALGO(meta_serial_hqueue, mln::morpho::impl::serial::maxtree_hqueue)
MAXTREE_ALGO(meta_parallel_hqueue, mln::morpho::impl::parallel::maxtree_hqueue)
MAXTREE_ALGO(meta_serial_pqueue, mln::morpho::impl::serial::maxtree_pqueue)
MAXTREE_ALGO(meta_parallel_pqueue, mln::morpho::impl::parallel::maxtree_pqueue)
MAXTREE_ALGO(meta_serial_union_find_rank, mln::morpho::impl::serial::maxtree_ufindrank)
MAXTREE_ALGO(meta_parallel_union_find_rank, mln::morpho::impl::parallel::maxtree_ufindrank)


template <typename V, class Algorithm>
void
bench_algo(const mln::image2d<V>&  ima,
	   unsigned		nthread,
	   Algorithm		algo,
	   int			ntest)
{
  using namespace tbb;
  task_scheduler_init ts(nthread);

  mln::image2d<std::size_t> parent;
  auto t0 = tick_count::now();
  for (int i = 0; i < ntest; ++i)
    parent = algo (ima, mln::c4, std::less<V> ());
  auto t1 = tick_count::now();
  std::cout << "Run in:\t" << (t1-t0).seconds() / ntest << std::endl;
}


template <typename V>
mln::image2d<V>
addnoise(const mln::image2d<mln::uint8>& ima)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  int bshft = mln::value_traits<V>::quant - 8;
  int sigma = (1 << (bshft-1));
  std::uniform_int_distribution<int> sampler(-sigma, sigma);

  mln::image2d<V> out = mln::transform(ima, [&sampler, &gen, bshft] (mln::uint8 v) {
      auto v_ = (((std::size_t)v << bshft) + sampler(gen));
      return (V) std::max<long>(mln::value_traits<V>::min(), std::min<long>(mln::value_traits<V>::max(), v_));
    });

  // mln::io::imprint(ima);
  // mln::io::imprint(out);

  return out;
}

template <typename Algorithm>
void run_test(int argc, char** argv, Algorithm algo)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("nthread", po::value<int>()->default_value((int) tbb::task_scheduler_init::automatic), "set number of thread (default: auto)")
    ("nbits", po::value<int>()->default_value(8), "set number of bits (default: 8)")
    ("ntest", po::value<int>()->default_value(3), "number of runs (default: 3)")
    ("input-file", po::value<std::string>()->required(), "input")
    ;
  po::positional_options_description p;
  p.add("input-file", -1);


  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
	    options(desc).positional(p).run(), vm);
  try {
    po::notify(vm);

    if (vm.count("help")) {
      std::cout << desc << "\n";
      std::exit(1);
    }
  } catch (...) {
    std::cout << desc << "\n";
    std::exit(1);
  }


  mln::image2d<mln::uint8> ima;
  mln::io::imread(vm["input-file"].as<std::string> (), ima);

# define DECL(z, n, text)						\
  case n:								\
    bench_algo(addnoise< mln::UInt<n> >(ima), vm["nthread"].as<int>(), algo, vm["ntest"].as<int>()); \
    break;

  int nbits = vm["nbits"].as<int>();
  switch (nbits)
    {
      BOOST_PP_REPEAT_FROM_TO(9,BOOST_PP_ADD(MLN_MAXBIT, 1), DECL, "")

      default:
	bench_algo(ima, vm["nthread"].as<int>(), algo, vm["ntest"].as<int>());
    }

}



#endif // ! BENCH_MAXTREE_ALGORITHM_HPP
