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

#ifndef GOTT_BASE_EVENTS_SIGNAL_MANAGER_HPP
#define GOTT_BASE_EVENTS_SIGNAL_MANAGER_HPP

#include <gott/visibility.hpp>
#include <gott/string/qid.hpp>
#include <gott/xany/xany.hpp>
#include <sigc++/signal.h>
#include <map>
#include <boost/noncopyable.hpp>

namespace gott {
namespace events {

class main_loop;
class inprocess_message_manager;

/**
 * Feature for main_loops able to deal with operating system signals.
 */
class GOTT_EXPORT signal_manager : boost::noncopyable, public sigc::trackable {
public:
  /**
   * Constructor.
   * \param message_manager Message manager to send signal information over. 
   */
  signal_manager(inprocess_message_manager *message_manager);
  /// Destructor.
  ~signal_manager();

  static QID const qid;

  /**
   * In-program signal for signal notifications. Each signal should be 
   * associated to a single main_loop or signal_manager - or a 
   * std::runtime_error will be thrown.
   */
  sigc::signal1<void, int> &on_signal(int sig);

public:
  class GOTT_LOCAL proxy_t : boost::noncopyable {
  public:
    sigc::signal1<void, int> &on_signal(int sig) {
      return db[sig];
    }

    GOTT_EXPORT void operator() (main_loop &) const;

  private:
    std::map<int, sigc::signal1<void, int> > db;
  };

  typedef boost::shared_ptr<proxy_t> proxy;
  typedef proxy_t &proxy_ref;

  GOTT_LOCAL static proxy_t *make_proxy() { return new proxy_t; }

private:
  static void register_signal(int sig, signal_manager *handler) GOTT_LOCAL;
  static void unregister_all(signal_manager *handler) GOTT_LOCAL;
  static signal_manager *find(int sig) GOTT_LOCAL;
  static void signal_handler(int sig) GOTT_LOCAL;

  void immediate_action(int sig);
  void receive_message(gott::xany::Xany const &);

private:
  inprocess_message_manager *message_manager;
  std::map<int, sigc::signal1<void, int> > handlers;
};

}}

#endif
