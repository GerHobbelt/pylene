#include <cstddef>
#include <memory>
#include <byte>


template <typename T>
class image2d_data;

struct image2d_data_base
{
    image2d_data_base(std::size_t n);

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
