#pragma once

#include <limits>

namespace mln::py
{

    template <typename T = void>
    struct value_set;

    template <>
    struct value_set<void>
    {
        virtual float normalize(void *val) const = 0;
    };

    template <typename T>
    struct value_set : value_set<void>
    {
        float normalize(void *val) const override final
        {
            return static_cast<float>(*static_cast<T*>(val)) / std::numeric_limits<T>::max();
        }

        float normalize(T val) const
        {
            return static_cast<float>(val) / std::numeric_limits<T>::max();
        }
    };
} //namespace mln::py