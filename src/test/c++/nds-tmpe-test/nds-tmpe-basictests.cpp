#include <gtest/gtest.h>
#include <nds3/nds.h>
#include <nds3/ndsTestFactory.h>
#include <unistd.h>
#include <cstddef>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

#include "nds-tmpe-fixture.hpp"
#include "nds-tmpeDAQ.hpp"

using BasicTests = ndstmpeFixture;

static constexpr size_t READ_VALUES = 200;

TEST_F(BasicTests, PlainPVTest) {
    int32_t val;
    timespec time;

    val = 1000;
    std::cout << "Setting sampleEOC to " << val << "...";
    iface->writeCSValue("/tmpeDevice-SampleEOCSet", time, val);
    std::cout << "OK" << std::endl;

    std::cout << "Reading sampleEOC: ";
    iface->readCSValue("/tmpeDevice-SampleEOCSet", &time, &val);
    std::cout << val << std::endl;
    ASSERT_EQ(val, 1000);
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

    const std::vector<float>* valPtr        = nullptr;
    const timespec* tsPtr                   = nullptr;
    for (int i = 0; i < readInt; i++) {
        iface->getPushedVectorFloat("/tmpeDevice-DAQ.Data", tsPtr, valPtr);
        ASSERT_NE(valPtr, nullptr) << "Data pointer is null";
        ASSERT_FALSE(valPtr->empty()) << "Obtained empty data block";
    }
    const std::vector<float> lastBlock = *valPtr;  // Copy vector

    std::ofstream output_file("output.csv");
    ASSERT_TRUE(output_file.is_open()) << "Error opening file for writing.";
    output_file << "value\n";
    output_file << std::setprecision(std::numeric_limits<float>::max_digits10);
    for (float sample : lastBlock) {
        output_file << sample << "\n";
    }
    output_file.close();
}