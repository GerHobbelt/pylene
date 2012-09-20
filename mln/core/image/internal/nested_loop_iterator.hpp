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
    /// for (p.fwd_init<0>(), v.fwd_init<0>(); p.fwd_finished(); p.fwd_inc<0>(), v.fwd_inc<0>())
    ///   for (p.fwd_init<1>(), v.fwd_init<1>(); p.fwd_finished(); p.fwd_inc<1>(), v.fwd_inc<1>())
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
    /// * `PVis::fwd_init<n>(P&)`
    /// * `PVis::bwd_init<n>(P&)`
    /// * `PVis::fwd_inc<n>(P&)`
    /// * `PVis::bwd_inc<n>(P&)`
    /// * `PVis::fwd_finished<n>(P&)`
    /// * `PVis::bwd_finished<n>(P&)`
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
    /// * `VVis::fwd_init<n>(VVis::arg)`
    /// * `VVis::fwd_inc<n>(VVis::arg)`
    /// * `VVis::bwd_init<n>(VVis::arg)`
    /// * `VVis::fwd_inc<n>(VVis::arg)`
    /// The type of `s.get_value()` must be convertible to `VVis::arg`
    /// \{
    template <typename V, typename Point> struct strided_pointer_value_visitor;
    struct no_op_value_visitor;

    template <typename V, typename Point>
    strided_pointer_value_visitor<V, Point> make_strided_pointer_value_visitor(V* start, const Point& shape, const size_t* strides);
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


    // Point Visitors

    template <typename P>
    struct origin_point_visitor
    {
      typedef P point_type;

      origin_point_visitor() : pmax_ () {}
      origin_point_visitor(const P& pmax) : pmax_ (pmax) {}

      template <size_t n> void  fwd_initialize(P& point) const { point = (P){0,}; }
      template <size_t n> void  fwd_init(P& point) const { point[n] = 0; }
      template <size_t n> void  fwd_inc(P& point) const  { ++point[n]; }
      template <size_t n> bool  fwd_finished(const P& point) const { return point[n] >= pmax_[n]; }

      template <size_t n> void  bwd_initialize(P& point) const { point = (P){0,}; point[0] = pmax_[0]; }
      template <size_t n> void  bwd_init(P& point) const { point[n] = pmax_[n] - 1; }
      template <size_t n> void  bwd_inc(P& point) const { --point[n]; }
      template <size_t n> bool  bwd_inc(P& point) const { return point[n] < 0; }

    private:
      P pmax_;
    };

    template <typename P>
    struct domain_point_visitor
    {
      typedef P point_type;

      domain_point_visitor():
	pmin_ (), pmax_ () {}

      domain_point_visitor(const P& pmin, const P& pmax) :
	pmin_ (pmin), pmax_ (pmax) {}

      template <size_t n> void  fwd_initialize(P& point) const { point = pmin_; }
      template <size_t n> void  fwd_init(P& point) const { point[n] = pmin_[n]; }
      template <size_t n> void  fwd_inc(P& point) const  { ++point[n]; }
      template <size_t n> bool  fwd_finished(const P& point) const { return point[n] >= pmax_[n]; }

      template <size_t n> void  bwd_initialize(P& point) const { point = pmin_; point[0] = pmax_[0]; }
      template <size_t n> void  bwd_init(P& point) const { point[n] = pmax_[n] - 1; }
      template <size_t n> void  bwd_inc(P& point) const { --point[n]; }
      template <size_t n> bool  bwd_finished(P& point) const { return point[n] < pmin_[n]; }
    private:
      P pmin_;
      P pmax_;
    };


    template <typename P>
    inline
    origin_point_visitor<P>
    make_point_visitor(const P& pmax)
    {
      return origin_point_visitor<P>(pmax);
    }

    template <typename P>
    domain_point_visitor<P>
    make_point_visitor(const P& pmin, const P& pmax)
    {
      return domain_point_visitor<P>(pmin, pmax);
    }



    // Value visitor


    template <typename V, typename Point>
     struct strided_pointer_value_visitor
     {
       typedef char* ptr_t;
       enum { ndim = Point::ndim };


       strided_pointer_value_visitor()
       {
       }

       strided_pointer_value_visitor(ptr_t start, Point shape, const size_t* strides)
         : shp_ (shape)
       {
	 stack_.fill(start);
	 std::copy(strides, strides + ndim, strides_.begin());
       }


       // Conversion non-const -> const
       template <typename U>
       strided_pointer_value_visitor(const strided_pointer_value_visitor<U, Point>& other,
                                     typename std::enable_if< std::is_convertible<U*, V*>::value, void>* dummy = NULL) :
         shp_ (other.shp_),
         strides_ (other.strides_),
         stack_ (other.stack_)
       {
       }

       template <size_t n>
       typename std::enable_if< (n > 0 and n < ndim-1),void>::type
         fwd_init (ptr_t& )
       {
	 stack_[n] = stack_[n-1];
       }

       template <size_t n>
       typename std::enable_if< (n > 0 and n < ndim-1),void>::type
       bwd_init (ptr_t& )
       {
	 stack_[n] = stack_[n-1] + (strides_[n] * (shp_[n] - 1));
       }

       template <size_t n>
       typename std::enable_if<(n > 0 and n == ndim-1), void>::type
       fwd_init(ptr_t& ptr)
       {
         ptr = stack_[n] = stack_[n-1];
       }

       template <size_t n>
       typename std::enable_if<(n > 0 and n == ndim-1), void>::type
       bwd_init(ptr_t& ptr)
       {
         ptr = stack_[ndim-1] = (stack_[ndim-2] + strides_[ndim-1] * (shp_[ndim-1] - 1));
       }

       template <size_t n>
       typename std::enable_if<(n < ndim-1), void>::type
       fwd_inc (ptr_t&)
       {
         stack_[n] += strides_[n];
       }

       template <size_t n>
       typename std::enable_if<(n == ndim-1), void>::type
       fwd_inc (ptr_t& ptr)
       {
         ptr = (stack_[n] += strides_[n]);
       }

       template <size_t n>
       typename std::enable_if<(n < ndim-1), void>::type
       bwd_inc (ptr_t&)
       {
         stack_[n] -= strides_[n];
       }

       template <size_t n>
       typename std::enable_if<(n == ndim-1), void>::type
       bwd_inc (ptr_t& ptr)
       {
         ptr = (stack_[n] -= strides_[n]);
       }


     private:
       Point shp_;
       std::array<size_t, ndim> strides_;
       std::array<char*, ndim> stack_;
    };

    template <typename V, typename Point>
    inline
    strided_pointer_value_visitor<V, Point>
    make_strided_pointer_value_visitor(V* start, const Point& shape, const size_t* strides)
    {
      return strided_pointer_value_visitor<V, Point>(start, shape, strides);
    }

    struct no_op_value_visitor
    {
      template <size_t n> void fwd_init (const boost::any& ) {}
      template <size_t n> void bwd_init (const boost::any& ) {}
      template <size_t n> void fwd_inc (const boost::any& ) {}
      template <size_t n> void bwd_inc (const boost::any& ) {}
    };


    template <typename PointVisitor, typename ValueVisitor,
	      typename InternalStruct, typename DereferencePolicy>
    struct nested_loop_iterator :
      public boost::iterator_facade< nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>,
                                     typename DereferencePolicy::template value_type<InternalStruct>,
				     boost::bidirectional_traversal_tag,
				     typename DereferencePolicy::template reference<InternalStruct> >,
      public internal::iterator_core_access
    {
      nested_loop_iterator()
      {
      }

      nested_loop_iterator(const InternalStruct s, const PointVisitor& pv, const ValueVisitor& vv)
        : s_ (s), p_ (pv), v_ (vv)
      {
      }

      template <typename ValueVisitor2, typename InternalStruct2>
      nested_loop_iterator(const nested_loop_iterator<PointVisitor, ValueVisitor2, InternalStruct2, DereferencePolicy>& other,
                           std::enable_if< std::is_convertible<InternalStruct2, InternalStruct>::value, void>* = NULL)
        : s_ (other.s_), p_ (other.p_), v_ (other.v_)
      {
      }

    private:
      friend class boost::iterator_core_access;

      template <typename, typename, typename, typename>
      friend struct nested_loop_iterator;

      enum { ndim = PointVisitor::point_type::ndim };


      template <size_t n> typename std::enable_if<(n > 0), void>::type inc();
      template <size_t n> typename std::enable_if<(n > 0), void>::type dec();
      template <size_t n> typename std::enable_if<n == 0, void>::type inc();
      template <size_t n> typename std::enable_if<n == 0, void>::type dec();




      void increment();
      void decrement();

      typename DereferencePolicy::template reference<InternalStruct> dereference() const;

      template <typename ValueVisitor2, typename InternalStruct2>
      typename std::enable_if< std::is_convertible<InternalStruct, InternalStruct2>::value, bool>::type
      equal(const nested_loop_iterator<PointVisitor, ValueVisitor2, InternalStruct2, DereferencePolicy>& other) const
      {

        return iterator_core_access::equal(s_, other.s_);
      }


    private:
      InternalStruct s_;
      PointVisitor p_;
      ValueVisitor v_;
    };




    template < typename PointVisitor, typename ValueVisitor, typename InternalStruct, typename DereferencePolicy>
    template <size_t n>
    inline
    typename std::enable_if< (n > 0), void >::type
    nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>::inc()
    {
      p_.template fwd_inc<n>(iterator_core_access::get_point(s_));
      if (not p_.template fwd_finished<n>(iterator_core_access::get_point(s_)))
        {
          v_.template fwd_inc<n>(get_value(s_));
          return;
        }
      this->inc<n-1>();
      p_.template fwd_init<n>(get_point(s_));
      v_.template fwd_init<n>(get_value(s_));
    }

    template <typename PointVisitor, typename ValueVisitor, typename InternalStruct, typename DereferencePolicy>
    template <size_t n>
    inline
    typename std::enable_if< (n == 0), void >::type
    nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>::inc()
    {
      p_.template fwd_inc<0>(get_point(s_));
      v_.template fwd_inc<0>(get_value(s_));
    }


    template < typename PointVisitor, typename ValueVisitor, typename InternalStruct, typename DereferencePolicy>
    template <size_t n>
    inline
    typename std::enable_if< (n > 0), void >::type
    nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>::dec()
    {
      p_.template bwd_inc<n>(get_point(s_));
      if (not p_.template bwd_finished<n>(get_point(s_)))
        {
          v_.template bwd_inc<n>(get_value(s_));
          return;
        }
      this->dec<n-1>();
      p_.template bwd_init<n>(get_point(s_));
      v_.template bwd_init<n>(get_value(s_));
    }

    template <typename PointVisitor, typename ValueVisitor, typename InternalStruct, typename DereferencePolicy>
    template <size_t n>
    inline
    typename std::enable_if< (n == 0), void >::type
    nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>::dec()
    {
      p_.template bwd_inc<0>(get_point(s_));
      v_.template bwd_inc<0>(get_value(s_));
    }

    template < typename PointVisitor, typename ValueVisitor, typename InternalStruct, typename DereferencePolicy>
    inline
    void nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>::increment()
    {
      this->inc<ndim-1>();
    }

    template < typename PointVisitor, typename ValueVisitor, typename InternalStruct, typename DereferencePolicy>
    inline
    void nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>::decrement()
    {
      this->dec<ndim-1>();
    }

    template < typename PointVisitor, typename ValueVisitor, typename InternalStruct, typename DereferencePolicy>
    inline
    typename DereferencePolicy::template reference<InternalStruct>
    nested_loop_iterator<PointVisitor, ValueVisitor, InternalStruct, DereferencePolicy>::dereference() const
    {
      return DereferencePolicy::dereference(s_);
    }

  }

}

#endif // ! MLN_CORE_IMAGE_INTERNAL_NESTED_LOOP_ITERATOR_HPP
