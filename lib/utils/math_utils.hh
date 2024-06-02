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

inline float DegreesToRadians(float degrees) { return degrees * 3.14f / 180; }

template <class T>
struct Vector2 {
    T x;
    T y;

    Vector2<T> operator+(const Vector2<T>& other) const {
        return {x + other.x, y + other.y};
    }

    Vector2<T> operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }
    Vector2<T> operator-(const Vector2<T>& other) const {
        return Vector2<T>(x - other.x, y - other.y);
    }
    T cross(const Vector2<T>& other) const {
        return x * other.y - y * other.x;
    }
    T dot(const Vector2<T>& other) const {
        return x * other.x + y * other.y;
    }
    template <typename U>
    operator Vector2<U>() const {
        return {static_cast<U>(x), static_cast<U>(y)};
    }
};

template <class T>
struct Triangle {
    Vector2<T> a;
    Vector2<T> b;
    Vector2<T> c;

    void Draw(SDL_Renderer* renderer, Vector2<float> cameraPos, SDL_Color color);
};

template <class T>
inline bool IsPointInTriangle(const Vector2<T>& p, const Triangle<T>& triangle) {
    Vector2<T> v0 = triangle.c - triangle.a;
    Vector2<T> v1 = triangle.b - triangle.a;
    Vector2<T> v2 = p - triangle.a;

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
inline void Triangle<T>::Draw(SDL_Renderer* renderer, Vector2<float> cameraPos, SDL_Color color) {
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
