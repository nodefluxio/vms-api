#ifndef VMS_VMS_EXCEPTION_H
#define VMS_VMS_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace vms {

class VMSException : public std::runtime_error {
 private:
  int _code;

 public:
  VMSException(int code, const char* message) : std::runtime_error(message), _code(code) {}

  int code() {
    return _code;
  }

};

}

#endif //VMS_VMS_EXCEPTION_H
