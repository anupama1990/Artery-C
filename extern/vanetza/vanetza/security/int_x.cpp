#include <vanetza/security/int_x.hpp>
#include <vanetza/security/length_coding.hpp>

namespace vanetza
{
namespace security
{

void IntX::set(integer_type x)
{
    m_value = x;
}

ByteBuffer IntX::encode() const
{
    return encode_length(m_value);
}

boost::optional<IntX> IntX::decode(const ByteBuffer& buffer)
{
    std::tuple<ByteBuffer::const_iterator, std::uintmax_t> decoded = decode_length(buffer);
    if (std::get<0>(decoded) != buffer.begin()) {
        IntX result;
        result.set(std::get<1>(decoded));
        return result;
    }
    return boost::none;
}

size_t get_size(IntX intx)
{
    return length_coding_size(intx.get());
}

void serialize(OutputArchive& ar, const IntX& intx)
{
    serialize_length(ar, intx.get());
}

size_t deserialize(InputArchive& ar, IntX& intx)
{
    const auto size = deserialize_length(ar);
    intx.set(size);
    return get_size(intx);
}

} // namespace security
} // namespace vanetza
