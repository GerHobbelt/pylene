#ifndef MLN_CORE_NEIGHBORHOOD_SLIDING_PIXTER_HPP
# define MLN_CORE_NEIGHBORHOOD_SLIDING_PIXTER_HPP

# include <mln/core/range/range.hpp>
# include <mln/core/image/image.hpp>
# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/pixel/pointer_pixel.hpp>
# include <mln/core/pixel/point_pixel.hpp>
# include <mln/core/pixel/index_pixel.hpp>
# include <vector>
# include <functional>

namespace mln
{


  /// \brief Define a pixel iterator over a siteset centered on a pixel.
  ///
  /// Define an iterator over a siteset centered on a pixel.
  ///
  /// \p Pixel can be anything convertible to a pixel, e.g.,
  /// * a pixel
  /// * a reference wrapper to a pixel
  /// * a proxy iterator to a pixel iterator
  ///
  /// Note that the siteset is not copied and
  /// its lifetime should be longer that the iterator's one.
  ///
  /// Internally, the iterator is optimized so that:
  /// * if I is raw, it uses pointers
  /// * if I is indexable, it uses indexes
  /// * otherwise, it uses points.
  ///
  /// \tparam Point
  /// \tparam SiteSet
  /// \pre The pixel's image must be accessible or indexable.
  template <class Pixel, class SiteSet>
  struct sliding_pixter;

  template <class Pixel, class SiteSet>
  sliding_pixter<Pixel, SiteSet>
  make_sliding_pixter(const Pixel& pixel, const SiteSet& pset)
  {
    return {pixel, pset};
  }

}

  /******************************************/
  /****          Facade                  ****/
  /******************************************/


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

// Fwd declaration
namespace mln {
  template <class T>
  struct iterator_proxy_wrapper;
}

namespace mln
{

  namespace internal
  {

    // Forward:
    // \tparam Pixel must be a pointer to a pixel or a pixel iterator.
    template <class Pixel,
	      class SiteSet,
	      class Enable = void>
    struct sliding_pixter_base;


    template <class Pixel>
    struct sliding_pixter_helper
    {
      using pixel_t = Pixel;
      using pixel_ref_t = const Pixel&;
      using image_t = typename Pixel::image_type;
    };

    template <class Pixel>
    struct sliding_pixter_helper< std::reference_wrapper<Pixel> >
    {
      using pixel_t = Pixel;
      using pixel_ref_t = const Pixel&;
      using image_t = typename Pixel::image_type;
    };

    template <class PixelIterator>
    struct sliding_pixter_helper< iterator_proxy_wrapper<PixelIterator> >
    {
      using pixel_t = typename mln::iterator_traits<PixelIterator>::value_type;
      using pixel_ref_t = typename mln::iterator_traits<const PixelIterator>::reference;
      using image_t = typename pixel_t::image_type;
    };

    template <typename Px>
    using spixter_image_t = typename sliding_pixter_helper<Px>::image_t;



    /********************************************************************/
    /****          Pixter Types : for accessible-only images         ****/
    /********************************************************************/

    template <class Pixel, class SiteSet>
    struct sliding_pixter_base<
      Pixel,
      SiteSet,
      typename std::enable_if< image_traits<typename sliding_pixter_helper<Pixel>::image_t>::accessible::value and
			       !image_traits<typename sliding_pixter_helper<Pixel>::image_t>::indexable::value
			       >::type>
    : iterator_base< sliding_pixter<Pixel, SiteSet>,
		     point_pixel< typename sliding_pixter_helper<Pixel>::image_t >,
                     point_pixel< typename sliding_pixter_helper<Pixel>::image_t >
		     >
    {
    private:
      using Image       = typename sliding_pixter_helper<Pixel>::image_t;
      using pixel_ref_t = typename sliding_pixter_helper<Pixel>::pixel_ref_t;
      using pixel_t     = typename sliding_pixter_helper<Pixel>::pixel_t;

    public:
      sliding_pixter_base() = default;
      sliding_pixter_base(const Pixel& p, const SiteSet& s)
	: m_pixel (p),
          m_pset_iter( rng::iter(s) )
      {
      }

      void init() { m_pset_iter.init(); }
      void next() { m_pset_iter.next(); }
      bool finished() const { return m_pset_iter.finished(); }

      point_pixel<Image>
      dereference() const
      {
        return {m_pix().image(), m_pix().point() + *m_pset_iter};
      }

    private:
      pixel_ref_t m_pix() const { return m_pixel; }

      Pixel m_pixel;
      typename range_const_iterator<SiteSet>::type m_pset_iter;
    };

    /***************************************************************/
    /****          Pixter Types: Spe for indexable images       ****/
    /***************************************************************/


    // Specialization for indexable images not raw.
    // with a static siteset
    template <class Pixel, std::size_t N, typename P>
    struct sliding_pixter_base
    < Pixel,
      std::array<P, N>,
      typename std::enable_if< image_traits<typename sliding_pixter_helper<Pixel>::image_t>::indexable::value and
			       !std::is_same<typename image_traits<typename sliding_pixter_helper<Pixel>::image_t>::category,
					     raw_image_tag>::value>::type >
    : iterator_base< sliding_pixter<Pixel, std::array<P, N> >,
		     index_pixel<typename sliding_pixter_helper<Pixel>::image_t>,
                     index_pixel<typename sliding_pixter_helper<Pixel>::image_t> >
    {
    private:
      using Image       = typename sliding_pixter_helper<Pixel>::image_t;
      using pixel_ref_t = typename sliding_pixter_helper<Pixel>::pixel_ref_t;
      using pixel_t     = typename sliding_pixter_helper<Pixel>::pixel_t;
      typedef std::array<P, N> S;
      typedef std::array<typename Image::difference_type, N> C;

    public:
      sliding_pixter_base() = default;
      sliding_pixter_base(const Pixel& px, const S& s)
	: m_site_set(s),
          m_pixel(px),
          m_i (0)
      {
	Image& ima = m_pix().image();
	for (unsigned i = 0; i < N; ++i)
	  m_index_set[i] = ima.delta_index(m_site_set[i]);
      }

      void init() { m_i = 0; }
      void next() { ++m_i; }
      bool finished() const { return m_i >= N; }

      index_pixel<Image>
      dereference() const
      {
	return {m_pix().image(),
            m_pix().point() + m_site_set[m_i],
            m_pix().index() + m_index_set[m_i] };
      }

    private:
      pixel_ref_t m_pix() const { return m_pixel; }

      S         m_site_set;
      C         m_index_set;
      Pixel     m_pixel;
      unsigned  m_i;
    };

    // Specialization for indexable images not raw.
    // with a dynamic siteset
    template <class Pixel, class SiteSet>
    struct sliding_pixter_base
    < Pixel,
      SiteSet,
      typename std::enable_if< image_traits<typename sliding_pixter_helper<Pixel>::image_t>::indexable::value and
			       !std::is_same<typename image_traits<typename sliding_pixter_helper<Pixel>::image_t>::category,
					     raw_image_tag>::value>::type >
    : iterator_base< sliding_pixter<Pixel, SiteSet>,
		     index_pixel< typename sliding_pixter_helper<Pixel>::image_t >,
                     index_pixel< typename sliding_pixter_helper<Pixel>::image_t > >
    {
    private:
      using Image       = typename sliding_pixter_helper<Pixel>::image_t;
      using pixel_ref_t = typename sliding_pixter_helper<Pixel>::pixel_ref_t;
      using pixel_t     = typename sliding_pixter_helper<Pixel>::pixel_t;
      typedef std::vector<typename range_value<SiteSet>::type > S;
      typedef std::vector<typename Image::difference_type> C;
    public:
      sliding_pixter_base() = default;

      sliding_pixter_base(const Pixel& px, const SiteSet& s)
	: m_size (rng::size(s)),
          m_site_set (m_size),
          m_index_set(m_size),
          m_pixel (px),
          m_i (0)
      {
	Image& ima = m_pix().image();
        auto it = rng::iter(s);
        it.init();
	for (unsigned i = 0; i < m_size; ++i, it.next()) {
          m_site_set[i] = *it;
	  m_index_set[i] = ima.delta_index(m_site_set[i]);
        }
      }

      void init() { m_i = 0; }
      void next() { ++m_i; }
      bool finished() const { return m_i >= m_size; }

      index_pixel<Image>
      dereference() const
      {
	return { m_pix().image(),
            m_pix().point() + m_site_set[m_i],
            m_pix().index() + m_index_set[m_i] };
      }

    private:
      pixel_ref_t m_pix() const { return m_pixel; }

      unsigned m_size;
      S       m_site_set;
      C       m_index_set;
      Pixel   m_pixel;
      unsigned m_i;
    };

    /***************************************************************/
    /****          Pixter Types: Spe for Raw images             ****/
    /***************************************************************/


    // Specialization for raw images.
    // with a static siteset
    template <class Pixel, class SiteSet>
    struct sliding_pixter_base
    < Pixel, SiteSet,
      typename std::enable_if< std::is_same<typename image_traits<typename sliding_pixter_helper<Pixel>::image_t>::category,
					    raw_image_tag>::value>::type >
    : iterator_base< sliding_pixter<Pixel, SiteSet>,
                     pointer_pixel< typename sliding_pixter_helper<Pixel>::image_t >,
                     pointer_pixel< typename sliding_pixter_helper<Pixel>::image_t > >
    {
    private:
      using Image       = typename sliding_pixter_helper<Pixel>::image_t;
      using pixel_ref_t = typename sliding_pixter_helper<Pixel>::pixel_ref_t;
      using pixel_t     = typename sliding_pixter_helper<Pixel>::pixel_t;
      typedef std::vector<typename range_value<SiteSet>::type > S;
      typedef std::vector<typename Image::difference_type> I;
      typedef std::vector<typename Image::difference_type> O;

    public:
      sliding_pixter_base() = default;

      sliding_pixter_base(const Pixel& px, const SiteSet& s)
	: m_size(rng::size(s)),
          m_site_set(m_size),
          m_index_set(m_size),
	  m_offset_set(m_size),
          m_pixel (px),
          m_i (0)
      {
	Image& ima = m_pix().image();
	auto it = rng::iter(s);
	it.init();
	for (unsigned i = 0; i < m_size; ++i, it.next()) {
          m_site_set[i] = *it;
	  m_index_set[i] = ima.delta_index(*it);
	  m_offset_set[i] = ima.delta_offset(*it);
	}
      }

      void init() { m_i = 0; }
      void next() { ++m_i; }
      bool finished() const { return m_i >= m_size; }

      pointer_pixel<Image>
      dereference() const
      {
        char* current_ptr = (char*)(&(m_pix().val())) + m_offset_set[m_i];
        return { m_pix().image(),
            current_ptr,
            m_pix().point() + m_site_set[m_i],
            m_pix().index() + m_index_set[m_i]
            };
      }

    private:
      pixel_ref_t m_pix() const { return m_pixel; }

      size_t            m_size;
      S                 m_site_set;
      I                 m_index_set;
      O                 m_offset_set;
      Pixel             m_pixel;
      unsigned          m_i;
    };


    template <class Pixel, std::size_t N, typename P>
    struct sliding_pixter_base
    < Pixel, std::array<P, N>,
		typename std::enable_if< std::is_same<typename image_traits< typename sliding_pixter_helper<Pixel>::image_t >::category,
					    raw_image_tag>::value>::type >
    : iterator_base< sliding_pixter<Pixel, std::array<P, N> >,
		     pointer_pixel< typename sliding_pixter_helper<Pixel>::image_t >,
		     pointer_pixel< typename sliding_pixter_helper<Pixel>::image_t >
		     >
    {
    private:
      using Image       = typename sliding_pixter_helper<Pixel>::image_t;
      using pixel_ref_t = typename sliding_pixter_helper<Pixel>::pixel_ref_t;
      using pixel_t     = typename sliding_pixter_helper<Pixel>::pixel_t;
      typedef std::array<P, N> S;
      typedef std::array<typename Image::difference_type, N> I;
      typedef std::array<typename Image::difference_type, N> O;

    public:
      sliding_pixter_base() = default;

      sliding_pixter_base(const Pixel& px, const S& s)
	: m_site_set (s),
          m_pixel    (px)
      {
	Image& ima = m_pix().image();
	for (unsigned i = 0; i < N; ++i) {
	  m_index_set[i] = ima.delta_index( m_site_set[i]);
	  m_offset_set[i] = ima.delta_offset( m_site_set[i]);
	}

      }

      void init() { m_i = 0; }
      void next() { ++m_i; }

      bool finished() const { return m_i >= N; }

      pointer_pixel<Image>
      dereference() const
      {
        mln_precondition(m_i < N);
	return {m_pix().image(),
            (char*)(&(m_pix().val())) + m_offset_set[m_i],
            m_pix().point() + m_site_set[m_i],
            m_pix().index() + m_index_set[m_i]
            };
      }

    private:
      pixel_ref_t m_pix() const { return m_pixel; }

      S              m_site_set;
      I              m_index_set;
      O              m_offset_set;
      Pixel          m_pixel;
      unsigned       m_i;
    };
  }

  template <class Pixel, class SiteSet>
  struct sliding_pixter : internal::sliding_pixter_base<Pixel, SiteSet>
  {
  private:
    using Image       = typename internal::sliding_pixter_helper<Pixel>::image_t;
    static_assert(image_traits<Image>::accessible::value or
		  image_traits<Image>::indexable::value,
		  "You cannot set a neighborhood on a pixel whose image is not"
		  "accessible <ima(p)> nor indexable <ima[i]>");

  public:
    sliding_pixter(const Pixel& px, const SiteSet& s)
      : internal::sliding_pixter_base<Pixel, SiteSet>(px, s)
    {
    }
  };

}

#endif // ! MLN_CORE_NEIGHBORHOOD_SLIDING_PIXTER_HPP
