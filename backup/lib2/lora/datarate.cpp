#include "datarate.h"

const dataRate dataRates::theDataRates[dataRates::nmbrUsedDataRates]{
    {spreadingFactor::SF12, 51},
    {spreadingFactor::SF11, 51},
    {spreadingFactor::SF10, 51},
    {spreadingFactor::SF9, 115},
    {spreadingFactor::SF8, 242},
    {spreadingFactor::SF7, 242},
};

uint32_t dataRates::getDownlinkDataRateIndex(uint32_t uplinkDataRateIndex, uint32_t Rx1DataRateOffset) {
    if (uplinkDataRateIndex > Rx1DataRateOffset) {
        return uplinkDataRateIndex - Rx1DataRateOffset;
    }
    return 0;
}
