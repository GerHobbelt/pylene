#ifndef MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP
# define MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP

# include <boost/iterator/iterator_facade.hpp>
# include <iterator>

namespace mln {

  
  template <typename PointIterator, typename ValueIterator>
  struct zip_point_value_pixel;

  template <typename PointIterator, typename ValueIterator>
  struct pixel_iterator;

/******************************************/
/****          Implementation          ****/
/******************************************/


  template <typename PointIterator, typename ValueIterator>
  struct zip_point_value_pixel
  {
    typedef typename std::iterator_traits<PointIterator>::value_type point_type;
    typedef typename std::iterator_traits<ValueIterator>::value_type value_type;
    typedef typename std::iterator_traits<ValueIterator>::reference reference;

    zip_point_value_pixel(PointIterator pit, ValueIterator vit)
      : pit_ (pit), vit_(vit)
    {
    }

    point_type point() const { return *pit_; }
    reference val() const { return *vit_; }

    friend class pixel_iterator<PointIterator, ValueIterator>;
  private:
    PointIterator pit_;
    ValueIterator vit_;
  };

  template <typename SiteSetIterator, typename ValueIterator>
  struct pixel_iterator : boost::iterator_facade< pixel_iterator<SiteSetIterator, ValueIterator>,
                                           const zip_point_value_pixel<SiteSetIterator, ValueIterator>,
                                           typename std::common_type< typename boost::iterator_traversal<SiteSetIterator>::type,
                                                                      typename boost::iterator_traversal<SiteSetIterator>::type >::type >
  {
    typedef zip_point_value_pixel<SiteSetIterator, ValueIterator> pixel_t;

    pixel_iterator() = default;

    pixel_iterator(const pixel_iterator&) = default;

    //  Const - Mutable interopability
    // template <typename ValueIterator2>
    // pixel_iterator(const pixel_iterator<SiteSetIterator, ValueIterator2>& other)
    //   : pixel_ (other.pixel_.pit_, other.pixel_.vit_)
    // {
    // }

    pixel_iterator(SiteSetIterator pit, ValueIterator vit)
      : pixel_ (pit, vit)
    {
    }

  private:
    friend class boost::iterator_core_access;

    void increment() { pixel_.pit_++; pixel_.vit_++;  }

    // Two pixel iterators are equals iif its point iterator are equals 
    bool equal(const pixel_iterator& other) const
    {
        return pixel_.pit_ == other.pixel_.pit_;
    }

    const pixel_t& dereference() const { return pixel_; }


  private:
    pixel_t pixel_;
  };

} // end of namespace mln

#endif //!MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP
