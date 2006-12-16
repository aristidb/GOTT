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

#ifndef GOTT_FORMAT_TOKENIZED_TEXT_HPP
#define GOTT_FORMAT_TOKENIZED_TEXT_HPP

#include "plaintext.hpp"
#include <gott/hci/container.hpp>
#include <boost/optional.hpp>

namespace gott { namespace format {

/**
 * Tokenized text. Contains a number of plaintext entities.
 * Entities are delimited by a delimiter string.
 * Can be rendered with gott::format::plaintext_renderer.
 */
class GOTT_EXPORT tokenized_text
: public gott::hci::container, plaintext {
public:
  /**
   * Constructor.
   * \param delimiter The delimiter string.
   */
  tokenized_text(string const &delimiter);

  /// Destructor.
  ~tokenized_text();

private:
  GOTT_LOCAL void *domain_specific(QID const &);
  GOTT_LOCAL string render() const;
  GOTT_LOCAL boost::signal<void ()> &on_invalidate() const;
  GOTT_LOCAL void before_add(object *);

  string delimiter;
  mutable boost::signal<void ()> invalidate_;

  mutable boost::optional<string> cache;
};

}}

#endif
