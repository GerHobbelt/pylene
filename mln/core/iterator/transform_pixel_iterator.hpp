#ifndef TRANSFORMED_PIXEL_ITERATOR_HPP
# define TRANSFORMED_PIXEL_ITERATOR_HPP

# include <type_traits>
# include <mln/core/iterator/iterator_base.hpp>

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
    typedef typename PixelIterator::value_type Pixel;

  public:
    typedef typename std::result_of<const UnaryFunction(typename Pixel::reference)>::type   reference;
    typedef typename std::remove_reference<reference>::type                   value_type;
    typedef Image                                                           image_type;
    typedef typename Pixel::point_type                                        point_type;
    typedef typename Pixel::site_type                                         site_type;

    transformed_pixel() = default;

    transformed_pixel(const UnaryFunction& fun, const PixelIterator& x, Image& ima)
      : x_ (x), fun_ (fun), ima_(&ima)
    {
    }

    template <typename PixelIterator2, typename Image2>
    transformed_pixel(const transformed_pixel<UnaryFunction, PixelIterator2, Image2>& other,
                      typename std::enable_if< std::is_convertible<PixelIterator2, PixelIterator>::value and
                      std::is_convertible<Image2*, Image*>::value >::type* = NULL)
      : x_ (other.x_), fun_ (other.fun_), ima_ (other.ima_)
    {
    }

    point_type point() const
    {
      return x_->point();
    }

    point_type site() const
    {
      return x_->site();
    }

    reference val() const
    {
      return fun_(x_->val());
    }

    image_type& image() const
    {
      return *ima_;
    }

  private:
    template <typename, typename, typename>
    friend class transform_pixel_iterator;

    template <typename, typename, typename>
    friend class transformed_pixel;

    PixelIterator x_;
    UnaryFunction fun_;
    Image* ima_;
  };


  template <typename UnaryFunction, typename PixelIterator, typename Image>
  struct transform_pixel_iterator
    : iterator_base< transform_pixel_iterator<UnaryFunction, PixelIterator, Image>,
                     const transformed_pixel<UnaryFunction, PixelIterator, Image> >
  {
  private:
    typedef transformed_pixel<UnaryFunction, PixelIterator, Image> pixel_t;

  public:
    transform_pixel_iterator() = default;

    transform_pixel_iterator(const UnaryFunction& fun, const PixelIterator& x, Image& ima)
      : px_(fun, x, ima)
    {
    }

    void init() { px_.x_.init(); }
    void next() { px_.x_.next(); }
    bool finished() const { return px_.x_.finished(); }

    const pixel_t& dereference() const { return px_; }

  private:
    pixel_t px_;
  };

}


#endif // ! TRANSFORMED_PIXEL_ITERATOR_HPP
