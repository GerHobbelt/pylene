#ifndef MLN_CORE_BOX_ITER_H
# define MLN_CORE_BOX_ITER_H

# include <boost/iterator/iterator_facade.hpp>
# include <mln/core/iterator/fast_reverse_iterator.hpp>
# include <boost/iterator/reverse_iterator.hpp>
# include <mln/core/image/internal/nested_loop_iterator.hpp>


namespace mln
{

  namespace internal
  {

    template <typename T, unsigned dim>
    struct point_structure
    {
      typedef mln::point<T, dim> point_type;

      point_structure() {}
      point_structure(const mln::point<T, dim>& p) : point_ (p) {}


      const mln::point<T, dim>& point() const { return point_; }


      mln::point<T, dim>&       get_point() { return point_; }
      const mln::point<T, dim>& get_point() const { return point_; }
      void			get_value() const  { }

      bool                equal(const point_structure& other) const { return point_ == other.point_; }

      mln::point<T, dim> point_;
    };

  }

}


#endif // !MLN_CORE_BOX_ITER_H
