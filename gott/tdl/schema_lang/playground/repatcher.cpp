// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/structure/print.hpp>
#include <iostream>

struct rep_acc : tdl::structure::writable_structure {
  typedef boost::shared_ptr<tdl::structure::repatcher> repp;

  void begin(tdl::source_position const &) {
    std::cout << "<rep> [\n" << std::flush;
  }

  void end() {
    std::cout << "<rep> ]\n" << std::flush;
  }

  void add_tag(gott::string const &s) {
    std::cout << "<rep> (" << s << ")\n" << std::flush;
  }

  void data(gott::Xany const &x) {
    std::cout << "<rep> $\n" << std::flush;
    rep = gott::Xany_cast<repp>(x);
  }

  repp rep;
};

int main() {
  rep_acc acc_r;
  tdl::structure::revocable_adapter helper(acc_r);
  using namespace tdl::schema;
  match m_r(
      rule_one("tdl::schema::document", rule_attr(tag = "doc"),
        rule("tdl::schema_lang::repatcher", rule_attr(tag = "R"))),
      helper);
  m_r.parse(std::cin);
  if (!acc_r.rep)
    std::cout << "no repatcher" << std::endl;
  else
    std::cout << typeid(*acc_r.rep).name() << std::endl;
  tdl::structure::container out;
  tdl::structure::revocable_adapter helper2(out);
  match m(
      rule_one("tdl::schema::document",
        rule_attr(tag = "doc", repatcher = acc_r.rep),
        rule("tdl::schema::node", rule_attr(tag = "E"))),
      helper2);
  m.parse(std::cin);
  std::cout << out << std::endl;
}
