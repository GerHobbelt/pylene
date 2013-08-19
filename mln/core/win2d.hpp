#ifndef MLN_CORE_WIN2D_HPP
# define MLN_CORE_WIN2D_HPP

# include <mln/core/neighborhood/dyn_neighborhood.hpp>
# include <mln/core/point.hpp>
# include <mln/core/domain/box.hpp>
# include <mln/core/assert.hpp>

///
/// \file
///

namespace mln
{

  ///
  /// \brief Define a dynamic rectangular window
  ///
  using rect2d = dyn_neighborhood<box2d, dynamic_neighborhood_tag>;


  /// \defgroup Free functions
  /// \{
  rect2d make_rectangle2d(unsigned height, unsigned width);
  rect2d make_rectangle2d(unsigned height, unsigned width, point2d center);
  /// \}



  /**************************/
  /***  Implementation     **/
  /**************************/

  inline
  rect2d make_rectangle2d(unsigned height, unsigned width)
  {
    mln_precondition(height % 2 == 1);
    mln_precondition(width % 2 == 1);
    int h = height / 2;
    int w = width / 2;
    box2d b = {point2d(-h,-w), point2d(h+1,w+1)};
    return b;
  }

  inline
  rect2d make_rectangle2d(unsigned height, unsigned width, point2d center)
  {
    mln_precondition(height % 2 == 1);
    mln_precondition(width % 2 == 1);
    unsigned h = height / 2;
    unsigned w = width / 2;
    point2d uleft = center;
    point2d lright = center;
    uleft[0] -= h;
    uleft[1] -= w;
    lright[0] += h+1;
    lright[1] += w+1;
    box2d b = {uleft, lright};
    return b;
  }

}

#endif // !  MLN_CORE_WIN2D_HPP
