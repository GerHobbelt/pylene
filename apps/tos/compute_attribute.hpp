#ifndef COMPUTE_ATTRIBUTE_HPP
# define COMPUTE_ATTRIBUTE_HPP

# include <mln/accu/accumulator.hpp>
# include <mln/core/algorithm/fill.hpp>

namespace mln
{

  namespace morpho
  {


    template <typename V, typename T, typename Accumulator2F = NoOpAccumulator, typename Accumulalor1F = NoOpAccumulator>
    tos_compute_attribute(const image2d<V>& K, const image2d<unsigned>& parent,
			  const std::vector<unsigned>& S, const image2d<T>& ori,
			  Accumulalor2f acc2f = Accumulalor2f (), Accumulalor1f acc1f = Accumulalor1f ())
    {
      static constexpr bool use_acc1f = !std::is_same<Accumulator1F, NoOpAccumulator>::value;
      static constexpr bool use_acc2f = !std::is_same<Accumulator2F, NoOpAccumulator>::value;

      static constexpr unsigned UNACTIVE = value_traits<unsigned>::max();

      mln_precondition(K.domain() == ori.domain());
      mln_precondition(K.domain() == parent.domain());

      struct edge_attribute
      {
	unsigned appear;
	unsigned vanish;
	float value;
      };

      box2d domain = K.domain();
      domain.pmax = domain.pmax * 2 - 1;
      mln_ch_value(I, edge_attribute) e(domain);

      edge_attribute default_value = { UNACTIVE, UNACTIVE, };
      fill(e, default_value);

      // Kge of accumulators
      mln_ch_value(I, Accumulator2F) im_acc2f;
      mln_ch_value(I, Accumulator1F) im_acc1f;

      resize(im_acc2f, f, f.border(), acc2f);
      resize(im_acc1f, f, f.border(), acc1f);



      auto didx_k1 = wrt_delta_index(K, c4);
      auto didx_k2 = wrt_delta_index(active, c4);

      // Compute appear/vanish for edges
      // and 2-face attributes
      for (int i = S.size() - 1; i >= 0; --i)
	{
	  unsigned p  = S[i];
	  unsigned rp = K[parent[p]] == K[p] ? parent[p] : p;

	  point2d realp = K.point_at_index(p);

	  if (K1::is_face_2(realp))
	    {
	      im_acc2f[rp].take(ori[p])

	      point2d realp2 = 2 * p;
	      unsigned p2 = active.index_of_point(realp2);
	      for (int k = 0; k < 4; ++k)
		{
		  unsigned n1 = p + didx_k1[k];
		  unsigned n2 = p2 + didx_k2[k];

		  if (e[n2].appear == UNACTIVE) {
		    e[n2].appear = rp;
		    auto grad = (ori[p] - ori[n1]);
		    e[n2].value = (grad*grad).sum();
		  } else
		    e[n2].vanish = rp;
		}
	    }
	}


      // Transmit edge atribute to nodes
      if (use_acc1f)
      {
	point2d pmin = e.domain().pmin;
	point2d pmax = e.domain().pmax;
	for (int i = pmin[0]; i < pmax[0]; ++i)
	  for (int j = pmin[1] + i % 2 + 1; j < pmax[1]; j += 2) // foreach edges
	    {
	      point2d p = {i,j};
	      unsigned cur_node  = e(p).appear;
	      unsigned last_node = e(p).vanish;
	      float    val = e(p).value;
	      while (cur_node != last_node) {
		im_acc1f[cur_node].take(val);
		cur_node = parent[cur_node];
	      }
	    }
      }

      return std::make_pair(im_acc2f, im_acc1f);
    }

  }

}

# ifndef MLN_INCLUDE_ONLY

# endif // ! MLN_INCLUDE_ONLY

#endif // ! COMPUTE_ATTRIBUTE_HPP
