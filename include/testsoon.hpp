// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:

/*
  testsoon.hpp: "Test soon" testing framework.

  Copyright (C) 2006 Aristid Breitkreuz, Ronny Pfannschmidt and 
                     Benjamin Bykowski

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Aristid Breitkreuz aribrei@arcor.de
  Ronny Pfannschmidt Ronny.Pfannschmidt@gmx.de
  Benjamin Bykowski bennybyko@gmx.de
*/

#ifndef TESTSOON_HPP
#define TESTSOON_HPP

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/replace.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/seq/to_array.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/array/pop_front.hpp>
#include <boost/preprocessor/array/data.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/control/expr_iif.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#ifndef NO_STDLIB
#include <string>
#include <iostream>//sollte das wirklich standard sein? aber zum debuggen gut
#include <sstream>
#include <vector>
#include <typeinfo>
#endif

namespace testsoon {

#ifndef NO_STDLIB

using std::string;

typedef std::vector<string> string_vector;
typedef std::ostream stream_class;
#define DEFAULT_STREAM std::cout

template<class T>
inline string object_to_string(T const &object) {
  std::ostringstream stream;
  stream << object;
  return stream.str();
}

inline string object_to_string(std::type_info const &object) {
  return string(object.name());
}

inline string object_to_string(char const * const object) {
  return string(object);
}

inline string object_to_string(string const &object) {
  return string(object);
}

#endif

#ifndef IN_DOXYGEN
class test_holder;
#endif

extern test_holder &tests();

#ifndef IN_DOXYGEN
#if defined(__EXCEPTIONS) || defined(_CPPUNWIND)
#define TESTSOON_EXCEPTIONS 1
#define TESTSOON_NO_EXCEPTIONS 0
#else
#define TESTSOON_EXCEPTIONS 0
#define TESTSOON_NO_EXCEPTIONS 1
#endif
#endif

class test_reporter;
class test_info;
class test_group;
class test_failure;

class statistics {
public:
  statistics() : good(0), bad(0) {}
  unsigned good;
  unsigned bad;
private:
  typedef bool (statistics::*bool_type)() const;
  bool do_not_compare_implicit_bool() const { return false; }
public:
  operator bool_type() const {
    return bad == 0 ? &statistics::do_not_compare_implicit_bool : 0;
  }
};

template<class T> bool operator==(statistics const &x, T const &) {
  return x.do_not_compare_implicit_bool();
}
template<class T> bool operator!=(statistics const &x, T const &) {
  return x.do_not_compare_implicit_bool();
}
template<class T> bool operator==(T const &, statistics const &x) {
  return x.do_not_compare_implicit_bool();
}
template<class T> bool operator!=(T const &, statistics const &x) {
  return x.do_not_compare_implicit_bool();
}

/**
 * Base class for reporters.
 */
class test_reporter {
public:
  /// Indicates the start of a whole test run.
  virtual void start() {}
  /// Indicates the end of a whole test run.
  virtual void stop() {}
  /// Indicates the start of a test group.
  virtual void begin_group(test_group const &group) { (void)group; }
  /// Indicates the end of a test group.
  virtual void end_group(test_group const &group) { (void)group; }
  /// Indicates that the next event will be success() or failure().
  virtual void before_tests(test_group const &group) { (void)group; }
  /// Indicates that the next event will not be success() or failure().
  virtual void after_tests(test_group const &group) { (void)group; }
  virtual void success(test_info const &info, string const &sequence_key) {
    (void)info;
    (void)sequence_key;
  }
  virtual void failure(test_info const &info, test_failure const &failure, 
                       string const &sequence_key) = 0;
  virtual void stats(statistics const &stat) { (void)stat; }
  virtual ~test_reporter() {}
};

#ifndef IN_DOXYGEN

class node {
public:
  node(test_group *, char const * const, bool = false);
  virtual void run(test_reporter &, statistics &) const = 0;
  virtual ~node() {}

  test_group const * const parent;
  char const * const name;

  void print(stream_class &out) const;

private:
  friend class test_group;
  node *next;
};

inline stream_class &operator<<(stream_class &out, node const &n) {
  n.print(out);
  return out;
}

class test_group : public node {
public:
  test_group(test_group *parent, char const *name)
    : node(parent, name), child(0) {}
  
  void add(node *, bool);

  void run(test_reporter &, statistics &) const;

private:
  node *child;
  test_info *test;
};

inline node::node(test_group *parent, char const *name, bool is_test)
: parent(parent), name(name), next(0) {
  if (parent)
    parent->add(this, is_test);
}

inline void node::print(stream_class &out) const {
  if (parent) {
    parent->print(out);
    if (parent->parent)
      out << '/' << name;
    else
      out << '"' << name << '"';
  }
}

class test_holder : public test_group {
public:
  test_holder() : test_group(0, "") {}
  statistics run(test_reporter &rep) {
    statistics stats;
    run(rep, stats);
    return stats;
  }
  void run(test_reporter &rep, statistics &stats) {
    rep.start();
    test_group::run(rep, stats);
    rep.stop();
    rep.stats(stats);
  }
};

class test_file : public test_group {
public:
  test_file(test_group *parent, char const *file)
    : test_group(parent, file) {}
};

class test_info : public node {
public:
  test_info(test_group *group,
            char const *name, char const *file, unsigned line)
  : node(group, name, true), file(file), line(line) {}

  char const *file;
  unsigned const line;
};

#endif

class test_failure {
public:
  test_failure() : line(0) {}
  test_failure(char const *message,
               unsigned line,
               string_vector const &data = string_vector())
    : message(message), line(line), data(data) {}
  ~test_failure() {}
  char const *message;
  unsigned line;
  string_vector data;

  bool is_failure() { return line; }
};

struct failure_info {
  failure_info(test_info const &test, test_failure const &failure, 
               string const &value)
    : ptest(&test), failure(failure), value(value) {}

  test_info const *ptest;
  test_failure failure;
  string value;
};

class simple_reporter : public test_reporter {
public:
  typedef stream_class stream;
  simple_reporter(stream &out = DEFAULT_STREAM) : out(out) {}
  void before_tests(test_group const &group) {
    out << group << " : ";
    out.flush();
  }

  void after_tests(test_group const &) {
    out << '\n';
    out.flush();
  }

  void success(test_info const &, string const &){
    out << '.';
    out.flush();
  }

  void failure(test_info const &i, test_failure const &x, string const &k) {
    out << "[F=" << i.line << '.' << x.line;
    if (k != string())
      out << '<' << k << '>';
    out << ']';
    out.flush();
  }

protected:
  stream &out;
};

class concise_reporter : public simple_reporter {
public:  
  concise_reporter(stream &out = DEFAULT_STREAM) 
  : simple_reporter(out) {}

private:
  typedef std::vector<failure_info> failure_vector;
  failure_vector reports;
  
  void stats(statistics const &x) {
    out << (x.good + x.bad) << " tests, "
        << x.good << " succeeded, "
        << x.bad << " failed.\n";
    out.flush();
  }
  
  void write_report() {
    for (failure_vector::const_iterator it = reports.begin();
        it != reports.end();
        ++it)
      write_report_entry(*it);
    out << '\n';
    out.flush();
  }

  void write_report_entry(failure_info const &info) {
    out << "\nError occured in test ";
    if (*info.ptest->name)
      out << '"' << info.ptest->name << "\" ";
    out << "in " << *info.ptest->parent
        << " on line " << info.ptest->line
        << " in check on line " << info.failure.line;
    if (info.value != string())
      out << " with value: \n\t" << info.value;
    else
      out << '.';
    out << "\nProblem: " << info.failure.message << '\n';

    if (!info.failure.data.empty()) {
      out << "Data:\n";
      for (string_vector::const_iterator it = info.failure.data.begin();
           it != info.failure.data.end();
           ++it) {
        out << "\t" << *it << '\n';
      }
    }
    out.flush();
  } 

  void success(test_info const &i, string const &k) {
    simple_reporter::success(i, k);
  }
    
  void failure(test_info const &i, test_failure const &x, string const &k) {
    simple_reporter::failure(i, x, k);
    reports.push_back(failure_info(i, x, k));
  }

  void stop() {
    write_report();
  }
};

/*
 * XML reporter first (not that beautiful) try
 * <?xml version=1.0?>
 * <testsoon>
     <group name="file.cpp">
       <group name="bla">
         <success line="LINE"> <generator>7</generator> </success>
         <failure line="LINE" name="TESTNAME">
         <problem>PROBLEM</problem>
         <rawdata>
            <item>BLA</item>
         </rawdata>
           <generator>7</generator>
 *       </failure>
 *     </group>
     </group>
 * </testsoon>
 */
 

class xml_reporter : public test_reporter {
  public:
    typedef stream_class stream;
    xml_reporter(stream &out = DEFAULT_STREAM) : out(out), indent(1) {}

    void start() {
      out << "<?xml version=\"1.0\"?>\n";
      out << "<testsoon>\n";
    }
    void stop() {
      out << "</testsoon>\n";
    }

    void begin_group(test_group const &group) {
      if (group.parent) {
        print_ind();
        ++indent;
        if (group.parent->parent)
          out << "<group";
        else
          out << "<file";
        out << " name=\"" << group.name << "\">\n";
      }
    }

    void end_group(test_group const &group) {
      if (group.parent) {
        --indent;
        print_ind();
        if (group.parent->parent)
          out << "</group>\n";
        else
          out << "</file>\n";
      }
    }

    void success(test_info const &i, string const &k) {
      print_ind() << "<success line=\"" << i.line << "\"";      
      if (*i.name)
        out << " name=\"" << i.name << "\"";
      if (k.empty())
        out << " />\n";
      else {
        out << ">\n";
        ++indent;
        print_ind() << "<generator>" << k << "</generator>\n";
        --indent;
        print_ind() << "</success>\n";
      }
    }
    
    void failure(test_info const &i, test_failure const &x, string const &k) {
      print_ind() << "<failure line=\"" << i.line << "\"";
      if (*i.name)
        out << " name=\"" << i.name << "\"";
      out << ">\n";
      ++indent;

      print_ind() << "<problem>" << x.message << "</problem>\n";
      if (!x.data.empty()) {
        print_ind() << "<rawdata>\n";
        ++indent;
        for (string_vector::const_iterator it = x.data.begin(); 
            it != x.data.end();
            ++it) {
          print_ind() << "<item>" << *it << "</item>\n";
        } 
        --indent;
        print_ind() << "</rawdata>\n";
      }

      if (!k.empty()) {
        print_ind() << "<generator>" << k << "</generator>\n";
      }

      --indent;
      print_ind() << "</failure>\n";
    } 

  private:
    stream_class &print_ind() {
      for (unsigned i = 0; i < indent; ++i)
        out << "  ";
      return out;
    }
    /*
    string create_attribute(string const &name, string const &value) {
      return string(" ") + name + "=\"" + value + "\"";
    }
    template<class T>
    string create_attribute(string const &name, T const &value) {
      return create_attribute(name, object_to_string(value));
    }
    
    void open_tag(string const &tagname, string const &attributes, bool indent = true, bool newline = true) {
      if (indent)
        print_ind();
      out << '<' << tagname << attributes << '>';
      if (newline)
      	out << '\n';
    }
    
    void close_tag(string const &tagname, bool indent = true, bool newline = true) {
      if (indent)
        print_ind();
      out << "</" << tagname << '>';
      if (newline)
      	out << '\n';
    }
    */
    stream &out;
    unsigned indent;
};

typedef concise_reporter default_reporter;
//typedef xml_reporter default_reporter;


#ifndef IN_DOXYGEN

inline void test_group::add(node *nchild, bool is_test) {
  node *tail = is_test ? test : child;
  if (!tail) {
    if (is_test) test = static_cast<test_info *>(nchild);
    else         child = nchild;
  } else {
    while (tail->next)
      tail = tail->next;    
    tail->next = nchild;
  }
}

inline void test_group::run(test_reporter &rep, statistics &stats) const {
  rep.begin_group(*this);
  if (test) {
    rep.before_tests(*this);
    for (node *it = test; it; it = it->next)
      it->run(rep, stats);
    rep.after_tests(*this);
  }
  for (node *it = child; it; it = it->next)
    it->run(rep, stats);
  rep.end_group(*this);
}

template<class T, class U>
inline void check_equals(T const &a, U const &b,
                         char const *msg, unsigned line,
                         test_failure &fail) {
  if (!(a == b)) {
    string_vector data;
    data.push_back(object_to_string(a));
    data.push_back(object_to_string(b));
    fail = test_failure(msg, line, data);
  }
}

template<class F, class T>
inline void do_check1(F fun, T const &val,
                      char const *msg, unsigned line,
                      test_failure &fail) {
  if (!fun(val)) {
    string_vector data;
    data.push_back(object_to_string(val));
    fail = test_failure(msg, line, data);
  }
}

template<class F, class T, class U>
inline void do_check2(F fun, T const &a, U const &b,
                      char const *msg, unsigned line,
                      test_failure &fail) {
  if (!fun(a, b)) {
    string_vector data;
    data.push_back(object_to_string(a));
    data.push_back(object_to_string(b));
    fail = test_failure(msg, line, data);
  }
}

#endif

template <typename Type>
class range_generator {
public:
  typedef Type value_type;
  typedef Type const &const_reference;

  range_generator(const_reference a, const_reference b) : a(a), b(b) {}

  class iterator {
  public:
    iterator(const_reference v = value_type()) : v(v) {}
    iterator(iterator const &x) : v(x.v) {}
    
    bool operator!=(iterator const &rhs) {
      return v != rhs.v;
    }

    value_type &operator*() { return v;}

    iterator &operator++() { 
      ++v;
      return *this; 
    }

    iterator operator++(int) { 
      iterator tmp(*this); 
      ++*this;
      return tmp;
    }

  private:
    value_type v;
  };

  iterator begin() { return iterator(a); }
  iterator end() { return iterator(b); }

private:
  value_type a;
  value_type b;
};

/**
 * Add this macro to exactly one source file to ensure proper instantiation.
 */
#define TEST_REGISTRY \
  namespace testsoon { \
    test_holder &tests() { \
      static test_holder t; \
      return t; \
    } \
  }

/**
 * Declare a test group.
 * Test groups are nestable.
 */
#define TEST_GROUP(name) \
    namespace BOOST_PP_CAT(name, _helper) { \
      static ::testsoon::test_group *upper_test_group() { \
        return test_group(__FILE__); \
      } \
    } \
    namespace BOOST_PP_CAT(name, _testgroup) { \
      static ::testsoon::test_group * \
      test_group(char const *) { \
        static ::testsoon::test_group current( \
          BOOST_PP_CAT(name, _helper)::upper_test_group(), #name); \
        return &current; \
      } \
    } \
    namespace BOOST_PP_CAT(name, _testgroup)

/**
 * Declare a test (positional). You do not want to use this directly.
 * @param name The name of the test (quoted).
 * @param fixture A tuple consisting of whether to use a fixture and a fixture 
 *             class.
 */
#define PTEST(name, fixture, group_fixture, generator) \
  TESTSOON_PTEST1( \
    name, \
    BOOST_PP_CAT(test_, __LINE__), \
    BOOST_PP_TUPLE_ELEM(2, 0, fixture), \
    BOOST_PP_TUPLE_ELEM(2, 1, fixture), \
    group_fixture, \
    BOOST_PP_TUPLE_ELEM(2, 0, generator), \
    BOOST_PP_TUPLE_ELEM(2, 1, generator))

/**
 * Declare a test (optional name only).
 * @param name The name of the test (not quoted).
 */
#define TEST(name) \
  PTEST(#name, (0, ~), 0, (0, ()))

/**
 * Declare a test with fixture.
 * @param name The name of the test (not quoted).
 * @param fixture_class The fixture class to use.
 */
#define FTEST(name, fixture_class) \
  PTEST(#name, (1, fixture_class), 0, (0, ()))

/**
 * Declare a test with default group fixture, named group_fixture.
 * @param name The name of the test (not quoted).
 */
#define GFTEST(name) \
  PTEST(#name, (0, ~), 1, (0, ()))

#ifndef IN_DOXYGEN

#if TESTSOON_NO_EXCEPTIONS
#define TESTSOON_FAILURE_P ::testsoon::test_failure &testsoon_failure
#else
#define TESTSOON_FAILURE_P int
#endif

#define TESTSOON_TEST_PARAM( \
    has_fixture, \
    fixture_class, \
    has_group_fixture, \
    has_generator, \
    generator_class) \
  BOOST_PP_IIF(has_fixture, fixture_class &fixture, int), \
  BOOST_PP_IIF(has_group_fixture, group_fixture_t &group_fixture, int), \
  BOOST_PP_IIF(has_generator, generator_class::const_reference value, int), \
  TESTSOON_FAILURE_P

#define TESTSOON_PTEST1( \
    name, \
    test_class, \
    has_fixture, \
    fixture_class, \
    group_fixture, \
    has_generator, \
    generator_seq) \
  TESTSOON_PTEST2( \
    name, test_class, has_fixture, fixture_class, \
    TESTSOON_TEST_PARAM( \
      has_fixture, fixture_class, group_fixture, \
      has_generator, BOOST_PP_SEQ_HEAD(generator_seq)), \
    group_fixture, \
    has_generator, \
    BOOST_PP_SEQ_HEAD(generator_seq), \
    BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_SEQ_TO_ARRAY(generator_seq)) \
  )

#if TESTSOON_EXCEPTIONS

#define TESTSOON_TESTRUN(x) \
  try { \
    x; \
    reporter.success(*this, key); \
    ++stats.good; \
  } catch (::testsoon::test_failure const &state) { \
    reporter.failure(*this, state, key); \
    ++stats.bad; \
  }

#else

#define TESTSOON_TESTRUN(x) \
  ::testsoon::test_failure state; \
  x; \
  if (!state.is_failure()) { \
    reporter.success(*this, key); \
    ++stats.good; \
  } else { \
    reporter.failure(*this, state, key); \
    ++stats.bad; \
  }

#endif

#define TESTSOON_PTEST2(\
    name, \
    test_class, \
    has_fixture, \
    fixture_class, \
    test_param, \
    has_group_fixture, \
    has_generator, \
    generator_class, \
    generator_param) \
  namespace { \
    struct test_class \
    : public ::testsoon::test_info { \
      test_class () : ::testsoon::test_info( \
            test_group(__FILE__), name, __FILE__, __LINE__) {} \
      void run( \
          ::testsoon::test_reporter &reporter, \
          ::testsoon::statistics &stats \
      ) const { \
        BOOST_PP_EXPR_IIF(has_fixture, fixture_class fixture;) \
        BOOST_PP_EXPR_IIF(has_group_fixture, group_fixture_t group_fixture;) \
        BOOST_PP_EXPR_IIF(has_generator, \
        generator_class gen \
          BOOST_PP_IIF( \
            BOOST_PP_EQUAL(BOOST_PP_ARRAY_SIZE(generator_param), 0), \
            BOOST_PP_EMPTY(), \
            BOOST_PP_ARRAY_DATA(generator_param)); \
        for (generator_class::iterator i = gen.begin(); i != gen.end(); ++i)) {\
          ::testsoon::string key \
            BOOST_PP_EXPR_IIF( \
              has_generator, \
              (::testsoon::object_to_string(*i))); \
          TESTSOON_TESTRUN( \
            do_test( \
                BOOST_PP_IIF(has_fixture, fixture, 0), \
                BOOST_PP_IIF(has_group_fixture, group_fixture, 0), \
                BOOST_PP_IIF(has_generator, *i, 0), \
                BOOST_PP_IIF(TESTSOON_NO_EXCEPTIONS, state, 0) \
            ) \
          ) \
        } \
      } \
      void do_test(test_param) const; \
    } BOOST_PP_CAT(test_obj_, __LINE__); \
  } \
  void test_class::do_test(test_param) const


#define TESTSOON_GEN_TUPLE2SEQ_PROCESS2(x, y) \
  ((x, y)) \
  TESTSOON_GEN_TUPLE2SEQ_PROCESS

#define TESTSOON_GEN_TUPLE2SEQ_PROCESS(x, y) \
  ((x, y)) \
  TESTSOON_GEN_TUPLE2SEQ_PROCESS2

#define TESTSOON_GEN_TUPLE2SEQ_PROCESS_ELIM
#define TESTSOON_GEN_TUPLE2SEQ_PROCESS2_ELIM

#define TESTSOON_GEN_TUPLE2SEQ(x) \
  BOOST_PP_CAT(TESTSOON_GEN_TUPLE2SEQ_PROCESS x, _ELIM)

#define TESTSOON_PARAM_CHANGES(x) \
  ((0, BOOST_PP_SEQ_ELEM(0, TESTSOON_PARAM_INITIAL))) \
  BOOST_PP_SEQ_FOR_EACH( \
    TESTSOON_PARAM_EXPAND, \
    ~, \
    TESTSOON_GEN_TUPLE2SEQ(x))

#define TESTSOON_PARAM_EXPAND(r, d, e) \
  TESTSOON_PARAM_EXPAND2 e

#define TESTSOON_PARAM_EXPAND2(x, y) \
  ((BOOST_PP_CAT(TESTSOON_PARAM__, x)(y)))

#define TESTSOON_PARAM_COMBINE(s, state, x) \
  BOOST_PP_SEQ_REPLACE( \
    state, \
    BOOST_PP_TUPLE_ELEM(2, 0, x), \
    BOOST_PP_TUPLE_ELEM(2, 1, x))

#define TESTSOON_PARAM_INVOKE2(x) \
  BOOST_PP_SEQ_TO_TUPLE( \
    BOOST_PP_SEQ_FOLD_LEFT( \
      TESTSOON_PARAM_COMBINE, \
      TESTSOON_PARAM_INITIAL, \
      TESTSOON_PARAM_CHANGES(x)))

#define TESTSOON_PARAM_INVOKE(x) \
  TESTSOON_PARAM_INVOKEx(TESTSOON_PARAM_INVOKE2(x))


#define TESTSOON_PARAM__name(x)           0, x
#define TESTSOON_PARAM__n(x)              TESTSOON_PARAM__name(x)
#define TESTSOON_PARAM__fixture(x)        1, (1, x)
#define TESTSOON_PARAM__f(x)              TESTSOON_PARAM__fixture(x)
#define TESTSOON_PARAM__group_fixture(x)  2, x
#define TESTSOON_PARAM__gf(x)             TESTSOON_PARAM__group_fixture(x)
#define TESTSOON_PARAM__generator(x)      3, (1, x)
#define TESTSOON_PARAM__gen(x)            TESTSOON_PARAM__generator(x)


#define TESTSOON_PARAM_INITIAL \
  ("") ((0, ~)) (0) ((0, ()))

#define TESTSOON_PARAM_INVOKEx(x) \
  PTEST x

#if TESTSOON_EXCEPTIONS

#define TESTSOON_FAIL(msg, data) \
    throw ::testsoon::test_failure(msg, __LINE__, data);

#define TESTSOON_ENCLOSURE(x) \
  do { \
    ::testsoon::test_failure testsoon_failure; \
    x; \
    if (testsoon_failure.is_failure()) \
      throw testsoon_failure; \
  } while (false)

#else //TESTSOON_NO_EXCEPTIONS

#define TESTSOON_FAIL(msg, data) \
  testsoon_failure = ::testsoon::test_failure(msg, __LINE__, data); \
  return;

#define TESTSOON_ENCLOSURE(x) \
  do { \
    x; \
    if (testsoon_failure.is_failure()) \
      return; \
  } while (false)

#endif

#endif //IN_DOXY

/**
 * Declare a test (named parameters).
 * @param name The name of the test (quoted).
 */
#define XTEST(named_parameter_sequence) \
  TESTSOON_PARAM_INVOKE(named_parameter_sequence)

/**
 * Check whether two values are equal.
 * If both values are not equal, the test will fail.
 * @param a Some value.
 * @param b Another value.
 */
#define TESTSOON_Equals(a, b) \
  TESTSOON_ENCLOSURE( \
    ::testsoon::check_equals(a, b, \
    "not equal: " #a " and " #b, __LINE__, testsoon_failure); \
  )

#define Equals(a, b) TESTSOON_Equals(a, b)

/**
 * Check that an expression throws.
 * If no exception is thrown, the test will fail.
 * @param x The expression to test.
 * @param t The exception type to check for.
 * @param w The expected value of caught_exception.what().
 */
#if TESTSOON_EXCEPTIONS
#define TESTSOON_Throws(x, t, w) \
	do { \
		try { \
			(x); \
			TESTSOON_FAIL("not throwed " #t, ::testsoon::string_vector()); \
		} catch (t &e) { \
			if ( \
        ::testsoon::string(w) != ::testsoon::string() && \
        ::testsoon::string(e.what()) != ::testsoon::string((w))) \
				TESTSOON_FAIL("throwed " #t " with wrong message", \
          ::testsoon::string_vector()); \
		} \
	} while (0)
#else
#define TESTSOON_Throws(x, t, w) \
  TESTSOON_Check(!"Throws check without exception support")
#endif

#define Throws(x, t, w) TESTSOON_Throws(x, t, w)

/**
 * Check that an expression does not throw.
 * If a specified exception is thrown, the test will fail.
 * @param x The expression to test.
 * @param t The exception type to check for or "..." (without quotes).
 */
#if TESTSOON_EXCEPTIONS
#define TESTSOON_Nothrows(x, t) \
	do { \
		try { \
			(x); \
		} catch (t) { \
			TESTSOON_FAIL("throwed " #t, ::testsoon::string_vector()); \
		} \
	} while (0)
#else
#define TESTSOON_Nothrows(x, t) \
  TESTSOON_Check(!"Nothrows check without exception support")
#endif
#define Nothrows(x, t) TESTSOON_Nothrows(x, t)

#define TESTSOON_Check(x) \
  do { \
    if (!(x)) \
      TESTSOON_FAIL("check " #x, ::testsoon::string_vector()); \
  } while (0)

#define Check(x) TESTSOON_Check(x)

#define TESTSOON_Check1(x, a) \
  TESTSOON_ENCLOSURE( \
    ::testsoon::do_check1(x, a, "check1 " #x, __LINE__, testsoon_failure); \
  )

#define Check1(x, a) TESTSOON_Check1(x, a)

#define TESTSOON_Check2(x, a, b) \
  TESTSOON_ENCLOSURE( \
    ::testsoon::do_check2(x, a, b, "check2 " #x, __LINE__, testsoon_failure); \
  )

#define Check2(x, a, b) TESTSOON_Check2(x, a, b)

} //namespace testsoon

#ifndef IN_DOXYGEN

inline static ::testsoon::test_group *
test_group(char const *filename) {
  static ::testsoon::test_file file(&::testsoon::tests(), filename);
  return &file;
}

#endif

/**
 * @mainpage
 *
 * "Test soon" is a testing framework trying to enable you to write tests
 * quickly, organize them easily and still being flexible.
 *
 * The goal is to utilize the strengths of C++ while minimizing the impact of
 * its weaknesses.
 *
 * @li @subpage tutorial
 * @li @subpage faq
 * @li @ref testsoon.hpp "Reference"
 *
 * @li <a href="http://sourceforge.net/projects/testsoon">SF.NET Project Page</a>
 *
 * @htmlonly
<a href="http://sourceforge.net"><img src="http://sflogo.sourceforge.net/sflogo.php?group_id=180756&amp;type=7" width="210" height="62" border="0" alt="SourceForge.net Logo" /></a>
@endhtmlonly
 *
 * @page tutorial Tutorial
 *
 * @section tut_start Getting started
 *
 * Let's start quickly. We must test whether the compiler actually works
 * decently! We do this by testing whether 1 is actually the same as 1.
 *
 * @code
#include <testsoon.hpp>

TEST(compiler_check) {
  Equals(1, 1); // let's hope it works!!
}

TEST_REGISTRY;

int main() {
  testsoon::default_reporter rep;
  testsoon::tests().run(rep);
}
@endcode
 *
 * In order to compile this ... important test, you first need to make sure that
 * a recent testsoon.hpp is in your include path. It can be found in the
 * include/ directory of the distribution. You may just copy it into your
 * project folder. No other installation is required.
 *
 * If you compile and run this program, you should see something like this on
 * your console:
 * @verbatim
"simple.cpp" : .

1 tests, 1 succeeded, 0 failed.
@endverbatim
 *
 * I guess this means that we can trust our compiler a little bit. Or so it
 * seems. Seriously, this is our first successful test. Let me explain what
 * the code above actually means. I shall do this by thoroughly commenting
 * the code.
 *
 * @code
// You really can guess why we do this.
#include <testsoon.hpp>

// Declare a simple test with name "compiler_check". Note that no quotes are
// required here.
TEST(compiler_check) {
  // Check whether the two numbers are equal.
  Equals(1, 1);
}

// This line is required _once_ per executable. It ensures that if the code
// compiles, everything works smoothly. The principle here: no surprises.
TEST_REGISTRY;

int main() {
  // Declare a reporter. The default_reporter should be a sensible setting.
  // That's why it's the default.
  testsoon::default_reporter rep;

  // Run all tests.
  testsoon::tests().run(rep);
}
@endcode
 *
 * So now let's play around and test something different: are 1 and 2 equal?
 * Change the check as follows:
 *
 * @code
Equals(1, 2);
@endcode
 *
 * Now, the output should look like something like this:
 *
 * @verbatim
"simple.cpp" : [F=3.4]

Error occured in test "compiler_check" in "simple.cpp" on line 3 in check on line 4.
Problem: not equal: 1 and 2
Data:
        1
        2

1 tests, 0 succeeded, 1 failed.
@endverbatim
 *
 * Obviously, both numbers differ. Lets look at the first strange thing:
 * "[F=3.4]". This little thing means that there was a failure in the test on
 * line 3 (simple.cpp), to be exact the check on line 4 failed. (I used the
 * version without comments.)
 *
 * The same information is represented below and with additional information.
 * "Data" are the two parameter values to Equals. This is necessary because
 * in other situations the "problem" might not be "not equal: 1 and 2" but
 * "not equal: a and b" where a and b are variables. In this case, "data" would
 * contain the values of both variables in (readable) string representation.
 *
 * @section tut_grouping Grouping
 *
 * Named tests are nice, of course. And grouping by files is practical, too.
 * However, you might want to group tests deeper. Or you might be to lazy to
 * create a new file for this new test group. Do not despair!
 *
 * @code
TEST_GROUP(group_one) {
  TEST() {
    Check(!"unnamed test in group_one");
  }

  TEST(named) {
    Check(!"named test in group_one");
  }
}

TEST_GROUP(group_two) {
  TEST() {
    Check(!"only test in group_two");
  }
  TEST_GROUP(nested) {
    TEST() {
      Check(!"except if you count this test in a nested group");
    }
  }
}
@endcode
 *
 * The Checks will all fail because they are passed a false value. \c ! applied
 * to a non-null pointer value is always \c false.
 *
 * @page faq Frequently Asked Questions (FAQ)
 *
 * <ol>
 *
 * <li><b>Does "Test soon" work when exceptions are disabled?</b>
 * <p>
 * Yes, it should be able to detect this automatically. When exceptions are
 * disabled, all checks must be directly in the test and not in a helper
 * function.
 *
 * If you have exception support, "Test soon" should be able to take advantage
 * of this automatically.
 * </p></li>
 *
 * <li><b>Why do you use macros (so much)?</b>
 * <p>
 * We believe that convenience is more important than purity. I have used
 * testing frameworks without macros, so I know what I'm talking about. We
 * believe that testing frameworks are one of the very few places where macros
 * should be part of the public interface.
 *
 * Note that \c #Equals etc. are macros, but you can use \c #TESTSOON_Equals
 * etc. instead and \#undef the short names if you have conflicts. Depending on
 * your naming convention, this will be more or less important.
 * </p></li>
 *
 * <li><b>But macros are evil!!!</b>
 * <p>
 * Indeed. There is even a Guru-of-the-Week article about the blessings of
 * the Win32 "Sleep" macro et al. However, if, after serious consideration,
 * macros are the only convenient alternative, we should and will use them.
 *
 * Convenience is especially important in testing, as you won't write tests
 * if it is not convenient. That's our experience. Other than that, we tried
 * hard to reduce risks. Especially dangerously named macros also have longer
 * names so you can \#undef the short version.
 * </p></li>
 *
 * <li><b>Where can I download "Test soon"?</b>
 * <p>
 * See the
 * <a href="http://sourceforge.net/project/showfiles.php?group_id=180756">
 * Sourceforge.NET download site</a>.
 * </p></li>
 *
 * <li><b>What does the name mean?</b>
 * <p>
 * Nice you ask. It's a promise. You will be able to test your code soon.
 * Of course, no legal warranties or anything, but that's what we
 * tried hard. Really hard - darkest magic we use inside.
 * </p></li>
 *
 * <li><b>Why is "Test soon" organized as a single header?</b>
 * <p>
 * For ease of use. You can simply copy the header into your project directory.
 * Or you can copy it to \c /usr/include. Whatever you prefer.
 * </p></li>
 *
 * <li><b>Why don't you use a plain singleton instead of #TEST_REGISTRY?</b>
 * <p>
 * Because it might not work. Depending on your compiler, the tests might build
 * wihout any error message or warning but would not work. We considered three
 * alternatives:
 *
 * -# Put the singleton into a \c .cpp file. The problem: "Test soon" could no
 *   longer be used as a single header file but would be a full-blown library.
 *   We discarded that quickly.
 * -# Require the user to write a variable / function definition.
 * -# Require the user to write a simple \c #TEST_REGISTRY; somewhere.
 *
 * We chose the macro alternative because we considered it the simplest and
 * most convenient.
 * </p></li>
 *
 * </ol>
 */

#endif
