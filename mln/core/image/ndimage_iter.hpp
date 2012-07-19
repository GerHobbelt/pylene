#ifndef MLN_NDIMAGE_ITER_HH
# define MLN_NDIMAGE_ITER_HH

# include <boost/iterator/reverse_iterator.hpp>

# include <mln/core/iterator/fast_reverse_iterator.hpp>
# include <mln/core/image/internal/nested_loop_iterator.hpp>
# include <mln/core/image/ndimage_pixel.hpp>

//# include <type_traits>

namespace mln
{

template <typename Image, typename T>
using ndimage_iter = internal::nested_loop_iterator<
  internal::origin_point_visitor< typename Image::point_type >,
  internal::strided_pointer_value_visitor<char, typename Image::point_type>,
  ndimage_pixel<T, Image::ndim, Image>,
  internal::deref_return_value_policy>;

  /*
  template <typename Image, typename T>
  struct ndimage_iter :
    public boost::iterator_facade< ndimage_iter<Image, T>, T, boost::bidirectional_traversal_tag>
  {
  private:
    enum { dim = Image::ndim };
    typedef point<short,dim>     site_type;

  private:
    typedef char* ptr_t;

  public:
    ndimage_iter() {}

    template <typename T2>
    ndimage_iter(const ndimage_iter<Image, T2>& other,
                 std::enable_if< std::is_convertible<T2, T>::value, void>* dummy = NULL)
      : shape_ (other.shape_),
        strides_ (other.strides_),
        indices_ (other.indices_),
        pstack_ (other.pstack_)
    {
      (void) dummy;
    }


    ndimage_iter(const Image& ima, site_type p, ptr_t ptr) :
      shape_ (ima.domain_.shape()),
      strides_ (ima.strides_),
      indices_ (p)
    {
      //std::copy(ima.strides_, ima.strides_ + dim, strides_);
      std::fill(pstack_.begin(), pstack_.end(), ptr);
    }


  public:
    friend class boost::iterator_core_access;
    template <typename, typename> friend struct ndimage_iter;


    inline
    void increment()
    {
      unsigned i = dim-1;
      while (true)
        {
          indices_[i] += 1;
          if (indices_[i] < shape_[i] || i == 0) {
            pstack_[i] += strides_[i];
            break;
          }
          indices_[i] = 0;
          --i;
        }
      if (i < dim-1)
        std::fill(pstack_.begin() + i + 1, pstack_.begin() + dim, pstack_[i]);
    }

    inline
    void decrement()
    {
      unsigned i = dim-1;
      while (true)
        {
          indices_[i] -= 1;
          if (indices_[i] >= 0 || i == 0) {
            pstack_[i] -= strides_[i];
            break;
          }
          indices_[i] = shape_[i] - 1;
          --i;
        }
      for (++i; i < dim; ++i)
        pstack_[i] = pstack_[i-1] + indices_[i] * strides_[i];
    }



    template <typename Image2, typename U>
    bool equal(const ndimage_iter<Image2, U>& other) const
    {
      return this->pstack_[dim-1] == other.pstack_[dim-1];
    }

    T& dereference() const
    {
      return *reinterpret_cast<T*>(pstack_[dim-1]);
    }


    site_type shape_; // Domain iteration
    std::array<size_t, dim> strides_; // Strides for memory access
    site_type indices_; // Site location
    std::array<char*, dim> pstack_;	// pointer stack
    };

  */

} // end of namespace mln

// Specialization of the boost::reverse_iterator
// Because increment and decrement have constant complexity but quite
// expensive compared to stl iterator.

namespace boost
{


  template <>
  template <typename Image, typename T>
  struct reverse_iterator< mln::ndimage_iter<Image, T> >:
    mln::fast_reverse_iterator< mln::ndimage_iter<Image, T> >
  {
  private:
    typedef mln::ndimage_iter<Image, T> base_it;
    typedef mln::fast_reverse_iterator<base_it> base;
  public:
    reverse_iterator() = default;
    reverse_iterator(const reverse_iterator&) = default;
    reverse_iterator(base_it it) : base(it) {}
  };

}








// namespace mln {

//   template <typename Image, typename T>
// struct ndimage_rev_iter :
//     public boost::iterator_facade< ndimage_rev_iter<Image, T>, T, boost::forward_traversal_tag>
//   {
//   private:
//     enum { dim = Image::ndim };

//   public:
//     typedef typename point<short,dim>::type site_type;

//   ndimage_rev_iter() {}


//   ndimage_rev_iter(const Image& ima, char* ptr) :
//     domain_ (ima.domain_),
//     indices_ (ima.domain_.pmax)
//   {
//     indices_ -= 1;
//     std::copy(ima.strides_, ima.strides_ + dim, strides_);
//     std::fill(pstack_, pstack_ + dim, ptr);
//   }




// public:
//   friend class boost::iterator_core_access;
//   template <typename, typename> friend struct ndimage_rev_iter;


//   inline
//   void increment()
//   {
//     unsigned i = dim-1;
//     while (true)
//       {
//         indices_[i] -= 1;
//         pstack_[i] -= strides_[i];
//         if (indices_[i] >= domain_.pmin[i] || i == 0)
//           break;
//         indices_[i] = domain_.pmax[i] - 1;
//         --i;
//       }
//     if (i < dim-1)
//       std::fill(pstack_ + i + 1, pstack_ + dim, pstack_[i]);
//   }


//     template <typename Image2, typename U>
//     bool equal(const ndimage_rev_iter<Image2, U>& other) const
//   {
//     return this->pstack_[dim-1] == other.pstack_[dim-1];
//   }

//   T& dereference() const
//   {
//     return *reinterpret_cast<T*>(pstack_[dim-1]);
//   }


//   box<short, dim> domain_; // Domain iteration
//   size_t strides_[dim]; // Strides for memory access
//   site_type indices_; // Site location 
//   char* pstack_[dim];	// pointer stack	
// };

//}

#endif
