#ifndef EXTENDED_PDU_HPP_TL2WFH9W
#define EXTENDED_PDU_HPP_TL2WFH9W

#include <vanetza/geonet/basic_header.hpp>
#include <vanetza/geonet/common_header.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_sink.hpp>
#include <vanetza/security/secured_message.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/optional/optional.hpp>
#include <memory>

namespace vanetza
{
namespace geonet
{

template<class HEADER> class ExtendedPduConstRefs;

template<class HEADER>
class ExtendedPdu : public Pdu
{
public:
    using SecuredMessage = security::SecuredMessage;
    using ExtendedHeader = HEADER;

    ExtendedPdu() = default;
    ExtendedPdu(const ExtendedPdu&) = default;
    ExtendedPdu& operator=(const ExtendedPdu&) = default;
    ExtendedPdu(const MIB& mib) : m_basic(mib), m_common(mib) {}
    ExtendedPdu(const DataRequest& request, const MIB& mib) :
        m_basic(request, mib), m_common(request, mib) {}
    ExtendedPdu(const BasicHeader& basic, const CommonHeader& common, const HEADER& extended) :
        m_basic(basic), m_common(common), m_extended(extended) {}
    ExtendedPdu(const BasicHeader& basic, const CommonHeader& common, const HEADER& extended,
            const SecuredMessage& secured) :
        m_basic(basic), m_common(common), m_extended(extended), m_secured(secured) {}
    ExtendedPdu(const ExtendedPduConstRefs<HEADER>& pdu) :
        m_basic(pdu.basic()), m_common(pdu.common()), m_extended(pdu.extended()),
        m_secured(pdu.secured() != nullptr, *pdu.secured()) {}

    BasicHeader& basic() override { return m_basic; }
    const BasicHeader& basic() const override { return m_basic; }
    CommonHeader& common() override { return m_common; }
    const CommonHeader& common() const override { return m_common; }
    HeaderConstRefVariant extended_variant() const override { return m_extended; }
    HEADER& extended() { return m_extended; }
    const HEADER& extended() const { return m_extended; }
    SecuredMessage* secured() override { return m_secured.get_ptr(); }
    const SecuredMessage* secured() const override { return m_secured.get_ptr(); }
    void secured(SecuredMessage* smsg) override
    {
        m_secured = boost::optional<SecuredMessage>(smsg, *smsg);
    }
    void secured(SecuredMessage&& smsg) override { m_secured = std::move(smsg); }

    std::unique_ptr<Pdu> clone() const override
    {
        return std::unique_ptr<ExtendedPdu> { new ExtendedPdu(*this) };
    }

private:
    BasicHeader m_basic;
    CommonHeader m_common;
    HEADER m_extended;
    boost::optional<SecuredMessage> m_secured;
};

template<class HEADER>
class ExtendedPduConstRefs : public ConstAccessiblePdu
{
public:
    using SecuredMessage = security::SecuredMessage;
    using ExtendedHeader = HEADER;

    ExtendedPduConstRefs(const BasicHeader& basic, const CommonHeader& common, const HEADER& extended) :
        mr_basic(basic), mr_common(common), mr_extended(extended), mp_secured(nullptr) {}
    ExtendedPduConstRefs(const BasicHeader& basic, const CommonHeader& common, const HEADER& extended,
            const SecuredMessage* secured) :
        mr_basic(basic), mr_common(common), mr_extended(extended), mp_secured(secured) {}

    const BasicHeader& basic() const override { return mr_basic; }
    const CommonHeader& common() const override { return mr_common; }
    HeaderConstRefVariant extended_variant() const override { return mr_extended; }
    const HEADER& extended() const { return mr_extended; }
    const SecuredMessage* secured() const override { return mp_secured; }

    std::unique_ptr<Pdu> clone() const override
    {
        return std::unique_ptr<ExtendedPdu<HEADER>> {
            mp_secured ?
                new ExtendedPdu<HEADER>(mr_basic, mr_common, mr_extended, *mp_secured) :
                new ExtendedPdu<HEADER>(mr_basic, mr_common, mr_extended)
        };
    }

private:
    const BasicHeader& mr_basic;
    const CommonHeader& mr_common;
    const HEADER& mr_extended;
    const SecuredMessage* mp_secured;
};

/**
 * \brief Serialize relevant header parts for signing
 * Uses common and extended headers.
 * \param pdu containing GN headers
 * \return binary form of relevant header parts
 */
template<class HEADER>
ByteBuffer convert_for_signing(const ExtendedPdu<HEADER>& pdu)
{
    ByteBuffer buf;
    byte_buffer_sink sink(buf);

    boost::iostreams::stream_buffer<byte_buffer_sink> stream(sink);
    OutputArchive ar(stream);

    serialize(pdu.common(), ar);
    serialize(pdu.extended(), ar);

    stream.close();
    return buf;
}

} // namespace geonet
} // namespace vanetza

#endif /* EXTENDED_PDU_HPP_TL2WFH9W */
