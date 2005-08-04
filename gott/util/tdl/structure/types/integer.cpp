// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
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

#include "integer.hpp"
#include <gott/util/tdl/exceptions.hpp>

namespace structure = gott::tdl::structure;

using structure::repatch_integer;
using structure::writable_structure;

repatch_integer::repatch_integer() {}
repatch_integer::~repatch_integer() {}

writable_structure *
repatch_integer::deferred_write(writable_structure &s) const {
  struct context : simple_repatcher_context {
    context(writable_structure &s) : simple_repatcher_context(s) {}

    void data(xany::Xany const &x) {
      if (x.compatible<long>())
        target.data(x);
      else if (x.compatible<std::wstring>()) {
        std::wstring input = xany::Xany_cast<std::wstring>(x);
        long result;
        if (!is_integer(input, result))
          throw failed_repatch(L"repatch_integer: could not match integer");
        target.data(xany::Xany(result));
      } else
        throw failed_repatch(L"repatch_integer: could not match integer");
    }

    bool is_integer(std::wstring const &s, long &val) {
      long sign = 1;
      val = 0;
  
      std::wstring::const_iterator it = s.begin();
  
      if (it == s.end())
        return false;

      if (*it == L'-') {
        ++it;
        sign = -1;
        if (it == s.end())
          return false;
      }
  
      if (!iswdigit(*it))
        return false;
    
//    if (*it == L'0' && it[1] == L'x')
//      return is_hex(it + 2, s.end(), v, sign)

      for (; it != s.end() && std::iswdigit(*it); ++it)
        val = val * 10 + (*it - L'0');

      if (it != s.end())
        return false;

      val *= sign;
      return true;
    }
  };
  return new context(s);
}
