// Author: David Andrino-Izquierdo <david.andrino@upm.es>
// Copyright 2026 I2A2-UPM
// SPDX-License-Identifier: Apache-2.0

#ifndef NDS_TMPE_FIXTURE_H
#define NDS_TMPE_FIXTURE_H

#include <nds3/nds.h>
#include <nds3/ndsTestInterface.h>
#include <gtest/gtest.h>

static const std::string NDSTMPE_DEVICEALIAS = "tmpe_daq_xdma";
static const std::string NDSTMPE_DMAPFILE    = "../tmpe-fwk.dmap";
static const std::string NDSTMPE_NODEMAPFILE = "../tmpe-fwk.xml";

class ndstmpeFixture : public testing::Test {
protected:
    nds::Factory factory;
    nds::namedParameters_t params;
    nds::tests::TestControlSystemInterfaceImpl* iface;

    ndstmpeFixture() : factory("test"), iface(nullptr) {
        params.emplace("DeviceAlias", NDSTMPE_DEVICEALIAS);
        params.emplace("DeviceMapFile", NDSTMPE_DMAPFILE);
        params.emplace("NodeMapFile", NDSTMPE_NODEMAPFILE);
    }
    // ~ndsDABasicDeviceFixture() override {}

    void SetUp() override {
        factory.createDevice("tmpeDevice", "tmpeDevice", params);
        iface = nds::tests::TestControlSystemInterfaceImpl::getInstance("tmpeDevice");
        ASSERT_NE(iface, nullptr);
    }
    void TearDown() override {
        factory.destroyDevice("tmpeDevice");
    }
};

#endif  //! NDS_TMPE_FIXTURE_H