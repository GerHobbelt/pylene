#ifndef MLN_POINT_HH
# define MLN_POINT_HH

# include <mln/core/vec_base.hpp>

namespace mln
{
	struct point_tag {};

	namespace internal
	{
		template <typename dummy>
		struct vec_base_traits<point_tag, dummy>
		{
			static const bool is_additive = true;
			static const bool is_additive_ext = true;
			static const bool is_multiplicative = false;
			static const bool is_multiplicative_ext = true;
			static const bool is_less_than_comparable = true;
			static const bool is_equality_comparable = true;
		};
	}

	template <typename T, std::size_t dim>
	struct point : internal::vec_base<T, dim, point_tag>
	{
	  typedef internal::vec_base<T, dim, point_tag> type;
	};

	typedef internal::vec_base<short, 1, point_tag> point1d;
	typedef internal::vec_base<short, 2, point_tag> point2d;
	typedef internal::vec_base<short, 3, point_tag> point3d;
	typedef internal::vec_base<float, 1, point_tag> point1df;
	typedef internal::vec_base<float, 2, point_tag> point2df;
	typedef internal::vec_base<float, 3, point_tag> point3df;

}


#endif
