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
 * The Original Code is Low-level System Wrapping Library.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com).
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com).
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

#ifndef GOTT_SYSWRAP_LD_UNIX_HPP
#define GOTT_SYSWRAP_Ld_UNIX_HPP

#include <gott/visibility.hpp>
#include <dlfcn.h>

namespace gott {


/**
 * \name Unix library loading routines
 * These functions wrap the dl routines of unix/posix. 
 * \{
 */
/**
 * Cast a symbol address to a function pointer. 
 * \endcode
 * \param handle the address of the symbol
 * \param FType a 
 * \return a function pointer
 */
template<typename FType>
GOTT_LOCAL inline FType* function_cast( void* handle )
{ return (FType*)(handle); }

/**
 * Open a dynamic library. See dlopen(3) for reference,
 * \code
 *   try{
 *     void * libm = gott::dlopen_unix( "libm.so", RTLD_NOW);
 *     double(*my_atan2)(double,double) = gott::function_cast<double(double,double)>(
 *       gott::dlsym_unix(libm, "atan2");
 *       );
 *   }
 *   catch(std::system_error const&e ){
 *   }
 * \endcode

 * \param filename The name of the library to open
 * \param flag either RTLD_NOW or RTLD_LAZY
 * \return a handle of the dynamic library
 * \throw gott::system_error
 */
GOTT_EXPORT void* dlopen_unix( char const* filename, int flag );

/**
 * Load a symbol from a dynamic library. See dlsym(3) for reference
 * \see gott::function_cast to do a clean cast. 
 * \param handle the handle of the dynamic library
 * \param symbol the name of the symbol 
 * \return the symbol address 
 * \throw gott::system_error
 */
GOTT_EXPORT void *dlsym_unix(void *handle, char const* symbol);

/**
 * Close a dynamic library. 
 * \param handle the handle of the previously loaded library 
 * \throw gott::system_error
 */
GOTT_EXPORT void dlclose_unix(void *handle);

/// \}

}

#endif
