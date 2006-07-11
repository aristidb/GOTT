#include <iostream>

struct null_type;

struct lambda_base {};

template<int i>
struct placeholder_t {};

template<>
struct placeholder_t<1> : lambda_base {
  template<class A1, class A2>
    struct sig {
      typedef null_type reference;
      typedef A1 type;
    };

  template<class A1>
  typename sig<A1, null_type>::type
  operator() (A1 const &a1) const {
    return a1;
  }

  template<class A1, class A2>
  typename sig<A1, A2>::type
  operator() (A1 const &a1, A2 const &a2) const {
    return a1;
  }
};

template<>
struct placeholder_t<2> : lambda_base {
  template<class A1, class A2 = null_type>
    struct sig {
      typedef A2 type;
    };

  template<class A1, class A2>
  typename sig<A1, A2>::type
  operator() (A1 const &a1, A2 const &a2) const {
    return a2;
  }
};

template<class A1, class A2>
struct plus_sig {
  typedef typeof(*(A1*)0 + *(A2*)0) type;
};

template<class A2>
struct plus_sig<null_type, A2> {
  typedef null_type type;
};

template<class A1>
struct plus_sig<A1, null_type> {
  typedef null_type type;
};

template<>
struct plus_sig<null_type, null_type> {
  typedef null_type type;
};

template<class X1, class X2>
struct plus_t : lambda_base {
  plus_t(X1 const &x1, X2 const &x2) : x1(x1), x2(x2) {}

  template<class A1, class A2>
  struct sig {
    typedef typename plus_sig<
          typename X1::template sig<A1, A2>::type,
          typename X2::template sig<A1, A2>::type>::type
      type;
  };

  typename sig<null_type, null_type>::type
  operator() () const {
    return x1() + x2();
  }

  template<class A1>
  typename sig<A1, null_type>::type
  operator() (A1 const &a1) const {
    return x1(a1) + x2(a1);
  }

  template<class A1, class A2>
  typename sig<A1, A2>::type
  operator() (A1 const &a1, A2 const &a2) const {
    return x1(a1, a2) + x2(a1, a2);
  }

  X1 x1;
  X2 x2;
};

template<class X1, class X2>
struct comma_t {
  comma_t(X1 const &x1, X2 const &x2) : x1(x1), x2(x2) {}

  template<class A1, class A2>
  struct sig {
    typedef typename X2::template sig<A1, A2>::type type;
  };

  typename sig<null_type, null_type>::type
  operator() () const {
    x1();
    return x2();
  }

  template<class A1>
  typename sig<A1, null_type>::type
  operator() (A1 const &a1) const {
    x1(a1);
    return x2(a1);
  }

  template<class A1, class A2>
  typename sig<A1, A2>::type
  operator() (A1 const &a1, A2 const &a2) const {
    x1(a1, a2);
    return x2(a1, a2);
  };

  X1 x1;
  X2 x2;
};

template<class X1, class X2>
struct assign_t {
  assign_t(X1 const &x1, X2 const &x2) : x1(x1), x2(x2) {}

  template<class A1, class A2>
  struct sig {
    typedef typename X1::template sig<A1, A2>::type type;
    typedef typename X1::template sig<A1, A2>::reference reference;
  };

  typename sig<null_type, null_type>::type
  operator() () const {
    return x1.reference() = x2();
  }

  template<class A1>
  typename sig<A1, null_type>::type
  operator() (A1 const &a1) const {
    return x1.reference(a1) = x2(a1);
  }

  template<class A1, class A2>
  typename sig<A1, A2>::type
  operator() (A1 const &a1, A2 const &a2) const {
    return x1.reference(a1, a2) = x2(a1, a2);
  }

  template<class A1, class A2>
  typename sig<A1, A2>::reference
  reference(A1 const &a1, A2 const &a2) const {
    return x1.reference(a1, a2) = x2(a1, a2);
  }

  X1 x1;
  X2 x2;
};

template<class T>
struct var_t {
  var_t(T &v) : v(v) {}

  template<class A1, class A2>
    struct sig {
      typedef T type;
      typedef T &reference;
    };

  typename sig<null_type, null_type>::type
  operator() () const {
    return v;
  }

  template<class A1>
  typename sig<A1, null_type>::type
  operator() (A1 const &a1) const {
    return v;
  }

  template<class A1, class A2>
  typename sig<A1, A2>::type
  operator() (A1 const &a1, A2 const &a2) const {
    return v;
  }

  typename sig<null_type, null_type>::reference
  reference() const {
    return v;
  }

  template<class A1>
  typename sig<A1, null_type>::reference
  reference(A1 const &a1) const {
    return v;
  }

  template<class A1, class A2>
  typename sig<A1, A2>::reference
  reference(A1 const &a1, A2 const &a2) const {
    return v;
  }

  T &v;
};

template<class T>
struct lambda {
  lambda(T const &real = T()) : real(real) {}

  template<class A1, class A2>
  struct sig {
    typedef typename T::template sig<A1, A2>::type type;
  };

  template<class X2>
  lambda<assign_t<T, X2> >
  operator=(lambda<X2> const &a2) const {
    return lambda<assign_t<T, X2> >(assign_t<T, X2>(real, a2.real));
  }

  typename sig<null_type, null_type>::type
  operator() () const {
    return real();
  }

  template<class A1>
  typename sig<A1, null_type>::type
  operator() (A1 const &a1) const {
    return real(a1);
  }

  template<class A1, class A2>
  typename sig<A1, A2>::type
  operator() (A1 const &a1, A2 const &a2) const {
    return real(a1, a2);
  }

  T real;
};

template<class X1, class X2>
lambda<plus_t<X1, X2> >
operator+(lambda<X1> const &a1, lambda<X2> const &a2) {
  return lambda<plus_t<X1, X2> >(plus_t<X1, X2>(a1.real, a2.real));
}

template<class X1, class X2>
lambda<comma_t<X1, X2> >
operator,(lambda<X1> const &a1, lambda<X2> const &a2) {
  return lambda<comma_t<X1, X2> >(comma_t<X1, X2>(a1.real, a2.real));
}

template<class T>
lambda<var_t<T> >
var(T &v) {
  return lambda<var_t<T> >(var_t<T>(v));
}

template<class F>
void four_times(F f) {
  f();
  f();
  f();
  f();
}

int main() {
  lambda<placeholder_t<1> > _1;
  lambda<placeholder_t<2> > _2;
  
  using namespace std;

  cout << (_1,_1+_2)((_1 + _2 + _1 + _1)(2, 6),-6) << endl;
  int x = 2;
  four_times(var(x) = var(x) + var(x));
  cout << x << endl;
}
