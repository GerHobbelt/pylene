#ifndef TUPLE_UTILITY_HPP
#define TUPLE_UTILITY_HPP

#include <tuple>
#include <utility>

namespace mln
{

  namespace internal
  {

    template <class F, class... TTypes>
    auto tuple_transform(std::tuple<TTypes...>& t, F f);

    template <class F, class... TTypes>
    auto tuple_transform(const std::tuple<TTypes...>& t, F f);

    template <class F, class... TTypes>
    void tuple_for_each(std::tuple<TTypes...>& t, F f);

    template <class F, class... TTypes>
    void tuple_for_each(const std::tuple<TTypes...>& t, F f);

    /************************/
    /*** Implementation   ***/
    /************************/

    namespace impl
    {

      template <class F, class... TTypes, std::size_t... k>
      auto tuple_transform(std::tuple<TTypes...>& tuple, F f, std::index_sequence<k...>)
      {
        using tuple_type = std::tuple<std::result_of_t<F(TTypes&)>...>;
        return tuple_type(f(std::get<k>(tuple))...);
      }

      template <class F, class... TTypes, std::size_t... k>
      auto tuple_transform(const std::tuple<TTypes...>& tuple, F f, std::index_sequence<k...>)
      {
        using tuple_type = std::tuple<std::result_of_t<F(const TTypes&)>...>;
        return tuple_type(f(std::get<k>(tuple))...);
      }

      template <class F, class Tuple, std::size_t... k>
      void tuple_for_each(Tuple& tuple, F f, std::index_sequence<k...>)
      {
        std::initializer_list<int> _dummy = {(f(std::get<k>(tuple)), 0)...};
        (void)_dummy;
      }
    }

    template <class F, class... TTypes>
    auto tuple_transform(std::tuple<TTypes...>& t, F f)
    {
      return impl::tuple_transform(t, f, std::make_index_sequence<sizeof...(TTypes)>());
    }

    template <class F, class... TTypes>
    auto tuple_transform(const std::tuple<TTypes...>& t, F f)
    {
      return impl::tuple_transform(t, f, std::make_index_sequence<sizeof...(TTypes)>());
    }

    template <class F, class... TTypes>
    void tuple_for_each(std::tuple<TTypes...>& t, F f)
    {
      impl::tuple_for_each(t, f, std::make_index_sequence<sizeof...(TTypes)>());
    }

    template <class F, class... TTypes>
    void tuple_for_each(const std::tuple<TTypes...>& t, F f)
    {
      impl::tuple_for_each(t, f, std::make_index_sequence<sizeof...(TTypes)>());
    }
  }
}

#endif // ! TUPLE_UTILITY_HPP
