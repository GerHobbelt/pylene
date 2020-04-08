#pragma once

// helper macro
#define PYLENE_CONCEPT_TS_ASSERT(expr, mess) static_assert((expr), mess);
#else
#define PYLENE_CONCEPT_TS_ASSERT(expr, mess)
