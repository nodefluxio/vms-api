#ifndef FLUSSONIC_H
#define FLUSSONIC_H

#include <ctime>
#include "vms_interface.h"

namespace vms {
namespace flussonic {
class Flussonic : public VMSInterface {
 public:
  Flussonic(const std::string &log_path);

  void login(const std::string &ip, unsigned int port,
             const std::string &username, const std::string &password) override;

  void logout() override;

  int session_id() override;

  std::vector<Device> nvr_list(unsigned int max) override;

  std::vector<Device> camera_list(unsigned int max) override;

  std::vector<Record> recording_list(const std::string &camera_code,
                                     const std::string &start_time,
                                     const std::string &end_time,
                                     unsigned int max) override;

  std::string playback(const std::string &camera_code,
                       const std::string &nvr_code,
                       const std::string &start_time,
                       const std::string &end_time) override;

  std::string download(const std::string &camera_code,
                       const std::string &nvr_code,
                       const std::string &start_time,
                       const std::string &end_time) override;

  std::string live_stream(const std::string &camera_code,
                          const std::string &nvr_code,
                          const std::string &transport) override;

  std::string create_url(const std::string &_ip, const std::string &camera_code,
                         const std::string &start_time,
                         const std::string &end_time);

  std::time_t to_unix_timestamp(const std::string &timestamp);

  bool status_stream(const std::string &_ip, const unsigned int &_port,
                     const std::string &camera_code);

  bool to_bool(std::string str);

 private:
  int _session_id;
  bool _logged_in;
  std::string _ip, _id, _pass;
  unsigned int _port;
};
}  // namespace flussonic
}  // namespace vms

#endif
