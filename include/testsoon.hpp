// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
#ifndef TESTSOON_HPP
#define TESTSOON_HPP

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/replace.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#ifndef NO_STDLIB
#include <string>
#include <iostream>
#endif

namespace testsoon {

#ifndef NO_STDLIB

typedef std::string test_string;
typedef std::ostream stream_class;
#define DEFAULT_STREAM std::cout

#endif

#ifndef IN_DOXYGEN

class test_reporter;
class test_info;
class test_group;

class node {
public:
  node(test_group *, test_string const &, bool = false);
  virtual void run(test_reporter &) const = 0;
  virtual ~node() {}

  test_group const * const parent;
  test_string const name;

  void print(stream_class &out) const;

private:
  friend class test_group;
  node *next;
};

inline stream_class &operator<<(stream_class &out, node const &n) {
  n.print(out);
  return out;
}

class test_info;

class test_group : public node {
public:
  test_group(test_group *parent, test_string const &name)
    : node(parent, name), child(0) {}
  
  void add(node *, bool);

  void run(test_reporter &rep) const;

private:
  node *child;
  test_info *test;
};

inline node::node(test_group *parent=0,
                  test_string const &name=test_string(),
                  bool is_test)
  : parent(parent), name(name), next(0) {
  if (parent)
    parent->add(this, is_test);
}

inline void node::print(stream_class &out) const {
  if (parent)
    parent->print(out);
  out << '/' << name;
#if 0
  out << '(' << typeid(*this).name() << ')';
#endif
}

class test_holder : public test_group {
public:
  test_holder() : test_group(0, test_string()) {}
  //void run(test_reporter &) const;
};

class test_file : public test_group {
public:
  test_file(test_group *parent, test_string const &file)
    : test_group(parent, file) {}
};

class test_info : public node {
public:
  test_info(test_group *group,
            test_string const &name, test_string const &file, unsigned line,
            void (*function)())
  : node(group, name, true), file(file), line(line), function(function) {}

  void run(test_reporter &) const;

  test_string const file;
  unsigned const line;

  typedef void test_function_type();
  test_function_type * const function;
};

#endif

class test_failure {
public:
  test_failure(test_string const &message, unsigned line)
    : message(message), line(line) {}
  ~test_failure() {}
  test_string message;
  unsigned line;
};

class test_reporter {
public:
  virtual void begin_group(test_group const &) {}
  virtual void end_group(test_group const &) {}
  virtual void before_tests(test_group const &) {}
  virtual void after_tests(test_group const &) {}
  virtual void success(test_info const &) {}
  virtual void failure(test_info const &, test_failure const &) = 0;
  virtual ~test_reporter() {}
};

class default_reporter : public test_reporter {
public:
  typedef stream_class stream;
  default_reporter(stream &out = DEFAULT_STREAM) : out(out) {}
  void tell_test(test_info const &it, char const *state) {
    out << '"' << it.name << "\", "
        << '"' << *it.parent << "\", "
        << it.file << ", "
        << it.line << ", "
        //<< (void*)it.function << " " //DAS hier stört -pedantic 
        << state << std::endl;
  }
  void before_tests(test_group const &group) {
    out << group << " : ";
    out.flush();
  }
  void after_tests(test_group const &) {
    out << '\n';
    out.flush();
  }
  void success(test_info const &){
    out << '.';
    out.flush();
  }
  void failure(test_info const &, test_failure const &x) {
    out << "[F=" << x.line << "]";
    out.flush();
  }
private:
  stream &out;
};

#ifndef IN_DOXYGEN

inline void test_group::add(node *nchild, bool is_test) {
  if (is_test) {
    test_info *ntest = static_cast<test_info *>(nchild);
    ntest->next = test;
    test = ntest;
  } else {
    nchild->next = child;
    child = nchild;
  }
}

inline void test_group::run(test_reporter &rep) const {
  rep.begin_group(*this);
  if (test) {
    rep.before_tests(*this);
    for (node *it = test; it; it = it->next)
      it->run(rep);
    rep.after_tests(*this);
  }
  for (node *it = child; it; it = it->next)
    it->run(rep);
  rep.end_group(*this);
}

inline void test_info::run(test_reporter &reporter) const {
  try {
    function();
    reporter.success(*this);
  } catch (test_failure const &x) {
    reporter.failure(*this, x);
  }
}

extern test_holder &tests();

inline bool operator<(test_info const &a, test_info const &b) {
  return a.function < b.function;
}

inline void fail(char const *msg, unsigned line) {
	throw test_failure(msg, line);
}

template<class T, class U>
inline void check_equals(T const &a, U const &b,
                         char const *msg, unsigned line) {
  if (!(a == b))
    fail(msg, line);
}

#endif

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
    namespace name { \
      static ::testsoon::test_group *test_group(::testsoon::test_string const &) { \
        static ::testsoon::test_group current( \
          BOOST_PP_CAT(name, _helper)::upper_test_group(), #name); \
        return &current; \
      } \
    } \
    namespace name

/**
 * Declare a test (positional).
 * @param name The name of the test (quoted).
 */
#define PTEST(name, fixture) \
  static void BOOST_PP_CAT(test_, __LINE__) (); \
  static ::testsoon::test_info BOOST_PP_CAT(reg_, __LINE__) \
    (test_group(__FILE__), \
    name, __FILE__, __LINE__, \
    &BOOST_PP_CAT(test_, __LINE__)); \
  static void BOOST_PP_CAT(test_, __LINE__) ()

/**
 * Declare a test (optional name only).
 * @param name The name of the test (not quoted).
 */
#define TEST(name) PTEST(#name, (0, ~))

#ifndef IN_DOXYGEN

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

#define TESTSOON_PARAM__name(x)        0, x
#define TESTSOON_PARAM__fixture(x)     1, (1, x)

#define TESTSOON_PARAM_INITIAL \
  ("") ((0, ~))

#define TESTSOON_PARAM_INVOKEx(x) \
  PTEST x

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
#define equals(a, b) \
  ::testsoon::check_equals(a, b, "not equal: " #a " and " #b, __LINE__)

/**
 * Check that an expression throws.
 * If no exception is thrown, the test will fail.
 * @param x The expression to test.
 * @param t The exception type to check for.
 * @param w The expected value of caught_exception.what().
 */
#define throws(x, t, w) \
	do { \
		try { \
			(x); \
			::testsoon::fail("not throwed " #t, __LINE__); \
		} catch (t &e) { \
			if (::testsoon::test_string(e.what()) != ::testsoon::test_string((w))) \
				::testsoon::fail("throwed " #t " with wrong message", __LINE__); \
		} \
	} while (0)

/**
 * Check that an expression does not throw.
 * If a specified exception is thrown, the test will fail.
 * @param x The expression to test.
 * @param t The exception type to check for or "..." (without quotes).
 */
#define nothrows(x, t) \
	do { \
		try { \
			(x); \
		} catch (t) { \
			::testsoon::fail("throwed " #t, __LINE__); \
		} \
	} while (0)

} //namespace testsoon

#ifndef IN_DOXYGEN

inline static ::testsoon::test_group *
test_group(::testsoon::test_string const &filename) {
  static ::testsoon::test_file file(&::testsoon::tests(), filename);
  return &file;
}

#endif

/**
 * @mainpage
 * Explanation here.
 * See @ref tutorial
 *
 * @page tutorial Tutorial
 * This is a tutorial.
 */

#endif
