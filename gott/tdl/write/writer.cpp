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

#include "writer.hpp"
#include <ostream>

using tdl::tdl_writer;
using gott::string;

class tdl_writer::impl {
public:
  impl(std::ostream &out, unsigned width)
    : indentation(-width), fresh_line(true), out(out), width(width) {}

  int indentation;
  bool fresh_line;

  std::ostream &out;
  unsigned width;

  void prepare_line() {
    if (!fresh_line) {
      out << '\n';
      fresh_line = true;
    }
    for (int i = 0; i < indentation; ++i)
      out << ' ';
  }
};

tdl_writer::tdl_writer(std::ostream &out, unsigned width) : p(new impl(out, width)) {}

tdl_writer::~tdl_writer() {}

void tdl_writer::down() {
  p->indentation += p->width;
}

void tdl_writer::up() {
  p->indentation -= p->width;
}

void tdl_writer::node(string const &s) {
  p->prepare_line();
  p->out << s;
  p->fresh_line = false;
}

void tdl_writer::comment(string const &s, bool new_line) {
  if (new_line || p->fresh_line)
    p->prepare_line();
  p->out << '#' << s;
  p->fresh_line = false;
}
