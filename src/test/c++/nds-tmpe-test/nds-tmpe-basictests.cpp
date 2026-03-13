#include <gtest/gtest.h>
#include <nds3/nds.h>
#include <nds3/ndsTestFactory.h>
#include <cstddef>
#include <ctime>
#include <fstream>
#include <iostream>

#include "nds-tmpe-fixture.hpp"

using BasicTests = ndstmpeFixture;

static constexpr size_t READ_VALUES = 200;

TEST_F(BasicTests, PlainPVTest) {
    int32_t val;
    timespec time;
    std::vector<std::uint32_t> values;
    std::vector<uint64_t> assembled_values;

    values.reserve(READ_VALUES);
    assembled_values.reserve(READ_VALUES / 2);

    val = 1000;
    std::cout << "Setting sampleEOC to " << val << "...";
    iface->writeCSValue("/tmpeDevice-SampleEOCSet", time, val);
    std::cout << "OK" << std::endl;

    std::cout << "Reading sampleEOC: ";
    iface->readCSValue("/tmpeDevice-SampleEOCSet", &time, &val);
    std::cout << val << std::endl;
    ASSERT_EQ(val, 1000);

    val = 1;
    std::cout << "Setting C2HEnaSet to " << val << "...";
    iface->writeCSValue("/tmpeDevice-C2HEnaSet", time, val);
    std::cout << "OK" << std::endl;

    std::cout << "Reading DMASamplesGet...";
    iface->readCSValue("/tmpeDevice-DMASamplesGet", &time, &values);
    std::cout << "OK" << std::endl;

    val = 0;
    std::cout << "Setting C2HEnaSet to " << val << "...";
    iface->writeCSValue("/tmpeDevice-C2HEnaSet", time, val);
    std::cout << "OK" << std::endl;

    for (size_t i = 0; i < READ_VALUES / 2; i++) {
        assembled_values.push_back((static_cast<uint64_t>(values[2 * i + 1]) << 32) | values[2 * i]);
    }

    // Write to a binary file
    std::ofstream output_file("output.bin", std::ios::binary);
    if (!output_file) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }
    for (const auto& v : assembled_values) {
        output_file.write(reinterpret_cast<const char*>(&v), sizeof(v));
    }
    output_file.close();
}

TEST_F(BasicTests, FirmwarePVTest) {
    timespec ts;
    std::string readStr;
    int32_t readInt;

    iface->writeCSValue("/tmpeDevice-FW.StateMachine.setState", ts, static_cast<std::int32_t>(nds::state_t::on));

    iface->readCSValue("/tmpeDevice-FW.Version", &ts, &readStr);
    std::cout << "Read " << readStr << " from " << "/tmpeDevice-FW.Version" << std::endl;
    iface->readCSValue("/tmpeDevice-FW.Status", &ts, &readInt);
    std::cout << "Read " << readInt << " from " << "/tmpeDevice-FW.Status" << std::endl;
    iface->readCSValue("/tmpeDevice-FW.HWRevision", &ts, &readStr);
    std::cout << "Read " << readStr << " from " << "/tmpeDevice-FW.HWRevision" << std::endl;
    iface->readCSValue("/tmpeDevice-FW.SerialNumber", &ts, &readStr);
    std::cout << "Read " << readStr << " from " << "/tmpeDevice-FW.SerialNumber" << std::endl;
    iface->readCSValue("/tmpeDevice-FW.Model", &ts, &readStr);
    std::cout << "Read " << readStr << " from " << "/tmpeDevice-FW.Model" << std::endl;
    iface->readCSValue("/tmpeDevice-FW.Type", &ts, &readStr);
    std::cout << "Read " << readStr << " from " << "/tmpeDevice-FW.Type" << std::endl;
    iface->readCSValue("/tmpeDevice-FW.DriverVersion", &ts, &readStr);
    std::cout << "Read " << readStr << " from " << "/tmpeDevice-FW.DriverVersion" << std::endl;
    iface->readCSValue("/tmpeDevice-FW.ChassisNumber", &ts, &readInt);
    std::cout << "Read " << readInt << " from " << "/tmpeDevice-FW.ChassisNumber" << std::endl;
    iface->readCSValue("/tmpeDevice-FW.SlotNumber", &ts, &readInt);
    std::cout << "Read " << readInt << " from " << "/tmpeDevice-FW.SlotNumber" << std::endl;
    // iface->readCSValue("/tmpeDevice-FW-FilePath", &ts, &readStr);
}
