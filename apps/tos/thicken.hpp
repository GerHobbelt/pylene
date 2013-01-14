#ifndef THICKEN_HPP
# define THICKEN_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/wrt_offset.hpp>
# include <mln/io/imprint.hpp>

namespace mln
{


  template <typename V>
  image2d<unsigned>
  thicken(const image2d<V>& ima, image2d<unsigned>& parent, const std::vector<unsigned>& S)
  {
    image2d<unsigned> depth;
    resize(depth, ima);

    auto offset = wrt_delta_index(ima, c4.dpoints);

    depth[S[0]] = 0;
    for (unsigned p : S)
      {
	unsigned q = parent[p];

	if (ima[p] == ima[q])
	  depth[p] = depth[q];
	else
	  depth[p] = depth[q] + 1;
      }


    image2d<unsigned> mindepth; // The depth of the highest node on the contour
    image2d<unsigned> validdepth; // The maximal depth required to be valid
    resize(mindepth, ima, ima.border(), value_traits<unsigned>::max());
    resize(validdepth, ima, ima.border(), value_traits<unsigned>::max());
    for (int i = S.size()-1; i >= 0; --i)
      {
        unsigned p = S[i];
        unsigned q = ima[p] == ima[parent[p]] ? parent[p] : p;

        mln_foreach(int k, offset)
          mindepth[q] = std::min(mindepth[q], depth[p + k]);

        if (ima[q] != ima[parent[q]]) // q is a non-root node
          {
            mindepth[parent[q]] = std::min(mindepth[parent[q]], mindepth[q]);
            if (depth[q] <= validdepth[q]) // Then q is a kept
              {
                validdepth[q] = depth[q];
                validdepth[parent[q]] = mindepth[q];
              }
            else // q collapsed with its parent
              validdepth[parent[q]] = validdepth[q];
          }
      }

    // propagate and collapse parent
    for (unsigned p: S)
      {
        unsigned q = parent[p];
        if (ima[p] == ima[q])
          validdepth[p] = validdepth[q];
        if (validdepth[q] == validdepth[parent[q]])
          parent[p] = parent[q];
      }

    //io::imprint(depth);
    //io::imprint(validdepth);
    return validdepth;
  }

}

#endif // ! THICKEN_HPP
