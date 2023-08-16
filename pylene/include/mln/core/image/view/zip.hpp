#pragma once

#include <mln/core/assert.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/range/view/zip.hpp>

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
  class zip_view : public mln::details::Image<zip_view<Images...>>
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

    template <typename V>
    using ch_value_type = image_ch_value_t<I0, V>;
    /// \}

    static_assert(std::conjunction_v<std::is_same<image_point_t<Images>, point_type>...>,
                  "The point type of all images to be zipped must be the same.");


  public:
    /// Pixel type definitions
    /// \{
    struct pixel_type : mln::details::Pixel<pixel_type>
    {
      using reference                = zip_view::reference;
      using value_type               = zip_view::value_type;
      using site_type [[deprecated]] = zip_view::point_type;
      using point_type               = zip_view::point_type;

      pixel_type(image_pixel_t<Images>... pixels)
        : m_pixels{std::move(pixels)...}
      {
      }

      pixel_type(const pixel_type&)            = default;
      pixel_type(pixel_type&&)                 = default;
      pixel_type& operator=(const pixel_type&) = delete;
      pixel_type& operator=(pixel_type&&)      = delete;

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

    zip_view(const zip_view<Images...>&)                       = default;
    zip_view(zip_view<Images...>&&)                            = default;
    zip_view<Images...>& operator=(const zip_view<Images...>&) = delete;
    zip_view<Images...>& operator=(zip_view<Images...>&&)      = delete;

    auto domain() const { return std::get<0>(m_images).domain(); }

    decltype(auto) concretize() const { return std::get<0>(m_images).concretize(); }

    template <typename V>
    decltype(auto) ch_value() const
    {
      return std::get<0>(m_images).template ch_value<V>();
    }

    auto values()
    {
      auto g_new_values = [](auto&&... images) { return ranges::view::zip(images.values()...); };
      return std::apply(g_new_values, m_images);
    }

    auto pixels()
    {
      auto g_pixels = [](auto&&... images) {
        return ranges::view::zip_with(mln::details::make_object<pixel_type>{}, images.pixels()...);
      };
      return std::apply(g_pixels, m_images);
    }


    /// Accessible-image related methods
    /// \{
    reference operator()(point_type p) requires(accessible::value)
    {
      mln_precondition(all_domain_has(p));
      return this->at(p);
    }

    reference at(point_type p) requires(accessible::value)
    {
      auto g = [p](auto&&... images) -> reference { return std::forward_as_tuple(images.at(p)...); };
      return std::apply(g, m_images);
    }

    pixel_type pixel(point_type p) requires(accessible::value)
    {
      mln_precondition(all_domain_has(p));
      return this->pixel_at(p);
    }

    pixel_type pixel_at(point_type p) requires(accessible::value)
    {
      auto g = [p](auto&&... images) { return pixel_type(images.pixel_at(p)...); };
      return std::apply(g, m_images);
    }
    /// \}

  private:
    bool all_domain_has(point_type p) requires(accessible::value)
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
      static_assert(std::conjunction_v<is_a<Images, mln::details::Image>...>, "All zip arguments must be images.");

      return zip_view<Images...>(std::move(images)...);
    }
  } // namespace view

} // namespace mln
