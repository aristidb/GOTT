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
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#include "../repatch.hpp"
#include <gott/exceptions.hpp>
#include <gott/tdl/exceptions.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/plugin/plugin_builder.hpp>
#include <gott/string/buffer.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <boost/assign/list_of.hpp>

using namespace tdl::structure;
using tdl::tdl_error;
using tdl::source_position;
using gott::Xany;
using gott::Xany_cast;
using gott::string;

namespace {

class repatch_substring 
: public concrete_repatcher<repatch_substring> {
public:
  repatch_substring(long left, long right);
  ~repatch_substring();
  writable_structure *deferred_write(writable_structure &) const;
  writable_structure *inverse(writable_structure &) const;
private:
  long left, right;
};

}

repatch_substring::repatch_substring(long l, long r) : left(l), right(r) {
  if (left < 0)
    throw tdl_error("TDL Structure repatcher loader",
        "substring must begin at non-negative position");
}
repatch_substring::~repatch_substring() {}

writable_structure *
repatch_substring::deferred_write(writable_structure &target) const {
  struct context : simple_repatcher_context {
    long left, right;
    context(writable_structure &target, long l, long r) 
      : simple_repatcher_context(target), left(l), right(r) {}
    void data(Xany const &x) {
      if (!x.compatible<string>())
        throw tdl_error("TDL Structure repatcher",
            "substring needs input string");
      string s = Xany_cast<string>(x);
      long len = s.length();
      if (left > len || right > len || right < -len)
        throw tdl_error("TDL Structure repatcher",
            "substring range out of input bounds");
      string::utf32_range in = s.as_utf32(), out;
      if (right > 0) 
        out = offset(simply(in.begin()), left, right);
      else
        out = offset(in, left, right);
      if (out.end() < out.begin())
        throw tdl_error("TDL Structure repatcher",
            "substring range out of input bounds");
      target.data(Xany(string(out)));
    }
  };
  return new context(target, left, right);
}

writable_structure *
repatch_substring::inverse(writable_structure &target) const {
  struct context : simple_repatcher_context {
    string pre, post;
    context(writable_structure &target, string const &pre, string const &post)
      : simple_repatcher_context(target), pre(pre), post(post) {}
    void data(Xany const &x) {
      if (!x.compatible<string>())
        throw tdl_error("TDL Structure repatcher",
            "substring (reverse) needs input string");
      target.data(Xany(pre + Xany_cast<string>(x) + post));
    }
  };
  string pre, post;
  if (left > 0) {
    gott::string_buffer helper;
    for (long i = 0; i < left; ++i)
      helper += "?";
    pre = helper;
  }
  if (right < 0) {
    gott::string_buffer helper;
    for (long i = 0; i < -right; ++i)
      helper += "?";
    post = helper;
  }
  return new context(target, pre, post);
}

namespace {
class factory : public repatcher_getter_factory {
  struct getter : public repatcher_getter {
    getter() : pos(outer) {}
    enum { outer, inner, p_left, p_right } pos;
    long left, right;
    boost::optional<long> delayed;
    void begin(source_position const &) {
      if (pos != outer) fail();
      pos = inner;
    }
    void end() {
      if (pos == outer || pos == inner)
        fail();
      pos = outer;
    }
    void data(Xany const &x) {
      if (delayed)
        fail();
      switch (pos) {
      case p_left:
        left = Xany_cast<long>(x);
        break;
      case p_right:
        right = Xany_cast<long>(x);
        break;
      case inner:
        delayed = Xany_cast<long>(x);
        break;
      case outer:
        fail();
      }
    }
    void add_tag(string const &s) {
      if (pos != inner)
        return;
      if (s == "left")
        pos = p_left;
      else if (s == "right")
        pos = p_right;
      else
        return;
      if (delayed) {
        if (pos == p_left)
          left = *delayed;
        else if (pos == p_right)
          right = *delayed;
        delayed = boost::none;
      }
    }
    void fail() {
      throw tdl_error("TDL Structure repatcher loader",
          "non-sensible arguments");
    }
    repatcher *result_alloc() const {
      return new repatch_substring(left, right);
    }
  };
public:
  factory() {}
  gott::atom get_id() const { return gott::atom("substring"); }
  repatcher_getter *alloc() const { return new getter; }
  tdl::schema::rule_t parameter_schema() const {
    boost::shared_ptr<repatcher const> r;
    {
      boost::scoped_ptr<repatcher_getter> g(repatcher_by_name());
      g->begin();
        g->data(gott::Xany("integer"));
        g->begin(); g->end();
      g->end();
      r.reset(g->result_alloc());
    }
    using namespace tdl::schema;
    return rule("ordered", rule_attr(),
        boost::assign::list_of
        (rule("node", 
              rule_attr(tag = "left", tdl::schema::repatcher = r)))
        (rule("node",
              rule_attr(tag = "right", tdl::schema::repatcher = r))));
  }
};
}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(
    plugin_repatcher_substring,
    factory)
