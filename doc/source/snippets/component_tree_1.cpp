
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/accu/concept/accumulator.hpp>
#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>
#include <mln/morpho/experimental/tos.hpp>
#include <mln/morpho/experimental/maxtree.hpp>


struct my_accu_t : mln::Accumulator<my_accu_t>
{
  using result_type   = my_accu_t;
  using argument_type = mln::image_pixel_t<mln::experimental::image2d<uint8_t>>;

  my_accu_t() = default;

  template <class Pix>
  void take(Pix px)
  {
    auto p = px.point();
    auto v = px.val();
    m_count++;
    m_sum += v;
    m_sum_sqr += v * v;
    m_xmin = std::min((int)p.x(), m_xmin);
    m_xmax = std::max((int)p.x(), m_xmax);
    m_ymin = std::min((int)p.y(), m_ymin);
    m_ymax = std::max((int)p.y(), m_ymax);
  }

  void take(const my_accu_t& other)
  {
    m_count   += other.m_count;
    m_sum     += other.m_sum;
    m_sum_sqr += other.m_sum_sqr;
    m_xmin    = std::min(m_xmin, other.m_xmin);
    m_ymin    = std::min(m_ymin, other.m_ymin);
    m_xmax    = std::max(m_xmax, other.m_xmax);
    m_ymax    = std::max(m_ymax, other.m_ymax);
  }

  int count() const { return m_count; }
  int area_bbox() const { return (m_xmax - m_xmin + 1) * (m_ymax - m_ymin + 1); }
  double variance() const { return (m_sum_sqr - (m_sum * m_sum) / m_count) / m_count; }

  my_accu_t to_result() const { return *this; }

private:
  int    m_count   = 0;
  double m_sum     = 0;
  double m_sum_sqr = 0;
  int    m_xmin    = INT_MAX;
  int    m_ymin    = INT_MAX;
  int    m_xmax    = INT_MIN;
  int    m_ymax    = INT_MIN;
};




int main()
{
  mln::experimental::image2d<uint8_t> f;
  mln::io::experimental::imread("roadsigns.png", f);

  // Compute the ToS
  auto [t, node_map] = mln::morpho::experimental::tos(f, {0,0});


  // Set f to the right size
  mln::experimental::image2d<uint8_t> f2 = t.reconstruct(node_map);


  // Compute the bounding box & the size of the component
  auto prop_map = t.compute_attribute_on_pixels(node_map, f2, my_accu_t{});

  // Predicate: keep the node if it fits 95% of the bbox
  // and its size is less than 100000px
  auto pred = [&prop_map](int x) {
    int sz = prop_map[x].count();
    return sz > (0.95 * prop_map[x].area_bbox()) && sz > 10000 and sz < 1000000 and prop_map[x].variance() > 250;
  };

  // Filter
  t.filter(mln::morpho::experimental::CT_FILTER_DIRECT, node_map, pred);

  // Reconstruct
  t.values[0] = 0;
  auto out = t.reconstruct(node_map);
  mln::io::experimental::imsave(out, "roadsigns-square-2.png");
}
