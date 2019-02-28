#pragma once

#include <mln/core/config.hpp>
#include <mln/core/foreach.hpp>
#include <mln/core/iterator/iterator_utils.hpp>
#include <mln/core/iterator/zip_iterator.hpp>
#include <mln/core/range/range.hpp>

#include <boost/preprocessor/arithmetic/div.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/first_n.hpp>
#include <boost/preprocessor/seq/rest_n.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>


/******************************************/
/****    Generic mln_forall macro      ****/
/******************************************/

namespace mln
{
  namespace details
  {
    template <class IType1, class IType2, class... ITypes>
    auto make_forall_iterator(IType1& it1, IType2& it2, ITypes&... iterators)
    {
      return make_zip_iterator(it1, it2, iterators...);
    }

    template <class IType>
    IType& make_forall_iterator(IType& it)
    {
      return it;
    }
  } // namespace details
} // namespace mln

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

#define mln_simple_forall(p) for (p.init(); !p.finished(); p.next())

/******************************************/
/****        mln_iter/mln_riter        ****/
/******************************************/

#define mln_iter(p, range) auto p = mln::rng::iter(range);

#define mln_riter(p, range) auto p = mln::rng::riter(range);

/******************************************/
/****      mln_viter/mln_rviter       *****/
/******************************************/

// DATA is pair of sequences (ids, images)
#define __mln_viter_decl__(z, i, DATA)                                                                                 \
  auto BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(0, DATA)) =                                                            \
      mln::rng::iter((BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(1, DATA))).values());

#define __mln_viter_impl__(ARGC, ARGV)                                                                                 \
  BOOST_PP_REPEAT(ARGC, __mln_viter_decl__, (BOOST_PP_SEQ_FIRST_N(ARGC, ARGV), BOOST_PP_SEQ_REST_N(ARGC, ARGV)))

#define mln_viter(...)                                                                                                 \
  __mln_viter_impl__(BOOST_PP_DIV(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define __mln_rviter_decl__(z, i, DATA)                                                                                \
  auto BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(0, DATA)) =                                                            \
      mln::rng::riter((BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(1, DATA))).values());

#define __mln_rviter_impl__(ARGC, ARGV)                                                                                \
  BOOST_PP_REPEAT(ARGC, __mln_viter_decl__, (BOOST_PP_SEQ_FIRST_N(ARGC, ARGV), BOOST_PP_SEQ_REST_N(ARGC, ARGV)))

#define mln_rviter(...)                                                                                                \
  __mln_rviter_impl__(BOOST_PP_DIV(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

/******************************************/
/****      mln_pixter/mln_rpixter     *****/
/******************************************/

// DATA is pair of sequences (ids, images)
#define __mln_pixter_decl__(z, i, DATA)                                                                                \
  auto BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(0, DATA)) =                                                            \
      mln::rng::iter((BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(1, DATA))).pixels());

#define __mln_pixter_impl__(ARGC, ARGV)                                                                                \
  BOOST_PP_REPEAT(ARGC, __mln_pixter_decl__, (BOOST_PP_SEQ_FIRST_N(ARGC, ARGV), BOOST_PP_SEQ_REST_N(ARGC, ARGV)))

#define mln_pixter(...)                                                                                                \
  __mln_pixter_impl__(BOOST_PP_DIV(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define __mln_rpixter_decl__(z, i, DATA)                                                                               \
  auto BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(0, DATA)) =                                                            \
      mln::rng::riter((BOOST_PP_SEQ_ELEM(i, BOOST_PP_TUPLE_ELEM(1, DATA))).pixels());

#define __mln_rpixter_impl__(ARGC, ARGV)                                                                               \
  BOOST_PP_REPEAT(ARGC, __mln_rpixter_decl__, (BOOST_PP_SEQ_FIRST_N(ARGC, ARGV), BOOST_PP_SEQ_REST_N(ARGC, ARGV)))

#define mln_rpixter(...)                                                                                               \
  __mln_rpixter_impl__(BOOST_PP_DIV(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

/******************************************/
/****       mln_piter/mln_rpiter       ****/
/******************************************/

#define mln_piter(p, ima) auto p = ima.domain().iter();

#define mln_rpiter(p, ima) auto p = ima.domain().riter();
