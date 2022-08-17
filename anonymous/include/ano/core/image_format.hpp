#pragma once

#include <cstddef>
#include <cstdint>

namespace ano
{
  // Forward declaration of some types.
  namespace internal
  {
    template <typename T, unsigned dim, typename tag>
    struct vec_base;
  }
  struct rgb_tag;
  using rgb8 = internal::vec_base<uint8_t, 3, rgb_tag>;


  enum class sample_type_id
  {
    OTHER = 0, // It has to be 0 (0-init makes it the default)
    BOOL,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT,
    DOUBLE,
    RGB8,
  };

  template <typename T>
  struct sample_type_traits;

  template <sample_type_id st>
  struct sample_type_id_traits;

  namespace details
  {
    template <sample_type_id st, class T, std::size_t S = sizeof(T)>
    struct sample_link
    {
      using type = T;
      static constexpr sample_type_id id() { return st; }
      static constexpr std::size_t    size() { return S; }
    };
  } // namespace details

  template <>
  struct sample_type_id_traits<sample_type_id::BOOL> : details::sample_link<sample_type_id::BOOL, bool>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::UINT8> : details::sample_link<sample_type_id::UINT8, std::uint8_t>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::UINT16> : details::sample_link<sample_type_id::UINT16, std::uint16_t>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::UINT32> : details::sample_link<sample_type_id::UINT32, std::uint32_t>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::UINT64> : details::sample_link<sample_type_id::UINT64, std::uint64_t>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::INT8> : details::sample_link<sample_type_id::INT8, std::int8_t>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::INT16> : details::sample_link<sample_type_id::INT16, std::int16_t>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::INT32> : details::sample_link<sample_type_id::INT32, std::int32_t>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::INT64> : details::sample_link<sample_type_id::INT64, std::int64_t>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::FLOAT> : details::sample_link<sample_type_id::FLOAT, float>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::DOUBLE> : details::sample_link<sample_type_id::DOUBLE, double>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::RGB8> : details::sample_link<sample_type_id::RGB8, rgb8, 3>
  {
  };
  template <>
  struct sample_type_id_traits<sample_type_id::OTHER> : details::sample_link<sample_type_id::OTHER, void, 0>
  {
  };

  template <>
  struct sample_type_traits<bool> : details::sample_link<sample_type_id::BOOL, bool>
  {
  };
  template <>
  struct sample_type_traits<std::uint8_t> : details::sample_link<sample_type_id::UINT8, std::uint8_t>
  {
  };
  template <>
  struct sample_type_traits<std::uint16_t> : details::sample_link<sample_type_id::UINT16, std::uint16_t>
  {
  };
  template <>
  struct sample_type_traits<std::uint32_t> : details::sample_link<sample_type_id::UINT32, std::uint32_t>
  {
  };
  template <>
  struct sample_type_traits<std::uint64_t> : details::sample_link<sample_type_id::UINT64, std::uint64_t>
  {
  };
  template <>
  struct sample_type_traits<std::int8_t> : details::sample_link<sample_type_id::INT8, std::int8_t>
  {
  };
  template <>
  struct sample_type_traits<std::int16_t> : details::sample_link<sample_type_id::INT16, std::int16_t>
  {
  };
  template <>
  struct sample_type_traits<std::int32_t> : details::sample_link<sample_type_id::INT32, std::int32_t>
  {
  };
  template <>
  struct sample_type_traits<std::int64_t> : details::sample_link<sample_type_id::INT64, std::int64_t>
  {
  };
  template <>
  struct sample_type_traits<float> : details::sample_link<sample_type_id::FLOAT, float>
  {
  };
  template <>
  struct sample_type_traits<double> : details::sample_link<sample_type_id::DOUBLE, double>
  {
  };
  template <>
  struct sample_type_traits<rgb8> : details::sample_link<sample_type_id::RGB8, rgb8, 3>
  {
  };
  template <typename T>
  struct sample_type_traits : details::sample_link<sample_type_id::OTHER, T, 0>
  {
  };


  struct dyn_sample_type_id_traits
  {
    using type = void;
    dyn_sample_type_id_traits(sample_type_id st);

    sample_type_id id() const { return m_id; }
    std::size_t    size() const { return m_size; }
    bool           is_multichannel() const { return m_id == sample_type_id::RGB8; }

  private:
    sample_type_id m_id;
    std::size_t    m_size;
  };

  dyn_sample_type_id_traits get_sample_type_id_traits(sample_type_id st);
} // namespace ano
