// Testapp
//
// Copyright (C) 2004 Philipp Matthias Schaefer
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

#include <iostream>
#ifndef DEBUG
#define DEBUG
#endif
#include "trace.hpp"

int r2 (int i) {
  TRACEME;
  if(i)
    return r2(i - 1);
  return i;
}

int rtest (int i) {
  TRACEME;

  for(int c = 0; c < 923423; c+=2) c--;

  if (i) {
    r2(i - 1);
    return rtest(i - 1);
  }
  return i;
}

int main() {
  rtest(6);

  ::gott::util::debug::tracefilter t2(-1,::gott::util::debug::TracefilterFlags(
                                      ::gott::util::debug::TF_ALLFIELDS));
  std::wcout << t2(std::clock());

  ::gott::util::debug::tracefilter t1(-1,::gott::util::debug::TracefilterFlags(
                                      ::gott::util::debug::TF_ALLFIELDS
                                      | ::gott::util::debug::TF_GROUPBYFUNC));
  std::wcout << t1(std::clock());
  return 0;
}
