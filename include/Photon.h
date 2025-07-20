#ifndef PHOTON_H
#define PHOTON_H

#include <cstdint>

struct Photon
{
    uint64_t m_id;
    double m_x, m_y, m_z;
    int m_side;
    uint64_t m_previousId;
    uint64_t m_nextId;
    uint64_t m_surfaceId;

    Photon() = default;

    Photon(uint64_t id, double x, double y, double z, int side,
           uint64_t previousId, uint64_t nextId, uint64_t surfaceId)
        : m_id(id), m_x(x), m_y(y), m_z(z), m_side(side),
          m_previousId(previousId), m_nextId(nextId), m_surfaceId(surfaceId)
    {}
};

#endif // PHOTON_H