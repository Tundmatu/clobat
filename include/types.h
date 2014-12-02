#pragma once

#include <glm/glm.hpp>
#include <SDL_types.h>

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;

typedef glm::mat3 Mat3;
typedef glm::mat4 Mat4;

static inline float kB(size_t size) { return size / 1e3f; }
static inline float MB(size_t size) { return size / 1e6f; }
static inline float GB(size_t size) { return size / 1e9f; }
static inline float TB(size_t size) { return size / 1e12f; }
