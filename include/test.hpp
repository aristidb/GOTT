#ifndef TEST_HPP
#define TEST_HPP

#include <boost/preprocessor/cat.hpp>

struct test_info;

#ifndef NO_STDLIB
#include <string>
#include <list>
#include <iostream>

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

static inline test_string test_group() { return std::string("/"); }

extern test_holder &tests();

inline bool operator<(test_info const &a, test_info const &b) {
  return a.function < b.function;
}

inline void fail(char const *msg, char const *file, unsigned line) {
	throw test_failure(msg, file, line);
}

template<class T, class U>
inline void check_equals(T const &a, U const &b,
                         char const *msg, char const *file, unsigned line) {
  if (!(a == b))
    fail(msg, file, line);
}

#define TEST_REGISTRY test_holder &tests() { static test_holder t; return t; }

#define TEST(name) \
  static void BOOST_PP_CAT(test_, __LINE__) (); \
  static test_info BOOST_PP_CAT(reg_, __LINE__) \
    (tests(), #name, __FILE__, __LINE__, test_group(), &BOOST_PP_CAT(test_, __LINE__)); \
  static void BOOST_PP_CAT(test_, __LINE__) ()

#define TEST_GROUP(name) \
    namespace BOOST_PP_CAT(name, _helper) { \
      static test_string upper_test_group() { return test_group(); } \
    } \
    namespace name { \
      static test_string test_group() { \
        return BOOST_PP_CAT(name, _helper)::upper_test_group() + (#name "/"); \
      }\
    }\
    namespace name





#define equals(a, b) \
  check_equals(a, b, "not equal: " #a " and " #b, __FILE__, __LINE__)
#define throws(x, t, w) \
	do { \
		try { \
			(x); \
			fail("not throwed " #t, __FILE__, __LINE__); \
		} catch (t &e) { \
			if (test_string(e.what()) != test_string((w))) \
				fail("throwed " #t " with wrong message", __FILE__, __LINE__); \
		} \
	} while (0)

#define nothrows(x, t) \
	do { \
		try { \
			(x); \
		} catch (t) { \
			fail("throwed " #t, __FILE__, __LINE__); \
		} \
	} while (0)


#endif
