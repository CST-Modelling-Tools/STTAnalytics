#ifndef PHOTONPROCESSOR_H
#define PHOTONPROCESSOR_H

#include "Photon.h"
#include "EnergyAggregator.h"

struct PhotonProcessor
{
    EnergyAggregator& m_aggregator;

    explicit PhotonProcessor(EnergyAggregator& aggregator)
        : m_aggregator(aggregator) {}

    void operator()(const Photon& photon) const
    {
        m_aggregator.processPhoton(photon);
    }
};

#endif // PHOTONPROCESSOR_H