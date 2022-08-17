#pragma once

#include <ano/core/canvas/parallel_pointwise.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/range/rows.hpp>

#include <range/v3/algorithm/fill.hpp>

#include <algorithm>


/// \file

namespace ano
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
    static_assert(ano::is_a<OutputImage, ano::details::Image>());

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

        static_assert(ano::is_a<InputImage, ano::details::Image>());

        ano::box2d GetDomain() const final { return _in.domain(); }

        void ExecuteTile(ano::box2d b) const final
        {
          auto subimage_in = _in.clip(b);
          ano::fill(subimage_in, _val);
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
} // namespace ano
