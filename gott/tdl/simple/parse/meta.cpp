// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Simple SAX-like TDL parser
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

#include "meta.hpp"
#include <gott/string/string.hpp>
#include <gott/string/stl.hpp>

using std::wistream;
using gott::tdl::simple::meta_parser;

static bool pass(gott::string const &, gott::string const &) {
  return false;
}

NTL_MOVEABLE(meta_parser::callback);

class meta_parser::IMPL {
public:
  IMPL() : def(pass) {}

  callback def;
  typedef VectorMap<string, callback> cb_t;
  cb_t cb;
  void exec(string const &);
};

meta_parser::meta_parser() : p(new IMPL) {
}

meta_parser::~meta_parser() {}

void meta_parser::parse(std::wistream &in, line_logger *) {
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
      std::wstring s;
      getline(in, s);
      p->exec(s);
    }
  }
}

void meta_parser::IMPL::exec(string const &line_) {
  string::utf32_range line = line_.as_utf32();
  utf8_iterator pos = line.begin;
  for (; pos != line.end; ++pos)
    if (*pos == L' ')
      break;
  
  string cmd(range(line.begin, pos));
  while (*pos == L' ' && ++pos != line.end)
    ;
  string param(range(pos, line.end));

  int i = cb.Find(cmd);

  if (i < 0 || !cb[i](cmd, param))
    def(cmd, param);
}

void meta_parser::set_default(callback const &f) {
  p->def = f;
}

void meta_parser::set_specific(string const &cmd, callback const &f) {
  p->cb.Add(cmd, f);
}
