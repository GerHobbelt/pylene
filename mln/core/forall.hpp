#ifndef MLN_CORE_FORALL_HPP
# define MLN_CORE_FORALL_HPP

# include <mln/core/config.hpp>
# include <mln/core/foreach.hpp>
# include <mln/core/iterator/zip_iterator.hpp>
# include <mln/core/iterator/iterator_utils.hpp>
# include <mln/core/range/range.hpp>

# include <boost/preprocessor/repetition/repeat.hpp>
# include <boost/preprocessor/arithmetic/div.hpp>
# include <boost/preprocessor/variadic/to_seq.hpp>
# include <boost/preprocessor/variadic/size.hpp>
# include <boost/preprocessor/seq/rest_n.hpp>
# include <boost/preprocessor/seq/first_n.hpp>
# include <boost/preprocessor/seq/elem.hpp>


/******************************************/
/****    Generic mln_forall macro      ****/
/******************************************/

namespace mln
{
  namespace details
  {
    template <class... ITypes>
    auto make_forall_iterator(ITypes&... iterators)
    {
      return make_zip_iterator(iterators...);
    }
  }
}

#define mln_forall(...)                                                                                                \
  MLN_DECL_VAR(__mln_it, mln::details::make_forall_iterator(__VA_ARGS__))                                              \
  if (bool __mln_has_been_broken = false)                                                                              \
  {                                                                                                                    \
  }                                                                                                                    \
  else                                                                                                                 \
    for (mln::outer_init(__mln_it.get()); !__mln_has_been_broken && !mln::outer_finished(__mln_it.get());              \
         !__mln_has_been_broken ? mln::outer_next(__mln_it.get()) : (void)0)                                           \
      for (mln::inner_init(__mln_it.get()); !mln::inner_finished(__mln_it.get());                                      \
           mln::inner_next(__mln_it.get()), __mln_has_been_broken = false)                                             \
        if ((__mln_has_been_broken = true), false)                                                                     \
        {                                                                                                              \
        }                                                                                                              \
        else

/******************************************/
/****             mln_iter             ****/
/******************************************/

# define mln_iter(p, range)                       \
  auto p = mln::rng::iter(range);

# define mln_riter(p, range)                       \
  auto p = mln::rng::riter(range);



/******************************************/
/****            mln_viter            *****/
/******************************************/

// DATA is pair of sequences (ids, images)
#define __mln_viter_decl__(z, i, DATA)                                                                                 \
  auto BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(0, DATA)) =                                                            \
    mln::rng::iter((BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(1, DATA))).values());

#define __mln_viter_impl__(ARGC, ARGV)                                                                                 \
  BOOST_PP_REPEAT(ARGC, __mln_viter_decl__, (BOOST_PP_SEQ_FIRST_N(ARGC, ARGV), BOOST_PP_SEQ_REST_N(ARGC, ARGV)))

#define mln_viter(...)                                                                                                 \
  __mln_viter_impl__(BOOST_PP_DIV(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

/******************************************/
/****            mln_pixter           *****/
/******************************************/

// DATA is pair of sequences (ids, images)
#define __mln_pixter_decl__(z, i, DATA)                                                                                 \
  auto BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(0, DATA)) =                                                            \
    mln::rng::iter((BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(1, DATA))).pixels());

#define __mln_pixter_impl__(ARGC, ARGV)                                                                                \
  BOOST_PP_REPEAT(ARGC, __mln_pixter_decl__, (BOOST_PP_SEQ_FIRST_N(ARGC, ARGV), BOOST_PP_SEQ_REST_N(ARGC, ARGV)))

#define mln_pixter(...)                                                                                                 \
  __mln_pixter_impl__(BOOST_PP_DIV(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))


/******************************************/
/****             mln_piter             ****/
/******************************************/

# define mln_piter(p, ima)                      \
  auto p = ima.domain().iter();

#endif // !MLN_CORE_FORALL_HPP
