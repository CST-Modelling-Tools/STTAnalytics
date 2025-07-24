#ifndef PHOTON_H
#define PHOTON_H

struct Photon
{
    double id;
    double x, y, z;
    double side;
    double previousId;
    double nextId;
    double surfaceId;

    Photon() = default;

    Photon(double i, double xx, double yy, double zz, double s, double prev, double next, double surf)
        : id(i), x(xx), y(yy), z(zz), side(s), previousId(prev), nextId(next), surfaceId(surf) {}
};

#endif // PHOTON_H