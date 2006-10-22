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

#include "../rule.hpp"
#include "../type.hpp"
#include "../match.hpp"
#include "../rule.hpp"
#include "../happy_once.hpp"
#include <gott/string/atom.hpp>
#include <gott/plugin/plugin_builder.hpp>

using namespace tdl::schema;

namespace {
class match_ordered : public happy_once {
public:
  match_ordered(rule_attr_t const &, std::vector<rule_t> const &, match &);
  ~match_ordered();

  static bool accept_empty(rule_attr_t const &, std::vector<rule_t> const &);

  static gott::atom const id;

private:
  static std::vector<rule_t> deflatten(std::vector<rule_t> const &);

  std::vector<rule_t> subrules;
  std::vector<rule_t>::iterator pos;

  bool play(ev::child_succeed const &);
  gott::string name() const;
};
}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(
    plugin_schema_ordered,
    concrete_type<match_ordered>)

gott::atom const match_ordered::id("ordered");

match_ordered::match_ordered(rule_attr_t const &a, std::vector<rule_t> const&r,
                             match  &m)
: happy_once(a, m), subrules(deflatten(r)), pos(subrules.begin()) {
  if (pos != subrules.end())
    matcher().add(*pos);
  else
    be_happy();
}

std::vector<rule_t> match_ordered::deflatten(
    std::vector<rule_t> const &in) {
  std::vector<rule_t> out(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i].attributes().outer().get_mode() == slotcfg::one)
      out[i] = in[i];
    else
      out[i] = rule_one("list", rule_attr(coat = false), in[i]);
  }
  return out;
}

match_ordered::~match_ordered() {}

bool match_ordered::play(ev::child_succeed const &) {
  if (++pos != subrules.end()) 
    matcher().add(*pos);
  else 
    be_happy();
  return true;
}

bool match_ordered::accept_empty(rule_attr_t const &, 
                                 std::vector<rule_t> const &children) {
  bool accept = true;
  for (std::vector<rule_t>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    accept &= it->accept_empty();
  return accept;
}

gott::string match_ordered::name() const {
  return gott::string("ordered");
}
