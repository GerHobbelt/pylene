#ifndef THICKEN_HPP
# define THICKEN_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/io/imprint.hpp>

namespace mln
{


  template <typename V>
  void
  thicken(const image2d<V>& ima, const image2d<unsigned>& parent, const std::vector<unsigned>& S)
  {
    static consexpr unsigned UNDEFINED = value_traits<unsigned>::max();

    image2d<unsigned> vanish;
    image2d<unsigned> appear;
    image2d<unsigned> depth;
    image2d<unsigned> last;

    resize(vanish, ima, ima.border(), UNDEFINED);
    resize(last, ima, ima.border(), UNDEFINED);
    resize(appear, ima);
    resize(depth, ima);

    auto offset = wrt_delta_indexes(ima, c4.dpoints);

    depth[S[0]] = 0;

    for (unsigned p : S)
      {
	unsigned q = parent[p];

	if (ima[p] == ima[q]) {
	  depth[p] = depth[q];
	} else {
	  depth[p] = depth[q] + 1;
	  q = p;
	}

	for (int k : offset)
	  {
	    unsigned n = p + k;
	    if (vanish[n] == UNDEFINED)
	      vanish[n] = depth[q];
	    else
	      appear[n] = depth[q];
	  }
      }

    for (int i = S.size()-1; i >= 0; --i)
      {




      }

  }



}

#endif // ! THICKEN_HPP
