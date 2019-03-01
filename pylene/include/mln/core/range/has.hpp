#pragma once

#include <mln/core/range/iter.hpp>

#include <utility>


namespace mln
{

  namespace rng
  {

    template <typename Range, typename T>
    bool has(const Range& rng, const T& val);

    /***********************/
    /*** Implementation  ***/
    /***********************/

    namespace internal
    {

      template <typename R, typename T, typename = void>
      struct has_lookup
      {
        static bool has(const R& rng, const T& val)
        {
          auto it = rng::iter(rng);
          for (it.init(); !it.finished(); it.next())
            if (*it == val)
              return true;
          return false;
        }
      };

      template <typename R, typename T>
      struct has_lookup<R, T, decltype(std::declval<R>().has(std::declval<T>()), (void)0)>
      {
        static bool has(const R& rng, const T& val) { return rng.has(val); }
      };
    } // namespace internal

    template <typename Range, typename T>
    inline bool has(const Range& rng, const T& val)
    {
      return internal::has_lookup<Range, T>::has(rng, val);
    }

  } // namespace rng

} // namespace mln
