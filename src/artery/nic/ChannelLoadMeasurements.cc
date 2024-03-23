#include "ChannelLoadMeasurements.h"
#include <omnetpp/csimulation.h>
#include <algorithm>

ChannelLoadMeasurements::ChannelLoadMeasurements() :
    m_samples(12500), m_busy(false)
{
}

void ChannelLoadMeasurements::reset()
{
    m_samples.clear();
    m_last_update = omnetpp::simTime();
    m_busy = false;
}

void ChannelLoadMeasurements::busy()
{
    fill(true);
}

void ChannelLoadMeasurements::idle()
{
    fill(false);
}

void ChannelLoadMeasurements::fill(bool busy)
{
    const omnetpp::SimTime now = omnetpp::simTime();
    const omnetpp::SimTime duration = now - m_last_update;

    const std::size_t samples = duration / omnetpp::SimTime { 8, omnetpp::SIMTIME_US };
    for (std::size_t i = 0; i < samples; ++i) {
        m_samples.push_back(m_busy);
    }

    m_busy = busy;
    m_last_update = now;
}

vanetza::dcc::ChannelLoad ChannelLoadMeasurements::channel_load()
{
    fill(m_busy);
    const double busy = std::count(m_samples.begin(), m_samples.end(), true);
    const double total = m_samples.capacity();
    return vanetza::dcc::ChannelLoad { busy / total };
}
