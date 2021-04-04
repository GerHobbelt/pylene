#pragma once

#include <vector>

#include "mln/hierarchies/hierarchy_tree.hpp"

namespace mln
{
  class QBT : public HierarchyTree
  {
    friend class QEBT;

  public:
    QBT(const Graph* leaf_graph);

    ~QBT() = default;

    void make_set(int q);
    int  make_union(int cx, int cy);
    int  find_canonical(int q);

    int get_nb_vertices() const override;
    int get_parent(int node) const override;

  private:
    int              nb_vertices_;
    int              size_;
    std::vector<int> parent_;
  };

  class QT : public HierarchyTree
  {
    friend class QEBT;

  public:
    QT(const Graph* leaf_graph);

    ~QT() = default;

    void make_set();
    int  make_union(int cx, int cy);
    int  find_canonical(int q);

    int get_nb_vertices() const override;
    int get_parent(int node) const override;

  private:
    int              nb_vertices_;
    int              size_;
    std::vector<int> parent_;
    std::vector<int> rank_;
  };

  class QEBT : public HierarchyTree
  {
  public:
    QEBT(const Graph* leaf_graph);

    ~QEBT() = default;

    void make_set(int q);
    int  make_union(int cx, int cy);
    int  find_canonical(int q);

    int get_nb_vertices() const override;
    int get_parent(int node) const override;

    inline const QBT& get_qbt() const { return qbt_; };

  private:
    QBT              qbt_;
    QT               qt_;
    std::vector<int> root_;
  };
} // namespace mln
