#ifndef MLN_BOX_HH
# define MLN_BOX_HH

# include <mln/core/point.hpp>
# include <mln/core/image/internal/nested_loop_iterator.hpp>
# include <mln/core/domain/box_iter.hpp>
# include <mln/core/ch_value.hpp>

namespace mln
{
  // Fwd
  //template <typename T, unsigned dim> struct box_iter;
  //template <typename T, unsigned dim> struct box_rev_iter;


  //
  // \pre \tparam T must be an integral type
  template <typename T, unsigned dim>
  struct box
  {
    typedef point<T, dim>          point_type;

    typedef internal::nested_loop_iterator<
      internal::domain_point_visitor_forward< point<T, dim> >,
      internal::no_op_visitor,
      internal::no_op_visitor,
      internal::point_structure<T, dim>,
      internal::deref_return_point_policy> iterator;

    typedef internal::nested_loop_iterator<
      internal::domain_point_visitor_backward< point<T, dim> >,
      internal::no_op_visitor,
      internal::no_op_visitor,
      internal::point_structure<T, dim>,
      internal::deref_return_point_policy> reverse_iterator;

    bool has(const point_type& p) const
    {
      mln_precondition(__is_valid());
      return (pmin <= p) && (p < pmax);
    }

    point_type shape() const
    {
      mln_precondition(__is_valid());
      return pmax - pmin;
    }

    bool empty() const
    {
      mln_precondition(__is_valid());
      for (int i = 0; i < dim; ++i)
	if (pmax[i] <= pmin[i])
	  return true;
      return false;
    }

    unsigned size() const
    {
      mln_precondition(__is_valid());
      unsigned sz = 1;
      for (unsigned i = 0; i < dim; ++i)
	sz *= pmax[i] - pmin[i];
      return sz;
    }

    iterator iter() const
    {
      mln_precondition(__is_valid());
      return iterator( internal::point_structure<T, dim> (),
		       internal::make_point_visitor_forward(pmin, pmax),
		       internal::no_op_visitor (),
		       internal::no_op_visitor ());
    }

    reverse_iterator riter() const
    {
      mln_precondition(__is_valid());
      return iterator( internal::point_structure<T, dim> (),
		       internal::make_point_visitor_backward(pmin, pmax),
		       internal::no_op_visitor (),
		       internal::no_op_visitor ());
    }


#ifdef MLN_TBB
#endif

    bool __is_valid() const
    {
      for (unsigned i = 0; i < dim; ++i)
	if (pmin[i] > pmax[i])
	  return false;
	else if (pmin[i] == pmax[i]) // empty <=> pmin = pmax
	  return pmin == pmax;
      return true;
    }


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

  /// Traits

  /// Forward
  template <typename> struct image2d;

  template <>
  struct image_from_domain<box2d>
  {
    typedef image2d<void> type;
  };

}


#endif
