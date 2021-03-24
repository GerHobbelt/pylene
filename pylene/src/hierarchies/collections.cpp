#include "mln/hierarchies/collections.hpp"

namespace mln
{
  QBT::QBT(unsigned nb_vertices)
  {
    this->size   = 0;
    this->parent = new int[2 * nb_vertices - 1];

    for (unsigned i = 0; i < 2 * nb_vertices - 1; i++)
    {
      this->parent[i] = -1;
    }
  }

  void QBT::make_set(unsigned q)
  {
    this->parent[q] = -1;
    this->size += 1;
  }

  unsigned QBT::make_union(unsigned cx, unsigned cy)
  {
    this->parent[cx] = this->size;
    this->parent[cy] = this->size;
    this->make_set(this->size);
    return this->size - 1;
  }

  unsigned QBT::find_canonical(unsigned q)
  {
    while (this->parent[q] >= 0)
      q = this->parent[q];

    return q;
  }

  QT::QT(unsigned nb_vertices)
  {
    this->size   = 0;
    this->parent = new int[nb_vertices];
    this->rank   = new int[nb_vertices];

    for (unsigned i = 0; i < nb_vertices; i++)
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

  unsigned QT::make_union(unsigned cx, unsigned cy)
  {
    if (this->rank[cx] > this->rank[cy])
    {
      unsigned tmp = cx;
      cx           = cy;
      cy           = tmp;
    }

    if (this->rank[cx] == this->rank[cy])
    {
      this->rank[cy] += 1;
    }

    this->parent[cx] = cy;
    return cy;
  }

  unsigned QT::find_canonical(unsigned q)
  {
    unsigned r = q;

    while (this->parent[r] >= 0)
      r = this->parent[r];

    while (this->parent[q] >= 0)
    {
      unsigned tmp      = q;
      q                 = this->parent[q];
      this->parent[tmp] = r;
    }

    return r;
  }

  QEBT::QEBT(unsigned nb_vertices)
    : qbt(QBT(nb_vertices))
    , qt(QT(nb_vertices))
  {
    this->root = new int[nb_vertices];

    for (unsigned i = 0; i < nb_vertices; i++)
    {
      this->root[i] = -1;
    }
  }

  void QEBT::make_set(unsigned q)
  {
    this->root[q] = q;
    this->qbt.make_set(q);
    this->qt.make_set();
  }

  unsigned QEBT::make_union(unsigned cx, unsigned cy)
  {
    unsigned tu = this->root[cx];
    unsigned tv = this->root[cy];

    this->qbt.parent[tu] = this->qbt.size;
    this->qbt.parent[tv] = this->qbt.size;

    unsigned c    = this->qt.make_union(cx, cy);
    this->root[c] = this->qbt.size;
    this->qbt.make_set(this->qbt.size);
    return this->qbt.size - 1;
  }

  unsigned QEBT::find_canonical(unsigned q) { return this->qt.find_canonical(q); }
} // namespace mln
