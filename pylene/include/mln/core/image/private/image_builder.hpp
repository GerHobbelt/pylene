#pragma once
#include <mln/core/concept/new/se.hpp>
#include <mln/core/concept/neighborhood.hpp>
#include <mln/core/image/private/image_traits.hpp>

#include <any>



/// \file

namespace mln
{

  /// \brief Structure holding building parameters
  /// It is composed of two fields, a border value and
  /// an initialization value.
  struct image_build_params
  {
    int                 border = -1; ///< Border value (-1 for non requested)
    std::any            init_value;  ///< Initialization value (can be empty)
  };


  /// \brief Error code returned by \ref image_builder or \ref image_resizer
  enum image_build_error_code
  {
    IMAGE_BUILD_OK = 0,
    IMAGE_BUILD_NO_BORDER = 0x01,
    IMAGE_BUILD_UNSUPPORTED_SE = 0x02,
  };


  namespace details
  {
    struct image_builder_base
    {
      /// Override the initialization parameters with those from \p params.
      void set_params(const image_build_params& params);

      /// Override the initialization border parameter with \p border.
      void set_border(int border);

      /// Override the initialization border parameter with the radial extent of
      /// \p nbh.
      template <class SE>
      void adjust(const SE& nbh);

      /// Override the initialization value parameter with \p v.
      template <class T>
      void set_init_value(T&& v);

      /// Retrieve the error code in the variable pointed by \p err. If `build()` encounters an error, it will be set
      /// according to the error type.
      void get_status(image_build_error_code * err);

      image_build_params      m_params;
      image_build_error_code  m_status     = IMAGE_BUILD_OK;
      image_build_error_code* m_status_ptr = nullptr;
    };
  } // namespace details

  /// \brief Provides advanced image initialization facilities
  ///
  /// An image builder objects is created when calling the `concretize()` or `ch_value<T>()` method of images (or the
  /// corresponding free functions \ref mln::imconcretize and \ref mln::imchvalue() ).
  template <class I, class From>
  class image_builder : protected details::image_builder_base
  {
    static_assert(std::is_constructible_v<I, From, image_build_params>);
    using base = details::image_builder_base;

  public:

    /// \brief Create a builder initializing from \p from
    image_builder(const From& from);

    /// \brief Create a builder initializing from \p from, overrided by \p params.
    image_builder(const From& from, const image_build_params& params);

    // Prevent mis-use of the builder with
    // auto result = image_builder<I>(from)
    // {
    image_builder(const image_builder&) = delete;
    image_builder(image_builder&&) = delete;
    image_builder& operator=(const image_builder&)  = delete;
    image_builder& operator=(image_builder&&) = delete;
    // }

    /// Override the initialization parameters with those from \p params.
    image_builder& set_params(const image_build_params& params) { base::set_params(params); return *this; }

    /// Override the initialization border parameter with \p border.
    image_builder& set_border(int border)
    {
      if (!image_has_border<I>())
        m_status = IMAGE_BUILD_NO_BORDER;
      base::set_border(border); return *this;
    }

    /// Override the initialization border parameter to be *at least* the radial extent of
    /// \p nbh. If the radial extent is less than the current border initialization parameter, it has no effet.
    template <class SE>
    image_builder& adjust(const SE& nbh) { base::adjust(nbh); return *this; }

    /// Override the initialization value parameter with \p v.
    image_builder& set_init_value(image_value_t<I> v) { base::set_init_value(v); return *this; }

    /// Retrieve the error code in the variable pointed by \p err. If `build()` encounters an error, it will be set
    /// according to the error type.
    image_builder& get_status(image_build_error_code* err) { base::get_status(err); return *this; }

    /// Build and return the new image
    I build() const;

    /// Conversion operator. Alias for \ref build() .
    operator I() const { return this->build(); }

  private:
    const From& m_from;
  };

  /// \brief Provides advanced image resizing facilities
  ///
  /// See \ref image_builder.
  template <class To, class From>
  class image_resizer : protected details::image_builder_base
  {
    // TODO: static assert that "to.resize(from.domain(), image_build_params)" is well formed (and add a concept for that)
    using base = details::image_builder_base;

  public:

    /// \brief Create a builder initializing \p to from \p from
    image_resizer(To& to, const From& from);

    /// \brief Create a builder initializing \p to from \p from, overrided by \p params.
    image_resizer(To& to, const From& from, const image_build_params& params);

    // Prevent mis-use of the builder with
    // auto result = image_resizer<I>(from)
    // {
    image_resizer(const image_resizer&) = delete;
    image_resizer(image_resizer&&) = delete;
    image_resizer& operator=(const image_resizer&)  = delete;
    image_resizer& operator=(image_resizer&&) = delete;
    // }

    /// Override the initialization parameters with those from \p params.
    image_resizer& set_params(const image_build_params& params) { base::set_params(params); return *this; }

    /// Override the initialization border parameter with \p border.
    image_resizer& set_border(int border)
    {
      if (!image_has_border<To>())
        m_status = IMAGE_BUILD_NO_BORDER;
      base::set_border(border);
      return *this;
    }


    /// Override the initialization border parameter to be *at least* the radial extent of
    /// \p nbh. If the radial extent is less than the current border initialization parameter, it has no effet.
    template <class SE>
    image_resizer& adjust(const SE& nbh) { base::adjust(nbh); return *this; }

    /// Override the initialization value parameter with \p v.
    image_resizer& set_init_value(image_value_t<To> v) { base::set_init_value(v); return *this; }

    /// Retrieve the error code in the variable pointed by \p err. If `build()` encounters an error, it will be set
    /// according to the error type.
    image_resizer& get_status(image_build_error_code* err) { base::get_status(err); return *this; }

    /// Resize the image
    void resize() const;

    /// Resizing operator. Alias for \ref resize() .
    ~image_resizer() { this->resize(); }

  private:
    To&         m_to;
    const From& m_from;
  };


  /// \brief Return an image builder for concrete version of \p ref able to store values of type \p V.
  /// This is an alias for `ref.template ch_value<V>()`.
  template <typename V, class I>
  auto imchvalue(const I& ref)
  {
    static_assert(mln::is_a<I, Image>() || mln::is_a<I, experimental::Image>());
    return ref.template ch_value<V>();
  }

  /// \brief Return an image builder for a concrete version of \p ref. This is an alias for
  /// `ref.concretize()`.
  template <class I>
  auto imconcretize(const I& ref)
  {
    static_assert(mln::is_a<I, Image>() || mln::is_a<I, experimental::Image>());
    return ref.concretize();
  }

  /// \brief Returns an image builder to resize the image \p to based on parameters of \p from
  template <class To, class From>
  image_resizer<To, From> resize(To& to, const From& from)
  {
    static_assert(mln::is_a<To, Image>() || mln::is_a<To, experimental::Image>());
    static_assert(mln::is_a<From, Image>() || mln::is_a<From, experimental::Image>());
    return {to, from};
  }



  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {

    inline void image_builder_base::set_params(const image_build_params& params)
    {
      m_params = params;
    }

    inline void image_builder_base::set_border(int border)
    {
      m_params.border = border;
    }

    template <class T>
    void image_builder_base::set_init_value(T&& v)
    {
      m_params.init_value = std::forward<T>(v);
    }

    inline void image_builder_base::get_status(image_build_error_code* err)
    {
      m_status_ptr = err;
    }

    template <class SE>
    void image_builder_base::adjust(const SE& nbh)
    {
      static_assert(mln::is_a<SE, experimental::StructuringElement>() || mln::is_a<SE, mln::Neighborhood>());

      if constexpr (std::is_convertible_v<typename SE::category, dynamic_neighborhood_tag>)
      {
        int r = nbh.radial_extent();
        if (r > m_params.border)
          m_params.border = r;
      }
      else
      {
        m_status = IMAGE_BUILD_UNSUPPORTED_SE;
      }
    }
  } // namespace details


  template <class I, class From>
  image_builder<I, From>::image_builder(const From& from)
    : m_from{from}
  {
    if constexpr (mln::is_a<From, Image>())
      if constexpr (image_has_border<From>())
        m_params.border = from.border();
  }


  template <class I, class From>
  image_builder<I, From>::image_builder(const From& from, const image_build_params& params)
    : base{params}
    , m_from{from}
  {
    if constexpr (image_has_border<From>())
      m_params.border = from.border();
  }


  template <class I, class From>
  I image_builder<I, From>::build() const
  {
    if (m_status_ptr)
      *m_status_ptr = m_status;
    I output{m_from, m_params};

    // Reindexation (required as ndImage is not cleaned up)
    // It ensures that both image have the same starting index
    if constexpr (mln::is_a<I, mln::Image>::value && mln::is_a<From, mln::Image>::value)
      if constexpr (image_traits<I>::indexable::value && image_traits<From>::indexable::value)
      {
        auto&& domain = m_from.domain();
        auto   p      = domain.iter();
        p.init();
        output.reindex(m_from.index_of_point(*p));
      }

    return output;
  }


  template <class To, class From>
  image_resizer<To, From>::image_resizer(To& to, const From& from)
    : m_to{to}
    , m_from{from}
  {
    if constexpr (mln::is_a<From, Image>())
      if constexpr (image_has_border<From>())
        m_params.border = from.border();
  }


  template <class To, class From>
  image_resizer<To, From>::image_resizer(To& to, const From& from, const image_build_params& params)
    : base{params}
    , m_to{to}
    , m_from{from}
  {
    if constexpr (image_has_border<From>())
      m_params.border = from.border();
  }


  template <class To, class From>
  void image_resizer<To, From>::resize() const
  {
    if (m_status_ptr)
      *m_status_ptr = m_status;
    m_to.resize(m_from.domain(), m_params);

    // Reindexation (required as ndImage is not cleaned up)
    // It ensures that both image have the same starting index
    if constexpr (mln::is_a<To, mln::Image>::value && mln::is_a<From, mln::Image>::value)
      if constexpr (image_traits<To>::indexable::value && image_traits<From>::indexable::value)
      {
        auto&& domain = m_from.domain();
        auto   p      = domain.iter();
        p.init();
        m_to.reindex(m_from.index_of_point(*p));
      }
  }


}
