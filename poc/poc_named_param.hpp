#include <boost/preprocessor/cat.hpp>

#define PROCESS2(x, y) ((x, y)) PROCESS
#define PROCESS(x, y) ((x, y)) PROCESS2
#define PROCESS_ELIM
#define PROCESS2_ELIM

#define T2S(x) BOOST_PP_CAT(PROCESS x, _ELIM)

T2S( (name, "hey") (extra, bla) (foo, bar) )
