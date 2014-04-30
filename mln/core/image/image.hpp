#ifndef MLN_CORE_IMAGE_IMAGE_HPP
# define MLN_CORE_IMAGE_IMAGE_HPP

/// \file

# include <mln/core/config.hpp>
# include <mln/core/concept/image.hpp>
# include <mln/core/image_traits.hpp>
# include <mln/core/image_base.hpp>
# include <mln/core/ch_value.hpp>

# include <mln/core/image/morphers/morpher_base.hpp>


# include <mln/core/forall.hpp>
# include <mln/core/image/image_routines.hpp>
# include <mln/core/image/image_expr.hpp>
# include <mln/core/image/image_ops.hpp>

// The most fundamental views are automatically included
# include <mln/core/image/sub_image.hpp>                        // Filter by domain
# include <mln/core/image/morphers/filtered_image.hpp>          // Filter by function
# include <mln/core/image/morphers/transformed_image.hpp>       // Map



#endif // !MLN_CORE_IMAGE_IMAGE_HPP
