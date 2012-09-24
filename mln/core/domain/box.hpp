#ifndef MLN_BOX_HH
# define MLN_BOX_HH

# include <mln/core/point.hpp>
# include <mln/core/image/internal/nested_loop_iterator.hpp>
//# include <mln/core/iterator/fast_reverse_iterator.hpp>
# include <mln/core/domain/box_iter.hpp>

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
      internal::no_op_value_visitor,
      internal::point_structure<T, dim>,
      internal::deref_return_point_policy> iterator;

    typedef internal::nested_loop_iterator<
      internal::domain_point_visitor_backward< point<T, dim> >,
      internal::no_op_value_visitor,
      internal::point_structure<T, dim>,
      internal::deref_return_point_policy> reverse_iterator;

    bool has(const point_type& p) const
    {
      return (pmin <= p) && (p < pmax);
    }

    point_type shape() const
    {
      return pmax - pmin;
    }


    iterator iter() const
    {
      return iterator( internal::point_structure<T, dim> (),
		       internal::make_point_visitor_forward(pmin, pmax),
		       internal::no_op_value_visitor ());
    }

    reverse_iterator riter() const
    {
      return iterator( internal::point_structure<T, dim> (),
		       internal::make_point_visitor_backward(pmin, pmax),
		       internal::no_op_value_visitor ());
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


#endif
