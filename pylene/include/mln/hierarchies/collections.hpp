#pragma once

#include <vector>

namespace mln
{
  class QBT
  {
  public:
    QBT(std::size_t nb_vertices);

    void        make_set(std::size_t q);
    std::size_t make_union(std::size_t cx, std::size_t cy);
    std::size_t find_canonical(std::size_t q);

    std::size_t size;
    int*        parent;
  };

  class QT
  {
  public:
    QT(std::size_t nb_vertices);

    void        make_set();
    std::size_t make_union(std::size_t cx, std::size_t cy);
    std::size_t find_canonical(std::size_t q);

    std::size_t size;
    int*        parent;
    int*        rank;
  };

  class QEBT
  {
  public:
    QEBT(std::size_t nb_vertices);

    void        make_set(std::size_t q);
    std::size_t make_union(std::size_t cx, std::size_t cy);
    std::size_t find_canonical(std::size_t q);


  private:
    int* root;
    QBT  qbt;
    QT   qt;
  };
} // namespace mln
