#include "smartdoc.hpp"
#include <mln/core/algorithm/copy.hpp>
#include <mln/io/imsave.hpp>
#include <mln/accu/accumulators/accu_as_it.hpp>
#include <mln/accu/accumulators/count.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <apps/tos/croutines.hpp>


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
          // Si les points du quadri sont distants de plus de 5%
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

std::pair<bool, std::array<point2d, 4> >
process(tree_t& tree,
        const image2d<uint16>& ima,
        image2d<rgb8>* feedback,
        const char* csvfile,
        const char* treefile)
{

  // Filter first
  grain_filter_inplace(tree, GRAINSIZE);


  // Accumulation du nombre de feuilles + profondeurs.
  property_map<tree_t, unsigned> nleaves(tree, 0);
  property_map<tree_t, unsigned> sumdepth(tree, 0);
  property_map<tree_t, unsigned> sumdepth2(tree, 0);
  property_map<tree_t, unsigned> depth = morpho::compute_depth(tree);
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
  auto quadri = morpho::paccumulate(tree, ima, accu_t ());



  // Recompute the point inside/outside
  using vec2df = vec<float, 2>;

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


  // Save tree
  if (treefile)
    morpho::save(tree, treefile);

  // {
  //   accu::accumulators::fitting_quad test;
  //   test.take({1,2});
  //   test.take({2,6});
  //   test.take({9,1});
  //   test.take({8,7});

  //   std::cout << test.m_pmin << test.m_pmax << "\n";
  //   for (int i = 0; i < 4; ++i)
  //     std::cout << test.m_quad[i];
  //   std::cout << "\n" << test.to_result() << std::endl;
  // }

  auto noising = make_functional_property_map<tree_t::node_type>
    ([&sumdepth2,&area](const tree_t::node_type& x) {
      return (sumdepth2[x] / float(area[x]));
    });


  float imsize = ima.domain().size();
  auto energy = make_functional_property_map<tree_t::node_type>
    ([&quadri,&noising,&area,&depth,imsize,maxdepth](const tree_t::node_type& x) -> float{
      if (area[x] < (MIN_OBJECT_SIZE * imsize) or
          area[x] > (MAX_OBJECT_SIZE * imsize))
        return 0;
      return WEIGHT_QUAD * quadri[x].to_result() + WEIGHT_NOISE * noising[x];
    });

  // Save attributes
  if (csvfile)
    {
      std::ofstream f(csvfile);
      f << "nleaves,sumdepth,noising,quadri,inside,energy,bbox,quad\n";
      mln_foreach(auto x, tree.nodes())
        f << nleaves[x]
          << "," << sumdepth[x]
          << "," << noising[x]
          << "," << quadri[x].to_result()
          << "," << (quadri[x].m_inside / float(quadri[x].m_count))
          << "," << energy[x]
          << "," << "\"" << quadri[x].m_pmin << quadri[x].m_pmax << "\""
          << "," << "\"" << quadri[x].m_quad[0] << quadri[x].m_quad[1] << quadri[x].m_quad[2] << quadri[x].m_quad[3] << "\""
          << "\n";
    }


  //
  tree_t::node_type shp = tree.get_root();
  {
    float emax = value_traits<float>::min();
    mln_foreach(auto x, tree.nodes())
      {
        float e = energy[x];
        if (e > emax) {
          emax = e;
          shp = x;
        }
      }
  }

  if (feedback)
    {
      image2d<rgb8> out = transform(ima, [maxdepth](uint16 x) {
          uint8 v = std::min(1.0f, (x / maxdepth)) * 127;
          return rgb8(v);
        });

      vec2df u = (quadri[shp].m_quad[1] - quadri[shp].m_quad[0]).as_vec();
      vec2df v = (quadri[shp].m_quad[2] - quadri[shp].m_quad[0]).as_vec();
      vec2df w = (quadri[shp].m_quad[3] - quadri[shp].m_quad[0]).as_vec();
      float n1 = (u[0]*v[1] - u[1]*v[0]);
      float n2 = (w[0]*v[1] - w[1]*v[0]);
      mln_foreach(point2d x, ima.domain())
        {
          vec2df p = x.as_vec() - quadri[shp].m_quad[0].as_vec();

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
      std::cout << out.domain() << feedback->domain() << std::endl;
      copy(out, *feedback);
    }
  std::array<point2d, 4> res;
  std::copy(quadri[shp].m_quad, quadri[shp].m_quad + 4, res.begin());
  return std::make_pair(true, res);
}

