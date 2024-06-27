/*

   Copyright (c) 2024
   Ryan Queiroz dos Santos.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

   THIS SOFTWARE IS PROVIDED BY Ryan Queiroz dos Santos “AS IS” AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
   EVENT SHALL [Name of Organisation] BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
   OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <sys/types.h>

#include <array>
#include <cstddef>
#include <ostream>
#include <type_traits>
#include <vector>

#include "./engine_utils.hh"

inline float DegreesToRadians(float degrees) { return degrees * 3.14f / 180; }

template <class T>
using Matrix2D = std::vector<std::vector<T>>;

template <typename ElementType, size_t NumRows, size_t NumCols>
using Array2D = std::array<std::array<ElementType, NumCols>, NumRows>;

template <typename T>
struct is_integer_or_float_pointer : std::false_type {};

template <typename T>
struct is_integer_or_float_pointer<T*>
    : std::integral_constant<bool, std::is_integral<T>::value || std::is_floating_point<T>::value> {};

template <class T>
struct Vec2 {
    static_assert(std::is_integral_v<T> | std::is_floating_point_v<T> | std::is_pointer_v<T> | is_integer_or_float_pointer<T>::value,
                  "ERROR: Could not create Gemotry: not integer or float");
    T x;
    T y;

    Vec2 operator+(const Vec2& other) const {
        return {x + other.x, y + other.y};
    }
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    bool operator==(const Vec2& other) {
        return (x == other.x, y == other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2<T>(x - other.x, y - other.y);
    }
    Vec2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }
    Vec2 operator/(float scalar) const {
        return {x / scalar, y / scalar};
    }
    Vec2 operator/(Vec2<T> other) const {
        return {x / other.x, y / other.y};
    }
    T cross(const Vec2<T>& other) const {
        return x * other.y - y * other.x;
    }
    T dot(const Vec2<T>& other) const {
        return x * other.x + y * other.y;
    }
    template <typename U>
    operator Vec2<U>() const {
        return {static_cast<U>(x), static_cast<U>(y)};
    }
    friend std::ostream& operator<<(std::ostream& os, const Vec2<T>& vec) {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }
};

template <class T>
struct Triangle {
    static_assert(std::is_integral_v<T> | std::is_floating_point_v<T> | std::is_pointer_v<T> | is_integer_or_float_pointer<T>::value,
                  "ERROR: Could not create Gemotry: not integer or float");
    Vec2<T> a;
    Vec2<T> b;
    Vec2<T> c;

    void Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, SDL_Color color);
};

template <class T>
inline bool IsPointInTriangle(const Vec2<T>& p, const Triangle<T>& triangle) {
    Vec2<T> v0 = triangle.c - triangle.a;
    Vec2<T> v1 = triangle.b - triangle.a;
    Vec2<T> v2 = p - triangle.a;

    T dot00 = v0.dot(v0);
    T dot01 = v0.dot(v1);
    T dot02 = v0.dot(v2);
    T dot11 = v1.dot(v1);
    T dot12 = v1.dot(v2);

    // Compute barycentric coordinates
    T invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    T u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    T v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

template <class T>
inline void Triangle<T>::Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, SDL_Color color) {
    SDL_Vertex vertices[3];

    vertices[0].position.x = a.x - cameraPos.x;
    vertices[0].position.y = a.y - cameraPos.y;
    vertices[0].color = color;

    vertices[1].position.x = b.x - cameraPos.x;
    vertices[1].position.y = b.y - cameraPos.y;
    vertices[1].color = color;

    vertices[2].position.x = c.x - cameraPos.x;
    vertices[2].position.y = c.y - cameraPos.y;
    vertices[2].color = color;

    // This should be the texture cordinates, because it's not really used I just set to 0
    vertices[0].tex_coord.x = 0;
    vertices[0].tex_coord.y = 0;
    vertices[1].tex_coord.x = 0;
    vertices[1].tex_coord.y = 0;
    vertices[2].tex_coord.x = 0;
    vertices[2].tex_coord.y = 0;

    int indices[3] = {0, 1, 2};

    SDL_RenderGeometry(renderer, nullptr, vertices, 3, indices, 3);
}

// CheckSideColision | returns true if the "side" given has collided
//
// @param SDL_Rect rect        | The rectangle that moves to the collidable surface
// @param SDL_Rect surface     | The collidable surface
// @param Direction side       | The side that will be checked
// @param Vec2<float> velocity | The velocity that the entity/model/rect is moving
// @param float timeDelta      | The time delta
// @param float timeMultiplier | The time multiplier
inline bool IsSideColliding(const SDL_Rect& rect, const SDL_Rect& surface, const Direction& side, const Vec2<float>& velocity, const float& timeDelta, const float& timeMultiplier) {
    int rTop = rect.y, rBottom = rect.y + rect.h,
        rLeft = rect.x, rRight = rect.x + rect.w;

    int sTop = surface.y, sBottom = surface.y + surface.h,
        sLeft = surface.x, sRight = surface.x + surface.w;

    bool hasCollided = false;

    bool isOverlapping = rRight > sLeft && rLeft < sRight;
    if (side == Direction::LEFT || side == Direction::RIGHT) {
        isOverlapping = rBottom > sTop && rTop < sBottom;
    }
    switch (side) {
        case Direction::UP: {
            hasCollided = rTop + timeDelta * velocity.y * timeMultiplier <= sBottom &&
                          rTop >= sBottom;
            break;
        }
        case Direction::DOWN: {
            hasCollided = rBottom + timeDelta * velocity.y * timeMultiplier >= sTop &&
                          rBottom <= sTop;
            break;
        }
        case Direction::LEFT: {
            hasCollided = rLeft + timeDelta * velocity.x * timeMultiplier <= sRight &&
                          rLeft >= sLeft;
            break;
        }
        case Direction::RIGHT: {
            hasCollided = rRight + timeDelta * velocity.x * timeMultiplier >= sLeft &&
                          rRight <= sRight;
            break;
        }
    }
    return (hasCollided && isOverlapping);
}

template <class T>
inline bool IsPointInRectangle(const Vec2<T>& point, const SDL_Rect& rect) {
    return point.x >= rect.x &&
           point.x <= rect.x + rect.w &&
           point.y >= rect.y &&
           point.y <= rect.y + rect.h;
}

/* Quad struct | Differently from the SDL_Rect it has no fixed angle or form
 *
 * This may change while rotating, but should be created like that
 * a = Top Left vertice
 * b = Top Right vertice
 * c = Bottom Right vertice
 * d = Bottom Left vertice
 *
 * AB-BC-CD-DA are the size sides of the rectangle
 */
template <class T>
struct Quad {
    Quad(std::array<Vec2<T>, 4> vertices_) : isRectangleOrSquare(false) {
        a = vertices_[0];
        b = vertices_[1];
        c = vertices_[2];
        d = vertices_[3];
        CalculateSize();
    }

    Quad(Vec2<T> topLeftPos, Vec2<T> dimentions) : isRectangleOrSquare(true) {
        a = Vec2<T>{topLeftPos};
        b = Vec2<T>{topLeftPos.x + dimentions.x, topLeftPos.y};
        c = Vec2<T>{topLeftPos + dimentions};
        d = Vec2<T>{topLeftPos.x, topLeftPos.y + dimentions.y};

        // easy calculations because it is a rectangle
        AB = dimentions.x, CD = dimentions.x;
        BC = dimentions.y, DA = dimentions.y;
    };

    float AB, BC, CD, DA;
    Vec2<T> a, b, c, d;
    const bool isRectangleOrSquare;

    friend std::ostream& operator<<(std::ostream& os, const Quad<T>& quad) {
        os << "a: " << quad.a << ", b: " << quad.b << " , c: " << quad.c << " , d: " << quad.d;
        return os;
    }

   private:
    float angleNow = 0;

   public:
    inline float GetAngle();

    void CalculateSize();

    void RotateVerticie(float angleInRadians, size_t verticieToRotate);
    void RotateCenter(float angleInRadians);

    void Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, SDL_Color color) const;

    void Move(Vec2<T> newPos);
    void SetPos(Vec2<T> newPos, size_t verticie);

   private:
    void RotateAroundPoint(float angleInRadians, const Vec2<T>& point);
    void RotateVertex(Vec2<T>& vertex, const Vec2<T>& point, T cosTheta, T sinTheta);
};

template <class T>
inline float Quad<T>::GetAngle() { return angleNow; }

template <class T>
inline void Quad<T>::CalculateSize() {
    AB = std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
    BC = std::sqrt(std::pow(c.x - b.x, 2) + std::pow(c.y - b.y, 2));
    CD = std::sqrt(std::pow(d.x - c.x, 2) + std::pow(d.y - c.y, 2));
    DA = std::sqrt(std::pow(a.x - d.x, 2) + std::pow(a.y - d.y, 2));
}

template <class T>
inline void Quad<T>::RotateVerticie(float angleInRadians, size_t verticieToRotate) {
    if (verticieToRotate > 3) return;

    Vec2<T>* pivot;
    switch (verticieToRotate) {
        case 0:
            pivot = &a;
            break;
        case 1:
            pivot = &b;
            break;
        case 2:
            pivot = &c;
            break;
        case 3:
            pivot = &d;
            break;
    }

    RotateAroundPoint(angleInRadians, *pivot);
    angleNow += angleInRadians;
    CalculateSize();
}

template <class T>
inline void Quad<T>::RotateCenter(float angleInRadians) {
    Vec2<T> center = Vec2<T>{
        (a.x + b.x + c.x + d.x) / 4,
        (a.y + b.y + c.y + d.y) / 4};

    RotateAroundPoint(angleInRadians, center);
    angleNow += angleInRadians;
    CalculateSize();
}
template <class T>
void Quad<T>::Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, SDL_Color color) const {
    SDL_Vertex vertices[4];
    vertices[0].position = {static_cast<float>(a.x - cameraPos.x), static_cast<float>(a.y - cameraPos.y)};
    vertices[1].position = {static_cast<float>(b.x - cameraPos.x), static_cast<float>(b.y - cameraPos.y)};
    vertices[2].position = {static_cast<float>(c.x - cameraPos.x), static_cast<float>(c.y - cameraPos.y)};
    vertices[3].position = {static_cast<float>(d.x - cameraPos.x), static_cast<float>(d.y - cameraPos.y)};

    for (int i = 0; i < 4; ++i) {
        vertices[i].color = color;
    }

    int indices[] = {0, 1, 2, 2, 3, 0};
    SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
}

template <class T>
inline void Quad<T>::RotateAroundPoint(float angleInRadians, const Vec2<T>& point) {
    T cosTheta = std::cos(angleInRadians);
    T sinTheta = std::sin(angleInRadians);

    RotateVertex(a, point, cosTheta, sinTheta);
    RotateVertex(b, point, cosTheta, sinTheta);
    RotateVertex(c, point, cosTheta, sinTheta);
    RotateVertex(d, point, cosTheta, sinTheta);
}

template <class T>
inline void Quad<T>::RotateVertex(Vec2<T>& vertex, const Vec2<T>& point, T cosTheta, T sinTheta) {
    Vec2<T> translated = vertex - point;

    T xNew = translated.x * cosTheta - translated.y * sinTheta;
    T yNew = translated.x * sinTheta + translated.y * cosTheta;

    vertex.x = xNew + point.x;
    vertex.y = yNew + point.y;
}

template <class T>
void Quad<T>::Move(Vec2<T> addPos) {
    a += addPos;
    b += addPos;
    c += addPos;
    d += addPos;
}

template <class T>
void Quad<T>::SetPos(Vec2<T> newPos, size_t verticie) {
    Vec2<T>* targetVertex;

    switch (verticie) {
        case 0:
            targetVertex = &a;
            break;
        case 1:
            targetVertex = &b;
            break;
        case 2:
            targetVertex = &c;
            break;
        case 3:
            targetVertex = &d;
            break;
        default:
            return;  // Invalid vertex index
    }

    Vec2<T> offset = newPos - *targetVertex;

    a += offset;
    b += offset;
    c += offset;
    d += offset;
}
