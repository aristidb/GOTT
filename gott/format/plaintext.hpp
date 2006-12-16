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
 * The Original Code is Formatting.
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

#ifndef GOTT_FORMAT_PLAINTEXT_HPP
#define GOTT_FORMAT_PLAINTEXT_HPP

#include <gott/hci/object.hpp>
#include <gott/string/qid.hpp>
#include <boost/signal.hpp>
#include <boost/optional.hpp>

namespace gott { namespace format {

/**
 * Baseclass / accessclass for plaintext entities. Usually used from
 * gott::hci::object::domain_specific().
 */
class GOTT_EXPORT plaintext {
public:
  /// The QID of the class.
  static QID const qid;

  /// Render freshly to a string.
  virtual string render() const = 0;

  /// Signal emitted when rendering must be done again.
  virtual boost::signal<void ()> &on_invalidate() const = 0;

  /// Destructor.
  virtual ~plaintext() = 0;
};

/**
 * Renderer for plain text. Takes a gott::hci::object and makes text from it.
 */
class plaintext_renderer {
public:
  /**
   * Constructor.
   * \param obj The object to render.
   */
  GOTT_EXPORT
  plaintext_renderer(gott::hci::object const &obj);

  /// Access the rendered string.
  GOTT_EXPORT string const &get();

private:
  void invalidate();

  plaintext const *obj;
  boost::optional<string> cache;
};

/**
 * Convenience interface to plaintext_renderer if you need to render only once.
 * \param obj The object to render.
 * \return The rendering result.
 */
inline string make_plaintext(gott::hci::object const &obj) {
  plaintext_renderer rend(obj);
  return rend.get();
}

}}

#endif
