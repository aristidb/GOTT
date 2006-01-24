// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)

#include "system_error.hpp"

using gott::system_error;

system_error::system_error(std::string const &desc)
: std::runtime_error(desc) {}

system_error::~system_error() throw() {}
