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

#ifndef GOTT_HCI_STEREOTYPE_HPP
#define GOTT_HCI_STEREOTYPE_HPP

#include "object.hpp"

namespace gott { namespace hci {

/**
 * A stereotype is an "incomplete object". It has to be bound to a %container
 * object.
 * \see gott::hci::bound_stereotype
 */
class GOTT_EXPORT stereotype {
public:
  /// Pure virtual destructor.
  virtual ~stereotype() = 0;

protected:
  friend class bound_stereotype;

  /**
   * Load a context for domain-specific objects.
   * \param container The %container object.
   * \return The context.
   */
  virtual void *load_context(object const *container) const = 0;

  /**
   * Unload a context for domain-specific objects.
   * \param context The context.
   */
  virtual void unload_context(void *context) const = 0;

  /**
   * Fetch a domain-specific object from a context.
   * \param context The context.
   * \param id The QID of the domain.
   */
  virtual void *fetch_domain(void *context, QID const &id) const = 0;
};

/**
 * A stereotype bound to a %container object.
 * \see gott::hci::stereotype
 * \see gott::hci::object
 */
class GOTT_EXPORT bound_stereotype : public object {
public:
  /**
   * Constructor.
   * \param unbound The unbound stereotype.
   * \param container The %container object.
   */
  bound_stereotype(stereotype const *unbound, object const *container);

  /// Destructor.
  ~bound_stereotype();

private:
  void *domain_specific(QID const &domain);

private:
  stereotype const *unbound;
  object const *container;
  void *context;
};

}}

#endif
