#ifndef MLN_CORE_IMAGE_INTERNAL_NESTED_LOOP_ITERATOR_HPP
# define MLN_CORE_IMAGE_INTERNAL_NESTED_LOOP_ITERATOR_HPP

# include <type_traits>
# include <utility>
# include <array>

# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/literal/vectorial.hpp>

namespace mln
{

  namespace internal
  {


    struct iterator_core_access
    {
      template <typename S>
      static decltype(auto) get_point(S& v) { return v.get_point(); }

      template <typename S>
      static decltype(auto) get_value(S& v) { return v.get_value(); }

      template <typename S>
      static decltype(auto) get_index(S& v) { return v.get_index(); }

      template <typename S>
      static decltype(auto) get_offset(S& v) { return v.get_offset(); }


      template <typename S, typename T>
      static bool equal(const S& v, const T& other) { return v.equal(other); }
    };

    struct nested_loop_pixel_structure_base
    {
    protected:
      friend iterator_core_access;

      std::nullptr_t get_value()	{ return nullptr; }
      std::nullptr_t get_value() const  { return nullptr; }
      std::nullptr_t get_index()        { return nullptr; }
      std::nullptr_t get_index() const  { return nullptr; }
      std::nullptr_t get_offset()        { return nullptr; }
      std::nullptr_t get_offset() const  { return nullptr; }
    };



    /// \defgroup _nested_loop_iterator Nested loop iterator facilities
    /// \{

    /// \brief Implement an iterator that has the semantic:
    ///
    /// \code
    /// for (p.init<0>(), v.init<0>(); p.finished(); p.inc<0>(), v.inc<0>())
    ///   for (p.init<1>(), v.init<1>(); p.finished(); p.inc<1>(), v.inc<1>())
    ///    .
    ///      .
    ///        yield
    /// \endcode
    ///
    /// \tparam PointVisitor
    template <typename PointVisitor, typename ValueVisitor, typename IndexVisitor,
	      typename InternalStruct, typename DereferencePolicy>
    struct nested_loop_iterator;


    /// \defgroup _deref_policy_ Dereference policies
    /// A dereference policy \p Deref provides:
    /// * `Deref::type<S>`  The return type of the object after dereferenciation
    /// * `Deref::type<S> Deref::dereference(const S&)` Dereferenciation operator
    /// \{
    struct deref_return_structure_policy;
    struct deref_return_value_policy;
    struct deref_return_point_policy;
    /// \}


    /// \defgroup _point_visitor_ Point Visitor
    /// A PointVisitor is a policy that provides behaviors about
    /// the way of handling the point
    /// The point is the object that guides the iteration.
    /// The point visitor \p PVis provides:
    /// * `PVis::point_type`
    /// * `PVis::initialize(P&)`
    /// * `PVis::init<n>(P&)`
    /// * `PVis::next<n>(P&)`
    /// * `PVis::finished<n>(P&)`
    /// \{
    template <typename Point> struct origin_point_visitor_forward;
    template <typename Point> struct origin_point_visitor_backward;
    template <typename Point> struct domain_point_visitor_forward;
    template <typename Point> struct domain_point_visitor_backward;
    template <typename Point> struct strided_domain_point_visitor_forward;
    template <typename Point> struct strided_domain_point_visitor_backward;

    template <typename P> origin_point_visitor_forward<P> make_point_visitor_forward(const P& pmax);
    template <typename P> origin_point_visitor_backward<P> make_point_visitor_backward(const P& pmax);
    template <typename P> domain_point_visitor_forward<P> make_point_visitor_forward(const P& pmin, const P& pmax);
    template <typename P> domain_point_visitor_backward<P> make_point_visitor_backward(const P& pmin, const P& pmax);
    template <typename P> strided_domain_point_visitor_forward<P>  make_strided_point_visitor_forward(const P& pmin, const P& pmax, const P& strides);
    template <typename P> strided_domain_point_visitor_backward<P> make_strided_point_visitor_backward(const P& pmin, const P& pmax, const P& strides);
    /// \}


    /// \defgroup _value_visitor_ Value Visitor
    /// A value visitor is a policy that provides bahviours
    /// about the way to iterate over values.
    /// The point visitor \p VVis provides:
    /// * `VVis::arg` : Type of argument
    /// * `VVis::initialize(VVis::arg)`
    /// * `VVis::init<n>(VVis::arg)`
    /// * `VVis::next<n>(VVis::arg)`
    /// The type of `s.get_value()` must be convertible to `VVis::arg`
    /// \{
    template <size_t dim, std::ptrdiff_t lastOffset> struct strided_offset_visitor;
    /// \}

    struct no_op_visitor;

    template <size_t dim> struct strided_index_visitor;
    /// \}

    /********************/
    /** Implementation **/
    /********************/

    // Dereference Policies

    struct deref_return_structure_policy {
      template <typename S>
      using value_type = const S;

      template <typename S>
      using reference = const S&;

      template <typename S>
      static const S& dereference(const S& s_) {
	return s_;
      }
    };

    struct deref_return_value_policy {
      template <typename S>
      using value_type = typename S::value_type;

      template <typename S>
      using reference = typename S::reference;

      template <typename S>
      static reference<S> dereference(const S& s_) {
	return s_.val();
      }
    };

    struct deref_return_point_policy {
      template <typename S>
      using value_type = typename S::point_type;

      template <typename S>
      using reference = const typename S::point_type &;

      template <typename S>
      static reference<S> dereference(const S& s_) {
	return s_.point();
      }
    };


    //// Point Visitors
    template <typename P>
    struct origin_point_visitor_forward
    {
      typedef P point_type;

      origin_point_visitor_forward() = default;
      origin_point_visitor_forward(const P& pmax) : pmax_ (pmax) {}

      void initialize(P& point) const { point.set_all(0); }
      template <size_t n> void  init(P& point) const { point[n] = 0; }
      template <size_t n> void  next(P& point) const  { ++point[n]; }
      template <size_t n> bool  finished(const P& point) const { return !(point[n] < pmax_[n]); }
      bool NL(const P& point) const { return point[P::ndim-1] == 0; }
      //private:
      P pmax_;
    };

    template <typename P>
    struct origin_point_visitor_backward
    {
      typedef P point_type;

      origin_point_visitor_backward() = default;
      origin_point_visitor_backward(const P& pmax) : pmax_ (pmax) {}

      void  initialize(P& point) const { point = pmax_; point -= 1; }
      template <size_t n> void  init(P& point) const { point[n] = pmax_[n] - 1; }
      template <size_t n> void  next(P& point) const { --point[n]; }
      template <size_t n> bool  finished(const P& point) const { return point[n] < 0; }
      bool NL(const P& point) const { return point[P::ndim-1] == pmax_[P::ndim-1] - 1; }
    private:
      P pmax_;
    };


    template <typename P>
    struct domain_point_visitor_backward
    {
      typedef P point_type;

      domain_point_visitor_backward() = default;
      domain_point_visitor_backward(const P& pmin, const P& pmax) : pmin_ (pmin), pmax_ (pmax) {}

      void  initialize(P& point) const { point = pmax_; point -= 1; }
      template <size_t n> void  init(P& point) const { point[n] = pmax_[n] - 1; }
      template <size_t n> void  next(P& point) const { --point[n]; }
      template <size_t n> bool  finished(const P& point) const { return point[n] < pmin_[n]; }
      bool NL(const P& point) const { return point[P::ndim-1] == pmax_[P::ndim-1] - 1; }
    private:
      P pmin_;
      P pmax_;
    };

    template <typename P>
    struct domain_point_visitor_forward
    {
      typedef P point_type;

      domain_point_visitor_forward() = default;
      domain_point_visitor_forward(const P& pmin, const P& pmax) : pmin_ (pmin), pmax_ (pmax) {}

      void  initialize(P& point) const                  { point = pmin_; }
      template <size_t n> void  init(P& point) const    { point[n] = pmin_[n]; }
      template <size_t n> void  next(P& point) const    { ++point[n]; }
      template <size_t n> bool  finished(const P& point) const { return !(point[n] < pmax_[n]); }
      bool NL(const P& point) const { return point[P::ndim-1] == pmin_[P::ndim-1]; }
    private:
      P pmin_;
      P pmax_;
    };

    template <typename P>
    struct strided_domain_point_visitor_backward
    {
      typedef P point_type;

      strided_domain_point_visitor_backward() = default;
      strided_domain_point_visitor_backward(const P& pmin, const P& pmax, const P& strides) :
	pmin_ (pmin), pmax_(pmax), strides_ (strides)
      {
	mln_precondition( (pmax - pmin).as_vec() % strides.as_vec() == P(literal::zero).as_vec() );
      }

      void  initialize(P& point) const { point = pmax_; point -= strides_; }
      template <size_t n> void  init(P& point) const { point[n] = pmax_[n] - strides_[n]; }
      template <size_t n> void  next(P& point) const { point[n] -= strides_[n]; }
      template <size_t n> bool  finished(const P& point) const { return point[n] < pmin_[n]; }
      bool NL(const P& point) const { return point[P::ndim-1] == pmax_[P::ndim-1] - strides_[P::ndim-1]; }
    private:
      P pmin_;
      P pmax_;
      P strides_;
    };

    template <typename P>
    struct strided_domain_point_visitor_forward
    {
      typedef P point_type;

      strided_domain_point_visitor_forward() = default;
      strided_domain_point_visitor_forward(const P& pmin, const P& pmax, const P& strides) :
	pmin_ (pmin), pmax_ (pmax), strides_ (strides) {}

      void  initialize(P& point) const                  { point = pmin_; }
      template <size_t n> void  init(P& point) const    { point[n] = pmin_[n]; }
      template <size_t n> void  next(P& point) const    { point[n] += strides_[n]; }
      template <size_t n> bool  finished(const P& point) const { return point[n] >= pmax_[n]; }
      bool NL(const P& point) const { return point[P::ndim-1] == pmin_[P::ndim-1]; }
    private:
      P pmin_;
      P pmax_;
      P strides_;
    };




    template <typename P>
    inline
    origin_point_visitor_forward<P>
    make_point_visitor_forward(const P& pmax)
    {
      return origin_point_visitor_forward<P>(pmax);
    }

    template <typename P>
    inline
    origin_point_visitor_backward<P>
    make_point_visitor_backward(const P& pmax)
    {
      return origin_point_visitor_backward<P>(pmax);
    }

    template <typename P>
    domain_point_visitor_forward<P>
    make_point_visitor_forward(const P& pmin, const P& pmax)
    {
      return domain_point_visitor_forward<P>(pmin, pmax);
    }

    template <typename P>
    domain_point_visitor_backward<P>
    make_point_visitor_backward(const P& pmin, const P& pmax)
    {
      return domain_point_visitor_backward<P>(pmin, pmax);
    }

    template <typename P>
    strided_domain_point_visitor_forward<P>
    make_strided_point_visitor_forward(const P& pmin, const P& pmax, const P& strides)
    {
      return strided_domain_point_visitor_forward<P>(pmin, pmax, strides);
    }

    template <typename P>
    strided_domain_point_visitor_backward<P>
    make_strided_point_visitor_backward(const P& pmin, const P& pmax, const P& strides)
    {
      return strided_domain_point_visitor_backward<P>(pmin, pmax, strides);
    }


    template <size_t dim, class T, T last_delta>
    struct strided_visitor
    {
      strided_visitor() = default;

      strided_visitor(T init_value, const std::array<T, dim>& deltas)
	: m_init_value(init_value),
          m_deltas(deltas)
      {
      }

      template <class U>
      void initialize(U& value)
      {
	value = m_init_value;
      }

      template <size_t n, class U>
      std::enable_if_t<(n < dim-1)> next(U& value)
      {
        value += m_deltas[n];
      }

      template <size_t n, class U>
      std::enable_if_t<(n == dim-1)> next(U& value)
      {
        value += last_delta;
      }


    private:
      T                   m_init_value;
      std::array<T, dim>  m_deltas;
    };


    struct no_op_visitor
    {
      template <typename T>
      void initialize(T) const {}

      template <size_t n, typename T> void next (T) const {}
    };



    template <typename PointVisitor, typename OffsetVisitor, typename IndexVisitor,
	      typename InternalStruct, typename DereferencePolicy>
    struct nested_loop_iterator :
      iterator_base< nested_loop_iterator<PointVisitor, OffsetVisitor, IndexVisitor, InternalStruct, DereferencePolicy>,
                     typename DereferencePolicy::template value_type<InternalStruct>,
                     typename DereferencePolicy::template reference<InternalStruct> >,
      public internal::iterator_core_access
    {
      typedef typename DereferencePolicy::template reference<InternalStruct> reference;
      typedef std::true_type has_NL;

      nested_loop_iterator() = default;

      nested_loop_iterator(const InternalStruct s, const PointVisitor& pv, const OffsetVisitor& vv, const IndexVisitor& iv)
        : m_s (s), m_pv (pv), m_vv (vv), m_iv (iv)
      {
      }

      nested_loop_iterator(const nested_loop_iterator&) = default;

      template <typename PointVisitor2, typename OffsetVisitor2, typename IndexVisitor2, typename InternalStruct2>
      nested_loop_iterator(const nested_loop_iterator<PointVisitor2, OffsetVisitor2, IndexVisitor2, InternalStruct2, DereferencePolicy>& other,
			   typename std::enable_if< std::is_convertible<PointVisitor, PointVisitor2>::value and
			   std::is_convertible<OffsetVisitor2, OffsetVisitor>::value and
			   std::is_convertible<InternalStruct2, InternalStruct>::value>::type* = NULL)
	: m_s (other.m_s), m_pv (other.m_pv), m_vv (other.m_vv), m_iv (other.m_iv)
      {
      }


      void init()
      {
        m_pv.initialize(get_point(m_s));
        m_vv.initialize(get_offset(m_s));
        m_iv.initialize(get_index(m_s));
      }

      void next()
      {
        this->next_<ndim-1>();
      }

      bool finished() const
      {
        return m_pv.template finished<0>(get_point(m_s));
      }

      bool NL() const
      {
        return m_pv.NL(get_point(m_s));
      }

      reference
      dereference() const
      {
        return DereferencePolicy::dereference(m_s);
      }

      void __inner_init()
      {
        m_pv.template init<ndim-1>(get_point(m_s));
      }

      void __inner_next()
      {
        m_pv.template next<ndim-1>(get_point(m_s));
        m_vv.template next<ndim-1>(get_offset(m_s));
        m_iv.template next<ndim-1>(get_index(m_s));
      }

      bool __inner_finished() const
      {
        return m_pv.template finished<ndim-1>(get_point(m_s));
      }

      void __outer_init()
      {
        m_pv.initialize(get_point(m_s));
        m_vv.initialize(get_offset(m_s));
        m_iv.initialize(get_index(m_s));
      }

    void __outer_next()
    {
      next_<ndim-2>();
    }

    bool __outer_finished() const
    {
      return m_pv.template finished<0>(get_point(m_s));
    }


  public:
      template <typename, typename, typename, typename, typename>
      friend struct nested_loop_iterator;

      enum { ndim = PointVisitor::point_type::ndim };


      template <int n>
      std::enable_if_t<(n >= 0)>
      next_()
      {
        mln_precondition(not this->finished());
        m_pv.template next<n>(get_point(m_s));
        m_vv.template next<n>(get_offset(m_s));
        m_iv.template next<n>(get_index(m_s));

        if (n != 0 and m_pv.template finished<n>(get_point(m_s)))
          {
            this->next_<n-1>();
            m_pv.template init<n>(get_point(m_s));
          }
      }

      template <int n>
      std::enable_if_t<(n < 0)>
      next_()
      {
      }

    public:
      InternalStruct m_s;
      PointVisitor   m_pv;
      OffsetVisitor  m_vv;
      IndexVisitor   m_iv;
    };

  }

}

#endif // ! MLN_CORE_IMAGE_INTERNAL_NESTED_LOOP_ITERATOR_HPP
