#ifndef VINO_ERROR_HPP
#define VINO_ERROR_HPP

#include <stdexcept>

namespace vino {

  class error, public std::runtime_error {
  public:
    explicit error(const std::string& s)
      : std::runtime_error(s), gl_error_number(glGetError()) {}

    virtual const char* what() throw() {
      return std::string(std::runtime_error::what())
        .append(std::string(", GL Error Number: "))
        .append(std::string(itoa(gl_error_number)))
        .c_str();
    }
  private:
    unsigned int gl_error_number;
  };
}

#endif
