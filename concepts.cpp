#include <iostream>
#include <boost/mpl/size.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>//das kommt aber bitte nich in greatcontainer
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/copy_if.hpp>
#include <boost/mpl/inserter.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/erase_key.hpp>

namespace mpl = boost::mpl;

namespace stl {
  namespace concepts {
    struct container { };
    struct random_access_container {
      typedef mpl::vector1<container> implies;
    };

    struct adapter { };
    struct sequence { };

    struct type_holder { };
  }

  struct vector {
    template<typename Before, typename After>
    struct policy : public Before { };

    typedef mpl::vector1<concepts::container> concept;
    typedef mpl::vector1<concepts::type_holder> require_before;
  };

  template<typename T>
  struct type {
    template<typename Before, typename After>
    struct policy : public Before {
      typedef T value_type; // ...
    };

    typedef mpl::vector1<concepts::type_holder> concept;
  };

  struct stack {
    template<typename Before, typename After>
    struct policy : protected Before { };

    typedef mpl::vector2<concepts::sequence, concepts::adapter> concept;
    typedef mpl::vector2<concepts::type_holder, concepts::container> require_before;
    typedef boost::mpl::true_ shadow;
  };
}

namespace concepts {
  struct container {
  };
  struct random_access_container {
    typedef mpl::vector1<container> implies;
  };

  struct foo { 
    typedef boost::mpl::false_ shadow;
  };
  struct bar {
    typedef mpl::vector1<random_access_container> implies;
    typedef boost::mpl::false_ shadow;
  };
  struct baz { typedef mpl::vector0< > implies; };

  struct shads {
    typedef mpl::vector1<random_access_container> implies;
    typedef boost::mpl::true_ shadow;
  };

  struct implies_shads {
    typedef mpl::vector1<shads> implies;
  };
}
using namespace concepts;


struct policy0 {
  template<typename Before, typename After>
  struct policy : public Before { };
  
  typedef mpl::vector2<container, foo> concept;
  /*  typedef mpl::vector<X> require;
  typedef mpl::vector<X> require_before;
  typedef mpl::vector<X> require_after;
  typedef mpl::vector<X> demote_concepts;*/
};

struct policy1 {
  template<typename Before, typename After>
  struct policy : public Before { };
  
  typedef mpl::vector1<bar> concept;
  /*typedef mpl::vector<container> require;
    typedef mpl::vector<X> require_before;
  typedef mpl::vector<X> require_after;
  typedef mpl::vector<X> demote_concepts;*/
};

struct policy2 {
  template<typename Before, typename After>
  struct policy : public Before { };
  
  typedef mpl::vector1<random_access_container> concept;
  /*typedef mpl::vector<container> require;
    typedef mpl::vector<X> require_before;
  typedef mpl::vector<X> require_after;
  typedef mpl::vector<X> demote_concepts;*/
};

struct policy3 {
  template<typename Before, typename After>
  struct policy : public Before { };
  
  typedef mpl::vector1<foo> concept;
  typedef mpl::vector1<container> require_before;
  /*  typedef mpl::vector<X> require_before;
  typedef mpl::vector<X> require_after;
  typedef mpl::vector<X> demote_concepts;*/
};

template<typename Sequence>
struct policy_holder {
  //
};

namespace utils {
  namespace detail {
    BOOST_MPL_HAS_XXX_TRAIT_DEF(implies)

    template<typename PolicySeq, typename Concept,
             bool empty = mpl::empty<PolicySeq>::value>
    struct checker {
      typedef typename mpl::front<PolicySeq>::type checked_concept;

      template<typename CheckedConcept, bool = has_implies<CheckedConcept>::value>
      struct implies {
        static bool const value =
          checker<typename CheckedConcept::implies, Concept>::value;
      };

      template<typename CheckedConcept>
      struct implies<CheckedConcept, false> {
        static bool const value = false;
      };

      static bool const value =
        boost::is_same<checked_concept, Concept>::value ?
        true :
        ( implies<checked_concept>::value ?
          true :
          checker<typename mpl::pop_front<PolicySeq>::type, Concept>::value);
    };

    template<typename PolicySeq, typename Concept>
    struct checker<PolicySeq, Concept, true> {
      static bool const value = false;
    };
  }

  template<typename PolicyClass, typename Concept>
  struct supports_concept
  : boost::integral_constant<bool, detail::checker<typename PolicyClass::concept, Concept>::value> 
  {};
  
  template<typename PolicyList, typename Concept>
  struct has_concept
  : boost::mpl::not_<
      boost::is_same<
        typename boost::mpl::find_if<
          PolicyList,
          supports_concept<boost::mpl::_1, Concept>
        >::type,
        typename boost::mpl::end<PolicyList>::type
      >
    >::type
  { };

  namespace detail {
    BOOST_MPL_HAS_XXX_TRAIT_DEF(require)
    BOOST_MPL_HAS_XXX_TRAIT_DEF(require_before)
    BOOST_MPL_HAS_XXX_TRAIT_DEF(require_after)

    template<
      typename Requirements,
      typename Sequence,
      bool empty = boost::mpl::empty<Requirements>::value
    >
    struct check_requirements
    : boost::mpl::and_<
        typename has_concept<
          Sequence,
          typename boost::mpl::front<Requirements>::type
        >::type,
        typename check_requirements<
          typename boost::mpl::pop_front<Requirements>::type,
          Sequence
        >::type
      >
    { };
    
    template<
      typename Requirements,
      typename Sequence
    >
    struct check_requirements<Requirements, Sequence, true>
    : boost::mpl::true_ { };

    template<typename Sequence, typename Iterator>
    struct check_requirements_all {
      typedef typename detail::check_requirements<
          typename boost::mpl::deref<Iterator>::type::require,
          Sequence
        >::type type;
    };

    template<typename Sequence, typename Iterator>
    struct check_requirements_before {
      typedef typename detail::check_requirements<
          typename boost::mpl::deref<Iterator>::type::require_before,
          boost::mpl::iterator_range<
            typename boost::mpl::begin<Sequence>::type,
            Iterator
          >
        >::type type;
    };

    template<typename Sequence, typename Iterator>
    struct check_requirements_after {
      typedef typename detail::check_requirements<
          typename boost::mpl::deref<Iterator>::type::require_after,
          boost::mpl::iterator_range<
            typename boost::mpl::next<Iterator>::type,
            typename boost::mpl::end<Sequence>::type
          >
        >::type type;
    };
  }

  template<typename Sequence,
           typename Iterator = typename boost::mpl::begin<Sequence>::type>
  struct check_concepts;
  
  namespace detail {
    template<typename Sequence, typename Iterator>
    struct check_concepts_impl
    {
      typedef
      boost::mpl::and_<
        check_concepts<
          Sequence,
          typename boost::mpl::next<Iterator>::type
        >,
        boost::mpl::eval_if<
          detail::has_require<
            typename boost::mpl::deref<Iterator>::type
          >,
          detail::check_requirements_all<Sequence, Iterator>,
          boost::mpl::true_
        >,
        boost::mpl::eval_if<
          detail::has_require_before<
            typename boost::mpl::deref<Iterator>::type
          >,
          detail::check_requirements_before<Sequence, Iterator>,
          boost::mpl::true_
        >,
        boost::mpl::eval_if<
          detail::has_require_after<
            typename boost::mpl::deref<Iterator>::type
          >,
          detail::check_requirements_after<Sequence, Iterator>,
          boost::mpl::true_
        >
      >
      type;
    };
  }

  template<typename Sequence,
           typename Iterator>
  struct check_concepts
  : boost::mpl::eval_if<
      boost::is_same<Iterator, typename boost::mpl::end<Sequence>::type>,
      boost::mpl::true_,
      detail::check_concepts_impl<Sequence, Iterator>
    >::type
  { };


  namespace detail {
    template<typename First, typename Last, typename Result>
    struct flatten {
      typedef typename boost::mpl::deref<First>::type deref;

      template<typename T>
      struct subflatten {
        typedef
          typename flatten<
            typename boost::mpl::begin<typename T::implies>::type,
            typename boost::mpl::end<typename T::implies>::type,
            boost::mpl::vector0<>
          >::type type;
      };

      typedef typename boost::mpl::insert_range<
          Result,
          typename boost::mpl::end<Result>::type,
          typename boost::mpl::eval_if<
            has_implies<deref>,
            subflatten<deref>,
            mpl::vector0<>
          >::type
        >::type with_implies;
      typedef typename boost::mpl::insert<
          with_implies,
          typename boost::mpl::end<with_implies>::type,
          deref
        >::type with_deref;

      typedef typename flatten<
          typename boost::mpl::next<First>::type,
          Last,
          with_deref
        >::type type;
    };

    template<typename First, typename Result>
    struct flatten<First, First, Result> {
      typedef Result type;
    };
  }

  template<typename Sequence>
  struct flatten {
    typedef typename detail::flatten<
        typename boost::mpl::begin<Sequence>::type,
        typename boost::mpl::end<Sequence>::type,
        boost::mpl::vector0<>
      >::type
    type;
  };

  namespace detail {
    BOOST_MPL_HAS_XXX_TRAIT_DEF(shadow)

    template<typename Concept, bool x = has_shadow<Concept>::value>
    struct is_shadowing
      : Concept::shadow
    { };

    template<typename Concept>
    struct is_shadowing<Concept, false>
      : boost::mpl::false_
    { };

    template<typename ConceptSeq,
             typename Iterator = typename boost::mpl::begin<ConceptSeq>::type>
    struct is_shadowing_concept_seq;

    template<typename Concept, bool x = has_implies<Concept>::value>
    struct is_shadowing_implies
      : is_shadowing_concept_seq<typename Concept::implies>
    { };

    template<typename Concept>
    struct is_shadowing_implies<Concept, false>
      : boost::mpl::false_
    { };

    template<typename ConceptSeq, typename Iterator>
    struct is_shadowing_concept_seq
    : boost::mpl::eval_if<
        is_shadowing<
          typename boost::mpl::deref<Iterator>::type
        >,
        boost::mpl::true_,
        boost::mpl::or_<
          typename is_shadowing_implies<
            typename boost::mpl::deref<Iterator>::type
          >::type,
          typename is_shadowing_concept_seq<
            ConceptSeq,
            typename boost::mpl::next<Iterator>::type
          >::type
        >
      >::type
    { };
    
    template<typename ConceptSeq>
    struct is_shadowing_concept_seq
           <
             ConceptSeq,
             typename boost::mpl::end<ConceptSeq>::type
           >
      : boost::mpl::false_
    { };

    template<typename Concept>
    struct is_shadowing_concept
      : boost::mpl::or_<
      typename is_shadowing<Concept>::type,
      typename is_shadowing_implies<Concept>::type
        >::type
    { };
    
    template<
      typename Sequence,
      typename Iterator = typename boost::mpl::begin<Sequence>::type
    >
    struct do_shadowing {
      typedef typename
        boost::mpl::eval_if<
          is_shadowing_concept<typename boost::mpl::deref<Iterator>::type>,
          do_shadowing<
            boost::mpl::iterator_range<
              Iterator,
              typename boost::mpl::end<Sequence>::type
            >,
            typename boost::mpl::next<Iterator>::type
          >,
          do_shadowing<
            Sequence,
            typename boost::mpl::next<Iterator>::type
          >
        >::type type;
    };
    
    template<typename Sequence>
    struct do_shadowing<
      Sequence,
      typename boost::mpl::end<Sequence>::type
    >
    {
      typedef Sequence type;
    };

    template<typename SeqA, typename SeqB>
    struct concat {
      typedef typename boost::mpl::insert_range<
          SeqA,
          typename boost::mpl::end<SeqA>::type,
          SeqB
        >::type type;
    };

    template<typename PolicySeq,
             bool empty = boost::mpl::empty<PolicySeq>::value >
    struct get_list_of_concepts {
      typedef typename concat<
          typename boost::mpl::front<PolicySeq>::type::concept,
          typename get_list_of_concepts<
            typename boost::mpl::pop_front<PolicySeq>::type
          >::type
        >::type
        type;
    };

    template<typename PolicySeq>
    struct get_list_of_concepts<PolicySeq, true> {
      typedef PolicySeq type;
    };
  }
  template<typename PolicySeq>
  struct resulting_concept {
    typedef typename flatten<
      typename detail::do_shadowing<
        typename detail::get_list_of_concepts<PolicySeq>::type
        >::type
      >::type
    type;
  };

  namespace detail {
    template<
      typename PolicySeq,
      typename Concept,
      bool no_policies_left = boost::mpl::empty<PolicySeq>::value
    >
    struct get_matching_policy {
      typedef typename boost::mpl::front<
          PolicySeq
         >::type front;
      typedef typename boost::mpl::eval_if<
          supports_concept<front, Concept>,
          boost::mpl::identity<front>,
          get_matching_policy<
            typename boost::mpl::pop_front<
              PolicySeq
            >::type,
            Concept
          >
        >::type type;
    };

    template<
      typename PolicySeq,
      typename Concept
    >
    struct get_matching_policy<
      PolicySeq,
      Concept,
      true
    >;

    template<
      typename Requirements,
      typename PolicySeq,
      typename Policy,
      typename Graph,
      template<typename, typename> class PairMaker,
      bool no_requirements_left = boost::mpl::empty<Requirements>::value
    >
    struct policy_graph {
      typedef typename PairMaker<
          Policy,
          typename get_matching_policy<
            PolicySeq,
            typename boost::mpl::front<Requirements>::type
          >::type
        >::type pair;
      typedef boost::mpl::pair<
          typename pair::first,
          typename boost::mpl::insert<
            typename boost::mpl::at<Graph, typename pair::first>::type,
            typename pair::second
          >::type
        > real_pair;
      typedef typename boost::mpl::insert<
          Graph,
          real_pair
        >::type graph;
      typedef typename policy_graph<
          typename boost::mpl::pop_front<Requirements>::type,
          PolicySeq,
          Policy,
          graph,
          PairMaker
        >::type type;
    };

    template<
      typename Requirements,
      typename PolicySeq,
      typename Policy,
      typename Graph,
      template<typename, typename> class PairMaker
    >
    struct policy_graph<
      Requirements,
      PolicySeq,
      Policy,
      Graph,
      PairMaker,
      true
    > {
      typedef Graph type;
    };

    template<typename From, typename To>
    struct before_pair {
      typedef boost::mpl::pair<From, To> type;
    };

    template<typename To, typename From>
    struct after_pair {
      typedef boost::mpl::pair<From, To> type;
    };

    template<
      typename PolicySeq,
      typename Policy,
      typename Graph,
      bool has_require_before = has_require_before<Policy>::value
    >
    struct policy_graph_before {
      typedef typename policy_graph<
          typename Policy::require_before,
          PolicySeq,
          Policy,
          Graph,
          before_pair
        >::type type;
    };

    template<typename PolicySeq, typename Policy, typename Graph>
    struct policy_graph_before<PolicySeq, Policy, Graph, false> {
      typedef Graph type;
    };

    template<
      typename PolicySeq,
      typename Policy,
      typename Graph,
      bool has_require_after = has_require_after<Policy>::value
    >
    struct policy_graph_after {
      typedef typename policy_graph<
          typename Policy::require_after,
          PolicySeq,
          Policy,
          Graph,
          after_pair
        >::type type;
    };

    template<typename PolicySeq, typename Policy, typename Graph>
    struct policy_graph_after<PolicySeq, Policy, Graph, false> {
      typedef Graph type;
    };

    template<
      typename PolicySeq,
      bool empty = boost::mpl::empty<PolicySeq>::value
    >
    struct init_graph {
      typedef typename boost::mpl::insert<
          typename init_graph<
            typename boost::mpl::pop_front<PolicySeq>::type
          >::type,
          boost::mpl::pair<
            typename boost::mpl::front<PolicySeq>::type,
            boost::mpl::set0<>
          >
        >::type type;
    };

    template<typename PolicySeq>
    struct init_graph<PolicySeq, true> {
      typedef boost::mpl::map0<> type;
    };

    template<
      typename PolicySeq,
      typename Graph = typename init_graph<PolicySeq>::type,
      bool = boost::mpl::empty<PolicySeq>::value
    >
    struct order_graph {
      typedef typename boost::mpl::front<PolicySeq>::type policy;

      typedef typename policy_graph_before<
          PolicySeq,
          policy,
          Graph
        >::type before;

      typedef typename policy_graph_after<
          PolicySeq,
          policy,
          before
        >::type after;

      typedef typename order_graph<
          typename boost::mpl::pop_front<PolicySeq>::type,
          after
        >::type type;
    };

    template<typename PolicySeq, typename Graph>
    struct order_graph<PolicySeq, Graph, true> {
      typedef Graph type;
    };

    template<
      typename Q,
      typename N,
      typename Graph
    >
    struct next_level { //(5)
      //TODO
    };

    template<
      typename Q,
      typename Graph,
      typename Result = boost::mpl::vector0<>,
      bool empty = boost::mpl::empty<Q>::value
    >
    struct reorder {  //(2)
      //(3)
      typedef typename boost::mpl::front<Q>::type n;
      typedef typename boost::mpl::erase_key<Q, n>::type Q2;
      //(4)
      typedef typename boost::mpl::push_back<Result, n>::type output;
      //(5)
      typedef next_level<Q2, n, Graph> next;
      //(2)
      typedef typename reorder<
          typename next::Q,
          typename next::graph,
          output
        >::type type;
    };

    template<typename Q, typename Graph, typename Result>
    struct reorder<Q, Graph, Result, true> { //(2)
      BOOST_STATIC_ASSERT(boost::mpl::empty<Graph>::value); //(9,10)
      typedef Result type;
    };
  }

  /*
  Q := Set of all nodes with no incoming edges        //(1)
  while Q is non-empty do                             //(2)
      remove a node n from Q                          //(3)
      output n                                        //(4)
      for each node m with an edge e from n to m do   //(5)
          remove edge e from the graph                //(6)
          if m has no other incoming edges then       //(7)
              insert m into Q                         //(8)
  if graph has edges then                             //(9)
      output error message (graph has a cycle)        //(10)
  */

  template<typename PolicySeq>
  struct reorder {
    typedef typename detail::order_graph<PolicySeq>::type graph;

    //(1)
    typedef typename boost::mpl::copy_if<
        PolicySeq,
        boost::mpl::empty<
          boost::mpl::at<
            graph,
            boost::mpl::_1
          >
        >,
        boost::mpl::inserter<
          boost::mpl::set0<>,
          boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
        >
      >::type Q;

    //(2)
    typedef typename detail::reorder<Q, graph>::type type;
  };

// ----
// create_vector from greatcontainer3
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

  BOOST_PP_REPEAT(20, CREATE_VECTOR_IMPL, ~)

  #undef CREATE_VECTOR_IMPL
  #undef ELEMENT_AT
// ----
}

namespace tests {
  template<typename Seq, typename ShadowConcept>
  void test_do_shadowing() {
    typedef typename utils::detail::do_shadowing<Seq>::type results;

    std::cout << "shadowing: " << boost::mpl::size<results>::value << '\n';
    BOOST_STATIC_ASSERT((boost::is_same<
                         typename boost::mpl::front<results>::type,
                         ShadowConcept>::value));
  }

  template<typename Seq, typename Has, bool expect>
  void has_concept() {
    std::cout << "has_concept\n";
    BOOST_STATIC_ASSERT((utils::has_concept<Seq, Has>::value == expect));
  }

  template<typename Policy, typename Concept, bool expect>
  void supports_concept() {
    std::cout << "supports_concept\n";
    BOOST_STATIC_ASSERT((utils::supports_concept<Policy, Concept>::value
                         == expect));
  }

  template<typename Seq, bool expect>
  void check_concepts() {
    std::cout << "check_concepts\n";
    BOOST_STATIC_ASSERT((utils::check_concepts<Seq>::value == expect));
  }

  template<typename Seq>
  void flatten() {
    typedef typename utils::flatten<Seq>::type result;
    std::cout << "flatten: " << boost::mpl::size<result>::value << '\n';
  }

  template<typename PolicySeq>
  void resulting_concept() {
    typedef typename utils::resulting_concept<PolicySeq>::type result_;
    typedef typename utils::create_vector<result_>::type result;
    std::cout << "resulting concept: " << boost::mpl::size<result>::value;
    std::cout << " _Z1x" << typeid(result).name() << '\n';
  }
}

int main() {
  tests::supports_concept<policy1, concepts::container, true>();

  tests::has_concept<mpl::vector2<policy3, policy1>, concepts::container,
    true>();
  tests::has_concept<mpl::vector0<>, concepts::container, false>();

  tests::check_concepts<mpl::vector2<policy3, policy1>, false>();
  tests::check_concepts<mpl::vector0< >, true>();
  tests::check_concepts<mpl::vector2<policy1, policy3>, true>();

  tests::test_do_shadowing<mpl::vector4<bar, foo, shads, baz>, shads>();
  tests::test_do_shadowing<mpl::vector4<bar, foo, implies_shads, baz>,
    implies_shads>();
  tests::test_do_shadowing<mpl::vector0< >, boost::mpl::void_>();

  tests::flatten<mpl::vector2<bar, bar> >();
  tests::flatten<mpl::vector0< > >();
  std::cout << "_Z1x" << typeid(utils::create_vector<utils::flatten<mpl::vector2<bar, bar> >::type>::type).name() << '\n';

  tests::resulting_concept<mpl::vector2<policy1, policy3> >();
  
  typedef mpl::vector3<stl::stack, stl::vector, stl::type<int> > c3;

  std::cout << "_Z1a" << typeid(utils::detail::order_graph<c3>::type).name() << '\n';
  std::cout << boost::mpl::size<utils::detail::order_graph<c3>::type>::value << '\n';
  /*
  stl::vector -> (stl::type<int>)
  stl::stack -> (stl::vector, stl::type<int>)
  stl::stack -> (stl::type<int>) --???
  stl::stack -> ()
  stl::vector -> ()
  stl::type<int> -> ()
  */
  //std::cout << "_Z1b" << typeid(utils::reorder<c3>::type).name() << '\n';

  //std::cout << "_Z1x" << typeid(utils::apply_default_policies<mpl::vector2<policy1, policy3> >::type).name() << '\n';
}
