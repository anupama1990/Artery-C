


#include <omnetpp/cmessage.h>

#include "traci/sumo/utils/traci/TraCIAPI.h"
#include "traci/sumo/libsumo/TraCIDefs.h"
#include "PoiRetrievalModule_m.h"
using libsumo::TraCIPosition;

namespace artery{

class PoiRetrievalModule: public PoiRetrievalModule_Base
{
  protected:

    std::vector<TraCIPosition> enbPosOmnet;

  public:

    PoiRetrievalModule(const char *name = NULL, int kind = 0) :
        PoiRetrievalModule_Base(name, kind)
    {
    }

    ~PoiRetrievalModule()
    {
    }

    PoiRetrievalModule(const PoiRetrievalModule& other)
    {
        operator=(other);
    }

    PoiRetrievalModule& operator=(const PoiRetrievalModule& other)
    {
        enbPosOmnet = other.enbPosOmnet;
        PoiRetrievalModule_Base::operator=(other);
        return *this;
    }

    virtual PoiRetrievalModule *dup() const
    {
        return new PoiRetrievalModule(*this);
    }

    virtual void setEnbPositionOmnet(const std::vector<TraCIPosition> enbPosOmnet )
    {
        this->enbPosOmnet = enbPosOmnet;
    }

    virtual const std::vector<TraCIPosition> getEnbPositionOmnet()
    {
        return enbPosOmnet;
    }
};
}


