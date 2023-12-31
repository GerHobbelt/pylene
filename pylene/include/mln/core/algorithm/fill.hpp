#pragma once

#include <mln/core/canvas/parallel_pointwise.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/range/rows.hpp>

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
    static_assert(mln::is_a<OutputImage, mln::details::Image>());

    auto&& vals = f.values();
    for (auto row : ranges::rows(vals))
      ::ranges::fill(row, v);
  }

  namespace parallel
  {
    namespace details
    {
      template <class InputImage, typename Value>
      class FillParallel : public ParallelCanvas2d
      {
        InputImage _in;
        Value&     _val;

        static_assert(mln::is_a<InputImage, mln::details::Image>());

        mln::box2d GetDomain() const final { return _in.domain(); }

        void ExecuteTile(mln::box2d b) const final
        {
          auto subimage_in = _in.clip(b);
          mln::fill(subimage_in, _val);
        }

      public:
        FillParallel(InputImage input, Value& v)
          : _in{input}
          , _val{v}
        {
        }
      };
    } // namespace details

    template <class InputImage, class Value>
    void fill(InputImage in, const Value& v)
    {
      details::FillParallel caller(in, v);
      parallel_execute2d(caller);
    }
  } // namespace parallel
} // namespace mln
