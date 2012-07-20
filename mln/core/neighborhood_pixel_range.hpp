#ifndef MLN_CORE_NEIGHBORHOOD_PIXEL_RANGE_HPP
# define MLN_CORE_NEIGHBORHOOD_PIXEL_RANGE_HPP

# include <mln/core/image/sliding_image_iterator.hpp>
# include <mln/core/iterator/pixel_iterator.hpp>
# include <mln/core/domain/dtranslate.hpp>

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
    struct neighborhood_pixel_range_base<Pixel, std::array<Point, N>, raw_image_tag>;


    template <typename Pixel, typename SiteSet, typename image_tag>
    struct neighborhood_pixel_range_base
    {
    private:
      typedef typename Pixel::image_type image_t;
      typedef typename Pixel::point_type point_t;
      typedef translated_domain<SiteSet> domain_t;
      //typedef typename restricted_img_pixter<image_t, domain_t> iterator;

    public:
      neighborhood_pixel_range_base(const SiteSet& s, const Pixel& pix);
      //iterator begin() const;
      //iterator end() const;

    private:
      image_t&  image_;
      domain_t domain_;
    };

    template <typename Pixel, typename Point, size_t N>
    struct neighborhood_pixel_range_base<Pixel, std::array<Point, N>, raw_image_tag>
    {
    private:
      typedef typename Pixel::image_type image_t;
      typedef typename Pixel::point_type point_t;
      typedef translated_domain< std::array<Point, N> > domain_t;
      typedef typename domain_t::iterator piter;
      typedef sliding_image_value_iterator<image_t, std::array<Point, N> > value_iterator;
      typedef mln::pixel_iterator<piter, value_iterator, image_t> pixel_iterator;

    public:
      neighborhood_pixel_range_base(const std::array<Point, N>& s, const Pixel& pix)
        : pix_ (pix),
          domain_ (dtranslate(s, pix.point()))
      {
        wrt_offset(pix.image(), s, offsets_.begin());
      }

      pixel_iterator begin() const
      {
        return pixel_iterator(std::begin(domain_),
                              value_iterator(&pix_.val(), offsets_, 0),
                              pix_.image());
      }


      pixel_iterator end() const
      {
        return pixel_iterator(std::end(domain_),
                              value_iterator(&pix_.val(), offsets_, N),
                              pix_.image());
      }

      void center(const Pixel& p)
      {
        mln_precondition(&p.image() == &pix_.image());
        pix_ = p;
      }

    private:
      Pixel  pix_;
      domain_t  domain_;
      std::array<typename image_t::difference_type, N> offsets_;
    };




  }


} // end of namespace mln


#endif //!MLN_CORE_NEIGHBORHOOD_PIXEL_RANGE_HPP
