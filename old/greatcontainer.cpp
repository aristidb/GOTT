// hier wird lustig gebastelt, erstmal ohne unterteilung in header

#ifndef PPTEST
#include <cstddef>
#include <algorithm>
#include <iostream>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>
#endif
#include <boost/preprocessor.hpp>

#ifndef POLICY_LIMIT
#define POLICY_LIMIT 15
#endif

namespace detail {

template<class Container, class Policy, int N>
struct get_policy {
  typedef typename Policy::template policy<Container>::type type;
};

template<class Container, int N>
struct get_policy<Container, void, N> {
  struct type {};
};

struct dummy {};

}

#define PL \
  BOOST_PP_ENUM_BINARY_PARAMS( \
    POLICY_LIMIT, \
    typename Policy, \
    = void BOOST_PP_INTERCEPT \
  )

#define PLE \
  BOOST_PP_ENUM_PARAMS( \
    POLICY_LIMIT, \
    Policy \
  )

#define BASE(z,n,data) \
  public detail::get_policy<container<T, PLE>, BOOST_PP_CAT(Policy, n), n>::type,

template<typename T, PL>
class container
:
  BOOST_PP_REPEAT(POLICY_LIMIT, BASE, ~)
  detail::dummy
{
private:
  typedef container<T, PLE> self;

  template<class Value>
  class iter
    : public boost::iterator_facade<
        iter<Value>,
        Value,
        boost::forward_traversal_tag
      >
  {
    struct enabler {};
  public:
    typedef typename self::storage_type storage_type;
    typedef typename storage_type::internal_iterator internal_iterator;
    iter() {}
    explicit iter(internal_iterator pos) : pos(pos) {}

    template<class OtherValue>
    iter(
      iter<OtherValue> const& other,
      typename boost::enable_if<
        boost::is_convertible<OtherValue*,Value*>,
        enabler
      >::type = enabler()
    ) : pos(other.pos) {}

  private:
    friend class boost::iterator_core_access;

    void increment() {
      pos = storage_type::it_next(pos);
    }

    bool equal(iter const &other) const {
      return storage_type::it_equals(pos, other.pos);
    }

    Value &dereference() const {
      return storage_type::it_dereference(pos);
    }

    internal_iterator pos;
  };

public:
  typedef T value_type;
  typedef T &reference_type;
  typedef T const &const_reference_type;
  typedef T *pointer;
  typedef T const *const_pointer;
  typedef std::ptrdiff_t difference_type;
  typedef std::size_t size_type;
  typedef iter<T> iterator;
  typedef iter<T const> const_iterator;

  iterator begin() {
    return iterator(storage.begin());
  }

  const_iterator begin() const {
    return const_iterator(storage.begin());
  }

  iterator end() {
    return iterator(storage.end());
  }

  const_iterator end() const {
    return const_iterator(storage.end());
  }

private:
  typename self::storage_type storage;
};

template<typename T, BOOST_PP_ENUM_PARAMS(POLICY_LIMIT, typename Policy)>
bool operator==(container<T, PLE> const &x, container<T, PLE> const &y) {
  typedef typename container<T, PLE>::const_iterator iterator;
  iterator i = x.begin();
  iterator j = y.begin();
  for (; i != x.end() && j != y.end(); ++i, ++j)
    if (*i != *j)
      return false;
  return i == x.end() && j == y.end();
}

template<typename T, BOOST_PP_ENUM_PARAMS(POLICY_LIMIT, typename Policy)>
bool operator!=(container<T, PLE> const &x, container<T, PLE> const &y) {
  return !(x == y);
}

template<typename T, BOOST_PP_ENUM_PARAMS(POLICY_LIMIT, typename Policy)>
bool operator<(container<T, PLE> const &x, container<T, PLE> const &y) {
  return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<typename T, BOOST_PP_ENUM_PARAMS(POLICY_LIMIT, typename Policy)>
bool operator<=(container<T, PLE> const &x, container<T, PLE> const &y) {
  return !(y < x);
}

template<typename T, BOOST_PP_ENUM_PARAMS(POLICY_LIMIT, typename Policy)>
bool operator>(container<T, PLE> const &x, container<T, PLE> const &y) {
  return y < x;
}

template<typename T, BOOST_PP_ENUM_PARAMS(POLICY_LIMIT, typename Policy)>
bool operator>=(container<T, PLE> const &x, container<T, PLE> const &y) {
  return !(x < y);
}

#undef PL
#undef PLE
#undef BASE

namespace detail {

template<typename T>
struct container_types {
  typedef typename T::value_type value_type;
};

template<typename T, BOOST_PP_ENUM_PARAMS(POLICY_LIMIT, typename Policy)>
struct container_types<container<T, BOOST_PP_ENUM_PARAMS(POLICY_LIMIT, Policy)> > {
  typedef T value_type;
};

}

namespace detail {

template<class T>
struct sll_storage {
  typedef T value_type;
  typedef T &reference;
  typedef T const &const_reference;

  struct node_base {
    node_base(node_base *next = 0)
    : next(next) {}
    node_base *next;
  };

  struct node : node_base {
    node(value_type const &value, node_base *next = 0)
    : node_base(next), value(value) {}
    value_type value;
  };

  typedef node_base *internal_iterator;

  static node_base *it_next(node_base *p) {
    return p->next;
  }

  static bool it_equals(node_base *x, node_base *y) {
    return x == y;
  }

  static reference it_dereference(node_base *p) {
    return *p;
  }

  node_base *begin() const {
    return dummy_head.next;
  }

  node_base *end() const {
    return 0;
  }

  bool empty() const {
    return !dummy_head->next;
  }

  std::size_t size() {
    std::size_t result = 0;
    for (node_base *p = dummy_head.next; p; p = p->next)
      ++result;
    return result;
  }

  const_reference front() const {
    node *head = static_cast<node *>(dummy_head.next);
    return head->value;
  }

  void push_front(value_type const &x) {
    insert_after(dummy_head, x);
  }

  void pop_front() {
    node *head = static_cast<node *>(dummy_head.next);
    dummy_head->next = head->next;
    delete head;
  }

  void insert(node_base *e, value_type const &x) {
    node_base *p = &dummy_head;
    while (p->next != e)
      p = p->next;
    insert_after(p, x);
  }

  void insert_after(node_base *p, value_type const &x) {
    node_base *n = new node(x);
    n->next = p->next;
    p->next = n;
  }

  node_base *erase_after(node_base *p) {
    node_base *d = p->next;
    p->next = d->next;
    delete d;
    return p;
  }

  void erase_after(node_base *before_first, node_base *last) {
    while (before_first->next != last)
      before_first = erase_after(before_first);
  }

  node_base dummy_head;
};

}

struct singly_linked_list {
  template<typename Container>
  struct policy {
    class type {
    protected:
      typedef detail::sll_storage<typename detail::container_types<Container>::value_type> storage_type;
    };
  };
};

struct cache_size {
  template<typename Container>
  struct policy {
    class type {
    public:
      std::size_t size() {
        if (!cached)
          cached_size = static_cast<Container *>(this)->calculate_size();
        return cached_size;
      }

    private:
      bool cached;
      std::size_t cached_size;
    };
  };
};

struct tp {
  tp() {
    std::cout << "hi" << std::endl;
  }

  void say_hi2() {
    std::cout << "hi2" << std::endl;
  }
};

struct test_policy {
  template<typename>
  struct policy {
    typedef tp type;
  };
};

struct xyz_policy {
  template<typename>
  struct policy {
    struct type {}; // so, jetzt macht die basisklasse nix
    
    // hier
  };

  // und hier
  // können tolle sachen rein
};

int main() {
  container<int, singly_linked_list, cache_size, test_policy> list;
/*
  list.push_front(4);
  list.push_front(5);
  std::cout << list.size() << ": " << list.front() << std::endl;
  list.pop_front();
  std::cout << list.size() << ": " << list.front() << std::endl;
  list.pop_front();
  std::cout << std::endl;
*/
  list.push_front(10);
  container<int, singly_linked_list, cache_size, test_policy>::iterator it = list.begin();
  std::cout << (it == list.end()) << std::endl;
  list.say_hi2();
}
