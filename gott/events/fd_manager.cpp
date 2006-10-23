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
 * The Original Code is An Event Handling Class Library.
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

#include "fd_manager.hpp"
#include "main_loop.hpp"
#include <map>
#include <boost/tuple/tuple.hpp>

using gott::events::fd_manager;

using namespace boost;
using namespace std;

fd_manager::fd_manager() {}
fd_manager::~fd_manager() {}

gott::QID const fd_manager::qid("gott::events::fd_manager");

struct fd_manager::proxy_t::impl {
  typedef
    map<int, 
      tuple<
        unsigned,
        function<void (unsigned)>,
        bool
      > >
    map_t;
  map_t db;
};

fd_manager::proxy_t::proxy_t() : p(new impl) {}
fd_manager::proxy_t::~proxy_t() {}

void fd_manager::proxy_t::add_fd(int fd, unsigned mask, 
    function<void (unsigned)> const &cb, bool wait) {
  p->db[fd] = make_tuple(mask, cb, wait);
}

void fd_manager::proxy_t::operator() (main_loop &m) const {
  fd_manager &f = m.feature<fd_manager>();
  for (impl::map_t::const_iterator it =p->db.begin(); it != p->db.end(); ++it) {
    impl::map_t::value_type::second_type const &t = it->second;
    f.add_fd(it->first, t.get<0>(), t.get<1>(), t.get<2>());
  }
}
