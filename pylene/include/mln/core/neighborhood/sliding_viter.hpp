#ifndef MLN_CORE_NEIGHBORHOOD_SLIDING_VITER_HPP
#define MLN_CORE_NEIGHBORHOOD_SLIDING_VITER_HPP

#include <boost/container/small_vector.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/iterator/iterator_base.hpp>
#include <mln/core/range/range.hpp>
#include <mln/core/utils/wrapper.hpp>

namespace mln
{

  /// \brief Define a pixel iterator over a siteset centered on a pixel.
  ///
  /// Define an iterator over a siteset centered on a pixel.
  ///
  /// \p Pixel can be a pixel type, in that case, the iterator will
  /// be binded to the Pixel or a reference wrapper to it.
  /// \p Pixel can be an iterator, in that case, the current iterator will
  /// be binded to the iterator.
  ///
  /// Note that the siteset is not copied and
  /// its lifetime should be longer that the iterator's one.
  ///
  /// Internally, the iterator is optimized so that:
  /// * if I is indexable, it uses indexes
  /// * otherwise, it uses points.
  ///
  /// \tparam Point
  /// \tparam SiteSet
  /// \pre The pixel's image must be accessible or indexable.
  template <class Pixel, class SiteSet>
  struct sliding_viter;

  template <class Pixel, class SiteSet>
  sliding_viter<Pixel, SiteSet> make_sliding_viter(const Pixel& pixel, const SiteSet& pset)
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

    // Forward:
    // \tparam Pixel must be a pixel or a pixel iterator.
    template <class Pixel, class SiteSet, class Enable = void>
    struct sliding_viter_base;

    /********************************************************************/
    /****          Pixter Types : for accessible-only images         ****/
    /********************************************************************/

    template <class Pixel, class SiteSet>
    struct sliding_viter_base<Pixel, SiteSet,
                              std::enable_if_t<!image_traits<typename Image<Pixel>::image_type>::indexable::value>>
      : iterator_base<sliding_viter<Pixel, SiteSet>, typename unwrap_t<Pixel>::value_type,
                      typename unwrap_t<Pixel>::reference>
    {
    public:
      sliding_viter_base() = default;
      sliding_viter_base(const Pixel& p, const SiteSet& s)
        : m_pixel(p)
        , m_pset_iter(rng::iter(s))
      {
      }

      void init() { m_pset_iter.init(); }
      void next() { m_pset_iter.next(); }
      bool finished() const { return m_pset_iter.finished(); }

      typename unwrap_t<Pixel>::reference dereference() const
      {
        auto& f = unwrap(m_pixel).image();
        return f(unwrap(m_pixel).point() + *m_pset_iter);
      }

    private:
      Pixel                                        m_pixel;
      typename range_const_iterator<SiteSet>::type m_pset_iter;
    };

    /***************************************************************/
    /****          Pixter Types: Spe for indexable images       ****/
    /***************************************************************/

    // Specialization for indexable images
    template <class Pixel, class SiteSet>
    struct sliding_viter_base<Pixel, SiteSet,
                              std::enable_if_t<image_traits<typename Image<Pixel>::image_type>::indexable::value>>
      : iterator_base<sliding_viter<Pixel, SiteSet>, typename unwrap_t<Pixel>::value_type,
                      typename unwrap_t<Pixel>::reference>
    {
    private:
      using Container = boost::container::small_vector<int, 25>;

    public:
      sliding_viter_base() = default;
      sliding_viter_base(const Pixel& px, const SiteSet& s)
        : m_pixel(px)
        , m_size(rng::size(s))
        , m_index_set(m_size)
        , m_i(0)
      {
        Image<Pixel>& ima = px->image();
        mln_foreach (auto p, s)
          m_index_set.push_back(ima.delta_index(s[m_i]));
      }

      void init() { m_i = 0; }
      void next() { ++m_i; }
      bool finished() const { return m_i >= m_size; }

      typename Image<Pixel>::reference dereference() const
      {
        auto& f = unwrap(m_pixel).image();
        return f[m_index_set[m_i]];
      }

    private:
      const Pixel     m_pixel;
      const unsigned  m_size;
      const Container m_index_set;
      unsigned        m_i;
    };
  }

  template <class Pixel, class SiteSet>
  struct sliding_viter : internal::sliding_viter_base<Pixel, SiteSet>
  {
  private:
    typedef typename mln::iterator_traits<Pixel>::value_type::image_type I;
    static_assert(image_traits<I>::accessible::value or image_traits<I>::indexable::value,
                  "You cannot set a neighborhood on a pixel whose image is not"
                  "accessible <ima(p)> nor indexable <ima[i]>");

  public:
    sliding_viter(const Pixel& px, const SiteSet& s)
      : internal::sliding_viter_base<Pixel, SiteSet>(px, s)
    {
    }
  };
}

#endif // ! MLN_CORE_NEIGHBORHOOD_SLIDING_VITER_HPP
