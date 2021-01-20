#include <mln/core/image/ndimage.hpp>
#include <mln/python/image_cast.hpp>
#include <mln/python/morpho.hpp>

#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <stdexcept>

namespace pln
{
  namespace impl
  {
    template <std::size_t N>
    auto ndtos(const mln::ndbuffer_image& img)
    {
      assert(img.sample_type() == mln::sample_type_id::UINT8 || img.sample_type() == mln::sample_type_id::UINT16);
      mln::__ndbuffer_image<std::uint16_t, N> in;
      if (img.sample_type() == mln::sample_type_id::UINT8)
      {
        const auto& casted = img.__cast<std::uint8_t, N>();
        in                 = mln::__ndbuffer_image<std::uint16_t, N>(casted.domain());
        mln::paste(casted, in);
      }
      else
        in = img.__cast<std::uint16_t, N>();
      auto [t, n]             = mln::morpho::tos(in, mln::ndpoint<N>());
      std::array<int, 1> size = {
          static_cast<int>(t.values.size()),
      };
      auto v =
          mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(t.values.data()),
                                           mln::sample_type_traits<std::uint16_t>::id(), 1, size.data(), nullptr, true);
      return std::make_tuple(std::move(t), std::move(n), std::move(v));
    }
  } // namespace impl

  auto tos(const mln::ndbuffer_image& img)
  {
    using namespace mln;

    morpho::component_tree<> t;
    ndbuffer_image           n;
    ndbuffer_image           v;

    if (img.pdim() == 2)
      std::tie(t, n, v) = impl::ndtos<2>(img);
    else if (img.pdim() == 3)
      std::tie(t, n, v) = impl::ndtos<3>(img);
    else
      throw std::runtime_error("Image dimension unhandled");
    return std::make_tuple(std::move(t), std::move(n), std::move(v));
  }

  void def_morpho_module(pybind11::module_& m)
  {
    using namespace mln::morpho;
    auto morpho_m = m.def_submodule("morpho");

    pybind11::enum_<ct_filtering>(morpho_m, "ct_filtering")
        .value("CT_FILTER_DIRECT", ct_filtering::CT_FILTER_DIRECT)
        .value("CT_FILTER_MIN", ct_filtering::CT_FILTER_MIN)
        .value("CT_FILTER_MAX", ct_filtering::CT_FILTER_MAX);

    using tree_t = component_tree<void>;
    pybind11::class_<tree_t>(morpho_m, "component_tree")
        .def("compute_depth", &tree_t::compute_depth)
        .def("filter",
             [](tree_t& t, ct_filtering st, std::function<bool(tree_t::node_id_type)> pred) { t.filter(st, pred); })
        .def("filter",
             [](tree_t& t, ct_filtering st, mln::ndbuffer_image& nodemap,
                std::function<bool(tree_t::node_id_type)> pred) {
               t.filter(st, nodemap.__cast<tree_t::node_id_type, 2>(), pred);
             })
        .def("reconstruct_from", [](tree_t& t, mln::ndbuffer_image& nodemap, mln::ndbuffer_image& values) {
          switch (values.sample_type())
          {
          case mln::sample_type_id::UINT8: {
            auto& v = values.__cast<typename mln::sample_type_id_traits<mln::sample_type_id::UINT8>::type, 1>();
            return static_cast<mln::ndbuffer_image>(t.reconstruct_from(nodemap.__cast<tree_t::node_id_type, 2>(),
                                                                       ranges::make_span(v.buffer(), v.size(0))));
          }
          case mln::sample_type_id::UINT16: {
            auto& v = values.__cast<typename mln::sample_type_id_traits<mln::sample_type_id::UINT16>::type, 1>();
            return static_cast<mln::ndbuffer_image>(t.reconstruct_from(nodemap.__cast<tree_t::node_id_type, 2>(),
                                                                       ranges::make_span(v.buffer(), v.size(0))));
          }
          default:
            throw std::runtime_error("Unhandled value type for reconstruction");
          }
        });

    morpho_m.def("tos", &tos);
  }
} // namespace pln