#ifndef SERVER_H
#define SERVER_H

#include "crow/crow_all.h"
#include "hw_ivs.h"
#include "session.h"

namespace vms {
namespace api {

class Server {
 public:
  Server();

  void run(int port = 8000);

 private:
  static std::shared_ptr<vms::hwivs::HuaweiIVS> _login(
      const crow::json::rvalue body);

  static void _camera_list(const crow::request &req, crow::response &res);

  static void _recording_list(const crow::request &req, crow::response &res);

  static void _playback(const crow::request &req, crow::response &res);

  static void _live_stream(const crow::request &req, crow::response &res);

  crow::SimpleApp _app;
};

}  // namespace api
}  // namespace vms

#endif
