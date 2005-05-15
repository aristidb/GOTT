#ifndef GOTT_UTIL_MISC_COMMON_HEADERS_H
#define GOTT_UTIL_MISC_COMMON_HEADERS_H

#ifdef HAVE_VISIBILITY
#pragma GCC visibility push(default)
#endif

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <exception>

#include <sstream>
#include <istream>
#include <ostream>
#include <iostream>

#include <new>
#include <locale>
#include <utility>
#include <typeinfo>
#include <memory>

#ifdef HAVE_VISIBILITY
#pragma GCC visibility pop
#endif

#include <algorithm>
#include <functional>

#include <cstring>
#include <cstddef>
#include <cwctype>

#include <boost/cstdint.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

#include <boost/optional/optional.hpp>
#include <boost/utility.hpp>
#include <boost/none.hpp>
#include <boost/variant.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/assert.hpp>

#include <boost/assign.hpp>

#include <gott/util/xany/xany.hpp>
#include <gott/util/misc/range_algo.hpp>
#include <gott/util/misc/my_hash_map.hpp>
#include <gott/util/misc/visibility.hpp>
#include <gott/util/misc/autoconv.hpp>

#include <gott/util/debug/warnings.hpp>

#endif
