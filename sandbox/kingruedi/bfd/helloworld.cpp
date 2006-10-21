#include "bfd.hpp"
#include <ffi.h>

#include <cstring>
#include <iostream>

namespace foo {
  void bar() {
    std::cout << "Hello Wordl!\n";
  }
}

namespace helloworld {
  symvalue get_b_a_r(char const *argv0) {
    binary::symbol_map symbols = binary::get_symbols(argv0, false);

    for(binary::symbol_map::const_iterator i = symbols.begin();
	i != symbols.end();
	++i)
      if(std::strstr(i->first, "bar") != 0x0) {
	symbol_info info;
	bfd_symbol_info(i->second, &info);
	return info.value;
      }
    return 0x0;
  }
}

int main(int, char **argv) {
  try {
    symvalue bar=helloworld::get_b_a_r(argv[1]);

    ffi_cif cif;
    if(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, &ffi_type_void, 0x0)
       != FFI_OK)
      return 1;

    std::cout << std::hex << bar << '\n';

    ffi_call(&cif, (void (*)())bar, 0x0, 0x0);
    return 0;
  }
  catch(char const *str) {
    std::cerr << "Error: " << str << '\n';
    return 1;
  }
}
