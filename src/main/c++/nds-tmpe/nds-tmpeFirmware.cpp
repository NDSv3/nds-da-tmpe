#include "nds-tmpeFirmware.hpp"
#include <nds3/exceptions.h>
#include <nds3/pvDelegateIn.h>
#include <ctime>

using nds_tmpe::ndstmpeFirmware;

static constexpr size_t FIRMWARE_MAX_ELEMENTS = 20;

// clang-format off
ndstmpeFirmware::ndstmpeFirmware(const std::string& name, ndsda::ndsdaDevice& daDevice)
    : nds::Firmware(name, FIRMWARE_MAX_ELEMENTS,
        [this]{ this->switchOn(); },  // Switch on
        [this]{ this->switchOff();},  // Switch off
        [this]{ this->start();    },  // Start
        [this]{ this->stop();     },  // Stop
        [this]{ this->recover();  },  // Recover
        [this](nds::state_t c, nds::state_t g, nds::state_t d) -> bool {
            return this->allowStateChange(c, g, d);
        }, // Allow state change
        [this](const timespec& t, const std::string& str) {
            this->firmwarePathWriter(t, str);
        }, // String writer fn
        nds::autoEnable_t::on
    ), daDevice(daDevice) { }
// clang-format on

void ndstmpeFirmware::switchOn() {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    this->setFirmwarePath(ts, FIRMWARE_PATH);
    this->setFirmwareVersion(ts, FIRMWARE_VERSION);
    this->setHardwareRevision(ts, HARDWARE_REVISION);
    this->setSerialNumber(ts, SERIAL_NUMBER);
    this->setDeviceModel(ts, DEVICE_MODEL);
    this->setDeviceType(ts, DEVICE_TYPE);
    this->setDriverVersion(ts, DRIVER_VERSION);
    this->setChassisNumber(ts, CHASSIS_NUMBER);
    this->setSlotNumber(ts, SLOT_NUMBER);

    auto res = this->daDevice.readPV<int32_t>("C2HEna");
    if (res) {
        this->setFirmwareStatus(ts, res.assume_value().first);
    } else {
        throw nds::NdsError(res.assume_error().what());
    }
}
void ndstmpeFirmware::switchOff() {}
void ndstmpeFirmware::start() {}
void ndstmpeFirmware::stop() {}
void ndstmpeFirmware::recover() {}
bool ndstmpeFirmware::allowStateChange(nds::state_t currentState, nds::state_t globalState, nds::state_t desiredState) {
    (void)currentState;
    (void)globalState;

    return desiredState <= nds::state_t::on;  // Don't allow running
}
void ndstmpeFirmware::firmwarePathWriter(const timespec& t, const std::string& str) {
    (void)t;
    (void)str;
}
