// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Generic Input/Output Helpers
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
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

#include "thunk.hpp"
#include <gott/string/string.hpp>
#include <iostream>

using namespace gott;
using std::cout;
using std::endl;

int main() {
  std::auto_ptr<thunk_t<char> > itos = thunk<char, integer_to_string>(12340);
  cout << string(thunk<gott::utf8_t, thunk_cast>(itos)->consume_all()) << endl;
}
