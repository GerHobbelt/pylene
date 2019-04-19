#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>


namespace mln::py
{
  template <typename T>
  class Tag
  {
  };

  struct Info
  {
    enum type_id
    {
      INT8_V,
      INT16_V,
      INT32_V,
      INT64_V,
      UINT8_V,
      UINT16_V,
      UINT32_V,
      UINT64_V,
      FLOAT_V,
      DOUBLE_V,
      OTHER
    };

    static constexpr std::size_t dyn_sizeof[] = {sizeof(int8_t),
                                                 sizeof(int16_t),
                                                 sizeof(int32_t),
                                                 sizeof(int64_t),
                                                 sizeof(uint8_t),
                                                 sizeof(uint16_t),
                                                 sizeof(uint32_t),
                                                 sizeof(uint64_t),
                                                 sizeof(float),
                                                 sizeof(double),
                                                 static_cast<std::size_t>(-1)};

    static constexpr long double max[] = {std::numeric_limits<int8_t>::max(),   //
                                          std::numeric_limits<int16_t>::max(),  //
                                          std::numeric_limits<int32_t>::max(),  //
                                          std::numeric_limits<int64_t>::max(),  //
                                          std::numeric_limits<uint8_t>::max(),  //
                                          std::numeric_limits<uint16_t>::max(), //
                                          std::numeric_limits<uint32_t>::max(), //
                                          std::numeric_limits<uint64_t>::max(), //
                                          std::numeric_limits<float>::max(),    //
                                          std::numeric_limits<double>::max(),   //
                                          static_cast<std::size_t>(-1)};
    constexpr std::size_t type_sizeof()
    {
        return dyn_sizeof[val];
    }

    type_id val;

    constexpr Info(type_id t) : val{t}
    {
    }


  };

  template <typename type>
  struct Trait
  {
    static constexpr Info::type_id value = Info::OTHER;
  };

  template <>
  struct Trait<std::int8_t>
  {
    static constexpr Info::type_id value = Info::INT8_V;
  };

  template <>
  struct Trait<std::uint8_t>
  {
    static constexpr Info::type_id value = Info::UINT8_V;
  };

  template <>
  struct Trait<std::int16_t>
  {
    static constexpr Info::type_id value = Info::INT16_V;
  };

  template <>
  struct Trait<std::uint16_t>
  {
    static constexpr Info::type_id value = Info::UINT16_V;
  };
  template <>
  struct Trait<std::int32_t>
  {
    static constexpr Info::type_id value = Info::INT32_V;
  };

  template <>
  struct Trait<std::uint32_t>
  {
    static constexpr Info::type_id value = Info::UINT32_V;
  };

  template <>
  struct Trait<std::int64_t>
  {
    static constexpr Info::type_id value = Info::INT64_V;
  };

  template <>
  struct Trait<std::uint64_t>
  {
    static constexpr Info::type_id value = Info::UINT64_V;
  };

  template <>
  struct Trait<float>
  {
    static constexpr Info::type_id value = Info::FLOAT_V;
  };

  template <>
  struct Trait<double>
  {
    static constexpr Info::type_id value = Info::DOUBLE_V;
  };

} // namespace mln::py
