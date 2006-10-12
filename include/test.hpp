#ifndef TEST_HPP
#define TEST_HPP

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/replace.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#ifndef NO_STDLIB
#include <string>
#include <list>
#include <iostream>
#endif

namespace test_soon {

struct test_info;

#ifndef NO_STDLIB
typedef std::string test_string;
typedef std::list<test_info> test_info_list;
#endif

class test_reporter;

struct test_holder {
  test_info_list tests;

  void run(test_reporter &);
  void add(test_info const &info);
};

struct test_info {
  test_info(
            test_holder &tests,
            char const *name, char const *file, unsigned line,
            test_string const &group, void (*function)())
    : name(name), file(file), line(line), group(group), function(function) {
    tests.add(*this);
  }
  char const *name;
  char const *file;
  unsigned line;
  test_string group;
  void (*function)();
};

class test_failure {
public:
  test_failure(test_string const &message, char const *file, unsigned line)
    : message(message), file(file), line(line) {}
  ~test_failure() {}
  test_string message;
  char const *file;
  unsigned line;
};

class test_reporter {
public:
  virtual void success(test_info const&)=0;
  virtual void failure(test_info const&,test_failure const&)=0;
  virtual ~test_reporter() {}
};

template<class stream = std::ostream>
struct default_reporter : test_reporter {
  default_reporter(stream &out = std::cout) : out(out) {}
  void tell_test(test_info const &it, char const *state) {
    out << '"' << it.name << "\", "
        << it.file << ", "
        << it.line << ", "
        << it.group << ", "
        //<< (void*)it.function << " " //DAS hier stört -pedantic 
        << state << std::endl;
  }
  void success(test_info const &it){
    tell_test(it,"success");
  }
  void failure(test_info const &it, test_failure const &fail){
    tell_test(it,"fail");
    out << "Reason: " << fail.message << std::endl;
  }
private:
  stream &out;
};

inline void test_holder::run(test_reporter &reporter) {
  for (std::list<test_info>::iterator it = tests.begin(); it != tests.end(); ++it) {
    try {
      it->function();
      reporter.success(*it);
    } catch (test_failure const &x) {
      reporter.failure(*it, x);
    }
  }
}

inline void test_holder::add(test_info const &info) {
  tests.push_back(info);
}

extern test_holder &tests();

inline bool operator<(test_info const &a, test_info const &b) {
  return a.function < b.function;
}

///@internal
inline void fail(char const *msg, char const *file, unsigned line) {
	throw test_failure(msg, file, line);
}

///@internal
template<class T, class U>
inline void check_equals(T const &a, U const &b,
                         char const *msg, char const *file, unsigned line) {
  if (!(a == b))
    fail(msg, file, line);
}

/**
 * Add this macro to exactly one source file to ensure proper instantiation.
 */
#define TEST_REGISTRY \
  namespace test_soon { \
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
      static ::test_soon::test_string upper_test_group() { return test_group(); } \
    } \
    namespace name { \
      static ::test_soon::test_string test_group() { \
        return BOOST_PP_CAT(name, _helper)::upper_test_group() + (#name "/"); \
      }\
    }\
    namespace name

/**
 * Declare a test (positional).
 * \param name The name of the test (quoted).
 */
#define PTEST(name) \
  static void BOOST_PP_CAT(test_, __LINE__) (); \
  static ::test_soon::test_info BOOST_PP_CAT(reg_, __LINE__) \
    (::test_soon::tests(), name, __FILE__, __LINE__, test_group(), &BOOST_PP_CAT(test_, __LINE__)); \
  static void BOOST_PP_CAT(test_, __LINE__) ()

/**
 * Declare a test (optional name only).
 * \param name The name of the test (not quoted).
 */
#define TEST(name) PTEST(#name)

#ifndef IN_DOXYGEN

#define TEST_SOON_GEN_TUPLE2SEQ_PROCESS2(x, y) \
  ((x, y)) \
  TEST_SOON_GEN_TUPLE2SEQ_PROCESS

#define TEST_SOON_GEN_TUPLE2SEQ_PROCESS(x, y) \
  ((x, y)) \
  TEST_SOON_GEN_TUPLE2SEQ_PROCESS2

#define TEST_SOON_GEN_TUPLE2SEQ_PROCESS_ELIM
#define TEST_SOON_GEN_TUPLE2SEQ_PROCESS2_ELIM

#define TEST_SOON_GEN_TUPLE2SEQ(x) \
  BOOST_PP_CAT(TEST_SOON_GEN_TUPLE2SEQ_PROCESS x, _ELIM)

#define TEST_SOON_PARAM_CHANGES(x) \
  ((0, BOOST_PP_SEQ_ELEM(0, TEST_SOON_PARAM_INITIAL))) \
  BOOST_PP_SEQ_FOR_EACH( \
    TEST_SOON_PARAM_EXPAND, \
    ~, \
    TEST_SOON_GEN_TUPLE2SEQ(x))

#define TEST_SOON_PARAM_EXPAND(r, d, e) \
  TEST_SOON_PARAM_EXPAND2 e

#define TEST_SOON_PARAM_EXPAND2(x, y) \
  BOOST_PP_CAT(TEST_SOON_PARAM__, x)(y)

#define TEST_SOON_PARAM__name(x) ((0, x))

#define TEST_SOON_PARAM_INITIAL \
  ("")

#define TEST_SOON_PARAM_COMBINE(s, state, x) \
  BOOST_PP_SEQ_REPLACE( \
    state, \
    BOOST_PP_TUPLE_ELEM(2, 0, x), \
    BOOST_PP_TUPLE_ELEM(2, 1, x))

#define TEST_SOON_PARAM_INVOKE2(x) \
  BOOST_PP_SEQ_TO_TUPLE( \
    BOOST_PP_SEQ_FOLD_LEFT( \
      TEST_SOON_PARAM_COMBINE, \
      TEST_SOON_PARAM_INITIAL, \
      TEST_SOON_PARAM_CHANGES(x)))

#define TEST_SOON_PARAM_INVOKEx(x) \
  PTEST x

#define TEST_SOON_PARAM_INVOKE(x) \
  TEST_SOON_PARAM_INVOKEx(TEST_SOON_PARAM_INVOKE2(x))

#endif //IN_DOXY

/**
 * Declare a test (named parameters).
 * \param name The name of the test (quoted).
 */
#define XTEST(named_parameter_sequence) \
  TEST_SOON_PARAM_INVOKE(named_parameter_sequence)

/**
 * Check whether two values are equal.
 * If both values are not equal, the test will fail.
 * \param a Some value.
 * \param b Another value.
 */
#define equals(a, b) \
  ::test_soon::check_equals(a, b, "not equal: " #a " and " #b, __FILE__, __LINE__)

/**
 * Check that an expression throws.
 * If no exception is thrown, the test will fail.
 * \param x The expression to test.
 * \param t The exception type to check for.
 * \param w The expected value of caught_exception.what().
 */
#define throws(x, t, w) \
	do { \
		try { \
			(x); \
			::test_soon::fail("not throwed " #t, __FILE__, __LINE__); \
		} catch (t &e) { \
			if (::test_soon::test_string(e.what()) != ::test_soon::test_string((w))) \
				::test_soon::fail("throwed " #t " with wrong message", __FILE__, __LINE__); \
		} \
	} while (0)

/**
 * Check that an expression does not throw.
 * If a specified exception is thrown, the test will fail.
 * \param x The expression to test.
 * \param t The exception type to check for or "..." (without quotes).
 */
#define nothrows(x, t) \
	do { \
		try { \
			(x); \
		} catch (t) { \
			::test_soon::fail("throwed " #t, __FILE__, __LINE__); \
		} \
	} while (0)

}

static inline ::test_soon::test_string test_group() { return std::string("/"); }

#endif
