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
 * The Original Code is TUT GOTT Extensions.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2___
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

#include <gott/tut/tut.h>
#include <gott/tut/tut_reporter.h>
#include <gott/tut/tut_html.h>
#include <iostream>

namespace tut
{
  test_runner_singleton runner;
}

//#define HTML_REPORT

int main(int argc,const char* argv[])
{
#ifdef HTML_REPORT
  tut::html_reporter visi;
#else
  tut::reporter visi;
#endif

  if( (argc == 2 && (
          std::string(argv[1]) == "help"
          || std::string(argv[1]) == "--help"
        )) 
      || argc > 3 )
  {
#define A << argv[0] << 
    std::cerr << "Usage: "A" [regression] | [list] | [ group] [test]\n";
    std::cerr << "         List all groups: "A" list\n";
    std::cerr << "         Run all tests: "A" regression\n";
    std::cerr << "         Run one group: "A" std::auto_ptr\n";
    std::cerr << "         Run one test: "A" std::auto_ptr 3\n";
#undef A
  }

  tut::runner.get().set_callback(&visi);

  try
  {
    if( argc == 1 || (argc == 2 && std::string(argv[1]) == "regression") )
    {
      std::cerr << "Testing everything...\n";
      tut::runner.get().run_tests();
    }
    else if( argc == 2 && std::string(argv[1]) == "list" )
    {
      std::cerr << "Registered test groups:" << std::endl;
      tut::groupnames gl = tut::runner.get().list_groups();
      tut::groupnames::const_iterator i = gl.begin();
      tut::groupnames::const_iterator e = gl.end();
      while( i != e )
      {
        std::cerr << "  " << *i << std::endl;
        ++i;
      }
    }
    else if( argc == 2 && std::string(argv[1]) != "regression" )
    {
      std::cerr << "Testing " << argv[1] << "...\n";
      tut::runner.get().run_tests(argv[1]);
    }
    else if( argc == 3 )
    {
      std::cerr << "Testing " << argv[1] << ", test<" << argv[2] << ">...\n";
      tut::runner.get().run_test(argv[1],::atoi(argv[2]));
    }
  }
  catch( const std::exception& ex )
  {
    std::cerr << "tut raised ex: " << ex.what() << std::endl;
  }

  return !visi.all_ok();
}
