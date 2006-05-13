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

#include "parser.hpp"
#include "../exceptions.hpp"
#include <boost/algorithm/string.hpp>
#include <gott/string/string.hpp>
#include <gott/string/buffer.hpp>
#include <gott/string/convert.hpp>

using std::istream;
using gott::string;
using gott::range;
using tdl::abstract_tdl_parser;

tdl::source_position const &abstract_tdl_parser::where() const {
  return where_;
}

abstract_tdl_parser::~abstract_tdl_parser() {}

namespace {
struct internal_line_logger {
  tdl::source_position &where;
  unsigned token_line;
  unsigned token_column;
  
  internal_line_logger(tdl::source_position &x) : where(x) {}

  void start_line() { 
    ++where.line;
    where.column = 0;
  }

  void new_char() { 
    ++where.column;
  }

  void add_char(unsigned x) { 
    where.column += x;
  }

  void set_char(unsigned x) { 
    where.column = x;
  }
  
  void start_token() { 
    token_line = where.line;
    token_column = where.column;
  }
  
  void end_token(string const &n) {
    where.token_line = token_line;
    where.token_column = token_column;
    where.token = n;
  }
};
  
class exec_parse {
  istream &stream;
  abstract_tdl_parser &parse;
  unsigned indent;
  bool up, started_document;
  unsigned buff_indent;
  string current_line;

  internal_line_logger ln;

  void normal(unsigned = 0);
  void normal_line(string const &);
  bool empty_line();
  void block();
  
  void get_indent();
  void restore_indent(unsigned, unsigned);
  void skip_whitespace(string::utf8_range &);
  static bool border(char);
  string read_quoted(string::utf8_range &);
  string read_paren(string::utf8_range &);
  string read_string(string::utf8_range &);

  bool read_line();

public:
  exec_parse(istream &s, abstract_tdl_parser &p, tdl::source_position &l) 
    : stream(s), parse(p), up(false), 
      started_document(false), buff_indent(0), ln(l) {}
  void run_parse();
};

class cancellor {};
}

void abstract_tdl_parser::parse(istream &s, source_position const &w) {
  where_ = w;
  exec_parse x(s, *this, where_);
  x.run_parse();
}

void exec_parse::run_parse() {
  parse.begin_parse();

  ln.start_line();
  
  get_indent();
  try {
    normal();
  } catch (cancellor&) {}
  parse.end_parse();
}

bool exec_parse::read_line() {
  return gott::getline(stream, current_line) || current_line.size() > 0;
}

void exec_parse::normal(unsigned last) {
  if (!up) parse.down();

  try {
    do {
      if (read_line())
        if (indent == 1 && current_line == "***") // no space before, no behind
          throw cancellor();
        else
          normal_line(current_line);

      unsigned old = indent;
      do {
        ln.start_line();
        get_indent();
      } while (empty_line());

      if (started_document && indent > old)
        normal(old);
      else if (started_document && up)
        parse.up();

      if (indent <= last)
        break;
    } while (stream);
  } catch (cancellor&) {
    parse.up();
    throw;
  }
  parse.up();
}

void exec_parse::normal_line(string const &s) {
  unsigned count = 0;

  bool to_down = false, empty = true;

  string::utf8_range unread = s.as_utf8();
  
  while (unread.filled()) {
    skip_whitespace(unread);
  
    if (*unread.begin() == '#') {
      ++unread.Begin;
      string y(unread);
      ln.new_char();
      ln.start_token();
      ln.end_token(y);
      parse.comment(y, empty);
      break;
    }

    empty = false;
      
    if (to_down) {
      ++count;
      parse.down();
    }
  
    if (*unread.begin() == '`') {
      started_document = true;
      block();
      break;
    }
    
    started_document = true;
    parse.node(read_string(unread));

    skip_whitespace(unread);
    
    ln.start_token();
    if (unread.filled() && *unread.begin() == ',') {
      ln.new_char();
      ln.end_token(",");
      ++unread.Begin;
      to_down = false;
    } else
      to_down = true;
  }

  for (; count > 1; --count)
    parse.up();

  up = (count == 1);
}

bool exec_parse::empty_line() {
  if (stream.peek() == '#') {
    stream.get();
    read_line();
    parse.comment(current_line, true);
    return true;
  }
  
  if (stream.peek() == '\n') {
    stream.get();
    return true;
  }
  
  return false;
}

void exec_parse::block() {
  unsigned old_indent = indent;

  gott::string_buffer str;
  
  get_indent();

  unsigned ind = 0;
  do {
    if (!read_line())
      break;
    
    ln.start_line();
    
    str += current_line;
    str += "\n";

    ind = 0;
    char c;
    while (stream.get(c) && c == ' ' && ind < indent - 1)
      ++ind;
    stream.putback(c);

    if (ind < indent - 1)
      break;
  } while (stream);

  parse.node(str);

  restore_indent(ind + 1, old_indent);
}

void exec_parse::skip_whitespace(string::utf8_range &unread) {
  while (unread.filled() && *unread.begin() == ' ') {
    ln.new_char();
    ++unread.Begin;
  }
}

bool exec_parse::border(char c) {
  return (c == ' ' || c == ',' || c == '"' || c == '#' || c == '(');
}

string exec_parse::read_quoted(string::utf8_range &unread) {
  string::utf8_range whole = unread;
  bool double_dquote = true;
  do {
    while (unread.filled() && *unread.begin() != '"')
      ++unread.Begin;
    
    gott::utf8_t saved = *unread.Begin++;
    if (!unread.filled()) {
      if (saved != '"')
        throw tdl::tdl_error("TDL parser", "unclosed quote", ln.where);
      break;
    }
    double_dquote = *unread.Begin++ == '"';
  } while (double_dquote);
  unread.Begin -= 1 + !double_dquote; 
    // double_dquote still set if break was executed

  ln.add_char(unread.begin() - whole.begin() + 1);

  ln.end_token(range(whole.begin() - 1, ++unread.Begin));
  
  gott::string_buffer s = string(range(whole.begin(), unread.begin() - 1));
  static const string two_quote("\"\""), one_quote("\"");
  boost::algorithm::replace_all(s, two_quote.as_utf32(), one_quote.as_utf32());

  return s;
}

static const gott::utf32_t newline = gott::to_utf32_char("\n", gott::ascii);

string exec_parse::read_paren(string::utf8_range &unread) {
  string::utf8_range whole = unread;
  struct balancer {
    unsigned long bal;
    balancer() : bal(1) {}
    operator bool() { return bal == 0; }
    void operator() (char c) {
      if (c == '(')
        ++bal;
      if (c == ')')
        --bal;
    }
  } balance;

  ++unread.Begin; // skip (

  for (; unread.filled() && !balance; ++unread.Begin) 
    balance(*unread.begin());

  gott::string_buffer result = string(range(whole.begin(), unread.begin()));
  if (!unread.filled() && !balance) { // multi-line
    result += newline;
    char c;
    while (!balance && stream.get(c)) {
      result += gott::to_utf32_char(&c, gott::utf8);
      if (c == '\n')
        ln.start_line();
      balance(c);
    }
  }
  if (!balance)
    throw tdl::tdl_error("TDL parser", "unbalanced parentheses", ln.where);
  ln.end_token(result);
  return result;
}

string exec_parse::read_string(string::utf8_range &unread) {
  ln.start_token();
  
  if (*unread.begin() == '"') {
    ++unread.Begin;
    return read_quoted(unread);
  } else if (*unread.begin() == '(')
    return read_paren(unread);

  string::utf8_range::value_type start = unread.begin();
  while (unread.filled() && !border(*unread.begin()))
    ++unread.Begin;
  
  ln.add_char(unread.begin() - start);

  string out(range(start, unread.begin()));
  ln.end_token(out);
  return out;
}

void exec_parse::get_indent() {
  if (buff_indent) {
    indent = buff_indent;
    buff_indent = 0;

    ln.set_char(indent - 1);
    return;
  }
  char c;
  unsigned x = 1;
  while (stream.get(c) && c == ' ')
    ++x;
  if (stream) stream.putback(c);
  indent = x;
  ln.set_char(indent - 1);
}

void exec_parse::restore_indent(unsigned ind, unsigned old) {
  buff_indent = ind;
  indent = old;
}
