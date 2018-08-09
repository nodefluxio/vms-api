#include "server.h"
#include "spdlog/spdlog.h"

namespace api = vms::api;

auto console = spdlog::stdout_color_mt("server");
// api::Session session;

api::Server::Server() {
  CROW_ROUTE(_app, "/camera-list").methods("POST"_method)(_camera_list);

  CROW_ROUTE(_app, "/recording-list").methods("POST"_method)(_recording_list);

  CROW_ROUTE(_app, "/playback").methods("POST"_method)(_playback);

  CROW_ROUTE(_app, "/live-stream").methods("POST"_method)(_live_stream);

  CROW_ROUTE(_app, "/sdk_playback").methods("POST"_method)(_sdk_playback);
}

void api::Server::run(int port) { _app.port(port).multithreaded().run(); }

std::shared_ptr<vms::hwivs::HuaweiIVS> api::Server::_login(
    const crow::json::rvalue body) {
  const std::string ip = body["ip"].s();
  const std::string username = body["username"].s();
  const std::string password = body["password"].s();
  const std::string vendor = body["vendor"].s();

  try {
    auto vms = std::make_shared<vms::hwivs::HuaweiIVS>("./log");
    vms->login(ip, 9900, username, password);
    return vms;
    // return session.login(ip, username, password, vendor);
  } catch (std::exception) {
    throw;
  }
}

void api::Server::_camera_list(const crow::request &req, crow::response &res) {
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

  } catch (std::invalid_argument err) {
    console->error(err.what());
    response["code"] = "camera/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (std::runtime_error err) {
    console->error(err.what());
    response["code"] = "camera/failed";
    response["message"] = "Failed to find camera.";
    res.code = 400;
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

void api::Server::_recording_list(const crow::request &req,
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
  } catch (std::invalid_argument err) {
    console->error(err.what());
    response["code"] = "recording/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (std::runtime_error err) {
    console->error(err.what());
    response["code"] = "recording/failed";
    response["message"] = "Failed to find recording.";
    res.code = 400;
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

void api::Server::_playback(const crow::request &req, crow::response &res) {
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
  } catch (std::invalid_argument err) {
    console->error(err.what());
    response["code"] = "recording/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (std::runtime_error err) {
    console->error(err.what());
    response["code"] = "recording/failed";
    response["message"] = "Failed to find recording.";
    res.code = 400;
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

void api::Server::_live_stream(const crow::request &req, crow::response &res) {
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
  } catch (std::invalid_argument err) {
    console->error(err.what());
    response["code"] = "live-stream/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (std::runtime_error err) {
    console->error(err.what());
    response["code"] = "live-stream/failed";
    response["message"] = "Failed to find live stream.";
    res.code = 400;
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}

void api::Server::_sdk_playback(const crow::request &req, crow::response &res) {
  crow::json::wvalue response;
  response["ok"] = false;

  try {
    auto data = crow::json::load(req.body);
    auto vms = _login(data);

    vms -> sdk_playback(data["camera_code"].s(),
                          data["start_time"].s(),
                          data["end_time"].s());

    response["ok"] = true;
    response["code"] = "sdk_playback/success";
  } catch (std::invalid_argument err) {
    console->error(err.what());
    response["code"] = "sdk_playback/invalid-argument";
    response["message"] = "Argument is invalid.";
    res.code = 400;
  } catch (std::runtime_error err) {
    console->error(err.what());
    response["code"] = "sdk_playback/failed";
    response["message"] = "Failed to find sdk playback.";
    res.code = 400;
  }

  res.add_header("Content-Type", "application/json");
  res.write(crow::json::dump(response));
  res.end();
}
