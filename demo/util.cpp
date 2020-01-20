//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "util.h"

using namespace Gfx_lib;

//----------------------------------------------------------------------------------------------------------------------

Torus::Torus(float inner_radius, float outer_radius, uint32_t side_count, uint32_t ring_count) :
    Primitive()
{
    init_vertices_(inner_radius, outer_radius, side_count, ring_count);
    init_indices_(side_count, ring_count);
}

//----------------------------------------------------------------------------------------------------------------------

void Torus::init_vertices_(float inner_radius, float outer_radius, uint32_t side_count, uint32_t ring_count)
{
    vertices.reserve(side_count * (ring_count + 1));

    auto ring_factor = glm::two_pi<float>() / ring_count;
    auto side_factor = glm::two_pi<float>() / side_count;

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
                { u / glm::two_pi<float>(), v / glm::two_pi<float>() }
            };

            vertex.normal = glm::normalize(vertex.normal);
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
