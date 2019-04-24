#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>


namespace mln::py
{
  template <typename T>
  class Tag
  {
  };

  class Info
  {
  public:
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

    constexpr Info(type_id tid)
      : m_tid{tid}
    {
    }

    constexpr type_id            tid() const { return m_tid; }
    constexpr std::size_t        dyn_sizeof() const { return m_sizeof[m_tid]; }
    static constexpr std::size_t dyn_sizeof(type_id tid) { return m_sizeof[tid]; }
    constexpr std::size_t        max() const { return m_max[m_tid]; }
    static constexpr std::size_t max(type_id tid) { return m_max[tid]; }

  private:
    type_id m_tid;

    static constexpr std::size_t m_sizeof[] = {
        sizeof(std::int8_t),  sizeof(std::int16_t),  sizeof(std::int32_t),        sizeof(std::int64_t),
        sizeof(std::uint8_t), sizeof(std::uint16_t), sizeof(std::uint32_t),       sizeof(std::uint64_t),
        sizeof(float),        sizeof(double),        static_cast<std::size_t>(-1)};

    static constexpr long double m_max[] = {std::numeric_limits<std::int8_t>::max(),
                                            std::numeric_limits<std::int16_t>::max(),  //
                                            std::numeric_limits<std::int32_t>::max(),  //
                                            std::numeric_limits<std::int64_t>::max(),  //
                                            std::numeric_limits<std::uint8_t>::max(),  //
                                            std::numeric_limits<std::uint16_t>::max(), //
                                            std::numeric_limits<std::uint32_t>::max(), //
                                            std::numeric_limits<std::uint64_t>::max(), //
                                            std::numeric_limits<float>::max(),         //
                                            std::numeric_limits<double>::max(),        //
                                            static_cast<std::size_t>(-1)};
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

  template <typename type>
  inline constexpr const auto Trait_v = Trait<type>::value;


  template <Info::type_id tid>
  struct VTrait
  {
    using type = void;
  };

  template <>
  struct VTrait<Info::UINT8_V>
  {
    using type = std::uint8_t;
  };

  template <>
  struct VTrait<Info::UINT16_V>
  {
    using type = std::uint16_t;
  };

  template <>
  struct VTrait<Info::UINT32_V>
  {
    using type = std::uint32_t;
  };

  template <>
  struct VTrait<Info::UINT64_V>
  {
    using type = std::uint64_t;
  };

  template <>
  struct VTrait<Info::INT8_V>
  {
    using type = std::int8_t;
  };

  template <>
  struct VTrait<Info::INT16_V>
  {
    using type = std::int16_t;
  };

  template <>
  struct VTrait<Info::INT32_V>
  {
    using type = std::int32_t;
  };

  template <>
  struct VTrait<Info::INT64_V>
  {
    using type = std::int64_t;
  };

  template <>
  struct VTrait<Info::FLOAT_V>
  {
    using type = float;
  };

  template <>
  struct VTrait<Info::DOUBLE_V>
  {
    using type = double;
  };

  template <Info::type_id tid>
  using VTrait_t = typename VTrait<tid>::type;

  template <template <typename> class F, typename... Args>
  void visit(Info::type_id tid, Args&&... args)
  {
    switch (tid)
    {
    case (Info::INT8_V):
      F<std::int8_t>{}(std::forward<Args>(args)...);
      break;
    case (Info::INT16_V):
      F<std::int16_t>{}(std::forward<Args>(args)...);
      break;
    case (Info::INT32_V):
      F<std::int32_t>{}(std::forward<Args>(args)...);
      break;
    case (Info::INT64_V):
      F<std::int64_t>{}(std::forward<Args>(args)...);
      break;
    case (Info::UINT8_V):
      F<std::uint8_t>{}(std::forward<Args>(args)...);
      break;
    case (Info::UINT16_V):
      F<std::uint16_t>{}(std::forward<Args>(args)...);
      break;
    case (Info::UINT32_V):
      F<std::uint32_t>{}(std::forward<Args>(args)...);
      break;
    case (Info::UINT64_V):
      F<std::uint64_t>{}(std::forward<Args>(args)...);
      break;
    case (Info::FLOAT_V):
      F<float>{}(std::forward<Args>(args)...);
      break;
    case (Info::DOUBLE_V):
      F<double>{}(std::forward<Args>(args)...);
      break;
    default:
      throw std::runtime_error("Unhandled data type");
    }
  }

  template <template <typename> class F, typename... Args>
  auto visit_r(Info::type_id tid, Args&&... args)
  {
    switch (tid)
    {
    case (Info::INT8_V):
      return F<std::int8_t>{}(std::forward<Args>(args)...);
    case (Info::INT16_V):
      return F<std::int16_t>{}(std::forward<Args>(args)...);
    case (Info::INT32_V):
      return F<std::int32_t>{}(std::forward<Args>(args)...);
    case (Info::INT64_V):
      return F<std::int64_t>{}(std::forward<Args>(args)...);
    case (Info::UINT8_V):
      return F<std::uint8_t>{}(std::forward<Args>(args)...);
    case (Info::UINT16_V):
      return F<std::uint16_t>{}(std::forward<Args>(args)...);
    case (Info::UINT32_V):
      return F<std::uint32_t>{}(std::forward<Args>(args)...);
    case (Info::UINT64_V):
      return F<std::uint64_t>{}(std::forward<Args>(args)...);
    case (Info::FLOAT_V):
      return F<float>{}(std::forward<Args>(args)...);
    case (Info::DOUBLE_V):
      return F<double>{}(std::forward<Args>(args)...);
    default:
      throw std::runtime_error("Unhandled data type");
    }
  }

} // namespace mln::py
