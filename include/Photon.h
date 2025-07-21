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
};

#endif // PHOTON_H
