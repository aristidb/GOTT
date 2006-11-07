# vim:ts=2:sw=2:expandtab:filetype=cmake:
# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is GOTT Build Scripts.
#
# The Initial Developer of the Original Code is
# Aristid Breitkreuz (aribrei@arcor.de).
# Portions created by the Initial Developer are Copyright (C) 2006
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#  Aristid Breitkreuz (aribrei@arcor.de)
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

project(GOTT)
cmake_minimum_required(VERSION 2.4)
enable_language(CXX)

# configuration -----------------------------------------------------

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/buildconf)

find_package(Boost REQUIRED)
set(Boost_SUFFIX "" CACHE STRING "Boost library suffix")
find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Curses REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${X11_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS} ${X11_LIBRARY_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/..)
include_directories(${PROJECT_SOURCE_DIR}/../ext/testsoon/include)

include(PlatformBasic)
include(PlatformHeaders)
include(PlatformExtra)
include(GccVisibility)

set(CMAKE_SHARED_MODULE_PREFIX )
set(CMAKE_SHARED_MODULE_SUFFIX .plugin)

add_definitions(-DBOOST_PARAMETER_MAX_ARITY=8)

# targets -----------------------------------------------------------

# LIBRARY gott_core
file(GLOB src_core
  exceptions.cpp xany/xany.cpp
  string/*.cpp
  plugin/*.cpp
  tdl/*.cpp tdl/schema/*.cpp tdl/schema_lang/*.cpp tdl/parse/*.cpp
  tdl/write/*.cpp tdl/structure/*.cpp
)
set(syswrap_portable syswrap/clocks.cpp syswrap/errno.cpp)
if (UNIX)
  file(GLOB syswrap_unix syswrap/*_unix*.cpp)
  if (LINUX)
    file(GLOB syswrap_linux syswrap/*_linux.cpp)
  endif (LINUX)
endif (UNIX)
set(syswrap ${syswrap_portable} ${syswrap_unix} ${syswrap_linux})

add_library(gott_core SHARED ${src_core} ${syswrap})
if (UNIX)
  target_link_libraries(
    gott_core
    boost_thread${Boost_SUFFIX}
    boost_signals${Boost_SUFFIX}
    dl
  )
endif (UNIX)

# LIBRARY gott_system
file(GLOB src_system
  events/*.cpp
  notify_fs/*.cpp
)
add_library(gott_system SHARED ${src_system})
target_link_libraries(gott_system gott_core)

# LIBRARY gott_graphics
file(GLOB src_graphics
  graphics/*.cpp
)
add_library(gott_graphics SHARED ${src_graphics})

# LIBRARY gott_ui
file(GLOB src_ui
  ui/*.cpp 
  ui/x11/window.cpp ui/x11/uicontext.cpp ui/x11/input.cpp
  ui/x11/renderer_factory.cpp
  ui/ncurses/window.cpp ui/ncurses/uicontext.cpp 
)
add_library(gott_ui SHARED ${src_ui})
target_link_libraries(gott_ui gott_system gott_graphics X11 ${CURSES_LIBRARIES} ${OPENGL_LIBRARIES})

# LIBRARY gott_hci
file(GLOB src_hci
  hci/*.cpp
  format/*.cpp
)
add_library(gott_hci SHARED ${src_hci})
target_link_libraries(gott_hci gott_core)

# EXECUTABLE thunk_test
add_executable(thunk_test thunk.cpp)
target_link_libraries(thunk_test gott_core)

# EXECUTABLE string_test
add_executable(string_test string/test/test.cpp)
target_link_libraries(string_test gott_core)

# EXECUTABLE properties_test
add_executable(properties_test properties/test.cpp)
target_link_libraries(properties_test gott_core)

# EXECUTABLE hci_test
file(GLOB src_hci_test
  hci/test/*.cpp)
add_executable(hci_test ${src_hci_test})
target_link_libraries(hci_test gott_hci)

# EXECUTABLE unit_test
file(GLOB src_unit_test
  tests/*.cpp tests/*/*.cpp
)
add_executable(unit_test ${src_unit_test})
target_link_libraries(unit_test gott_system)

# EXECUTABLE tdl_module_test
file(GLOB src_tdl_module_test
  tdl/test/*.cpp
  tdl/test/schema/*.cpp
)
add_executable(tdl_module_test ${src_tdl_module_test})
target_link_libraries(tdl_module_test gott_core)

# EXECUTABLE tdl_unit_test
file(GLOB src_tdl_unit_test
  tdl/test/unit/*.cpp
)
add_executable(tdl_unit_test ${src_tdl_unit_test})
target_link_libraries(tdl_unit_test gott_core)

# EXECUTABLE pongbar_loop
add_executable(pongbar_loop examples/pongbar_loop.cpp)
target_link_libraries(pongbar_loop gott_system)

# EXECUTABLE producer_consumer1
add_executable(producer_consumer1 examples/producer_consumer1.cpp)
target_link_libraries(producer_consumer1 gott_core)

# EXECUTABLE throttled_yes
add_executable(throttled_yes examples/throttled_yes.cpp)
target_link_libraries(throttled_yes gott_system)

# EXECUTABLE just_windows
add_executable(just_windows examples/just_windows.cpp)
target_link_libraries(just_windows gott_ui gott_graphics)

# EXECUTABLE test_repatcher
add_executable(test_repatcher tdl/schema_lang/playground/test_repatcher.cpp)
target_link_libraries(test_repatcher gott_core)

# EXECUTABLE mq_test
add_executable(mq_test thread/mq_test.cpp)
if (UNIX)
  target_link_libraries(mq_test boost_thread${Boost_SUFFIX})
endif (UNIX)

# EXECUTABLE syswrap_test
add_executable(syswrap_test syswrap/test.cpp)
target_link_libraries(syswrap_test gott_core)

# EXECUTABLE transformations
add_executable(transformations graphics/test/transformations.cpp)
target_link_libraries(transformations gott_graphics)

# EXECUTABLE notify_test
file(GLOB src_notify_test
  notify_fs/test/*.cpp
)
add_executable(notify_test ${src_notify_test})
target_link_libraries(notify_test gott_system)

add_custom_target(plugins)

# PLUGIN tdl_builtins
file(GLOB src_tdl_builtins
  tdl/schema/types/*.cpp
  tdl/structure/repatchers/*.cpp
)
add_library(tdl_builtins MODULE ${src_tdl_builtins})
add_dependencies(plugins tdl_builtins)

# PLUGIN tdl_schema_lang
file(GLOB src_tdl_schema_lang
  tdl/schema_lang/types/*.cpp
)
add_library(tdl_schema_lang MODULE ${src_tdl_schema_lang})
add_dependencies(plugins tdl_schema_lang)

# PLUGIN notify_fs_kqueue
if (BUILD_KQUEUE)
  file(GLOB src_notify_fs_kqueue
    notify_fs/kqueue/*.cpp
  )
  add_library(notify_fs_kqueue MODULE ${src_notify_fs_kqueue})
  add_dependencies(plugins notify_fs_kqueue)
endif (BUILD_KQUEUE)

# PLUGIN notify_fs_inotify
if (BUILD_INOTIFY)
  file(GLOB src_notify_fs_inotify
    notify_fs/inotify/*.cpp
  )
  add_library(notify_fs_inotify MODULE ${src_notify_fs_inotify})
  add_dependencies(plugins notify_fs_inotify)
endif (BUILD_INOTIFY)

# PLUGIN configuration
file(GLOB src_configuration
  configuration/*.cpp
)
add_library(configuration MODULE ${src_configuration})
add_dependencies(plugins configuration)

# PLUGIN events_kqueue_loop
if (BUILD_KQUEUE)
  add_library(events_kqueue_loop MODULE events/loops/kqueue_loop.cpp)
  add_dependencies(plugins events_kqueue_loop)
endif (BUILD_KQUEUE)

# PLUGIN events_epoll_loop
if (BUILD_EPOLL)
  add_library(events_epoll_loop MODULE events/loops/epoll_loop.cpp)
  add_dependencies(plugins events_epoll_loop)
endif (BUILD_EPOLL)

# PLUGIN events_select_loop
add_library(events_select_loop MODULE events/loops/select_loop.cpp)
add_dependencies(plugins events_select_loop)

# PLUGIN graphics_glx_renderer
file(GLOB src_graphics_glx_renderer
  graphics/gl/*.cpp
  ui/x11/glx_factory.cpp
  )
add_library(graphics_glx_renderer MODULE ${src_graphics_glx_renderer})
add_dependencies(plugins graphics_glx_renderer)
target_link_libraries(graphics_glx_renderer gott_graphics ${OPENGL_LIBRARIES})
