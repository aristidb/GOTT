// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
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

#include "structure.hpp"
#include "../simple/parse/parser.hpp"
#include <gott/util/nstring/nstring.hpp>


using gott::xany::Xany;
using gott::tdl::simple::parser;
using gott::tdl::structure::writable_structure;

namespace {
  class my_parser : public parser {
    writable_structure &s;
    bool data;

    void begin_parse() {}
    void end_parse() {}
    void comment(gott::nstring const &, bool) {}

    void down() {
      if (!data)
        s.begin();
      else
        data = false;
    }

    void up() {
      if (data) {
        s.end();
        data = false;
      }
      s.end();
    }

    void node(gott::nstring const &x) {
      if (data)
        s.end();
  
      s.begin();
      s.data(Xany(x));
    
      data = true;
    }

  public:
    my_parser(writable_structure &ss) : s(ss), data(false) {}
    virtual ~my_parser() {}
  };
}

void gott::tdl::structure::direct_struc(std::wistream &in, 
                                              writable_structure &out) {
  my_parser pp(out);
  parser &p = pp;
  parse(in, p);
}
