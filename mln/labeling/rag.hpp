#ifndef MLN_LABELING_RAG_HPP
# define MLN_LABELING_RAG_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/extension/extension.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/trace.hpp>
# include <utility>

namespace mln
{
  namespace labeling
  {

    template <typename I, typename N, typename Label = unsigned>
    std::pair<mln_ch_value(I, Label), Label>
    rag(const Image<I>& ima, const N& nbh, Label lbl = unsigned());


/******************************************/
/****          Implementation          ****/
/******************************************/

    namespace impl
    {


      template <typename I, typename N, typename Label, typename J>
      Label
      rag(const I& ima, const N& nbh, Label lbl, J&& out)
      {
        typedef mln_value(I) V;
        typedef mln_point(I) P;

        const Label bg = value_traits<Label>::max();
        const Label INFRONT = lbl;

        extension::fill(out, INFRONT);

        P* queue = new P[ima.domain().size()];

        unsigned qstart = 0, qend = 0;
        unsigned fstart = ima.domain().size();
        unsigned fend = ima.domain().size();

        auto qpush = [&queue, &qstart, &qend] (const P& p) { queue[qend++] = p; };
        auto qpop = [&queue, &qstart, &qend] () -> P { return queue[--qend]; };
        auto qempty = [&queue, &qstart, &qend] () -> bool { return qend == 0; };

        auto fpush = [&queue, &fstart, &fend] (const P& p) { queue[--fstart] = p; };
        auto fpop =  [&queue, &fstart, &fend] () -> P { return queue[--fend]; };
        auto fempty = [&queue, &fstart, &fend] () -> bool { return fend == fstart; };

        P q;
        mln_iter(n, nbh(q));

        mln_foreach(P p, ima.domain())
        {
          if (ima(p) and out(p) == bg)
            {
              qpush(p);
              out(p) = ++lbl;
              mln_assertion(lbl < value_traits<Label>::max());
              while (not qempty())
                {
                  q = qpop();
                  mln_forall(n)
                    if (out.at(*n) == bg)
                      {
                        if (ima(*n)) {
                          qpush(*n);
                          out(*n) = lbl;
                        } else {
                          fpush(*n);
                          out(*n) = INFRONT;
                        }
                      }
                }
            }
        }

        while (not fempty())
          {
            q = fpop();
            Label& v = out(q);
            mln_forall(n)
            {
              Label x = out.at(*n);
              if (x != INFRONT and x != bg)
                {
                  if (v == INFRONT)
                    v = x;
                  else if (v != x) {
                    v = INFRONT;
                    break;
                  }
                }
            }

            if (v == INFRONT) {
              continue;
            }

            mln_forall(n)
              if (out.at(*n) == bg)
                {
                  fpush(*n);
                  out(*n) = INFRONT;
                }
          }

        mln_pixter(px, out);
        mln_forall(px)
          if (px->val() == bg)
            px->val() = INFRONT;


        delete [] queue;
        return lbl;
      }


    } // end of namespace mln::labeling::impl


    template <typename I, typename N, typename Label>
    std::pair<mln_ch_value(I, Label), Label>
    rag(const Image<I>& ima_, const N& nbh, Label lbl)
    {
      static_assert(std::is_convertible<mln_value(I), bool>::value,
                    "Image value type must be convertible to bool.");


      mln_entering("mln::labeling::blobs");

      const I& ima = exact(ima_);
      const Label bg = value_traits<Label>::max();

      mln_ch_value(I, Label) out;
      bool nocheck_boundary = resize(out, ima).adjust(nbh).init(bg);

      if (nocheck_boundary)
	lbl = impl::rag(ima, nbh, lbl, out);
      else
	lbl = impl::rag(ima, nbh, lbl, extension::add_value_extension(out,bg));

      mln_exiting();

      return std::make_pair(out, lbl);
    }

  } // end of namespace mln::labeling

} // end of namespace mln

#endif //!MLN_LABELING_RAG_HPP
