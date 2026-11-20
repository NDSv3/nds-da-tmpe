// Author: David Andrino-Izquierdo <david.andrino@upm.es>
// Copyright 2026 I2A2-UPM
// SPDX-License-Identifier: Apache-2.0

#ifndef NDS_TMPE_FIRMWARE_HPP
#define NDS_TMPE_FIRMWARE_HPP

#include <cstdint>
#include <ndsdaDevice.hpp>
#include <nds3/firmware.h>

static constexpr int32_t CHASSIS_NUMBER        = 0;
static constexpr int32_t SLOT_NUMBER           = 0;
static constexpr int32_t FIRMWARE_STATUS       = 0;
static constexpr std::string FIRMWARE_PATH     = "/firmware/path";
static constexpr std::string FIRMWARE_VERSION  = "v0.1.0";
static constexpr std::string HARDWARE_REVISION = "Rev. 0";
static constexpr std::string SERIAL_NUMBER     = "tmpe-0001";
static constexpr std::string DEVICE_MODEL      = "TMPE627";
static constexpr std::string DEVICE_TYPE       = "mPCIe FPGA Card";
static constexpr std::string DRIVER_VERSION    = "v0.1.0";

namespace nds_tmpe {
class ndstmpeFirmware : public nds::Firmware {
public:
    ndstmpeFirmware(const std::string& name, ndsda::ndsdaDevice& daDevice);

private:
    ndsda::ndsdaDevice& daDevice;

    void switchOn();
    void switchOff();
    void start();
    void stop();
    void recover();
    bool allowStateChange(nds::state_t currentState, nds::state_t globalState, nds::state_t desiredState);
    void firmwarePathWriter(const timespec& t, const std::string& str);
};
}  // namespace nds_tmpe

#endif  // NDS_TMPE_FIRMWARE_HPP
