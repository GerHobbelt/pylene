#pragma once

#include <type_traits>
#include <mln/core/grays.hpp>

/// \file

namespace mln
{

  enum class sample_t
  {
    BIT,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64,
    FLOAT,
    DOUBLE,
    OTHER
  };


  enum class layout_t
  {
    INTERLEAVED,
    INTERLEAVED_BY_LINE,
    PLANAR,
    UNKNOWN
  };


  template <class T>
  struct sample_type_of : std::integral_constant<sample_t, sample_t::OTHER> {};

  template <>
  struct sample_type_of<mln::int8> : std::integral_constant<sample_t, sample_t::INT8> {};

  template <>
  struct sample_type_of<mln::uint8> : std::integral_constant<sample_t, sample_t::UINT8> {};

  template <>
  struct sample_type_of<mln::int16> : std::integral_constant<sample_t, sample_t::INT16> {};

  template <>
  struct sample_type_of<mln::uint16> : std::integral_constant<sample_t, sample_t::UINT16> {};

  template <>
  struct sample_type_of<mln::int32> : std::integral_constant<sample_t, sample_t::INT32> {};

  template <>
  struct sample_type_of<mln::uint32> : std::integral_constant<sample_t, sample_t::UINT32> {};

  template <>
  struct sample_type_of<mln::int64> : std::integral_constant<sample_t, sample_t::INT64> {};

  template <>
  struct sample_type_of<mln::uint64> : std::integral_constant<sample_t, sample_t::UINT64> {};

  template <>
  struct sample_type_of<float> : std::integral_constant<sample_t, sample_t::FLOAT> {};

  template <>
  struct sample_type_of<double> : std::integral_constant<sample_t, sample_t::DOUBLE> {};


} // end of namespace mln


