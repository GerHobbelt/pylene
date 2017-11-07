#include <mln/core/foreach.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/graph/graph_image2d.hpp>

#include <gtest/gtest.h>

TEST(Graph, graph_allocation)
{
  using namespace mln;

  box2d b({-2, -3}, {3, 4});

  static int nv;
  static int ne;

  nv = 0;
  ne = 0;

  struct Vobj
  {
    char x[8];
    Vobj() { ++nv; }            // std::cout << "con" << std::endl; }
    Vobj(const Vobj&) { ++nv; } // std::cout << "cpy" << std::endl; }
    ~Vobj() { --nv; }           // std::cout << "des" << std::endl;  }
  };

  struct Eobj
  {
    Eobj() { ++ne; }
    Eobj(const Eobj&) { ++ne; }
    ~Eobj() { --ne; }

    char x[6];
  };

  {
    mln::graph::undirected_graph_image2d<Vobj, Eobj, c4_t> g(b, c4, 0);
    int n = b.shape()[0];
    int m = b.shape()[1];
    ASSERT_EQ(nv, n * m);
    ASSERT_EQ(ne, n * (m - 1) + (n - 1) * (2 * m - 1));
  }

  ASSERT_EQ(nv, 0);
  ASSERT_EQ(ne, 0);
}

TEST(Graph, graph_iteration)
{
  using namespace mln;

  box2d b({-2, -3}, {3, 4});

  mln::graph::undirected_graph_image2d<char, float, c4_t> g(b, c4);
  unsigned nv = 0, ne = 0;

  mln_foreach (point2d p, g.vertices())
    g.vertex(p) = 'a' + (nv++ % 26);

  mln_foreach (point2d p, g.edges())
    g.edge(p) = float(ne++);

  {
    std::cout << "Vertex list: ";
    mln_foreach (point2d p, g.vertices())
      std::cout << "(" << p << ":" << g.vertex(p) << "), ";
    std::cout << std::endl;
  }
  {
    std::cout << "Edges list: ";
    mln_foreach (point2d p, g.edges())
      std::cout << "(" << p << ":" << g.edge(p) << "), ";
    std::cout << std::endl;
  }

  int n = b.shape()[0];
  int m = b.shape()[1];
  ASSERT_EQ(nv, n * m);
  ASSERT_EQ(ne, n * (m - 1) + (n - 1) * (2 * m - 1));
}

TEST(Graph, graph_nbh)
{
  using namespace mln;

  box2d b({-2, -3}, {3, 4});

  mln::graph::undirected_graph_image2d<char, float, c4_t> g(b, c4);
  unsigned nv = 0, ne = 0;

  mln_foreach (point2d p, g.vertices())
    g.vertex(p) = 'a' + (nv++ % 26);

  mln_foreach (point2d p, g.edges())
    g.edge(p) = float(ne++);

  {
    std::cout << "Vertex list: " << std::endl;
    mln_foreach (point2d p, g.vertices())
    {
      std::cout << "(" << p << ":" << g.vertex(p) << "): ";
      mln_foreach (point2d e, g.edges(p))
        std::cout << "(" << e << ":" << g.edge(e) << "), ";
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  {
    std::cout << "Vertex list: " << std::endl;
    mln_foreach (point2d p, g.vertices())
    {
      std::cout << "(" << p << ":" << g.vertex(p) << "): ";
      mln_foreach (point2d q, g.adjacent_vertices(p))
        std::cout << "(" << q << ":" << g.vertex(q) << "), ";
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}
