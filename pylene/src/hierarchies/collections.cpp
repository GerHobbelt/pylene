#include "mln/hierarchies/collections.hpp"
#include "mln/hierarchies/graph.hpp"

namespace mln
{
  QBT::QBT(const Graph* leaf_graph)
    : HierarchyTree(leaf_graph)
    , nb_vertices_(2 * leaf_graph->get_nb_vertices() - 1)
    , size_(0)
    , parent_(nb_vertices_, -1)
  {
  }

  void QBT::make_set(int q)
  {
    this->parent_[q] = -1;
    this->size_ += 1;
  }

  int QBT::make_union(int cx, int cy)
  {
    this->parent_[cx] = this->size_;
    this->parent_[cy] = this->size_;
    this->make_set(this->size_);
    return this->size_ - 1;
  }

  int QBT::find_canonical(int q)
  {
    while (this->parent_[q] >= 0)
      q = this->parent_[q];

    return q;
  }

  int QBT::get_nb_vertices() const { return nb_vertices_; }

  int QBT::get_parent(int node) const { return parent_[node]; }

  QT::QT(const Graph* leaf_graph)
    : HierarchyTree(leaf_graph)
    , nb_vertices_(leaf_graph->get_nb_vertices())
    , size_(0)
    , parent_(nb_vertices_, -1)
    , rank_(nb_vertices_, -1)
  {
  }

  void QT::make_set()
  {
    this->parent_[this->size_] = -1;
    this->rank_[this->size_]   = 0;
    this->size_ += 1;
  }

  int QT::make_union(int cx, int cy)
  {
    if (this->rank_[cx] > this->rank_[cy])
    {
      int tmp = cx;
      cx      = cy;
      cy      = tmp;
    }

    if (this->rank_[cx] == this->rank_[cy])
    {
      this->rank_[cy] += 1;
    }

    this->parent_[cx] = cy;
    return cy;
  }

  int QT::find_canonical(int q)
  {
    int r = q;

    while (this->parent_[r] >= 0)
      r = this->parent_[r];

    while (this->parent_[q] >= 0)
    {
      int tmp            = q;
      q                  = this->parent_[q];
      this->parent_[tmp] = r;
    }

    return r;
  }

  int QT::get_nb_vertices() const { return nb_vertices_; }

  int QT::get_parent(int node) const { return parent_[node]; }

  QEBT::QEBT(const Graph* leaf_graph)
    : HierarchyTree(leaf_graph)
    , qbt_(QBT(leaf_graph))
    , qt_(QT(leaf_graph))
    , root_(qbt_.nb_vertices_, -1)
  {
  }

  void QEBT::make_set(int q)
  {
    this->root_[q] = q;
    this->qbt_.make_set(q);
    this->qt_.make_set();
  }

  int QEBT::make_union(int cx, int cy)
  {
    int tu = this->root_[cx];
    int tv = this->root_[cy];

    this->qbt_.parent_[tu] = this->qbt_.size_;
    this->qbt_.parent_[tv] = this->qbt_.size_;

    int c          = this->qt_.make_union(cx, cy);
    this->root_[c] = this->qbt_.size_;
    this->qbt_.make_set(this->qbt_.size_);
    return this->qbt_.size_ - 1;
  }

  int QEBT::find_canonical(int q) { return this->qt_.find_canonical(q); }

  int QEBT::get_nb_vertices() const { return qbt_.nb_vertices_; }

  int QEBT::get_parent(int node) const { return qbt_.parent_[node]; }
} // namespace mln
