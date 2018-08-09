#ifndef HW_IVS_H
#define HW_IVS_H

#include "vms_interface.h"

#ifdef _WIN32
#include "windows.h"
#endif

#include "IVS_SDK.h"

#ifndef _WIN32
#define HWND void*
#endif

namespace vms {
namespace hwivs {
class HuaweiIVS : public VMSInterface {
 public:
  HuaweiIVS(const std::string &log_path);

  ~HuaweiIVS();

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

  std::string live_stream(const std::string &camera_code,
                          const std::string &nvr_code,
                          const std::string &transport) override;

  void sdk_playback(const std::string &camera_code,
                    const std::string &start_time,
                    const std::string &end_time);

 private:
  int _session_id;
  bool _logged_in;

  IVS_ULONG _playback_handle = 0;
  HWND _show_box;
};
}  // namespace hwivs
}  // namespace vms

#endif
