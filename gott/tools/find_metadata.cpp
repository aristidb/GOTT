// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
 * The Original Code is A Plugin / Loadable Module Engine.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;

char const begin[] = "GOTT PLUGIN METADATA BEGIN";
string end = "GOTT PLUGIN METADATA END";

int main(int argc, char **argv) {
  cout << "# AUTOMATICALLY GENERATED: UTC " <<
    boost::posix_time::second_clock::universal_time() << endl;
  while (--argc) {
    char const *file = *++argv;
    ifstream stream(file, ios::binary);
    while (stream) {
      stream.ignore(8192, '\n');
      char const *p = begin;
      char const *ep = begin + sizeof(begin) - 1;
      while (stream && p != ep && *p == stream.peek()) {
        ++p;
        stream.get();
      }
      if (p == ep) { // found
        string buf;
        string cur;
        while (getline(stream, cur) && cur != end) {
          char cwdb[8192];
          string fname;
          if (file[0] != '/') {
            fname += getcwd(cwdb, sizeof(cwdb));
            fname += '/';
          }
          fname += file;
          boost::algorithm::replace_all(cur, "??FILE??", fname);
          buf += cur;
          buf += '\n';
        }
        cout << "# " << file << ":\n" << buf << "\n# --------------\n";
      }
    }
  }
}
