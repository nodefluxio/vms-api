#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

#include "windows.h"
#include "IVS_SDK.h"
#include "hw_ivs.h"
#include "../../external/spdlog/spdlog.h"

namespace hwivs = vms::hwivs;

auto hw_console = spdlog::stdout_color_mt("huawei");

hwivs::HuaweiIVS::HuaweiIVS(const std::string &log_path) {
  IVS_SDK_SetLogPath(log_path.c_str());
  IVS_SDK_Init();
}

hwivs::HuaweiIVS::~HuaweiIVS() {
  if (_logged_in) {
    logout();
  }

  IVS_SDK_Cleanup();
};

void hwivs::HuaweiIVS::login(const std::string &ip, unsigned int port,
                             const std::string &username,
                             const std::string &password) {
  hw_console->info("Logging in to Huawei VMS...");
  IVS_LOGIN_INFO login_info;
  strncpy(login_info.cUserName, username.c_str(), IVS_IP_LEN);
  strncpy(login_info.pPWD, password.c_str(), IVS_PWD_LEN);
  strncpy(login_info.stIP.cIP, ip.c_str(), IVS_IP_LEN);
  strncpy(login_info.cDomainName, "", IVS_DOMAIN_LEN);
  strncpy(login_info.cMachineName, "", IVS_MACHINE_NAME_LEN);
  login_info.stIP.uiIPType = IP_V4;
  login_info.uiPort = port;
  login_info.uiClientType = 1;
  login_info.uiLoginType = LOGIN_BASIC;

  int return_code = IVS_SDK_Login(&login_info, &_session_id);

  if (return_code != IVS_SUCCEED) {
    throw std::runtime_error("Login failed. Error code: " +
                             std::to_string(return_code));
  }

  _logged_in = true;
}

void hwivs::HuaweiIVS::logout() {
  hw_console->info("Logging out from Huawei VMS...");
  int return_code = IVS_SDK_Logout(_session_id);

  if (return_code != IVS_SUCCEED) {
    hw_console->error("Logout failed. Error code: " +
                      std::to_string(return_code));
  }

  _logged_in = false;
}

int hwivs::HuaweiIVS::session_id() { return _session_id; }

std::vector<vms::Device> hwivs::HuaweiIVS::nvr_list(unsigned int max) {
  IVS_INDEX_RANGE range = {1, max};
  unsigned int buffer_size = sizeof(IVS_DEVICE_BRIEF_INFO_LIST) +
                             (max - 1) * sizeof(IVS_DEVICE_BRIEF_INFO);

  auto nvr_list = std::make_unique<char[]>(buffer_size);
  auto nvr_list_ptr = (IVS_DEVICE_BRIEF_INFO_LIST *)nvr_list.get();

  int return_code =
      IVS_SDK_GetNVRList(_session_id, "", 1, &range, nvr_list_ptr, buffer_size);

  if (return_code != IVS_SUCCEED) {
    throw std::runtime_error("Failed to list nvr. Error code: " +
                             std::to_string(return_code));
  }

  std::vector<vms::Device> nvrs;
  for (unsigned int i = 0; i < nvr_list_ptr->uiTotal; i++) {
    std::string name = nvr_list_ptr->stDeviceBriefInfo[i].stDeviceInfo.cName;
    std::string code = nvr_list_ptr->stDeviceBriefInfo[i].stDeviceInfo.cCode;
    std::string ip = nvr_list_ptr->stDeviceBriefInfo[i].stDeviceInfo.stIP.cIP;
    unsigned int port = nvr_list_ptr->stDeviceBriefInfo[i].stDeviceInfo.uiPort;

    vms::Device nvr{name, ip, code, port};
    nvrs.push_back(nvr);
  }

  return nvrs;
}

std::vector<vms::Device> hwivs::HuaweiIVS::camera_list(unsigned int max) {
  IVS_INDEX_RANGE index_range = {1, max};
  unsigned int buffer_size = sizeof(IVS_CAMERA_BRIEF_INFO_LIST) +
                             (max - 1) * sizeof(IVS_CAMERA_BRIEF_INFO);

  auto camera_list = std::make_unique<char[]>(buffer_size);
  auto camera_list_ptr = (IVS_CAMERA_BRIEF_INFO_LIST *)camera_list.get();

  int return_code =
      IVS_SDK_GetDeviceList(_session_id, DEVICE_TYPE_CAMERA, &index_range,
                            camera_list_ptr, buffer_size);

  if (return_code != IVS_SUCCEED) {
    throw std::runtime_error("Failed to list camera. Error code: " +
                             std::to_string(return_code));
  }

  std::vector<vms::Device> cameras;
  for (unsigned int i = 0; i < camera_list_ptr->uiTotal; i++) {
    std::string code = camera_list_ptr->stCameraBriefInfo[i].cCode;
    std::string name = camera_list_ptr->stCameraBriefInfo[i].cName;
    std::string ip = camera_list_ptr->stCameraBriefInfo[i].cDevIp;

    vms::Device camera{name, ip, code};

    cameras.push_back(camera);
  }

  return cameras;
}

unsigned int _get_num_of_records(IVS_RECORD_INFO_LIST *recording_list_ptr) {
  unsigned int num_of_records = (recording_list_ptr->stIndexRange.uiToIndex -
                                 recording_list_ptr->stIndexRange.uiToIndex) +
                                1;

  if (num_of_records > recording_list_ptr->uiTotal) {
    num_of_records = recording_list_ptr->uiTotal;
  }

  return num_of_records;
}

std::vector<vms::Record> hwivs::HuaweiIVS::recording_list(
    const std::string &camera_code, const std::string &start_time,
    const std::string &end_time, unsigned int max) {
  IVS_TIME_SPAN time_span = {0};
  strncpy(time_span.cStart, start_time.c_str(), start_time.size());
  strncpy(time_span.cEnd, end_time.c_str(), end_time.size());

  IVS_INDEX_RANGE index_range = {1, max};

  unsigned int buffer_size =
      sizeof(IVS_RECORD_INFO_LIST) + (max - 1) * sizeof(IVS_RECORD_INFO);
  auto recording_list = std::make_unique<char[]>(buffer_size);
  auto recording_list_ptr = (IVS_RECORD_INFO_LIST *)recording_list.get();

  int return_code =
      IVS_SDK_GetRecordList(_session_id, camera_code.c_str(), 0, &time_span,
                            &index_range, recording_list_ptr, buffer_size);

  if (return_code != IVS_SUCCEED) {
    throw std::runtime_error("Failed to list recording. Error code: " +
                             std::to_string(return_code));
  }

  unsigned int num_of_records = _get_num_of_records(recording_list_ptr);

  std::vector<vms::Record> records;
  for (unsigned int i = 0; i < num_of_records; i++) {
    vms::Record record;
    record.start_time = recording_list_ptr->stRecordInfo[i].stTime.cStart;
    record.end_time = recording_list_ptr->stRecordInfo[i].stTime.cEnd;

    records.push_back(record);
  }

  return records;
}

IVS_URL_MEDIA_PARAM create_url_media_param(IVS_SERVICE_TYPE service_type,
                                           const std::string &nvr_code,
                                           const std::string &start_time = {},
                                           const std::string &end_time = {}) {
  IVS_URL_MEDIA_PARAM param;

  param.ServiceType = service_type;
  param.AudioDecType = AUDIO_DEC_G711U;
  param.BroadCastType = BROADCAST_UNICAST;
  param.PackProtocolType = PACK_PROTOCOL_ES;
  param.ProtocolType = PROTOCOL_RTP_OVER_UDP;
  param.StreamType = STREAM_TYPE_MAIN;
  param.TransMode = MEDIA_TRANS;
  param.VideoDecType = VIDEO_DEC_H265;
  param.iClientType = 1;
  strncpy(param.cNVRCode, nvr_code.c_str(), nvr_code.size());

  if (!start_time.empty()) {
    strncpy(param.stTimeSpan.cStart, start_time.c_str(), start_time.size());
    strncpy(param.stTimeSpan.cEnd, end_time.c_str(), end_time.size());
  }

  return param;
}

std::string hwivs::HuaweiIVS::playback(const std::string &camera_code,
                                       const std::string &nvr_code,
                                       const std::string &start_time,
                                       const std::string &end_time) {
  auto param = create_url_media_param(SERVICE_TYPE_PLAYBACK, nvr_code,
                                      start_time, end_time);

  auto rtsp_url = std::make_unique<char[]>(1024);
  auto rtsp_url_ptr = rtsp_url.get();

  int return_code = IVS_SDK_GetRtspURL(_session_id, camera_code.c_str(), &param,
                                       rtsp_url_ptr, 1024);

  if (return_code != IVS_SUCCEED) {
    throw std::runtime_error(
        "Failed to get RTSP URL for live streaming . Error code: " +
        std::to_string(return_code));
  }

  return std::string(rtsp_url.get());
}

std::vector<IVS_STREAM_INFO> get_stream_info(int session_id,
                                             const std::string &camera_code) {
  IVS_CAMERA_STREAM_CFG stream_config = {0};

  int return_code = IVS_SDK_GetDeviceConfig(
      session_id, camera_code.c_str(), CONFIG_CAMERA_STREAM_CFG, &stream_config,
      sizeof(stream_config));

  if (return_code != IVS_SUCCEED) {
    throw std::runtime_error("Failed to get stream info. Error code: " +
                             std::to_string(return_code));
  }

  std::vector<IVS_STREAM_INFO> stream_info;
  for (unsigned int i = 0; i < stream_config.uiStreamInfoNum; i++) {
    stream_info.push_back(stream_config.stStreamInfo[i]);
  }

  return stream_info;
}

std::string hwivs::HuaweiIVS::live_stream(const std::string &camera_code,
                                          const std::string &nvr_code,
                                          const std::string &transport) {
  auto stream_info = get_stream_info(_session_id, camera_code);
  auto param = create_url_media_param(SERVICE_TYPE_REALVIDEO, nvr_code);

  if (transport == "tcp") {
    param.ProtocolType = PROTOCOL_RTP_OVER_TCP;
  }

  for (auto info : stream_info) {
    if (info.uiStreamType == STREAM_TYPE_MAIN) {
      param.VideoDecType = static_cast<IVS_VIDEO_DEC_TYPE>(info.uiEncodeType);
      break;
    }
  }

  auto rtsp_url = std::make_unique<char[]>(1024);
  auto rtsp_url_ptr = rtsp_url.get();

  int return_code = IVS_SDK_GetRtspURL(_session_id, camera_code.c_str(), &param,
                                       rtsp_url_ptr, 1024);

  if (return_code != IVS_SUCCEED) {
    throw std::runtime_error(
        "Failed to get RTSP URL for live streaming . Error code: " +
        std::to_string(return_code));
  }

  return std::string(rtsp_url.get());
}

void hwivs::HuaweiIVS::sdk_playback(const std::string &camera_code,
                                      const std::string &start_time,
                                      const std::string &end_time) {
  // Playback parameter
  IVS_PLAYBACK_PARAM param = {0};

  strncpy_s(param.stTimeSpan.cStart, start_time.c_str(), IVS_TIME_LEN);
  strncpy_s(param.stTimeSpan.cEnd, end_time.c_str(), IVS_TIME_LEN);
  param.uiProtocolType = 2;  // 1:UDP, 2:TCP
  param.fSpeed = 1.0;

  int return_code = IVS_SDK_StartPlatformPlayBack(_session_id, camera_code.c_str(),
                                                  &param, _show_box, &_playback_handle);

  if (return_code != IVS_SUCCEED) {
    throw std::runtime_error("Failed to start platform playback. Error code: " +
                              std::to_string(return_code));
  }
}
