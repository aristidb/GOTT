#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/replace.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#define PROCESS2(x, y) ((x, y)) PROCESS
#define PROCESS(x, y) ((x, y)) PROCESS2
#define PROCESS_ELIM
#define PROCESS2_ELIM

#define T2S(x) BOOST_PP_CAT(PROCESS x, _ELIM)

#define INSTRS(x) ((0, 0)) BOOST_PP_SEQ_FOR_EACH(EXPAND, ~, T2S(x))

#define EXPAND(r, d, e) EXPAND2 e
#define EXPAND2(x, y) BOOST_PP_CAT(PARAM_, x)(y)

#define PARAM_name(x) ((0, x))
#define PARAM_extra(x) ((1, x))
#define PARAM_foo(x) ((2, x))

#define INITIAL (0) (0) (0)

#define COMBINE(s, state, x) \
  BOOST_PP_SEQ_REPLACE( \
    state, \
    BOOST_PP_TUPLE_ELEM(2, 0, x), \
    BOOST_PP_TUPLE_ELEM(2, 1, x))

#define INVOKE2(x) \
  BOOST_PP_SEQ_TO_TUPLE( \
    BOOST_PP_SEQ_FOLD_LEFT( \
      COMBINE, \
      INITIAL, \
      INSTRS(x)))

#define RUN(name, extra, foo) NAME=name, EXTRA=extra, FOO=foo

#define INVOKEx(x) RUN x
#define INVOKE(x) INVOKEx(INVOKE2(x))

INVOKE( (name, "hey") (extra, bla) (foo, bar) )
INVOKE( (extra, "test") (name, "hallo") (foo, bar))
INVOKE( (extra, "test") (name, "hallo"))
INVOKE( (foo, "test") (name, "hallo"))
INVOKE()
