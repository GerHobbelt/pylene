#pragma once

namespace mln::details
{
  template <class T>
  struct ndbuffer_image_impl_base_1
  {
    static std::size_t get_sample_byte_size(const ndbuffer_image_info_t* ima);
    static T* get_pointer(const ndbuffer_image_info_t* ima, std::ptrdiff_t index);
  };

  template <int pdim>
  struct ndbuffer_image_impl_base_2
  {
    static constexpr int get_pdim(const ndbuffer_image_info_t* ima);

    // Get the point of the given index
    static void get_point(const ndbuffer_image_info_t* ima, int index, int coordinates[]);

    // Return the index of the gicen coordinates
    static std::ptrdiff_t get_index(const ndbuffer_image_info_t* ima, const int coordinates[]);

    // Return true if the point belongs to the domain
    static bool is_point_in_domain(const ndbuffer_image_info_t* ima, const int coordinates[]);

    // Return true if the point belongs to the domain or extension
    static bool is_point_valid(const ndbuffer_image_info_t* ima, const int coordinates[]);
  };


  template <class T, int pdim>
  struct ndbuffer_image_impl : ndbuffer_image_impl_base_1<T>, ndbuffer_image_impl_base_2<pdim>
  {
    using ndbuffer_image_impl_base_1<T>::get_pointer;
    using ndbuffer_image_impl_base_1<T>::get_sample_byte_size;
    using ndbuffer_image_impl_base_2<pdim>::get_pdim;
    using ndbuffer_image_impl_base_2<pdim>::get_index;


    // Return a pointer to the loc given by p
    static T* get_pointer(const ndbuffer_image_info_t* ima, const int coordinates[]);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <int pdim>
  constexpr int ndbuffer_image_impl_base_2<pdim>::get_pdim(const ndbuffer_image_info_t*)
  {
    return pdim;
  }

  template <>
  inline constexpr int ndbuffer_image_impl_base_2<-1>::get_pdim(const ndbuffer_image_info_t* ima)
  {
    return ima->m_pdim;
  }

  template <class T>
  std::size_t ndbuffer_image_impl_base_1<T>::get_sample_byte_size(const ndbuffer_image_info_t*)
  {
    return sizeof(T);
  }

  template <>
  inline std::size_t ndbuffer_image_impl_base_1<void>::get_sample_byte_size(const ndbuffer_image_info_t* ima)
  {
    return get_sample_type_id_traits(ima->m_sample_type).size();
  }


  template <int pdim>
  bool ndbuffer_image_impl_base_2<pdim>::is_point_in_domain(const ndbuffer_image_info_t* ima, const int p[])
  {
    for (int k = 0; k < get_pdim(ima); ++k)
      if (p[k] < ima->m_axes[k].domain_begin || ima->m_axes[k].domain_end <= p[k])
        return false;
    return true;
  }

  // Return true if the point belongs to the domain or extension
  template <int pdim>
  bool ndbuffer_image_impl_base_2<pdim>::is_point_valid(const ndbuffer_image_info_t* ima, const int p[])
  {
    for (int k = 0; k < get_pdim(ima); ++k)
      if (p[k] < ima->m_axes[k].vbox_begin || ima->m_axes[k].vbox_end <= p[k])
        return false;
    return true;
  }


  // Return the index of the loc given by p
  template <int pdim>
  std::ptrdiff_t ndbuffer_image_impl_base_2<pdim>::get_index(const ndbuffer_image_info_t* ima, const int coordinates[])
  {
    std::ptrdiff_t i = coordinates[0];
    for (int k = 1; k < get_pdim(ima); ++k)
      i += coordinates[k] * ima->m_axes[k].stride;
    return i;
  }


  // Return a pointer to the loc given by p
  template <class T, int pdim>
  T* ndbuffer_image_impl<T, pdim>::get_pointer(const ndbuffer_image_info_t* ima, const int coordinates[])
  {
    return get_pointer(ima, get_index(ima, coordinates));
  }

  template <class T>
  T* ndbuffer_image_impl_base_1<T>::get_pointer(const ndbuffer_image_info_t* ima, std::ptrdiff_t index)
  {
    return reinterpret_cast<T*>(ima->m_buffer + static_cast<std::ptrdiff_t>(index * get_sample_byte_size(ima)));
  }


  template <int pdim>
  void ndbuffer_image_impl_base_2<pdim>::get_point(const ndbuffer_image_info_t* ima, int index, int coords[])
  {
    std::ptrdiff_t diff = index;
    for (int i = 0; i < get_pdim(ima); i++)
    {
      coords[i] = ima->m_axes[i].vbox_begin;
      diff -= coords[i] * ima->m_axes[i].stride;
    }
    assert(diff >= 0);

    for (int i = get_pdim(ima) - 1; i >= 0; --i)
    {
      auto q = std::div(diff, ima->m_axes[i].stride);
      coords[i] += static_cast<short>(q.quot);
      diff = q.rem;
    }
  }




} // namespace mln::details
