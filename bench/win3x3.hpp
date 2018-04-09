#pragma once
#include <mln/core/image/image2d.hpp>
#include <range/v3/view_facade.hpp>







template <class PixelCursor, class NbhTraits>
class nbh_facade : public ranges::view_facade<nbh_facade<PixelCursor, NbhTraits>, ranges::finite>
{
  static constexpr std::size_t N = NbhTraits::dpoints.size();
  friend ranges::range_access;
  class cursor
  {
    friend ranges::range_access;
  public:
    cursor() = default;
    cursor(const PixelCursor& cursor)
      : m_cursor(cursor)
      {}

  private:
    auto read() const
    {
      auto cur =  m_cursor;
      cur.advance(NbhTraits::dpoints[m_i][1], NbhTraits::dpoints[m_i][0]);
      return cur;
    }
    bool equal(const ranges::default_sentinel& ) const { return m_i == NbhTraits::dpoints.size(); }
    void next() { m_i++; }

    //const std::array<std::ptrdiff_t, N>*   m_offsets;
    PixelCursor                            m_cursor;
    int m_i = 0;
  };


  cursor begin_cursor() const
  {
    return cursor(m_pix);
  }

public:
  nbh_facade() = default;

  nbh_facade(const PixelCursor& pix)
    : m_pix(pix)
  {
    // auto& ima = pix.image();
    // for (std::size_t i = 0; i < N; ++i)
    //   m_offsets[i] = ima.delta_index(NbhTraits::dpoints[i]);
  }

  nbh_facade& operator() (const PixelCursor& pix)
  {
    m_pix = pix;
    return *this;
  }


private:
  //std::array<std::ptrdiff_t, N>        m_offsets;
  PixelCursor                          m_pix;
};


struct win3x3_traits
{
  static constexpr std::array<mln::point2d, 8> dpoints = {{
    {-1,-1}, {-1,0}, {-1, 1},
    {0,-1},          {0, 1},
    {1,-1},  {1,0},  {1, 1}}};
};
const std::array<mln::point2d, 8> win3x3_traits::dpoints;

template <class Pix>
class win3x3 : public nbh_facade<Pix, win3x3_traits>
{
public:
  win3x3(const Pix& pix) : nbh_facade<Pix, win3x3_traits>(pix) {};
};





struct c4_traits
{
  static constexpr std::array<mln::point2d, 4> dpoints = {{
    {-1,0}, {0,-1}, {0, 1}, {1,0}
    }};
};
const std::array<mln::point2d, 4> c4_traits::dpoints;

template <class Pix>
class c4 : public nbh_facade<Pix, c4_traits>
{
public:
  c4(const Pix& pix) : nbh_facade<Pix, c4_traits>(pix) {};
};
