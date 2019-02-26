#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <range/v3/algorithm/fill.hpp>

#include <algorithm>


/// \file

namespace mln
{

  /// \brief \p fill assigns the value \p val to every element of the image \p
  /// ima.
  /// \ingroup Algorithms
  ///
  /// \param[out] output The output image.
  /// \param val The value to assign.
  ///
  /// \return The image.
  ///
  /// \tparam OutputImage is a model of the Writable Forward Image.
  /// \tparam Value must be convertible to Image's value type.
  /// \ingroup algorithms
  template <typename OutputImage, typename Value>
  [[deprecated]] OutputImage& fill(Image<OutputImage>& output, const Value& val);

  /// \overload
  /// \ingroup Algorithms
  template <typename OutputImage, typename Value>
  [[deprecated]] OutputImage&& fill(Image<OutputImage>&& output, const Value& val);

  namespace experimental
  {
    /// \brief Assigns the value \p val to every element of the image \p ima.
    ///
    /// \ingroup Algorithms
    ///
    /// \param[out] f The output image.
    /// \param v The value to assign.
    ///
    ///
    /// \tparam OutputImage is a model of the Writable Forward Image.
    /// \tparam Value must be convertible to Image's value type.
    template <class OutputImage, class Value>
    void fill(OutputImage f, const Value& v);
  } // namespace experimental

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {
    template <typename I, typename V>
    void fill(I& ima, const V& v)
    {
      mln_viter(pin, ima);
      mln_forall (pin)
        *pin = v;
    }
  } // namespace impl

  template <typename OutputImage, typename Value>
  OutputImage&& fill(Image<OutputImage>&& output_, const Value& val)
  {
    fill(output_, val);
    return move_exact(output_);
  }

  template <typename OutputImage, typename Value>
  OutputImage& fill(Image<OutputImage>& output_, const Value& val)
  {
    OutputImage& output = exact(output_);
    impl::fill(output, val);
    return output;
  }

  namespace experimental
  {
    template <class OutputImage, class Value>
    void fill(OutputImage f, const Value& v)
    {
      static_assert(mln::is_a<OutputImage, Image>());

      auto&& vals = f.new_values();
      for (auto row : ranges::rows(vals))
        ::ranges::fill(row, v);
    }
  } // namespace experimental

} // namespace mln
