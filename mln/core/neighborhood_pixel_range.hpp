#ifndef MLN_CORE_NEIGHBORHOOD_PIXEL_RANGE_HPP
# define MLN_CORE_NEIGHBORHOOD_PIXEL_RANGE_HPP

# include <mln/core/image/sliding_image_iterator.hpp>
# include <mln/core/iterator/pixel_iterator.hpp>
# include <mln/core/domain/dtranslate.hpp>
# include <mln/core/wrt_offset.hpp>
# include <mln/core/range/adaptor/transformed.hpp>
# include <mln/core/std/array.hpp>

namespace mln
{

  namespace internal
  {
    /// \brief Generic case
    template <typename Pixel, typename SiteSet, typename image_tag>
    struct neighborhood_pixel_range_base;

    /// \brief Fast case if offset
    template <typename Pixel, typename Point, size_t N>
    struct neighborhood_pixel_range_base<Pixel, std::array<Point, N>, raw_image_tag>;
  }

  template <typename Pixel, typename SiteSet>
  struct neighborhood_pixel_range :
    internal::neighborhood_pixel_range_base<Pixel, SiteSet, typename image_traits<typename Pixel::image_type>::category>
  {
  private:
    typedef internal::neighborhood_pixel_range_base<Pixel, SiteSet, typename image_traits<typename Pixel::image_type>::category> base;

  public:
    neighborhood_pixel_range(const SiteSet& s, const Pixel& pix)
      : base(s, pix)
    {
    }
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace internal
  {
    /// \brief Generic case
    template <typename Pixel, typename SiteSet, typename image_tag>
    struct neighborhood_pixel_range_base;

    /// \brief Fast case if offset
    template <typename Pixel, typename Point, size_t N>
    struct neighborhood_pixel_range_base<Pixel, mln::array<Point, N>, raw_image_tag>;


    template <typename Pixel, typename SiteSet, typename image_tag>
    struct neighborhood_pixel_iterator;


    template <typename P, typename V, typename I>
    struct neighborhood_pixel
    {
      typedef P point_type;
      typedef P site_type;
      typedef V value_type;
      typedef V& reference;
      typedef I image_type;

      neighborhood_pixel() = default;
      neighborhood_pixel(const neighborhood_pixel&) = default;

      // Non-const -> const conversion
      template <typename U, typename J>
      neighborhood_pixel(const neighborhood_pixel<P, U, J>& x,
			 typename std::enable_if< std::is_convertible<U*, V*>::value >::type* dummy = NULL)
	: v_ (x.v_), p_ (x.p_), image_ (x.image_)
      {
      }


      V& val() const { return *v_; }
      const P& point() const { return p_; }
      const P& site() const { return p_; }
      image_type& image() const { return *image_; }

    private:
      template <typename, typename, typename>
      friend struct neighborhood_pixel_iterator;

      V* v_;
      P  p_;
      I* image_;
    };


    template  <typename Pixel, typename Point, size_t N>
    struct neighborhood_pixel_iterator<Pixel, mln::array<Point, N>, raw_image_tag>
      : iterator_base< neighborhood_pixel_iterator<Pixel, mln::array<Point, N>, raw_image_tag>,
		       const neighborhood_pixel<typename Pixel::point_type,
						typename Pixel::value_type,
						typename Pixel::image_type> >
    {
    private:
      typedef typename Pixel::image_type I;
      typedef typename I::difference_type difference_type;
      typedef typename Pixel::value_type V;

    public:
      neighborhood_pixel_iterator() = default;

      neighborhood_pixel_iterator(const mln::array<Point, N>& domain, const Pixel& pix)
	: bind_ (&pix), domain_(domain)
      {
	wrt_offset(pix.image(), domain_, offset_);
      }

      void init() {
	i_ = 0;
	p_ = bind_->point();
	ptr_ = (char*)(& bind_->val());
	mypix_.p_ = p_ + domain_[i_];
	mypix_.v_ = (V*)(ptr_ + offset_[i_]);
      }

      void next() {
	++i_;
	mypix_.p_ = p_ + domain_[i_];
	mypix_.v_ = (V*)(ptr_ + offset_[i_]);
      }

      bool finished() const {
	return i_ == N;
      }

      const neighborhood_pixel<typename Pixel::point_type, typename Pixel::value_type, I>&
      dereference() const {
	return mypix_;
      }

    private:
      const Pixel* bind_;
      mln::array<Point, N> domain_;
      mln::array<difference_type, N> offset_;
      neighborhood_pixel<typename Pixel::point_type, typename Pixel::value_type, I> mypix_;

      int i_;
      char* ptr_;
      Point p_;
    };



    template <typename Pixel, typename Point, size_t N>
    struct neighborhood_pixel_range_base<Pixel, mln::array<Point, N>, raw_image_tag>
    {

    public:
      typedef neighborhood_pixel_iterator<Pixel, mln::array<Point, N>, raw_image_tag> iterator;
      typedef iterator const_iterator;

      neighborhood_pixel_range_base(const mln::array<Point, N>& s, const Pixel& pix)
        : pix_ (pix), arr_(s)

      {
      }

      iterator iter() const
      {
        return iterator(arr_, pix_);
      }

    private:
      const Pixel&	pix_;
      const mln::array<Point, N>& arr_;
    };

  }

} // end of namespace mln


#endif //!MLN_CORE_NEIGHBORHOOD_PIXEL_RANGE_HPP
