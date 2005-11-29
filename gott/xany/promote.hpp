// GOTT utilities
//
// Copyright (C) 2005 Aristid Breitkreuz
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GOTT_UTIL_XANY_PROMOTE_HPP
#define GOTT_UTIL_XANY_PROMOTE_HPP

namespace gott {
class string;

namespace xany {

/**
 * Allows for less rigid and more efficient typing in xany::Xany.
 * The concept is easy: With template specialization any type can be mapped
 * to another type - but transparently. 
 *
 * Example: you save an int with value 4 to a typeless object. Then you extract
 * a short out of it. The whole thing works because both short and int are 
 * mapped to long. Because of advanced mapping techniques such as proxying, you
 * don't notice though.
 */
template<class T> 
struct promote {
  typedef T type;
  typedef T &reference;

  static type get(T const &v) { return v; }
  static reference get_back(type &v) { return v; }
};

template<class Old, class New> 
struct standard_promote {
  typedef New type;
  class reference {
  public:
    operator Old() const { return static_cast<Old>(ref_val); }
    reference operator=(Old const &new_val) {
      ref_val = new_val;
      return *this;
    }
  private:
    New &ref_val;
    reference(New &v) : ref_val(v) {}
    friend struct standard_promote;
  };
  static New get(Old const &v) { return New(v); }
  static reference get_back(New &v) { return reference(v); }
};

#define GOTT_XANY_DECLARE_PROMOTER(Old, New) \
  template<> struct promote<Old> : standard_promote<Old, New> {}

template<class ElementT, class New>
struct array_promote {
  typedef New type;

  static New get(ElementT *v) { return New(v); }
  static New get(ElementT const *v) { return New(v); }
};

#define GOTT_XANY_ARRAY_PROMOTER(OldE, New) \
  template<int Size> \
  struct promote<OldE [Size]> : array_promote<OldE, New> {}

GOTT_XANY_DECLARE_PROMOTER(char, long);
GOTT_XANY_DECLARE_PROMOTER(short, long);
GOTT_XANY_DECLARE_PROMOTER(int, long);
GOTT_XANY_DECLARE_PROMOTER(unsigned char, unsigned long);
GOTT_XANY_DECLARE_PROMOTER(unsigned short, unsigned long);
GOTT_XANY_DECLARE_PROMOTER(unsigned int, unsigned long);

GOTT_XANY_DECLARE_PROMOTER(float, double);

GOTT_XANY_DECLARE_PROMOTER(char const *, string);
GOTT_XANY_DECLARE_PROMOTER(char *, string);
GOTT_XANY_DECLARE_PROMOTER(std::string, string);

GOTT_XANY_DECLARE_PROMOTER(wchar_t const *, string);
GOTT_XANY_DECLARE_PROMOTER(wchar_t *, string);
#ifdef HAVE_WIDE_STDLIB
GOTT_XANY_DECLARE_PROMOTER(std::wstring, string);
#endif

}}

#endif
