#ifndef MLN_CORE_NEIGHBORHOOD_SLIDING_PIXTER_HPP
# define MLN_CORE_NEIGHBORHOOD_SLIDING_PIXTER_HPP

# include <mln/core/range/range.hpp>
# include <mln/core/image/image.hpp>
# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/pixel/pointer_pixel.hpp>
# include <mln/core/pixel/point_pixel.hpp>
# include <mln/core/pixel/index_pixel.hpp>
# include <vector>

namespace mln
{


  /// \brief Define a pixel iterator over a siteset centered on a pixel.
  ///
  /// Define an iterator over a siteset centered on a pixel.
  ///
  /// \p Pixel can be a pointer type, in that case, the iterator will
  /// be binded to the Pixel.
  /// \p Pixel can be an itetaror, in that case, the current iterator will
  /// be binded to the iterator.
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


  /******************************************/
  /****          Facade                  ****/
  /******************************************/


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal
  {

    template <typename Px>
    using image_t = typename mln::iterator_traits<Px>::value_type::image_type;

    // Forward:
    // \tparam Pixel must be a pointer to a pixel or a pixel iterator.
    template <class Pixel,
	      class SiteSet,
	      class Enable = void>
    struct sliding_pixter_base;


    /********************************************************************/
    /****          Pixter Types : for accessible-only images         ****/
    /********************************************************************/

    template <class Pixel, class SiteSet>
    struct sliding_pixter_base<
      Pixel,
      SiteSet,
      typename std::enable_if< image_traits<image_t<Pixel>>::accessible::value and
			       !image_traits<image_t<Pixel>>::indexable::value
			       >::type>
    : iterator_base< sliding_pixter<Pixel, SiteSet>,
		     point_pixel< image_t<Pixel> >,
                     point_pixel< image_t<Pixel> >
		     >
    {
    private:
      using Image = image_t<Pixel>;

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
        return {m_pixel->image(), m_pixel->point() + *m_pset_iter};
      }

    private:
      typename std::conditional<std::is_pointer<Pixel>::value,
                                const Pixel, const Pixel&>::type m_pixel;
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
      typename std::enable_if< image_traits<image_t<Pixel>>::indexable::value and
			       !std::is_same<typename image_traits<image_t<Pixel>>::category,
					     raw_image_tag>::value>::type >
    : iterator_base< sliding_pixter<Pixel, std::array<P, N> >,
		     index_pixel< image_t<Pixel> >,
                     index_pixel< image_t<Pixel> > >
    {
    private:
      typedef image_t<Pixel> Image;
      typedef std::array<P, N> S;
      typedef std::array<typename Image::difference_type, N> C;

    public:
      sliding_pixter_base() = default;
      sliding_pixter_base(const Pixel& px, const S& s)
	: m_site_set(s),
          m_pixel(px),
          m_i (0)
      {
	Image& ima = px->image();
	for (unsigned i = 0; i < N; ++i)
	  m_index_set[i] = ima.delta_index(m_site_set[i]);
      }

      void init() { m_i = 0; }
      void next() { ++m_i; }
      bool finished() const { return m_i >= N; }

      index_pixel<Image>
      dereference() const
      {
	return {m_pixel->image(),
            m_pixel->point() + m_site_set[m_i],
            m_pixel->index() + m_index_set[m_i] };
      }

    private:
      S m_site_set;
      C m_index_set;
      typename std::conditional<std::is_pointer<Pixel>::value,
                                const Pixel, const Pixel&>::type m_pixel;
      unsigned m_i;
    };

    // Specialization for indexable images not raw.
    // with a dynamic siteset
    template <class Pixel, class SiteSet>
    struct sliding_pixter_base
    < Pixel,
      SiteSet,
      typename std::enable_if< image_traits<image_t<Pixel>>::indexable::value and
			       !std::is_same<typename image_traits<image_t<Pixel>>::category,
					     raw_image_tag>::value>::type >
    : iterator_base< sliding_pixter<Pixel, SiteSet>,
		     index_pixel< image_t<Pixel> >,
                     index_pixel< image_t<Pixel> > >
    {
    private:
      typedef image_t<Pixel> Image;
      typedef std::vector<typename image_t<Pixel>::difference_type> C;
    public:
      sliding_pixter_base() = default;

      sliding_pixter_base(const Pixel& px, const SiteSet& s)
	: m_size (rng::size(s)),
          m_site_set (s),
          m_index_set(m_size),
          m_pixel (px),
          m_i (0)
      {
	Image& ima = px->image();
	for (unsigned i = 0; i < m_size; ++i)
	  m_index_set[i] = ima.delta_index(m_site_set[m_i]);
      }

      void init() { m_i = 0; }
      void next() { ++m_i; }
      bool finished() const { return m_i >= m_size; }

      index_pixel<Image>
      dereference() const
      {
	return { m_pixel->image(),
            m_pixel->point() + m_site_set[m_i],
            m_pixel->index() + m_index_set[m_i] };
      }

    private:
      unsigned m_size;
      SiteSet m_site_set;
      C       m_index_set;
      typename std::conditional<std::is_pointer<Pixel>::value,
                                const Pixel, const Pixel&>::type m_pixel;
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
      typename std::enable_if< std::is_same<typename image_traits<image_t<Pixel>>::category,
					    raw_image_tag>::value>::type >
    : iterator_base< sliding_pixter<Pixel, SiteSet>,
                     pointer_pixel< image_t<Pixel> >,
                     pointer_pixel< image_t<Pixel> > >
    {
    private:
      typedef image_t<Pixel> Image;
      typedef std::vector<typename Image::difference_type> I;
      typedef std::vector<typename Image::difference_type> O;

    public:
      sliding_pixter_base() = default;

      sliding_pixter_base(const Pixel& px, const SiteSet& s)
	: m_size(rng::size(s)),
          m_site_set(s),
          m_index_set(m_size),
	  m_offset_set(m_size),
          m_pixel (px),
          m_i (0)
      {
	Image& ima = px->image();
	auto it = rng::iter(m_site_set);
	it.init();
	for (unsigned i = 0; i < m_size; ++i, it.next()) {
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
        char* current_ptr = static_cast<char*>(&(m_pixel->val())) + m_offset_set[m_i];
        return { m_pixel->image(),
            current_ptr,
            m_pixel->point() + m_site_set[m_i],
            m_pixel->index() + m_index_set[m_i]
            };
      }

    private:
      size_t            m_size;
      SiteSet           m_site_set;
      I                 m_index_set;
      O                 m_offset_set;
      typename std::conditional<std::is_pointer<Pixel>::value,
                                const Pixel, const Pixel&>::type m_pixel;
      unsigned          m_i;
    };


    template <class Pixel, std::size_t N, typename P>
    struct sliding_pixter_base
    < Pixel, std::array<P, N>,
		typename std::enable_if< std::is_same<typename image_traits< image_t<Pixel> >::category,
					    raw_image_tag>::value>::type >
    : iterator_base< sliding_pixter<Pixel, std::array<P, N> >,
		     pointer_pixel< image_t<Pixel> >,
		     pointer_pixel< image_t<Pixel> >
		     >
    {
    private:
      typedef image_t<Pixel> Image;
      typedef std::array<P, N> S;
      typedef std::array<typename Image::difference_type, N> I;
      typedef std::array<typename Image::difference_type, N> O;

    public:
      sliding_pixter_base() = default;

      sliding_pixter_base(const Pixel& px, const S& s)
	: m_site_set (s),
          m_pixel    (px)
      {
	Image& ima = px->image();
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
	return {m_pixel->image(),
            reinterpret_cast<char*>(&(m_pixel->val())) + m_offset_set[m_i],
            m_pixel->point() + m_site_set[m_i],
            m_pixel->index() + m_index_set[m_i]
            };
      }

    private:
      S              m_site_set;
      I              m_index_set;
      O              m_offset_set;
      typename std::conditional<std::is_pointer<Pixel>::value,
                                const Pixel, const Pixel&>::type m_pixel;
      unsigned       m_i;
    };
  }

  template <class Pixel, class SiteSet>
  struct sliding_pixter : internal::sliding_pixter_base<Pixel, SiteSet>
  {
  private:
    typedef typename mln::iterator_traits<Pixel>::value_type::image_type I;
    static_assert(image_traits<I>::accessible::value or
		  image_traits<I>::indexable::value,
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
