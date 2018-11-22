#pragma once

#include <mln/core/image/image.hpp>


namespace mln
{

  // Identity pixel adaptor
  template <class Pixel>
  struct pixel_adaptor
  {
    using point_type               = typename Pixel::point_type;
    using site_type [[deprecated]] = point_type;
    using value_type               = typename Pixel::value_type;
    using reference                = typename Pixel::reference;

    decltype(auto) val() const { return m_pix.val(); }
    auto           point() const { return m_pix.point(); }
    void           advance(point_type p) { m_pix.advance(p); }

    pixel_adaptor() = default;
    pixel_adaptor(Pixel px)
      : m_pix{std::move(px)}
    {
    }

  protected:
    Pixel&       base() { return m_pix; }
    const Pixel& base() const { return m_pix; }

  private:
    Pixel m_pix;
  };


  namespace details
  {
    template <class I, class = void>
    struct image_adaptor_base_indexable
    {
    };


    template <class I>
    struct image_adaptor_base_indexable<I, std::enable_if_t<I::indexable::value>>
    {
      using size_type       [[deprecated]] = typename I::size_type;
      using difference_type [[deprecated]] = typename I::difference_type;
    };

  }


  template <class I>
  struct image_adaptor : details::image_adaptor_base_indexable<I>
  {
  public:
    /// Type definitions
    /// \{
    using reference                      = typename I::reference;
    using value_type                     = typename I::value_type;
    using point_type                     = typename I::point_type;
    using domain_type                    = typename I::domain_type;
    /// \}

    /// Traits & Image Properties
    /// \{
    using accessible         = typename I::accessible;
    using indexable          = typename I::indexable;
    using extension_category = typename I::extension_category;
    using concrete_type      = typename I::concrete_type;

    template <class V>
    using ch_value_type      = typename I::template ch_value_type<V>;
    /// \}


    struct new_pixel_type : pixel_adaptor<typename I::new_pixel_type>
    {
      using pixel_adaptor<typename I::new_pixel_type>::pixel_adaptor;
    };


    image_adaptor() = default;
    image_adaptor(I ima)
      : m_ima(std::move(ima))
    {
    }

    auto domain() const { return m_ima.domain(); }
    auto new_values() { return m_ima.new_values(); }
    auto new_pixels() { return m_ima.new_pixels(); }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> operator()(point_type p)
    {
      return m_ima(p);
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> at(point_type p)
    {
      return m_ima.at(p);
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel(point_type p)
    {
      return m_ima.new_pixel(p);
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel_at(point_type p)
    {
      return m_ima.new_pixel_at(p);
    }

    /// Indexable-image related methods
    /// \{
    template <typename dummy = I>
    reference operator[](typename dummy::size_type i)
    {
      return m_ima[i];
    }

    template <typename dummy = I>
    std::enable_if_t<indexable::value, typename dummy::size_type> index_of_point(point_type p) const
    {
      return m_ima.index_of_point(p);
    }

    template <typename dummy = I>
    std::enable_if_t<indexable::value, typename dummy::difference_type> delta_index(point_type p) const
    {
      return m_ima.delta_index(p);
    }

    template <typename dummy = I>
    reference point_at_index(typename dummy::size_type i) const
    {
      return m_ima.point_at_index(i);
    }
    /// \}


    /// \}

  protected:
    I&       base() { return m_ima; }
    const I& base() const { return m_ima; }


  private:
    I m_ima;
  };

}
