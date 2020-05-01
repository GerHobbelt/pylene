#pragma once
#include <mln/core/canvas/parallel_pointwise.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/range/rows.hpp>

#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/functional/concepts.hpp>
/// \file

namespace mln
{
  /// \brief Apply a function on the values of an image.
  template <class InputImage, class UnaryFunction>
  void for_each(InputImage in, UnaryFunction f);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/
  template <class InputImage, class UnaryFunction>
  void for_each(InputImage input, UnaryFunction f)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::invocable<UnaryFunction, image_reference_t<InputImage>>);

    auto&& vals = input.new_values();
    for (auto r : ranges::rows(vals))
      ::ranges::for_each(r, f);
  }

  namespace parallel
  {
    namespace details
    {
      template <class Function, class InputImage>
      class ForEachParallel : public ParallelCanvas2d
      {
        InputImage _in;
        Function _fun;

        static_assert(mln::is_a<InputImage, experimental::Image>());
        static_assert(::ranges::invocable<Function, image_reference_t<InputImage>>);

        mln::experimental::box2d GetDomain() const final { return _in.domain(); }

        void ExecuteTile(mln::experimental::box2d b) const final
        {
          auto subimage = _in.clip(b);
          mln::for_each(subimage, _fun);
        }
      public:
        ForEachParallel(InputImage input, Function fun)
            : _in{input}
            , _fun{fun}
        {}
      };
    } // namespace details

    template <class InputImage, class UnaryFunction>
    void for_each(InputImage in, UnaryFunction f)
    {
      details::ForEachParallel caller(in, f);
      parallel_execute2d(caller);
    }
  } // namespace parallel

} // namespace mln
