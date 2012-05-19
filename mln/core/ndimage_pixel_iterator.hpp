#ifndef MLN_CORE_NDIMAGE_PIXEL_ITERATOR_HPP
# define MLN_CORE_NDIMAGE_PIXEL_ITERATOR_HPP

# include <mln/core/ndimage_pixel.hpp>
# include <boost/iterator/reverse_iterator.hpp>
# include <boost/iterator/iterator_facade.hpp>
# include <mln/core/iterator/fast_reverse_iterator.hpp>
# include <algorithm>

namespace mln {

  template <typename T, unsigned dim, typename I>
  struct ndimage_pixel_iterator :
    public boost::iterator_facade< ndimage_pixel_iterator<T, dim, I>,
                                   const ndimage_pixel<T, dim, I>,
                                   boost::bidirectional_traversal_tag>
  {
    typedef typename I::point_type     site_type;
    typedef ndimage_pixel<T,dim,I>     pixel_type;

    ndimage_pixel_iterator() {}


    ndimage_pixel_iterator(I* ima, site_type pstart, char* ptr) :
      domain_           (ima->domain_)
    {
      pixel_.point_ = pstart;
      pixel_.ima_ = ima;
      pixel_.ptr_ = ptr;
      std::copy(ima->strides_, ima->strides_ + dim, strides_);
      std::fill(pstack_, pstack_ + dim, ptr);
    }

  public:
    friend class boost::iterator_core_access;
    template <typename, unsigned, typename> friend struct ndimage_pixel_iterator;

    void increment();
    void decrement();

    template <typename U, typename I2>
    bool equal(const ndimage_pixel_iterator<U, dim, I2>& other) const;

    const pixel_type& dereference() const;

  private:
    box<short, dim> domain_;             // Domain iteration
    size_t          strides_[dim];    // Strides for memory access
    char*           pstack_[dim];               // pointer stack
    pixel_type      pixel_;
  };


  // template <typename T, unsigned dim, typename I>
  // struct ndimage_rev_pixel_iterator :
  //   public boost::iterator_facade< ndimage_rev_pixel_iterator<T, dim, I>,
  //                                  const ndimage_pixel<T, dim, I>,
  //                                  boost::forward_traversal_tag>
  // {

  //   typedef typename point<short,dim>::type site_type;
  //   typedef typename ndimage_pixel<T,dim,I>::type pixel_type;

  //   ndimage_rev_pixel_iterator() {}

  //   ndimage_rev_pixel_iterator(I* ima, char* ptr) :
  //     domain_           (ima.domain_)
  //   {
  //     pixel_.point_ = ima->domain_.pmin - 1;
  //     pixel_.ima_ = ima;
  //     pixel_.ptr_ = ptr;
  //     std::copy(ima->strides_, ima->strides_ + dim, strides_);
  //     std::fill(pstack_, pstack_ + dim, ptr);
  //   }

  // public:
  //   friend class boost::iterator_core_access;
  //   template <typename, unsigned, typename> friend struct ndimage_pixel_iterator;

  //   void increment();

  //   template <typename U, typename I2>
  //   bool equal(const ndimage_rev_pixel_iterator<U, dim, I2>& other) const;

  //   const pixel_type& dereference() const;

  // private:
  //   box<short, dim> domain_; // Domain iteration
  //   size_t          strides_[dim]; // Strides for memory access
  //   char*           pstack_[dim];	// pointer stack
  //   pixel_type      pixel_;
  // };





  template <typename T, unsigned dim, typename I>
  inline
  void
  ndimage_pixel_iterator<T,dim,I>::increment()
  {
    unsigned i = dim-1;
    while (true)
      {
        pixel_.point_[i] += 1;
        if (pixel_.point_[i] < domain_.pmax[i] || i == 0) {
          pstack_[i] += strides_[i];
          break;
        }
        pixel_.point_[i] = domain_.pmin[i];
        --i;
      }
    if (i < dim-1)
      std::fill(pstack_ + i + 1, pstack_ + dim, pstack_[i]);
    pixel_.ptr_ = pstack_[i];
  }


  template <typename T, unsigned dim, typename I>
  inline
  void
  ndimage_pixel_iterator<T,dim,I>::decrement()
  {
    unsigned i = dim-1;
    while (true)
      {
        pixel_.point_[i] -= 1;
        if (pixel_.point_[i] >= domain_.pmin[i] || i == 0) {
          pstack_[i] -= strides_[i];
          break;
        }
        pixel_.point_[i] = domain_.pmax[i] - 1;
        --i;
      }
    for (++i; i < dim; ++i)
      pstack_[i] = pstack_[i-1] + (pixel_.point_[i] - domain_.pmin[i]) * strides_[i];
    pixel_.ptr_ = pstack_[i-1];
  }

  template <typename T, unsigned dim, typename I>
  template <typename U, typename I2>
  inline
  bool
  ndimage_pixel_iterator<T,dim,I>::equal(const ndimage_pixel_iterator<U, dim, I2>& other) const
  {
    return this->pstack_[dim-1] == other.pstack_[dim-1];
  }

  template <typename T, unsigned dim, typename I>
  inline
  const typename ndimage_pixel_iterator<T,dim,I>::pixel_type&
  ndimage_pixel_iterator<T,dim,I>::dereference() const
  {
    return pixel_;
  }

  // template <typename T, unsigned dim, typename I>
  // inline
  // void
  // ndimage_rev_pixel_iterator<T,dim,I>::increment()
  // {
  //   int i = dim-1;
  //   while (true)
  //     {
  //       pixel_.point_[i] -= 1;
  //       pstack_[i] -= strides_[i];
  //       if (pixel_.point_[i] >= domain_.pmax[i] || i == 0)
  //         break;
  //       pixel_.point_[i] = domain_.pmax[i] - 1;
  //       --i;
  //     }
  //   if (i < dim-1)
  //     std::fill(pstack_ + i + 1, pstack_ + dim, pstack_[i]);
  //   pixel_.ptr_ = pstack_[i];
  // }

  // template <typename T, unsigned dim, typename I>
  // template <typename U, typename I2>
  // inline
  // bool
  // ndimage_rev_pixel_iterator<T,dim,I>::equal(const ndimage_rev_pixel_iterator<U, dim, I2>& other) const
  // {
  //   return this->pstack_[dim-1] == other.pstack_[dim-1];
  // }


  // template <typename T, unsigned dim, typename I>
  // inline
  // const typename ndimage_rev_pixel_iterator<T,dim,I>::pixel_type&
  // ndimage_rev_pixel_iterator<T,dim,I>::dereference() const
  // {
  //   return pixel_;
  // }



} // end of namespace mln

// Specialization of the boost::reverse_iterator
// Because increment and decrement have constant complexity but quite
// expensive compared to stl iterator.
namespace boost
{

  template <>
  template <typename T, unsigned dim, typename I>
  struct reverse_iterator< mln::ndimage_pixel_iterator<T, dim, I> >:
    mln::fast_reverse_iterator< mln::ndimage_pixel_iterator<T, dim, I> >
  {
  private:
    typedef mln::ndimage_pixel_iterator<T, dim, I> base_it;
    typedef mln::fast_reverse_iterator<base_it> base;
  public:
    reverse_iterator() = default;
    reverse_iterator(const reverse_iterator&) = default;
    reverse_iterator(base_it it) : base(it) {}
  };

}


#endif //!MLN_CORE_NDIMAGE_PIXEL_ITERATOR_HPP

