#include <artery/application/CpObject.h>
#include <omnetpp.h>
#include <cassert>

namespace artery
{

using namespace vanetza::asn1;

Register_Abstract_Class(CpObject)

CpObject::CpObject(Cpm&& cpm) :
    m_cpm_wrapper(std::make_shared<Cpm>(std::move(cpm)))
{
}

CpObject& CpObject::operator=(Cpm&& cpm)
{
    m_cpm_wrapper = std::make_shared<Cpm>(std::move(cpm));
    return *this;
}

CpObject::CpObject(const Cpm& cpm) :
    m_cpm_wrapper(std::make_shared<Cpm>(cpm))
{
}

CpObject& CpObject::operator=(const Cpm& cpm)
{
    m_cpm_wrapper = std::make_shared<Cpm>(cpm);
    return *this;
}

CpObject::CpObject(const std::shared_ptr<const Cpm>& ptr) :
    m_cpm_wrapper(ptr)
{
    assert(m_cpm_wrapper);
}

CpObject& CpObject::operator=(const std::shared_ptr<const Cpm>& ptr)
{
    m_cpm_wrapper = ptr;
    assert(m_cpm_wrapper);
    return *this;
}

std::shared_ptr<const Cpm> CpObject::shared_ptr() const
{
    assert(m_cpm_wrapper);
    return m_cpm_wrapper;
}

const vanetza::asn1::Cpm& CpObject::asn1() const
{
    return *m_cpm_wrapper;
}


using namespace omnetpp;

class CpmStationIdResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto cpm = dynamic_cast<CpObject*>(object)) {
            const auto id = cpm->asn1()->header.stationID;
            fire(this, t, id, details);
        }
    }
};

Register_ResultFilter("cpmStationId", CpmStationIdResultFilter)


class CpmGenerationDeltaTimeResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto cpm = dynamic_cast<CpObject*>(object)) {
            const auto genDeltaTime = cpm->asn1()->cpm.generationDeltaTime;
            fire(this, t, genDeltaTime, details);
        }
    }
};

Register_ResultFilter("cpmGenerationDeltaTime", CpmGenerationDeltaTimeResultFilter)

} // namespace artery
