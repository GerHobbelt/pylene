#pragma once

#include <mln/core/assert.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <tuple>
#include <type_traits>


namespace mln
{

  namespace details
  {
    template <class T>
    struct make_object
    {
      template <class... Args>
      T operator()(Args&&... args) const
      {
        return T{std::forward<Args>(args)...};
      }
    };
  } // namespace details

  template <class... Images>
  class zip_view : public experimental::Image<zip_view<Images...>>
  {
    using tuple_t         = std::tuple<Images...>;
    using I0              = typename std::tuple_element<0, tuple_t>::type;
    using common_category = std::common_type_t<image_category_t<Images>...>;

    tuple_t m_images;

  public:
    /// Type definitions
    /// \{
    using reference   = std::tuple<image_reference_t<Images>...>;
    using value_type  = std::tuple<image_value_t<Images>...>;
    using point_type  = std::common_type_t<image_point_t<Images>...>;
    using domain_type = std::common_type_t<image_domain_t<Images>...>;
    /// \}

    /// Traits & Image Properties
    /// \{
    using accessible         = std::conjunction<typename Images::accessible...>;
    using indexable          = std::false_type; // Preservative behavior
    using view               = std::true_type;
    using extension_category = mln::extension::none_extension_tag; // Preservative behavior
    // Zip doesn't preserve contiguity, so it decays from raw_image_tag
    using category_type = std::common_type_t<common_category, bidirectional_image_tag>;
    using concrete_type = image_ch_value_t<I0, typename I0::value_type>;

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <concepts::Value V>
#else
    template <typename V>
#endif
    using ch_value_type = image_ch_value_t<I0, V>;
    /// \}

    static_assert(std::conjunction_v<std::is_same<image_point_t<Images>, point_type>...>,
                  "The point type of all images to be zipped must be the same.");


  public:
    /// Pixel type definitions
    /// \{
    struct new_pixel_type : mln::experimental::Pixel<new_pixel_type>
    {
      using reference               = zip_view::reference;
      using value_type              = zip_view::value_type;
      using site_type[[deprecated]] = zip_view::point_type;
      using point_type              = zip_view::point_type;

      new_pixel_type(image_pixel_t<Images>... pixels)
        : m_pixels{std::move(pixels)...}
      {
      }

      new_pixel_type(const new_pixel_type&) = default;
      new_pixel_type(new_pixel_type&&)      = default;
      new_pixel_type& operator=(const new_pixel_type&) = delete;
      new_pixel_type& operator=(new_pixel_type&&) = delete;

      point_type point() const { return std::get<0>(m_pixels).point(); }
      reference  val() const
      {
        auto build_value = [](auto&&... pixels) -> reference { return {pixels.val()...}; };
        return std::apply(build_value, m_pixels);
      }
      void shift(point_type q)
      {
        auto g = [q](auto&&... pixels) { (pixels.shift(q), ...); };
        std::apply(g, m_pixels);
      }

    private:
      std::tuple<image_pixel_t<Images>...> m_pixels;
    };
    /// \}


    zip_view(Images... images)
      : m_images{std::move(images)...}
    {
    }

    zip_view(const zip_view<Images...>&) = default;
    zip_view(zip_view<Images...>&&)      = default;
    zip_view<Images...>& operator=(const zip_view<Images...>&) = delete;
    zip_view<Images...>& operator=(zip_view<Images...>&&) = delete;

    auto domain() const { return std::get<0>(m_images).domain(); }

    decltype(auto) concretize() const { return std::get<0>(m_images).concretize(); }

    template <typename V>
    decltype(auto) ch_value() const
    {
      return std::get<0>(m_images).template ch_value<V>();
    }

    auto new_values()
    {
      auto g_new_values = [](auto&&... images) { return ranges::view::zip(images.new_values()...); };
      return std::apply(g_new_values, m_images);
    }

    auto new_pixels()
    {
      auto g_new_pixels = [](auto&&... images) {
        return ranges::view::zip_with(mln::details::make_object<new_pixel_type>{}, images.new_pixels()...);
      };
      return std::apply(g_new_pixels, m_images);
    }


    /// Accessible-image related methods
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      mln_precondition(all_domain_has(p));
      return this->at(p);
    }

    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      auto g = [p](auto&&... images) { return std::forward_as_tuple(images.at(p)...); };
      return std::apply(g, m_images);
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel(point_type p)
    {
      mln_precondition(all_domain_has(p));
      return this->new_pixel_at(p);
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel_at(point_type p)
    {
      auto g = [p](auto&&... images) { return new_pixel_type(images.new_pixel_at(p)...); };
      return std::apply(g, m_images);
    }
    /// \}

  private:
    template <typename dummy = bool>
    std::enable_if_t<accessible::value, dummy> all_domain_has(point_type p)
    {
      auto g_has = [p](auto&&... images) { return (images.domain().has(p) && ...); };
      return std::apply(g_has, m_images);
    }
  };


  namespace view
  {
    template <class... Images>
    zip_view<Images...> zip(Images... images)
    {
      static_assert(std::conjunction_v<is_a<Images, experimental::Image>...>, "All zip arguments must be images.");

      return zip_view<Images...>(std::move(images)...);
    }
  } // namespace view

} // namespace mln
