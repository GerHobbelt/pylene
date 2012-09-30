#ifndef MLN_CORE_ITERATOR_SLIDING_WIN_PIXTER_HPP
# define MLN_CORE_ITERATOR_SLIDING_WIN_PIXTER_HPP

# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/std/array.hpp>
# include <mln/core/wrt_offset.hpp>


namespace mln
{


  template <typename SiteSet, typename Pixel>
  struct sliding_win_pixter;



  namespace internal
  {
    template <typename P, typename V, typename I>
    struct sliding_win_pixel;

    template <typename SiteSet, typename Pixel, typename image_tag, typename E>
    struct sliding_win_pixter_base;

    template <typename Point, size_t N, typename Pixel, typename E>
    struct sliding_win_pixter_base< mln::array<Point, N>, Pixel, raw_image_tag, E>;
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
			 typename std::enable_if< std::is_convertible<U*, V*>::value >::type* dummy = NULL)
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



  }

  template <typename SiteSet, typename Pixel>
  struct sliding_win_pixter :
    internal::sliding_win_pixter_base< SiteSet, Pixel, typename Pixel::image_type::category,
                             sliding_win_pixter<SiteSet, Pixel> >
  {
  private:
    typedef internal::sliding_win_pixter_base< SiteSet, Pixel, typename Pixel::image_type::category,
                                               sliding_win_pixter<SiteSet, Pixel> > base;

  public:
    sliding_win_pixter() = default;
    sliding_win_pixter(const SiteSet& domain, const Pixel& pixel)
      : base(domain, pixel)
    {
    }

  };

  /******************************************/
  /****          Specialization          ****/
  /******************************************/

  namespace internal
  {


    template <typename Point, size_t N, typename Pixel, typename E>
    struct sliding_win_pixter_base< mln::array<Point, N>, Pixel, raw_image_tag, E>
      : iterator_base<E, const sliding_win_pixel<typename Pixel::point_type,
                                                 typename Pixel::value_type,
                                                 typename Pixel::image_type> >
    {
    private:
      typedef typename Pixel::point_type P;
      typedef typename Pixel::image_type I;
      typedef typename Pixel::value_type V;
      typedef typename I::difference_type difference_type;
      typedef sliding_win_pixel<P,V,I> pixel_t;

    public:
      sliding_win_pixter_base() = default;

      sliding_win_pixter_base(const mln::array<Point, N>& domain, const Pixel& pix)
	: bind_ (&pix), domain_(&domain)
      {
	wrt_offset(pix.image(), domain, offset_);
      }

      void init() {
	i_ = 0;
	p_ = bind_->point();
	ptr_ = (char*)(& bind_->val());
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
      const Pixel* bind_;
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
