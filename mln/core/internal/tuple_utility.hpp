#ifndef TUPLE_UTILITY_HPP
# define TUPLE_UTILITY_HPP

# include <tuple>
# include <utility>

namespace mln
{

  namespace internal
  {


    template < class F, class... TTypes>
    auto
    tuple_transform(std::tuple<TTypes...>& t, F f);

    template < class F, class... TTypes>
    auto
    tuple_transform(const std::tuple<TTypes...>& t, F f);

    template < class F, class... TTypes>
    std::tuple<TTypes...>&
    tuple_for_each(std::tuple<TTypes...>& t, F f);

    template < class F, class... TTypes>
    const std::tuple<TTypes...>&
    tuple_for_each(const std::tuple<TTypes...>& t, F f);

    template <class Tuple, class F>
    struct tuple_meta_transform;


    /************************/
    /*** Implementation   ***/
    /************************/

    namespace impl
    {

      // Ensure that: F(x) -> T& will store T& (make_tuple stores T)
      // and F(x) -> T will store T (forward_as_tuple stores dangling T&&)
      template <class F, class Tuple, std::size_t... Ints>
      std::tuple< std::result_of_t<F(std::tuple_element_t<Ints,
                                     std::remove_reference_t<Tuple>>)>... >
      tuple_transform_helper(F f, Tuple&& t, std::index_sequence<Ints...>)
      {
        return std::forward_as_tuple(f(std::get<Ints>(std::forward<Tuple>(t)))...);
      }

      template <class F, class Tuple, std::size_t... Ints>
      void
      tuple_foreach_helper(F f, Tuple&& t, std::index_sequence<Ints...>)
      {
        (void) std::initializer_list<int> {
          (f(std::get<Ints>(std::forward<Tuple>(t))), 0)...
        };
      }
    }

    template <class F, class... TTypes>
    auto
    tuple_transform(std::tuple<TTypes...>& t, F f)
    {
      return impl::tuple_transform_helper(f, t, std::make_index_sequence<sizeof...(TTypes)> ());
    }

    template <class F, class... TTypes>
    auto
    tuple_transform(const std::tuple<TTypes...>& t, F f)
    {
      return impl::tuple_transform_helper(f, t, std::make_index_sequence<sizeof...(TTypes)> ());
    }

    template <class F, class... TTypes>
    std::tuple<TTypes...>&
    tuple_for_each(std::tuple<TTypes...>& t, F f)
    {
      impl::tuple_foreach_helper(f, t, std::make_index_sequence<sizeof...(TTypes)> ());
      return t;
    }

    template <class F, class... TTypes>
    const std::tuple<TTypes...>&
    tuple_for_each(const std::tuple<TTypes...>& t, F f)
    {
      impl::tuple_foreach_helper(f, t, std::make_index_sequence<sizeof...(TTypes)> ());
      return t;
    }


    template <class F, class... TTypes>
    struct tuple_meta_transform< std::tuple<TTypes...>, F>
    {
      typedef std::tuple< typename F::template apply<TTypes&>::type... > type;
    };

    template <class F, class... TTypes>
    struct tuple_meta_transform< const std::tuple<TTypes...>, F>
    {
      typedef std::tuple< typename F::template apply<const TTypes&>::type... > type;
    };

  }

}

#endif // ! TUPLE_UTILITY_HPP
