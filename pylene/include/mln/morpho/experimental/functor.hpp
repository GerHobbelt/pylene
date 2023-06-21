#pragma once

namespace mln::morpho::experimental
{
  class functor
  {
    // check static cast
    // mieux preciser les types dans les cast (+ const)
  public:
    void* call_distance(const void* a, const void* b, mln::sample_type_id tid) const
    {
      switch (tid)
      {
      case mln::sample_type_id::BOOL:
        return reinterpret_cast<void*>(distance(*static_cast<const bool*>(a), *static_cast<const bool*>(b)));
      case mln::sample_type_id::UINT8:
        return reinterpret_cast<void*>(distance(*static_cast<const std::uint8_t*>(a), *static_cast<const std::uint8_t*>(b)));
      case mln::sample_type_id::UINT16:
        return reinterpret_cast<void*>(distance(*static_cast<const std::uint16_t*>(a), *static_cast<const std::uint16_t*>(b)));
      case mln::sample_type_id::UINT32:
        return reinterpret_cast<void*>(distance(*static_cast<const std::uint32_t*>(a), *static_cast<const std::uint32_t*>(b)));
      case mln::sample_type_id::UINT64:
        return reinterpret_cast<void*>(distance(*static_cast<const std::uint64_t*>(a), *static_cast<const std::uint64_t*>(b)));
      case mln::sample_type_id::INT8:
        return reinterpret_cast<void*>(distance(*static_cast<const std::int8_t*>(a), *static_cast<const std::int8_t*>(b)));
      case mln::sample_type_id::INT16:
        return reinterpret_cast<void*>(distance(*static_cast<const std::int16_t*>(a), *static_cast<const std::int16_t*>(b)));
      case mln::sample_type_id::INT32:
        return reinterpret_cast<void*>(distance(*static_cast<const std::int32_t*>(a), *static_cast<const std::int32_t*>(b)));
      case mln::sample_type_id::INT64:
        return reinterpret_cast<void*>(distance(*static_cast<const std::int64_t*>(a), *static_cast<const std::int64_t*>(b)));
      /*case mln::sample_type_id::FLOAT:
        return reinterpret_cast<void*>(static_cast<const long&>(distance<std::float_t>(*static_cast<std::float_t*>(a), *static_cast<std::float_t*>(b))));
      case mln::sample_type_id::DOUBLE:
        return reinterpret_cast<void*>(static_cast<const long&>(distance<std::double_t>(*static_cast<std::double_t*>(a), *static_cast<std::double_t*>(b))));*/
      default:
        return 0;
      }
    }
    bool call_compare(const void* a, const void* b, mln::sample_type_id tid) const
    {
      switch (tid)
      {
      case mln::sample_type_id::BOOL:
        return compare(*static_cast<const bool*>(a), *static_cast<const bool*>(b));
      case mln::sample_type_id::UINT8:
        return compare(reinterpret_cast<const std::uint8_t&>(a), reinterpret_cast<const std::uint8_t&>(b));
      /*case mln::sample_type_id::UINT16:
        return compare<std::uint16_t>(reinterpret_cast<const std::uint16_t&>(a), reinterpret_cast<const std::uint16_t&>(b));
      case mln::sample_type_id::UINT32:
        return compare<std::uint32_t>(reinterpret_cast<const std::uint32_t&>(a), reinterpret_cast<const std::uint32_t&>(b));
      case mln::sample_type_id::UINT64:
        return compare<std::uint64_t>(reinterpret_cast<const std::uint64_t&>(a), reinterpret_cast<const std::uint64_t&>(b));
      case mln::sample_type_id::INT8:
        return compare<std::int8_t>(reinterpret_cast<const std::int8_t&>(a), reinterpret_cast<const std::int8_t&>(b));
      case mln::sample_type_id::INT16:
        return compare<std::int16_t>(reinterpret_cast<const std::int16_t&>(a), reinterpret_cast<const std::int16_t&>(b));
      case mln::sample_type_id::INT32:
        return compare<std::int32_t>(reinterpret_cast<const std::int32_t&>(a), reinterpret_cast<const std::int32_t&>(b));
      case mln::sample_type_id::INT64:
        return compare<std::int64_t>(reinterpret_cast<const std::int64_t&>(a), reinterpret_cast<const std::int64_t&>(b));
      case mln::sample_type_id::FLOAT:
        return compare<std::float_t>(reinterpret_cast<const std::float_t&>(a), reinterpret_cast<const std::float_t&>(b));
      case mln::sample_type_id::DOUBLE:
        return compare<std::double_t>(reinterpret_cast<const std::double_t&>(a), reinterpret_cast<const std::double_t&>(b));*/
      default:
        return false;
      }
    }

    bool call_equal(const void* a, const void* b, mln::sample_type_id tid) const
    {
      switch (tid)
      {
      case mln::sample_type_id::BOOL:
        return equal<bool>(*static_cast<const bool*>(a), *static_cast<const bool*>(b));
      case mln::sample_type_id::UINT8:
        return equal<std::uint8_t>(reinterpret_cast<const std::uint8_t&>(a), reinterpret_cast<const std::uint8_t&>(b));
      /*case mln::sample_type_id::UINT16:
        return equal<std::uint16_t>(reinterpret_cast<const std::uint16_t&>(a), reinterpret_cast<const std::uint16_t&>(b));
      case mln::sample_type_id::UINT32:
        return equal<std::uint32_t>(reinterpret_cast<const std::uint32_t&>(a), reinterpret_cast<const std::uint32_t&>(b));
      case mln::sample_type_id::UINT64:
        return equal<std::uint64_t>(reinterpret_cast<const std::uint64_t&>(a), reinterpret_cast<const std::uint64_t&>(b));
      case mln::sample_type_id::INT8:
        return equal<std::int8_t>(reinterpret_cast<const std::int8_t&>(a), reinterpret_cast<const std::int8_t&>(b));
      case mln::sample_type_id::INT16:
        return equal<std::int16_t>(reinterpret_cast<const std::int16_t&>(a), reinterpret_cast<const std::int16_t&>(b));
      case mln::sample_type_id::INT32:
        return equal<std::int32_t>(reinterpret_cast<const std::int32_t&>(a), reinterpret_cast<const std::int32_t&>(b));
      case mln::sample_type_id::INT64:
        return equal<std::int64_t>(reinterpret_cast<const std::int64_t&>(a), reinterpret_cast<const std::int64_t&>(b));
      case mln::sample_type_id::FLOAT:
        return equal<std::float_t>(reinterpret_cast<const std::float_t&>(a), reinterpret_cast<const std::float_t&>(b));
      case mln::sample_type_id::DOUBLE:
        return equal<std::double_t>(reinterpret_cast<const std::double_t&>(a), reinterpret_cast<const std::double_t&>(b));
      */default:
        return false;
      }
    }

  private:
    template <class T>
    T distance(T a, T b) const
    {
      return mln::l2dist(a, b);
    }

    template <class T>
    bool compare(T a, T b) const
    {
      return a < b;
    }

    template <class T>
    bool equal(T a, T b) const
    {
      return a == b;
    }
  };
} // namespace mln::morpho::experimental