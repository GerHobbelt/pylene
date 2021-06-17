#include <mln/core/se/se_2d.hpp>

namespace mln::se
{

  namespace
  {

    struct visitor_
    {
      struct is_decomposable
      {
        template <class E>
        bool operator()(const E& se)
        {
          if constexpr (E::decomposable::value)
            return se.is_decomposable();
          return false;
        }
      };

      struct decompose
      {
        template <class E>
        std::vector<structuring_element_2d> operator()(const E& se)
        {
          if constexpr (E::decomposable::value)
          {
            auto ses_ = se.decompose();

            std::vector<structuring_element_2d> ses;
            ses.reserve(ses_.size());

            for (auto se : ses_)
              ses.push_back(se);

            return ses;
          }
          return {};
        }
      };

    };
  }


  // clang-format off

  structuring_element_2d::structuring_element_2d(const rect2d& x)          : m_se {x} {}
  structuring_element_2d::structuring_element_2d(const disc& x)            : m_se {x} {}
  structuring_element_2d::structuring_element_2d(const mask2d& x)          : m_se {x} {}
  structuring_element_2d::structuring_element_2d(const periodic_line2d& x) : m_se {x} {}


  bool structuring_element_2d::is_decomposable() const noexcept { return std::visit(visitor_::is_decomposable{}, m_se); }
  std::vector<structuring_element_2d> structuring_element_2d::decompose() const { return std::visit(visitor_::decompose{}, m_se); }

  mln::box2d structuring_element_2d::compute_input_region(mln::box2d roi) const noexcept {  return std::visit([roi](const auto& x) -> mln::box2d { return x.compute_input_region(roi); }, m_se); }
  mln::box2d structuring_element_2d::compute_output_region(mln::box2d roi) const noexcept { return std::visit([roi](const auto& x) -> mln::box2d { return x.compute_output_region(roi); }, m_se); }

  structuring_element_2d::kind structuring_element_2d::get_kind() const noexcept { return (kind) m_se.index(); }

  rect2d structuring_element_2d::as_rectangle() const                   { return std::get<rect2d>(m_se); }
  disc   structuring_element_2d::as_disc() const                        { return std::get<disc>(m_se); }
  mask2d structuring_element_2d::as_mask() const                        { return std::get<mask2d>(m_se); }
  periodic_line2d structuring_element_2d::as_periodic_line() const      { return std::get<periodic_line2d>(m_se); }

  // clang-format on
}
