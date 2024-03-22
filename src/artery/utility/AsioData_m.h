//
// Generated file, do not edit! Created by opp_msgtool 6.0 from /home/hegde/workspace/main/Artery-C/src/artery/utility/AsioData.msg.
//

#ifndef __ARTERY_ASIODATA_M_H
#define __ARTERY_ASIODATA_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif


namespace artery {

class AsioData;

}  // namespace artery


namespace artery {

// cplusplus {{
#include <array>
#include <cstdint>
typedef std::array<uint8_t, 1024> AsioBuffer;
typedef std::size_t AsioLength;
// }}

/**
 * Class generated from <tt>/home/hegde/workspace/main/Artery-C/src/artery/utility/AsioData.msg:30</tt> by opp_msgtool.
 * <pre>
 * message AsioData
 * {
 *     //  AsioBuffer Buffer;
 *     int Length;
 * }
 * </pre>
 */
class AsioData : public ::omnetpp::cMessage
{
  protected:
    int Length = 0;

  private:
    void copy(const AsioData& other);

  protected:
    bool operator==(const AsioData&) = delete;

  public:
    AsioData(const char *name=nullptr, short kind=0);
    AsioData(const AsioData& other);
    virtual ~AsioData();
    AsioData& operator=(const AsioData& other);
    virtual AsioData *dup() const override {return new AsioData(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getLength() const;
    virtual void setLength(int Length);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const AsioData& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, AsioData& obj) {obj.parsimUnpack(b);}


}  // namespace artery


namespace omnetpp {

template<> inline artery::AsioData *fromAnyPtr(any_ptr ptr) { return check_and_cast<artery::AsioData*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __ARTERY_ASIODATA_M_H

