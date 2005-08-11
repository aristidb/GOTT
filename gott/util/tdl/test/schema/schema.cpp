// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
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

rule_attr ra(wchar_t const *t) { 
  return rule_attr(wstring(t)); 
}

namespace {
struct Schema : tut::schema_basic {
  schema::context rec;
  
  Schema() {
    schema::context_template document, type;

    document.begin(L"document");
      document.begin(L"ordered", ra(L"tdl-schema"));
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
    document.begin(L"named", match_named::attributes(L"module"));
      document.begin(L"follow", rule_attr(false));
        document.begin(L"node", ra(L"module-id"));
        document.end();

        document.begin(L"node", ra(L"version-spec"), 
                       slotcfg(slotcfg::range, 1, 2));
        document.end();
      document.end();
    document.end();
  }

  void type_declarations(schema::context_template &document) {
    document.begin(L"list", ra(L"type-declarations"));
      document.begin(L"ordered", ra(L"type-declaration"));
        document.begin(L"node", rule_attr(L"export", true,
             new stru::repatch_enumeration(std::vector<nstring>(1, L"export"))),
             slotcfg(slotcfg::optional));
        document.end();

        document.begin(L"named", match_named::attributes(L"type", false));
          document.begin(L"ordered", rule_attr(false));
            document.begin(L"follow", rule_attr(L"T1", false, 0));
              document.begin(L"node", ra(L"name"));
              document.end();

              document.begin(L"node", ra(L"parameter"), 
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
     document.begin(L"follow", ra(L"qualified-type-definition"));
       {
         std::vector<nstring> choice(2);
         choice[0] = L"enclosed"; choice[1] = L"flat";
         document.begin(L"node", rule_attr(L"coat", true,
                                        new stru::repatch_enumeration(choice)));
         document.end();
       }

       document.begin(L"follow", 
                      rule_attr(false));
         document.begin(L"named", 
                        match_named::attributes(L":", false),
                        slotcfg(slotcfg::optional));
           document.begin(L"node", ra(L"tag")); 
           document.end();
         document.end();

         document.begin(L"follow", ra(L"type-definition"));
           slot(document);
           normal_type(document);
         document.end();
       document.end();
     document.end();
  }

  void normal_type(schema::context_template &document) {
    document.begin(L"follow", ra(L"normal-type"));
      document.begin(L"node", ra(L"type"));
      document.end();

      document.begin(L"any", ra(L"parameters"), slotcfg(slotcfg::list));
        document.param(0);
        //document.begin(L"node");
        //document.end();
      document.end();
    document.end();
  }

  void slot(schema::context_template &document) {
    document.begin(L"any", ra(L"slot-specification"),
                   slotcfg(slotcfg::optional));
      {
        std::vector<nstring> single(4);
        single[0] = L"one";  single[1] = L"optional";
        single[2] = L"list"; single[3] = L"some";
        document.begin(L"node", rule_attr(L"slot", true, 
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
