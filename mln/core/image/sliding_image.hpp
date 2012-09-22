#ifndef MLN_CORE_SLIDING_IMAGE_HPP
# define MLN_CORE_SLIDING_IMAGE_HPP

# include <mln/core/image_traits.hpp>
# include <mln/core/image_base.hpp>
# include <mln/core/iterator/pixel_iterator.hpp>
# include <mln/core/wrt_offset.hpp>
# include <mln/core/domain/dtranslate.hpp>
# include <array>

# include <mln/core/image/sliding_image_iterator.hpp>
#include <mln/core/point.hpp>
namespace mln {



  ///
  /// \brief A sliding image is an image reduced to a domain
  /// which is defined by a siteset surrounding a point
  /// Typical applications are:
  /// + Neigborhood image restriction implementation
  /// + Sliding Window image restriction implementation
  ///
  /// \tparam Image The image to be restricted
  /// \tparam SiteSet The set of points that defines the domain
  /// \pre Image must model a Forward Image
  template <typename Image, typename SiteSet>
  struct sliding_image;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal {
    template <typename E, typename Image, typename SiteSet, typename image_category>
    struct sliding_image_base;

  };

  template <typename Image, typename SiteSet>
  struct sliding_image : internal::sliding_image_base< sliding_image<Image, SiteSet>, Image, SiteSet, typename image_traits<Image>::category>
  {
    typedef internal::sliding_image_base<sliding_image<Image, SiteSet>, Image, SiteSet, typename image_traits<Image>::category> base;

    sliding_image(Image& ima, const SiteSet& dpoints)
      :  base(ima, dpoints)
    {
    }

    sliding_image()
      : base()
    {
    }


  };

  template <typename Image, typename SiteSet>
  struct image_traits< sliding_image<Image, SiteSet> >
  {
    typedef forward_image_tag   category;
    typedef std::false_type     accessible;
  };

  namespace internal
  {

    template <typename E, typename Image, size_t N>
    struct sliding_image_base<E, Image, std::array<point2d, N>, raw_image_tag>
      : mln::image_base< E,
                         typename Image::point_type, // point
                         typename Image::value_type, // value
                         typename std::conditional<std::is_const<Image>::value,
                                                   typename Image::const_pixel_type,
                                                   typename Image::pixel_type>::type, // pixel
                         typename Image::const_pixel_type, // const pixel
                         typename std::conditional<std::is_const<Image>::value,
                                                   typename Image::const_reference,
                                                   typename Image::reference>::type, // reference
                         typename Image::const_reference,
                         typename std::conditional<std::is_const<Image>::value,
                                                   typename Image::const_pointer,
                                                   typename Image::pointer>::type>
  {


  private:
    static const bool isconst = std::is_const<Image>::value;
    typedef std::array<typename Image::point_type, N> SiteSet;

  public:
    enum {ndim = Image::ndim};
    typedef typename translated_domain<SiteSet>::type   domain_type;
    typedef typename Image::point_type                  point_type;

    typedef typename std::conditional<isconst,
                                      typename Image::const_pixel_type,
                                      typename Image::pixel_type>::type pixel_type;


    typedef typename Image::size_type size_type;
    typedef typename Image::difference_type difference_type;

    typedef sliding_image_value_iterator<Image, SiteSet> value_iterator;
    typedef sliding_image_value_iterator<const Image, SiteSet> const_value_iterator;
    typedef mln::pixel_iterator<typename domain_type::iterator, value_iterator, E> pixel_iterator;
    typedef mln::pixel_iterator<typename domain_type::iterator, const_value_iterator, E> const_pixel_iterator;


    typedef boost::iterator_range<value_iterator> value_range;
    typedef boost::iterator_range<const_value_iterator> const_value_range;
    typedef boost::iterator_range<pixel_iterator> pixel_range;
    typedef boost::iterator_range<const_pixel_iterator> const_pixel_range;

    sliding_image_base()
      : ima_ (NULL)
    {
    }


    sliding_image_base(Image& ima, const SiteSet& dpoints)
      : ima_(&ima),
        dpoints_ (dpoints),
	//offsets_ (new std::array<typename Image::difference_type, N> ),
        pix_ (&ima)
    {
      //std::cout << "cstor" << std::endl;
      wrt_offset(ima, dpoints, offsets_.begin());

      //wrt_offset(ima, dpoints, const_cast<SiteSet>(offsets_).begin());
    }

    ~sliding_image_base()
    {
      //std::cout << "dstor" << std::endl;
    }


    domain_type domain() const
    {
      return dtranslate(dpoints_, pix_.point());
    }


    value_range values()
    {
      return value_range(value_iterator(&pix_.val(), offsets_, 0),
                         value_iterator(&pix_.val(), offsets_, N));
    }

    const_value_range values() const
    {
      return const_value_range(const_value_iterator(&pix_.val(), offsets_, 0),
                               const_value_iterator(&pix_.val(), offsets_, N));
    }

    pixel_range pixels()
    {
      domain_type prange = this->domain();
      return pixel_range(pixel_iterator(std::begin(prange),
					value_iterator(&pix_.val(), offsets_, 0),
					*reinterpret_cast<E*>(this)),
			 pixel_iterator(std::end(prange),
					value_iterator(&pix_.val(), offsets_, N),
					*reinterpret_cast<E*>(this) ));
    }

    const_pixel_range pixels() const
    {
      domain_type prange = this->domain();
      return const_pixel_range(const_pixel_iterator(std::begin(prange),
						    const_value_iterator(&pix_.val(), offsets_, 0),
						    exact(*this)),
			       const_pixel_iterator(std::end(prange),
						    const_value_iterator(&pix_.val(), offsets_, N),
						    exact(*this) ));
    }


    void center(const pixel_type& p)
    {
      mln_precondition(&p.image() == ima_);
      pix_ = p;
    }

    void center(point_type p)
    {
      pix_ = ima_->pix_at(p);
    }

    Image* image() const {
      return ima_;
    }

  private:
    Image* const				   ima_;
    const SiteSet&				   dpoints_;
    //std::shared_ptr< std::array<typename Image::difference_type, N> > offsets_;
    std::array<typename Image::difference_type, N> offsets_;
    pixel_type					   pix_;
  };

    //template <typename Image, size_t N>
    //int sliding_image_base<Image, std::array<point2d, N>, raw_image_tag>::cnt = 0;


  } // end of namespace internal

} // end of namespace mln

#endif //!MLN_CORE_SLIDING_IMAGE_HPP
