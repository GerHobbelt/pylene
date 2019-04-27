#pragma once

#include <mln/core/image/private/image_builder.hpp>

#include "image_format.hpp"
#include <functional>
#include <memory>


namespace mln
{
  constexpr int PYLENE_NDBUFFER_DEFAULT_DIM = 4;

  /// TbeI
  struct Point
  {
    constexpr Point(int x = 0, int y = 0, int z = 0, int w = 0)
      : data{x, y, z, w}
    {
    }

    constexpr int  operator[](int k) const { return data[k]; }
    constexpr int& operator[](int k) { return data[k]; }
    int            data[PYLENE_NDBUFFER_DEFAULT_DIM];
  };

  struct Box
  {
    constexpr Box() = default;

    constexpr Box(int width, int height = 1, int depth = 1, int w = 1)
      : begin{}
      , end{width, height, depth, w}
    {
    }

    constexpr Box(Point tl, Point br)
      : begin{tl}
      , end{br}
    {
    }

    int get_real_dim() const
    {
      int i = PYLENE_NDBUFFER_DEFAULT_DIM - 1;
      while (i >= 0 && (end[i] - begin[i]) <= 1)
        i--;
      return i + 1;
    }

    Point begin;
    Point end;
  };

  ///
  template <class T, int N>
  class __ndbuffer_image;

  namespace internal
  {
    struct ndbuffer_image_data;
  }

  class ndbuffer_image
  {
  public:
    static constexpr int DEFAULT_BORDER_SIZE = 3;
    static constexpr int DEFAULT_DIM         = 4;


    /// \brief Constructors
    /// \{
    ndbuffer_image();
    ndbuffer_image(sample_type_id sample_type, int width, int height = 1, int depth = 1);
    ndbuffer_image(sample_type_id sample_type, int width, const image_build_params& params);
    ndbuffer_image(sample_type_id sample_type, int width, int height, const image_build_params& params);
    ndbuffer_image(sample_type_id sample_type, int width, int height, int depth, const image_build_params& params);

    ndbuffer_image(sample_type_id sample_type, Box domain);
    ndbuffer_image(sample_type_id sample_type, Box domain, const image_build_params&);

    ndbuffer_image(const ndbuffer_image& other, const image_build_params&);
    /// \}


    /// \brief Resizer
    /// \{
  public:
    void resize(sample_type_id sample_type, int width, int height = 1, int depth = 1);
    void resize(sample_type_id sample_type, int width, const image_build_params&);
    void resize(sample_type_id sample_type, int width, int height, const image_build_params&);
    void resize(sample_type_id sample_type, int width, int height, int depth, const image_build_params&);
    void resize(sample_type_id sample_type, Box domain);
    void resize(sample_type_id sample_type, Box domain, const image_build_params&);
    void resize(const ndbuffer_image& other, image_build_params params);

    // unsafe API (consider make it private ?)
    void resize(sample_type_id sample_type, int dim, int sizes[], const image_build_params&);
    void resize(sample_type_id sample_type, int dim, int topleft[], int sizes[], const image_build_params&);
    /// \}


  public:
    /// \{
    static ndbuffer_image from_buffer(std::byte* buffer, sample_type_id sample_type, int dim, int sizes[],
                                      std::ptrdiff_t strides[] = nullptr, bool copy = false);
    static ndbuffer_image from_buffer(std::byte* buffer, sample_type_id sample_type, int dim, int topleft[],
                                      int sizes[], std::ptrdiff_t strides[] = nullptr, bool copy = false);
    /// \}

    /// \brief Geometry information
    /// \{
    int pdim() const;
    int width() const;
    int height() const;
    int depth() const;
    int size(int dim = 0) const;

    int border() const;
    Box domain() const;
    /// \}


    /// \brief Data and layout information
    /// \{
    sample_type_id sample_type() const;
    std::byte*     buffer() const;
    std::ptrdiff_t byte_stride(int dim = 0) const;
    std::ptrdiff_t stride(int dim = 0) const;

    // FIXME: To Be Implemented
    // anyspan values() const;
    // anyspan values();
    /// \}


    /// Casting
    /// \{
    template <class T, int N>
    const __ndbuffer_image<T, N>* cast_to() const;

    template <class T, int N>
    __ndbuffer_image<T, N>* cast_to();
    /// \}


  private:
    std::byte* __at(const int coords[]) const;

  protected:
    using alloc_fun_t = std::function<std::byte*(sample_type_id, std::size_t, const image_build_params&,
                                                 std::shared_ptr<internal::ndbuffer_image_data>&)>;

    // Note __init is called from a constructor, we need to pass the allocation function from children (it cannot be
    // made virtual !!) Create from a custom storage (either external or internal)
    void __init(alloc_fun_t __allocate, sample_type_id sample_type, int dim, int topleft[], int sizes[],
                std::ptrdiff_t strides[], const image_build_params& params);

  private:
    struct axis_info_t
    {
      std::ptrdiff_t stride;
      int domain_begin;
      int domain_end;
      int vbox_begin;
      int vbox_end;
    };


    std::shared_ptr<internal::ndbuffer_image_data> m_data;
    sample_type_id m_sample_type; // Sample type of the data
    int            m_pdim;        // Dim of the domain
    axis_info_t    m_axes[PYLENE_NDBUFFER_DEFAULT_DIM];
    std::byte*     m_buffer; // Pointer to origin (0,0) (might be out of range)
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class T, int N>
  const __ndbuffer_image<T, N>* ndbuffer_image::cast_to() const
  {
    if (m_pdim == N && sample_type_traits<T>::id() == m_sample_type)
      return static_cast<const __ndbuffer_image<T, N>*>(this);

    return nullptr;
  }

  template <class T, int N>
  __ndbuffer_image<T, N>* ndbuffer_image::cast_to()
  {
    if (m_pdim == N && sample_type_traits<T>::id() == m_sample_type)
      return static_cast<__ndbuffer_image<T, N>*>(this);

    return nullptr;
  }


} // namespace mln
