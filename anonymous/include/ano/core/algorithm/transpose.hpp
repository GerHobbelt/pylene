#pragma once
#include <ano/core/algorithm/copy.hpp>
#include <ano/core/assert.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/range/rows.hpp>
#include <ano/core/range/view/zip.hpp>
#include <ano/core/trace.hpp>


#include <range/v3/algorithm/transform.hpp>
#include <range/v3/functional/concepts.hpp>
#include <range/v3/utility/functional.hpp>
/// \file

namespace ano
{

  /// \ingroup Algorithms
  /// \brief Transpose an image. The transposition is done on the last contiguous rows traversed in the 2nd inner loop
  ///
  ///
  /// \tparam InputImage A model of :concept:ref:`forward_image`
  /// \tparam OutputImage A model of Writable :concept:ref:`forward_image`
  /// \param input The input image.
  /// \param output The output image.

  template <class InputImage, class OutputImage>
  void transpose(InputImage in, OutputImage out);

  template <class InputImage>
  image_ch_value_t<InputImage, image_value_t<InputImage>> transpose(InputImage in);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class InputImage, class OutputImage>
  void transpose(InputImage in, OutputImage out)
  {

    auto&& ivals      = in.values();
    auto&& ivals_rows = ano::ranges::rows(ivals);
    using row_type    = ::ranges::range_value_t<std::decay_t<decltype(ivals_rows)>>;
    std::vector<row_type> vec_rows{};
    for (auto&& row : ivals_rows)
    {
      vec_rows.push_back(row);
    }

    {
      int size    = vec_rows.size();
      int num_row = 0;

      // empty, do nothing
      if (size == 0)
        return;
      // one dimension, nothing to transpose
      else if (size == 1)
        ano::copy(in, out);
      // more than one row, we transpose them
      else
      {
        auto&& ovals     = out.values();
        auto&& row_ovals = ano::ranges::rows(ovals);
        int    num_col   = 0;
        for (auto&& row : row_ovals)
        {
          for (auto&& oval : row)
          {
            if (num_row == size)
            {
              num_row = 0;
              ++num_col;
            }
            auto&& row = vec_rows[num_row];
            auto   b   = row.begin();
            for (int j = 0; j < num_col; ++j)
              ++b;
            oval = *b;
            ++num_row;
          }
        }
      }
    }
  }


  template <class InputImage>
  image_ch_value_t<InputImage, image_value_t<InputImage>> transform(InputImage in)
  {
    static_assert(ano::is_a<InputImage, ano::details::Image>());

    using O = image_ch_value_t<InputImage, image_value_t<InputImage>>;

    // Check concretizable
    O out = imchvalue<image_value_t<InputImage>>(in);
    ano::transpose(in, out);
    return out;
  }


} // namespace ano
