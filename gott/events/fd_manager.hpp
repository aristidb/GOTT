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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#ifndef GOTT_BASE_EVENTS_FD_MANAGER_HPP
#define GOTT_BASE_EVENTS_FD_MANAGER_HPP

#include <gott/string/qid.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace gott {
namespace events {

class main_loop;

/**
 * Feature for main_loops able to watch file descriptors (*NIX) or sockets 
 * (*NIX + Win32).
 */
class GOTT_EXPORT fd_manager {
public:
  /// Constructor.
  GOTT_LOCAL fd_manager();
  /// Pure virtual destructor.
  virtual ~fd_manager() = 0;

  static QID const qid;

  /// Event on a file descriptor.
  enum event {
    /// A read has occurred.
    read = 1,
    /// A write has occurred.
    write = 2,
    /// Something other happened.
    exception = 4,
    /// Anything happened.
    any = read | write | exception
  };

  class GOTT_EXPORT installation_error {}; // FIXME

  /**
   * Add a file descriptor to watch.
   * \param fd The file descriptor of the soon watched object.
   * \param mask Any combination of read, write and exception.
   *           Determines which events are caught for this descriptor.
   * \param cb Callback called whenever an event happens on this descriptor
   *           and it is in the @p mask. Its parameter will be a combination
   *           of the events that occurred.
   * \param wait Specifies whether the main_loop::run shall wait for the
   *           descriptor to be removed.
   * \throw installation_error If the descriptor is already registered or
   *           something else happens.
   */
  virtual void add_fd(int fd, unsigned mask, 
      boost::function<void (unsigned)> const &cb, bool wait = true) = 0;

#if 0
  /**
   * Change an already watched file descriptor to watch.
   * \param fd The file descriptor of the watched object.
   * \param mask Any combination of read, write and exception.
   *           Determines which events are caught for this descriptor.
   * \param cb Callback called whenever an event happens on this descriptor
   *           and it is in the @p mask.
   * \throw installation_error If the descriptor is not yet registered or
   *           something else happens.
   */ 
  virtual void change_fd(int fd, unsigned mask,
      boost::function<void (event)> const &cb) = 0;
#endif

  /**
   * Remove a file descriptor from the set of watched objects.
   * \param fd The file descriptor of the watched objects.
   * \throw installation_error If the registered was not registered.
   */
  virtual void remove_fd(int fd) = 0;

public:
  class GOTT_EXPORT proxy_t : boost::noncopyable {
  public:
    proxy_t();
    ~proxy_t();

    void add_fd(int fd, unsigned mask,
        boost::function<void (unsigned)> const &cb, bool wait = true);

    void operator() (main_loop &) const;

  private:
    struct impl;
    boost::scoped_ptr<impl> p;
  };

  typedef boost::shared_ptr<proxy_t> proxy;
  typedef proxy_t &proxy_ref;

  GOTT_LOCAL static proxy_t *make_proxy() { return new proxy_t; }
};

}}

#endif
