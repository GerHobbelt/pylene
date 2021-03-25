#pragma once

#include <vector>

#include "mln/hierarchies/hierarchy_tree.hpp"

namespace mln
{
  class QBT : public HierarchyTree
  {
  public:
    QBT(std::size_t nb_vertices);

    void        make_set(std::size_t q);
    std::size_t make_union(std::size_t cx, std::size_t cy);
    std::size_t find_canonical(std::size_t q);

    std::size_t get_nb_vertices() const override;
    std::size_t get_parent(std::size_t node) const override;

    std::size_t size;
    int*        parent;
  };

  class QT : public HierarchyTree
  {
  public:
    QT(std::size_t nb_vertices);

    void        make_set();
    std::size_t make_union(std::size_t cx, std::size_t cy);
    std::size_t find_canonical(std::size_t q);

    std::size_t get_nb_vertices() const override;
    std::size_t get_parent(std::size_t node) const override;

    std::size_t size;
    int*        parent;
    int*        rank;
  };

  class QEBT : public HierarchyTree
  {
  public:
    QEBT(std::size_t nb_vertices);

    void        make_set(std::size_t q);
    std::size_t make_union(std::size_t cx, std::size_t cy);
    std::size_t find_canonical(std::size_t q);

    std::size_t get_nb_vertices() const override;
    std::size_t get_parent(std::size_t node) const override;

  private:
    std::size_t nb_vertices;
    int*        root;
    QBT         qbt;
    QT          qt;
  };
} // namespace mln
