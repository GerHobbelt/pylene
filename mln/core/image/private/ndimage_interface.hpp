#pragma once

#include <mln/core/imageformat.hpp>
#include <mln/core/domain/box.hpp>

namespace mln
{

  class NDImageInterface
  {
  public:
    using index_type = std::ptrdiff_t;
    using size_type [[deprecated]] = index_type;
    using difference_type [[deprecated]] = index_type;

    virtual ~NDImageInterface() = default;

    /// Value related information
    /// \{

    /// Get the number of channels in the image i.e. the dimension of the co-domain
    /// (this is independant to the data layout).
    virtual int nchannels() const = 0;
    int get_vdim() const { return this->nchannels(); }

    /// Get the type of pixel sample
    virtual sample_type_id_t sample_type_id() const = 0;
    /// \}

    /// Domain related information
    /// \{

    /// Get the dimension of the domain
    virtual int get_pdim() const = 0;

    /// Get the number of elements in the k-th dimension
    virtual int dim(int k = 0) const = 0;
    /// \}


    /// Data storage related information
    /// \{

    /// Get the layout storage (planar or interleaved)
    virtual layout_t layout() const = 0;

    /// Get the number of buffers. This is dependant on the layout, i.e. the number of channels
    /// if the layout is planar. If the layout is interleaved, the number of buffer is 1.
    virtual int nbuffers() const = 0;

    /// Get a pointer to the k-th buffer.
    void* buffer(int k = 0) { return this->__buffer(k); }
    const void* buffer(int k = 0) const { return this->__buffer(k); }

    /// Get the stride, the number of bytes between two consecutives element of the k-th dimension
    virtual std::ptrdiff_t byte_stride(int k = 0) const = 0;
    /// \}

  private:
    virtual void* __buffer(int k = 0) const = 0;
  };

  template <int N, class T = void>
  class ImageNdInterface;


  template <int N>
  class ImageNdInterface<N, void> : public NDImageInterface
  {
  public:
    enum { pdim = N };
    using domain_type = mln::box<int, N>;
    using point_type = mln::point<std::ptrdiff_t, N>;
    using small_point_type = mln::point<short, N>;
    using site_type [[deprecated]] = point_type;

    virtual const domain_type& domain() const = 0;
  };

  template <int N, class T>
  class ImageNdInterface : public ImageNdInterface<N, void>
  {
    using base = NDImageInterface;
  public:
    using sample_type = T;

    /// Get a typed-pointer to the k-th buffer.
    T* buffer(int k = 0) { return reinterpret_cast<T*>(this->base::buffer(k)); }
    const T* buffer(int k = 0) const { return reinterpret_cast<const T*>(this->base::buffer(k)); }
  };


  template <int N, class T>
  class ImageNdPlanarInterface : public ImageNdInterface<N, T>
  {
    //virtual mln::ndimage<N, const T> channel(int k = 0) const = 0;
    //virtual mln::ndimage<N, T>       channel(int k = 0) = 0;
  };

} // end of namespace mln

