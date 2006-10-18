// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#include <gott/tdl/parse/meta.hpp>
#include <sstream>
#include <gott/string/string.hpp>
#include <vector>
#include <testsoon.hpp>

using std::pair;
using std::ostream;
using std::istringstream;
using tdl::meta_parser;
using tdl::source_position;
using gott::string;

typedef pair<string, string> twostring;

namespace {
struct group_fixture_t {
  meta_parser parser;
  string data, rest;
  std::vector<twostring> xp, ev;
  source_position final_position;
  bool operator() (string const &cmd, string const &param) {
    ev.push_back(pair<string,string>(cmd, param));
    return true;
  }
  void expect(string const &cmd, string const &param = string()) {
    xp.push_back(pair<string,string>(cmd, param));
  }
  void run_test() {
    istringstream x(data);
    final_position = parser.parse(x);
    rest = gott::string(x.str().c_str() + x.tellg(), gott::utf8);
  }
  void run_test(string const &data) {
    this->data = data;
    run_test();
  }
  group_fixture_t() { parser.set_default(boost::ref(*this)); }
};

}

namespace gott {
ostream &operator<<(ostream &o, pair<string,string> const &x) {
  return o << x.first << "::" << x.second << '\n';
}
}

GFTEST() {
  group_fixture.run_test("#?foobar qulux-dei zql");
  group_fixture.expect("foobar", "qulux-dei zql");
  Equals(range(group_fixture.ev), range(group_fixture.xp));
  Equals(group_fixture.final_position, 
      source_position("", 1, 0, "foobar qulux-dei zql", 1, 0));
}

GFTEST() {
  group_fixture.data = "\n\n\n\n#?real     kluft\n\n\n\n\n   \n       \n\n#?delta_x yz";
  group_fixture.run_test();
  group_fixture.expect("real", "kluft");
  Equals(range(group_fixture.ev), range(group_fixture.xp));
  Equals(group_fixture.rest, string("   \n       \n\n#?delta_x yz"));
  Equals(group_fixture.final_position,
      source_position("", 9, 0, "real     kluft", 5, 0));
}

GFTEST() {
  group_fixture.data = "#?a\n#? b\n#\n#?c";
  group_fixture.run_test();
  group_fixture.expect("a");
  group_fixture.expect("", "b");
  Equals(range(group_fixture.ev), range(group_fixture.xp));
  Equals(group_fixture.rest, string("#\n#?c"));
  Equals(group_fixture.final_position,
      source_position("", 2, 0, " b", 2, 0));
}

