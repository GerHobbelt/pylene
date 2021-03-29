#include "mln/hierarchies/collections.hpp"

namespace mln
{
  QBT::QBT(std::size_t nb_vertices)
  {
    this->size   = 0;
    this->parent = new int[2 * nb_vertices - 1];

    for (std::size_t i = 0; i < 2 * nb_vertices - 1; i++)
    {
      this->parent[i] = -1;
    }
  }

  void QBT::make_set(std::size_t q)
  {
    this->parent[q] = -1;
    this->size += 1;
  }

  std::size_t QBT::make_union(std::size_t cx, std::size_t cy)
  {
    this->parent[cx] = this->size;
    this->parent[cy] = this->size;
    this->make_set(this->size);
    return this->size - 1;
  }

  std::size_t QBT::find_canonical(std::size_t q)
  {
    while (this->parent[q] >= 0)
      q = this->parent[q];

    return q;
  }

  QT::QT(std::size_t nb_vertices)
  {
    this->size   = 0;
    this->parent = new int[nb_vertices];
    this->rank   = new int[nb_vertices];

    for (std::size_t i = 0; i < nb_vertices; i++)
    {
      this->parent[i] = -1;
      this->rank[i]   = -1;
    }
  }

  void QT::make_set()
  {
    this->parent[this->size] = -1;
    this->rank[this->size]   = 0;
    this->size += 1;
  }

  std::size_t QT::make_union(std::size_t cx, std::size_t cy)
  {
    if (this->rank[cx] > this->rank[cy])
    {
      std::size_t tmp = cx;
      cx              = cy;
      cy              = tmp;
    }

    if (this->rank[cx] == this->rank[cy])
    {
      this->rank[cy] += 1;
    }

    this->parent[cx] = cy;
    return cy;
  }

  std::size_t QT::find_canonical(std::size_t q)
  {
    std::size_t r = q;

    while (this->parent[r] >= 0)
      r = this->parent[r];

    while (this->parent[q] >= 0)
    {
      std::size_t tmp   = q;
      q                 = this->parent[q];
      this->parent[tmp] = r;
    }

    return r;
  }

  QEBT::QEBT(std::size_t nb_vertices)
    : qbt(QBT(nb_vertices))
    , qt(QT(nb_vertices))
  {
    this->root = new int[nb_vertices];

    for (std::size_t i = 0; i < nb_vertices; i++)
    {
      this->root[i] = -1;
    }
  }

  void QEBT::make_set(std::size_t q)
  {
    this->root[q] = q;
    this->qbt.make_set(q);
    this->qt.make_set();
  }

  std::size_t QEBT::make_union(std::size_t cx, std::size_t cy)
  {
    std::size_t tu = this->root[cx];
    std::size_t tv = this->root[cy];

    this->qbt.parent[tu] = this->qbt.size;
    this->qbt.parent[tv] = this->qbt.size;

    std::size_t c = this->qt.make_union(cx, cy);
    this->root[c] = this->qbt.size;
    this->qbt.make_set(this->qbt.size);
    return this->qbt.size - 1;
  }

  std::size_t QEBT::find_canonical(std::size_t q) { return this->qt.find_canonical(q); }
} // namespace mln
