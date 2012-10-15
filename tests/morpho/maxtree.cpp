#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/range/algorithm/generate.hpp>
#include <mln/morpho/maxtree_ufind.hpp>
#include <mln/morpho/maxtree_ufindrank.hpp>
#include <mln/morpho/maxtree_hqueue.hpp>
#include <mln/morpho/maxtree_ufind_parallel.hpp>
#include <mln/core/grays.hpp>
#include <mln/io/imprint.hpp>
#include <tbb/task_scheduler_init.h>

#define BOOST_TEST_MODULE Morpho
#include <boost/test/unit_test.hpp>
#include <random>

namespace mln
{
  void unify_parent(const mln::image2d<uint8>& f,
                    const std::vector<std::size_t>& S,
                    mln::image2d<std::size_t>& parent)

  {
    mln::image2d< std::set<std::size_t> > ima;
    resize(ima, parent);

    mln_foreach(auto& px, parent.pixels())
      {
        std::size_t p = px.index();
        std::size_t q = parent[p];
        if (f[p] == f[parent[p]]) // p and q belong to the same node.
          if (p < parent[p])
            {
              parent[p] = parent[q];
              parent[q] = p;
            }
      }

    // Recanonize
    for (unsigned i = 0; i < S.size(); ++i)
      {
        std::size_t p = S[i];
        std::size_t q = parent[p];
        if (f[q] == f[parent[q]])
          parent[p] = parent[q];
      }
  }

}


BOOST_AUTO_TEST_CASE(Maxtree)
{
  using namespace mln;

  image2d<uint8> ima(96, 64);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8> sampler(0, 20);
  range::generate(ima.values(), [&sampler, &gen] () { return sampler(gen); }) ;

  tbb::task_scheduler_init ts(1);

  // {
  //   image2d<std::size_t> f;
  //   resize(f, ima);
  //   mln_foreach(auto px, f.pixels())
  //     px.val() = px.index();
  //   io::imprint(f);
  // }

  //io::imprint(ima);

  {
    image2d<std::size_t> parent1, parent2, parent3;
    image2d<point2d> parent4;
    std::vector<std::size_t> S1, S2, S3;
    std::tie(parent1, S1) = morpho::maxtree(ima, c4, std::less<uint8> ());
    std::tie(parent2, S2) = morpho::maxtree_hqueue(ima, c4, std::less<uint8> () );
    std::tie(parent3, S3) = morpho::maxtree_ufindbyrank(ima, c4, std::less<uint8> () );
    parent4 = morpho::maxtree_ufind_parallel(ima, c4, std::less<uint8> () );
    // io::imprint(parent1);
    // io::imprint(parent3);

    unify_parent(ima, S1, parent1);
    unify_parent(ima, S1, parent2);
    unify_parent(ima, S3, parent3);
    //io::imprint(parent1);
    //io::imprint(parent3);
    BOOST_CHECK(all(parent1 == parent2));
    BOOST_CHECK(all(parent1 == parent3));
  }


  {
    image2d<std::size_t> parent1, parent2, parent3;
    std::vector<std::size_t> S1, S2, S3;
    std::tie(parent1, S1) = morpho::maxtree(ima, c4, std::greater<uint8> ());
    std::tie(parent2, S2) = morpho::maxtree_hqueue(ima, c4, std::greater<uint8> () );
    std::tie(parent3, S3) = morpho::maxtree_ufindbyrank(ima, c4, std::greater<uint8> () );
    // io::imprint(parent1);
    // io::imprint(parent2);

    unify_parent(ima, S1, parent1);
    unify_parent(ima, S1, parent2);
    unify_parent(ima, S3, parent3);
    // io::imprint(parent1);
    // io::imprint(parent2);
    BOOST_CHECK(all(parent1 == parent2));
    BOOST_CHECK(all(parent1 == parent3));
  }

}
