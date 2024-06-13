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
#include <vector>

#include "./engine_utils.hh"

inline float DegreesToRadians(float degrees) { return degrees * 3.14f / 180; }

template <class Type>
using Matrix2D = std::vector<std::vector<Type>>;

template <typename ElementType, size_t NumRows, size_t NumCols>
using Array2D = std::array<std::array<ElementType, NumCols>, NumRows>;

template <class Type>
struct Vec2 {
    Type x;
    Type y;

    Vec2<Type> operator+(const Vec2<Type>& other) const {
        return {x + other.x, y + other.y};
    }

    Vec2<Type> operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }
    Vec2<Type> operator-(const Vec2<Type>& other) const {
        return Vec2<Type>(x - other.x, y - other.y);
    }
    Type cross(const Vec2<Type>& other) const {
        return x * other.y - y * other.x;
    }
    Type dot(const Vec2<Type>& other) const {
        return x * other.x + y * other.y;
    }
    template <typename U>
    operator Vec2<U>() const {
        return {static_cast<U>(x), static_cast<U>(y)};
    }
};

template <class Type>
struct Triangle {
    Vec2<Type> a;
    Vec2<Type> b;
    Vec2<Type> c;

    void Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, SDL_Color color);
};

template <class Type>
inline bool IsPointInTriangle(const Vec2<Type>& p, const Triangle<Type>& triangle) {
    Vec2<Type> v0 = triangle.c - triangle.a;
    Vec2<Type> v1 = triangle.b - triangle.a;
    Vec2<Type> v2 = p - triangle.a;

    Type dot00 = v0.dot(v0);
    Type dot01 = v0.dot(v1);
    Type dot02 = v0.dot(v2);
    Type dot11 = v1.dot(v1);
    Type dot12 = v1.dot(v2);

    // Compute barycentric coordinates
    Type invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    Type u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    Type v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

template <class Type>
inline void Triangle<Type>::Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, SDL_Color color) {
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
inline bool CheckSideCollision(const SDL_Rect& rect, const SDL_Rect& surface, const Direction& side, const Vec2<float>& velocity, const float& timeDelta, const float& timeMultiplier) {
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

template <class Type>
inline bool IsPointInRectangle(const Vec2<Type>& point, const SDL_Rect& rect) {
    return point.x >= rect.x &&
           point.x <= rect.x + rect.w &&
           point.y >= rect.y &&
           point.y <= rect.y + rect.h;
}
