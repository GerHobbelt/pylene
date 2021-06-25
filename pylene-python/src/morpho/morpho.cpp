#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/morpho/alphatree.hpp>

#include <pln/core/image_cast.hpp>

#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include <functional>
#include <stdexcept>
#include <tuple>

#include "morpho.hpp"

namespace pln::morpho
{
    namespace
    {
        using result_type = std::tuple<mln::morpho::component_tree<void>, std::vector<double>, mln::ndbuffer_image>;

        result_type alphatree(mln::ndbuffer_image img)
        {
            if (img.pdim() != 2)
                throw std::invalid_argument("Input image must be a 2D image");
            if (img.sample_type() == mln::sample_type_id::UINT8)
            {
                auto* image_casted = img.cast_to<std::uint8_t, 2>();
                auto dist = [](const std::uint8_t a, const std::uint8_t b) -> double { return mln::functional::l2dist(a, b);};
                auto [t, nm] = mln::morpho::alphatree(*image_casted, mln::c4, dist);
                return {t, std::move(t.values), nm};
            }
            else if (img.sample_type() == mln::sample_type_id::RGB8)
            {
                auto* image_casted = img.cast_to<mln::rgb8, 2>();
                auto dist = [](const mln::rgb8& a, const mln::rgb8& b) -> double { return mln::functional::l2dist(a, b);};
                auto [t, nm] = mln::morpho::alphatree(*image_casted, mln::c4, dist);
                return {t, std::move(t.values), nm};
            }
            else
            {
                throw std::invalid_argument("Only alphatree for uint8 and rgb8 image are available");
            }
        }
    }

    void define_morpho_module(pybind11::module_& m)
    {
        using tree_t = mln::morpho::component_tree<void>;
        using image_t = mln::ndbuffer_image;

        auto morpho = m.def_submodule("morpho", "Mathematical morphology module");

        pybind11::enum_<mln::morpho::ct_filtering>(morpho, "ct_filtering")
            .value("CT_FILTER_DIRECT", mln::morpho::ct_filtering::CT_FILTER_DIRECT)
            .value("CT_FILTER_MIN", mln::morpho::ct_filtering::CT_FILTER_MIN)
            .value("CT_FILTER_MAX", mln::morpho::ct_filtering::CT_FILTER_MAX);

        pybind11::class_<mln::morpho::component_tree<void>>(morpho, "component_tree")
            .def_readwrite("parent", &tree_t::parent, "The mapping from a node to its parent")
            .def("filter", [](tree_t& self, mln::morpho::ct_filtering strategy, image_t nodemap, std::function<bool(int)> pred) {
                auto* nodemap_casted = nodemap.cast_to<int, 2>();
                if (!nodemap_casted)
                    throw std::invalid_argument("Nodemap should be a 2D int image.");
                self.filter(strategy, *nodemap_casted, pred);
            },
            "Filter the tree with a filtering strategy given a predicate\nstrategy: the strategy to filter\n"
            "nodemap: the mapping image point -> tree node\npred: the predicate for the filtering",
            pybind11::arg("strategy"), pybind11::arg("nodemap"), pybind11::arg("pred"))
            .def("compute_depth", &tree_t::compute_depth)
            .def("horizontal_cut", [](tree_t& self, const double th, image_t nodemap, const std::vector<double>& values) 
                -> image_t
            {
                auto* nodemap_casted = nodemap.cast_to<int, 2>();
                if (!nodemap_casted)
                    throw std::invalid_argument("Nodemap should be a 2D int image.");
                return self.horizontal_cut_from_levels(th, *nodemap_casted, ::ranges::make_span(values.data(), values.size()));
            },
            "Make a horizontal cut of the tree at a given altitude and return the nodemap corresponding to the cut\n"
            "th: the altitude to do the horizontal cut\n"
            "nodemap: the mapping image point -> tree node\n"
            "values: the altitude of the tree",
            pybind11::arg("th"), pybind11::arg("nodemap"), pybind11::arg("values"))
            .def("reconstruct_from_mean", [](tree_t& self, image_t nodemap, image_t image) 
                -> image_t
            {
                if (nodemap.domain() != image.domain())
                    throw std::invalid_argument("nodemap and domain must have the same dimensions.");
                if (nodemap.pdim() != 2)
                    throw std::invalid_argument("Only 2D images are handled.");
                auto* nodemap_casted = nodemap.cast_to<int, 2>();
                if (!nodemap_casted)
                    throw std::invalid_argument("Nodemap should be a 2D int image.");
                if (image.sample_type() == mln::sample_type_id::UINT8)
                {
                    auto* image_casted = image.cast_to<std::uint8_t, 2>();
                    auto mean = self.compute_attribute_on_values(*nodemap_casted, *image_casted, mln::accu::accumulators::mean<std::uint8_t>());
                    return self.reconstruct_from(*nodemap_casted, ::ranges::make_span(mean.data(), mean.size()));
                }
                else if (image.sample_type() == mln::sample_type_id::RGB8)
                {
                    auto* image_casted = image.cast_to<mln::rgb8, 2>();
                    auto mean = self.compute_attribute_on_values(*nodemap_casted, *image_casted, mln::accu::accumulators::mean<mln::rgb8>());
                    std::vector<mln::rgb8> mean_casted(mean.size());
                    for (std::size_t i = 0; i < mean.size(); i++)
                        mean_casted[i] = mean[i];
                    return self.reconstruct_from(*nodemap_casted, ::ranges::make_span<mln::rgb8>(mean_casted.data(), mean_casted.size()));
                }
                else
                {
                    throw std::invalid_argument("Image have to be uint8 or rgb8");
                }
            })
            .def("area", [](tree_t self, image_t nodemap) {
                auto* nodemap_casted = nodemap.cast_to<int, 2>();
                if (!nodemap_casted)
                    throw std::invalid_argument("Nodemap should be 2D int image.");
                return self.compute_attribute_on_points(*nodemap_casted, mln::accu::accumulators::count<>());
            });

            morpho.def("alphatree", &alphatree, "Compute the alpahtree of a graph computed from an image using the 4-connectivity and a distance L2", pybind11::arg("img"));
    }
}