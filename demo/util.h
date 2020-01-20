//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_DEMO_UTIL_GUARD
#define GFX_DEMO_UTIL_GUARD

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gfx/Pipeline.h>

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
T pow2_align(T value, uint64_t alignment)
{
    return ((value + static_cast<T>(alignment) - 1) & ~(static_cast<T>(alignment) - 1));
}

//----------------------------------------------------------------------------------------------------------------------

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

//----------------------------------------------------------------------------------------------------------------------

class Primitive {
public:
    std::vector<Vertex> vertices;
    Gfx_lib::Vertex_input vertex_input;
    std::vector<uint16_t> indices;
    uint32_t draw_count;

    virtual ~Primitive() = default;
};

//----------------------------------------------------------------------------------------------------------------------

class Torus : public Primitive {
public:
    Torus(float inner_radius, float outer_radius, uint32_t side_count, uint32_t ring_count);

private:
    void init_vertices_(float inner_radius, float outer_radius, uint32_t side_count, uint32_t ring_count);

    void init_indices_(uint32_t side_count, uint32_t ring_count);
};

//----------------------------------------------------------------------------------------------------------------------

#endif // GFX_DEMO_UTIL_GUARD
