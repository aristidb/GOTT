// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Simple SAX-like TDL parser
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

#include "parser.hpp"

using std::wistream;
using std::wstring;
using gott::util::tdl::simple::meta_parser;

void gott::util::tdl::simple::parse_meta(wistream &in, meta_parser &p, 
                                         line_logger *) {
  while (in) {
    if (in.peek() != L'#') {
      if (in.peek() != L'\n')
        break;
      else
        in.get();
    } else {
      in.get();
      if (in.peek() != L'?') {
        in.putback(L'#');
        break;
      }
      in.get();
      wstring s;
      getline(in, s);
      p.exec(s);
    }
  }
}

namespace {
  struct pass {
    bool operator() (wstring const &, wstring const &) const {
      return false;
    }
  };
}

meta_parser::meta_parser() : def(pass()) {
}

void meta_parser::exec(wstring const &line) {
  wstring::const_iterator start = line.begin(), pos;
  for (pos = line.begin(); pos != line.end(); ++pos)
    if (*pos == L' ')
      break;
  
  wstring cmd(start, pos);
  wstring param;
  while (*pos == L' ' && ++pos != line.end())
    ;
  param.assign(pos, line.end());

  cb_t::iterator it = cb.find(cmd);

  if (it == cb.end())
    def(cmd, param);
  else
    if (!it->second(cmd, param))
      def(cmd, param);
}

void meta_parser::set_default(callback const &f) {
  def = f;
}

void meta_parser::set_specific(wstring const &cmd, callback const &f) {
  cb.insert(make_pair(cmd, f));
}
