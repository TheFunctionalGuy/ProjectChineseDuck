#ifndef ACE_VECTOR_H_
#define ACE_VECTOR_H_

#include <iostream>
#include <math.h>
#include <array>

inline bool Cmpf(float A, float B, float epsilon = 0.005f)
{
    return (fabs(A - B) < epsilon);
}

namespace vec {
    inline std::string ToString(const std::array<float, 3> v) {
        char buffer[100];
        sprintf_s(buffer, "{ %8.2f, %8.2f, %8.2f }", v[0], v[1], v[2]);

        return std::string(buffer);
    }

    inline void Inverse(std::array<float, 3>& v) {
        v[0] = -v[0];
        v[1] = -v[1];
        v[2] = -v[2];
    }

    inline bool Cmp(const std::array<float, 3> v0, const std::array<float, 3> v1) {
        return (Cmpf(v0[0], v1[0]) &&
                Cmpf(v0[1], v1[1]) &&
                Cmpf(v0[2], v1[2]));
    }

    inline void Copy(const std::array<float, 3> v, std::array<float, 3>& out) {
        out[0] = v[0];
        out[1] = v[1];
        out[2] = v[2];
    }

    inline void Translate(const std::array<float, 3> v0, const std::array<float, 3> v1, std::array<float, 3>& out) {
        out[0] = v0[0] + v1[0];
        out[1] = v0[1] + v1[1];
        out[2] = v0[2] + v1[2];
    }

    inline void Subtract(const std::array<float, 3> v0, const std::array<float, 3> v1, std::array<float, 3>& out) {
        out[0] = v0[0] - v1[0];
        out[1] = v0[1] - v1[1];
        out[2] = v0[2] - v1[2];
    }

    inline void Scale(const std::array<float, 3> v0, const std::array<float, 3> v1, std::array<float, 3>& out) {
        out[0] = v0[0] * v1[0];
        out[1] = v0[1] * v1[1];
        out[2] = v0[2] * v1[2];
    }

    inline float Dist(const std::array<float, 3> p1, const std::array<float, 3> p2) {
        float x = p1[0] - p2[0];
        float y = p1[1] - p2[1];
        float z = p1[2] - p2[2];

        return sqrt(x * x + y * y + z * z);
    }

    inline bool DistToFixPoint(const std::array<float, 3> f, const std::array<float, 3> p1, const std::array<float, 3> p2) {
        if (Dist(f, p1) - Dist(f, p2) > 0) {
            return true;
        } else {
            return false;
        }
    }

    inline float Magnitude(const std::array<float, 3> v) {
        return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }
}

#endif // ACE_VECTOR_H_
