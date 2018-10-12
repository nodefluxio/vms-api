#include "hw_errors.h"

namespace vms {
namespace hwivs {

std::string error_message(int code) {
  switch (code) {
    case 109100000:
      return "Incorrect user name or password.";
    case 109100001:
      return "The user does not exist. The user ID has been deregistered or is unavailable.";
    case 109100002:
      return "Insufficient permission.";
    case 109100019:
      return "Failed to connect to the network. (Connection timeout due to an offline server or a network problem.)";
    case 109100020:
      return "The number of connections exceeds the maximum limit.";
    case 109100055:
      return "Invalid URL.";
    case 119108128:
      return "The NVR is offline.";
    case 119101306:
      return "The number of concurrent online users who use the same account exceeded the maximum.";
    case 119101307:
      return "The total number of concurrent online users exceeded the maximum.";
    case 119132005:
      return "The device does not exist or license restriction exists.";
    case 999115312:
      return "Invalid SessionID.";
    default:
      return std::string("Unidentified error happened: ") + std::to_string(code);
  }
}

}
}
