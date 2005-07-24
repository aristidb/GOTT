// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

namespace structure = gott::util::tdl::structure;

using structure::repatch_integer;
using structure::repatch_context;
using structure::writable_structure;

repatch_context *repatch_integer::deferred_write(writable_structure &s) {
  struct context : repatch_context {
    context(writable_structure &s) : target(s) {}

    writable_structure &target;

    void data(xany::Xany const &x) {
      if (x.compatible<long>())
        target.data(x);
      else if (x.compatible<std::wstring>()) {
        std::wstring input = xany::Xany_cast<std::wstring>(x);
        wchar_t const *begin = input.c_str();
        wchar_t const *end = begin + input.length();
        wchar_t **int_end;
        long result = std::wcstol(input.c_str(), int_end, 0);
        if (*int_end != end)
          throw failed_repatch(L"repatch_integer: could not match integer");
        target.data(xany::Xany(result));
      }
    }

    void begin() { sorry(); }
    void end() { sorry(); }
    void add_tag(std::wstring const &) { sorry(); }
    void set_tags(std::list<std::wstring> const &) { sorry(); }

    void sorry() {
      throw failed_repatch(L"repatch_integer: accept data solely");
    }
  };
  return new context(s);
}
