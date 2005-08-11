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
using gott::nstring;
using std::wstring;
using namespace stru::cf;
using schema::slotcfg;
using schema::rule_attr;

using schema::match_named;

rule_attr ra(char const *t) { 
  return rule_attr(t); 
}

namespace {
struct Schema : tut::schema_basic {
  schema::context rec;
  
  Schema() {
    schema::context_template document, type;

    document.begin("document");
      document.begin("ordered", ra("tdl-schema"));
        module_id(document);
      
        type_declarations(document);
      document.end();
    document.end();

    qualified(type);

    std::vector<schema::context*> cc(1, &rec);
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
             new stru::repatch_enumeration(std::vector<nstring>(1, "export"))),
             slotcfg(slotcfg::optional));
        document.end();

        document.begin("named", match_named::attributes("type", false));
          document.begin("ordered", rule_attr(rule_attr::simple, false));
            document.begin("follow", rule_attr("T1", false, 0));
              document.begin("node", ra("name"));
              document.end();

              document.begin("node", ra("parameter"), 
                             slotcfg(slotcfg::list));
              document.end();
            document.end();

            document.param(0);
          document.end();
        document.end();
      document.end();
    document.end();
  }

  void qualified(schema::context_template &document) {
     document.begin("follow", ra("qualified-type-definition"));
       {
         std::vector<nstring> choice(2);
         choice[0] = "enclosed"; choice[1] = "flat";
         document.begin("node", rule_attr("coat", true,
                                        new stru::repatch_enumeration(choice)));
         document.end();
       }

       document.begin("follow", 
                      rule_attr(rule_attr::simple, false));
         document.begin("named", 
                        match_named::attributes(":", false),
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

      document.begin("any", ra("parameters"), slotcfg(slotcfg::list));
        document.param(0);
        //document.begin("node");
        //document.end();
      document.end();
    document.end();
  }

  void slot(schema::context_template &document) {
    document.begin("any", ra("slot-specification"),
                   slotcfg(slotcfg::optional));
      {
        std::vector<nstring> single(4);
        single[0] = "one";  single[1] = "optional";
        single[2] = "list"; single[3] = "some";
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

namespace {
  tut::tf Tschema("schema::$schema");
}

#define IGNORE_ERRORS

namespace tut {
template<> template<>
void object::test<1>(int) {
  try {
  run_test(
      L"module anything 0.0\n"
       "export\n"
       "type\n"
       "  anything\n"
       "  optional follow string, anything\n"
  );
  S(Xany()).write_to(xp);
  ensure_equals(tree, xp);
  } catch (schema::mismatch const &m) {
#ifndef IGNORE_ERRORS
    std::cout << tree << std::endl;
    throw;
#endif
  }
}

template<> template<>
void object::test<2>(int) {
  try {
  run_test(
      L"module anything 0.0\n"
       "export\n"
       "type\n"
       "  anything param1,param2\n"
       "  optional follow string, anything\n"
  );
  S(Xany()).write_to(xp);
  ensure_equals(tree, xp);
  } catch (schema::mismatch const &m) {
#ifndef IGNORE_ERRORS
    std::cout << tree << std::endl;
    throw;
#endif
  }
}

template<> template<>
void object::test<3>(int) {
  no_test();
}

// further tests
}
