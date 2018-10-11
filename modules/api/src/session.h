#ifndef SESSION_H
#define SESSION_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "vms_interface.h"

namespace vms {
namespace api {

struct Auth {
  std::string username;
  std::string password;
};

class Session {
 public:
  std::shared_ptr<vms::VMSInterface> login(const std::string &ip,
                                           const std::string &username,
                                           const std::string &password,
                                           const std::string &vendor);

  void logout(const std::string &ip, const std::string &username,
              const std::string &password);

 private:
  std::shared_ptr<vms::VMSInterface> _create_vendor_vms(
      const std::string &vendor);

  std::string _session_key(const std::string &ip,
                           const std::string &username,
                           const std::string &password);

  void _add_session(const std::string &ip,
                    const std::string &username,
                    const std::string &password,
                    std::shared_ptr<vms::VMSInterface>);

  std::shared_ptr<vms::VMSInterface> _get_session(const std::string &ip,
                                                  const std::string &username,
                                                  const std::string &password);

  bool _match(const std::string &ip, const std::string &username,
              const std::string &password);

  std::unordered_map<std::string, std::shared_ptr<vms::VMSInterface>> _sessions;
  std::unordered_map<std::string, Auth> _auths;
  std::mutex _mutex;
};

}  // namespace api
}  // namespace vms

#endif
