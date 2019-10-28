#pragma once

#include <mln/core/experimental/point.hpp>
#include <mln/core/se/private/se_facade.hpp>
#include <mln/core/private/weighted_point.hpp>

// [legacy]
#include <mln/core/point.hpp>
#include <mln/core/neighborhood/dyn_neighborhood.hpp>

#include <range/v3/span.hpp>

#include <initializer_list>
#include <vector>

/// \file

namespace mln::se
{

  class mask2d : public dyn_neighborhood_base<dynamic_neighborhood_tag, mask2d>
  {
  public:
    /// \brief Initializer from a 2D array of values
    mask2d(std::initializer_list<std::initializer_list<int>> values);

    const std::vector<mln::point2d>& offsets() const { return m_points; }

    /// \brief Returns the radial extent of the mask
    int radial_extent() const { return m_radial_extent; }

  private:
    std::vector<mln::point2d> m_points;
    int                       m_radial_extent;
  };


  namespace experimental
  {

    /// \brief Creates a 2D structuring element based on a mask. Every non-zero value will add the 2D-point in the
    /// set. The SE is assumed to be anchored at the center of the mask, thus the mask must have odd dimensions.
    ///
    /// \code
    /// mln::se::experimental::mask2d mask = {{0, 0, 1, 0, 0},
    ///                                       {1, 1, 1, 1, 1}
    ///                                       {0, 0, 1, 0, 0}};
    /// \endcode
    class mask2d : public se_facade<mask2d>
    {
    public:
      using category     = dynamic_neighborhood_tag;
      using separable    = std::false_type;
      using decomposable = std::false_type;
      using incremental  = std::false_type;

      /// \brief Initializer from a 2D array of values.
      /// \param values List of 0-1 to define the mask
      /// \exception std::runtime_error if the sizes of the list are not odd.
      mask2d(std::initializer_list<std::initializer_list<int>> values);

      [[gnu::pure]] ::ranges::span<const mln::experimental::point2d> offsets() const;

      /// \brief Returns the radial extent of the mask
      int radial_extent() const { return m_radial_extent; }

    private:
      std::vector<mln::experimental::point2d> m_points;
      int                                     m_radial_extent;
    };


    struct wmask2d_base
    {
      // Initialize from type-erased initializer list
      void init(void* values, void* zero, std::size_t sample_size, std::function<void(int x, int y, void*)> callback);

      int m_before_size   = 0;  // Number of points before (0,0)
      int m_after_size    = 0;  // Number of points after (0,0)
      int m_radial_extent = 0;
    };

    /// \brief Creates a 2D weighted structuring element based on a mask. Every non-zero value will add the 2D-point in the
    /// set. The SE is assumed to be anchored at the center of the mask, thus the mask must have odd dimensions.
    ///
    /// \code
    /// mln::se::experimental::mask2d mask = {{0,  0, 1,  0, 0},
    ///                                       {1, .5, 0, .5, 1}
    ///                                       {0,  0, 1,  0, 0}};
    /// \endcode

    template <class W>
    class wmask2d : private wmask2d_base, public wneighborhood_facade<wmask2d<W>>
    {
      using wpoint = mln::weighted_point<mln::experimental::point2d, W>;

    public:
      static_assert(std::is_arithmetic_v<W>);

      using category     = dynamic_neighborhood_tag;
      using separable    = std::false_type;
      using decomposable = std::false_type;
      using incremental  = std::false_type;

      wmask2d() = default;

      /// \brief Initializer from a 2D array of values.
      /// \param values List of 0-1 to define the mask
      /// \exception std::runtime_error if the sizes of the list are not odd.
      wmask2d(std::initializer_list<std::initializer_list<W>> values);

      [[gnu::pure]] ::ranges::span<const wpoint> offsets() const noexcept;
      [[gnu::pure]] ::ranges::span<const wpoint> before_offsets() const noexcept;
      [[gnu::pure]] ::ranges::span<const wpoint> after_offsets() const noexcept;

      /// \brief Returns the radial extent of the mask
      int radial_extent() const noexcept { return m_radial_extent; }

    private:
      std::vector<wpoint> m_points;
    };

    /******************************************/
    /****          Implementation          ****/
    /******************************************/


    template <class W>
    wmask2d<W>::wmask2d(std::initializer_list<std::initializer_list<W>> values)
    {
      W zero = 0;
      auto callback = [this](int x, int y, void* w) {
        this->m_points.push_back({{x, y}, *reinterpret_cast<W*>(w)});
      };
      init(reinterpret_cast<void*>(&values), &zero, sizeof(W), callback);
    }

    template <class W>
    auto wmask2d<W>::offsets() const noexcept -> ::ranges::span<const wpoint>
    {
      return ::ranges::make_span(m_points.data(), m_points.size());
    }

    template <class W>
    auto wmask2d<W>::before_offsets() const noexcept -> ::ranges::span<const wpoint>
    {
      return ::ranges::make_span(m_points.data(), m_before_size);
    }

    template <class W>
    auto wmask2d<W>::after_offsets() const noexcept -> ::ranges::span<const wpoint>
    {
      return ::ranges::make_span(m_points.data() + m_points.size() - m_after_size, m_after_size);
    }


  } // namespace experimental
} // namespace mln::se
