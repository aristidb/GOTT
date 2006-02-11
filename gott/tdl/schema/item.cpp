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

#include "item.hpp"
#include "rule_attr.hpp"
#include "match.hpp"
#include "event.hpp"
#include <gott/range_algo.hpp>
#include <boost/bind.hpp>
#include <gott/string/stl.hpp>

using gott::string;
namespace schema = gott::tdl::schema;
using schema::match;
using schema::item;
using schema::rule_attr_t;
namespace ev = schema::ev;
using gott::tdl::structure::writable_structure;

class item::impl {
public:
  match *cont;
  rule_attr_t attrib;

  impl(match &m, rule_attr_t const &a, source_position const &w) 
  : cont(&m), attrib(a) {
    start_structure(w);
    add_tags();
  }

  void end_structure() {
    if (attrib.coat())
      cont->direct_structure().end();
  }

private:
  void add_tags() {
    for (std::vector<string>::const_iterator it = attrib.tags().begin();
         it != attrib.tags().end(); ++it)
      cont->direct_structure().add_tag(*it);
  }

  void start_structure(source_position const &w) {
    if (attrib.coat())
      cont->direct_structure().begin(w);
  }
};

item::item(rule_attr_t const &a, match &m) 
: pimpl(new impl(m, a, m.where_out())) {
}

void item::finish() {
  pimpl->end_structure();
}

item::~item() {}

rule_attr_t const &item::attributes() const { return pimpl->attrib; }
match &item::matcher() { return *pimpl->cont; }

bool item::play(ev::begin_parse const &) { return play_other(); }
bool item::play(ev::down const &) { return play_other(); }
bool item::play(ev::up const &) { return play_other(); }
bool item::play(ev::end_parse const &) { return play_other(); }
bool item::play(ev::node const &) { return play_other(); }

bool item::play(ev::child_succeed const &) { return play_other(); }
bool item::play(ev::child_fail const &) { return play_other(); }

bool item::play_other() { return false; }

bool item::miss_events(ev::event const &, unsigned) { return false; }

string item::long_name() const {
  static string const s_open("(");
  static string const s_close(")");
  static string const sep(",");

  std::vector<string> out;
  std::vector<string> const &tags = attributes().tags();
  out.reserve(1 + 
      (tags.size() > 0 
       ? 1 + tags.size() * 2
       : 0));
  out.push_back(name());
  if (tags.size() > 0) {
    out.push_back(s_open);
    range_t<std::vector<string>::const_iterator> r = range(tags);
    out.push_back(*r.begin());
    for (std::vector<string>::const_iterator it = r.begin() + 1; 
        it != r.end(); ++it) {
      out.push_back(sep);
      out.push_back(*it);
    }
    out.push_back(s_close);
  }
  return string(range(out), string::concatenate);
}

std::ostream &schema::operator<<(std::ostream &s, rule_attr_t const &a) {
  s << '(' << (a.coat() ? "coat" : "flat");
  s << ",tags:" << range(a.tags());
  if (dynamic_cast<xany::printable const *>(&a.user().get_operations()))
    s << ",user:" << a.user();
  s << ",repatcher:" << a.repatcher() << ')';
  return s;
}
