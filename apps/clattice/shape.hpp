#ifndef SHAPE_HPP
# define SHAPE_HPP

# include <type_traits>
# include <functional>
# include <vector>
# include <unordered_set>
# include <ostream>
# include <mln/core/domain/box.hpp>

/**
*
* \tparam LowerCompare Model of relation
* \tparam UpperCompare Model of relation
*
* A relation R must verify:
*
* \f[
*  a R b ^ b R c ⇒ a R c
*  a R b ^ a R c ⇒ a R k  V  a = k with k=relation_traits<R>::comm(b,c)
*
*
* \f]
*/
template <typename V, class LowerCompare, class UpperCompare>
struct shape
{
  enum { NONE = 0, LOWER = 1, UPPER = 2, LEQ = 4, GEQ = 8 } ;

public:
  shape(V level, LowerCompare cmp);

  template <class dummy = void>
  shape(V level, UpperCompare cmp,
	typename std::enable_if
	< not std::is_same<LowerCompare, UpperCompare>::value, dummy >::type* = NULL);

  shape(const shape& ) = delete;
  shape(shape&&) = default;
  shape& operator= (const shape& ) = delete;
  shape& operator= (shape&&) = default;

  bool operator== (const shape&) const;

  bool		islower() const;
  bool		isupper() const;
  bool		isleq() const;
  bool		isgeq() const;
  V		lower_level() const;
  V		upper_level() const;
  unsigned	size() const;
  const mln::box2d&	bbox() const;

  // Update a shape with another shape which is equal.
  void		update_with(const shape& other) const;

private:
  mutable int				m_type;
  mutable V				m_level_1;
  mutable V				m_level_2;
  mutable LowerCompare			m_cmp_1;
  mutable UpperCompare			m_cmp_2;

public:
  std::vector<mln::point2d>	m_set;
  mln::box2d			m_bbox;
};

template <typename shape_t>
using shape_set = std::unordered_set<shape_t>;

template <typename V, class LowerCompare, class UpperCompare>
bool
is_shape_included(const shape<V, LowerCompare, UpperCompare>& a,
		  const shape<V, LowerCompare, UpperCompare>& b);

template <typename V, class LowerCompare, class UpperCompare>
bool
is_shape_equal(const shape<V, LowerCompare, UpperCompare>& a,
	       const shape<V, LowerCompare, UpperCompare>& b);

template <typename V, class LowerCompare, class UpperCompare>
void
prettyprint_shape(const shape<V, LowerCompare, UpperCompare>& shp,
		  const mln::box2d& domain);

namespace std {
  /// \brief specialization of hash function for the unordered set of
  template <typename V, class LowerCompare, class UpperCompare>
  struct hash< shape<V, LowerCompare, UpperCompare> >;
}


/*************************************/
/***       Implementation          ***/
/*************************************/

namespace impl
{
  inline
  bool
  is_box_inc(const mln::box2d& a, const mln::box2d& b)
  {
    return
      vecprod_isgreaterequal(a.pmin, b.pmin) and
      vecprod_islessequal(a.pmax, b.pmax);
  }

}

template <typename V, class LowerCompare, class UpperCompare>
bool
is_shape_included(const shape<V, LowerCompare, UpperCompare>& a,
		  const shape<V, LowerCompare, UpperCompare>& b)
{
  // Heuristique 1.
  // A c B => |A| < |B| et bbox(A) c bbox(B)
  if (not (a.size() <= b.size() and impl::is_box_inc(a.bbox(), b.bbox())))
    return false;

  // Heuristique 2.
  // La < Lb => A ^ B = 0 or A c B
  if (a.islower() and b.islower())
    {
      LowerCompare cmp;
      if (a.lower_level() == b.lower_level() or
	  cmp(a.lower_level(), b.lower_level()))
	return std::binary_search(b.m_set.begin(), b.m_set.end(), a.m_set[0]);
    }
  if (a.isupper() and b.isupper() and
      !std::is_same<LowerCompare, UpperCompare>::value)
    {
      UpperCompare cmp;
      if (a.upper_level() == b.upper_level() or
	  cmp(a.upper_level(), b.upper_level()))
	return std::binary_search(b.m_set.begin(), b.m_set.end(), a.m_set[0]);
    }

  // Heuristique 3.
  return std::includes(b.m_set.begin(), b.m_set.end(),
		       a.m_set.begin(), a.m_set.end());
}


template <typename V, class LowerCompare, class UpperCompare>
bool
is_shape_equal(const shape<V, LowerCompare, UpperCompare>& a,
	       const shape<V, LowerCompare, UpperCompare>& b)
{
  // Heuristique 1.
  // A = B => |A| = |B| et bbox(A) = bbox(B)
  if (not (a.size() == b.size() and a.bbox() == b.bbox()))
    return false;

  // Heuristique 2.
  // La < Lb or Lb < a => A ^ B = 0 or A c B or B c A
  // Heuristique 2.
  // La < Lb => A ^ B = 0 or A c B
  if (a.islower() and b.islower())
    {
      LowerCompare cmp;
      if (a.lower_level() == b.lower_level() or
	  cmp(a.lower_level(), b.lower_level()))
	return b.m_set[0] == a.m_set[0];
    }
  if (a.isupper() and b.isupper() and
      !std::is_same<LowerCompare, UpperCompare>::value)
    {
      UpperCompare cmp;
      if (a.upper_level() == b.upper_level() or
	  cmp(a.upper_level(), b.upper_level()))
	return b.m_set[0] == a.m_set[0];
    }

  // Heuristique 3.
  return std::equal(a.m_set.begin(), a.m_set.end(), b.m_set.begin());
}


template <typename V, class LowerCompare, class UpperCompare>
shape<V, LowerCompare, UpperCompare>::shape(V x, LowerCompare)
  : m_type(LOWER),
    m_level_1(x)
{
}

template <typename V, class LowerCompare, class UpperCompare>
template <class dummy>
shape<V, LowerCompare, UpperCompare>::shape(V x, UpperCompare,
  typename std::enable_if<!std::is_same<LowerCompare, UpperCompare>::value, dummy >::type*)
  : m_type(UPPER),
    m_level_2(x)
{
}


template <typename V, class LowerCompare, class UpperCompare>
bool
shape<V, LowerCompare, UpperCompare>::operator== (const shape& other) const
{
  return is_shape_equal(*this, other);
}

template <typename V, class LowerCompare, class UpperCompare>
bool
shape<V, LowerCompare, UpperCompare>::isupper() const
{
  return (m_type & UPPER) != 0;
}

template <typename V, class LowerCompare, class UpperCompare>
bool
shape<V, LowerCompare, UpperCompare>::islower() const
{
  return (m_type & LOWER) != 0;
}

template <typename V, class LowerCompare, class UpperCompare>
bool
shape<V, LowerCompare, UpperCompare>::isleq() const
{
  return (m_type & LEQ) != 0;
}

template <typename V, class LowerCompare, class UpperCompare>
bool
shape<V, LowerCompare, UpperCompare>::isgeq() const
{
  return (m_type & GEQ) != 0;
}


template <typename V, class LowerCompare, class UpperCompare>
V
shape<V, LowerCompare, UpperCompare>::upper_level() const
{
  return m_level_2;
}

template <typename V, class LowerCompare, class UpperCompare>
V
shape<V, LowerCompare, UpperCompare>::lower_level() const
{
  return m_level_1;
}

template <typename V, class LowerCompare, class UpperCompare>
unsigned
shape<V, LowerCompare, UpperCompare>::size() const
{
  return m_set.size();
}

template <typename V, class LowerCompare, class UpperCompare>
const mln::box2d&
shape<V, LowerCompare, UpperCompare>::bbox() const
{
  return m_bbox;
}




template <typename V, class LowerCompare, class UpperCompare>
void
shape<V, LowerCompare, UpperCompare>::update_with(const shape& other) const
{
  // Same shape type [u <(=) a] ^ [u <(=) b] => [u <= inf(a,b)]
  if (other.islower()) {
    if (!this->islower())
      m_level_1 = other.m_level_1;
    else {
      m_level_1 = m_cmp_1.inf(other.m_level_1, this->m_level_1);
      m_type |= LEQ;
    }
  }

  // Same shape type [u > a] ^ [u > b] => [u >= sup(a,b)]
  if (other.isupper()) {
    if (!this->isupper())
      m_level_2 = other.m_level_2;
    else {
      // std::cout << "updated: ";
      // format(std::cout, m_level_2);
      // format(std::cout, other.m_level_2);

      m_level_2 = m_cmp_2.inf(other.m_level_2, this->m_level_2);
      m_type |= GEQ;

      //std::cout << " --> ";
      //format(std::cout, m_level_2) << std::endl;
    }
  }
  m_type |= other.m_type;
}

template <typename V, class LowerCompare, class UpperCompare>
std::ostream&
operator<< (std::ostream& os, const shape<V, LowerCompare, UpperCompare>& shp)
{
  os << "==Info== Size:" << shp.size();
  if (shp.islower()) {
    os << "\tfrom: [u" << LowerCompare::str
       << (shp.isleq() ? '=' : ' ');
    format(os, shp.lower_level()) << "]";
  }
  if (shp.isupper()) {
    os << "\tfrom: [u" << UpperCompare::str
       << (shp.isgeq() ? '=' : ' ');
    format(os, shp.upper_level()) << "]";
  }
  return os;
}



template <typename V, class LowerCompare, class UpperCompare>
void
prettyprint_shape(const shape<V, LowerCompare, UpperCompare>& shp,
		  const mln::box2d& domain)
{
  using namespace mln;
  using mln::io::format;

  image2d<bool> ima;
  ima.resize(domain, 0, false);

  for (auto x: shp.m_set)
    ima(x) = true;

  // if (not std::is_same<Ktag, K0_tag>::value)
  //   for (auto x: shp.nonprimaries)
  // 	ima(x) = true;

  std::cout << "==Info== Size:" << shp.size() <<
    "(0x" << std::hex << shp.size() << ")" << std::dec << std::endl;

  if (shp.islower()) {
    std::cout << "\tfrom: [u" << LowerCompare::str
	      << (shp.isleq() ? '=' : ' ');
    format(std::cout, shp.lower_level()) << "]" << std::endl;
  }

  if (shp.isupper()) {
    std::cout << "\tfrom: [u" << UpperCompare::str
	      << (shp.isgeq() ? '=' : ' ');
    format(std::cout, shp.upper_level()) << "]" << std::endl;
  }

  std::cout << "hash: " << std::hex
	    << "0x" << std::hash< shape<V, LowerCompare, UpperCompare> >() (shp)
	    << std::dec << std::endl;

  io::imprint(ima);
}


namespace std
{


  template <typename V, class LowerCompare, class UpperCompare>
  struct hash< shape<V, LowerCompare, UpperCompare> >
  {
    typedef shape<V, LowerCompare, UpperCompare> shape_t;

    size_t
    operator() (const shape_t& x) const
    {
      size_t h = ((x.size() & 0x00FF) << 24) |
	((x.bbox().pmin[0] & 0x00FF) << 16) |
	((x.bbox().pmin[1] & 0x00FF) << 8) |
	((x.bbox().pmax[0] + x.bbox().pmax[1]) & 0x00FF);
      return h;
    }
  };

}

#endif // ! SHAPE_HPP
