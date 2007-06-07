/*
ich arbeite gerade an einem Concept-Modell (habe alle concepts hier in policy umgetauft :))

Also eine Klasse gibt an, welche(s) Concept(e) sie genügt. zB

struct array {
  typedef concepts::random_access_container<concepts::interface> concept;
  typedef boost::mpl::vector<concepts::storage, concepts::allocator> require;
};

struct getter_interface {
  typedef concepts::interface concept;
  typedef concepts::single_holder require; // require_before/require_after ?!
};

struct container_requirements {
  typedef boost::mpl::vector<concepts::container, concepts::storage, concepts::interface> requirements;
};

typedef policy_holder<type<int>, array, pool_allocator, reserve<20> > int_array;

BOOST_STATIC_ASSERT(meets_requirements<int_array, container_requirements>::value);

dann könnte container einfach ein policy_holder sein, der das ASSERT bereits eingebaut hat.
*/

#ifndef PPTEST
#include <iostream>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/type_traits/is_same.hpp>
#endif
#include <boost/preprocessor.hpp>

#ifndef POLICY_LIMIT
#define POLICY_LIMIT 5
#endif

namespace detail {
  template<typename Sequence>
  struct nada {
    typedef Sequence policies;
  };

  template<typename Sequence, int Size>
  struct create_vector_impl;

  template<typename Sequence>
  struct create_vector {
    typedef
      typename create_vector_impl<Sequence, boost::mpl::size<Sequence>::value >::type
      type;
  };

  #define ELEMENT_AT(z, n, t) \
    BOOST_PP_COMMA_IF(n) \
    typename boost::mpl::at_c<Sequence, n>::type

  #define CREATE_VECTOR_IMPL(z, n, t) \
    template<typename Sequence> \
    struct create_vector_impl<Sequence, n> { \
      typedef BOOST_PP_CAT(boost::mpl::vector, n)< \
        BOOST_PP_REPEAT(n, ELEMENT_AT, ~) \
      > type; \
    };

  BOOST_PP_REPEAT(BOOST_PP_INC(POLICY_LIMIT), CREATE_VECTOR_IMPL, ~)

  #undef CREATE_VECTOR_IMPL
  #undef ELEMENT_AT

  template<typename Sequence, typename Before>
  struct iterate_policies {
    typedef Sequence sequence;
    typedef typename boost::mpl::front<sequence>::type first;

    typedef typename boost::mpl::pop_front<sequence>::type next_sequence_raw;
    typedef typename create_vector<next_sequence_raw>::type next_sequence;
    typedef typename first::template policy<Before, next_sequence> second;
  };

  template<typename, typename> struct next_policy;

  template<typename Sequence, typename Policy, typename Before = detail::nada<Sequence> >
  struct find_policy {
    typedef iterate_policies<Sequence, Before> it;
    typedef
      typename boost::mpl::eval_if<
        boost::is_same<Policy, typename it::first>,
        boost::mpl::identity<typename it::second>,
        next_policy<it, Policy>
      >::type type;
  };

  template<typename It, typename Policy>
  struct next_policy {
    typedef
      typename find_policy<
        typename It::next_sequence,
        Policy,
        typename It::second
      >::type type;
  };

  template<typename Sequence>
  struct combine {
    typedef typename create_vector<Sequence>::type sequence;
    typedef typename boost::mpl::back<sequence>::type back;
    typedef typename find_policy<sequence, back>::type type;
  };
}

template<typename Sequence>
struct policy_holder
  : detail::combine<Sequence>::type
{
  // typedef typename resulting_concept<Sequence>::concept concept;
};

template<typename T, typename H>
typename detail::find_policy<typename H::policies, T>::type &
as_policy(H &c) {
  return *static_cast<
      typename detail::find_policy<typename H::policies, T>::type *
    >(&c);
}

template<typename T, typename H>
typename detail::find_policy<typename H::policies, T>::type const &
as_policy(H const &c) {
  return *static_cast<
      typename detail::find_policy<typename H::policies, T>::type const *
    >(&c);
}

template<typename T, typename Policy>
struct with_policy
: boost::mpl::contains<typename T::policies, Policy>::type {};

template<typename Policy, typename T>
bool has_policy(T const &) {
  return with_policy<T, Policy>::value;
}

template<typename T>
struct type {
  template<typename Before, typename After>
  struct policy : public Before {
    typedef T value_type;
  };
};

template<typename Adapt>
struct adaptor {
  template<typename Before, typename After>
  struct policy : public Before, public Adapt { };
};

// EXAMPLE:
struct array {
  template<typename Before, typename After>
  struct policy : public Before {
    void push_back(typename Before::value_type const &) {
      std::cout << "PUSH_BACK\n";
    }
  };
};
struct simple_storage {  template<typename Before, typename After>  struct policy : public Before {  protected:    typename Before::value_type storage;  };};struct setter_interface {  template<typename Before, typename After>  struct policy : public Before {    void set(typename Before::value_type const &n) {
      Before::storage = n;
    }  };};

struct getter_x2_interface {  template<typename Before, typename After>  struct policy : public Before {    typename Before::value_type get() const {
      return Before::storage * 2;
    }  };};

struct getter_interface {  template<typename Before, typename After>  struct policy : public Before {    typename Before::value_type get() const {
      //return Before::storage;
      return as_policy<getter_x2_interface>(*this).get() / 2;
    }  };};int main() {
  std::cout << "_Z6__type" << typeid(detail::combine<boost::mpl::vector2<type<int>, array> >::type).name() << '\n';
  policy_holder<boost::mpl::vector2<type<int>, array> > foo;
  foo.push_back(10);

  policy_holder<boost::mpl::vector5<type<int>, simple_storage, setter_interface, getter_interface, getter_x2_interface> > bar;
  bar.set(99);  std::cout << bar.get() << '\n';

  std::cout << as_policy<getter_interface>(bar).get() << '\n';
  std::cout << as_policy<getter_interface>(as_policy<getter_x2_interface>(bar)).get() << '\n';
  std::cout << as_policy<getter_x2_interface>(as_policy<type<int> >(bar)).get() << '\n';

  std::cout << has_policy<type<double> >(bar) << ',' << has_policy<type<int> >(bar) << std::endl;

  //as_policy<type<double> >(bar); //funny error message

  std::cout << "sizes: " << sizeof(foo) << ", " << sizeof(bar) << std::endl;
}
