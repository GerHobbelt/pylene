#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/concept/new/ranges.hpp>

#ifdef PYLENE_CONCEPT_TS_ENABLED
#define PYLENE_CONCEPT_REQUIRE_CHECK(Concept, ...) requires Concept<__VA_ARGS__>
#else
#define PYLENE_CONCEPT_REQUIRE_CHECK(Concept, ...)
#endif // PYLENE_CONCEPT_TS_ENABLED
