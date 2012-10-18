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
    auto px = parent.pixels().riter();
    mln_forall(px)
      {
	//std::cout << px->val() << " ! " << px->index() << std::endl;
        std::size_t p = px->index();
        std::size_t q = mln::morpho::internal::zfind_repr(f, parent, parent[p]);
        if (f[p] == f[parent[p]]) // p and q belong to the same node.
          if (p < parent[p])
            {
	      if (q == parent[q]) // transmit root property
		parent[p] = p;
	      else
		parent[p] = parent[q];
              parent[q] = p;
            }
      }

    mln_foreach(auto& p, parent.values())
      p = mln::morpho::internal::zfind_repr(f, parent, p);
  }

}

bool iscanonized(const mln::image2d<mln::uint8>& ima,
		 const mln::image2d<std::size_t>& parent)
{
  mln_pixter(px, parent);
  mln_forall(px)
  {
    std::size_t q = px->val();
    if (not(q == parent[q] or ima[q] != ima[parent[q]]))
      {
	std::cout << "canaonization error @ " << px->index() << std::endl;
	return false;
      }
  }
  return true;
}

mln::image2d<std::size_t>
pt2idx(const mln::image2d<mln::point2d>& parent)
{
  mln::image2d<std::size_t> out;
  mln::resize(out, parent);


  mln_viter(vin, vout, parent, out);
  mln_forall(vin, vout)
    *vout = parent.index_of_point(*vin);
  return out;
}



template <typename StrictWeakOrdering>
void runtest(const mln::image2d<mln::uint8>& ima, StrictWeakOrdering cmp)
{
  using namespace mln;

  

  image2d<std::size_t> parent1, parent2, parent3, parent4;
  image2d<point2d> parent4_;
  std::vector<std::size_t> S1, S2, S3;
  std::tie(parent1, S1) = morpho::maxtree(ima, c4, cmp);
  std::tie(parent2, S2) = morpho::maxtree_hqueue(ima, c4, cmp );
  std::tie(parent3, S3) = morpho::maxtree_ufindbyrank(ima, c4, cmp );
  parent4_ = morpho::impl::parallel::maxtree_ufind(ima, c4, cmp );
  parent4 = pt2idx(parent4_);

  //io::imprint(parent1);
  //io::imprint(parent4);
  //io::imprint(parent4_);
  // io::imprint(parent3);
  BOOST_CHECK(iscanonized(ima, parent1));
  BOOST_CHECK(iscanonized(ima, parent2));
  BOOST_CHECK(iscanonized(ima, parent3));
  BOOST_CHECK(iscanonized(ima, parent4));

  unify_parent(ima, S1, parent1);
  unify_parent(ima, S1, parent2);
  unify_parent(ima, S3, parent3);
  unify_parent(ima, S1, parent4);
  //io::imprint(parent1);
  //io::imprint(parent4);
  BOOST_CHECK(all(parent1 == parent2));
  BOOST_CHECK(all(parent1 == parent3));
  BOOST_CHECK(all(parent1 == parent4));
}


BOOST_AUTO_TEST_CASE(Maxtree)
{
  using namespace mln;

  image2d<uint8> ima(300, 200);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8> sampler(0, 20);
  range::generate(ima.values(), [&sampler, &gen] () { return sampler(gen); }) ;

  tbb::task_scheduler_init ts;

  // {
  //   image2d<std::size_t> f;
  //   resize(f, ima);
  //   mln_foreach(auto px, f.pixels())
  //     px.val() = px.index();
  //   io::imprint(f);
  // }

  // io::imprint(ima);

  runtest(ima, std::less<uint8> ());
  runtest(ima, std::greater<uint8> ());

}
