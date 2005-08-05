// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Common GOTT headers
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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
#include <stdexcept>

#ifdef HAVE_VISIBILITY
#pragma GCC visibility pop
#endif

#include <sstream>
#include <istream>
#include <ostream>
#include <iostream>

#include <new>
#include <locale>
#include <utility>
#include <typeinfo>
#include <memory>

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
#include <gott/util/range_algo.hpp>
#include <gott/util/my_hash_map.hpp>
#include HH_HASH_MAP
#include HH_HASH_SET
#include <gott/util/visibility.hpp>
#include <gott/util/autoconv.hpp>

#include <gott/util/debug/warnings.hpp>
#include <gott/util/debug/assert.hpp>
#include <gott/util/debug/util.hpp>

#endif
