#ifndef MLN_BOX_HH
# define MLN_BOX_HH

# include <mln/core/point.hpp>
# include <mln/core/iterator/fast_reverse_iterator.hpp>

namespace mln
{
	// Fwd
	template <typename T, unsigned dim> struct box_iter;
  template <typename T, unsigned dim> struct box_rev_iter;


	//
	// \pre \tparam T must be an integral type
	template <typename T, unsigned dim>
	struct box
	{
          typedef typename point<T, dim>::type point_type;

          typedef box_iter<T, dim> iterator;
          typedef iterator const_iterator;
          typedef mln::fast_reverse_iterator<iterator> reverse_iterator;
          typedef reverse_iterator const_reverse_iterator;

          bool has(const point_type& p) const
          {
            return (pmin <= p) && (p < pmax);
          }

          point_type shape() const
          {
            return pmax - pmin;
          }

          iterator begin() const
          {
            return iterator(*this, pmin);
          }

          iterator end() const
          {
            point_type pend = pmin;
            pend[0] = pmax[0];
            return iterator(*this, pend );
          }

          reverse_iterator rbegin() const
          {
            //point_type pend = pmax - (short)1;
            return reverse_iterator(end());
          }

          reverse_iterator rend() const
          {
            //point_type pbeg = pmax - (short)1;
            //pbeg[0] = pmin[0] - 1;
            return reverse_iterator(begin());
          }


#ifdef MLN_TBB
#endif

          point_type pmin;
          point_type pmax;
	};

  typedef box<short, 1> box1d;
  typedef box<float, 1> box1df;
  typedef box<short, 2> box2d;
  typedef box<float, 2> box2df;
  typedef box<short, 3> box3d;

  template <typename T, unsigned dim>
  inline
  std::ostream&
  operator<< (std::ostream& s, const box<T, dim>& b)
  {
    return s << "[" << b.pmin << " ... " << b.pmax << "]";
  }
}

# include <mln/core/domain/box_iter.hpp>


#endif
