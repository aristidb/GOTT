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

#include "comfort.hpp"
#include "structure.hpp"
#include <boost/bind.hpp>
#include <gott/range_algo.hpp>
#include <gott/tdl/source_position.hpp>

using gott::xany::Xany;
namespace cf = gott::tdl::structure::cf;
using cf::node_inserter_t;
using gott::tdl::structure::writable_structure;
using boost::bind;
using boost::ref;

node_inserter_t::node_inserter_t(Xany const &d, nd_list const &c, 
                                 tag_list const &t)
: data(d), children(c), tags(t) {}

node_inserter_t::~node_inserter_t() {}

void node_inserter_t::write_to(writable_structure &o) const {
  o.begin(source_position());
    o.data(data);
    for (tag_list::const_iterator it=tags.begin(); it != tags.end(); ++it)
      o.add_tag(*it);
    for_each(range(children), bind(&node_inserter_t::write_to, _1, ref(o)));
  o.end();
}

node_inserter_t cf::S(xany::Xany const &data, tag_list const &tags) {
  return node_inserter_t(data, nd_list(), tags);
}

node_inserter_t cf::S(xany::Xany const &data, string const &tag) {
  return node_inserter_t(data, nd_list(), tag_list(1, tag));
}

node_inserter_t cf::M(nd_list const &children, tag_list const &tags) {
  return node_inserter_t(Xany(), children, tags);
}

node_inserter_t cf::M(nd_list const &children, string const &tag) {
  return node_inserter_t(Xany(), children, tag_list(1, tag));
}

node_inserter_t cf::MD(xany::Xany const &data, nd_list const &children, 
                       tag_list const &tags) {
  return node_inserter_t(data, children, tags);
}

node_inserter_t cf::MD(xany::Xany const &data, nd_list const &children, 
                       string const &tag) {
  return node_inserter_t(data, children, tag_list(1, tag));
}

node_inserter_t cf::C(node_inserter_t const &child, tag_list const &tags) {
  return node_inserter_t(Xany(), nd_list(1, child), tags);
}

node_inserter_t cf::C(node_inserter_t const &child, string const &tag) {
  return node_inserter_t(Xany(), nd_list(1, child), tag_list(1, tag));
}
