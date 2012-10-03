#ifndef MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP
# define MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP

# include <mln/core/iterator/iterator_base.hpp>

// FIXME: specialize for raw image

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
    typedef typename PointIterator::value_type point_type;
    typedef typename PointIterator::value_type site_type;
    typedef typename ValueIterator::value_type value_type;
    typedef typename ValueIterator::reference  reference;
    typedef Image			       image_type;

    zip_point_value_pixel() = default;

    zip_point_value_pixel(const PointIterator& pit, const ValueIterator& vit, Image& ima)
      : pit_ (pit), vit_(vit), ima_ (&ima)
    {
    }

    template <typename PointIterator2, typename ValueIterator2, typename Image2>
    zip_point_value_pixel(const zip_point_value_pixel<PointIterator2, ValueIterator2, Image2>& other,
			  typename std::enable_if<std::is_convertible<PointIterator2, PointIterator>::value and
			  std::is_convertible<ValueIterator2, ValueIterator>::value and
			  std::is_convertible<Image2*, Image*>::value>::type* = NULL)
      : pit_ (other.pit_), vit_ (other.vit_), ima_ (other.ima_)
    {
    }


    point_type point() const  { return *pit_; }
    point_type site() const  { return *pit_; }
    reference val() const     { return *vit_; }
    image_type& image() const { return *ima_; }


    friend class pixel_iterator<PointIterator, ValueIterator, Image>;

  private:
    template <typename, typename, typename>
    friend struct zip_point_value_pixel;

    PointIterator pit_;
    ValueIterator vit_;
    Image*        ima_;
  };

  template <typename SiteSetIterator, typename ValueIterator, typename Image>
  struct pixel_iterator
    : iterator_base< pixel_iterator<SiteSetIterator, ValueIterator, Image>,
		     const zip_point_value_pixel<SiteSetIterator, ValueIterator, Image> >
  {
    typedef zip_point_value_pixel<SiteSetIterator, ValueIterator, Image> pixel_t;

    pixel_iterator() = default;
    pixel_iterator(const pixel_iterator&) = default;

    template <typename SiteSetIterator2, typename ValueIterator2, typename Image2>
    pixel_iterator(const pixel_iterator<SiteSetIterator2, ValueIterator2, Image2>& other,
		   typename std::enable_if< std::is_convertible<typename pixel_iterator<SiteSetIterator2, ValueIterator2, Image2>::pixel_t,
								pixel_t>::value >::type* = NULL)
      : pixel_ (other.pixel_)
    {
    }

    pixel_iterator(const SiteSetIterator& pit,
                   const ValueIterator& vit,
                   Image& ima)
      : pixel_ (pit, vit, ima)
    {
    }


    void init() { pixel_.pit_.init(); pixel_.vit_.init(); }
    void next() { pixel_.pit_.next(); pixel_.vit_.next(); }
    bool finished() const { return pixel_.pit_.finished(); }
    const pixel_t& dereference() const { return pixel_; }


    // Two pixel iterators are equals iif its point iterator are equals
    // bool equal(const pixel_iterator& other) const
    // {
    //     return pixel_.vit_ == other.pixel_.vit_;
    // }

  private:
    template <typename, typename, typename>
    friend struct pixel_iterator;

    pixel_t pixel_;
  };

} // end of namespace mln

#endif //!MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP
