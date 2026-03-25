#ifndef NDS_TMPE_DAQ_H
#define NDS_TMPE_DAQ_H

#include <atomic>
#include <cstdint>
#include <nds3/dataAcquisition.h>
#include <nds3/pvVariableIn.h>
#include <ndsdaDevice.hpp>

namespace nds_tmpe {

class tmpeDAQ : public nds::DataAcquisition<std::vector<float>> {
public:
    enum class ADCGainRange {
        PLUS_5_12       = 0,
        PLUS_10         = 1,
        PLUS_10_24      = 2,
        PLUSMINUS_5     = 3,
        PLUSMINUS_5_12  = 4,
        PLUSMINUS_10    = 5,
        PLUSMINUS_10_24 = 6
    };

    tmpeDAQ(const std::string& name, ndsda::ndsdaDevice& daDevice);
    ~tmpeDAQ() override = default;

private:
    ndsda::ndsdaDevice& daDevice;
    std::thread acquisitionThread;

    std::atomic_uint32_t pushedBlocks;
    std::atomic_bool isRunning;
    std::atomic<ADCGainRange> currentRange;

    nds::PVDelegateOut<int32_t> rangePV;
    nds::PVVariableIn<int32_t> rangePV_RBV;

    void switchOn(const timespec& time);
    void switchOff(const timespec& time);
    void start(const timespec& time);
    void stop(const timespec& time);
    void recover(const timespec& time);
    bool allowStateChange(nds::state_t c, nds::state_t g, nds::state_t d);

    // PV Writer functions
    void PV_Gain_Writer(const timespec& time, int32_t value);
    void PV_Offset_Writer(const timespec& time, int32_t value);
    void PV_Bandwidth_Writer(const timespec& time, int32_t value);
    void PV_Resolution_Writer(const timespec& time, int32_t value);
    void PV_Impedance_Writer(const timespec& time, int32_t value);
    void PV_Coupling_Writer(const timespec& time, int32_t value);
    void PV_SignalRefType_Writer(const timespec& time, int32_t value);
    void PV_Ground_Writer(const timespec& time, int32_t value);
    void PV_DMAEnable_Writer(const timespec& time, int32_t value);
    void PV_SamplingRate_Writer(const timespec& time, int32_t value);

    // PV Functions
    void PV_Range_Writer(const timespec& time, int32_t value);

    void threadBody();
};

}  // namespace nds_tmpe

#endif  //! NDS_TMPE_DAQ_H
