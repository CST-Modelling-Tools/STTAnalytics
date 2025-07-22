#ifndef PHOTON_H
#define PHOTON_H

#include <cstdint>

struct Photon
{
    uint64_t id;
    float x, y, z;
    uint32_t side;
    uint64_t previousId;
    uint64_t nextId;
    uint64_t surfaceId;

    Photon() = default;

    Photon(uint64_t i, float xx, float yy, float zz, uint32_t s, uint64_t prev, uint64_t next, uint64_t surf)
        : id(i), x(xx), y(yy), z(zz), side(s), previousId(prev), nextId(next), surfaceId(surf) {}
};

#endif // PHOTON_H