#ifndef MLN_MORPHO_FILTERING_HPP
# define MLN_MORPHO_FILTERING_HPP

namespace mln
{

  namespace morpho
  {

    template <typename V, typename Compare = std::less<V> >
    image2d<unsigned>
    area_compute(const image2d<V>& ima,
		 const image2d<typename image2d<V>::size_type>& parent,
		 const std::vector<typename image2d<V>::size_type>& S,
		 const Compare& cmp = Compare ());



    /*************************/
    /** Implementation      **/
    /*************************/

    template <typename V, typename Compare = std::less<V> >
    image2d<unsigned>
    area_compute(const image2d<V>& ima,
		 const image2d<typename image2d<V>::size_type>& parent,
		 const std::vector<typename image2d<V>::size_type>& S,
		 const Compare& cmp = Compare ())
    {
      typedef typename image2d<V>::size_type size_type;
      image2d<unsigned> area;
      resize(area, ima, ima.border(), 0);

      point2d q = ima.point_at_index(S[0]);
      area[S[0]] += (q[0] % 2 == 0 and q[1] % 2 == 0);
      for (int i = S.size()-1; i > 0; --i) // all except root
	{
	  size_type p = S[i];

	  point2d q = ima.point_at_index(p);
	  if (q[0] % 2 == 0 and q[1] % 2 == 0) // 2-face
	    area[p] += 1;
	  area[parent[p]] += area[p];
	}

      // propagate to canonical element to non-canonical
      for (size_type p: S)
	{
	  V a = ima[p], b = ima[parent[p]];
	  if (!cmp(a, b) and !cmp(b,a)) // a == b, same node
	    area[p] = area[parent[p]];
	}

      return area;
    }
  }

}

#endif // ! MLN_MORPHO_FILTERING_HPP
