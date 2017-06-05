#ifndef MLN_CORE_FORALL_HPP
# define MLN_CORE_FORALL_HPP

# include <mln/core/range/range.hpp>
# include <mln/core/config.hpp>
# include <mln/core/foreach.hpp>
# include <boost/preprocessor/comparison/equal.hpp>
# include <boost/preprocessor/control/iif.hpp>
# include <boost/preprocessor/control/if.hpp>
# include <boost/preprocessor/punctuation/comma_if.hpp>
//# include <boost/preprocessor/control/if.hpp>
# include <boost/preprocessor/repetition/repeat.hpp>
# include <boost/preprocessor/arithmetic/div.hpp>
# include <boost/preprocessor/variadic/to_seq.hpp>
//# include <boost/preprocessor/variadic/elem.hpp>
# include <boost/preprocessor/variadic/size.hpp>
# include <boost/preprocessor/cat.hpp>
# include <boost/preprocessor/seq/rest_n.hpp>
# include <boost/preprocessor/seq/enum.hpp>
# include <boost/preprocessor/seq/elem.hpp>
# include <boost/preprocessor/seq/push_front.hpp>
# include <boost/preprocessor/seq/seq.hpp>


/******************************************/
/****    Generic mln_forall macro      ****/
/******************************************/



# define MLN_FORALL_INIT(z, n, args)                            \
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).init()

# define MLN_FORALL_NEXT(z, n, args)                            \
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).next()

# define MLN_FORALL_OUTER_INIT(z, n, args)                              \
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).__outer_init()

# define MLN_FORALL_OUTER_NEXT(z, n, args)                              \
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).__outer_next()

# define MLN_FORALL_INNER_INIT(z, n, args)                              \
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).__inner_init()

# define MLN_FORALL_INNER_NEXT(z, n, args)                              \
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).__inner_next()

# define MLN_FORALL_UNSET_DEJAVU(z, n, args)                            \
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).set_dejavu_(false)

# define __mln_forall__(ARGC, ARGV)                                     \
  for( BOOST_PP_REPEAT(ARGC, MLN_FORALL_OUTER_INIT, ARGV),              \
	 BOOST_PP_REPEAT(ARGC, MLN_FORALL_UNSET_DEJAVU, ARGV);		\
       !BOOST_PP_SEQ_ELEM(0, ARGV).__outer_finished();                  \
       BOOST_PP_REPEAT(ARGC, MLN_FORALL_OUTER_NEXT, ARGV),              \
	 BOOST_PP_REPEAT(ARGC, MLN_FORALL_UNSET_DEJAVU, ARGV))          \
    for( BOOST_PP_REPEAT(ARGC, MLN_FORALL_INNER_INIT, ARGV),            \
           BOOST_PP_REPEAT(ARGC, MLN_FORALL_UNSET_DEJAVU, ARGV);        \
         !BOOST_PP_SEQ_ELEM(0, ARGV).__inner_finished();                \
         BOOST_PP_REPEAT(ARGC, MLN_FORALL_INNER_NEXT, ARGV),            \
           BOOST_PP_REPEAT(ARGC, MLN_FORALL_UNSET_DEJAVU, ARGV))


# define __mln_forall__1(p, ...)                                        \
  if (bool __mln_has_been_broken = false) {} else                       \
    for (p.__outer_init(); !p.__outer_finished() and (!__mln_has_been_broken); \
         !__mln_has_been_broken ? p.__outer_next() : (void)0)            \
      for (p.__inner_init(); !p.__inner_finished();                     \
           p.__inner_next(), __mln_has_been_broken = false)             \
        if ((__mln_has_been_broken = true), false) {} else              \

# define mln_forall(...)                                                \
  BOOST_PP_IF( BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1),  \
               __mln_forall__1(__VA_ARGS__),                            \
               __mln_forall__(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),      \
                              BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

/******************************************/
/****             mln_iter             ****/
/******************************************/

# define mln_iter(p, range)                       \
  auto p = mln::rng::iter(range);

# define mln_riter(p, range)                       \
  auto p = mln::rng::riter(range);


#endif

/// FIXME: move image specific macros to its own file.


#ifndef MLN_CORE_IMAGE_IMAGE_HPP
# include <mln/core/image/image.hpp>
# if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
# warning "this part should be included by mln/core/image/image.hpp only"
# elif defined(_MSC_VER)
# pragma message("this part should be included by mln/core/image/image.hpp only")
# endif
#endif


#ifndef MLN_CORE_FORALL_1_HPP
# define MLN_CORE_FORALL_1_HPP

# include <mln/core/image/morphers/zip_image.hpp>
# include <mln/core/iterator/unzip_proxy_iterator.hpp>


/************************************/
/***        Helper macros          **/
/************************************/

# define MLN_DECLARE(z, n, var)						\
  if (bool _mln_continue_##n = false) {} else				\
    for (BOOST_PP_SEQ_ELEM(n, var) = boost::get<n>(*_mln_for_cur_.get()); !_mln_continue_##n; _mln_continue_##n = true)

# define MLN_DECLARE_2(z, n, var)                                       \
  if (bool _mln_continue_##n = false) {} else				\
    for (BOOST_PP_SEQ_ELEM(n, var) =					\
	   internal::unzip_pixel_proxy<n, decltype(*_mln_for_cur_.get())>(*_mln_for_cur_.get()); !_mln_continue_##n; _mln_continue_##n = true)


/******************************************/
/****            mln_viter            *****/
/******************************************/


# define __mln_viter_decl__(z, N, ARGV)					\
  auto BOOST_PP_SEQ_ELEM(N, BOOST_PP_SEQ_TAIL(ARGV)) =			\
    (mln::make_unzip_proxy_iterator<N, decltype(BOOST_PP_SEQ_HEAD(ARGV))> \
     (BOOST_PP_SEQ_HEAD(ARGV)));


# define __mln_viter__(ARGC, ARGV, ID_IMA, ID_ITER)					\
  auto ID_IMA = imzip(BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_REST_N(BOOST_PP_DIV(ARGC, 2), ARGV))); \
  auto ID_ITER = mln::make_proxy_iterator(ID_IMA.values().iter());	\
  BOOST_PP_REPEAT(BOOST_PP_DIV(ARGC, 2), __mln_viter_decl__,		\
		  BOOST_PP_SEQ_PUSH_FRONT(ARGV, ID_ITER));

# define __mln_viter_chap__(ARGC, ARGV)					\
  static_assert(ARGC > 0 and ARGC % 2 == 0,				\
		"Number of arguments of the forall macros should be odd"); \
  __mln_viter__(ARGC, ARGV,						\
		BOOST_PP_CAT(__mln_zip_ima__,BOOST_PP_SEQ_HEAD(ARGV)),	\
		BOOST_PP_CAT(__mln_zip_iter__,BOOST_PP_SEQ_HEAD(ARGV)))

# define __mln_viter_chap__2(p, ima, ...)                           \
  auto&& BOOST_PP_CAT(__mln_ima_, p) = ima;                         \
  auto p = BOOST_PP_CAT(__mln_ima_, p).values().iter();

# define mln_viter(...)                                                 \
  BOOST_PP_IF( BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2),	\
               __mln_viter_chap__2(__VA_ARGS__),                        \
               __mln_viter_chap__(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),  \
                                  BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))


/******************************************/
/****            mln_pixter           *****/
/******************************************/


# define __mln_pixter_decl__(z, N, ARGV)                                \
  auto BOOST_PP_SEQ_ELEM(N, BOOST_PP_SEQ_TAIL(ARGV)) =			\
    (mln::make_unzip_proxy_pixel_iterator<N, decltype(BOOST_PP_SEQ_HEAD(ARGV))> \
     (BOOST_PP_SEQ_HEAD(ARGV)));


# define __mln_pixter__(ARGC, ARGV, ID_IMA, ID_ITER)					\
  auto ID_IMA = imzip(BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_REST_N(BOOST_PP_DIV(ARGC, 2), ARGV))); \
  auto ID_ITER = mln::make_proxy_iterator(ID_IMA.pixels().iter());	\
  BOOST_PP_REPEAT(BOOST_PP_DIV(ARGC, 2), __mln_pixter_decl__,		\
		  BOOST_PP_SEQ_PUSH_FRONT(ARGV, ID_ITER));

# define __mln_pixter_chap__(ARGC, ARGV)					\
  static_assert(ARGC > 0 and ARGC % 2 == 0,				\
		"Number of arguments of the forall macros should be odd"); \
  __mln_pixter__(ARGC, ARGV,						\
		BOOST_PP_CAT(__mln_zip_ima__,BOOST_PP_SEQ_HEAD(ARGV)),	\
		BOOST_PP_CAT(__mln_zip_iter__,BOOST_PP_SEQ_HEAD(ARGV)))

# define __mln_pixter_chap__2(p, ima, ...)                          \
  auto&& BOOST_PP_CAT(__mln_ima_, p) = ima;                         \
  auto p = BOOST_PP_CAT(__mln_ima_, p).pixels().iter();

# define mln_pixter(...)                                                 \
  BOOST_PP_IF( BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2),	\
               __mln_pixter_chap__2(__VA_ARGS__),                       \
               __mln_pixter_chap__(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), \
                                    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))


/******************************************/
/****             mln_piter             ****/
/******************************************/

# define mln_piter(p, ima)                       \
  auto p = ima.domain().iter();

#endif // !MLN_CORE_FORALL_1_HPP
