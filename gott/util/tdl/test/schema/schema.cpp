// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TD Testing
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU esser General Public
// icense as published by the Free Software Foundation; either
// version 2.1 of the icense, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABIITY or FITNESS FOR A PARTICUAR PURPOSE.  See the GNU
// esser General Public icense for more details.
//
// You should have received a copy of the GNU esser General Public
// icense along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "common.hpp"
#include <gott/util/tdl/schema/context_template.hpp>
#include <gott/util/tdl/schema/slot.hpp>
#include <gott/util/tdl/schema/types/named.hpp>
#include <gott/util/tdl/structure/types/enumeration.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;

using namespace stru::cf;
using schema::slotcfg;
using schema::rule_attr;

using schema::match_named;

rule_attr ra(char const *t) { 
  return rule_attr(t); 
}

const int TYPE = 0;
const int PARAM = 1;

namespace {

struct Schema : tut::schema_basic {
  schema::context rec, param;
  
  Schema() {
    schema::context_template document, type, p;

    document.begin("document");
      document.begin("ordered", ra("tdl-schema"));
        module_id(document);
      
        type_declarations(document);
      document.end();
    document.end();

    qualified(type);

    parameters(p);

    Vector<schema::context*> cc;
    cc.Add(&rec);
    cc.Add(&param);
    p.instantiate(cc, param);
    type.instantiate(cc, rec);
    document.instantiate(cc, context);
  }

  void module_id(schema::context_template &document) {
    document.begin("named", match_named::attributes("module"));
      document.begin("follow", rule_attr(rule_attr::simple, false));
        document.begin("node", ra("module-id"));
        document.end();

        document.begin("node", ra("version-spec"), 
                       slotcfg(slotcfg::range, 1, 2));
        document.end();
      document.end();
    document.end();
  }

  void type_declarations(schema::context_template &document) {
    document.begin("list", ra("type-declarations"));
      document.begin("ordered", ra("type-declaration"));
        document.begin("node", rule_attr("export", true,
             new stru::repatch_enumeration(Vector<string>() | "export")),
             slotcfg(slotcfg::optional));
        document.end();

        document.begin("named", match_named::attributes("type", false));
          document.begin("ordered", rule_attr(rule_attr::simple, false));
            document.begin("follow", rule_attr("T1", false, 0));
              document.begin("node", ra("name"));
              document.end();

              document.begin("list", rule_attr(rule_attr::simple, false));
                document.begin("node", ra("parameter")); 
                document.end();
              document.end();
            document.end();

            document.param(TYPE);
          document.end();
        document.end();
      document.end();
    document.end();
  }

  void qualified(schema::context_template &document) {
     document.begin("follow", ra("qualified-type-definition"));
       {
         Vector<string> choice;
         choice.Add("enclosed"); choice.Add("flat");
         document.begin("node", rule_attr("coat", true,
                                        new stru::repatch_enumeration(choice)),
                        slotcfg(slotcfg::optional));
         document.end();
       }

       document.begin("follow", 
                      rule_attr(rule_attr::simple, false));
         document.begin("named", 
                        match_named::attributes(":"),
                        slotcfg(slotcfg::optional));
           document.begin("node", ra("tag")); 
           document.end();
         document.end();

         document.begin("follow", ra("type-definition"));
           slot(document);
           normal_type(document);
         document.end();
       document.end();
     document.end();
  }

  void normal_type(schema::context_template &document) {
    document.begin("follow", ra("normal-type"));
      document.begin("node", ra("type"));
      document.end();

      document.begin("ordered", ra("parameters"));
        document.param(PARAM);
        document.begin("ordered", rule_attr(rule_attr::simple, false),
                       slotcfg(slotcfg::list));
          document.param(TYPE);
        document.end();
      document.end();
    document.end();
  }

  void parameters(schema::context_template &document) {
    document.begin("follow", rule_attr(rule_attr::simple, false));
      document.begin("named", match_named::attributes("@"), 
                     slotcfg(slotcfg::list));
        document.begin("node", rule_attr(rule_attr::simple, false));
        document.end();
      document.end();
    document.end();
  }

  void slot(schema::context_template &document) {
    document.begin("any", ra("slot-specification"), slotcfg(slotcfg::optional));
      {
      Vector<string> single;
      single.Add("one");  single.Add("optional");
      single.Add("list"); single.Add("some");
      document.begin("node", rule_attr("slot", true, 
                                       new stru::repatch_enumeration(single)));
      document.end();
      }

    // ...
    document.end();
  }
};
}

namespace tut {
typedef test_group<Schema> tf;
typedef tf::object object;
}

#define ACTIVATE_SCHEMA_TEST

namespace {
#ifdef ACTIVATE_SCHEMA_TEST
  tut::tf Tschema("schema::$schema");
#endif
}

//#define IGNORE_ERRORS

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test(
      L"module anything 0.0\n"
       "export\n"
       "type\n"
       "  anything\n"
       "  optional\n"
       "    follow\n"
       "      string\n"
       "      anything\n"
  );
  C(
    M(
      nd_list() <<
      MD(Xany(0),
        nd_list() << S(Xany("anything"), "module-id")
          << S(Xany("0.0"), "version-spec")
        , "module") <<
        C(
          MD(Xany(0),
            nd_list() <<
            S(Xany(0), "export") <<
            S(Xany("anything"), "name") <<
            C(
              M(
                nd_list() <<
                C(
                  S(Xany(1), "slot"),
                "slot-specification") <<
                M(
                  nd_list() <<
                  S(Xany("follow"), "type") <<
                  M(
                      nd_list() <<
                      C(
                        C(
                          C( S(Xany("string"), "type"), "normal-type"),
                        "type-definition"),
                      "qualified-type-definition") <<
                      C(
                        C(
                          C( S(Xany("anything"), "type"), "normal-type"),
                        "type-definition"),
                      "qualified-type-definition"),
                  "parameters"),
                "normal-type"),
              "type-definition"),
            "qualified-type-definition"),
            tag_list() << "type-declaration" << "type" << "T1"
          )
        , "type-declarations")
      , "tdl-schema")
  ).write_to(xp);
  ensure_equals(tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(
      L"module anything 0.0\n"
       "export\n"
       "type\n"
       "  anything param1,param2\n"
       "  optional follow string, anything\n"
  );
  C(
    M(
      nd_list() <<
      MD(Xany(0),
        nd_list() << S(Xany("anything"), "module-id")
          << S(Xany("0.0"), "version-spec")
        , "module") <<
        C(
          MD(Xany(0),
            nd_list() <<
            S(Xany(0), "export") <<
            S(Xany("anything"), "name") <<
            S(Xany("param1"), "parameter") <<
            S(Xany("param2"), "parameter") <<
            C(
              M(
                nd_list() <<
                C(
                  S(Xany(1), "slot"),
                "slot-specification") <<
                M(
                  nd_list() <<
                  S(Xany("follow"), "type") <<
                  M(
                      nd_list() <<
                      C(
                        C(
                          C( S(Xany("string"), "type"), "normal-type"),
                        "type-definition"),
                      "qualified-type-definition") <<
                      C(
                        C(
                          C( S(Xany("anything"), "type"), "normal-type"),
                        "type-definition"),
                      "qualified-type-definition"),
                  "parameters"),
                "normal-type"),
              "type-definition"),
            "qualified-type-definition"),
            tag_list() << "type-declaration" << "type" << "T1"
          )
        , "type-declarations")
      , "tdl-schema")
  ).write_to(xp);
  ensure_equals(tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
  run_test(
    L"module param SOMEVERSION\n"
     "type\n"
     "  some\n"
     "  ordered\n"
     "    @ p1 @ p2 @ p3\n"
     "    string\n"
  );
  } catch (...) {
    std::cout << std::endl << tree << std::endl;
    throw;
  }
}

template<> template<>
void object::test<4>(int) {
  no_test();
}

// further tests
}
