#include <iostream>
#include <stdexcept>

#include "hw_ivs.h"
#include "session.h"

namespace vms {
namespace api {

std::shared_ptr<vms::VMSInterface> Session::login(
    const std::string &ip, const std::string &username,
    const std::string &password, const std::string &vendor) {
  auto session = _get_session(ip, username, password);

  if (session) {
    return session;
  }

  try {
    std::shared_ptr<vms::VMSInterface> vms = _create_vendor_vms(vendor);
    vms->login(ip, 9900, username, password);

    std::unique_lock<std::mutex> lock{_mutex};
    _add_session(ip, username, password, vms);

    return vms;
  } catch (std::runtime_error &) {
    throw;
  }

}

std::shared_ptr<vms::VMSInterface> Session::_create_vendor_vms(
    const std::string &vendor) {
  std::shared_ptr<vms::VMSInterface> vms{nullptr};

  if (vendor == "huawei") {
    vms = std::make_shared<vms::hwivs::HuaweiIVS>("./log");
  }

  return vms;
}

void Session::logout(const std::string &ip, const std::string &username,
                     const std::string &password) {
  auto session = _get_session(ip, username, password);

  if (session) {
    try {
      session->logout();
      _delete_session(ip, username, password);
    } catch (std::runtime_error &) {
      _delete_session(ip, username, password);
      throw;
    }
  } else {
    throw std::invalid_argument("IP address does not exists in the session.");
  }
}

std::string Session::_session_key(const std::string &ip,
                                  const std::string &username,
                                  const std::string &password) {
  return username + std::string(":") + password + std::string("@") + ip;
}

void Session::_add_session(const std::string &ip,
                           const std::string &username,
                           const std::string &password,
                           std::shared_ptr<vms::VMSInterface> vms) {
  auto key = _session_key(ip, username, password);
  _sessions.insert({key, vms});
}

std::shared_ptr<vms::VMSInterface> Session::_get_session(
    const std::string &ip,
    const std::string &username,
    const std::string &password) {
  std::unique_lock<std::mutex> lock{_mutex};

  auto key = _session_key(ip, username, password);
  auto session = _sessions.find(key);

  if (session == _sessions.end()) {
    return nullptr;
  }

  return session->second;
}

void Session::_delete_session(const std::string &ip,
                              const std::string &username,
                              const std::string &password) {
  auto key = _session_key(ip, username, password);

  _sessions.erase(key);
}

}
}
