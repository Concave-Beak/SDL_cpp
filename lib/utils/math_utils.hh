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
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_shape.h>
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
    static_assert(std::is_integral<T>::value | std::is_floating_point<T>::value | std::is_pointer<T>::value | is_integer_or_float_pointer<T>::value,
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
    bool operator==(const Vec2& other) const {
        return (x == other.x, y == other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2<T>(x - other.x, y - other.y);
    }
    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Vec2 operator*(float scalar) const {
        return {static_cast<T>(x * scalar), static_cast<T>(y * scalar)};
    }
    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
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
    static_assert(std::is_integral<T>::value | std::is_floating_point<T>::value | std::is_pointer<T>::value | is_integer_or_float_pointer<T>::value,
                  "ERROR: Could not create Gemotry: not integer or float");
    Vec2<T> a;
    Vec2<T> b;
    Vec2<T> c;

    void Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, SDL_Color color) const;
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
inline bool IsRectCollidinWithTriangle(const SDL_Rect& rect, const Triangle<T>& triangle) {
    Vec2<T> point1, point2, point3, point4;
    point1 = Vec2<T>{rect.x, rect.y};
    point2 = Vec2<T>{rect.x + rect.w, rect.y};
    point3 = Vec2<T>{rect.x, rect.y + rect.h};
    point4 = Vec2<T>{rect.x + rect.w, rect.y + rect.h};
    return IsPointInTriangle(point1, triangle) || IsPointInTriangle(point2, triangle) ||
           IsPointInTriangle(point3, triangle) || IsPointInTriangle(point4, triangle);
}

template <class T>
inline void Triangle<T>::Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, SDL_Color color) const {
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

inline bool IsRectangleCollidingWithRect(const SDL_Rect rect1, const SDL_Rect& rect2) {
    // Check if one rectangle is to the left of the other
    if (rect1.x + rect1.w <= rect2.x || rect2.x + rect2.w <= rect1.x) {
        return false;
    }

    // Check if one rectangle is above the other
    if (rect1.y + rect1.h <= rect2.y || rect2.y + rect2.h <= rect1.y) {
        return false;
    }

    // If neither of the above, the rectangles are colliding
    return true;
}
