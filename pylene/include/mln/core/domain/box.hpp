#pragma once

#include <mln/core/domain/box_iter.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/internal/nested_loop_iterator.hpp>
#include <mln/core/point.hpp>
#include <mln/core/rangev3/multi_indices.hpp>

#if MLN_HAS_TBB
#include <tbb/tbb_stddef.h>
#endif


namespace mln
{
  // Fwd
  // template <typename T, unsigned d> struct box_iter;
  // template <typename T, unsigned d> struct box_rev_iter;

  template <typename T, unsigned d>
  struct strided_box
  {
    typedef point<T, d> point_type;
    typedef point<T, d> value_type;

    typedef internal::nested_loop_iterator<internal::strided_domain_point_visitor_forward<point<T, d>>,
                                           internal::no_op_visitor, internal::no_op_visitor,
                                           internal::point_structure<T, d>, internal::deref_return_point_policy>
        iterator;

    typedef iterator const_iterator;

    typedef internal::nested_loop_iterator<internal::strided_domain_point_visitor_backward<point<T, d>>,
                                           internal::no_op_visitor, internal::no_op_visitor,
                                           internal::point_structure<T, d>, internal::deref_return_point_policy>
        reverse_iterator;

    typedef reverse_iterator const_reverse_iterator;

    strided_box() = default;
    strided_box(const point_type& pmin, const point_type& pmax, const point_type& strides);

    bool       has(const point_type& p) const;
    point_type shape() const;
    bool       empty() const;
    unsigned   size() const;

    bool __is_valid() const;

    iterator         iter() const;
    reverse_iterator riter() const;

    bool operator==(const strided_box& other) const;
    bool operator!=(const strided_box& other) const;

    point_type pmin;
    point_type pmax;
    point_type strides;
  };

  template <typename T, unsigned d>
  strided_box<T, d> make_strided_box(point<T, d> pmin, point<T, d> pmax, point<T, d> step);

  //
  // \pre \tparam T must be an integral type
  template <typename T, unsigned d>
  struct box : ranges::details::multi_indices_facade<d, box<T, d>>
  {
    typedef point<T, d>          point_type;
    typedef point<T, d>          value_type;
    static constexpr std::size_t ndim = d;

    typedef internal::nested_loop_iterator<internal::domain_point_visitor_forward<point<T, d>>, internal::no_op_visitor,
                                           internal::no_op_visitor, internal::point_structure<T, d>,
                                           internal::deref_return_point_policy>
        iterator;

    typedef iterator const_iterator;

    typedef internal::nested_loop_iterator<internal::domain_point_visitor_backward<point<T, d>>,
                                           internal::no_op_visitor, internal::no_op_visitor,
                                           internal::point_structure<T, d>, internal::deref_return_point_policy>
                             reverse_iterator;
    typedef reverse_iterator const_reverse_iterator;

    box() = default;

    constexpr box(const point_type& pmin_, const point_type& pmax_)
      : pmin(pmin_)
      , pmax(pmax_)
    {
    }

    bool has(const point_type& p) const
    {
      mln_precondition(__is_valid());
      for (unsigned i = 0; i < d; ++i)
        if (p[i] < pmin[i] or p[i] >= pmax[i])
          return false;
      return true;
    }

    point_type shape() const
    {
      mln_precondition(__is_valid());
      return pmax - pmin;
    }

    constexpr int dim() const { return ndim; }

    std::array<std::size_t, d> extents() const
    {
      std::array<std::size_t, d> r;
      for (unsigned i = 0; i < d; ++i)
        r[i] = pmax[i] - pmin[i];
      return r;
    }

    bool empty() const
    {
      mln_precondition(__is_valid());
      for (unsigned i = 0; i < d; ++i)
        if (pmax[i] <= pmin[i])
          return true;
      return false;
    }

    unsigned size() const
    {
      mln_precondition(__is_valid());
      unsigned sz = 1;
      for (unsigned i = 0; i < d; ++i)
        sz *= pmax[i] - pmin[i];
      return sz;
    }

    iterator iter() const
    {
      mln_precondition(__is_valid());
      return iterator(internal::point_structure<T, d>(), internal::make_point_visitor_forward(pmin, pmax),
                      internal::no_op_visitor(), internal::no_op_visitor());
    }

    reverse_iterator riter() const
    {
      mln_precondition(__is_valid());
      return reverse_iterator(internal::point_structure<T, d>(), internal::make_point_visitor_backward(pmin, pmax),
                              internal::no_op_visitor(), internal::no_op_visitor());
    }

#if MLN_HAS_TBB

    box(box& r, tbb::split)
    {
      mln_precondition(r.is_divisible());
      pmin      = r.pmin;
      pmax      = r.pmax;
      r.pmax[0] = r.pmin[0] + (pmax[0] - pmin[0]) / 2;
      pmin[0]   = r.pmax[0];
    }

#endif

    bool is_divisible() const
    {
      mln_precondition(__is_valid());
      return (pmax[0] - pmin[0]) > 1;
    }

    void join(const box& other)
    {
      mln_precondition(__is_valid());
      mln_precondition(pmax[0] == other.pmin[0]);
      pmax[0] = other.pmax[0];
    }

    bool __is_valid() const
    {
      for (unsigned i = 0; i < d; ++i)
        if (pmin[i] > pmax[i])
          return false;
        else if (pmin[i] == pmax[i]) // empty <=> pmin = pmax
          return pmin == pmax;
      return true;
    }

    bool operator==(const box& other) const { return pmin == other.pmin and pmax == other.pmax; }

    bool operator!=(const box& other) const { return pmin != other.pmin or pmax != other.pmax; }

    point_type __from() const { return pmin; }
    point_type __to() const { return pmax; }


    point_type pmin;
    point_type pmax;
  };

  typedef box<short, 1> box1d;
  typedef box<float, 1> box1df;
  typedef box<short, 2> box2d;
  typedef box<float, 2> box2df;
  typedef box<short, 3> box3d;
  typedef box<float, 3> box3df;

  typedef strided_box<short, 1> sbox1d;
  typedef strided_box<short, 2> sbox2d;
  typedef strided_box<short, 3> sbox3d;

  template <typename T, unsigned d>
  inline std::ostream& operator<<(std::ostream& s, const box<T, d>& b)
  {
    return s << "[" << b.pmin << " ... " << b.pmax << "]";
  }

  template <typename T, unsigned d>
  inline std::ostream& operator<<(std::ostream& s, const strided_box<T, d>& b)
  {
    return s << "[" << b.pmin << " ... " << b.pmax << "..." << b.strides << "]";
  }

  /// Traits

  /// Forward
  template <typename>
  struct image2d;
  template <typename>
  struct image3d;

  template <>
  struct image_from_domain<box2d>
  {
    template <typename T>
    struct apply
    {
      typedef image2d<T> type;
    };
  };

  template <>
  struct image_from_domain<box3d>
  {
    template <typename T>
    struct apply
    {
      typedef image3d<T> type;
    };
  };

  template <>
  struct image_from_domain<sbox2d>
  {
    template <typename T>
    struct apply
    {
      typedef image2d<T> type;
    };
  };

  template <>
  struct image_from_domain<sbox3d>
  {
    template <typename T>
    struct apply
    {
      typedef image3d<T> type;
    };
  };

  template <typename T, unsigned d>
  struct grain_box : box<T, d>
  {
  private:
    typedef box<T, d> base;

  public:
    grain_box()                 = default;
    grain_box(const grain_box&) = default;

    grain_box(const box<T, d>& b, unsigned grain = 1)
      : base(b)
      , m_grain(grain)
    {
      mln_assertion(grain > 0);
    }

    bool is_divisible() const
    {
      mln_precondition(this->__is_valid());
      return (this->pmax[0] - this->pmin[0]) > (int)m_grain;
    }

#if MLN_HAS_TBB

    grain_box(grain_box& r, tbb::split)
      : base(r, tbb::split())
      , m_grain(r.m_grain)
    {
    }

#endif

  private:
    unsigned m_grain;
  };

  typedef grain_box<short, 1> grain_box1d;
  typedef grain_box<float, 1> grain_box1df;
  typedef grain_box<short, 2> grain_box2d;
  typedef grain_box<float, 2> grain_box2df;
  typedef grain_box<short, 3> grain_box3d;
  typedef grain_box<float, 3> grain_box3df;

  /**************************/
  /** Implementation        */
  /**************************/

  template <typename T, unsigned d>
  strided_box<T, d>::strided_box(const point_type& pmin_, const point_type& pmax_, const point_type& strides_)
    : pmin(pmin_)
    , strides(strides_)
  {
    mln_precondition(pmin_ <= pmax_);
    for (unsigned i = 0; i < d; ++i)
    {
      T q = (pmax_[i] - pmin_[i]) % strides_[i];
      if (q > 0)
        pmax[i] = pmax_[i] - q + strides_[i];
      else
        pmax[i] = pmax_[i];
    }
  }

  template <typename T, unsigned d>
  inline bool strided_box<T, d>::has(const point_type& p) const
  {
    mln_precondition(__is_valid());
    for (unsigned i = 0; i < d; ++i)
      if (p[i] < pmin[i] or p[i] >= pmax[i] or (p[i] - pmin[i]) % strides[i] != 0)
        return false;
    return true;
  }

  template <typename T, unsigned d>
  inline bool strided_box<T, d>::__is_valid() const
  {
    for (unsigned i = 0; i < d; ++i)
      if (pmin[i] > pmax[i])
        return false;
      else if (pmin[i] == pmax[i]) // empty <=> pmin = pmax
        return pmin == pmax;
    return true;
  }

  template <typename T, unsigned d>
  inline typename strided_box<T, d>::point_type strided_box<T, d>::shape() const
  {
    point_type shp((pmax - pmin).as_vec() / strides.as_vec());
    return shp;
  }

  template <typename T, unsigned d>
  inline bool strided_box<T, d>::empty() const
  {
    for (unsigned i = 0; i < d; ++i)
      if (pmax[i] < pmin[i] + strides[i])
        return true;
    return false;
  }

  template <typename T, unsigned d>
  inline typename strided_box<T, d>::iterator strided_box<T, d>::iter() const
  {
    return iterator(internal::point_structure<T, d>(),
                    internal::make_strided_point_visitor_forward(pmin, pmax, strides), internal::no_op_visitor(),
                    internal::no_op_visitor());
  }

  template <typename T, unsigned d>
  inline typename strided_box<T, d>::reverse_iterator strided_box<T, d>::riter() const
  {
    return reverse_iterator(internal::point_structure<T, d>(),
                            internal::make_strided_point_visitor_backward(pmin, pmax, strides),
                            internal::no_op_visitor(), internal::no_op_visitor());
  }

  template <typename T, unsigned d>
  inline bool strided_box<T, d>::operator==(const strided_box& other) const
  {
    return pmin == other.pmin and pmax == other.pmax and strides == other.strides;
  }

  template <typename T, unsigned d>
  inline bool strided_box<T, d>::operator!=(const strided_box& other) const
  {
    return pmin != other.pmin or pmax != other.pmax or strides != other.strides;
  }


  template <typename T, unsigned d>
  strided_box<T, d> make_strided_box(point<T, d> pmin, point<T, d> pmax, point<T, d> step)
  {
    return {pmin, pmax, step};
  }
} // namespace mln
