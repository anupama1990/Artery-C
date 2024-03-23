//
// Generated file, do not edit! Created by opp_msgtool 6.0 from inet/physicallayer/wireless/apsk/packetlevel/ApskPhyHeader.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "ApskPhyHeader_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace inet {

Register_Class(ApskPhyHeader)

ApskPhyHeader::ApskPhyHeader() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(6));

}

ApskPhyHeader::ApskPhyHeader(const ApskPhyHeader& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

ApskPhyHeader::~ApskPhyHeader()
{
}

ApskPhyHeader& ApskPhyHeader::operator=(const ApskPhyHeader& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void ApskPhyHeader::copy(const ApskPhyHeader& other)
{
    this->headerLengthField = other.headerLengthField;
    this->payloadLengthField = other.payloadLengthField;
    this->crc = other.crc;
    this->crcMode = other.crcMode;
    this->payloadProtocol = other.payloadProtocol;
}

void ApskPhyHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->headerLengthField);
    doParsimPacking(b,this->payloadLengthField);
    doParsimPacking(b,this->crc);
    doParsimPacking(b,this->crcMode);
    doParsimPacking(b,this->payloadProtocol);
}

void ApskPhyHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->headerLengthField);
    doParsimUnpacking(b,this->payloadLengthField);
    doParsimUnpacking(b,this->crc);
    doParsimUnpacking(b,this->crcMode);
    doParsimUnpacking(b,this->payloadProtocol);
}

b ApskPhyHeader::getHeaderLengthField() const
{
    return this->headerLengthField;
}

void ApskPhyHeader::setHeaderLengthField(b headerLengthField)
{
    handleChange();
    this->headerLengthField = headerLengthField;
}

b ApskPhyHeader::getPayloadLengthField() const
{
    return this->payloadLengthField;
}

void ApskPhyHeader::setPayloadLengthField(b payloadLengthField)
{
    handleChange();
    this->payloadLengthField = payloadLengthField;
}

uint16_t ApskPhyHeader::getCrc() const
{
    return this->crc;
}

void ApskPhyHeader::setCrc(uint16_t crc)
{
    handleChange();
    this->crc = crc;
}

CrcMode ApskPhyHeader::getCrcMode() const
{
    return this->crcMode;
}

void ApskPhyHeader::setCrcMode(CrcMode crcMode)
{
    handleChange();
    this->crcMode = crcMode;
}

const Protocol * ApskPhyHeader::getPayloadProtocol() const
{
    return this->payloadProtocol;
}

void ApskPhyHeader::setPayloadProtocol(const Protocol * payloadProtocol)
{
    handleChange();
    this->payloadProtocol = payloadProtocol;
}

class ApskPhyHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_headerLengthField,
        FIELD_payloadLengthField,
        FIELD_crc,
        FIELD_crcMode,
        FIELD_payloadProtocol,
    };
  public:
    ApskPhyHeaderDescriptor();
    virtual ~ApskPhyHeaderDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ApskPhyHeaderDescriptor)

ApskPhyHeaderDescriptor::ApskPhyHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::ApskPhyHeader)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

ApskPhyHeaderDescriptor::~ApskPhyHeaderDescriptor()
{
    delete[] propertyNames;
}

bool ApskPhyHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ApskPhyHeader *>(obj)!=nullptr;
}

const char **ApskPhyHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ApskPhyHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ApskPhyHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int ApskPhyHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_headerLengthField
        FD_ISEDITABLE,    // FIELD_payloadLengthField
        FD_ISEDITABLE,    // FIELD_crc
        0,    // FIELD_crcMode
        FD_ISCOMPOUND | FD_ISPOINTER,    // FIELD_payloadProtocol
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *ApskPhyHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "headerLengthField",
        "payloadLengthField",
        "crc",
        "crcMode",
        "payloadProtocol",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int ApskPhyHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "headerLengthField") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "payloadLengthField") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "crc") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "crcMode") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "payloadProtocol") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *ApskPhyHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "inet::b",    // FIELD_headerLengthField
        "inet::b",    // FIELD_payloadLengthField
        "uint16_t",    // FIELD_crc
        "inet::CrcMode",    // FIELD_crcMode
        "inet::Protocol",    // FIELD_payloadProtocol
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **ApskPhyHeaderDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_crcMode: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *ApskPhyHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_crcMode:
            if (!strcmp(propertyName, "enum")) return "inet::CrcMode";
            return nullptr;
        default: return nullptr;
    }
}

int ApskPhyHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ApskPhyHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ApskPhyHeader'", field);
    }
}

const char *ApskPhyHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        case FIELD_payloadProtocol: { const Protocol * value = pp->getPayloadProtocol(); return omnetpp::opp_typename(typeid(*const_cast<Protocol *>(value))); }
        default: return nullptr;
    }
}

std::string ApskPhyHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        case FIELD_headerLengthField: return unit2string(pp->getHeaderLengthField());
        case FIELD_payloadLengthField: return unit2string(pp->getPayloadLengthField());
        case FIELD_crc: return ulong2string(pp->getCrc());
        case FIELD_crcMode: return enum2string(pp->getCrcMode(), "inet::CrcMode");
        case FIELD_payloadProtocol: { auto obj = pp->getPayloadProtocol(); return obj == nullptr ? "" : obj->str(); }
        default: return "";
    }
}

void ApskPhyHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        case FIELD_headerLengthField: pp->setHeaderLengthField(b(string2long(value))); break;
        case FIELD_payloadLengthField: pp->setPayloadLengthField(b(string2long(value))); break;
        case FIELD_crc: pp->setCrc(string2ulong(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ApskPhyHeader'", field);
    }
}

omnetpp::cValue ApskPhyHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        case FIELD_headerLengthField: return cValue(pp->getHeaderLengthField().get(),"b");
        case FIELD_payloadLengthField: return cValue(pp->getPayloadLengthField().get(),"b");
        case FIELD_crc: return (omnetpp::intval_t)(pp->getCrc());
        case FIELD_crcMode: return static_cast<int>(pp->getCrcMode());
        case FIELD_payloadProtocol: return omnetpp::toAnyPtr(pp->getPayloadProtocol()); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ApskPhyHeader' as cValue -- field index out of range?", field);
    }
}

void ApskPhyHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        case FIELD_headerLengthField: pp->setHeaderLengthField(b(value.intValueInUnit("b"))); break;
        case FIELD_payloadLengthField: pp->setPayloadLengthField(b(value.intValueInUnit("b"))); break;
        case FIELD_crc: pp->setCrc(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ApskPhyHeader'", field);
    }
}

const char *ApskPhyHeaderDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_payloadProtocol: return omnetpp::opp_typename(typeid(Protocol));
        default: return nullptr;
    };
}

omnetpp::any_ptr ApskPhyHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        case FIELD_payloadProtocol: return omnetpp::toAnyPtr(pp->getPayloadProtocol()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ApskPhyHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ApskPhyHeader *pp = omnetpp::fromAnyPtr<ApskPhyHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ApskPhyHeader'", field);
    }
}

}  // namespace inet

namespace omnetpp {

}  // namespace omnetpp

