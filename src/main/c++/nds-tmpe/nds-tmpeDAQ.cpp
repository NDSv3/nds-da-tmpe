#include "nds-tmpeDAQ.hpp"
#include <nds3/exceptions.h>
#include <cstdlib>
#include <ctime>
#include <ndsdaDevice.hpp>
#include "nds-tmpe.hpp"

namespace nds_tmpe {

static constexpr size_t DAQ_MAX_ELEMENTS = 20;
static constexpr size_t DAQ_BLOCK_SIZE   = 4096;
static constexpr int32_t CLK_BASE_FREQ   = 100'000'000;  // 100 MHz base clock frequency

// clang-format off
tmpeDAQ::tmpeDAQ(const std::string& name, ndsda::ndsdaDevice& daDevice)
    : nds::DataAcquisition<std::vector<int32_t>>(name, DAQ_MAX_ELEMENTS,
        [this](){ this->switchOn({}); },  // Switch on
        [this](){ this->switchOff({}); },  // Switch off
        [this](){ this->start({}); },  // Start
        [this](){ this->stop({}); },  // Stop
        [this](){ this->recover({}); },  // Recover
        [this](nds::state_t c, nds::state_t g, nds::state_t d) -> bool {
            return this->allowStateChange(c, g, d);
        }, // Allow state change
        [this](const timespec& time, int32_t value) { this->PV_Gain_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_Offset_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_Bandwidth_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_Resolution_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_Impedance_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_Coupling_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_SignalRefType_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_Ground_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_DMAEnable_Writer(time, value); },
        [this](const timespec& time, int32_t value) { this->PV_SamplingRate_Writer(time, value); },
        nds::autoEnable_t::on
    ),
    daDevice(daDevice), acquisitionThread(), pushedBlocks(0),
    rangePV("Range", [this](const timespec& time, int32_t value) { this->PV_Range_Writer(time, value); }),
    rangePV_RBV("Range_RBV") {
        this->addChild(rangePV);
        this->addChild(rangePV_RBV);
    }

// clang-format on

void tmpeDAQ::switchOn(const timespec& time) {
    // Read initial configuration from the device
    std::pair<int32_t, timespec> samplingRate, adcConfig, dacConfig;
    samplingRate = TRY_OR_THROW(this->daDevice.readPV<int32_t>("SampleEOC"));
    adcConfig    = TRY_OR_THROW(this->daDevice.readPV<int32_t>("ADCConfig"));
    dacConfig    = TRY_OR_THROW(this->daDevice.readPV<int32_t>("DACConfig"));

    // Default sampling rate to 1 kHz
    if (samplingRate.first == 0) {
        samplingRate.first = CLK_BASE_FREQ / 1000;
        this->daDevice.writePV<int32_t>("SampleEOC", samplingRate.first, samplingRate.second);
    }

    // Setup initial acquisition state
    this->daDevice.writePV("ADCConfig", adcConfig.first & ~(1 << 31), time);  // ADC ON
    this->daDevice.writePV("C2HEna", (int32_t)0, time);                       // DAQ OFF

    // Write config to RBV PVs
    this->setSamplingRate(samplingRate.second, (double)CLK_BASE_FREQ / static_cast<double>(samplingRate.first));
    this->rangePV_RBV.setValue(adcConfig.first & 0x7);
    this->rangePV_RBV.push(time, adcConfig.first & 0x7);
}

void tmpeDAQ::switchOff(const timespec& time) {
    (void)time;
}

void tmpeDAQ::start(const timespec& time) {
    this->isRunning.store(true);
    acquisitionThread = std::thread(&tmpeDAQ::threadBody, this);

    this->daDevice.writePV("C2HEna", (int32_t)1, time);
}

void tmpeDAQ::stop(const timespec& time) {
    this->isRunning.store(false);  // First stop acquisition to avoid block on half-filled buffer
    this->acquisitionThread.join();

    this->daDevice.writePV("C2HEna", (int32_t)0, time);
}

void tmpeDAQ::recover(const timespec& time) {
    (void)time;
}

bool tmpeDAQ::allowStateChange(nds::state_t currentState, nds::state_t /*globalState*/, nds::state_t desiredState) {
    switch (currentState) {
        case nds::state_t::off:
            return desiredState == nds::state_t::on;
        case nds::state_t::on:
            return desiredState == nds::state_t::running || desiredState == nds::state_t::off;
        case nds::state_t::running:
            return desiredState == nds::state_t::on;
        case nds::state_t::fault:
            return desiredState == nds::state_t::off;
        default:
            return false;
    }
}

// PV Writer implementations

void tmpeDAQ::PV_Gain_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
    throw nds::NdsError("PV_Gain_Writer is not implemented");
}

void tmpeDAQ::PV_Offset_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
    throw nds::NdsError("PV_Offset_Writer is not implemented");
}

void tmpeDAQ::PV_Bandwidth_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
    throw nds::NdsError("PV_Bandwidth_Writer is not implemented");
}

void tmpeDAQ::PV_Resolution_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
    throw nds::NdsError("PV_Resolution_Writer is not implemented");
}

void tmpeDAQ::PV_Impedance_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
    throw nds::NdsError("PV_Impedance_Writer is not implemented");
}

void tmpeDAQ::PV_Coupling_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
    throw nds::NdsError("PV_Coupling_Writer is not implemented");
}

void tmpeDAQ::PV_SignalRefType_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
    throw nds::NdsError("PV_SignalRefType_Writer is not implemented");
}

void tmpeDAQ::PV_Ground_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
    throw nds::NdsError("PV_Ground_Writer is not implemented");
}

void tmpeDAQ::PV_DMAEnable_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;
}

void tmpeDAQ::PV_SamplingRate_Writer(const timespec& time, int32_t value) {
    (void)time;
    (void)value;

    // Calculate the closest sampling rate
    int32_t closestRateEOC  = CLK_BASE_FREQ / value;
    int32_t closestRateFreq = CLK_BASE_FREQ / closestRateEOC;

    this->daDevice.writePV<int32_t>("SampleEOC", closestRateEOC, time);
    this->setSamplingRate(time, closestRateFreq);
}

void tmpeDAQ::PV_Range_Writer(const timespec& time, int32_t value) {
    auto res = this->daDevice.readPV<int32_t>("ADCConfig");

    if (res.has_value()) {
        this->daDevice.writePV(
            "ADCConfig",
            std::bit_cast<int32_t>((res.assume_value().first & 0xFFFFFFF8) | value),
            time
        );
    } else {
        throw nds::NdsError(res.assume_error().what());
    }
}

static void compactVector(std::vector<int32_t>& vec) {
    size_t writeIndex = 0;
    for (size_t readIndex = 1; readIndex < vec.size(); readIndex += 2) {
        vec[writeIndex++] = vec[readIndex];
    }
    vec.resize(writeIndex);
    vec.shrink_to_fit();
}

void tmpeDAQ::threadBody() {
    while (this->isRunning.load()) {
        auto [block, ts] = TRY_OR_THROW(this->daDevice.readPV<std::vector<int32_t>>("DMASamples"));
        compactVector(block);

        this->push(ts, block);
        this->pushedBlocks++;
        this->setNumberOfPushedDataBlocks(ts, pushedBlocks);
    }
}

}  // namespace nds_tmpe
