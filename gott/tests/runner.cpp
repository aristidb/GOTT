#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>

#include <cppunit/TestPath.h>

#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
namespace po = boost::program_options;

void test(std::string const &path,
	  CppUnit::TestResult &controller,
	  CppUnit::TestRunner &runner,
	  CppUnit::Outputter &out)
{
  try {
    std::cout << "Running: "  <<  (path == "" ? "*" : path) << '\n';
    runner.run(controller, path);
    out.write();
  }
  catch(std::invalid_argument &e)  { // Test path not resolved
    std::cerr << '\n' <<  "ERROR: " << e.what() << '\n';
  }
}

typedef std::vector<std::string> string_vector;

int main(int argc, char **argv) {
  po::options_description desc("options");
  desc.add_options()
    ("help", "display help messsage")
    ("output", po::value<std::string>(),
     "specify output type (text, xml, compiler)");

  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("test-path", po::value<string_vector>(), "tests");

  po::positional_options_description p;
  p.add("test-path", -1);

  po::options_description cmdopts;
  cmdopts.add(desc).add(hidden);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdopts).
	    positional(p).run(), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    std::cerr << "usage: " << *argv << " [options] tests*\n"
	      << desc << '\n';
    return 1;
  }

  CppUnit::TestResult controller;
 
  CppUnit::TestResultCollector result;
  controller.addListener(&result);        
 
  CppUnit::TextTestProgressListener progress;
  controller.addListener(&progress);      
 
  CppUnit::TestRunner runner;
  runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

  boost::scoped_ptr<CppUnit::Outputter> out;

  if(vm.count("output")) {
    std::string format=vm["output"].as<std::string>();
    if(format == "xml")
      out.reset(new CppUnit::XmlOutputter(&result, std::cerr, "utf-8"));
    else if(format == "compiler")
      out.reset(new CppUnit::CompilerOutputter(&result, std::cerr));
  }
  else
    out.reset(new CppUnit::TextOutputter(&result, std::cerr));

  if(vm.count("test-path")) {
    int ret = 0;
    string_vector vec=vm["test-path"].as<string_vector>();
    for(string_vector::iterator i=vec.begin();
	i != vec.end();
	++i)
      {
	test(*i, controller, runner, *out.get());
	if(!result.wasSuccessful())
	  ret = 1;
      }
    return ret;
  }
  else
    test("", controller, runner, *out.get());
 
  return result.wasSuccessful() ? 0 : 1;
}
