#include <string>
#include <map>

#include <bfd.h>

namespace binary {
  namespace helper {
    char const * const target="i486-pc-linux-gnu";

    /// ugly smart pointer for handling bfd handles
    class bfd_handle {
      bfd *hnd;
    public:
      bfd_handle(bfd *h) : hnd(h) { }
      ~bfd_handle() { if(hnd) bfd_close(hnd); }

      bfd *handle() { return hnd; }
    };

    /// ugly smart pointer for handling C style memory deallocation
    template<typename T>
    class c_ptr {
      T *ptr;
    public:
      c_ptr() : ptr(0x0) { }
      c_ptr(T *p) : ptr(p) { }
      ~c_ptr() { free(ptr); }

      T *get() { return ptr; }
    };

    struct ltstr {
      bool operator()(char const* s1, char const* s2) const {
	return strcmp(s1, s2) < 0;
      }
    };
  }

  typedef std::map<char const *, asymbol*> symbol_map;

  symbol_map get_symbols(char const *filename, bool is_dynamic)
  {
    helper::bfd_handle file(bfd_openr(filename, helper::target));
    if(!file.handle())
      throw "file not found";

    if(!bfd_check_format(file.handle(), bfd_object))
      throw "Not an object";

    void *tmp;
    unsigned int size;
    long mini_count = bfd_read_minisymbols(file.handle(), is_dynamic, 
					   &tmp, &size);
    helper::c_ptr<void> mini_syms(tmp);
    if(mini_count <= 0)
      throw "couldn't read minisymbols";

    symbol_map ret;
    for(long i=0; i<mini_count; ++i) {
      unsigned char *msyms = reinterpret_cast<unsigned char*>(mini_syms.get());
      asymbol *sym=bfd_minisymbol_to_symbol(file.handle(), is_dynamic,
					    msyms+size*i,
					    bfd_make_empty_symbol
					                    (file.handle()));
      if(!sym)
	throw "couldn't convert minisymbol to symbol";

      ret[sym->name]=sym;
    }
    return ret;
  }
}
