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
class sigsafe_message_manager;

class GOTT_EXPORT signal_manager {
public:
  static QID const qid;

  /// Constructor.
  signal_manager();

  /// Pure-virtual destructor.
  virtual ~signal_manager() = 0;

  /**
   * In-program signal for signal notifications. Each signal should be 
   * associated to a single main_loop or signal_manager - or a 
   * user_error will be thrown.
   */
  virtual sigc::signal1<void, int> &on_signal(int sig) = 0;

public:
  static signal_manager *get_for(main_loop &) { return 0; }
};

/**
 * Feature for main_loops able to deal with operating system signals.
 */
class standard_signal_manager :
  public signal_manager,
  boost::noncopyable,
  public sigc::trackable {
public:
  /**
   * Constructor.
   * \param message_manager Message manager to send signal information over. 
   */
  standard_signal_manager(sigsafe_message_manager *message_manager);
  /// Destructor.
  ~standard_signal_manager();

  sigc::signal1<void, int> &on_signal(int sig);

private:
  static void register_signal(
      int sig, standard_signal_manager *handler) GOTT_LOCAL;
  static void unregister_all(standard_signal_manager *handler) GOTT_LOCAL;
  static standard_signal_manager *find(int sig) GOTT_LOCAL;
  static void signal_handler(int sig) GOTT_LOCAL;

  void immediate_action(int sig);
  void receive_message(gott::xany::Xany const &);

private:
  sigsafe_message_manager *message_manager;
  typedef std::map<int, sigc::signal1<void, int> > handler_map_t;
  handler_map_t handlers;
};

}}

#endif
