//#include <cstddef>

#include <boost/preprocessor.hpp>

#ifndef POLICY_LIMIT
#define POLICY_LIMIT 4
#endif

namespace detail {
struct nada { };
template<typename Before, typename T>
struct dummy_concept : public Before {
    
};
}

#define TEMPL_PARAM(z, N, data)                                \
  BOOST_PP_COMMA_IF(N)                                         \
  template<typename, typename> class BOOST_PP_CAT(Policy, N) = \
    detail::dummy_concept

#define PL                                                      \
  BOOST_PP_REPEAT(POLICY_LIMIT, TEMPL_PARAM, BOOST_PP_EMPTY())

#define PP_PRED(r, state)                              \
  BOOST_PP_NOT_EQUAL(                                  \
       BOOST_PP_TUPLE_ELEM(3, 0, state),               \
       BOOST_PP_DEC(BOOST_PP_TUPLE_ELEM(3, 1, state))  \
                    )                                  \
   /**/

#define PP_OP(r, state) \
   ( \
      BOOST_PP_DEC(BOOST_PP_TUPLE_ELEM(3, 0, state)), \
      BOOST_PP_TUPLE_ELEM(3, 1, state),               \
      BOOST_PP_TUPLE_ELEM(3, 2, state)                \
   ) \
   /**/

#define PP_MACRO(r, state) \
  BOOST_PP_TUPLE_ELEM(3, 2, state) ( BOOST_PP_TUPLE_ELEM(3, 0, state) )

#define PP_DOWN_FROM(N, macro)                                        \
  BOOST_PP_FOR((BOOST_PP_DEC(N), 0, macro), PP_PRED, PP_OP, PP_MACRO) \
  macro(0)


#define NEXT_POLICY(N) \
  BOOST_PP_CAT(Policy, N)<

#define VECTOR(z, N, data)                      \
  , T>

#define DERIVE                                                          \
  public BOOST_PP_CAT(Policy, BOOST_PP_DEC(POLICY_LIMIT))<              \
  PP_DOWN_FROM(BOOST_PP_DEC(POLICY_LIMIT), NEXT_POLICY) \
  detail::nada, T >                            \
  BOOST_PP_REPEAT(BOOST_PP_DEC(POLICY_LIMIT), VECTOR, BOOST_PP_EMPTY())

template<typename T, PL>
class container
  : DERIVE
{
public:
  typedef T value_type;
  typedef std::size_t size_type;
  // ...
};

#undef DERIVE
#undef VECTOR
#undef POLICY
#undef NEXT_POLICY
#undef PP_DOWN_FROM
#undef PP_MACRO
#undef PP_OP
#undef PP_PRED
#undef PL
#undef TEMPL_PARAM

template<typename Before, typename T>
struct array { };

int main() {
  container<int, array> foo;
}
