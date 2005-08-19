// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Simple SAX-like TDL parser
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

#include "parser.hpp"
#include "line_logger.hpp"
#include <boost/algorithm/string.hpp>
#include <gott/util/string/string.hpp>
#include <gott/util/string/buffer.hpp>

using std::wistream;
using gott::string;
using gott::range;
using gott::tdl::simple::parser;
using gott::tdl::simple::line_logger;

parser::~parser() {}

namespace {
struct internal_line_logger {
  line_logger *ref;
  unsigned p;
  unsigned rp;
  
  internal_line_logger(line_logger *x) : ref(x) {}

  void start_line() { 
    p = 0; 
    if (ref)
      ref->start_line(); 
  }

  void new_char() { 
    ++p; 
    if (ref)
      ref->line_position(p); 
  }

  void add_char(unsigned x) { 
    p += x; 
    if (ref)
      ref->line_position(p); 
  }

  void set_char(unsigned x) { 
    p = x; 
    if (ref)
      ref->line_position(p); 
  }
  
  void start_token() { rp = p; }
  
  void end_token(string const &n) {
    if (ref)
      ref->token(rp, p, n);
  }
};
  
class exec_parse {
  wistream &stream;
  parser &parse;
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
  void restore_indent(unsigned);
  void skip_whitespace(string::utf8_range &);
  static bool border(char);
  string read_quoted(string::utf8_range &);
  string read_paren(string::utf8_range &);
  string read_string(string::utf8_range &);

  bool read_line();

public:
  exec_parse(wistream &s, parser &p, line_logger *l) 
    : stream(s), parse(p), up(false), 
      started_document(false), buff_indent(0), ln(l) {}
  void run_parse();
};

class cancellor {};
}

line_logger::~line_logger() {}

void parser::parse(std::wistream &s) {
  exec_parse x(s, *this, ll);
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
  std::wstring line;
  if (!getline(stream, line))
    return false;
  current_line = line;
  return true;
}

void exec_parse::normal(unsigned last) {
  if (!up) parse.down();

  try {
    do {
      if (read_line())
        if (indent == 1 && current_line == L"***") // no space before, no behind
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
  
  while (!unread.empty()) {
    skip_whitespace(unread);
  
    if (*unread.begin == '#') {
      ++unread.begin;
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
  
    if (*unread.begin == '`') {
      block();
      break;
    }
    
    parse.node(read_string(unread));
    started_document = true;

    skip_whitespace(unread);
    
    ln.start_token();
    if (!unread.empty() && *unread.begin == ',') {
      ln.new_char();
      ln.end_token(L",");
      ++unread.begin;
      to_down = false;
    } else
      to_down = true;
  }

  for (; count > 1; --count)
    parse.up();

  up = (count == 1);
}

bool exec_parse::empty_line() {
  if (stream.peek() == L'#') {
    stream.get();
    read_line();
    parse.comment(current_line, true);
    return true;
  }
  
  if (stream.peek() == L'\n') {
    stream.get();
    return true;
  }
  
  return false;
}

void exec_parse::block() {
  unsigned old_indent = indent;

  gott::string_buffer str;
  
  get_indent();

  do {
    if (!read_line())
      break;
    
    ln.start_line();
    
    str += current_line;
    str += "\n";

    unsigned ind = 0;
    wchar_t c;
    while (stream.get(c) && c == L' ' && ind < indent - 1)
      ++ind;
    stream.putback(c);

    if (ind < indent - 1)
      break;
  } while (stream);

  parse.node(str);

  restore_indent(old_indent);
}

void exec_parse::skip_whitespace(string::utf8_range &unread) {
  while (!unread.empty() && *unread.begin == ' ') {
    ln.new_char();
    ++unread.begin;
  }
}

bool exec_parse::border(char c) {
  return (c == ' ' || c == ',' || c == '"' || c == '#' || c == '(');
}

string exec_parse::read_quoted(string::utf8_range &unread) {
  string::utf8_range whole = unread;
  bool double_dquote = true;
  do {
    while (!unread.empty() && *unread.begin != '"')
      ++unread.begin;
    if (++unread.begin >= unread.end)
      break;
    double_dquote = *unread.begin++ == L'"';
  } while (double_dquote);
  unread.begin -= 1 + !double_dquote; // double_dquote still set if break was executed

  ln.add_char(unread.begin - whole.begin + 1);

  ln.end_token(range(whole.begin - 1, ++unread.begin));
  
  gott::string_buffer s(string(range(whole.begin, unread.begin - 1)));
  boost::algorithm::replace_all(s, string("\"\"").as_utf32(), string("\"").as_utf32());
 
  return s;
}

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

  ++unread.begin; // skip (

  for (; !unread.empty() && !balance; ++unread.begin) 
    balance(*unread.begin);

  gott::string_buffer result(string(range(whole.begin, unread.begin)));
  if (unread.empty() && !balance) { // multi-line
    result += L'\n';
    wchar_t c;
    while (!balance && stream.get(c)) {
      result += c;
      if (c == L'\n')
        ln.start_line();
      balance(c);
    }
  }
  ln.end_token(result);
  return result;
}

string exec_parse::read_string(string::utf8_range &unread) {
  ln.start_token();
  
  if (*unread.begin == '"') {
    ++unread.begin;
    return read_quoted(unread);
  } else if (*unread.begin == '(')
    return read_paren(unread);

  string::utf8_range::value_type start = unread.begin;
  while (!unread.empty() && !border(*unread.begin))
    ++unread.begin;
  
  ln.add_char(unread.begin - start);

  string out(range(start, unread.begin));
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
  
  wchar_t c;
  unsigned x = 1;
  while (stream.get(c) && c == L' ')
    ++x;
  if (stream) stream.putback(c);
  indent = x;
  
  ln.set_char(indent - 1);
}

void exec_parse::restore_indent(unsigned x) {
  buff_indent = indent;
  indent = x;
}
