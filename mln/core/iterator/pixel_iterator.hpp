#ifndef MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP
# define MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP

# include <boost/iterator/iterator_facade.hpp>
# include <iterator>

// FIXME: 

namespace mln {

  
  template <typename PointIterator, typename ValueIterator, typename Image>
  struct zip_point_value_pixel;

  template <typename PointIterator, typename ValueIterator, typename Image>
  struct pixel_iterator;

/******************************************/
/****          Implementation          ****/
/******************************************/


  template <typename PointIterator, typename ValueIterator, typename Image>
  struct zip_point_value_pixel
  {
    typedef typename std::iterator_traits<PointIterator>::value_type point_type;
    typedef typename std::iterator_traits<PointIterator>::value_type site_type;
    typedef typename std::iterator_traits<ValueIterator>::value_type value_type;
    typedef typename std::iterator_traits<ValueIterator>::reference  reference;
    typedef Image image_type;

    zip_point_value_pixel() = default;

    zip_point_value_pixel(const PointIterator& pit, const ValueIterator& vit, Image& ima)
      : pit_ (pit), vit_(vit), ima_ (&ima)
    {
    }

    point_type point() const { return *pit_; }
    reference val() const { return *vit_; }
    image_type& image() const { return *ima_; }


    friend class pixel_iterator<PointIterator, ValueIterator, Image>;
  private:
    PointIterator pit_;
    ValueIterator vit_;
    Image*        ima_;
  };

  template <typename SiteSetIterator, typename ValueIterator, typename Image>
  struct pixel_iterator : boost::iterator_facade< pixel_iterator<SiteSetIterator, ValueIterator, Image>,
                                                  const zip_point_value_pixel<SiteSetIterator, ValueIterator, Image>,
                                           typename std::common_type< typename boost::iterator_traversal<SiteSetIterator>::type,
                                                                      typename boost::iterator_traversal<SiteSetIterator>::type >::type >
  {
    typedef zip_point_value_pixel<SiteSetIterator, ValueIterator, Image> pixel_t;

    pixel_iterator() = default;

    pixel_iterator(const pixel_iterator&) = default;

    //  Const - Mutable interopability
    // template <typename ValueIterator2>
    // pixel_iterator(const pixel_iterator<SiteSetIterator, ValueIterator2>& other)
    //   : pixel_ (other.pixel_.pit_, other.pixel_.vit_)
    // {
    // }

    pixel_iterator(const SiteSetIterator& pit,
                   const ValueIterator& vit,
                   Image& ima)
      : pixel_ (pit, vit, ima)
    {
    }

  private:
    friend class boost::iterator_core_access;

    void increment() { pixel_.pit_++; pixel_.vit_++;  }

    // Two pixel iterators are equals iif its point iterator are equals 
    bool equal(const pixel_iterator& other) const
    {
        return pixel_.vit_ == other.pixel_.vit_;
    }

    const pixel_t& dereference() const { return pixel_; }


  private:
    pixel_t pixel_;
  };

} // end of namespace mln

#endif //!MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP
