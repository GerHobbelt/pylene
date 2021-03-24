#pragma once

#include <vector>

namespace mln
{
    class QEBT
    {
        public:

        QEBT(unsigned nb_vertices);

        void make_set(unsigned q);
        unsigned make_union(unsigned q);
        unsigned find_canonical(unsigned q);


        private:

        unsigned nb_vertices;
        std::vector<unsigned> root;
        QBT qbt;
        QT qt;
    };

    class QT
    {
        public:

        QT(unsigned nb_vertices);

        void make_set(unsigned q);
        unsigned make_union(unsigned q);
        unsigned find_canonical(unsigned q);


        private:

        unsigned size;
        std::vector<unsigned> parent;
        std::vector<unsigned> rank;
    };

    class QBT
    {
        public:

        QBT(unsigned nb_vertices);

        void make_set(unsigned q);
        unsigned make_union(unsigned q);
        unsigned find_canonical(unsigned q);


        private:

        unsigned size;
        std::vector<unsigned> parent;
    };
} // namespace mln
