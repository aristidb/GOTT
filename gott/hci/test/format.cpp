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

#include <testsoon.hpp>
#include <gott/hci/reference.hpp>
#include <gott/format/plaintext.hpp>
#include <gott/format/text.hpp>
#include <gott/format/tokenized_text.hpp>

using gott::hci::reference;
using namespace gott::format;

TEST_GROUP(text) {

TEST(simple) {
  text t("hallo");
  Equals(make_plaintext(t), "hallo");
}

TEST(tokenized) {
  tokenized_text t(", ");
  t.add(new text("Hallo"));
  t.add(new text("Welt!"));
  Equals(make_plaintext(t), "Hallo, Welt!");
}

TEST(nested tokenized) {
  tokenized_text *inner = new tokenized_text("-");
  inner->add(new text("Super"));
  inner->add(new text("Duper"));
  inner->add(new text("C++"));
  inner->add(new text("Dingsie"));

  tokenized_text outer(" ");
  outer.add(new text("Wer"));
  outer.add(new text("ist"));
  outer.add(new text("das"));
  outer.add(inner);
  outer.add(new text("?"));

  // excuse the space before the question mark, please
  Equals(make_plaintext(outer), "Wer ist das Super-Duper-C++-Dingsie ?");
}

TEST(change) {
  text t("old");
  plaintext_renderer rend(t);
  Equals(rend.get(), "old");
  t.contents().set("new");
  Equals(rend.get(), "new");
}

TEST(inner change) {
  tokenized_text outer(";");
  text *t1 = new text("1");
  outer.add(t1);
  text *t2 = new text("2");
  outer.add(t2);

  plaintext_renderer rend(outer);
  Equals(rend.get(), "1;2");

  t1->contents().set("a");
  Equals(rend.get(), "a;2");
}

TEST(reference) {
  tokenized_text outer("#");
  text inner("x");
  outer.add(new reference(&inner));
  outer.add(new reference(&inner));
  outer.add(new reference(&inner));
  
  plaintext_renderer rend(outer);
  Equals(rend.get(), "x#x#x");

  inner.contents().set("y");
  Equals(rend.get(), "y#y#y");
}

}
