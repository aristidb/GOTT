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
 * The Original Code is A Human Computer Interaction Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#ifndef GOTT_HCI_OBJECT_HPP
#define GOTT_HCI_OBJECT_HPP

#include <gott/string/string.hpp>
#include <gott/string/qid.hpp>
#include <boost/function.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <vector>

namespace gott { namespace hci {

/**
 * An HCI object.
 */
class GOTT_EXPORT object {
public:
  /// Path element type.
  typedef unsigned short path_element;
  /// Path type for sub-objects.
  typedef std::vector<path_element> path_type;
  /// Size type sufficient for path depths.
  typedef path_type::size_type size_type;
  /// Special value for specifying lack (or infinite) of size value.
  static const size_type npos = size_type(-1);

public:
  /// Constructor.
  object();

  /// Virtual destructor.
  virtual ~object();

  /**
   * Get a pointer to a domain-specific representation for this object or 0 if
   * not applicable.
   * \return A pointer to the representation.
   */
  virtual void *domain_specific(QID const &domain);

  /**
   * Get a pointer to a domain-specific representation for this object or 0 if
   * not applicable.
   * \return A pointer to the representation.
   */
  template<class Domain>
  GOTT_LOCAL Domain *domain_specific() {
    return static_cast<Domain *>(domain_specific(Domain::qid));
  }

  /**
   * Get a pointer to a domain-specific representation for this object or 0 if
   * not applicable.
   * \return A pointer to the representation.
   */
  template<class Domain>
  GOTT_LOCAL Domain const *domain_specific() const {
    return const_cast<object *>(this)->domain_specific<Domain>();
  }

  /**
   * Find an object with specified path.
   * \param path The path to find.
   * \param offset The number of path elements to ignore.
   * \return The found object or 0.
   */
  virtual object *find(path_type const &path, size_type offset = 0);

  /**
   * Find an object with specified path.
   * \param path The path to find.
   * \return The found object or 0.
   */
  GOTT_LOCAL inline 
  object const *find(path_type const &path, size_type offset = 0) const {
    return const_cast<object *>(this)->find(path, offset);
  }

  /**
   * Find an object with a name.
   * \param name The name of the object to find.
   * \return The found object or 0.
   */
  virtual object *find_named(string const &name);

  /**
   * Find an object with a name.
   * \param name The name of the object to find.
   * \return The found object or 0.
   */
  GOTT_LOCAL inline object const *find_named(string const &name) const {
    return const_cast<object *>(this)->find_named(name);
  }

  /**
   * Get number of first child.
   * \param current Store the number of the first child.
   * \return Whether there is a first child at all.
   */
  virtual bool first_child(path_element &current) const;

  /**
   * Get number of next child.
   * \param current Number of child, will be changed.
   * \return Whether there is a next child at all.
   */
  virtual bool next_child(path_element &current) const;

public:
  /**
   * A depth first iterator with stored path in tree and optional depth limit.
   */
  template<class Type>
  class GOTT_LOCAL basic_df_iterator
    : public boost::iterator_facade <
        basic_df_iterator<Type>,
        Type,
        boost::forward_traversal_tag
      > {
  private:
    friend class object;
    basic_df_iterator(Type *root)
      : root(root), current(0) {}
    basic_df_iterator(Type *root, size_type max_depth)
      : root(root), current(root), max_depth(max_depth) {}

    template<class Other>
    basic_df_iterator(basic_df_iterator<Other> const &o)
      : root(o.root), current(o.current), path(o.path), max_depth(o.max_depth)
    {}

  public:
    /**
     * Get the path of the current element.
     */
    path_type const &get_path() const {
      return path;
    }

    /**
     * Skip all children and go to the next sibling.
     */
    void next_sibling() {
      path_element no;
      do {
        if (path.empty()) {
          current = 0;
          return;
        }
        no = path.back();
        path.pop_back();
        current = root->find(path);
      } while (!current->next_child(no));
      go(no);
    }

  private:
    friend class boost::iterator_core_access;

    bool equal(basic_df_iterator const &other) const {
      return root == other.root && current == other.current &&
        path == other.path;
    }

    void increment() {
      if (path.size() >= max_depth)
        return next_sibling();
      path_element no;
      if (current->first_child(no))
        go(no);
      else
        next_sibling();
    }

    void go(path_element no) {
      path.push_back(no);
      current = current->find(path, path.size() - 1);
    }

    Type &dereference() const {
      return *current;
    }

  private:
    Type *root;
    Type *current;
    path_type path;
    size_type max_depth;
  };

public:
  /// Depth-first iterator.
  typedef basic_df_iterator<object> df_iterator;
  /// Constant depth-first iterator.
  typedef basic_df_iterator<object const> const_df_iterator;

  /**
   * Get the depth-first "begin" iterator.
   * \param max_depth Depth limit.
   */
  GOTT_LOCAL
  df_iterator depth_first_begin(size_type max_depth = npos) {
    return df_iterator(this, max_depth);
  }

  /**
   * Get the depth-first "end" iterator.
   */
  GOTT_LOCAL
  df_iterator depth_first_end() {
    return df_iterator(this);
  }

  GOTT_LOCAL
  const_df_iterator depth_first_begin(size_type max_depth = npos) const {
    return const_df_iterator(this, max_depth);
  }

  GOTT_LOCAL
  const_df_iterator depth_first_end() const {
    return const_df_iterator(this);
  }
};

}}

#endif
