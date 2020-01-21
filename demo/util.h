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
T align_pow2(T value, uint64_t alignment)
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

class Plane : public Primitive {
public:
    Plane(float w, float h);
    
private:
    void init_vertices_(float half_x, float half_y);
    
    void init_indices_();
};

//----------------------------------------------------------------------------------------------------------------------

class Cube : public Primitive {
public:
    explicit Cube(float size = 1.0f);
    
private:
    void init_vertices_(float half_size);
    
    void init_indices_();
};

//----------------------------------------------------------------------------------------------------------------------

class Sphere : public Primitive {
public:
    Sphere(float r, uint32_t sector, uint32_t stack);
    
private:
    void init_vertices_(float r, uint32_t sector, uint32_t stack);
    
    void init_indices_(uint32_t sector, uint32_t stack);
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
