#ifndef SIMPLIFY_HPP
# define SIMPLIFY_HPP

#include <vector>

#include <mln/core/image/image2d.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/algorithm/transform.hpp>

#include <apps/tos/topology.hpp>


namespace mln
{




  /// \brief Simplify an image removing overlapping level lines.
  ///
  /// \param f: The image on which the tree has been computed
  /// \param K: K outputed by the ToS algorithm (twice as big as \p f)
  /// \param parent: parent outputed by the ToS algorithm
  /// \param S: S outputed by the ToS algorithm
  /// \param lambda: radius for non overlapping level lines
  /// \param area: perform a area grain filter
  /// \param areafactor: avoid sur-simplification by limiting node desactivation
  ///                    a shape S1 can only be desactived by a shape S2 if area(S1) * areafactor < area(S2)
  template <typename V, typename T>
  image2d<V>
  simplify_bottom_up(const image2d<V>& f, const image2d<T>& K,
		     const image2d<unsigned>& parent, const std::vector<unsigned>& S,
		     int lambda, int area = 0, float areafactor = 0);


  template <typename V, typename T>
  image2d<V>
  simplify_top_down(const image2d<V>& f, const image2d<T>& K,
		    const image2d<unsigned>& parent, const std::vector<unsigned>& S, int eps);

  /// \brief For any shape \p S, retrieve the smallest shape \p A such that $\delta_\epslison(s) \subset A$
  ///
  /// A stucturing element used for dilation is square of radius $\epsilon$.
  template <typename T>
  image2d<unsigned>
  smallest_enclosing_shape(const image2d<T>& K, const image2d<unsigned>& parent, const std::vector<unsigned>& S, int eps);



  /*******************************/
  /** Implementation            **/
  /*******************************/

  namespace internal
  {

    template <typename T, typename N>
    unsigned
    common_ancestor(unsigned x, const N& dx,
		    const image2d<T>& K,
		    const image2d<unsigned>& parent,
		    const image2d<unsigned>& depth)
    {
      std::vector<unsigned> v;
      unsigned minp = K[x] == K[parent[x]] ? parent[x] : x;
      unsigned mindepth = depth[minp];
      v.push_back(minp);
      for (int offset : dx)
	{
	  unsigned q = x + offset;
	  if (K.domain().has(K.point_at_index(q)))
	    {
	      // WARNING: Le root est pute, dans le cas du thickening
	      // il ne faut pas le considérer car de nombreuses ont au moins un edge
	      // en commun avec lui (border effect).
	      unsigned rq = K[q] == K[parent[q]] ? parent[q] : q;
	      if (parent[rq] == rq) // Root -> casse toi
		continue;

	      v.push_back(rq);
	      if (depth[rq] < mindepth) {
		mindepth = depth[rq];
		minp = rq;
	      }
	    }
	}

      bool modif;
      do {
	modif = false;
	for(unsigned& x: v)
	  if (depth[x] > mindepth) {
	    x = parent[x];
	    modif = true;
	  } else if (depth[x] == mindepth and x != minp) {
	    x = parent[x];
	    mindepth--;
	    minp = x;
	    modif = true;
	  }
      } while (modif);

      mln_assertion(std::all_of(v.begin(), v.end(), [minp](unsigned x) { return x == minp; }));
      return minp;
    }



    unsigned
    common_ancestor(unsigned x, unsigned y, const image2d<unsigned>& parent, const image2d<unsigned>& depth)
    {
      while (x != y) {
	if (depth[x] > depth[y])
	  x = parent[x];
	else
	  y = parent[y];
      }
      return x;
    }
  }


  template <typename T>
  image2d<unsigned>
  smallest_enclosing_shape(const image2d<T>& K, const image2d<unsigned>& parent, const std::vector<unsigned>& S, int eps)
  {
    image2d<unsigned> depth;
    resize(depth, K);

    // Compute depth attribute
    {
      depth[S[0]] = 0;
      for (unsigned i = 1; i < S.size(); ++i)
	{
	  unsigned x = S[i];
	  if (K[x] != K[parent[x]]) // canonical element
	    depth[x] = depth[parent[x]] + 1;
	  else
	    depth[x] = depth[parent[x]];
	}
    }

    image2d<unsigned> enc;
    resize(enc, parent);


    // Init at beginning, the most enclosing shape of S is S itself.
    mln_pixter(px, enc);
    mln_forall(px) {
      unsigned i = px->index();
      px->val() = (K[i] == K[parent[i]]) ? parent[i] : i;
    }

    // Nbh
    std::vector<int> dx;
    for (short i = -eps*1; i <= eps*1; i += 1)
      for (short j = -eps*1; j <= eps*1; j += 1)
	dx.push_back(K.delta_index(point2d{i,j}));


    for (int i = S.size()-1; i > 0; --i)
      {
	unsigned x = S[i];

	if (not K1::is_face_2(K.point_at_index(x)))
	  continue;

	unsigned anc = internal::common_ancestor(x, dx, K, parent, depth);
	enc[x] = internal::common_ancestor(enc[x], anc, parent, depth);
	enc[parent[x]] = internal::common_ancestor(enc[parent[x]], enc[x], parent, depth);
      }

    return enc;
  }




  template <typename V, typename T>
  image2d<V>
  simplify_bottom_up(const image2d<V>& f,
		     const image2d<T>& K,
		     const image2d<unsigned>& parent,
		     const std::vector<unsigned>& S,
		     int eps,
		     int area,
		     float areafactor)
  {
    image2d<unsigned> areas;
    resize(areas, K).init(0);

    // Compute node activity
    image2d<bool> active;
    resize(active, K).init(true);


    // Compute area
    for (int i = S.size() - 1; i > 0; --i)
      {
	unsigned x = S[i];
	if (not K1::is_face_2(K.point_at_index(x)))
	  continue;
	areas[x] += 1;
	areas[parent[x]] += areas[x];
	active[x] = areas[x] >= area;
      }
    // root
    areas[S[0]] += 1;

    image2d<unsigned> enc = smallest_enclosing_shape(K, parent, S, eps);
    image2d<unsigned> depth;
    resize(depth, K);
    // Compute depth attribute
    {
      depth[S[0]] = 0;
      for (unsigned i = 1; i < S.size(); ++i)
	{
	  unsigned x = S[i];
	  if (K[x] != K[parent[x]]) // canonical element
	    depth[x] = depth[parent[x]] + 1;
	  else
	    depth[x] = depth[parent[x]];
	}
    }


    int nactive = 0;
    for (int i = S.size() - 1; i > 0; --i)
      {
	unsigned x = S[i];

	if (K[x] == K[parent[x]] or !active[x])
	  continue;

	++nactive;

	unsigned anc = enc[x];
	//std::cout << "Node depth: " << depth[x] << " -> Ancestor depth: " << depth[anc] << std::endl;

	// unactive ]x ---> anc[
	if (x != anc) {
	  unsigned p = parent[x];
	  while (p != anc and areas[p] * areafactor < areas[x])
	    {
	      active[p] = false;
	      p = parent[p];
	    }
	}
      }

    // Simplify
    image2d<V> out;
    resize(out, f);
    nactive = 0;
    for (unsigned x: S)
      {
	point2d p = K.point_at_index(x);
	if (K1::is_face_2(p))
	  {
	    if ((active[x] and K[parent[x]] != K[x]) or x == parent[x]) {
	      ++nactive;
	      out(p/2) = f(p/2);
	    } else {
	      point2d q = K.point_at_index(parent[x]);
	      mln_assertion(K1::is_face_2(q));
	      out(p/2) = out(q/2);
	    }
	  }
      }
    std::cout << "Number of nodes: " << nactive << std::endl;
    //io::imsave(transform(enc, [&areas, &S](unsigned x) -> float { return (float) areas[x] / areas[S[0]]; }), "tmp.tiff");

    return out;
  }

  template <typename V, typename T, class Predicate>
  image2d<V>
  simplify_top_down_pred(const image2d<V>& f, const image2d<T>& K, const image2d<unsigned>& parent,
			 const std::vector<unsigned>& S, Predicate pred)
  {
    // Compute node activity
    image2d<unsigned> alive;
    resize(alive, K);

    alive[S[0]] = S[0];

    for (int i = 1; i < S.size(); ++i)
      {
	unsigned x = S[i];
	if (K[x] == K[parent[x]])
	  continue;

	unsigned y = parent[x];
	unsigned z = alive[y]; // last ancestor alive
	alive[x] = pred(x, z) ? x : z;
      }

    // Simplify
    image2d<V> out;
    resize(out, f);
    int nactive = 0;

    // point2d p = K.point_at_index(S[0]);
    // out(p/2) = f(p/2);
    // for (unsigned x: S)
    //   {
    // 	point2d p = K.point_at_index(x);
    // 	if (K1::is_face_2(p))
    // 	  {
    // 	    if ((active[x] and K[parent[x]] != K[x]) or x == parent[x]) {
    // 	      ++nactive;
    // 	      out(p/2) = f(p/2);
    // 	    } else {
    // 	      point2d q = K.point_at_index(parent[x]);
    // 	      mln_assertion(K1::is_face_2(q));
    // 	      out(p/2) = out(q/2);
    // 	    }
    // 	  }
    //   }
    // std::cout << "Number of nodes: " << nactive << std::endl;
    // std::cout << "Number of nodes with root connection:" << root_connection << std::endl;
    //io::imsave(transform(enc, [&areas, &S](unsigned x) -> float { return (float) areas[x] / areas[S[0]]; }), "tmp.tiff");

    return out;


  }



  template <typename V, typename T>
  image2d<V>
  simplify_top_down(const image2d<V>& f, const image2d<T>& K,
   		    const image2d<unsigned>& parent, const std::vector<unsigned>& S, int eps)
  {
    // Compute node activity
    image2d<bool> active;
    resize(active, K).init(false);


    image2d<unsigned> enc = smallest_enclosing_shape(K, parent, S, eps);
    active[S[0]] = false;
    unsigned root_connection = 0;
    for (unsigned i = 1; i < S.size(); ++i)
      {
	unsigned x = S[i];
	if (K[x] == K[parent[x]])
	  continue;

	unsigned anc = enc[x];
	// Prevent bordering effect (if anc == root, set x active)
	if (anc == S[0]) {
	  root_connection++;
	  // active[x] = true;
	  // continue;
	}

	unsigned p = x;
	if (p != anc) {
	  // A node is active if ]x --> anc[ are inactive
	  p = parent[x];
	  while (p != anc and !active[p])
	    p = parent[p];
	}

	if (p == anc) {
	  active[x] = true;
	}
      }

    // Simplify
    image2d<V> out;
    resize(out, f);
    int nactive = 0;

    point2d p = K.point_at_index(S[0]);
    out(p/2) = f(p/2);
    for (unsigned x: S)
      {
	point2d p = K.point_at_index(x);
	if (K1::is_face_2(p))
	  {
	    if ((active[x] and K[parent[x]] != K[x]) or x == parent[x]) {
	      ++nactive;
	      out(p/2) = f(p/2);
	    } else {
	      point2d q = K.point_at_index(parent[x]);
	      mln_assertion(K1::is_face_2(q));
	      out(p/2) = out(q/2);
	    }
	  }
      }
    std::cout << "Number of nodes: " << nactive << std::endl;
    std::cout << "Number of nodes with root connection:" << root_connection << std::endl;
    //io::imsave(transform(enc, [&areas, &S](unsigned x) -> float { return (float) areas[x] / areas[S[0]]; }), "tmp.tiff");

    return out;

  }

}

#endif // ! SIMPLIFY_HPP
