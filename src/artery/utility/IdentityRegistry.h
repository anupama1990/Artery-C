/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef IDENTITYREGISTRY_H_WQ2TLDAU
#define IDENTITYREGISTRY_H_WQ2TLDAU

#include "artery/utility/Identity.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/optional/optional.hpp>

namespace artery
{

class IdentityRegistry : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    static const omnetpp::simsignal_t updateSignal;
    static const omnetpp::simsignal_t removeSignal;

    void initialize() override;
    using omnetpp::cIListener::finish;  // [-Woverloaded-virtual]
    void finish() override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

    template<typename TAG, typename VALUE>
    boost::optional<Identity> lookup(const VALUE& value)
    {
        boost::optional<Identity> result;
        auto& index = mIdentities.get<TAG>();
        auto found = index.find(value);
        if (found != index.end()) {
            result = *found;
        }
        return result;
    }

    struct traci {};
    struct application {};
    struct mac {};

private:
    boost::multi_index_container<Identity,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<traci>,
                boost::multi_index::member<Identity, std::string, &Identity::traci>>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<application>,
                boost::multi_index::member<Identity, uint32_t, &Identity::application>>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<mac>,
                boost::multi_index::const_mem_fun<Identity, vanetza::MacAddress, &Identity::mid>>
        >> mIdentities;
};

} // namespace artery

#endif /* IDENTITYREGISTRY_H_WQ2TLDAU */
