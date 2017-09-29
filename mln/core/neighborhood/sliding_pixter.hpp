#ifndef MLN_CORE_NEIGHBORHOOD_SLIDING_PIXTER_HPP
# define MLN_CORE_NEIGHBORHOOD_SLIDING_PIXTER_HPP

# include <mln/core/range/range.hpp>
# include <mln/core/image/image.hpp>
# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/pixel/pointer_pixel.hpp>
# include <mln/core/pixel/point_pixel.hpp>
# include <mln/core/pixel/index_pixel.hpp>
# include <mln/core/utils/wrapper.hpp>
# include <boost/container/small_vector.hpp>

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

    template <typename PixelProxy>
    using spixter_image_t = typename unwrap_t<PixelProxy>::image_type;

    // Forward:
    // \tparam Pixel must be a pointer to a pixel or a pixel iterator.
    template <class PixelProxy, class SiteSet,
              class __is_indexable  = typename image_traits<internal::spixter_image_t<PixelProxy>>::indexable,
              class __category = typename image_traits<internal::spixter_image_t<PixelProxy>>::category>
    struct sliding_pixter_base;

    /********************************************************************/
    /****          Pixter Types : for accessible-only images         ****/
    /********************************************************************/

    template <class PixelProxy, class SiteSet, class __category>
    struct sliding_pixter_base<PixelProxy, SiteSet, std::false_type /* __is_indexable */, __category>
      : iterator_base<sliding_pixter<PixelProxy, SiteSet>,
                      point_pixel<spixter_image_t<PixelProxy>>,
                      point_pixel<spixter_image_t<PixelProxy>>>
    {
    private:
      using Image = spixter_image_t<PixelProxy>;

    public:
      sliding_pixter_base() = default;
      sliding_pixter_base(const PixelProxy& p, const SiteSet& s)
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
        return {m_pixel.get().image(), m_pixel.get().point() + *m_pset_iter};
      }

    private:
      PixelProxy                                   m_pixel;
      typename range_const_iterator<SiteSet>::type m_pset_iter;
    };

    /***************************************************************/
    /****          Pixter Types: Spe for indexable images       ****/
    /***************************************************************/

    template <class SiteSet>
    struct sliding_pixter_container_base
    {
    private:
      using Point          = typename SiteSet::value_type;
      using IndexContainer = boost::container::small_vector<int, 25>;
      using SiteContainer  = boost::container::small_vector<Point, 25>;

    public:
      sliding_pixter_container_base(const SiteSet& s)
        : m_size(rng::size(s)), m_site_set(m_size), m_index_set(m_size)
      {
        auto it = rng::iter(s);
        it.init();
	for (int i = 0; i < m_size; ++i, it.next())
          this->m_site_set[i] = *it;
      }

    protected:
      const int          m_size;
      SiteContainer      m_site_set;
      IndexContainer     m_index_set;
    };

    template <class Point, std::size_t N>
    struct sliding_pixter_container_base<std::array<Point, N>>
    {
    public:
      sliding_pixter_container_base(const std::array<Point, N>& s) : m_site_set(s) {}

    protected:
      using IndexContainer = std::array<int, N>;
      using SiteContainer  = std::array<Point, N>;


      static constexpr int m_size = N;
      const SiteContainer& m_site_set;
      IndexContainer       m_index_set;
    };


    template <class PixelProxy, class SiteSet>
    struct sliding_pixter_base<PixelProxy, SiteSet, std::true_type /* __is_indexable */, void>
      : sliding_pixter_container_base<SiteSet>
    {
    private:
      using Image = spixter_image_t<PixelProxy>;

    public:
      sliding_pixter_base() = default;

      sliding_pixter_base(const PixelProxy& px, const SiteSet& s)
	: sliding_pixter_container_base<SiteSet>(s),
          m_image (px.get().image()),
          m_pixel(px)
      {
        for (int i = 0; i < this->m_size; ++i)
          this->m_index_set[i] = m_image.delta_index(this->m_site_set[i]);
      }

      void init() { m_i = 0; }
      void next() { ++m_i; }
      bool finished() const { return m_i >= this->m_size; }

    protected:
      Image&             m_image;
      const PixelProxy   m_pixel;
      int                m_i;
    };


    // Specialization for indexable images NOT raw.
    // with a dynamic siteset
    template <class PixelProxy, class SiteSet, class __category>
    struct sliding_pixter_base<PixelProxy, SiteSet, std::true_type /* __is_indexable */, __category> :
      sliding_pixter_base<PixelProxy, SiteSet, std::true_type, void>,
      iterator_base< sliding_pixter<PixelProxy, SiteSet>,
                     index_pixel< spixter_image_t<PixelProxy> >,
                     index_pixel< spixter_image_t<PixelProxy> > >
    {
    private:
      using base = sliding_pixter_base<PixelProxy, SiteSet, std::true_type, void>;
      using Image = spixter_image_t<PixelProxy>;

    public:
      using base::base;

      index_pixel<Image>
      dereference() const
      {
        return {this->m_image,
            this->m_pixel.get().point() + this->m_site_set[this->m_i],
            this->m_pixel.get().index() + this->m_index_set[this->m_i] };
      }
    };


    // // Specialization for indexable images raw.
    // // with a dynamic siteset
    // template <class PixelProxy, class SiteSet>
    // struct sliding_pixter_base<PixelProxy, SiteSet, std::true_type /* __is_indexable */, raw_image_tag> :
    //   sliding_pixter_base<PixelProxy, SiteSet, std::true_type, void>,
    //   iterator_base< sliding_pixter<PixelProxy, SiteSet>,
    //                  pointer_pixel< spixter_image_t<PixelProxy> >,
    //                  pointer_pixel< spixter_image_t<PixelProxy> > >
    // {
    // private:
    //   using base = sliding_pixter_base<PixelProxy, SiteSet, std::true_type, void>;
    //   using Image = spixter_image_t<PixelProxy>;

    // public:
    //   using base::base;

    //   pointer_pixel<Image>
    //   dereference() const
    //   {
    //     auto index = this->m_index_set[this->m_i];
    //     return {this->m_pixel.get().image(),
    //             &(this->m_pixel.get().val()) + index,
    //             this->m_pixel.get().point() + this->m_site_set[this->m_i],
    //             this->m_pixel.get().index() + index};
    //   }
    // };

  }

  template <class PixelProxy, class SiteSet>
  struct sliding_pixter : internal::sliding_pixter_base<PixelProxy, SiteSet>
  {
  private:
    using Image       = internal::spixter_image_t<PixelProxy>;
    static_assert(image_traits<Image>::accessible::value or
		  image_traits<Image>::indexable::value,
		  "You cannot set a neighborhood on a pixel whose image is not"
		  "accessible <ima(p)> nor indexable <ima[i]>");

  public:
    sliding_pixter(const PixelProxy& px, const SiteSet& s)
      : internal::sliding_pixter_base<PixelProxy, SiteSet>(px, s)
    {
    }
  };

}

#endif // ! MLN_CORE_NEIGHBORHOOD_SLIDING_PIXTER_HPP
