#pragma once

//#include <tbb/blocked_range2d.h>
#include <mln/core/box.hpp>

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/paste.hpp>

namespace mln
{
  struct ParallelCanvas2d
  {
    static constexpr int TILE_WIDTH  = 128;
    static constexpr int TILE_HEIGHT = 128;

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


  template <class InputImage, class OutputImage>
  class CopyParallel : public ParallelCanvas2d
  {
    InputImage _in;
    OutputImage _out;

    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(mln::is_a<OutputImage, experimental::Image>());
    static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    CopyParallel(InputImage input, OutputImage output)
        : _in{input}
        , _out{output}
    {}

    mln::experimental::box2d GetDomain() const final { return _in.domain(); }
  public:
    void ExecuteTile(mln::experimental::box2d b) const final 
    {
      auto subimage_in = _in.clip(b);
      auto subimage_out = _out.clip(b);
      mln::experimental::copy(subimage_in, subimage_out);
    }
  };

  template <class InputImage, class OutputImage>
  class PasteParallel : public ParallelCanvas2d
  {
    InputImage _in;
    OutputImage _out;

    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(mln::is_a<OutputImage, experimental::Image>());
    static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    PasteParallel(InputImage input, OutputImage output)
        : _in{input}
        , _out{output}
    {}

    mln::experimental::box2d GetDomain() const final { return _in.domain(); }
  public:
    void ExecuteTile(mln::experimental::box2d b) const final
    {
      auto subimage_in = _in.clip(b);
      auto subimage_out = _out.clip(b);
      mln::paste(subimage_in, subimage_out);
    }
  };

  template <class InputImage, typename Value>
  class FillParallel : public ParallelCanvas2d
  {
    InputImage _in;
    Value& _val;

    static_assert(mln::is_a<InputImage, experimental::Image>());

    FillParallel(InputImage input, Value& v)
        : _in{input}
        , _val{v}
    {}

    mln::experimental::box2d GetDomain() const final { return _in.domain(); }
  public:
    void ExecuteTile(mln::experimental::box2d b) const final 
    {
      auto subimage_in = _in.clip(b);
      mln::fill(subimage_in, _val);
    }
  };
} // namespace mln
