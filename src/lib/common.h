#ifndef MGCP_LIB_COMMON_H
#define MGCP_LIB_COMMON_H

#include "./helpers.h"

#define stdlog(x) std::cout << x << '\n';

// add custom assert so a message can be appended
#define m_assert(expr, msg) assert((msg, expr))

// #include "boost/type_index.hpp"
// #define reflectType(x) std::cout << boost::typeindex::type_id_runtime(x).pretty_name() << '\n';

#endif