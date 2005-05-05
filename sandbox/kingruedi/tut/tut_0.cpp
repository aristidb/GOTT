#include <tut.h>
#include <iostream>

template<typename T>
class test_me {
  T t;
public:
  typedef T value_type;

  test_me() { }
  test_me(value_type const &t_) : t(t_) { }

  value_type &get() { return t; }
};

namespace tut {
  struct test_me_data {
    typedef int test_type;
  };

  typedef test_group<test_me_data> tf;
  typedef tf::object object;
  tf test_me_group("test_me");

  template<>
  template<>
  void object::test<1>() {
    test_me<test_type> obj(10);

    ensure_equals("Ctor", obj.get(), 10);
  };

  template<>
  template<>
  void object::test<2>() {
    test_me<test_type> obj;

    obj.get()=10;

    ensure_equals("Ctor", obj.get(), 10);
  };

  template<>
  template<>
  void object::test<3>() {
    fail("this test sucks!");
  };

  test_runner_singleton runner;
}

class callback : public tut::callback {
public:
  void run_started() { std::cerr << "begin:\n"; }

  void test_completed(const tut::test_result& tr) {
    std::cerr << tr.test << " => ";
    switch(tr.result) {
    case tut::test_result::ok: {
      std::cerr << "Ok\n";
      break;
    }
    case tut::test_result::warn: {
      std::cerr << "Warn ('" << tr.message << "')\n";
      break;
    }
    case tut::test_result::fail: {
      std::cerr << "Failed ('" << tr.message << "')\n";
      break;
    }
    case tut::test_result::term: {
      std::cerr << "Terminated! ('" << tr.message << "')\n";
      break;
    }
    case tut::test_result::ex: {
      std::cerr << "Exception! (" << tr.exception_typeid << " - '"
		<< tr.message << "')\n";
      break;
    }
    }
  }

  void run_completed() { std::cerr << "end\n"; }
};

int main() {
  callback cbr;
  tut::runner.get().set_callback(&cbr);
  tut::runner.get().run_tests();
}
