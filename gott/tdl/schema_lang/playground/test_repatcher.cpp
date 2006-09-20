// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/repatchable_adapter.hpp>
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/structure/print.hpp>
#include <iostream>

namespace stru = tdl::structure;

struct rep_acc : stru::writable_structure {
  void begin(tdl::source_position const &) {
  }

  void end() {
  }

  void add_tag(gott::string const &) {
  }

  void data(gott::Xany const &x) {
    rep.reset(static_cast<stru::repatcher*>(gott::Xany_cast<void*>(x)));
  }

  boost::shared_ptr<stru::repatcher> rep;
};

int main() {
  rep_acc acc_r;
  stru::repatchable_adapter2 helper3(acc_r);
  stru::revocable_adapter helper2(helper3);
  stru::repatchable_adapter helper(helper2);
  using namespace tdl::schema;
  match m_r(
      rule_one("document", rule_attr(tag = "doc"),
        rule("tdl::schema_lang::repatcher", rule_attr(tag = "R"))),
      helper);
  m_r.parse(std::cin);
  if (!acc_r.rep)
    std::cout << "no repatcher" << std::endl;
  else
    std::cout << typeid(*acc_r.rep).name() << std::endl;
  stru::container out;
  stru::repatchable_adapter2 helper6(out);
  stru::revocable_adapter helper5(helper6);
  stru::repatchable_adapter helper4(helper5);
  match m(
      rule_one("document",
        rule_attr(tag = "doc", repatcher = acc_r.rep),
        rule("node", rule_attr(tag = "E"))),
      helper4);
  m.parse(std::cin);
  std::cout << out << std::endl;
}
