#include <iostream>
#include <stdexcept>

#include "session.h"

namespace api = vms::api;

std::shared_ptr<vms::VMSInterface> api::Session::login(
    const std::string &ip, const std::string &username,
    const std::string &password, const std::string &vendor) {
  auto session = _get_session(ip);

  if (session) {
    if (_match(ip, username, password)) {
      return session;
    } else {
      throw std::invalid_argument("Username and password does not match.");
    }
  } else {
    try {
      std::shared_ptr<vms::VMSInterface> vms = _create_vendor_vms(vendor);
      vms->login(ip, 9900, username, password);

      std::unique_lock<std::mutex> lock{_mutex};
      _sessions.insert({ip, vms});
      _auths.insert({ip, {username, password}});

      return vms;
    } catch (std::runtime_error) {
      throw;
    }
  }
}

std::shared_ptr<vms::VMSInterface> api::Session::_create_vendor_vms(
    const std::string &vendor) {
  std::shared_ptr<vms::VMSInterface> vms{nullptr};

  if (vendor == "huawei") {
    // vms = std::make_shared<vms::hwivs::HuaweiIVS>("./log");
  }

  return vms;
}

void api::Session::logout(const std::string &ip) {
  auto session = _get_session(ip);

  if (session) {
    try {
      session->logout();
    } catch (std::runtime_error) {
      throw;
    }
  } else {
    throw std::invalid_argument("IP address does not exists in the session.");
  }
}

bool api::Session::_match(const std::string &ip, const std::string &username,
                          const std::string &password) {
  auto auth = _auths.find(ip);

  if (auth != _auths.end()) {
    if (auth->second.username == username &&
        auth->second.password == password) {
      return true;
    }
  }

  return false;
}

std::shared_ptr<vms::VMSInterface> api::Session::_get_session(
    const std::string &ip) {
  std::unique_lock<std::mutex> lock{_mutex};
  auto session = _sessions.find(ip);

  if (session == _sessions.end()) {
    return nullptr;
  }

  return session->second;
}
