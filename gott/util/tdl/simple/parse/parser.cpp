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

using std::wistream;
using std::wstring;
using gott::string;
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
  wstring current_line;

  internal_line_logger ln;

  void normal(unsigned = 0);
  void normal_line(wstring const &);
  bool empty_line();
  void block();
  
  void get_indent();
  void restore_indent(unsigned);
  void skip_whitespace(wstring::const_iterator &, 
                       wstring::const_iterator const &);
  static bool border(wchar_t);
  wstring read_quoted(wstring::const_iterator &, 
                      wstring::const_iterator const &); 
  wstring read_paren(wstring::const_iterator &, 
                     wstring::const_iterator const &);
  wstring read_string(wstring::const_iterator &, 
                      wstring::const_iterator const &);

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

void exec_parse::normal(unsigned last) {
  if (!up) parse.down();

  try {
    do {
      if (getline(stream, current_line))
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

void exec_parse::normal_line(wstring const &s) {
  unsigned count = 0;

  bool to_down = false, empty = true;

  wstring::const_iterator pos = s.begin();
  
  while (pos != s.end()) {
    skip_whitespace(pos, s.end());
  
    if (*pos == L'#') {
      string y(wstring(++pos, s.end()));
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
  
    if (*pos == L'`') {
      block();
      break;
    }
    
    parse.node(read_string(pos, s.end()));
    started_document = true;

    skip_whitespace(pos, s.end());
    
    ln.start_token();
    if (pos != s.end() && *pos == L',') {
      ln.new_char();
      ln.end_token(L",");
      ++pos;
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
    wstring s;
    getline(stream, s);
    parse.comment(s, true);
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

  wstring str;
  
  get_indent();

  do {
    wstring tmp;
    if (!getline(stream, tmp))
      break;
    
    ln.start_line();
    
    str.reserve(str.size() + tmp.size() + 1);
    str += tmp;
    str += '\n';

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

void exec_parse::skip_whitespace(wstring::const_iterator &it, 
                                     wstring::const_iterator const &end) {
  while (it != end && *it == L' ') {
    ln.new_char();
    ++it;
  }
}

bool exec_parse::border(wchar_t c) {
  return (c == L' ' || c == L',' || c == L'"' || c == L'#' || c == L'(');
}

wstring exec_parse::read_quoted(wstring::const_iterator &it, 
                                    wstring::const_iterator const &end) {
  wstring::const_iterator start = it;
  bool double_dquote = true;
  do {
    while (it != end && *it != L'"')
      ++it;
    if (++it == end)
      break;
    double_dquote = *it++ == L'"';
  } while (double_dquote);
  it -= 1 + !double_dquote; // double_dquote still set if break was executed

  ln.add_char(it - start + 1);

  ln.end_token(wstring(start - 1, ++it));
  
  wstring s(start, it - 1);
  boost::algorithm::replace_all(s, L"\"\"", L"\"");
 
  return s;
}

wstring exec_parse::read_paren(wstring::const_iterator &it, 
                                   wstring::const_iterator const &end) {
  wstring::const_iterator start = it++;
  struct balancer {
    unsigned long bal;
    balancer() : bal(1) {}
    operator bool() { return bal == 0; }
    void operator() (wchar_t c) {
      if (c == L'(')
        ++bal;
      if (c == L')')
        --bal;
    }
  } balance;

  for (; it != end && !balance; ++it) 
    balance(*it);

  wstring result(start, it);
  if (it == end && !balance) { // multi-line
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

wstring exec_parse::read_string(wstring::const_iterator &it, 
                                    wstring::const_iterator const &end) {
  ln.start_token();
  
  if (*it == L'"')
    return read_quoted(++it, end);
  else if (*it == L'(')
    return read_paren(it, end);

  wstring::const_iterator start = it;
  while (it != end && !border(*it))
    ++it;
  
  ln.add_char(it - start);

  wstring out(start, it);
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
