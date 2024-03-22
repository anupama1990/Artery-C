#ifndef ARTERY_CPOBJECT_H_
#define ARTERY_CPOBJECT_H_

#include <omnetpp/cobject.h>
#include <vanetza/asn1/cpm.hpp>
#include <memory>

namespace artery
{

class CpObject : public omnetpp::cObject
{
public:
    CpObject(const CpObject&) = default;
    CpObject& operator=(const CpObject&) = default;

    CpObject(vanetza::asn1::Cpm&&);
    CpObject& operator=(vanetza::asn1::Cpm&&);

    CpObject(const vanetza::asn1::Cpm&);
    CpObject& operator=(const vanetza::asn1::Cpm&);

    CpObject(const std::shared_ptr<const vanetza::asn1::Cpm>&);
    CpObject& operator=(const std::shared_ptr<const vanetza::asn1::Cpm>&);

    const vanetza::asn1::Cpm& asn1() const;

    std::shared_ptr<const vanetza::asn1::Cpm> shared_ptr() const;

private:
    std::shared_ptr<const vanetza::asn1::Cpm> m_cpm_wrapper;
};

} // namespace artery

#endif /* ARTERY_CPOBJECT_H_ */
