// Copyright (C) 2005 by Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
// Content: Compatibility Macros for increasing Code-Safety
// Authors: Rüdiger Sonderfeld
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

#ifndef GOTT_UTIL_DEBUG_WARNINGS_HPP
#define GOTT_UTIL_DEBUG_WARNINGS_HPP

#ifdef __GNUC__

#define GOTT_WARN_UNUSED_RETURN __attribute__ ((warn_unused_result))
#define GOTT_WARN_NONNULL(x) __attribute__ ((nonnull x))
#define GOTT_WARN_DEPRECATED __attribute__ ((deprecated))

#else

#define GOTT_WARN_UNUSED_RETURN
#define GOTT_WARN_NONNULL(x)
#define GOTT_WARN_DEPRECATED

#endif

#endif
