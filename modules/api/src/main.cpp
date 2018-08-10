#include "IVS_SDK.h"
#include "server.h"

int main() {
  IVS_SDK_SetLogPath("./log");
  IVS_SDK_Init();

  vms::api::Server server;
  server.run();

  IVS_SDK_Cleanup();

  return 0;
}
