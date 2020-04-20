#pragma once

/// \file Dontcare type (a dummy type used in metaprogramming)

namespace mln
{

  /// \brief dontcare type.
  ///
  /// It can (implicitely) constucted from any type.
  struct dontcare_t
  {

    template <class... T>
    constexpr dontcare_t(T&&...){};

    template <class... T>
    void operator()(T&&...) const
    {
    }
  };

  namespace
  {
    static constexpr dontcare_t dontcare = {};
  }

} // namespace mln
