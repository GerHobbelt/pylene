#pragma once


#include <mln/core/neighborhood/dyn_neighborhood.hpp> // [legacy]
#include <mln/core/point.hpp>
#include <mln/core/se/private/se_facade.hpp>

#include <range/v3/span.hpp>

#include <vector>
#include <initializer_list>

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
      using category = dynamic_neighborhood_tag;
      using separable = std::false_type;
      using decomposable = std::false_type;
      using incremental = std::false_type;

      /// \brief Initializer from a 2D array of values.
      /// \param values List of 0-1 to define the mask
      /// \exception std::runtime_error if the sizes of the list are not odd.
      mask2d(std::initializer_list<std::initializer_list<int>> values);

      ::ranges::span<const mln::point2d> offsets() const;

      /// \brief Returns the radial extent of the mask
      int radial_extent() const { return m_radial_extent; }

    private:
      std::vector<mln::point2d> m_points;
      int                       m_radial_extent;
    };
  } // namespace experimental
} // namespace mln
