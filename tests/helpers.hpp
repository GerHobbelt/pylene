#ifndef MILENA_HELPERS_H
#define MILENA_HELPERS_H

#include <mln/core/image/image2d.hpp>
#include <mln/io/imprint.hpp>

#include <gtest/gtest.h>

namespace mln
{

  template <class I, class J>
  void imcheck_equal(const Image<I>& f, const Image<J>& g)
  {
    mln_pixter(px1, exact(f));
    mln_pixter(px2, exact(g));

    mln_forall (px1, px2)
    {
      if (px1->point() != px2->point() or px1->val() != px2->val())
      {
        std::stringstream msg;
        msg << "The following images differs:\n";
        io::imprint(f, msg);
        msg << " and\n:";
        io::imprint(g, msg);

        ASSERT_TRUE(msg.str() == ""); // fail
        return;
      }
    }
  }
}

#define MLN_CHECK_IMEQUAL(f, g) mln::imcheck_equal(f, g);

#endif // MILENA_HELPERS_H
