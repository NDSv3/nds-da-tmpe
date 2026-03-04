#ifndef NDS_TMPE_H
#define NDS_TMPE_H

#include "ndsdaDevice.hpp"

namespace nds_tmpe {
class tmpeDevice : ndsda::ndsdaDevice {
public:
    tmpeDevice(nds::Factory& factory, const std::string& deviceName, const nds::namedParameters_t& params);
};
}  // namespace nds_tmpe

int nds_tmpe_test(int x);

#endif  //! NDS_TMPE_H
