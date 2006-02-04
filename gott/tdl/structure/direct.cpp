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

#include "structure.hpp"
#include "../simple/parse/parser.hpp"
#include <gott/string/string.hpp>

using gott::xany::Xany;
using gott::tdl::simple::parser;
using gott::tdl::structure::writable_structure;

namespace {
  class my_parser : public parser {
    writable_structure &s;
    bool data;

    void begin_parse() {}
    void end_parse() {}
    void comment(gott::string const &, bool) {}

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

    void node(gott::string const &x) {
      if (data)
        s.end();
  
      s.begin();
      s.data(Xany(x));
    
      data = true;
    }

  public:
    my_parser(writable_structure &ss) : parser(0), s(ss), data(false) {}
    virtual ~my_parser() {}
  };
}

void gott::tdl::structure::direct_struc(std::istream &in, 
                                              writable_structure &out) {
  my_parser(out).parse(in);
}
