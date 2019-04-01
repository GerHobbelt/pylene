#include <range/v3/span.hpp>
#include <iostream>
#include <memory>
#include <cstddef>

template <typename T = void>
class image2d;

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

    static constexpr std::size_t dyn_sizeof[] =
    {
        sizeof(int8_t),
        sizeof(int16_t),
        sizeof(int32_t),
        sizeof(int64_t),
        sizeof(uint8_t),
        sizeof(uint16_t),
        sizeof(uint32_t),
        sizeof(uint64_t),
        sizeof(float),
        sizeof(double),
        static_cast<std::size_t>(-1)
    };

    static constexpr long double max[] =
    {
        std::numeric_limits<int8_t>::max(),
        std::numeric_limits<int16_t>::max(),
        std::numeric_limits<int32_t>::max(),
        std::numeric_limits<int64_t>::max(),
        std::numeric_limits<uint8_t>::max(),
        std::numeric_limits<uint16_t>::max(),
        std::numeric_limits<uint32_t>::max(),
        std::numeric_limits<uint64_t>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<double>::max(),
        static_cast<std::size_t>(-1)
    };

    type_id val;
};

template <typename type>
struct Trait
{
    static constexpr Info::type_id value = Info::OTHER;
};

template<>
struct Trait<std::int8_t>
{
    static constexpr Info::type_id value = Info::INT8_V;
};

template<>
struct Trait<std::uint8_t>
{
    static constexpr Info::type_id value = Info::UINT8_V;
};

template<>
struct Trait<std::int16_t>
{
    static constexpr Info::type_id value = Info::INT16_V;
};

template<>
struct Trait<std::uint16_t>
{
    static constexpr Info::type_id value = Info::UINT16_V;
};
template<>
struct Trait<std::int32_t>
{
    static constexpr Info::type_id value = Info::INT32_V;
};

template<>
struct Trait<std::uint32_t>
{
    static constexpr Info::type_id value = Info::UINT32_V;
};

template<>
struct Trait<std::int64_t>
{
    static constexpr Info::type_id value = Info::INT64_V;
};

template<>
struct Trait<std::uint64_t>
{
    static constexpr Info::type_id value = Info::UINT64_V;
};

template<>
struct Trait<float>
{
    static constexpr Info::type_id value = Info::FLOAT_V;
};

template<>
struct Trait<double>
{
    static constexpr Info::type_id value = Info::DOUBLE_V;
};

template <typename T>
class image2d_data;

struct image2d_data_base
{
    image2d_data_base(std::size_t n)
        : m_size{n}
    {}

    virtual ~image2d_data_base() = default;

    std::size_t m_size;
    std::byte *m_buf;
};

template<>
class image2d_data<void> : public image2d_data_base
{
public:
    image2d_data() = default;

    image2d_data(std::size_t n, Info::type_id type)
        : image2d_data_base{n * Info::dyn_sizeof[type]}
    {
        m_buf = m_alloc.allocate(m_size);
        std::uninitialized_default_construct_n(m_buf, m_size);
    }

    ~image2d_data() override
    {
        m_alloc.deallocate(m_buf, m_size);
    }

    std::byte *buf() const { return m_buf; }
protected:
    std::allocator<std::byte> m_alloc;
};

template <typename T>
class image2d_data : public image2d_data_base
{
public:
    image2d_data(std::size_t n)
        : image2d_data_base{n}
    {
        m_buf = reinterpret_cast<std::byte*>(m_alloc.allocate(n));
        std::uninitialized_default_construct_n(reinterpret_cast<T*>(m_buf), m_size);
    }

    image2d_data(std::size_t n, T val)
        : image2d_data_base{n}
    {
        m_buf = reinterpret_cast<std::byte*>(m_alloc.allocate(n));
        std::uninitialized_fill_n(reinterpret_cast<T*>(m_buf), m_size, val);
    }

    ~image2d_data() override
    {
        std::destroy_n(reinterpret_cast<T*>(m_buf), m_size);
        m_alloc.deallocate(reinterpret_cast<T*>(m_buf), m_size);
    }

private:
    std::allocator<T> m_alloc;
};

template<>
class image2d<void>
{
    using data_t = std::shared_ptr<image2d_data_base>;
public:
    image2d() = default;
    image2d(std::size_t w, std::size_t h, Info::type_id type)
        : m_width{w}
        , m_height{h}
        , m_type{type}
    {
        m_data = std::make_shared<image2d_data<void>>(h * w, m_type.val);
        m_buffer = m_data->m_buf;
        std::cout << "initialized a non-templated image\n";
    }

    template <typename T>
    image2d(std::size_t w, std::size_t h, Tag<T>)
        : m_width{w}
        , m_height{h}
        , m_type{Trait<T>::value}
    {
        m_data = std::make_shared<image2d_data<T>>(h * w);
        m_buffer = m_data->m_buf;
        std::cout << "initialized a templated img\n";
    }

    template <typename T>
    image2d(size_t w, size_t h, T val)
        : m_width{w}
        , m_height{h}
        , m_type{Trait<T>::value}
    {
        m_data = std::make_shared<image2d_data<T>>(h * w, val);
        m_buffer = m_data->m_buf;
        std::cout << "initialized a templated img with values\n";
    }

    static image2d from_buffer(std::byte* buffer, std::size_t w, std::size_t h, Info::type_id type)
    {
        auto res = image2d(w, h, type);
        res.m_buffer = buffer;
        return res;
    }

    size_t width() const { return m_width; }
    size_t height() const { return m_height; }
    std::byte *buffer() const { return m_buffer; }
    Info type() const { return m_type; }

protected:
    std::byte *m_buffer;            // buffer contained by m_data

private:
    size_t m_width = 0;
    size_t m_height = 0;
    Info m_type = {Info::OTHER};    // info for the type the data
    data_t m_data;                  // ptr to the data
};

template <typename T>
class image2d : public image2d<void>
{
public:
    image2d(std::size_t w, std::size_t h)
        : image2d<void>(w, h, Tag<T>{})
    {
    }

    image2d(std::size_t w, std::size_t h, T val)
        : image2d<void>(w, h, val)
    {
    }

    T *buffer() const
    {
        return reinterpret_cast<T*>(this->image2d<void>::buffer());
    }

    ranges::span<T> values()
    {
        return ranges::span(buffer(), width() * height());
    }

    ranges::span<const T> values() const
    {
        return ranges::span(buffer(), width() * height());
    }

    static image2d<T> from_buffer(T* buffer, std::size_t w, std::size_t h)
    {
        auto res = image2d(w, h);
        res.m_buffer = reinterpret_cast<std::byte*>(buffer);
        return res;
    }
};

template <class T>
image2d<float> stretch(const image2d<T>& src)
{
    auto res = image2d<float>(src.width(), src.height());
    auto span = src.values();
    std::transform(span.begin(), span.end(), res.values().begin(),
        [](T val) -> float
        {
            return static_cast<float>(val) / std::numeric_limits<T>::max();
        });
    return res;
}

image2d<void> stretch(const image2d<>& src)
{
    switch(src.type().val)
    {
        case (Info::INT8_V):
            return stretch(static_cast<const image2d<int8_t>&>(src));
        case (Info::INT16_V):
            return stretch(static_cast<const image2d<int16_t>&>(src));
        default:
            throw std::runtime_error("Data type not implemented");
    }
    /*
    auto res = image2d<float>(src.width(), src.height());
    for (std::size_t i = 0; i < src.height() * src.width(); ++i)
    {
        res.buffer()[i] = reinterpret_cast<float*>(src.buffer())[i] / Info::max[src.type().val];
    }
    return res;
    */
}
//#include "ima.hxx"