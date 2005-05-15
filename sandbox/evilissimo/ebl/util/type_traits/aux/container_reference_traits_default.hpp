#ifndef GUARD_EBL_UTIL_TYPE_TRAITS_AUX_CONTAINER_REFERENCE_TRAITS_DEFAULT_HPP_INCLUDED
#define GUARD_EBL_UTIL_TYPE_TRAITS_AUX_CONTAINER_REFERENCE_TRAITS_DEFAULT_HPP_INCLUDED 1

namespace ebl
{
  namespace util
  {
    namespace type_traits
    {
      namespace aux
      {
        template < class Container >
          struct container_reference_traits_default
 	  {
	    typedef typename Container::reference type;
	  };
      }//namespace ebl::util::type_traits::aux
    }//namespace ebl::util::type_traits
  }//namespace ebl::util
}//namespace ebl

#endif //GUARD_EBL_UTIL_TYPE_TRAITS_AUX_CONTAINER_REFERENCE_TRAITS_DEFAULT_HPP_INCLUDED 
