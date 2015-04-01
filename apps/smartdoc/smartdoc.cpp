#include "smartdoc.hpp"
#include <mln/core/algorithm/copy.hpp>
#include <mln/io/imsave.hpp>
#include <mln/accu/accumulators/accu_as_it.hpp>
#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/max.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/extinction.hpp>
#include <apps/tos/croutines.hpp>

static constexpr int EXTINCTION_THRESHOLD = 0.05;
static constexpr int MINIMUM_ABSOLUTE_SIZE = 300000; // 500 * 500; // 400*400*4

namespace mln
{

  template <typename T>
  using vec2d = vec<T, 2>;

  template <typename T>
  float
  mydist(vec2d<T> x, vec2d<T> y)
  {
    int a = abs(y[0] - x[0]);
    int b = abs(y[1] - x[1]);
    int i,j;
    std::tie(i,j) = std::minmax(a,b);
    return i * 2 + (j-i) * 1;
  }


  namespace accu
  {
    namespace accumulators
    {
      struct fitting_quad : accumulator_base<fitting_quad, point2d, float, void>
      {
        typedef point2d argument_type;
        typedef float   result_type;

        fitting_quad()
        {
          for (int i = 0; i < 4; ++i)
            m_quad[i] = value_traits<point2d>::sup();
          m_pmin = value_traits<point2d>::sup();
          m_pmax = value_traits<point2d>::inf();
          m_count = 0;
          m_inside = 0;
        }

        void init()
        {
          for (int i = 0; i < 4; ++i)
            m_quad[i] = value_traits<point2d>::sup();
          m_pmin = value_traits<point2d>::sup();
          m_pmax = value_traits<point2d>::inf();
          m_count = 0;
          m_inside = 0;
        }

        void take(point2d x)
        {
          m_pmin = inf(m_pmin, x);
          m_pmax = sup(m_pmax, x);

          vec2d<short> pts[4];
          pts[0] = {m_pmin[0], m_pmin[1]};
          pts[1] = {m_pmin[0], m_pmax[1]};
          pts[2] = {m_pmax[0], m_pmax[1]};
          pts[3] = {m_pmax[0], m_pmin[1]};

          for (int i = 0; i < 4; ++i)
            {
              float d1 = mydist(pts[i], (vec2d<short>)m_quad[i]);
              float d2 = mydist(pts[i], (vec2d<short>)x);
              if (d2 < d1) {
                m_quad[i] = x;
              }
            }
          m_count++;
        }

        void take(const fitting_quad& other)
        {
          m_pmin = inf(m_pmin, other.m_pmin);
          m_pmax = sup(m_pmax, other.m_pmax);

          vec2d<short> pts[4];
          pts[0] = {m_pmin[0], m_pmin[1]};
          pts[1] = {m_pmin[0], m_pmax[1]};
          pts[2] = {m_pmax[0], m_pmax[1]};
          pts[3] = {m_pmax[0], m_pmin[1]};

          for (int i = 0; i < 4; ++i)
            {
              float d1 = mydist(pts[i], (vec2d<short>)m_quad[i]);
              float d2 = mydist(pts[i], (vec2d<short>)other.m_quad[i]);
              if (d1 < d2) {
                m_quad[i] = m_quad[i];
              } else {
                m_quad[i] = other.m_quad[i];
              }
            }

          m_count += other.m_count;
        }

        friend
        unsigned
        extract(const fitting_quad& accu, accu::features::count<>)
        {
          return accu.m_count;
        }


        float to_result() const
        {
          // Si les points du quadri sont distants de plus de CORNER_TOLERANCE %
          // de bbbox, c'est vain
          float scale = 1.0 / mydist(m_pmin.as_vec(), m_pmax.as_vec());
          vec2d<short> pts[4];
          pts[0] = {m_pmin[0], m_pmin[1]};
          pts[1] = {m_pmin[0], m_pmax[1]};
          pts[2] = {m_pmax[0], m_pmax[1]};
          pts[3] = {m_pmax[0], m_pmin[1]};

          // float sumdist = 0;
          // for (int i = 0; i < 4; ++i) {
          //   sumdist += l2dist_sqr(m_quad[i], (point2df)pts[i]) * scale;
          // }
          // return sumdist;


          for (int i = 0; i < 4; ++i) {
            float d = mydist((vec2d<short>)m_quad[i], pts[i]) * scale;
            if (d > CORNER_TOLERANCE)
              return 0;
          }


          // Calcul de l'aire du //gramme
          // Somme des cross products entre les vertexes
          float s = 0.0;

          for (int i = 0; i < 3; ++i)
            s += m_quad[i][0] * m_quad[i+1][1] - m_quad[i+1][0] * m_quad[i][1];
          s += m_quad[3][0] * m_quad[0][1] - m_quad[0][0] * m_quad[3][1];

          float area = (0.5 * abs(s));
          float m_outside = m_count - m_inside;
          return (0.8 * m_inside - 1.2 * m_outside) / area;
        }

      public:
        unsigned m_count;
        unsigned m_inside;
        point2d  m_pmin, m_pmax;
        point2df m_quad[4];
      };

    }

  }

}


using namespace mln;
using vec2df = vec<float, 2>;


std::array<process_result_t, NQUAD>
process(tree_t& tree,
        const image2d<uint16>& ima,
        const char* csvfile,
        const char* treefile)
{

  // Filter first
  {
    grain_filter_inplace(tree, GRAINSIZE);
  }

  property_map<tree_t, float> energy(tree);
  property_map<tree_t, accu::accumulators::fitting_quad> quadri;
  property_map<tree_t, unsigned> depth = morpho::compute_depth(tree);

  // 1. Compute energy related attributes
  {

    // Accumulation du nombre de feuilles + profondeurs.
    property_map<tree_t, unsigned> nleaves(tree, 0);
    property_map<tree_t, unsigned> sumdepth(tree, 0);
    property_map<tree_t, unsigned> sumdepth2(tree, 0);
    property_map<tree_t, unsigned> area = morpho::accumulate(tree, accu::features::count<unsigned> ());

    float maxdepth = 0;
    mln_reverse_foreach(auto x, tree.nodes())
      {
        if (not x.has_child())
          {
            nleaves[x]++;
            sumdepth[x] += depth[x];
          }
        if (depth[x] > maxdepth)
          maxdepth = depth[x];
        nleaves[x.parent()] += nleaves[x];
        sumdepth[x.parent()] += sumdepth[x];
        sumdepth2[x] = sumdepth[x] - (depth[x] * nleaves[x]);
      }

    typedef accu::accumulators::accu_as_it<accu::accumulators::fitting_quad> accu_t;
    quadri = morpho::paccumulate(tree, ima, accu_t ());

  // Recompute the point inside/outside
    mln_pixter(px, ima);
    mln_forall(px)
    {
      tree_t::node_type x = tree.get_node_at(px->index());
      vec2df p_ = px->point().as_vec();

      while (x.id() != tree.npos())
        {
          vec2df p = p_ - quadri[x].m_quad[0].as_vec();
          vec2df u = (quadri[x].m_quad[1] - quadri[x].m_quad[0]).as_vec();
          vec2df v = (quadri[x].m_quad[2] - quadri[x].m_quad[0]).as_vec();
          vec2df w = (quadri[x].m_quad[3] - quadri[x].m_quad[0]).as_vec();
          bool inside1, inside2;
          {
            float n = (u[0]*v[1] - u[1]*v[0]);
            float alpha = (u[0]*p[1] - u[1]*p[0]) / n;
            float beta = (p[0]*v[1] - p[1]*v[0]) / n;
            inside1 = 0 <= alpha and 0 <= beta and (alpha + beta) <= 1;
          }
          {
            float n = (w[0]*v[1] - w[1]*v[0]);
            float alpha = (w[0]*p[1] - w[1]*p[0]) / n;
            float beta = (p[0]*v[1] - p[1]*v[0]) / n;
            inside2 = 0 <= alpha and 0 <= beta and (alpha + beta) <= 1;
          }

          quadri[x].m_inside += inside1 or inside2;
          x = x.parent();
        }
    }

    auto noising = make_functional_property_map<tree_t::node_type>
      ([&sumdepth2,&area](const tree_t::node_type& x) {
        return (sumdepth2[x] / float(area[x]));
      });

    float imsize = ima.domain().size();

    // property_map<tree_t, unsigned> area = morpho::accumulate(tree, accu::features::count<unsigned> ());
    // unsigned root_id = tree.get_root_id();
    // auto predicate = make_functional_property_map<tree_t::vertex_id_t>([&area, imsize, root_id](tree_t::vertex_id_t x) {
    //     return (x == root_id) or
    //     ((area[x] > (MIN_OBJECT_SIZE * imsize)) and
    //      (area[x] < (MAX_OBJECT_SIZE * imsize)));
    //   });
    // filter_direct_inplace(tree, predicate);

    {
      int w = ima.ncols();
      int h = ima.nrows();

      mln_foreach (const tree_t::node_type& x, tree.nodes())
        {
          if ((area[x] < (MINIMUM_ABSOLUTE_SIZE)) or (area[x] > (MAX_OBJECT_SIZE * imsize)))
            energy[x] = 0;
          else if (quadri[x].m_pmin[0] < 50 or
                   quadri[x].m_pmin[1] < 50 or
                   (w-quadri[x].m_pmax[1]) < 50 or
                   (h-quadri[x].m_pmax[0]) < 50) // if we are to close from the border
            energy[x] = 0;
          else
            energy[x] = WEIGHT_QUAD * quadri[x].to_result() + WEIGHT_NOISE * noising[x];
        }
    }

    // Save tree
    if (treefile)
      morpho::save(tree, treefile);

    // Save attributes
    if (csvfile)
      {
        property_map<tree_t, float> extvalues;
        {
          auto eimg = morpho::make_image(tree, energy);
          auto extimg = morpho::extinction(eimg, morpho::tree_neighb_t (), std::greater<float> ());
          extvalues = std::move(extimg.get_vmap());
        }

        std::ofstream f(csvfile);
        f << "nleaves,sumdepth,noising,quadri,inside,energy,bbox,quad,extinction\n";
        mln_foreach(auto x, tree.nodes())
          f << nleaves[x]
            << "," << sumdepth[x]
            << "," << noising[x]
            << "," << quadri[x].to_result()
            << "," << (quadri[x].m_inside / float(quadri[x].m_count))
            << "," << energy[x]
            << "," << "\"" << quadri[x].m_pmin << quadri[x].m_pmax << "\""
            << "," << "\"" << quadri[x].m_quad[0] << quadri[x].m_quad[1] << quadri[x].m_quad[2] << quadri[x].m_quad[3] << "\""
            << "," << extvalues[x]
            << "\n";
      }
  }

  // 2. Compute extinction values
  property_map<tree_t, float> extvalues;
  {
    auto eimg = morpho::make_image(tree, energy);
    auto extimg = morpho::extinction(eimg, morpho::tree_neighb_t (), std::greater<float> ());
    extvalues = std::move(extimg.get_vmap());
  }

  // 3. Get the top three shapes
  constexpr int NMINIMA = 10;

  tree_t::node_type shps[NMINIMA];

  {
    auto heapcmp = [&extvalues](const tree_t::node_type& x, const tree_t::node_type& y) {
      return extvalues[x] > extvalues[y];
    };
    std::fill(shps, shps + NMINIMA, tree.get_root());

    mln_foreach(auto x, tree.nodes())
      {
        if (extvalues[x] > extvalues[shps[0]])
          {
            std::pop_heap(shps, shps + NMINIMA, heapcmp);
            shps[NMINIMA-1] = x;
            std::push_heap(shps, shps + NMINIMA, heapcmp);
          }
      }

    std::sort_heap(shps, shps + NMINIMA, heapcmp);
    std::cout << "emin:" << extvalues[shps[NMINIMA-1]]
              << " , emax: " << extvalues[shps[0]] << std::endl;

    // Filter the nodes whose extinction is below 0.1
    for (int i = NMINIMA - 1; i >= 0 and extvalues[shps[i]] < EXTINCTION_THRESHOLD; --i)
      energy[shps[i]] = 0;
  }

  // 4. Sort the shapes by energy
  std::sort(shps, shps+NMINIMA, [&energy] (const tree_t::node_type& x, const tree_t::node_type& y) {
      return energy[x] > energy[y];
    });

  // 5. If there are two shapes nested, get the biggest shape. (Reject threshold of 70% energetic)
  // {
  //   for (int i = 1; i < 3; ++i)
  //     if (energy[shps[i]] > 0.9 and
  //         energy[shps[i]] > (0.70 * energy[shps[0]]) and
  //         lca(tree, depth, shps[0], shps[i]) == shps[i]) {
  //       std::swap(shps[0], shps[i]);
  //       break;
  //     }
  // }


  std::array<process_result_t, NQUAD> res;
  for (int i = 0; i < NQUAD; ++i) {
    std::copy(quadri[shps[i]].m_quad, quadri[shps[i]].m_quad + 4, res[i].points.begin());
    res[i].energy = energy[shps[i]];
  }

  return res;
}

void
draw_quad_superimpose(std::array<mln::point2d, 4> quad,
                      const image2d<uint16>& depth,
                      image2d<rgb8>& out)
{
  float maxdepth = accumulate(depth, accu::features::max<>());

  transform(depth, [maxdepth](uint16 x) {
      uint8 v = std::min(1.0f, (x / maxdepth)) * 127;
      return rgb8(v);
    }, out);

  vec2df u = (quad[1] - quad[0]).as_vec();
  vec2df v = (quad[2] - quad[0]).as_vec();
  vec2df w = (quad[3] - quad[0]).as_vec();
  float n1 = (u[0]*v[1] - u[1]*v[0]);
  float n2 = (w[0]*v[1] - w[1]*v[0]);
  mln_foreach(point2d x, out.domain())
    {
      vec2df p = x.as_vec() - quad[0].as_vec();

      bool inside1, inside2;
      {
        float alpha = (u[0]*p[1] - u[1]*p[0]) / n1;
        float beta = (p[0]*v[1] - p[1]*v[0]) / n1;
        inside1 = 0 <= alpha and 0 <= beta and (alpha + beta) <= 1;
      }
      {
        float alpha = (w[0]*p[1] - w[1]*p[0]) / n2;
        float beta = (p[0]*v[1] - p[1]*v[0]) / n2;
        inside2 = 0 <= alpha and 0 <= beta and (alpha + beta) <= 1;
      }

      if (inside1 or inside2)
        out(x)[0] += 128;
    }
}
