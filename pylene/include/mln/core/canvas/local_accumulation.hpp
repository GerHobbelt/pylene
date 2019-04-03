#pragma once

#include <mln/core/canvas/local_algorithm.hpp>


namespace mln::canvas
{


  template <class Accu, class SE, class I, class J,
            bool __incremental__ = SE::incremental::value&& Accu::has_untake::value>
  class LocalAccumulation;


  template <class Accu, class SE, class I, class J>
  class LocalAccumulation<Accu, SE, I, J, false> : public LocalAlgorithm<SE, I, J>
  {
  private:
    using base_t = LocalAlgorithm<SE, I, J>;
    Accu m_accu;


  public:
    LocalAccumulation(Accu accu, SE se, I& f, J& g)
      : base_t{std::move(se), f, g}
      , m_accu{std::move(accu)}
    {
    }


  private:
    void ExecuteAtLineStart() final { m_accu.init(); }

    void EvalBeforeLocalLoop(image_reference_t<I> /*pval_i*/, image_reference_t<J> /*pval_j*/) final { m_accu.init(); }

    void EvalAfterLocalLoop(image_reference_t<I> /* pval_i */, image_reference_t<J> pval_j) final
    {
      pval_j = m_accu.to_result();
    }

    void EvalInLocalLoop(image_reference_t<I> nval_i, image_reference_t<I> /* pval_i */,
                         image_reference_t<J> /*pval_j*/) final
    {
      m_accu.take(nval_i);
    }
  };


  template <class Accu, class SE, class I, class J>
  class LocalAccumulation<Accu, SE, I, J, true> : public IncrementalLocalAlgorithm<SE, I, J>
  {
  private:
    using base_t = IncrementalLocalAlgorithm<SE, I, J>;
    Accu m_accu;

  public:
    LocalAccumulation(Accu accu, SE se, I& f, J& g)
      : base_t{std::move(se), f, g}
      , m_accu{std::move(accu)}
    {
    }


  private:
    void ExecuteAtLineStart() final { m_accu.init(); }

    void EvalBeforeLocalLoop(image_reference_t<I> /* pval_i */, image_reference_t<J> /* pval_j */) final
    {
      m_accu.init();
    }

    void EvalAfterLocalLoop(image_reference_t<I> /* pval_i */, image_reference_t<J> pval_j) final
    {
      pval_j = m_accu.to_result();
    }

    void EvalInLocalLoop(image_reference_t<I> nval_i, image_reference_t<I> /* pval_i */,
                         image_reference_t<J> /*pval_j*/) final
    {
      m_accu.take(nval_i);
    }


    void EvalInIncLoop(image_reference_t<I> nval_i, image_reference_t<I> /*pval_i*/,
                       image_reference_t<J> /*pval_j*/) final
    {
      m_accu.take(nval_i);
    }

    void EvalInDecLoop(image_reference_t<I> nval_i, image_reference_t<I> /*pval_i*/,
                       image_reference_t<J> /*pval_j*/) final
    {
      m_accu.untake(nval_i);
    }
  };


  template <class Accu, class SE, class I, class J>
  LocalAccumulation(Accu, SE, I&, J&)->LocalAccumulation<Accu, SE, I, J>;

  // Clang7 doesn't deduce the template parameters correctly
  template <class Accu, class SE, class I, class J>
  auto make_LocalAccumulation(Accu&& accu, SE&& se, I&& i, J&& j)
      -> LocalAccumulation<std::remove_cv_t<std::remove_reference_t<Accu>>,
                           std::remove_cv_t<std::remove_reference_t<SE>>, std::remove_cv_t<std::remove_reference_t<I>>,
                           std::remove_cv_t<std::remove_reference_t<J>>>
  {
    return {std::forward<Accu>(accu), std::forward<SE>(se), std::forward<I>(i), std::forward<J>(j)};
  }

} // namespace mln::canvas
