#include "mln/hierarchies/graph.hpp"

namespace mln
{
  void Graph::add_edge(int source, int destination, double weight)
  {
    this->edges_.emplace_back(std::make_tuple(source, destination, weight));
  }

  int Graph::get_edge(int n) const { return n - this->nb_vertices_; }

  double Graph::weight_node(int n) const
  {
    auto triplet = this->mst[this->get_edge(n)];

    return std::get<2>(triplet);
  }

  static bool sortbyth(const Edge& a, const Edge& b) { return (std::get<2>(a) < std::get<2>(b)); }

  QEBT Graph::kruskal()
  {
    QEBT qebt(*this);

    for (int xi = 0; xi < this->nb_vertices_; xi++)
      qebt.make_set(xi);

    std::sort(this->edges_.begin(), this->edges_.end(), sortbyth);

    for (auto edge : this->edges_)
    {
      int cx = qebt.find_canonical(std::get<0>(edge));
      int cy = qebt.find_canonical(std::get<1>(edge));

      if (cx != cy)
      {
        qebt.make_union(cx, cy);
        this->mst.emplace_back(edge);
      }
    }

    return qebt;
  }

  Graph::Graph(int height, int width, const mln::image2d<rgb8>& image)
    : nb_vertices_(height * width)
    , height_(height)
    , width_(width)
  {
    auto shape = image.domain();

    int x0 = shape.x();
    int y0 = shape.y();

    for (int x = 0; x < shape.height(); x++)
    {
      for (int y = 0; y < shape.width(); y++)
      {
        rgb8 pix = image({x0 + x, y0 + y});

        // FIXME Edges weight must be of type double

        if (x0 + x < shape.height() - 1)
        {
          rgb8   pix3    = image({x0 + x + 1, y0 + y});
          double weighty = sqrt(pow(pix3[0] - pix[0], 2) + pow(pix3[1] - pix[1], 2) + pow(pix3[2] - pix[2], 2));
          this->add_edge((x0 + x) * shape.width() + (y0 + y), (x0 + x + 1) * shape.width() + (y0 + y), weighty);
        }

        if (y0 + y < shape.width() - 1)
        {
          rgb8   pix2    = image({x0 + x, y0 + y + 1});
          double weightx = sqrt(pow(pix2[0] - pix[0], 2) + pow(pix2[1] - pix[1], 2) + pow(pix2[2] - pix[2], 2));
          this->add_edge((x0 + x) * shape.width() + (y0 + y), (x0 + x) * shape.width() + (y0 + y + 1), weightx);
        }
      }
    }
  }
} // namespace mln