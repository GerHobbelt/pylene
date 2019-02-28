#ifndef APPS_ATTRIBUTES_CIRCULARITY_HPP
# define APPS_ATTRIBUTES_CIRCULARITY_HPP

# include "attributes.hpp"


/// \brief Compute the ratio of the principal axes
///
/// C(x) = 1 - (λ₂ / λ₁)
/// where: λ₁, λ₂ are the two major scale parameters
///
/// C(x) = 0 ⇒ Circle/Square
/// C(x) → 1 ⇒ Elongated object

mln::property_map<tree_t, float>
circularity(const tree_t& tree);

#endif // ! APPS_ATTRIBUTES_CIRCULARITY_HPP
