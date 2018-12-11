#pragma once

namespace mln
{

  template <class D>
  using domain_value_t = typename D::value_type;

  template <class D>
  using domain_reference_t = typename D::reference;

  template <class D>
  using domain_point_t = typename D::value_type;

} // namespace mln
