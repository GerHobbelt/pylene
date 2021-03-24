#pragma once

#include <vector>

namespace mln
{
  class QBT
  {
  public:
    QBT(unsigned nb_vertices);

    void     make_set(unsigned q);
    unsigned make_union(unsigned cx, unsigned cy);
    unsigned find_canonical(unsigned q);

    unsigned size;
    int*     parent;
  };

  class QT
  {
  public:
    QT(unsigned nb_vertices);

    void     make_set();
    unsigned make_union(unsigned cx, unsigned cy);
    unsigned find_canonical(unsigned q);

    unsigned size;
    int*     parent;
    int*     rank;
  };

  class QEBT
  {
  public:
    QEBT(unsigned nb_vertices);

    void     make_set(unsigned q);
    unsigned make_union(unsigned cx, unsigned cy);
    unsigned find_canonical(unsigned q);


  private:
    unsigned nb_vertices;
    int*     root;
    QBT      qbt;
    QT       qt;
  };
} // namespace mln
