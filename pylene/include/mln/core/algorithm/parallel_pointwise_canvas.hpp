#pragma once
#ifndef MLN_HAS_TBB
#define MLN_HAS_TBB 1
#endif

#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include "mln/core/box.hpp"
#include "mln/core/concept/object.hpp"
#include <mln/core/image/image.hpp>

namespace mln
{
    template <class Function, class InputImage>
    struct ApplyPointwise
    /*
    ** Caller for the TBB parallel_for
    ** Rationale being that every pointwise algorithm applies a function to every pixel,
    ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
    */
    {
        Function _fun;
        InputImage _in;

        static_assert(mln::is_a<InputImage, experimental::Image>());
        // FIXME we are trying to see if
        // std::is_invocable<Function, InputImage({x,y})>::value is true, but how to access field of template ?
        // static_assert(std::is_invocable<Function, InputImage>::value);

        ApplyPointwise(InputImage input, Function fun)
            : _fun{fun}
            , _in{input}
        {
            tbb::parallel_for(tbb::blocked_range2d<int>(0, _in.width(), _in.width() / 4, 0, _in.height(), _in.height() / 4),
                                [&](tbb::blocked_range2d<int> tile)
                                {
                                    for (auto i = tile.rows().begin(); i != tile.rows().end(); ++i)
                                        for (auto j = tile.cols().begin(); j != tile.cols().end(); ++j)
                                        {
                                            call_function_on_point(i, j);
                                        }
                                });
        }

        void call_function_on_point(int x, int y)
        {
            _fun(_in({x,y}));
        }
    };
} // namespace mln