//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "util.h"

using namespace glm;
using namespace Gfx_lib;

//----------------------------------------------------------------------------------------------------------------------

Plane::Plane(float w, float h) :
    Primitive {}
{
    init_vertices_(w / 2.0f, h / 2.0f);
    init_indices_();
}

//----------------------------------------------------------------------------------------------------------------------

void Plane::init_vertices_(float half_x, float half_y)
{
    vertices = {
        {{-half_x, -half_y, 0.0f}, {0.0f, 0.0f,  1.0f}, {0.0f, 1.0f}},
        {{ half_x, -half_y, 0.0f}, {0.0f, 0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-half_x,  half_y, 0.0f}, {0.0f, 0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ half_x,  half_y, 0.0f}, {0.0f, 0.0f,  1.0f}, {1.0f, 0.0f}},
        {{-half_x, -half_y, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{ half_x, -half_y, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{-half_x,  half_y, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ half_x,  half_y, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}
    };
    
    vertex_input.bindings[0].stride = sizeof(Vertex);
    vertex_input.attributes[0].binding = 0;
    vertex_input.attributes[0].format = Format::rgb32_float;
    vertex_input.attributes[0].offset = offsetof(Vertex, position);
    vertex_input.attributes[1].binding = 0;
    vertex_input.attributes[1].format = Format::rgb32_float;
    vertex_input.attributes[1].offset = offsetof(Vertex, normal);
    vertex_input.attributes[2].binding = 0;
    vertex_input.attributes[2].format = Format::rg32_float;
    vertex_input.attributes[2].offset = offsetof(Vertex, tex_coords);
}

//----------------------------------------------------------------------------------------------------------------------
    
void Plane::init_indices_()
{
    indices = {0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7};
    draw_count = indices.size();
}

//----------------------------------------------------------------------------------------------------------------------

Cube::Cube(float size) :
    Primitive {}
{
    init_vertices_(size / 2.0f);
    init_indices_();
}

//----------------------------------------------------------------------------------------------------------------------

void Cube::init_vertices_(float half_size)
{
    vertices = {
        {{-half_size, -half_size,  half_size}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ half_size, -half_size,  half_size}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ half_size,  half_size,  half_size}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-half_size,  half_size,  half_size}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
        {{ half_size, -half_size,  half_size}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ half_size, -half_size, -half_size}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ half_size,  half_size, -half_size}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{ half_size,  half_size,  half_size}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{-half_size, -half_size, -half_size}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{-half_size,  half_size, -half_size}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ half_size,  half_size, -half_size}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ half_size, -half_size, -half_size}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-half_size, -half_size,  half_size}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-half_size,  half_size,  half_size}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-half_size,  half_size, -half_size}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-half_size, -half_size, -half_size}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{-half_size, -half_size,  half_size}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{-half_size, -half_size, -half_size}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ half_size, -half_size, -half_size}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ half_size, -half_size,  half_size}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{-half_size,  half_size,  half_size}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ half_size,  half_size,  half_size}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ half_size,  half_size, -half_size}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-half_size,  half_size, -half_size}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
    };
    
    vertex_input.bindings[0].stride = sizeof(Vertex);
    vertex_input.attributes[0].binding = 0;
    vertex_input.attributes[0].format = Format::rgb32_float;
    vertex_input.attributes[0].offset = offsetof(Vertex, position);
    vertex_input.attributes[1].binding = 0;
    vertex_input.attributes[1].format = Format::rgb32_float;
    vertex_input.attributes[1].offset = offsetof(Vertex, normal);
    vertex_input.attributes[2].binding = 0;
    vertex_input.attributes[2].format = Format::rg32_float;
    vertex_input.attributes[2].offset = offsetof(Vertex, tex_coords);
}

//----------------------------------------------------------------------------------------------------------------------

void Cube::init_indices_()
{
    indices = {
         0,  1,  2,  0,  2,  3,
         4,  5,  6,  4,  6,  7,
         8,  9, 10,  8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };
    
    draw_count = indices.size();
}

//----------------------------------------------------------------------------------------------------------------------

Sphere::Sphere(float r, uint32_t sector, uint32_t stack) :
    Primitive {}
{
    init_vertices_(r, sector, stack);
    init_indices_(sector, stack);
}

//----------------------------------------------------------------------------------------------------------------------

void Sphere::init_vertices_(float r, uint32_t sector, uint32_t stack)
{
    vertices.reserve((sector + 1) * (stack + 1));

    auto sector_step = two_pi<float>() / sector;
    auto stack_step = pi<float>() / stack;

    for (auto i = 0; i <= stack; ++i) {
        auto stack_angle = pi<float>() / 2 - i * stack_step;
        auto xy = r * cosf(stack_angle);

        for (auto j = 0; j <= sector; ++j) {
            auto sector_angle = j * sector_step;
            
            Vertex vertex;
            
            vertex.position = {xy * cosf(sector_angle), xy * sinf(sector_angle), r * sinf(stack_angle)};
            vertex.normal = vertex.position / r;
            vertex.tex_coords = {static_cast<float>(j) / sector, static_cast<float>(i) / stack};
            
            vertices.push_back(vertex);
        }
    }
    
    vertex_input.bindings[0].stride = sizeof(Vertex);
    vertex_input.attributes[0].binding = 0;
    vertex_input.attributes[0].format = Format::rgb32_float;
    vertex_input.attributes[0].offset = offsetof(Vertex, position);
    vertex_input.attributes[1].binding = 0;
    vertex_input.attributes[1].format = Format::rgb32_float;
    vertex_input.attributes[1].offset = offsetof(Vertex, normal);
    vertex_input.attributes[2].binding = 0;
    vertex_input.attributes[2].format = Format::rg32_float;
    vertex_input.attributes[2].offset = offsetof(Vertex, tex_coords);
}

//----------------------------------------------------------------------------------------------------------------------

void Sphere::init_indices_(uint32_t sector, uint32_t stack)
{
    indices.reserve((sector * 2 * (stack - 1)) * 3);
    
    for (auto i = 0; i != stack; ++i) {
        auto cur_stack_start = i * (sector + 1);
        auto nxt_stack_start = cur_stack_start + sector + 1;

        for (auto j = 0; j != sector; ++j, ++cur_stack_start, ++nxt_stack_start) {
            if(i != 0) {
                indices.push_back(cur_stack_start);
                indices.push_back(nxt_stack_start);
                indices.push_back(cur_stack_start + 1);
            }

            if (i != stack - 1) {
                indices.push_back(cur_stack_start + 1);
                indices.push_back(nxt_stack_start);
                indices.push_back(nxt_stack_start + 1);
            }
        }
    }
    
    draw_count = indices.size();
}

//----------------------------------------------------------------------------------------------------------------------

Torus::Torus(float inner_radius, float outer_radius, uint32_t side_count, uint32_t ring_count) :
    Primitive {}
{
    init_vertices_(inner_radius, outer_radius, side_count, ring_count);
    init_indices_(side_count, ring_count);
}

//----------------------------------------------------------------------------------------------------------------------

void Torus::init_vertices_(float inner_radius, float outer_radius, uint32_t side_count, uint32_t ring_count)
{
    vertices.reserve(side_count * (ring_count + 1));

    auto ring_factor = two_pi<float>() / ring_count;
    auto side_factor = two_pi<float>() / side_count;

    for (auto ring = 0; ring <= ring_count; ++ring) {
        auto u = ring * ring_factor;
        auto cos_u = cos(u);
        auto sin_u = sin(u);

        for (auto side = 0; side != side_count; ++side) {
            auto v = side * side_factor;
            auto cos_v = cos(v);
            auto sin_v = sin(v);
            auto r = outer_radius + inner_radius * cos_v;

            Vertex vertex {
                { r * cos_u, r * sin_u, inner_radius * sin_v },
                { cos_v * cos_u * r, cos_v * sin_u * r, sin_v * r},
                { u / two_pi<float>(), v / two_pi<float>() }
            };

            vertex.normal = normalize(vertex.normal);
            vertices.push_back(vertex);
        }
    }

    vertex_input.bindings[0].stride = sizeof(Vertex);
    vertex_input.attributes[0].binding = 0;
    vertex_input.attributes[0].format = Format::rgb32_float;
    vertex_input.attributes[0].offset = offsetof(Vertex, position);
    vertex_input.attributes[1].binding = 0;
    vertex_input.attributes[1].format = Format::rgb32_float;
    vertex_input.attributes[1].offset = offsetof(Vertex, normal);
    vertex_input.attributes[2].binding = 0;
    vertex_input.attributes[2].format = Format::rg32_float;
    vertex_input.attributes[2].offset = offsetof(Vertex, tex_coords);
}

//----------------------------------------------------------------------------------------------------------------------

void Torus::init_indices_(uint32_t side_count, uint32_t ring_count)
{
    indices.reserve(side_count * ring_count);

    for (auto ring = 0; ring != ring_count; ++ring) {
        auto cur_ring_offset = ring * side_count;
        auto nxt_ring_offset = cur_ring_offset + side_count;

        for (auto side = 0; side != side_count; ++side) {
            auto nxt_side = (side + 1) % side_count;

            indices.push_back(cur_ring_offset + side);
            indices.push_back(nxt_ring_offset + side);
            indices.push_back(nxt_ring_offset + nxt_side);
            indices.push_back(cur_ring_offset + side);
            indices.push_back(nxt_ring_offset + nxt_side);
            indices.push_back(cur_ring_offset + nxt_side);
        }
    }

    draw_count = indices.size();
}

//----------------------------------------------------------------------------------------------------------------------
