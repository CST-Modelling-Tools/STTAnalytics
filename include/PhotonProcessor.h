#ifndef PHOTONPROCESSOR_H
#define PHOTONPROCESSOR_H

#include "Photon.h"
#include "EnergyAggregator.h"
#include <iostream>
#include <cstdint>

struct PhotonProcessor
{
    EnergyAggregator& m_aggregator;
    mutable uint64_t m_count = 0;
    uint64_t m_reportEvery = 1'000'000;

    explicit PhotonProcessor(EnergyAggregator& aggregator)
        : m_aggregator(aggregator) {}

    void operator()(const Photon& photon) const
    {
        m_aggregator.processPhoton(photon);
        ++m_count;
        if (m_count % m_reportEvery == 0)
        {
            std::cout << "Processed " << m_count << " photons...\n";
        }
    }

    uint64_t getTotalCount() const { return m_count; }
};

#endif // PHOTONPROCESSOR_H