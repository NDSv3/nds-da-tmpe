#include <gtest/gtest.h>
#include <nds3/nds.h>
#include <nds3/ndsTestFactory.h>
#include <unistd.h>
#include <cstddef>
#include <ctime>
#include <fstream>
#include <iostream>

#include "nds-tmpe-fixture.hpp"
#include "nds-tmpeDAQ.hpp"

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

TEST_F(BasicTests, DAQTest) {
    timespec ts;
    int32_t readInt;
    double readDouble;

    iface->writeCSValue("/tmpeDevice-DAQ.StateMachine.setState", ts, static_cast<std::int32_t>(nds::state_t::on));

    constexpr double selectedFreq = static_cast<double>(100000);
    std::cout << "[TEST] Setting DAQ sampling frequency to 100 kHz...";
    iface->writeCSValue("/tmpeDevice-DAQ.SamplingRate", ts, selectedFreq);
    iface->readCSValue("/tmpeDevice-DAQ.SamplingRate_RBV", &ts, &readDouble);
    ASSERT_EQ(readDouble, selectedFreq) << "Incorrect sampling frequency";
    std::cout << "OK" << std::endl;

    constexpr int32_t selectedRange = static_cast<std::int32_t>(nds_tmpe::tmpeDAQ::ADCGainRange::PLUS_10);
    std::cout << "[TEST] Setting DAQ range to +10V...";
    iface->writeCSValue("/tmpeDevice-DAQ.Range", ts, selectedRange);
    iface->readCSValue("/tmpeDevice-DAQ.Range_RBV", &ts, &readInt);
    ASSERT_EQ(readInt, selectedRange) << "Incorrect range";
    std::cout << "OK" << std::endl;

    std::cout << "[TEST] Starting acquisition for 1 second...";
    iface->writeCSValue("/tmpeDevice-DAQ.StateMachine.setState", ts, static_cast<std::int32_t>(nds::state_t::running));
    ::sleep(1);
    iface->writeCSValue("/tmpeDevice-DAQ.StateMachine.setState", ts, static_cast<std::int32_t>(nds::state_t::on));
    std::cout << "OK" << std::endl;

    iface->readCSValue("/tmpeDevice-DAQ.NumberOfPushedDataBlocks", &ts, &readInt);
    ASSERT_GT(readInt, 0) << "No data blocks were pushed";
    std::cout << "[TEST] Read " << readInt << " data blocks" << std::endl;

    const std::vector<std::int32_t>* valPtr = nullptr;
    const timespec* tsPtr                   = nullptr;
    for (int i = 0; i < readInt; i++) {
        iface->getPushedVectorInt32("/tmpeDevice-DAQ.Data", tsPtr, valPtr);
        ASSERT_NE(valPtr, nullptr) << "Data pointer is null";
        ASSERT_FALSE(valPtr->empty()) << "Obtained empty data block";
    }
    const std::vector<std::int32_t> lastBlock = *valPtr;  // Copy vector

    std::ofstream output_file("daq_output.bin", std::ios::binary);
    ASSERT_TRUE(output_file.is_open()) << "Error opening file for writing.";
    for (const auto& v : lastBlock) {
        output_file.write(reinterpret_cast<const char*>(&v), sizeof(v));
    }
    output_file.close();
}