#include <iostream>
#include <boost/mpl/size.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>//das kommt aber bitte nich in greatcontainer
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/insert_range.hpp>

namespace mpl = boost::mpl;

namespace concepts {
  struct container {
    typedef mpl::vector0< > implies;
  };
  struct random_access_container {
    typedef mpl::vector1<container> implies;
  };

  struct foo { 
    typedef mpl::vector0< > implies;
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
  namespace details {
    template<typename PolicySeq, typename Concept,
             bool empty = mpl::empty<PolicySeq>::value>
    struct checker {
      typedef typename mpl::front<PolicySeq>::type checked_concept;

      static bool const value =
        boost::is_same<checked_concept, Concept>::value ?
        true :
        ( checker<typename checked_concept::implies, Concept>::value ?
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
  : boost::integral_constant<bool, details::checker<typename PolicyClass::concept, Concept>::value> 
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

  template<typename Sequence, typename Iterator>
  struct check_concepts
  : boost::mpl::eval_if<
      boost::is_same<Iterator, typename boost::mpl::end<Sequence>::type>,
      boost::mpl::true_,
      check_concepts_impl<Sequence, Iterator>
    >::type
  { };


  namespace detail {
    BOOST_MPL_HAS_XXX_TRAIT_DEF(implies);

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
          Result,
          typename boost::mpl::end<Result>::type,
          deref
        >::type with_deref;

      typedef typename boost::mpl::next<First>::type next;
      typedef typename boost::mpl::eval_if<
          boost::is_same<next, Last>,
          with_deref,
          flatten<next, Last, with_deref>
        >::type type;
    };

    BOOST_MPL_HAS_XXX_TRAIT_DEF(shadow);

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

    template<typename SeqA, typename SeqB,
             bool empty = boost::mpl::empty<SeqA>::value>
    struct concat {
      typedef typename boost::mpl::push_front<
        typename concat<
          typename boost::mpl::pop_front<SeqA>::type,
          SeqB
        >::type,
        typename boost::mpl::front<SeqA>::type
        >::type type;
    };

    template<typename SeqA, typename SeqB>
    struct concat<SeqA, SeqB, true> {
      typedef SeqB type;
    };

    template<typename PolicySeq,
             bool empty = boost::mpl::empty<PolicySeq>::value >
    struct get_list_of_concepts {
      typedef typename concat<
        typename PolicySeq::concept,
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
  template<typename Sequence>
  struct flatten {
    typedef typename detail::flatten<
        typename boost::mpl::begin<Sequence>::type,
        typename boost::mpl::end<Sequence>::type,
        boost::mpl::vector0<>
      >::type
    type;
  };
  
  template<typename PolicySeq>
  struct resulting_concept {
    typedef typename flatten<
      typename details::do_shadowing<
        typename details::get_list_of_concepts<PolicySeq>::type
        >::type
      >::type
    type;
  };
}

int main() {
  std::cout << utils::supports_concept<policy1, concepts::container>::value
            << ' ' // 1
            << utils::has_concept<mpl::vector2<policy3, policy1>,
                                  concepts::container>::value
            << ' ' // 1
            << utils::has_concept<mpl::vector0<>, concepts::container>::value
            << ' ' // 0
            << utils::check_concepts<mpl::vector2<policy3, policy1> >::value
            << ' ' // 0
            << utils::check_concepts<mpl::vector0<> >::value << ' ' // 1
            << utils::check_concepts<mpl::vector2<policy1, policy3> >::value
            << ' ' // 1
            << '\n';

  typedef utils::detail::do_shadowing<mpl::vector4<bar, foo, implies_shads, baz> >::type
    results;

  std::cout << "shadowing: " << boost::mpl::size<results>::value << ' '
            << boost::is_same<boost::mpl::front<results>::type,
                              implies_shads>::value << ' '
            << '\n';

  //std::cout << "_Z1x" << typeid(utils::flatten<mpl::vector2<bar, bar> >::type).name() << '\n';
}
