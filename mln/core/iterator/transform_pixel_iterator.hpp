#ifndef TRANSFORMED_PIXEL_ITERATOR_HPP
# define TRANSFORMED_PIXEL_ITERATOR_HPP

# include <type_traits>
# include <boost/iterator/iterator_facade.hpp>

namespace mln
{

  template <typename UnaryFunction, typename PixelIterator, typename Image>
  struct transformed_pixel;


  template <typename UnaryFunction, typename PixelIterator, typename Image>
  struct transformed_pixel_iterator;


  /***********************/
  /*** Implementation ****/
  /***********************/


  template <typename UnaryFunction, typename PixelIterator, typename Image>
  struct transformed_pixel
  {
  private:
    typedef typename std::iterator_traits<PixelIterator>::value_type	Pixel;

  public:
    typedef typename std::result_of< UnaryFunction(typename Pixel::reference) >::type        reference;
    typedef typename std::remove_cv<typename std::remove_reference<reference>::type>::type   value_type;
    typedef typename Pixel::point_type	point_type;
    typedef Image                       image_type;

    transformed_pixel() = default;

    transformed_pixel(const UnaryFunction& f, const PixelIterator& pix, Image* ima)
      : f_ (f),
	pixit_ (pix),
	ima_ (ima)
    {
    }

    template <typename PixelIterator2, typename Image2>
    transformed_pixel(const transformed_pixel<UnaryFunction, PixelIterator2, Image2>& other,
		      typename std::enable_if< std::is_convertible<PixelIterator2, PixelIterator>::value >::type* dummy1 = NULL,
		      typename std::enable_if< std::is_convertible<Image2*, Image*>::value >::type* dummy2 = NULL)
      : f_ (other.f_),
	pixit_ (other.pixit_),
	ima_ (other.ima_)
    {
    }


    point_type  point() const
    {
      return pixit_->point();
    }

    reference   val()   const
    {
      return f(pixit_->val());
    }

    image_type& image() const
    {
      mln_precondition(ima_ != NULL);
      return *ima_;
    }

    template <typename F, typename PixelIterator2, typename Image2>
    friend struct transformed_pixel;

    friend struct transformed_pixel_iterator<UnaryFunction, PixelIterator, Image>;


  private:
    UnaryFunction	f_;
    PixelIterator	pixit_;
    Image*		ima_;
  };


  template <typename UnaryFunction, typename PixelIterator, typename Image>
  struct transformed_pixel_iterator : boost::iterator_facade< transformed_pixel_iterator<UnaryFunction, PixelIterator, Image>,
							    const transformed_pixel<UnaryFunction, PixelIterator, Image>,
							    typename boost::iterator_traversal<PixelIterator>::type
							    >
  {
    transformed_pixel_iterator() = default;

    transformed_pixel_iterator(const UnaryFunction& f, const PixelIterator& pix, Image* ima)
      : pix_ (f, pix, ima)
    {
    }

    template <typename PixelIterator2, typename Image2>
    transformed_pixel_iterator(const transformed_pixel_iterator<UnaryFunction, PixelIterator2, Image2>& other,
			     typename std::enable_if< std::is_convertible<PixelIterator2, PixelIterator>::value >::type* dummy1 = NULL,
			     typename std::enable_if< std::is_convertible<Image2*, Image*>::value >::type* dummy2 = NULL)
      : pix_ (other.pix_)
    {
    }

  private:
    typedef transformed_pixel<UnaryFunction, PixelIterator, Image> Pixel;
    friend class boost::iterator_core_access;

    template <typename F, typename PixelIterator2, typename Image2>
    friend class transformed_pixel_iterator;

    void increment() { ++(pix_.pixit_); }
    void decrement() { --(pix_.pixit_); }
    const Pixel& dereference() const { return pix_; }

    bool equal(const transformed_pixel_iterator& other) const
    {
      return other.pixit_ == this->pixit_;
    }

  private:
    Pixel  pix_;
  };

}


#endif // ! TRANSFORMED_PIXEL_ITERATOR_HPP
