#ifndef TESTS_HELPERS_H
#define TESTS_HELPERS_H

#include <mln/core/image/image.hpp>
#include <gtest/gtest.h>
#include <mln/io/imprint.hpp>


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
bool compare(const mln::Image<I>& f, const mln::Image<J>& g)
{
  mln_pixter(px1, mln::exact(f));
  mln_pixter(px2, mln::exact(g));

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
  std::string err_compare_msg(const mln::Image<I>& f, const mln::Image<J>& g)
  {
    std::stringstream msg;
    msg << "The following images differs:\n";
    mln::io::imprint(f, msg);
    msg << " and\n:";
    mln::io::imprint(g, msg);
    return msg.str();
  }
}

#endif // TESTS_HELPERS_H
