#pragma once
#include <mln/core/box.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/mask2d.hpp>
#include <mln/core/se/periodic_line2d.hpp>


#include <vector>
#include <variant>


namespace mln::se
{
  class rect2d;
  class disc;
  class mask2d;
  class periodic_line2d;


  /// Kind-erased structuring element
  class structuring_element_2d
  {
  public:
    enum kind {
      RECTANGLE = 0,
      DISC = 1,
      MASK = 2,
      PERIODIC_LINE = 3,
    };


    structuring_element_2d(const rect2d& r);
    structuring_element_2d(const disc& d);
    structuring_element_2d(const mask2d& m);
    structuring_element_2d(const periodic_line2d& l);


    /// \brief Return true if decomposable (for any non-empty rectangle)
    bool is_decomposable() const noexcept;

    /// \brief Return a set of simpler structuring elements
    std::vector<structuring_element_2d> decompose() const;


    /// \brief Compute the input region of a ROI
    mln::box2d compute_input_region(mln::box2d roi) const noexcept;

    /// \brief Compute the output region of a ROI
    mln::box2d compute_output_region(mln::box2d roi) const noexcept;


    /// \brief Return the kind of structuring element
    kind get_kind() const noexcept;


    rect2d          as_rectangle() const;
    disc            as_disc() const;
    mask2d          as_mask() const;
    periodic_line2d as_periodic_line() const;


  private:
    std::variant<rect2d, disc, mask2d, periodic_line2d> m_se;
  };


}

