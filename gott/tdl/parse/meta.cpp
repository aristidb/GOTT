/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "meta.hpp"
#include <gott/string/string.hpp>
#include <map>

using gott::string;
using std::istream;
using tdl::meta_parser;

static bool pass(gott::string const &, gott::string const &) {
  return false;
}

class meta_parser::impl {
public:
  impl() : def(pass) {}

  callback def;
  typedef std::map<string, callback> cb_t;
  cb_t cb;
  void exec(string const &);
};

meta_parser::meta_parser() : p(new impl) {
}

meta_parser::~meta_parser() {}

void meta_parser::parse(istream &in, line_logger *) {
  while (in) {
    if (in.peek() != '#') {
      if (in.peek() != '\n')
        break;
      else
        in.get();
    } else {
      in.get();
      if (in.peek() != '?') {
        in.putback('#');
        break;
      }
      in.get();
      std::string s;
      getline(in, s);
      p->exec(s);
    }
  }
}

void meta_parser::impl::exec(string const &line_) {
  string::utf32_range line = line_.as_utf32();
  gott::utf8_iterator pos = line.begin();
  for (; pos != line.end(); ++pos)
    if (*pos == L' ')
      break;
  
  string cmd(range(line.begin(), pos));
  while (*pos == L' ' && ++pos != line.end())
    ;
  string param(range(pos, line.end()));

  cb_t::const_iterator it = cb.find(cmd);

  if (it == cb.end() || !it->second(cmd, param))
    def(cmd, param);
}

void meta_parser::set_default(callback const &f) {
  p->def = f;
}

void meta_parser::set_specific(string const &cmd, callback const &f) {
  p->cb.insert(impl::cb_t::value_type(cmd, f));
}
