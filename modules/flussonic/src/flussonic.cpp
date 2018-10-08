#include "flussonic.h"
#include <cpr/cpr.h>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include "spdlog/spdlog.h"

namespace vms {
namespace flussonic {
auto fls_console = spdlog::stdout_color_mt("flussonic");

Flussonic::Flussonic(const std::string &log_path) {}

void Flussonic::login(const std::string &ip, unsigned int port,
                      const std::string &username,
                      const std::string &password) {
  fls_console->info("Logging in to Flussonic VMS...");
  _ip = ip;
  _port = port;
  _logged_in = true;
  _id = username;
  _pass = password;
}

void Flussonic::logout() {
  fls_console->info("Logging out from Flussonic VMS...");
  _logged_in = false;
}

int Flussonic::session_id() { return _session_id; }

std::vector<Device> Flussonic::nvr_list(unsigned int max) {
  std::vector<vms::Device> nvrs;
  return nvrs;
}

std::vector<Device> Flussonic::camera_list(unsigned int max) {
  std::vector<vms::Device> cameras;
  return cameras;
}

std::vector<Record> Flussonic::recording_list(const std::string &camera_code,
                                              const std::string &start_time,
                                              const std::string &end_time,
                                              unsigned int max) {
  std::vector<vms::Record> records;
  return records;
}

std::string Flussonic::playback(const std::string &camera_code,
                                const std::string &nvr_code,
                                const std::string &start_time,
                                const std::string &end_time) {
  std::string url = create_url(_ip, camera_code, start_time, end_time);
  if (status_stream(_ip, _port, camera_code) == true) {
    return url;
  } else {
    throw std::invalid_argument("No available stream");
  }
}

std::string Flussonic::download(const std::string &camera_code,
                                const std::string &nvr_code,
                                const std::string &start_time,
                                const std::string &end_time) {
  std::string url = create_url(_ip, camera_code, start_time, end_time);

  auto r = cpr::Get(cpr::Url(url));

  // Create filename based on url, starting from camera_code
  std::size_t pos = url.find(camera_code);
  std::string filename = url.substr(pos) + ".mp4";
  filename = boost::replace_all_copy(filename, "/", "_");

  std::ofstream downloaded_file;
  downloaded_file.open(filename);
  downloaded_file << r.text;
  downloaded_file.close();

  return url;
}

std::string Flussonic::live_stream(const std::string &camera_code,
                                   const std::string &nvr_code,
                                   const std::string &transport) {
  std::string live_stream_url = "http://" + _ip + ":" + std::to_string(_port) +
                                "/" + camera_code + "/index.m3u8";

  return live_stream_url;
}

std::string Flussonic::create_url(const std::string &_ip,
                                  const std::string &camera_code,
                                  const std::string &start_time,
                                  const std::string &end_time) {
  std::string duration = std::to_string(to_unix_timestamp(end_time) -
                                        to_unix_timestamp(start_time));

  std::string start_time_vid = std::to_string(to_unix_timestamp(start_time));

  std::string url = _ip + ":" + std::to_string(_port) + "/" + camera_code +
                    "/" + "archive-" + start_time_vid + "-" + duration + ".mp4";
  return url;
}

/**
 * @brief unixtimestamp converter
 *
 * @param timestamp timestamp in string format
 * @return std::time_t unixtimestamp
 */
std::time_t Flussonic::to_unix_timestamp(const std::string &timestamp) {
  std::tm tm;
  std::string str = timestamp;

  std::string tahun = str.substr(0, 4);
  std::string bulan = str.substr(4, 2);
  std::string tanggal = str.substr(6, 2);

  std::string jam = str.substr(8, 2);
  std::string menit = str.substr(10, 2);
  std::string detik = str.substr(12, 2);

  tm.tm_year = stoi(tahun) - 1900;
  tm.tm_mon = stoi(bulan) - 1;
  tm.tm_mday = stoi(tanggal);

  tm.tm_hour = stoi(jam);
  tm.tm_min = stoi(menit);
  tm.tm_sec = stoi(detik);

  time_t time = mktime(&tm);

  return time;
}
bool Flussonic::status_stream(const std::string &_ip, const unsigned int &_port,
                              const std::string &camera_code) {
  std::string url = "http://" + _id + ":" + _pass + "@" + _ip + ":" +
                    std::to_string(_port) + "/flussonic/api/stream_health/" +
                    camera_code;

  auto r = cpr::Get(cpr::Url(url));

  return to_bool(r.text);
}

bool Flussonic::to_bool(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  std::istringstream is(str);
  bool boolean;
  is >> std::boolalpha >> boolean;
  return boolean;
}
}  // namespace flussonic
}  // namespace vms