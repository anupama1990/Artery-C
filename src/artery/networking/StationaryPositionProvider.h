#ifndef ARTERY_STATIONARYPOSITIONPROVIDER_H_LHFQCP18
#define ARTERY_STATIONARYPOSITIONPROVIDER_H_LHFQCP18

#include "artery/networking/PositionFixObject.h"
#include "artery/utility/Geometry.h"
#include "traci/Listener.h"
#include <omnetpp/csimplemodule.h>
#include <vanetza/common/position_provider.hpp>

namespace artery
{

class StationaryPositionProvider : public omnetpp::cSimpleModule, public traci::Listener, public vanetza::PositionProvider
{
    public:
        // cSimpleModule
        void initialize(int stage) override;

        // PositionProvider
        const vanetza::PositionFix& position_fix() override { return mPositionFix; }

    protected:
        void traciInit() override;
        virtual Position getInitialPosition();

    private:
        void initializePosition(const Position&);

        PositionFixObject mPositionFix;
};

} // namespace artery

#endif /* ARTERY_STATIONARYPOSITIONPROVIDER_H_LHFQCP18 */

