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

      point_structure();
      point_structure(const mln::point<T, dim>& p) : point_ (p) {}


      const mln::point<T, dim>& point() const { return point_; }


      mln::point<T, dim>& get_point() { return point_; }
      void                get_value() { }
      bool                equal(const point_structure& other) const { return point_ == other.point_; }

      mln::point<T, dim> point_;
    };

  }

  template <typename T, unsigned dim>
  using box_iter = internal::nested_loop_iterator<
    internal::domain_point_visitor< point<T, dim> >,
    internal::no_op_value_visitor,
    internal::point_structure<T, dim>,
    internal::deref_return_point_policy>;


  /*
  template <typename T, unsigned dim>
  struct box_iter :
    public boost::iterator_facade< box_iter<T, dim>, const typename box<T, dim>::point_type,
                                   boost::bidirectional_traversal_tag, const typename box<T, dim>::point_type&>
  {
    typedef typename box<T, dim>::point_type point_type;

    box_iter() {}

    // box_iter(const box_rev_iter<T, dim>& it) :
    //   box_ (it.box_),
    //   point_ (it.point_)
    // {
    // }

    box_iter(const box<T, dim>& box, point_type p) :
      box_ (box),
      point_ (p)
    {
    }

  public:
    friend class boost::iterator_core_access;
    template <typename, unsigned> friend struct box_rev_iter;

    inline
    void increment()
    {
      for (int i = dim-1; ;--i)
        {
          point_[i] += 1;
          if (point_[i] < box_.pmax[i] || i == 0)
            break;
          point_[i] = box_.pmin[i];
        }
    }

    inline
    void decrement()
    {
      for (int i = dim-1; ;--i)
        {
          point_[i] -= 1;
          if (point_[i] >= box_.pmin[i] || i == 0)
            break;
          point_[i] = box_.pmax[i] - 1;
        }
    }

    template <typename U>
    bool equal(const box_iter<U, dim>& other) const
    {
      return this->point_ == other.point_;
    }

    const point_type& dereference() const
    {
      return point_;
    }


    box<short, dim> box_; // Domain iteration (do faster than reference)
    point_type point_; // Site location
  };


  template <typename T, unsigned dim>
  struct box_rev_iter :
    public boost::iterator_facade< box_rev_iter<T, dim>, const typename box<T, dim>::point_type,
                                   boost::bidirectional_traversal_tag, const typename box<T, dim>::point_type&>
  {
    typedef typename box<T, dim>::point_type point_type;

    box_rev_iter() {}

    box_rev_iter(const box_iter<T, dim>& it) :
      box_ (it.box_),
      point_ (it.point_)
    {
    }

    box_rev_iter(const box<T, dim>& box, point_type p) :
      box_ (box),
      point_ (p)
    {
    }

  public:
    friend class boost::iterator_core_access;
    template <typename, unsigned> friend struct box_iter;



    inline
    void decrement()
    {
      for (int i = dim-1; ;--i)
        {
          point_[i] += 1;
          if (point_[i] < box_.pmax[i] || i == 0)
            break;
          point_[i] = box_.pmin[i];
        }
    }

    inline
    void increment()
    {
      for (int i = dim-1; ;--i)
        {
          point_[i] -= 1;
          if (point_[i] >= box_.pmin[i] || i == 0)
            break;
          point_[i] = box_.pmax[i] - 1;
        }
    }

    template <typename U>
    bool equal(const box_rev_iter<U, dim>& other) const
    {
      return this->point_ == other.point_;
    }

    const point_type& dereference() const
    {
      return point_;
    }


    box<short, dim> box_; // Domain iteration (do faster than reference)
    point_type point_; // Site location
  };
  */
}

// Specialization of the boost::reverse_iterator
// Because increment and decrement have constant complexity but quite
// expensive compared to stl iterator.

namespace boost
{

  template <>
  template <typename T, unsigned dim>
  struct reverse_iterator< mln::box_iter<T, dim> > :
    mln::fast_reverse_iterator< mln::box_iter<T, dim> >
  {
  private:
    typedef mln::box_iter<T, dim> base_it;
    typedef mln::fast_reverse_iterator<base_it> base;
  public:
    reverse_iterator() = default;
    reverse_iterator(const reverse_iterator&) = default;
    reverse_iterator(const base_it& it) : base(it) {}
  };


  // template <>
  // template <typename T, unsigned dim>
  // struct reverse_iterator< mln::box_rev_iter<T, dim> > : mln::box_iter<T, dim>
  // {
  // private:
  //   typedef mln::box_iter<T, dim> base;
  // public:
  //   reverse_iterator() = default;
  //   reverse_iterator(const reverse_iterator&) = default;
  //   reverse_iterator(mln::box_rev_iter<T, dim> it) : base(--it) {}
  // };

}


#endif // !MLN_CORE_BOX_ITER_H
