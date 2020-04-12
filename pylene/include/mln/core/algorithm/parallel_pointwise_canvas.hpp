#pragma once

#include <tbb/blocked_range2d.h>
#include <mln/core/box.hpp>

#include <mln/core/algorithm/for_each.hpp>

namespace mln
{
  struct ParallelCanvas2d
  {
    static constexpr int TILE_WIDTH  = 128;
    static constexpr int TILE_HEIGHT = 128;

    void operator()(const tbb::blocked_range2d<int>&) const;

    virtual mln::experimental::box2d GetDomain() const = 0;
    virtual void ExecuteTile(mln::experimental::box2d b) const = 0;
  };

  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  ** We create a wrapper class to circumvent TBB not allowing abstract classes as parallel_for body
  */
  void parallel_execute2d(ParallelCanvas2d&);


  template <class Function, class InputImage>
  class ForEachPointwise : public ParallelCanvas2d
  {
    InputImage _in;
    Function _fun;

    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::invocable<Function, image_reference_t<InputImage>>);

    ForEachPointwise(InputImage input, Function fun)
        : _in{input}
        , _fun{fun}
    {}

    mln::experimental::box2d GetDomain() const final { return _in.domain(); }
  public:
    void ExecuteTile(mln::experimental::box2d b) const final 
    {
      auto subimage = _in.clip(b);
      mln::for_each(subimage, _fun);
    }
  };

/*
  template <class Function, class InputImage, class OutputImage>
  class TransformPointwise : ParallelCanvas2d
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(mln::is_a<OutputImage, experimental::Image>());
    static_assert(::ranges::invocable<Function, image_reference_t<InputImage>>);

    TransformPointwise(InputImage input, Function fun);

    mln::experimental::box2d GetDomain() const final { return input.domain(); }

    void ExecuteTile(mln::experimental::box2d b) const final
    {
      auto subimage_in = input.clip(domain);
      auto subimage_out = input.clip(domain);
      mln::transform(subimage, fun);
    }
  };
*/
} // namespace mln
