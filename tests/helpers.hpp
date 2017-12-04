#ifndef TESTS_HELPERS_H
#define TESTS_HELPERS_H

#include <mln/core/image/image.hpp>
#include <mln/io/imprint.hpp>
#include <gtest/gtest.h>


/// \brief Compare if two images are equals
///
/// \param[in] f First image to compare
/// \param[in] g Second image to compare
/// Two image are equals iff every pixel (point + value) are equals.
template <class I, class J>
bool compare(const mln::Image<I>& f, const mln::Image<J>& g);


/// \brief GTest macro for image equality test
#define ASSERT_IMAGES_EQ(f, g) ASSERT_TRUE(::compare(f, g)) << ::impl::err_compare_msg(f, g)

/******************************************/
/****          Implementation          ****/
/******************************************/

template <class I, class J>
bool compare(const mln::Image<I>& reference, const mln::Image<J>& input)
{
  mln_pixter(px1, mln::exact(reference));
  mln_pixter(px2, mln::exact(input));

  mln_forall (px1, px2)
    {
      if (px1->point() != px2->point() or px1->val() != px2->val())
        return false;
    }
  if (px1.finished() && px2.finished())
    return true;
  return false;
}

namespace impl
{
  template <class I, class J>
  std::string err_compare_msg(const mln::Image<I>& reference, const mln::Image<J>& input)
  {
    std::stringstream msg;
    msg << "The following images differs:\n";
    mln::io::imprint(reference, msg);
    msg << " and\n:";
    mln::io::imprint(input, msg);
    return msg.str();
  }
}

#endif // TESTS_HELPERS_H
