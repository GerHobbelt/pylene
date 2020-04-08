#pragma once


#include <mln/core/concept/neighborhoods.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <utility>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/container/flat_set.hpp>


namespace mln::labeling::experimental
{

  // Compute the IZ of a binary image and its RAG
  template <typename Label = int16_t, typename I, typename N, typename Graph>
  image_ch_value_t<I, Label> rag(I input, N nbh, Graph& graph, int& nlabel);



  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {

    template <typename I, typename N, typename Graph, typename J>
    int rag(I ima, N nbh, Graph& graph, J&& out)
    {
      using P       = image_point_t<I>;
      using label_t = image_value_t<std::remove_reference_t<J>>;

      constexpr label_t LABEL_MAX = std::numeric_limits<label_t>::max();
      constexpr label_t FRONTIER  = 0;
      constexpr label_t INFRONT   = -1;
      constexpr label_t UNSEEN    = -2;

      P* queue = new P[ima.domain().size()];

      unsigned qend   = 0;
      std::size_t fstart = ima.domain().size();
      std::size_t fend   = ima.domain().size();

      auto qpush  = [&queue, &qend](const P& p) { queue[qend++] = p; };
      auto qpop   = [&queue, &qend]() -> P { return queue[--qend]; };
      auto qempty = [&qend]() -> bool { return qend == 0; };

      auto fpush  = [&queue, &fstart](const P& p) { queue[--fstart] = p; };
      auto fpop   = [&queue, &fend]() -> P { return queue[--fend]; };
      auto fempty = [&fstart, &fend]() -> bool { return fend == fstart; };

      auto dom = ima.domain();
      int nlabel = 0;

      // 1. Push labelize CC and add border pixel in queue
      mln_foreach_new (auto p, dom)
      {
        if (!ima(p) || out(p) > 0)
          continue;

        if (nlabel++ >= LABEL_MAX)
          throw std::runtime_error("Detected overflow in the number of labels.");

        qpush(p);
        out(p) = nlabel;

        do
        {
          auto p = qpop();
          for (auto n : nbh(p))
          {
            if (!dom.has(n) || out(n) != UNSEEN)
              continue;

            if (ima(n))
            {
              qpush(n);
              out(n) = nlabel;
            }
            else
            {
              fpush(n);
              out(n) = INFRONT;
            }
          }
        } while (not qempty());
      }

      graph = Graph(nlabel + 1);

      // 2. Add vertices to the graph
      boost::container::flat_set<int> tomerge;

      while (not fempty())
      {
        auto p = fpop();

        int pval = INFRONT;
        tomerge.clear();

        // Check if it has to be labeled
        for (auto q : nbh(p))
        {
          int qval;
          if (!dom.has(q) || (qval = out(q)) < 0)
            continue;

          if (pval <= 0)
            pval = qval;
          else if (pval != qval && qval > 0) // We merge two IZ
            tomerge.insert(qval);
        }

        if (!tomerge.empty())
        {
          assert(pval > 0);
          tomerge.insert(pval);
          auto        beg = tomerge.begin();
          std::size_t sz  = tomerge.size();
          for (std::size_t i = 0; i < sz; ++i)
            for (std::size_t j = i + 1; j < sz; ++j)
              boost::add_edge(beg[i], beg[j], graph);

          out(p) = FRONTIER;
          continue;
        }

        // Not a frontier pixel so labelize and add neighbors in the queue
        out(p) = pval;
        for (auto q : nbh(p))
        {
          if (!dom.has(q) || out(q) != UNSEEN)
            continue;

          fpush(q);
          out(q) = INFRONT;
        }
      }
      // Every unlabeled pixel gets the background label
      mln::for_each(out, [](auto& v) { if (v < 0) v = FRONTIER; });

      delete[] queue;
      return nlabel;
    }

  } // namespace impl


  template <typename Label, typename I, typename N, typename Graph>
  image_ch_value_t<I, Label> rag(I input, N nbh, Graph& graph, int& nlabel)
  {
    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<N, mln::experimental::Neighborhood>());

    static_assert(std::is_convertible<image_value_t<I>, bool>::value, "Image value type must be convertible to bool.");
    static_assert(std::is_same<typename boost::graph_traits<Graph>::directed_category, boost::undirected_tag>::value,
                  "Graph must undirected");


    using descr_type = typename boost::graph_traits<Graph>::vertex_descriptor;
    static_assert(std::is_integral_v<descr_type>, "Graph must have integral vertex descriptor.");
    static_assert(std::is_signed_v<Label>, "Label type must be signed.");

    mln_entering("mln::labeling::rag");

    constexpr Label UNSEEN = -2;
    image_ch_value_t<I, Label> out = imchvalue<Label>(input).set_init_value(UNSEEN);
    nlabel = impl::rag(std::move(input), std::move(nbh), graph, out);

    return out;
  }

}

