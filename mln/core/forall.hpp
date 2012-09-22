#ifndef MLN_CORE_FORALL_HPP
# define MLN_CORE_FORALL_HPP

# include <mln/core/image/zip_image.hpp>

# include <boost/preprocessor/comparison/equal.hpp>
# include <boost/preprocessor/control/iif.hpp>
# include <boost/preprocessor/punctuation/comma_if.hpp>
//# include <boost/preprocessor/control/if.hpp>
# include <boost/preprocessor/repetition/repeat.hpp>
# include <boost/preprocessor/arithmetic/div.hpp>
# include <boost/preprocessor/variadic/to_seq.hpp>
//# include <boost/preprocessor/variadic/elem.hpp>
# include <boost/preprocessor/variadic/size.hpp>
# include <boost/preprocessor/seq/rest_n.hpp>
# include <boost/preprocessor/seq/enum.hpp>
# include <boost/preprocessor/seq/elem.hpp>


namespace mln
{

  namespace internal
  {

    template <typename T>
    struct false_var_t
    {
      constexpr operator bool() const { return false; }
      T& get() { return x_;}

      T x_;
    };

  }

}

# define MLN_DECL_VAR(ID, VALUE)				\
  if (mln::internal::false_var_t< decltype(VALUE) > ID = {VALUE}) {} else



# define MLN_DECLARE(z, n, var)						\
  if (bool _mln_continue_##n = false) {} else				\
    for (BOOST_PP_SEQ_ELEM(n, var) = boost::get<n>(*_mln_for_cur_.get()); !_mln_continue_##n; _mln_continue_##n = true)

# define MLN_DECLARE_2(z, n, var)                                       \
  if (bool _mln_continue_##n = false) {} else				\
    for (BOOST_PP_SEQ_ELEM(n, var) =					\
	   internal::unzip_pixel_proxy<n, decltype(*_mln_for_cur_.get())>(*_mln_for_cur_.get()); !_mln_continue_##n; _mln_continue_##n = true)




# define forall_v_(ARGC, ARGV)						\
  static_assert( ARGC > 0 and ARGC % 2 == 0,				\
		 "Number of arguments of the forall macros should be odd"); \
  using std::begin;							\
  using std::end;							\
  MLN_DECL_VAR(_mln_zip_image__,					\
	       imzip(BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_REST_N(BOOST_PP_DIV(ARGC, 2), ARGV)))) \
  MLN_DECL_VAR(_mln_for_range_, _mln_zip_image__.get().values())	\
  MLN_DECL_VAR(_mln_for_cur_, begin(_mln_for_range_.get()))		\
  MLN_DECL_VAR(_mln_for_end_, end(_mln_for_range_.get()))		\
  for (; _mln_for_cur_.get() != _mln_for_end_.get(); ++(_mln_for_cur_.get())) \
    BOOST_PP_REPEAT(BOOST_PP_DIV(ARGC, 2), MLN_DECLARE, ARGV)


# define forall_pix_(ARGC, ARGV)						\
  static_assert( ARGC > 0 and ARGC % 2 == 0,				\
		 "Number of arguments of the forall macros should be odd"); \
  using std::begin;							\
  using std::end;							\
  MLN_DECL_VAR(_mln_zip_image__,					\
	       imzip(BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_REST_N(BOOST_PP_DIV(ARGC, 2), ARGV)))) \
  MLN_DECL_VAR(_mln_for_range_, _mln_zip_image__.get().pixels())	\
  MLN_DECL_VAR(_mln_for_cur_, begin(_mln_for_range_.get()))		\
  MLN_DECL_VAR(_mln_for_end_, end(_mln_for_range_.get()))		\
  for (; _mln_for_cur_.get() != _mln_for_end_.get(); ++(_mln_for_cur_.get())) \
    BOOST_PP_REPEAT(BOOST_PP_DIV(ARGC, 2), MLN_DECLARE_2, ARGV)

# define forall_v_2(Arg1, Arg2, ...) for (Arg1 : Arg2.values())

# define forall_v(...)							\
  BOOST_PP_IIF( BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2),	\
	       forall_v_2(__VA_ARGS__),					\
	       forall_v_(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),		\
			 BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))



# define forall_pix_2(Arg1, Arg2, ...)		\
  for (Arg1 : Arg2.pixels())


# define forall_pix(...)						\
  BOOST_PP_IIF( BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2),	\
	       forall_pix_2(__VA_ARGS__),				\
	       forall_pix_(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),		\
			   BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))


# define MLN_FORALL_INIT(z, n, args)		\
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).init()

# define MLN_FORALL_NEXT(z, n, args)		\
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).next()

# define MLN_FORALL_UNSET_DEJAVU(z, n, args)	\
  BOOST_PP_COMMA_IF(n) BOOST_PP_SEQ_ELEM(n, args).set_dejavu_(false)

# define forall_(ARGC, ARGV)						\
  for( BOOST_PP_REPEAT(ARGC, MLN_FORALL_INIT, ARGV);			\
       BOOST_PP_SEQ_ELEM(0, ARGV).is_valid();				\
       BOOST_PP_REPEAT(ARGC, MLN_FORALL_NEXT, ARGV),			\
	 BOOST_PP_REPEAT(ARGC, MLN_FORALL_UNSET_DEJAVU, ARGV))

# define forall(...)				\
  forall_(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),	\
	  BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

# define mln_pixter(ID, IMAGE)						\
  mln::internal::pix_range_iterator_proxy<decltype(IMAGE.pixels())> ID(IMAGE.pixels());

# define mln_nixter(ID, PIXRANGE)						\
  mln::internal::pix_range_iterator_proxy<decltype(PIXRANGE)> ID(PIXRANGE);


#endif //!MLN_CORE_FORALL_HPP
