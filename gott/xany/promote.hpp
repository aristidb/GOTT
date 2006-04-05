/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A Type-agnostic Storage Class.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef GOTT_UTIL_XANY_PROMOTE_HPP
#define GOTT_UTIL_XANY_PROMOTE_HPP

#include <boost/type_traits/is_enum.hpp>
#include <gott/string/string.hpp>
#include <string>

namespace gott {
class string_buffer;

namespace xany {

enum fundamental_class {
  no_class, enum_class
};

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
template<class T, 
  fundamental_class is_enum = boost::is_enum<T>::value 
  ? enum_class
  : no_class> 
struct promote {
  typedef T type;
  typedef T &reference;

  static type get(T const &v) { return v; }
  static reference get_back(type &v) { return v; }
};

template<class Old, class New>
struct standard_converter {
  static Old back(New const &v) { return static_cast<Old>(v); }
  static New forward(Old const &v) { return static_cast<New>(v); }
  static void assign(New &dest, Old const &src) { dest = src; }
};

template<class Old, class New, class Conv = standard_converter<Old, New> > 
struct standard_promote {
  typedef New type;
  class reference {
  public:
    operator Old() const { return Conv::back(ref_val); }
    reference operator=(Old const &new_val) {
      Conv::assign(ref_val, new_val);
      return *this;
    }
  private:
    New &ref_val;
    reference(New &v) : ref_val(v) {}
    friend struct standard_promote;
  };
  static New get(Old const &v) { return Conv::forward(v); }
  static reference get_back(New &v) { return reference(v); }
};

#define GOTT_XANY_DECLARE_PROMOTER(Old, New) \
  template<> struct promote<Old, no_class> : standard_promote<Old, New> {}

#define GOTT_XANY_DECLARE_PROMOTER2(Old, New, Conv) \
  template<> struct promote<Old, no_class> : standard_promote<Old, New, Conv> {}

template<class T>
struct promote<T, enum_class> : standard_promote<T, long> {};

GOTT_XANY_DECLARE_PROMOTER(char, long);
GOTT_XANY_DECLARE_PROMOTER(short, long);
GOTT_XANY_DECLARE_PROMOTER(int, long);
GOTT_XANY_DECLARE_PROMOTER(unsigned char, unsigned long);
GOTT_XANY_DECLARE_PROMOTER(unsigned short, unsigned long);
GOTT_XANY_DECLARE_PROMOTER(unsigned int, unsigned long);

GOTT_XANY_DECLARE_PROMOTER(float, double);

template<gott::encoding enc>
struct enc_string_converter {
  template<class Old>
  static string forward(Old const &v) { return string(v, enc); }
  template<class Old>
  static void assign(string &dest, Old const &src) { dest = forward(src); }
};

GOTT_XANY_DECLARE_PROMOTER2(char *, string, enc_string_converter<utf8>);
GOTT_XANY_DECLARE_PROMOTER2(char const *, string, enc_string_converter<utf8>);
GOTT_XANY_DECLARE_PROMOTER2(std::string, string, enc_string_converter<utf8>);

GOTT_XANY_DECLARE_PROMOTER2(wchar_t *, string, enc_string_converter<wide>);
GOTT_XANY_DECLARE_PROMOTER2(wchar_t const *, string,enc_string_converter<wide>);
#ifdef HAVE_WIDE_STDLIB
GOTT_XANY_DECLARE_PROMOTER2(std::wstring, string, enc_string_converter<wide>);
#endif
GOTT_XANY_DECLARE_PROMOTER(string_buffer, string);

}}

#endif
