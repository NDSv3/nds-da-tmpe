#include "nds-tmpe.hpp"
#include <nds3/definitions.h>
#include <nds3/impl/baseImpl.h>
#include <nds3/nds.h>
#include <nds3/impl/pvBaseInImpl.h>
#include <nds3/stateMachine.h>
#include <memory>
#include "nds-tmpeFirmware.hpp"

using nds_tmpe::tmpeDevice;

static const nds::namedParameters_t addDefaultParams(const nds::namedParameters_t& params) {
    auto newParams = params;  // Copy contents
    if (!newParams.contains("DeviceAlias")) {
        newParams.emplace("DeviceAlias", "../tmpe_daq_xdma");
    }
    if (!newParams.contains("DeviceMapFile")) {
        newParams.emplace("DeviceMapFile", "../tmpe-fwk.dmap");
    }
    if (!newParams.contains("NodeMapFile")) {
        newParams.emplace("NodeMapFile", "../tmpe-fwk.xml");
    }
    return newParams;
}

tmpeDevice::tmpeDevice(nds::Factory& factory, const std::string& deviceName, const nds::namedParameters_t& params)
    : ndsda::ndsdaDevice(factory, deviceName, addDefaultParams(params)),
      firmwareNode("FW", *this) {

    rootNode.addChild(firmwareNode);

    rootNode.initialize(this, factory);
}

NDS_DEFINE_DRIVER(tmpeDevice, tmpeDevice)
