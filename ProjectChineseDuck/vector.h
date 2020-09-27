#ifndef ACE_VECTOR_H_
#define ACE_VECTOR_H_

#include <math.h>

#include <array>

inline bool Cmpf(const float a, const float b, const float epsilon = 0.005f) {
    return (fabs(a - b) < epsilon);
}

namespace vec {
    inline void Inverse(std::array<float, 3>& v) {
        v[0] = -v[0];
        v[1] = -v[1];
        v[2] = -v[2];
    }

    inline bool Cmp(const std::array<float, 3>& v1, const std::array<float, 3>& v2) {
        return (Cmpf(v1[0], v2[0]) &&
                Cmpf(v1[1], v2[1]) &&
                Cmpf(v1[2], v2[2]));
    }

    inline void Copy(const std::array<float, 3>& src, std::array<float, 3>& dst) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
    }

    inline void Translate(const std::array<float, 3>& v1, const std::array<float, 3>& v2, std::array<float, 3>& dst) {
        dst[0] = v1[0] + v2[0];
        dst[1] = v1[1] + v2[1];
        dst[2] = v1[2] + v2[2];
    }

    inline void Subtract(const std::array<float, 3>& v1, const std::array<float, 3>& v2, std::array<float, 3>& dst) {
        dst[0] = v1[0] - v2[0];
        dst[1] = v1[1] - v2[1];
        dst[2] = v1[2] - v2[2];
    }

    inline void Scale(const std::array<float, 3>& v1, const std::array<float, 3>& v2, std::array<float, 3>& dst) {
        dst[0] = v1[0] * v2[0];
        dst[1] = v1[1] * v2[1];
        dst[2] = v1[2] * v2[2];
    }

    inline float Dist(const std::array<float, 3>& p1, const std::array<float, 3>& p2) {
        float x = p1[0] - p2[0];
        float y = p1[1] - p2[1];
        float z = p1[2] - p2[2];

        return sqrt(x * x + y * y + z * z);
    }

    inline float Magnitude(const std::array<float, 3>& v) {
        return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }
}

#endif // ACE_VECTOR_H_
