#ifndef NDS_TMPE_H
#define NDS_TMPE_H

#include <nds3/definitions.h>
#include "nds-tmpeDAQ.hpp"
#include "nds-tmpeFirmware.hpp"

namespace nds_tmpe {

const auto TRY_OR_THROW = [](auto&& result) {
    if (result.has_error()) {
        throw nds::NdsError(result.assume_error().what());
    } else {
        return result.assume_value();
    }
};

class tmpeDevice : ndsda::ndsdaDevice {
private:
    ndstmpeFirmware firmwareNode;
    tmpeDAQ daqNode;

public:
    tmpeDevice(nds::Factory& factory, const std::string& deviceName, const nds::namedParameters_t& params);
};

}  // namespace nds_tmpe

int nds_tmpe_test(int x);

#endif  //! NDS_TMPE_H
