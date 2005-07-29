// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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
#include <gott/util/tdl/schema/types/literal.hpp>
#include <gott/util/tdl/schema/types/named.hpp>
#include <gott/util/tdl/schema/types/enumeration.hpp>

namespace util = gott::util;
namespace schema = util::tdl::schema;
namespace stru = util::tdl::structure;
namespace simple = util::tdl::simple;
using util::xany::Xany;
using std::wstring;
using namespace stru::cf;
using schema::slotcfg;
using schema::rule;

using schema::match_literal;
using schema::match_named;
using schema::match_enumeration;

rule::attributes ra(wchar_t const *t) { 
  return rule::attributes(wstring(t)); 
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
      document.begin(L"follow", rule::attributes(false));
        document.begin(L"string", ra(L"module-id"));
        document.end();

        document.begin(L"string", ra(L"version-spec"), 
                       slotcfg(slotcfg::range, 1, 2));
        document.end();
      document.end();
    document.end();
  }

  void type_declarations(schema::context_template &document) {
    document.begin(L"list", ra(L"type-declarations"));
      document.begin(L"ordered", ra(L"type-declaration"));
        document.begin(L"literal", match_literal::attributes(L"export"),
                       slotcfg(slotcfg::optional));
        document.end();

        document.begin(L"named", match_named::attributes(L"type", false));
          document.begin(L"ordered", rule::attributes(false));
            document.begin(L"follow", rule::attributes(L"T1", false, 0));
              document.begin(L"string", ra(L"name"));
              document.end();

              document.begin(L"string", ra(L"parameter"), 
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
         std::vector<wstring> choice(2);
         choice[0] = L"enclosed"; choice[1] = L"flat";
         document.begin(L"enumeration", 
                        match_enumeration::attributes(choice));
         document.end();
       }

       document.begin(L"follow", 
                      rule::attributes(false));
         document.begin(L"named", 
                        match_named::attributes(L":", false),
                        slotcfg(slotcfg::optional));
           document.begin(L"string", ra(L"tag")); 
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
      document.begin(L"string", ra(L"type"));
      document.end();

      document.begin(L"any", ra(L"parameters"), slotcfg(slotcfg::list));
        document.param(0);
        //document.begin(L"string");
        //document.end();
      document.end();
    document.end();
  }

  void slot(schema::context_template &document) {
    document.begin(L"any", ra(L"slot-specification"),
                   slotcfg(slotcfg::optional));
      {
        std::vector<wstring> single(4);
        single[0] = L"one";  single[1] = L"optional";
        single[2] = L"list"; single[3] = L"some";
        document.begin(L"enumeration", 
                       match_enumeration::attributes(single));
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
