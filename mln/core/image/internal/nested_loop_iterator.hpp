#ifndef MLN_CORE_IMAGE_INTERNAL_NESTED_LOOP_ITERATOR_HPP
# define MLN_CORE_IMAGE_INTERNAL_NESTED_LOOP_ITERATOR_HPP

# include <type_traits>
# include <utility>

# include <boost/iterator/iterator_facade.hpp>
# include <boost/any.hpp>

namespace mln
{

  namespace internal
  {


    struct iterator_core_access
    {
      template <typename S, typename R = decltype( std::declval<S>().get_value() ) >
      struct get_value_helper
      {
        typedef R type;
        static R get_value(S& v) { return v.get_value(); }
      };

      template <typename S>
      struct get_value_helper<S, void>
      {
        typedef boost::any type;
        static boost::any get_value(S&) { return boost::any (); }
      };

    protected:
      template <typename S>
      auto get_point(S& v) -> decltype(v.get_point()) { return v.get_point(); }

      template <typename S>
      typename get_value_helper<S>::type
      get_value(S& v) {
        return get_value_helper<S>::get_value(v);
      }

      template <typename S, typename T>
      bool equal(const S& v, const T& other) const { return v.equal(other); }
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
    template <typename PointVisitor, typename ValueVisitor,
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
    template <typename Point> struct origin_point_visitor;
    template <typename Point> struct domain_point_visitor;
    template <typename Point> struct strided_domain_point_visitor;

    template <typename P> origin_point_visitor<P> make_point_visitor(const P& pmax);
    template <typename P> domain_point_visitor<P> make_point_visitor(const P& pmin, const P& pmax);
    template <typename P> strided_domain_point_visitor<P> make_point_visitor(const P& pmin, const P& pmax, const P& strides);
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
    template <typename V, size_t dim, bool forward> struct strided_pointer_value_visitor;
    struct no_op_value_visitor;

    /// \}


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

      origin_point_visitor_forward() : pmax_ () {}
      origin_point_visitor_forward(const P& pmax) : pmax_ (pmax) {}

      void initialize(P& point) const { point = (P){0,}; }
      template <size_t n> void  init(P& point) const { point[n] = 0; }
      template <size_t n> void  next(P& point) const  { ++point[n]; }
      template <size_t n> bool  finished(const P& point) const { return point[n] >= pmax_[n]; }
    private:
      P pmax_;
    };

    template <typename P>
    struct origin_point_visitor_backward
    {
      typedef P point_type;

      origin_point_visitor_backward() : pmax_ () {}
      origin_point_visitor_backward(const P& pmax) : pmax_ (pmax) {}

      void  initialize(P& point) const { point = (P){0,}; point[0] = pmax_[0]; }
      template <size_t n> void  init(P& point) const { point[n] = pmax_[n] - 1; }
      template <size_t n> void  next(P& point) const { --point[n]; }
      template <size_t n> bool  finished(P& point) const { return point[n] < 0; }

    private:
      P pmax_;
    };


    template <typename P>
    struct domain_point_visitor_backward
    {
      typedef P point_type;

      domain_point_visitor_backward(): pmin_ (), pmax_ () {}
      domain_point_visitor_backward(const P& pmin, const P& pmax) : pmin_ (pmin), pmax_ (pmax) {}

      void  initialize(P& point) const { point = pmin_; point[0] = pmax_[0]; }
      template <size_t n> void  init(P& point) const { point[n] = pmax_[n] - 1; }
      template <size_t n> void  next(P& point) const { --point[n]; }
      template <size_t n> bool  finished(P& point) const { return point[n] < pmin_[n]; }

    private:
      P pmin_;
      P pmax_;
    };

    template <typename P>
    struct domain_point_visitor_forward
    {
      typedef P point_type;

      domain_point_visitor_forward(): pmin_ (), pmax_ () {}
      domain_point_visitor_forward(const P& pmin, const P& pmax) : pmin_ (pmin), pmax_ (pmax) {}

      void  initialize(P& point) const                  { point = pmin_; }
      template <size_t n> void  init(P& point) const    { point[n] = pmin_[n]; }
      template <size_t n> void  next(P& point) const    { ++point[n]; }
      template <size_t n> bool  finished(const P& point) const { return point[n] >= pmax_[n]; }
    private:
      P pmin_;
      P pmax_;
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


    // Value visitor
    template <typename V, size_t dim, bool forward>
    struct strided_pointer_value_visitor
    {
      typedef char* ptr_t;
      enum { ndim = dim };

      strided_pointer_value_visitor()
      {
      }

      strided_pointer_value_visitor(ptr_t start, const size_t* strides)
        : start_ (start)
       {
	 std::copy(strides, strides + ndim, strides_.begin());
       }

      // Conversion non-const -> const
      template <typename U>
      strided_pointer_value_visitor(const strided_pointer_value_visitor<U, dim, forward>& other,
                                    typename std::enable_if< std::is_convertible<U*, V*>::value >* dummy = NULL) :
        start_ (other.start_),
        strides_ (other.strides_),
        stack_ (other.stack_)
       {
       }

      void
      initialize(ptr_t& ptr)
      {
        ptr = start_;
        stack_.fill(start_);
      }


      template <size_t n>
      typename std::enable_if<(n < ndim-1)>::type
      init (ptr_t& )
      {
        static_assert(n > 0, "");
        stack_[n] = stack_[n-1];
      }

      template <size_t n>
      typename std::enable_if<(n == ndim-1)>::type
      init(ptr_t& ptr)
      {
        static_assert(n > 0, "");
        ptr = stack_[n] = stack_[n-1];
      }

    private:
      char* start_;
      std::array<size_t, ndim> strides_;
      std::array<char*, ndim> stack_;
    };

    template <typename V, size_t dim>
    template <size_t n>
    typename std::enable_if<(n < dim-1)>::type
    strided_pointer_value_visitor<V, dim, false>::next (ptr_t&)
    {
      stack_[n] -= strides_[n];
    }

    template <typename V, size_t dim>
    template <size_t n>
    typename std::enable_if<(n < dim-1)>::type
    strided_pointer_value_visitor<V, dim, true>::next (ptr_t&)
    {
      stack_[n] += strides_[n];
    }

    template <typename V, size_t dim>
    template <size_t n>
    typename std::enable_if<(n == dim-1)>::type
    strided_pointer_value_visitor<V, dim, false>::next (ptr_t& ptr)
    {
      ptr = (stack_[n] -= strides_[n]);
    }

    template <typename V, size_t dim>
    template <size_t n>
    typename std::enable_if<(n == dim-1)>::type
    strided_pointer_value_visitor<V, dim, true>::next (ptr_t& ptr)
    {
      ptr = (stack_[n] += strides_[n]);
    }



    struct no_op_value_visitor
    {
      void initialize(const boost::any& ) {}
      template <size_t n> void init (const boost::any& ) {}
      template <size_t n> void next (const boost::any& ) {}
    };



    template <typename PointVisitor, typename ValueVisitor,
	      typename InternalStruct, typename DereferencePolicy>
    struct nested_loop_iterator :
      iterator_base< nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>,
                     typename DereferencePolicy::template value_type<InternalStruct>,
                     typename DereferencePolicy::template reference<InternalStruct> >,
      public internal::iterator_core_access
    {
      typedef typename DereferencePolicy::template reference<InternalStruct> reference;

      nested_loop_iterator()
      {
      }


      nested_loop_iterator(const InternalStruct s, const PointVisitor& pv, const ValueVisitor& vv)
        : s_ (s), p_ (pv), v_ (vv)
      {
      }


      void init()
      {
        p_.initialize();
        v_.initialize()
      }

      void next()
      {
        this->next_<ndim-1>();
      }

      void finished() const
      {
        return p_.finished();
      }

      reference
      dereference() const
      {
        return DereferencePolicy::dereference(s_);
      }


  private:
      template <typename, typename, typename, typename>
      friend struct nested_loop_iterator_forward;

      enum { ndim = PointVisitor::point_type::ndim };


      template <size_t n>
      typename std::enable_if<(n > 0), void>::type
      next_()
      {
        mln_precondition(not this->finished());
        p_.template next<n>(iterator_core_access::get_point(s_));
        if (not p_.template finished<n>(iterator_core_access::get_point(s_)))
          {
            v_.template next<n>(get_value(s_));
            return;
          }
        this->next<n-1>();
        p_.template init<n>(get_point(s_));
        v_.template init<n>(get_value(s_));
      }

      template <size_t n>
      typename std::enable_if<n == 0, void>::type
      next_()
      {
        mln_precondition(not this->finished());
        p_.template next<0>(get_point(s_));
        v_.template next<0>(get_value(s_));
      }

    private:
      InternalStruct s_;
      PointVisitor p_;
      ValueVisitor v_;
    };

  }

}

#endif // ! MLN_CORE_IMAGE_INTERNAL_NESTED_LOOP_ITERATOR_HPP
