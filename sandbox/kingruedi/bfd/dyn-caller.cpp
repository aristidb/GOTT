#include "bfd.hpp"
#include <ffi.h>
#include <dlfcn.h>

#include <cstring>
#include <iostream>

char const * const plugin="./libdyn-helloworld.so";

namespace helloworld {
  char const *get_b_a_r() {
    binary::symbol_map symbols = binary::get_symbols(plugin, true);

    for(binary::symbol_map::const_iterator i = symbols.begin();
		i != symbols.end();
		++i) {
      if(std::strstr(i->first, "bar") != 0x0)
		return i->first;
    }
    return 0x0;
  }
}

int main() {
  char const *bar_sym=helloworld::get_b_a_r();

  if(!bar_sym)
    return 1;

  void *dl=dlopen(plugin, RTLD_NOW); //uh, unsafe resource handling!
                                     // (don't try this at home kids)
  if(!dl) {
    throw dlerror();
    return 1;
  }
  void *bar=dlsym(dl, bar_sym);
  //dlclose(dl);
  if(!bar)
    return 1;

  //Both Codes cause a Segfault!

#if 1
  ((void (*)())bar)();
#else
  ffi_cif cif;
  if(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, &ffi_type_void, 0x0)
     != FFI_OK)
    return 1;

  ffi_call(&cif, (void (*)())bar, 0x0, 0x0);
#endif
  return 0;
}
