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

#ifndef GOTT_UTIL_TDL_STRUCTURE_COMFORT_HPP
#define GOTT_UTIL_TDL_STRUCTURE_COMFORT_HPP

#include <gott/xany.hpp>
#include <gott/visibility.hpp>
#include <vector>

namespace gott {
class gott::string;
}

namespace tdl {
namespace structure {

class writable_structure;

/**
 * More comfortable structure generators.
 */
namespace cf {

typedef std::vector<gott::string> tag_list;
class node_inserter_t;
typedef std::vector<node_inserter_t> nd_list;

/**
 * General concatenateable structure generator.
 */
class node_inserter_t {
public:
  /**
   * Actually writes the structure in spe.
   * \param ws The structure to write to.
   */
  GOTT_EXPORT void write_to(writable_structure &ws) const;

  node_inserter_t(gott::xany::Xany const &d, nd_list const &c, tag_list const &t);
  GOTT_EXPORT ~node_inserter_t();

private:
  gott::xany::Xany data;
  nd_list children;
  tag_list tags;
};

/**
 * Create a leaf node (generator).
 * \param data The node's data.
 * \param tags The nodes's tags.
 */
GOTT_EXPORT 
node_inserter_t S(gott::xany::Xany const &data, tag_list const &tags = tag_list());

/**
 * Create a leaf node (generator).
 * \param data The node's data.
 * \param tag The nodes's sole tag.
 */
GOTT_EXPORT 
node_inserter_t S(gott::xany::Xany const &data, gott::string const &tag);

/**
 * Create a node (generator) without data but with children.
 * \param children The node's children in spe.
 * \param tags The node's tags.
 */
GOTT_EXPORT 
node_inserter_t M(nd_list const &children, tag_list const &tags = tag_list());

/**
 * Create a node (generator) without data but with children.
 * \param children The node's children in spe.
 * \param tag The node's sole tag.
 */
GOTT_EXPORT 
node_inserter_t M(nd_list const &children, gott::string const &tag);

/**
 * Create a node (generator) with data and children.
 * \param data The node's data.
 * \param children The node's children in spe.
 * \param tags The node's tags.
 */
GOTT_EXPORT 
node_inserter_t MD(gott::xany::Xany const &data, nd_list const &children,
                   tag_list const &tags = tag_list());

/**
 * Create a node (generator) with data and children.
 * \param data The node's data.
 * \param children The node's children in spe.
 * \param tag The node's sole tag.
 */
GOTT_EXPORT 
node_inserter_t MD(gott::xany::Xany const &data, nd_list const &children, 
                   gott::string const &tag);

/**
 * Create a node (generator) without data and with a single child.
 * \param child The node's child in spe.
 * \param tags The node's tags.
 */
GOTT_EXPORT 
node_inserter_t C(node_inserter_t const &child, 
                  tag_list const &tags = tag_list());

/**
 * Create a node (generator) without data and with a single child.
 * \param child The node's child in spe.
 * \param tag The node's sole tag.
 */
GOTT_EXPORT 
node_inserter_t C(node_inserter_t const &child, gott::string const &tag);

}}}

#endif
