#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/range/algorithm/generate.hpp>
#include <mln/morpho/maxtree_ufind.hpp>
#include <mln/morpho/maxtree_ufindrank_parallel.hpp>
#include <mln/morpho/maxtree_hqueue_parallel.hpp>
#include <mln/morpho/maxtree_ufind_parallel.hpp>
#include <mln/morpho/maxtree1d.hpp>
#include <mln/core/grays.hpp>
#include <mln/io/imprint.hpp>
#include <tbb/task_scheduler_init.h>

#define BOOST_TEST_MODULE Morpho
#include <boost/test/unit_test.hpp>
#include <random>

namespace mln
{


  template <typename V>
  void unify_parent(const mln::image2d<V>& f,
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
        std::size_t q = mln::morpho::internal::zfind_repr(f, parent, p);
	if (p != q)
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

template <typename V>
bool iscanonized(const mln::image2d<V>& ima,
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



template <typename V, typename StrictWeakOrdering>
void runtest(const mln::image2d<V>& ima, StrictWeakOrdering cmp)
{
  using namespace mln;



  image2d<std::size_t> parent1, parent2, parent3, parent4, parent5, parent6, parent7;
  std::vector<std::size_t> S1;
  std::tie(parent1, S1) = morpho::maxtree(ima, c4, cmp);
  parent2 = morpho::impl::serial::maxtree_hqueue(ima, c4, cmp );
  parent3 = morpho::impl::parallel::maxtree_hqueue(ima, c4, cmp );
  parent4 = morpho::impl::parallel::maxtree_ufind(ima, c4, cmp );
  parent5 = morpho::impl::parallel::maxtree_ufind_line(ima, c4, cmp );
  parent6 = morpho::impl::serial::maxtree_ufindrank(ima, c4, cmp );
  parent7 = morpho::impl::parallel::maxtree_ufindrank(ima, c4, cmp );

  //io::imprint(parent1);
  //io::imprint(parent4);
  //io::imprint(parent4_);
  // io::imprint(parent3);
  BOOST_CHECK(iscanonized(ima, parent1));
  BOOST_CHECK(iscanonized(ima, parent2));
  BOOST_CHECK(iscanonized(ima, parent3));
  BOOST_CHECK(iscanonized(ima, parent4));
  BOOST_CHECK(iscanonized(ima, parent5));
  BOOST_CHECK(iscanonized(ima, parent6));
  BOOST_CHECK(iscanonized(ima, parent7));

  unify_parent(ima, S1, parent1);
  unify_parent(ima, S1, parent2);
  unify_parent(ima, S1, parent3);
  unify_parent(ima, S1, parent4);
  unify_parent(ima, S1, parent5);
  unify_parent(ima, S1, parent6);
  unify_parent(ima, S1, parent7);
  //io::imprint(parent1);
  //io::imprint(parent4);
  BOOST_CHECK(all(parent1 == parent2));
  BOOST_CHECK(all(parent1 == parent3));
  BOOST_CHECK(all(parent1 == parent4));
  BOOST_CHECK(all(parent1 == parent5));
  BOOST_CHECK(all(parent1 == parent6));
  BOOST_CHECK(all(parent1 == parent7));
}


BOOST_AUTO_TEST_CASE(Maxtree)
{
  using namespace mln;
  typedef UInt<12> V;
  image2d<V> ima(100, 50);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> sampler(0, value_traits<V>::max());
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

  // {
  //   image2d<point2d> parent_;
  //   image2d<std::size_t> parent1;
  //   std::vector<std::size_t> S;
  //   std::less<uint8> cmp;
  //   resize(parent_, ima);

  //   morpho::maxtree1d(ima, parent_, 0, cmp);
  //   std::tie(parent1, S) = morpho::maxtree(ima, c4, cmp);
  //   auto parent = pt2idx(parent_);

  //   BOOST_CHECK(iscanonized(ima, parent));
  //   BOOST_CHECK(iscanonized(ima, parent1));

  //   unify_parent(ima, S, parent1);
  //   unify_parent(ima, S, parent);
  //   BOOST_CHECK(all(parent == parent1));
  // }

  runtest(ima, std::less<V> ());
  runtest(ima, std::greater<V> ());

}
