#pragma once

// helper macro
#ifdef PYLENE_CONCEPT_TS_ENABLED
#define PYLENE_CONCEPT_TS_ASSERT(expr, mess) static_assert((expr), mess);
#else
#define PYLENE_CONCEPT_TS_ASSERT(expr, mess)
#endif // PYLENE_CONCEPT_TS_ENABLED
