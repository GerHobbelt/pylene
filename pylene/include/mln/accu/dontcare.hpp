#pragma once

namespace mln
{

  namespace accu
  {

    struct dontcare
    {
      template <typename T>
      dontcare(T&&){};
    };
  } // namespace accu
} // namespace mln
