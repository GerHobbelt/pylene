#include <mln/python/numpy_format.hpp>

namespace pln
{
    namespace details
    {
        template <mln::sample_type_id t>
        using numpy_desc = pybind11::format_descriptor<typename mln::sample_type_id_traits<t>::type>;
    }

    std::string get_sample_type(mln::sample_type_id type)
    {
        switch (type)
        {
            case mln::sample_type_id::INT8: return details::numpy_desc<mln::sample_type_id::INT8>::format();
            case mln::sample_type_id::INT16: return details::numpy_desc<mln::sample_type_id::INT16>::format();
            case mln::sample_type_id::INT32: return details::numpy_desc<mln::sample_type_id::INT32>::format();
            case mln::sample_type_id::INT64: return details::numpy_desc<mln::sample_type_id::INT64>::format();
            case mln::sample_type_id::UINT8: return details::numpy_desc<mln::sample_type_id::UINT8>::format();
            case mln::sample_type_id::UINT16: return details::numpy_desc<mln::sample_type_id::UINT16>::format();
            case mln::sample_type_id::UINT32: return details::numpy_desc<mln::sample_type_id::UINT32>::format();
            case mln::sample_type_id::UINT64: return details::numpy_desc<mln::sample_type_id::UINT64>::format();
            case mln::sample_type_id::FLOAT: return details::numpy_desc<mln::sample_type_id::FLOAT>::format();
            case mln::sample_type_id::DOUBLE: return details::numpy_desc<mln::sample_type_id::DOUBLE>::format();
            case mln::sample_type_id::BOOL: return details::numpy_desc<mln::sample_type_id::BOOL>::format();
            case mln::sample_type_id::RGB8: return details::numpy_desc<mln::sample_type_id::UINT8>::format();
            default: return std::string("");
        }
    }

    mln::sample_type_id get_sample_type(const std::string& s)
    {
        if (s.size() > 1)
            return mln::sample_type_id::OTHER;

        char type = s[0];
        switch (type)
        {
            case pybind11::format_descriptor<std::int8_t>::value[0]: return mln::sample_type_id::INT8;
            case pybind11::format_descriptor<std::int16_t>::value[0]: return mln::sample_type_id::INT16;
            case pybind11::format_descriptor<std::int32_t>::value[0]: return mln::sample_type_id::INT32;
            case 'l':
            case pybind11::format_descriptor<std::int64_t>::value[0]: return mln::sample_type_id::INT64;
            case pybind11::format_descriptor<std::uint8_t>::value[0]: return mln::sample_type_id::UINT8;
            case pybind11::format_descriptor<std::uint16_t>::value[0]: return mln::sample_type_id::UINT16;
            case pybind11::format_descriptor<std::uint32_t>::value[0]: return mln::sample_type_id::UINT32;
            case 'L':
            case pybind11::format_descriptor<std::uint64_t>::value[0]: return mln::sample_type_id::UINT64;
            case pybind11::format_descriptor<float>::value[0]: return mln::sample_type_id::FLOAT;
            case pybind11::format_descriptor<double>::value[0]: return mln::sample_type_id::DOUBLE;
            case pybind11::format_descriptor<bool>::value[0]: return mln::sample_type_id::BOOL;
            default: return mln::sample_type_id::OTHER;
        }
    }
}