# VMS-API

VMS-API is a multi-vendor Video Management System API.

The API supports these operations:

- List camera
- List recording
- Playback
- Live Stream

For more detail, please read the [API documentation](https://github.com/nodefluxio/vms-api/wiki/API-Documentation).

### Supported Device

- Huawei IVS

### How to Build

#### General Requirements

- CMake
- Boost library

#### Vendor Specific Requirements
- **Huawei IVS**: Set IVS_SDK_PATH environment variable to point to the [IVS SDK](http://developer.huawei.com/ict/en/rescenter/CMDA_FIELD_VCN?developlan=Other) path.


#### Building

```
mkdir build
cd build
cmake ..
```
Build `hwivs` project first, then build `vmsapi` project in Visual Studio
