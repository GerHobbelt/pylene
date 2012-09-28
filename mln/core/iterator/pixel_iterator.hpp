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

    point_type point() const  { return *pit_; }
    reference val() const     { return *vit_; }
    image_type& image() const { return *ima_; }


    friend class pixel_iterator<PointIterator, ValueIterator, Image>;
  private:
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
    pixel_t pixel_;
  };

} // end of namespace mln

#endif //!MLN_CORE_ITERATOR_PIXEL_ITERATOR_HPP
