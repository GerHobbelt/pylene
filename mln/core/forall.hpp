#ifndef MLN_CORE_FORALL_HPP
# define MLN_CORE_FORALL_HPP

# include <mln/core/image/zip_image.hpp>

# include <boost/preprocessor/repetition/repeat.hpp>
# include <boost/preprocessor/arithmetic/div.hpp>
# include <boost/preprocessor/variadic/to_seq.hpp>
# include <boost/preprocessor/variadic/elem.hpp>
# include <boost/preprocessor/variadic/size.hpp>
# include <boost/preprocessor/seq/rest_n.hpp>
# include <boost/preprocessor/seq/enum.hpp>
# include <boost/preprocessor/seq/elem.hpp>




# define MLN_DECLARE(z, n, var)						\
  if (bool _mln_continue_##n = false) {} else				\
    for (BOOST_PP_SEQ_ELEM(n, var) = boost::get<n>(*_mln_for_cur_); !_mln_continue_##n; _mln_continue_##n = true)

# define MLN_DECLARE_2(z, n, var)                                       \
  if (bool _mln_continue_##n = false) {} else				\
    for (BOOST_PP_SEQ_ELEM(n, var) = internal::unzip_pixel_proxy<n, decltype(*_mln_for_cur_)>(*_mln_for_cur_); !_mln_continue_##n; _mln_continue_##n = true)


# define forall_v(...)							\
  static_assert( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__) > 0 and		\
		 BOOST_PP_VARIADIC_SIZE(__VA_ARGS__) % 2 == 0,		\
		 "Number of arguments of the forall macros should be odd"); \
  auto _mln_zip_image__ = imzip( BOOST_PP_SEQ_ENUM(			\
    BOOST_PP_SEQ_REST_N(BOOST_PP_DIV( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), \
			BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) )));	\
  auto _mln_for_range_ =  _mln_zip_image__.values();			\
  using std::begin;							\
  using std::end;							\
  auto _mln_for_cur_ = begin(_mln_for_range_);				\
  auto _mln_for_end_ = end(_mln_for_range_);				\
  for (; _mln_for_cur_ != _mln_for_end_; ++_mln_for_cur_)		\
    BOOST_PP_REPEAT(							\
      BOOST_PP_DIV( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2),		\
      MLN_DECLARE,							\
      BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

# define forall_pix(...)							\
  static_assert( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__) > 0 and		\
		 BOOST_PP_VARIADIC_SIZE(__VA_ARGS__) % 2 == 0,		\
		 "Number of arguments of the forall macros should be odd"); \
  auto _mln_zip_image__ = imzip( BOOST_PP_SEQ_ENUM(			\
    BOOST_PP_SEQ_REST_N(BOOST_PP_DIV( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), \
			BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) )));	\
  auto _mln_for_range_ =  _mln_zip_image__.pixels();			\
  using std::begin;							\
  using std::end;							\
  auto _mln_for_cur_ = begin(_mln_for_range_);				\
  auto _mln_for_end_ = end(_mln_for_range_);				\
  for (; _mln_for_cur_ != _mln_for_end_; ++_mln_for_cur_)		\
    BOOST_PP_REPEAT(							\
      BOOST_PP_DIV( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2),		\
      MLN_DECLARE_2,							\
      BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))



#endif //!MLN_CORE_FORALL_HPP
