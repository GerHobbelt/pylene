#ifndef MLN_CORE_ITERATOR_SLIDING_WIN_PIXTER_HPP
# define MLN_CORE_ITERATOR_SLIDING_WIN_PIXTER_HPP

# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/std/array.hpp>
# include <mln/core/wrt_offset.hpp>


namespace mln
{


  template <typename SiteSet, typename PixelOrPixelIterator>
  struct sliding_win_pixter;


  namespace internal
  {
    template <typename P, typename V, typename I>
    struct sliding_win_pixel;

    template <typename SiteSet, typename Pixel, typename image_tag, typename E>
    struct sliding_win_pixter_base;

    template <typename Point, size_t N, typename PixelOrPixelIterator, typename E>
    struct sliding_win_pixter_base< mln::array<Point, N>, PixelOrPixelIterator, raw_image_tag, E>;

    template <typename PixelOrPixelIterator, bool is_iterator = is_a<PixelOrPixelIterator, Iterator>::value >
    struct sliding_win_pixter_dispatch;
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal
  {

    template <typename P, typename V, typename I>
    struct sliding_win_pixel
    {
      typedef P point_type;
      typedef P site_type;
      typedef V value_type;
      typedef V& reference;
      typedef I image_type;

      sliding_win_pixel() = default;
      sliding_win_pixel(const sliding_win_pixel&) = default;

      // Non-const -> const conversion
      template <typename U, typename J>
      sliding_win_pixel(const sliding_win_pixel<P, U, J>& x,
                        typename std::enable_if< std::is_convertible<typename std::remove_reference<U>::type*,
                        typename std::remove_reference<V>::type*>::value >::type* dummy = NULL)
	: v_ (x.v_), p_ (x.p_), image_ (x.image_)
      {
      }


      V& val() const { return *v_; }
      const P& point() const { return p_; }
      const P& site() const { return p_; }
      image_type& image() const { return *image_; }

    private:
      template <typename, typename, typename, typename>
      friend struct sliding_win_pixter_base;

      V* v_;
      P  p_;
      I* image_;
    };

    template <typename PixelOrPixelIterator>
    struct sliding_win_pixter_dispatch<PixelOrPixelIterator, true>
    {
      typedef typename PixelOrPixelIterator::value_type Pixel;
      static typename PixelOrPixelIterator::reference getpixel(const PixelOrPixelIterator* x) { return x->dereference(); } ;
    };

    template <typename PixelOrPixelIterator>
    struct sliding_win_pixter_dispatch<PixelOrPixelIterator, false>
    {
      typedef PixelOrPixelIterator Pixel;
      static const Pixel& getpixel(const PixelOrPixelIterator* x) { return *x; } ;
    };

  }

  template <typename SiteSet, typename PixelOrPixelIterator>
  struct sliding_win_pixter :
    internal::sliding_win_pixter_base< SiteSet, PixelOrPixelIterator,
				       typename internal::sliding_win_pixter_dispatch<PixelOrPixelIterator>::Pixel::image_type::category,
				       sliding_win_pixter<SiteSet, PixelOrPixelIterator> >
  {
  private:
    typedef internal::sliding_win_pixter_base< SiteSet, PixelOrPixelIterator,
					       typename internal::sliding_win_pixter_dispatch<PixelOrPixelIterator>::Pixel::image_type::category,
                                               sliding_win_pixter<SiteSet, PixelOrPixelIterator> > base;

  public:
    sliding_win_pixter() = default;
    sliding_win_pixter(const SiteSet& domain, const PixelOrPixelIterator& pix)
      : base(domain, pix)
    {
    }

  };

  /******************************************/
  /****          Specialization          ****/
  /******************************************/

  namespace internal
  {


    template <typename Point, size_t N, typename PixelOrPixelIterator, typename E>
    struct sliding_win_pixter_base< mln::array<Point, N>, PixelOrPixelIterator, raw_image_tag, E>
      : iterator_base<E, const sliding_win_pixel<typename sliding_win_pixter_dispatch<PixelOrPixelIterator>::Pixel::point_type,
                                                 typename sliding_win_pixter_dispatch<PixelOrPixelIterator>::Pixel::value_type,
                                                 typename sliding_win_pixter_dispatch<PixelOrPixelIterator>::Pixel::image_type> >,
	protected sliding_win_pixter_dispatch<PixelOrPixelIterator>
    {
    private:
      typedef typename sliding_win_pixter_dispatch<PixelOrPixelIterator>::Pixel Pixel;
      typedef typename Pixel::point_type P;
      typedef typename Pixel::image_type I;
      typedef typename Pixel::value_type V;
      typedef typename I::difference_type difference_type;
      typedef sliding_win_pixel<P,V,I> pixel_t;

    public:
      sliding_win_pixter_base() = default;

      sliding_win_pixter_base(const mln::array<Point, N>& domain, const PixelOrPixelIterator& pix)
	: bind_ (&pix), domain_(&domain)
      {
	wrt_offset( this->getpixel(bind_).image(), domain, offset_);
      }

      void init() {
	i_ = 0;
	p_ = this->getpixel(bind_).point();
	ptr_ = (char*)(& this->getpixel(bind_).val());
	mypix_.p_ = p_ + (*domain_)[0];
	mypix_.v_ = (V*)(ptr_ + offset_[0]);
      }

      void next() {
	++i_;
	mypix_.p_ = p_ + (*domain_)[i_];
	mypix_.v_ = (V*)(ptr_ + offset_[i_]);
      }

      bool finished() const {
	return i_ == N;
      }

      const pixel_t&
      dereference() const {
	return mypix_;
      }

    private:
      const PixelOrPixelIterator* bind_;
      const mln::array<Point, N>* domain_;
      mln::array<difference_type, N> offset_;
      pixel_t mypix_;

      int i_;
      char* ptr_;
      Point p_;

    };



  }

} // end of namespace mln


#endif //!MLN_CORE_ITERATOR_SLIDING_WIN_PIXTER_HPP
