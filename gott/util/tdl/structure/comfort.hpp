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

#ifndef GOTT_UTIL_TDL_STRUCTURE_COMFORT_HPP
#define GOTT_UTIL_TDL_STRUCTURE_COMFORT_HPP

#include <gott/util/misc/commonheaders.hpp>

namespace gott {
namespace util {
namespace tdl {
namespace structure {
class writable_structure;

/**
 * More comfortable structure generators.
 */
namespace cf {

typedef std::list<std::wstring> tag_list;
class node_inserter_t;
typedef std::list<node_inserter_t> nd_list;

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

  node_inserter_t(xany::Xany const &d, nd_list const &c, tag_list const &t);
  GOTT_EXPORT ~node_inserter_t();

private:
  xany::Xany data;
  nd_list children;
  tag_list tags;
};

/**
 * Create a leaf node (generator).
 * \param data The node's data.
 * \param tags The nodes's tags.
 */
GOTT_EXPORT node_inserter_t S(xany::Xany data, tag_list tags = tag_list());

/**
 * Create a leaf node (generator).
 * \param data The node's data.
 * \param tag The nodes's sole tag.
 */
GOTT_EXPORT node_inserter_t S(xany::Xany data, std::wstring tag);

/**
 * Create a node (generator) without data but with children.
 * \param children The node's children in spe.
 * \param tags The node's tags.
 */
GOTT_EXPORT node_inserter_t M(nd_list children, tag_list tags = tag_list());

/**
 * Create a node (generator) without data but with children.
 * \param children The node's children in spe.
 * \param tag The node's sole tag.
 */
GOTT_EXPORT node_inserter_t M(nd_list children, std::wstring tag);

/**
 * Create a node (generator) with data and children.
 * \param data The node's data.
 * \param children The node's children in spe.
 * \param tags The node's tags.
 */
GOTT_EXPORT node_inserter_t MD(xany::Xany data, nd_list children,
                   tag_list tags = tag_list());

/**
 * Create a node (generator) with data and children.
 * \param data The node's data.
 * \param children The node's children in spe.
 * \param tag The node's sole tag.
 */
GOTT_EXPORT node_inserter_t MD(xany::Xany data, nd_list children, std::wstring tag);

/**
 * Create a node (generator) without data and with a single child.
 * \param child The node's child in spe.
 * \param tags The node's tags.
 */
GOTT_EXPORT node_inserter_t C(node_inserter_t child, tag_list tags = tag_list());

/**
 * Create a node (generator) without data and with a single child.
 * \param child The node's child in spe.
 * \param tag The node's sole tag.
 */
GOTT_EXPORT node_inserter_t C(node_inserter_t child, std::wstring tag);

}}}}}

#endif
