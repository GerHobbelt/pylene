#pragma once


/// \file
/// \brief Static integer sequence generator
///

namespace mln
{

  /// \brief Integer sequence type.
  ///
  /// Use int_list_seq to generate a static integer list that can then be used
  /// with parameters unpacking for metaprogramming purpose.
  ///
  /// \code
  /// typedef typename int_list_seq<5>::type S;
  /// // S has type intseq<0,1,2,3,4>
  /// \endcode
  ///
  template <int... N>
  struct intseq
  {
  };

  /// \brief Integer sequence type generator
  /// \see intseq
  template <int N, int... I>
  struct int_list_seq : int_list_seq<N - 1, N - 1, I...>
  {
  };

  template <int... I>
  struct int_list_seq<0, I...>
  {
    typedef intseq<I...> type;
  };

} // namespace mln
