#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <range/v3/algorithm/fill.hpp>

#include <algorithm>


/// \file

namespace mln
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

  /******************************************/
  /****          Implementation          ****/
  /******************************************/
  template <class OutputImage, class Value>
  void fill(OutputImage f, const Value& v)
  {
    static_assert(mln::is_a<OutputImage, experimental::Image>());

    auto&& vals = f.new_values();
    for (auto row : ranges::rows(vals))
      ::ranges::fill(row, v);
  }

} // namespace mln
