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
make
```

#### VMS Client
For more detail, please read the [VMS CLient](https://github.com/nodefluxio/vms-api/wiki/VMS-Client).

## Changelog

### v0.1.8

- Improve session management
- Add more informative error message to response and log
