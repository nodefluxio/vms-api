#ifndef VMS_INTERFACE_H
#define VMS_INTERFACE_H

#include <string>
#include <vector>

namespace vms {

class Device {
 public:
  Device() = default;

  Device(const std::string &name, const std::string &ip,
         const std::string &code)
      : _name(name), _ip(ip), _code(code){};

  Device(const std::string &name, const std::string &ip,
         const std::string &code, unsigned int port)
      : _name(name), _ip(ip), _code(code), _port(port){};

  ~Device() = default;

  std::string name() { return _name; };
  std::string ip() { return _ip; };
  std::string code() { return _code; };
  unsigned int port() { return _port; };

 private:
  std::string _name;
  std::string _ip;
  std::string _code;
  unsigned int _port;
};

struct Record {
  std::string camera_code;
  std::string nvr_code;
  std::string start_time;
  std::string end_time;
};

class VMSInterface {
 public:
  virtual ~VMSInterface() = default;

  virtual void login(const std::string &ip, unsigned int port,
                     const std::string &username,
                     const std::string &password) = 0;

  virtual void logout() = 0;

  virtual int session_id() = 0;

  virtual std::vector<Device> nvr_list(unsigned int max) = 0;

  virtual std::vector<Device> camera_list(unsigned int max) = 0;

  virtual std::vector<Record> recording_list(const std::string &camera_code,
                                             const std::string &start_time,
                                             const std::string &end_time,
                                             unsigned int max) = 0;

  virtual std::string playback(const std::string &camera_code,
                               const std::string &nvr_code,
                               const std::string &start_time,
                               const std::string &end_time) = 0;

  virtual std::string live_stream(const std::string &camera_code,
                                  const std::string &nvr_code,
                                  const std::string &transport) = 0;
};

}  // namespace vms

#endif
