#pragma once

#include <mln/core/image/ndimage.hpp>

#include <stdexcept>

namespace mln::py
{
  template <template <typename> class F, typename... Args>
  auto visit(mln::sample_type_id tid, Args&&... args)
  {
    switch (tid)
    {
    case (mln::sample_type_id::INT8):
      return F<std::int8_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::INT16):
      return F<std::int16_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::INT32):
      return F<std::int32_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::INT64):
      return F<std::int64_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::UINT8):
      return F<std::uint8_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::UINT16):
      return F<std::uint16_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::UINT32):
      return F<std::uint32_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::UINT64):
      return F<std::uint64_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::FLOAT):
      return F<float>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::DOUBLE):
      return F<double>{}(std::forward<Args>(args)...);
    // case (mln::sample_type_id::RGB8):
    //   return F<mln::rgb8>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::OTHER):
      [[fallthrough]];
    default:
      throw std::runtime_error("Unhandled data type");
    }
  }

  template <template <typename> class F, typename D, typename... Args>
  auto visit_d(mln::sample_type_id tid, D&& d, Args&&... args)
  {
    switch (tid)
    {
    case (mln::sample_type_id::INT8):
      return F<std::int8_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::INT16):
      return F<std::int16_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::INT32):
      return F<std::int32_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::INT64):
      return F<std::int64_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::UINT8):
      return F<std::uint8_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::UINT16):
      return F<std::uint16_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::UINT32):
      return F<std::uint32_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::UINT64):
      return F<std::uint64_t>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::FLOAT):
      return F<float>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::DOUBLE):
      return F<double>{}(std::forward<Args>(args)...);
    // case (mln::sample_type_id::RGB8):
    //   return F<mln::rgb8>{}(std::forward<Args>(args)...);
    case (mln::sample_type_id::OTHER):
      [[fallthrough]];
    default:
      return std::forward<D>(d);
    }
  }
} // namespace mln::py
