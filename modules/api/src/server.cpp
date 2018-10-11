#include <vms_exception.h>
#include "server.h"
#include "spdlog/spdlog.h"

namespace vms {
namespace api {

Session Server::_session;
std::shared_ptr<spdlog::logger> Server::_logger = spdlog::stdout_color_mt("server");

Server::Server() {
  CROW_ROUTE(_app, "/camera-list").methods("POST"_method)(_camera_list);

  CROW_ROUTE(_app, "/recording-list").methods("POST"_method)(_recording_list);

  CROW_ROUTE(_app, "/playback").methods("POST"_method)(_playback);

  CROW_ROUTE(_app, "/live-stream").methods("POST"_method)(_live_stream);

  CROW_ROUTE(_app, "/health-check").methods("GET"_method)(_health_check);
}

void Server::run(int port) { _app.port(port).multithreaded().run(); }

std::shared_ptr<vms::VMSInterface> Server::_login(
    const crow::json::rvalue &body) {
  const std::string ip = body["ip"].s();
  const std::string username = body["username"].s();
  const std::string password = body["password"].s();
  const std::string vendor = body["vendor"].s();

  try {
    return _session.login(ip, username, password, vendor);
  } catch (std::exception &) {
    throw;
  }
}

void Server::_logout_on_user_error(int code, const crow::json::rvalue &body) {
  if (code == 109100000 || code == 109100001 || code == 999115312) {
    const std::string ip = body["ip"].s();
    const std::string username = body["username"].s();
    const std::string password = body["password"].s();

    _session.logout(ip, username, password);
  }
}

void Server::_camera_list(const crow::request &req, crow::response &res) {
  crow::json::wvalue response;
  response["ok"] = false;

  try {
    auto data = crow::json::load(req.body);
    auto vms = _login(data);
    auto cameras = vms->camera_list(data["max_list"].i());

    response["ok"] = true;
    response["code"] = "camera/success";

    for (unsigned int i = 0; i < cameras.size(); i++) {
      response["camera"][i]["ip"] = cameras.at(i).ip();
      response["camera"][i]["name"] = cameras.at(i).name();
      response["camera"][i]["code"] = cameras.at(i).code();
    }

  } catch (std::invalid_argument &err) {
    _logger->error(err.what());
    response["code"] = "camera/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (vms::VMSException &err) {
    _logger->error(err.what());
    response["code"] = "camera/failed";
    response["message"] = err.what();
    res.code = 400;

    _logout_on_user_error(err.code(), crow::json::load(req.body));
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

void Server::_recording_list(const crow::request &req,
                             crow::response &res) {
  crow::json::wvalue response;
  response["ok"] = false;

  try {
    auto data = crow::json::load(req.body);
    auto vms = _login(data);

    std::vector<vms::Record> records =
        vms->recording_list(data["camera_code"].s(), data["start_time"].s(),
                            data["end_time"].s(), data["max_list"].i());

    response["ok"] = true;
    response["code"] = "recording/success";

    for (unsigned int i = 0; i < records.size(); i++) {
      response["records"][i]["start_time"] = records.at(i).start_time;
      response["records"][i]["end_time"] = records.at(i).end_time;
    }
  } catch (std::invalid_argument &err) {
    _logger->error(err.what());
    response["code"] = "recording/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (vms::VMSException &err) {
    _logger->error(err.what());
    response["code"] = "recording/failed";
    response["message"] = err.what();
    res.code = 400;

    _logout_on_user_error(err.code(), crow::json::load(req.body));
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

void Server::_playback(const crow::request &req, crow::response &res) {
  crow::json::wvalue response;
  response["ok"] = false;

  try {
    auto data = crow::json::load(req.body);
    auto vms = _login(data);

    std::string rtsp_url =
        vms->playback(data["camera_code"].s(), data["nvr_code"].s(),
                      data["start_time"].s(), data["end_time"].s());

    response["ok"] = true;
    response["code"] = "recording/success";
    response["url"] = rtsp_url;
  } catch (std::invalid_argument &err) {
    _logger->error(err.what());
    response["code"] = "playback/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (vms::VMSException &err) {
    _logger->error(err.what());
    response["code"] = "playback/failed";
    response["message"] = err.what();
    res.code = 400;

    _logout_on_user_error(err.code(), crow::json::load(req.body));
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

void Server::_live_stream(const crow::request &req, crow::response &res) {
  crow::json::wvalue response;
  response["ok"] = false;

  try {
    auto data = crow::json::load(req.body);
    auto vms = _login(data);

    std::string transport = "udp";
    if (data.has("transport") && data["transport"].s() == "tcp") {
      transport = "tcp";
    }

    std::string rtsp_url = vms->live_stream(data["camera_code"].s(),
                                            data["nvr_code"].s(), transport);

    response["ok"] = true;
    response["code"] = "live-stream/success";
    response["url"] = rtsp_url;
  } catch (std::invalid_argument &err) {
    _logger->error(err.what());
    response["code"] = "live-stream/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (vms::VMSException &err) {
    _logger->error(err.what());
    response["code"] = "live-stream/failed";
    response["message"] = err.what();
    res.code = 400;

    _logout_on_user_error(err.code(), crow::json::load(req.body));
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

void Server::_health_check(const crow::request &req, crow::response &res) {
  crow::json::wvalue response;
  response["ok"] = true;

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

}
}
